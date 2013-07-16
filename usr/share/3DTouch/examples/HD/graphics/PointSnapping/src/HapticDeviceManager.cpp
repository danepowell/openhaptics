/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticDeviceManager.cpp

Description:

  Implements the IHapticDeviceManager interface.

*******************************************************************************/

#include "PointSnappingAfx.h"

#include "HapticDeviceManager.h"
#include "PointManager.h"
#include "PointSnappingUtil.h"
#include "ViewApparentPointConstraint.h"

#include <cassert>
#include <algorithm>

#include <HDU/hdu.h>
#include <HDU/hduError.h>
#include <HDU/hduMath.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduHapticDevice.h>

#include <SnapConstraints/ISnapConstraintsAPI.h>
#include <SnapConstraints/SnapConstraint.h>

#define CURSOR_SIZE_PIXELS  15
#define DEVICE_UPDATE_PRIORITY          (HD_MAX_SCHEDULER_PRIORITY)
#define SYNCHRONIZE_STATE_PRIORITY      HD_DEFAULT_SCHEDULER_PRIORITY

using namespace SnapConstraints;

namespace
{

/******************************************************************************
 HapticDeviceManager
 
 This class.provides the application glue that marries haptic interactions with
 the application logic/state. The HapticDeviceManager knows how to interact
 with the points maintained by the IPointManager instance to support
 interactions like snapping, highlighting, and selection of points. This class
 also orchestrates the synchronization between haptic and graphics state.
 Additionally, this class handles the workspace mapping that allows us to
 perform transformations between device coordinates and world coordinates.
******************************************************************************/
class HapticDeviceManager : public IHapticDeviceManager
{
public:

    HapticDeviceManager();
    ~HapticDeviceManager();

    void setup(IPointManager *pPointManager);
    void cleanup();

    void updateState();
    void updateWorkspace();

    void drawCursor();

private:

    void updateSnapping();

    static HDCallbackCode HDCALLBACK deviceUpdateCallback(void *pUserData);
    static HDCallbackCode HDCALLBACK setDeviceTransformCallback(void *pUserData);
    static HDCallbackCode HDCALLBACK setConstraintCallback(void *pUserData);
    static HDCallbackCode HDCALLBACK clearConstraintCallback(void *pUserData);

    static void madeContactCallbackGT(IHapticDevice::EventType event,
                     const IHapticDevice::IHapticDeviceState * const pState,
                     void *pUserData);

    static void lostContactCallbackGT(IHapticDevice::EventType event,
                     const IHapticDevice::IHapticDeviceState * const pState,
                     void *pUserData);

    static void buttonDownCallbackGT(IHapticDevice::EventType event,
                     const IHapticDevice::IHapticDeviceState * const pState,
                     void *pUserData);

    static void errorCallbackGT(IHapticDevice::EventType event,
                     const IHapticDevice::IHapticDeviceState * const pState,
                     void *pUserData);

    HHD m_hHD;
    HDSchedulerHandle m_hUpdateCallback;
    IHapticDevice *m_pHapticDeviceHT;
    IHapticDevice *m_pHapticDeviceGT;

    IPointManager *m_pPointManager;

    ISnapConstraintsAPI *m_pSnapAPI;
    ViewApparentPointConstraint *m_pConstraint;

    hduMatrix m_workspaceXform;
    HDdouble m_cursorScale;
    hduVector3Dd m_cameraPosWC;

    GLuint m_nCursorDisplayList;
};

} /* anonymous namespace */

/******************************************************************************
 Factory method for creating an instance of the HapticDeviceManager.
******************************************************************************/
IHapticDeviceManager *IHapticDeviceManager::create()
{
    return new HapticDeviceManager;
}

/******************************************************************************
 Factory method for destroying an instance of the HapticDeviceManager.
******************************************************************************/
void IHapticDeviceManager::destroy(IHapticDeviceManager *&pInterface)
{
    if (pInterface)
    {
        HapticDeviceManager *pImp = static_cast<HapticDeviceManager *>(pInterface);
        delete pImp;
        pInterface = 0;
    }
}

/******************************************************************************
 HapticDeviceManager Constructor.
******************************************************************************/
HapticDeviceManager::HapticDeviceManager() :
    m_hHD(HD_INVALID_HANDLE),
    m_hUpdateCallback(HD_INVALID_HANDLE),
    m_pHapticDeviceHT(0),
    m_pHapticDeviceGT(0),
    m_pPointManager(0),
    m_pSnapAPI(0),
    m_pConstraint(0),
    m_nCursorDisplayList(0)
{
}

/*******************************************************************************
 HapticDeviceManager Destructor.
*******************************************************************************/
HapticDeviceManager::~HapticDeviceManager()
{
}

/*******************************************************************************
 This is the main initialization needed for the haptic glue code.
*******************************************************************************/
void HapticDeviceManager::setup(IPointManager *pPointManager)
{
    HDErrorInfo error;

    /* Intialize a device configuration. */
    m_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to initialize haptic device" << std::endl;
        std::cerr << "Press any key to quit." << std::endl;
        getchar();
        exit(-1);
    }

    /* Create the IHapticDevice instances for the haptic and graphic threads
       These interfaces are useful for handling the synchronization of
       state between the two main threads. */
    m_pHapticDeviceHT = IHapticDevice::create(
        IHapticDevice::HAPTIC_THREAD_INTERFACE, m_hHD);
    m_pHapticDeviceGT = IHapticDevice::create(
        IHapticDevice::GRAPHIC_THREAD_INTERFACE, m_hHD);

    /* Setup callbacks so we can be notified about events in the graphics 
       thread. */
    m_pHapticDeviceGT->setCallback(
        IHapticDevice::MADE_CONTACT, madeContactCallbackGT, this);
    m_pHapticDeviceGT->setCallback(
        IHapticDevice::LOST_CONTACT, lostContactCallbackGT, this);
    m_pHapticDeviceGT->setCallback(
        IHapticDevice::BUTTON_1_DOWN, buttonDownCallbackGT, this);    
    m_pHapticDeviceGT->setCallback(
        IHapticDevice::DEVICE_ERROR, errorCallbackGT, this);    

    m_pSnapAPI = ISnapConstraintsAPI::create();
    m_pConstraint = new ViewApparentPointConstraint(false);

    m_pPointManager = pPointManager;

    hdEnable(HD_FORCE_OUTPUT);

    /* Schedule a haptic thread callback for updating the device every
       tick of the servo loop. */
    m_hUpdateCallback = hdScheduleAsynchronous(
        deviceUpdateCallback, this, DEVICE_UPDATE_PRIORITY);

    /* Start the scheduler to get the haptic loop going. */
    hdStartScheduler();
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        std::cerr << error << std::endl;
        std::cerr << "Failed to start scheduler" << std::endl;
        std::cerr << "Press any key to quit." << std::endl;
        getchar();
        exit(-1);
    }
}

/*******************************************************************************
 Reverse the setup process by shutting down and destructing the services
 used by the HapticDeviceManager.
*******************************************************************************/
void HapticDeviceManager::cleanup()
{
    hdStopScheduler();

    if (m_hUpdateCallback != HD_INVALID_HANDLE)
    {
        hdUnschedule(m_hUpdateCallback);
        m_hUpdateCallback = HD_INVALID_HANDLE;
    }

    if (m_hHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(m_hHD);
        m_hHD = HD_INVALID_HANDLE;
    }

    IHapticDevice::destroy(m_pHapticDeviceGT);
    IHapticDevice::destroy(m_pHapticDeviceHT);

    ISnapConstraintsAPI::destroy(m_pSnapAPI);
    delete m_pConstraint;

    glDeleteLists(m_nCursorDisplayList, 1);
}

/*******************************************************************************
 This method will get called every tick of the graphics loop. It is primarily
 responsible for synchronizing state with the haptics thread as well as
 updating snap state.
*******************************************************************************/
void HapticDeviceManager::updateState()
{
    /* Capture the latest state from the servoloop. */
    m_pHapticDeviceGT->beginUpdate(m_pHapticDeviceHT);
    m_pHapticDeviceGT->endUpdate(m_pHapticDeviceHT);

    updateSnapping();
}

/*******************************************************************************
 This method works with the PointManager to determine if there are any points
 within proximity to the haptic device for snapping. This can be evaluated at
 the graphics rate since snapping is determined as a proximity condition. Plus,
 humans only make controlled motions at ~10-15 Hz, so it would be wasteful to
 check for the snapping condition any faster.
*******************************************************************************/
void HapticDeviceManager::updateSnapping()
{
    IHapticDevice::IHapticDeviceState *pState = 
        m_pHapticDeviceGT->getCurrentState();

    /* Don't disturb snap state if we're already in contact. */
    if (pState->isInContact())
        return;

    hduMatrix parentTworld(pState->getParentCumulativeTransform());
    hduMatrix worldTparent = parentTworld.getInverse();

    /* Get the local position of the device. */
    hduVector3Dd devicePositionLC = pState->getPosition();

    /* Setup the point and line constraint for each. */
    PointConstraint *pPointConstraint = m_pConstraint->getPointConstraint();
    LineConstraint *pLineConstraint = m_pConstraint->getLineConstraint();

    /* Create a view apparent constraint by using a line that passes from
       the eye position through the point of interest. This allows
       the user to select the point just by being overtop of it in the view. */
    hduVector3Dd cameraPosLC;
    worldTparent.multVecMatrix(m_cameraPosWC, cameraPosLC);
    pLineConstraint->setStartPoint(cameraPosLC);

    double minDist = m_pConstraint->getSnapDistance();
    int nClosestPoint = -1;    

    /* Search for the constraint that is closest to the device position. */
    int nPoints = m_pPointManager->getNumPoints();
    for (int i = 0; i < nPoints; i++)
    {
        hduVector3Dd proxyPositionLC;        
        double dist;

        /* Compute position of the test point in device coordinates. */
        hduVector3Dd pointPositionWC = m_pPointManager->getPointPosition(i);
        hduVector3Dd pointPositionLC;
        worldTparent.multVecMatrix(pointPositionWC, pointPositionLC);

        pPointConstraint->setPoint(pointPositionLC);
        pLineConstraint->setEndPoint(pointPositionLC);
        
        dist = m_pConstraint->testConstraint(devicePositionLC, proxyPositionLC);
        
        if (dist < minDist)
        {
            minDist = dist;
            nClosestPoint = i;
        }
    }

    /* Finally set the constraint based on the closest point. */
    if (nClosestPoint >= 0)
    {
        /* Compute position of the test point in device coordinates. */
        hduVector3Dd pointPositionWC = m_pPointManager->
            getPointPosition(nClosestPoint);
        hduVector3Dd pointPositionLC;
        worldTparent.multVecMatrix(pointPositionWC, pointPositionLC);

        pPointConstraint->setPoint(pointPositionLC);
        pLineConstraint->setEndPoint(pointPositionLC);

        /* Set the point index as the constraint private data, so the
           event handlers can know which point is being referenced. */
        m_pConstraint->setUserData((void *) nClosestPoint);

        hdScheduleSynchronous(setConstraintCallback, this,
            SYNCHRONIZE_STATE_PRIORITY);
    }
}

/*******************************************************************************
 Uses the current OpenGL viewing transforms to determine a mapping from device
 coordinates to world coordinates.
*******************************************************************************/
void HapticDeviceManager::updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    /* Compute the transform for going from device coordinates to world
       coordinates based on the current viewing transforms. */
    hduMapWorkspaceModel(modelview, projection, m_workspaceXform);

    /* Compute the scale factor that can be applied to a unit sized object
       in world coordinates that will make it a particular size in pixels. */
    HDdouble screenTworkspace = hduScreenToWorkspaceScale(
        modelview, projection, viewport, m_workspaceXform);
    
    m_cursorScale = CURSOR_SIZE_PIXELS * screenTworkspace;

    /* Compute the updated camera position in world coordinates. */
    hduMatrix worldTeye(modelview);
    hduMatrix eyeTworld = worldTeye.getInverse();
    eyeTworld.multVecMatrix(hduVector3Dd(0, 0, 0), m_cameraPosWC);

    hdScheduleSynchronous(setDeviceTransformCallback, this,
                          SYNCHRONIZE_STATE_PRIORITY);    
}

/*******************************************************************************
 Draws a 3D cursor using the current device transform and the workspace
 to world transform.
*******************************************************************************/
void HapticDeviceManager::drawCursor()
{
    IHapticDevice::IHapticDeviceState *pState = 
        m_pHapticDeviceGT->getCurrentState();

    /* Don't draw the cursor if we're snapped to a point. */
    if (pState->isInContact())
        return;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    glPushMatrix();

    if (!m_nCursorDisplayList)
    {
        m_nCursorDisplayList = glGenLists(1);
        glNewList(m_nCursorDisplayList, GL_COMPILE);
        GLUquadricObj *qobj = gluNewQuadric();
        gluSphere(qobj, 0.5, 10, 10);
        gluDeleteQuadric(qobj);
        glEndList();
    }

    glMultMatrixd(pState->getParentCumulativeTransform());

    glMultMatrixd(pState->getTransform());

    glScaled(m_cursorScale, m_cursorScale, m_cursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0, 1.0, 1.0);

    glCallList(m_nCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}


/******************************************************************************
 Scheduler Callbacks
 These callback routines get performed in the haptics thread.
******************************************************************************/

/******************************************************************************
 This is the main haptic thread scheduler callback. It handles updating the
 currently applied constraint.
******************************************************************************/
HDCallbackCode HDCALLBACK HapticDeviceManager::deviceUpdateCallback(
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);

    /* Force the haptic device to update its state. */
    pThis->m_pHapticDeviceHT->beginUpdate(0);

    IHapticDevice::IHapticDeviceState *pCurrentState = 
        pThis->m_pHapticDeviceHT->getCurrentState();
    IHapticDevice::IHapticDeviceState *pLastState = 
        pThis->m_pHapticDeviceHT->getLastState();

    /* Get the position of the device. */
    hduVector3Dd devicePositionLC = pCurrentState->getPosition();
    
    /* Update the snap constraint state based on the new position date. */
    ISnapConstraintsAPI *pSnapAPI = pThis->m_pSnapAPI;
    pSnapAPI->updateConstraint(devicePositionLC);

    pCurrentState->setIsInContact(pSnapAPI->getConstraint() != 0);

    if (pCurrentState->isInContact())
    {
        /* If currently in contact, use the freshest contact data. */
        pCurrentState->setContactData(pSnapAPI->getConstraint()->getUserData());        
    }
    else if (pLastState->isInContact())
    {
        /* If was in contact the last frame, use that contact data, since it
           will get reported to the event callbacks. */
        pCurrentState->setContactData(pLastState->getContactData());        
    }
    else
    {
        pCurrentState->setContactData((void *) -1);
    }

    /* Transform result from world coordinates back to device coordinates. */
    hduVector3Dd proxyPositionLC = pSnapAPI->getConstrainedProxy();
    pCurrentState->setProxyPosition(proxyPositionLC);

    hdGetDoublev(HD_CURRENT_TRANSFORM, pCurrentState->getProxyTransform());
    pCurrentState->getProxyTransform()[12] = proxyPositionLC[0];
    pCurrentState->getProxyTransform()[13] = proxyPositionLC[1];
    pCurrentState->getProxyTransform()[14] = proxyPositionLC[2];

    double kStiffness;
    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);
    kStiffness = hduMin(0.4, kStiffness);
    
    /* Compute spring force to attract device to constrained proxy. */
    hduVector3Dd force = kStiffness * (proxyPositionLC - devicePositionLC);
    hdSetDoublev(HD_CURRENT_FORCE, force);

    pThis->m_pHapticDeviceHT->endUpdate(0);

    return HD_CALLBACK_CONTINUE;
}

/******************************************************************************
 Scheduler callback to set the workspace transform both for use in the graphics
 thread and haptics thread.
******************************************************************************/
HDCallbackCode HDCALLBACK HapticDeviceManager::setDeviceTransformCallback(
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);

    IHapticDevice::IHapticDeviceState *pStateGT = 
        pThis->m_pHapticDeviceGT->getCurrentState();
    IHapticDevice::IHapticDeviceState *pStateHT = 
        pThis->m_pHapticDeviceHT->getCurrentState();

    pStateGT->setParentCumulativeTransform(pThis->m_workspaceXform);
    pStateHT->setParentCumulativeTransform(pThis->m_workspaceXform);

    return HD_CALLBACK_DONE;
}


/******************************************************************************
 Scheduler callback to set the current constraint
******************************************************************************/
HDCallbackCode HDCALLBACK HapticDeviceManager::setConstraintCallback(
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);

    pThis->m_pSnapAPI->setConstraint(pThis->m_pConstraint);

    return HD_CALLBACK_DONE;
}


/******************************************************************************
 Scheduler callback to set the current constraint
******************************************************************************/
HDCallbackCode HDCALLBACK HapticDeviceManager::clearConstraintCallback(
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);

    pThis->m_pSnapAPI->clearConstraint();

    return HD_CALLBACK_DONE;
}


/******************************************************************************
 Event Callbacks

 These are event callbacks that are registered with the IHapticDevice
******************************************************************************/

/******************************************************************************
 This handler gets called in the graphics thread whenever the device makes
 contact with a constraint. Provide a visual cue (i.e. highlighting) to
 accompany the haptic cue of being snapped to the point.
******************************************************************************/
void HapticDeviceManager::madeContactCallbackGT(
    IHapticDevice::EventType event,
    const IHapticDevice::IHapticDeviceState * const pState,
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);    

    /* Get the index of the contacted point from the contact data. */
    long int index = (long int) pState->getContactData();    

    if (index >= 0)
    {
        pThis->m_pPointManager->setPointHighlighted(index, true);    
    }
}

/******************************************************************************
 This handler gets called in the graphics thread whenever the device loses
 contact with a constraint. Provide a visual cue (i.e. highlighting) to
 accompany the haptic cue of losing contact with the point.
******************************************************************************/
void HapticDeviceManager::lostContactCallbackGT(
    IHapticDevice::EventType event,
    const IHapticDevice::IHapticDeviceState * const pState,
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);     

    /* Get the index of the contacted point from the contact data. */
    long int index = (long int) pState->getContactData();    

    if (index >= 0)
    {
        pThis->m_pPointManager->setPointHighlighted(index, false);
    }
}

/******************************************************************************
 This handler gets called in the graphics thread whenever a button press is
 detected. Interpret the click as a select/deselect operation.
******************************************************************************/
void HapticDeviceManager::buttonDownCallbackGT(
    IHapticDevice::EventType event,
    const IHapticDevice::IHapticDeviceState * const pState,
    void *pUserData)
{
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);     

    if (pState->isInContact())
    {
        /* Get the index of the contacted point from the contact data. */
        long int index = (long int) pState->getContactData();    

        if (index >= 0)
        {
            bool isSelected = pThis->m_pPointManager->isPointSelected(index);
            pThis->m_pPointManager->setPointSelected(index, !isSelected);
        }
    }
}

/******************************************************************************
 This handler gets called to handle errors
******************************************************************************/
void HapticDeviceManager::errorCallbackGT(
    IHapticDevice::EventType event,
    const IHapticDevice::IHapticDeviceState * const pState,
    void *pUserData)
{   
    HapticDeviceManager *pThis = static_cast<HapticDeviceManager *>(pUserData);

    if (hduIsForceError(&pState->getLastError()))
    {
        /* Clear the current constraint. */
        hdScheduleSynchronous(clearConstraintCallback, pThis,
            SYNCHRONIZE_STATE_PRIORITY);
    }
    else
    {
        /* This is likely a more serious error, so just bail. */
        std::cerr << pState->getLastError() << std::endl;
        std::cerr << "Error during haptic rendering" << std::endl;
        std::cerr << "Press any key to quit." << std::endl;
        getchar();
        exit(-1);
    }
}

/******************************************************************************/
