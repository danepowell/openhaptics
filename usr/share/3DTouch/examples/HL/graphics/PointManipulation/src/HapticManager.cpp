/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  HapticManager.cpp

Description:

  Implementation of the IHapticManager interface.

*******************************************************************************/

#include <stdlib.h>

#include "PointManipulationAfx.h"
#include <PointManager.h>
#include <HapticManager.h>
#include <ViewManager.h>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>

#include <HLU/hlu.h>

#include <assert.h>

namespace 
{

class HapticManager : public IHapticManager
{
public:

    HapticManager();
    virtual ~HapticManager();

    virtual void setup(IPointManager *pPointManager,
                       IViewManager *pViewManager);
    virtual void cleanup();

    virtual void updateWorkspace();

    virtual void drawCursor();
    virtual void feelPoints();

    virtual bool isManipulating() const;

protected:

    void startManipulating(int nIndex);
    void stopManipulating();
    void updateManipPoint(const hduVector3Dd &proxyPos);

    void pushWorkspaceOffset(const hduVector3Dd &offsetWC);
    void popWorkspaceOffset();

    int getPointIndexFromTouchId(HLuint id) const;

    static void HLCALLBACK hlTouchCB(HLenum event, HLuint object, HLenum thread,
                                     HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonDownCB(HLenum event, HLuint object, HLenum thread,
                                          HLcache *cache, void *userdata);
    static void HLCALLBACK hlButtonUpCB(HLenum event, HLuint object, HLenum thread,
                                        HLcache *cache, void *userdata);
    static void HLCALLBACK hlMotionCB(HLenum event, HLuint object, HLenum thread,
                                      HLcache *cache, void *userdata);

private:

    HHD m_hHD;
    HHLRC m_hHLRC;

    IPointManager *m_pPointManager;
    IViewManager *m_pViewManager;

    int m_nManipPoint;
    
    HLuint m_nPointNameBase;

    double m_cursorScale;
    GLuint m_nCursorDisplayList;

    HLuint m_effectName;       
};

} // anonymous namespace


/*******************************************************************************
 Creates a HapticManager.                                                                                
*******************************************************************************/
IHapticManager *IHapticManager::create()
{
    return new HapticManager;
}


/*******************************************************************************
 Destroys the HapticManager.
*******************************************************************************/
void IHapticManager::destroy(IHapticManager *&pInterface)
{
    HapticManager *pImp = static_cast<HapticManager *>(pInterface);
    if (pImp)
    {
        delete pImp;
        pInterface = 0;
    }
}


/*******************************************************************************
 Constructor.
*******************************************************************************/
HapticManager::HapticManager() : 
    m_pPointManager(0),
    m_pViewManager(0),
    m_nManipPoint(-1),
    m_nPointNameBase(0),
    m_effectName(0),
    m_cursorScale(0),
    m_nCursorDisplayList(0),
    m_hHD(HD_INVALID_HANDLE),
    m_hHLRC(NULL)
{
}


/*******************************************************************************
 Destructor.
*******************************************************************************/
HapticManager::~HapticManager()
{
}


/*******************************************************************************
 Sets up the haptics.
*******************************************************************************/
void HapticManager::setup(IPointManager *pPointManager, 
                          IViewManager *pViewManager)
{        
    m_pPointManager = pPointManager;
    assert(m_pPointManager);

    m_pViewManager = pViewManager;
    assert(m_pViewManager);

    HDErrorInfo error;
    m_hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(1);
    }   
    
    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    m_hHLRC = hlCreateContext(m_hHD);
    hlMakeCurrent(m_hHLRC); 

    int numPoints = pPointManager->getNumPoints();

    m_nPointNameBase = hlGenShapes(numPoints);

    for (int i = 0; i < numPoints; i++)
    {
        // Register event callbacks for all of the points, so we can be
        // notified when the points are touched and untouched.
        HLuint nPointName = m_nPointNameBase + i;
        
        hlAddEventCallback(HL_EVENT_TOUCH, nPointName,
            HL_CLIENT_THREAD, hlTouchCB, this);
        hlAddEventCallback(HL_EVENT_UNTOUCH, nPointName,
            HL_CLIENT_THREAD, hlTouchCB, this);
        hlAddEventCallback(HL_EVENT_1BUTTONDOWN, nPointName,
            HL_CLIENT_THREAD, hlButtonDownCB, this);   
    }

    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, hlButtonUpCB, this);

    // Start an ambient drag friction effect.
    m_effectName = hlGenEffects(1);

    hlBeginFrame();
    hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.4);
    hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.1);
    hlStartEffect(HL_EFFECT_FRICTION, m_effectName);    
    hlEndFrame();
}


/*******************************************************************************
 Cleans up the haptics.
*******************************************************************************/
void HapticManager::cleanup()
{   
    if (m_hHLRC != NULL)
    {
        assert(m_pPointManager);

        hlBeginFrame();
        hlStopEffect(m_effectName);
        hlEndFrame();    

        hlDeleteEffects(m_effectName, 1);

        int numPoints = m_pPointManager->getNumPoints();

        for (int i = 0; i < numPoints; i++)
        {
            HLuint nPointName = m_nPointNameBase + i;

            // Unregister event callbacks for all of the points.
            hlRemoveEventCallback(HL_EVENT_TOUCH, nPointName,
                                  HL_CLIENT_THREAD, hlTouchCB);
            hlRemoveEventCallback(HL_EVENT_UNTOUCH, nPointName,
                                  HL_CLIENT_THREAD, hlTouchCB);
            hlRemoveEventCallback(HL_EVENT_1BUTTONDOWN, nPointName,
                                  HL_CLIENT_THREAD, hlButtonDownCB);                            
        }    

        hlRemoveEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY,
                              HL_CLIENT_THREAD, hlButtonUpCB);                            

        hlDeleteShapes(m_nPointNameBase, numPoints);

        hlMakeCurrent(NULL);
        hlDeleteContext(m_hHLRC);
    }

    if (m_hHD != HD_INVALID_HANDLE)
        hdDisableDevice(m_hHD);
}


/*******************************************************************************
 Uses the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void HapticManager::updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();
    
    // This example uses a non-uniform workspace fit, which offers a maximal
    // fit to the view frustum but introduces non-uniform scale in the
    // workspace.  This non-uniform scale is fine for point and line based
    // haptics, such as in this example, but can introduce distortion when 
    // actually feeling surfaces (e.g. a sphere might feel like an ellipsoid).
    hluFitWorkspaceNonUniform(projection);
}


/******************************************************************************
 Displays a cursor using the current haptic device proxy transform and the
 mapping between the workspace and world coordinates.
******************************************************************************/
void HapticManager::drawCursor()
{
    static const double kCursorSizePixels = 15;
    static const double kCursorRadius = 0.5;
    static const int kCursorTess = 15;
    HLdouble transform[16];

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!m_nCursorDisplayList)
    {
        m_nCursorDisplayList = glGenLists(1);
        glNewList(m_nCursorDisplayList, GL_COMPILE);

        qobj = gluNewQuadric();
               
        gluSphere(qobj, kCursorRadius, kCursorTess, kCursorTess);
   
        gluDeleteQuadric(qobj);

        glEndList();
    }  

    // Apply the local position/rotation transform of the haptic device proxy.
    hlGetDoublev(HL_PROXY_TRANSFORM, transform);
    glMultMatrixd(transform);
        
    // Apply the local cursor scale factor.
    double scale = m_pViewManager->getWindowToWorldScale() * kCursorSizePixels;
    glScaled(scale, scale, scale);

    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(1.0, 1.0, 1.0);

    glCallList(m_nCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}


/*******************************************************************************
 The main draw routine for displaying points.  Points can be drawn in different
 colors based on their highlighted/selected state.  Each point is drawn as a
 small lit sphere.
*******************************************************************************/
void HapticManager::feelPoints()
{            
    hlBeginFrame();

    int nPoints = m_pPointManager->getNumPoints();
    for (int i = 0; i < nPoints; i++)
    {   
        // Don't haptically draw a selected point, since it's being manipulated.
        if (m_nManipPoint == i)
            continue;

        // Now draw a shape that represents the projection of the points to
        // the near and far planes.
        hlHinti(HL_SHAPE_FEEDBACK_BUFFER_VERTICES, 3);
        hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, m_nPointNameBase + i);

        hlTouchModel(HL_CONSTRAINT);
        hlTouchModelf(HL_SNAP_DISTANCE, 3.5);
        hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.5);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.0);
        hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.0);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.0);
        
        const hduVector3Dd &pt = m_pPointManager->getPointPosition(i);
        
        // Draw a line that represents the projection of the point
        // to the near and far of the view volume.
        hduVector3Dd winPt;
        hduVector3Dd nearPt, farPt;
        m_pViewManager->toScreen(pt, winPt);
        
        winPt[2] = 0;
        m_pViewManager->fromScreen(winPt, nearPt);
        
        winPt[2] = 1;
        m_pViewManager->fromScreen(winPt, farPt);
        
        glBegin(GL_LINES);
        glVertex3dv(nearPt);
        glVertex3dv(farPt);
        glEnd();

        glBegin(GL_POINTS);
        glVertex3dv(pt);
        glEnd();

        hlEndShape();
    }

    // Call any event callbacks that have been triggered.
    hlCheckEvents();

    hlEndFrame();
}


/*******************************************************************************
 Returns whether the manager is manipulating.
*******************************************************************************/
bool HapticManager::isManipulating() const
{
    return m_nManipPoint != -1;
}


/*******************************************************************************
 Starts the manipulation.
*******************************************************************************/
void HapticManager::startManipulating(int nIndex)
{
    assert(!isManipulating());
    assert(nIndex >= 0 && nIndex < m_pPointManager->getNumPoints());

    m_pPointManager->setPointSelected(nIndex, true);
    
    hlAddEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, hlMotionCB, this);                            
    
    m_nManipPoint = nIndex;

    hduVector3Dd proxyPos;
    hlGetDoublev(HL_PROXY_POSITION, proxyPos);

    hduVector3Dd offsetWC = m_pPointManager->getPointPosition(nIndex) - proxyPos;

    // This is a VERY useful technique for offseting the user in space so
    // that they begin their manipulation exactly at the location of the
    // selected point. 
    pushWorkspaceOffset(offsetWC);
}


/*******************************************************************************
 Stops the manipulation.
*******************************************************************************/
void HapticManager::stopManipulating()
{
    assert(isManipulating());

    m_pPointManager->setPointSelected(m_nManipPoint, false);
    
    hlRemoveEventCallback(HL_EVENT_MOTION, HL_OBJECT_ANY,
        HL_CLIENT_THREAD, hlMotionCB);
    
    m_nManipPoint = -1;

    popWorkspaceOffset();
}


/*******************************************************************************
 Updates the manipulation point.
*******************************************************************************/
void HapticManager::updateManipPoint(const hduVector3Dd &proxyPos)
{       
    m_pPointManager->setPointPosition(m_nManipPoint, proxyPos);
}


/*******************************************************************************
 Highlights the touched object.
*******************************************************************************/
void HLCALLBACK HapticManager::hlTouchCB(HLenum event, HLuint object,
                                         HLenum thread, HLcache *cache, 
                                         void *userdata)
{
    HapticManager *pThis = static_cast<HapticManager *>(userdata);

    int nIndex = pThis->getPointIndexFromTouchId(object);

    if (event == HL_EVENT_TOUCH)
    {
        pThis->m_pPointManager->setPointHighlighted(nIndex, true);
    }
    else if (event == HL_EVENT_UNTOUCH)
    {
        pThis->m_pPointManager->setPointHighlighted(nIndex, false);
    }
}


/*******************************************************************************
 Starts manipulation upon button press.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonDownCB(HLenum event, HLuint object,
                                              HLenum thread, HLcache *cache,
                                              void *userdata)
{
    HapticManager *pThis = static_cast<HapticManager *>(userdata);

    int nIndex = pThis->getPointIndexFromTouchId(object);

    if (!pThis->isManipulating())
    {
        pThis->startManipulating(nIndex);
    }
}


/*******************************************************************************
 Stops manipulation upon button release.
*******************************************************************************/
void HLCALLBACK HapticManager::hlButtonUpCB(HLenum event, HLuint object,
                                            HLenum thread, HLcache *cache, 
                                            void *userdata)
{
    HapticManager *pThis = static_cast<HapticManager *>(userdata);

    if (pThis->isManipulating())
    {
        pThis->stopManipulating();
    }
}


/*******************************************************************************
 Called whenever the device position changes.
*******************************************************************************/
void HLCALLBACK HapticManager::hlMotionCB(HLenum event, HLuint object,
                                          HLenum thread, HLcache *cache,
                                          void *userdata)
{
    HapticManager *pThis = static_cast<HapticManager *>(userdata);

    if (pThis->isManipulating())
    {
        // Get the position of the proxy when the motion was detected.
        hduVector3Dd proxyPos;
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxyPos);

        pThis->updateManipPoint(proxyPos);
    }
}


/*******************************************************************************
 Applies a haptic workspace offset.
*******************************************************************************/
void HapticManager::pushWorkspaceOffset(
    const hduVector3Dd &offsetWC)
{
    const hduMatrix &worldTview = m_pViewManager->getViewTransform();
    hduMatrix viewTtouch;
    hduMatrix touchTworkspace;

    hlGetDoublev(HL_VIEWTOUCH_MATRIX, viewTtouch);
    hlGetDoublev(HL_TOUCHWORKSPACE_MATRIX, touchTworkspace);

    hduMatrix worldTworkspace = worldTview * viewTtouch * touchTworkspace;

    hduVector3Dd offsetWS;
    worldTworkspace.multDirMatrix(offsetWC, offsetWS);

    // Apply the translation in the local coordinates of the workspace.
    hduMatrix translateWS = hduMatrix::createTranslation(-offsetWS);
    touchTworkspace.multRight(translateWS);

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlPushMatrix();
    hlLoadMatrixd(touchTworkspace);
}

    
/*******************************************************************************
 Restores from a workspace offset.
*******************************************************************************/
void HapticManager::popWorkspaceOffset()
{
    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlPopMatrix();
}


/*******************************************************************************
 Returns the point index given the id.
*******************************************************************************/
int HapticManager::getPointIndexFromTouchId(HLuint id) const
{
    int nIndex = id - m_nPointNameBase;
    assert(nIndex >= 0 && nIndex < m_pPointManager->getNumPoints());
    return nIndex;
}

/******************************************************************************/

