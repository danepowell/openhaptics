/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ViewManager.cpp

Description:

  Implementation of the IHapticManager interface.

*******************************************************************************/

#include "PointManipulationAfx.h"

#include <ViewManager.h>

#include <HDU/hduMatrix.h>
#include <HDU/hduMath.h>

#include <assert.h>

namespace
{

class ViewManager : public IViewManager
{
public:
    
    ViewManager();
    virtual ~ViewManager();

    virtual void setup();
    virtual void cleanup();

    virtual void reshapeView(int width, int height);
    virtual void updateView();

    virtual int getWindowWidth() const { return m_viewport[2]; }

    virtual int getWindowHeight() const { return m_viewport[3]; }

    virtual bool isModifyingView() const;
    virtual void startModifyView(ModifyViewType type, int x, int y);
    virtual void stopModifyView();
    virtual void modifyView(int x, int y);

    virtual bool toScreen(const hduVector3Dd &obj, hduVector3Dd &win) const;
    virtual bool fromScreen(const hduVector3Dd &win, hduVector3Dd &obj) const;        

    virtual double getWindowToWorldScale() const;

    virtual const hduMatrix &getViewTransform() const;
    virtual const hduMatrix &getProjectionTransform() const;

private:

    double projectToTrackball(double radius, double x, double y);
    void updateWindowToWorldScale();

    hduVector3Dd m_viewPos;
    hduMatrix m_viewRotation;
    double m_viewScale;
    
    double m_windowTworldScale;

    hduMatrix m_worldTview;
    hduMatrix m_viewTclip;
    int m_viewport[4];
    
    int m_lastMouseX, m_lastMouseY;
    ModifyViewType m_modifyViewType;
};

} // anonymous namespace

/*******************************************************************************
 Creates a view manager.
*******************************************************************************/
IViewManager *IViewManager::create()
{
    return new ViewManager;
}


/*******************************************************************************
 Destroys a view manager.
*******************************************************************************/
void IViewManager::destroy(IViewManager *&pInterface)
{
    ViewManager *pImp = static_cast<ViewManager *>(pInterface);
    if (pImp)
    {
        delete pImp;
        pInterface = 0;
    }
}


/*******************************************************************************
 Constructor.
*******************************************************************************/
ViewManager::ViewManager() :
    m_viewScale(1),
    m_windowTworldScale(1),
    m_lastMouseX(0),
    m_lastMouseY(0),
    m_modifyViewType(INVALID_MODIFY_VIEW)
{
}


/*******************************************************************************
 Destructor.
*******************************************************************************/
ViewManager::~ViewManager()
{
}

/*******************************************************************************
 Sets up general OpenGL rendering properties: lights, depth buffering, etc.
*******************************************************************************/
void ViewManager::setup()
{
    static const GLfloat light_model_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    static const GLfloat light0_diffuse[] = {0.9f, 0.9f, 0.9f, 0.9f};   
    static const GLfloat light0_direction[] = {0.0f, -0.4f, 1.0f, 0.0f};    
    
    // Enable depth buffering for hidden surface removal.
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // Cull back faces.
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    
    // Setup other misc features.
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    
    // Setup lighting model.
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_direction);
    glEnable(GL_LIGHT0);   
}

/*******************************************************************************
 Cleans up the view manager.
*******************************************************************************/
void ViewManager::cleanup()
{
}


/*******************************************************************************
 Computes view parameters whenever view is reshaped.  This is the main place 
 where the viewing and workspace transforms get initialized.
*******************************************************************************/
void ViewManager::reshapeView(int width, int height)
{
    static const double kFovY = 40;
    static const double kCanonicalSphereRadius = sqrt(3.0);
    static const double kPI = 3.1415926535897932384626433832795;

    glViewport(0, 0, width, height);

    // Compute the viewing parameters based on a fixed fov and viewing
    // sphere enclosing a canonical box centered at the origin.

    double nearDist = kCanonicalSphereRadius / tan((kFovY / 2.0) * kPI / 180.0);
    double farDist = nearDist + 2.0 * kCanonicalSphereRadius;
    double aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);

    // Place the view down the Z axis looking at the origin.
    m_viewPos[0] = 0;
    m_viewPos[1] = 0;
    m_viewPos[2] = nearDist + kCanonicalSphereRadius;
 
    updateView();
}


/*******************************************************************************
 Updates the view.
*******************************************************************************/
void ViewManager::updateView()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(m_viewPos[0], m_viewPos[1], m_viewPos[2],
              0, 0, 0,
              0, 1, 0);
    
    glMultMatrixd(m_viewRotation);
    glScaled(m_viewScale, m_viewScale, m_viewScale);    

    // Refresh the cached OpenGL view state
    glGetDoublev(GL_MODELVIEW_MATRIX, m_worldTview);
    glGetDoublev(GL_PROJECTION_MATRIX, m_viewTclip);
    glGetIntegerv(GL_VIEWPORT, m_viewport);

    updateWindowToWorldScale();
}


/*******************************************************************************
 Returns whether the view is being modified.
*******************************************************************************/
bool ViewManager::isModifyingView() const
{
    return m_modifyViewType != INVALID_MODIFY_VIEW;
}


/*******************************************************************************
 Starts modifying the view.
*******************************************************************************/
void ViewManager::startModifyView(ModifyViewType type, int x, int y)
{
    if (type != MODIFY_VIEW_ROTATE && type != MODIFY_VIEW_SCALE)
        return;

    m_modifyViewType = type;

    m_lastMouseX = x;
    m_lastMouseY = y;
}


/*******************************************************************************
 Stops modifying the view.
*******************************************************************************/
void ViewManager::stopModifyView()
{
    m_modifyViewType = INVALID_MODIFY_VIEW;
}


/*******************************************************************************
 Modifies the view.
*******************************************************************************/
void ViewManager::modifyView(int x, int y)
{
    static const double kTrackBallRadius = 0.8;   

    if (m_modifyViewType == MODIFY_VIEW_ROTATE)
    {
        hduVector3Dd lastPos;
        lastPos[0] = m_lastMouseX * 2.0 / getWindowWidth() - 1.0;
        lastPos[1] = (getWindowHeight() - m_lastMouseY) * 2.0 / getWindowHeight() - 1.0;
        lastPos[2] = projectToTrackball(kTrackBallRadius, lastPos[0], lastPos[1]);

        hduVector3Dd currPos;
        currPos[0] = x * 2.0 / getWindowWidth() - 1.0;
        currPos[1] = (getWindowHeight() - y) * 2.0 / getWindowHeight() - 1.0;
        currPos[2] = projectToTrackball(kTrackBallRadius, currPos[0], currPos[1]);

        currPos.normalize();
        lastPos.normalize();

        hduVector3Dd rotateVec = lastPos.crossProduct(currPos);
        
        double rotateAngle = asin(rotateVec.magnitude());
        if (!hduIsEqual(rotateAngle, 0.0, DBL_EPSILON))
        {
            hduMatrix deltaRotation = hduMatrix::createRotation(
                rotateVec, rotateAngle);            
            m_viewRotation.multRight(deltaRotation);
        
            updateView();
        }
    }
    else if (m_modifyViewType == MODIFY_VIEW_SCALE)
    {
        float y1 = getWindowHeight() - m_lastMouseY;
        float y2 = getWindowHeight() - y;

        m_viewScale *= 1 + (y1 - y2) / getWindowHeight();  

        updateView();
    }

    m_lastMouseX = x;
    m_lastMouseY = y;
}


/*******************************************************************************
 Converts the object coordinates to screen (window) coordinates.
*******************************************************************************/
bool ViewManager::toScreen(const hduVector3Dd &obj, hduVector3Dd &win) const
{
    int nResult = gluProject(
        obj[0], obj[1], obj[2],
        m_worldTview,
        m_viewTclip,
        m_viewport,
        &win[0], &win[1], &win[2]);
    
    return nResult == GL_TRUE;
}


/*******************************************************************************
 Converts from screen *window) coordinates to object coordinates.
*******************************************************************************/
bool ViewManager::fromScreen(const hduVector3Dd &win, hduVector3Dd &obj) const
{
    int nResult = gluUnProject(
        win[0], win[1], win[2],
        m_worldTview,
        m_viewTclip,
        m_viewport,
        &obj[0], &obj[1], &obj[2]);
    
    return nResult == GL_TRUE;
}

/*******************************************************************************
 Uses the viewing transforms to determine a scale factor that will allow us
 to specify the size of objects on the screen in pixel dimensions.
*******************************************************************************/
void ViewManager::updateWindowToWorldScale()
{
    hduVector3Dd p0, p1;
    bool bNoError;

    bNoError = fromScreen(hduVector3Dd(0, 0, 0), p0);
    assert(bNoError);
    
    bNoError = fromScreen(hduVector3Dd(1, 1, 0), p1);
    assert(bNoError);

    m_windowTworldScale = (p1 - p0).magnitude() / sqrt(2.0);
}



/*******************************************************************************
 Gets the window to world scale.
*******************************************************************************/
double ViewManager::getWindowToWorldScale() const
{
    return m_windowTworldScale;
}


/*******************************************************************************
 Gets the view transform.
*******************************************************************************/
const hduMatrix &ViewManager::getViewTransform() const
{
    return m_worldTview;
}


/*******************************************************************************
 Gets the projection transform.
*******************************************************************************/
const hduMatrix &ViewManager::getProjectionTransform() const
{
    return m_viewTclip;
}


/*******************************************************************************
This routine is used by the view rotation code for simulating a virtual
 trackball.  This math computes the z height for a 2D projection onto the
 surface of a 2.5D sphere.  When the input point is near the center of the
 sphere, this routine computes the actual sphere intersection in Z.  When 
 the input point moves towards the outside of the sphere, this routine will 
 solve for a hyperbolic projection, so that it still yields a meaningful answer.
*******************************************************************************/
double ViewManager::projectToTrackball(double radius, double x, double y)
{
    static const double kUnitSphereRadius2D = sqrt(2.0);
    double z;

    double dist = sqrt(x * x + y * y);
    if (dist < radius * kUnitSphereRadius2D / 2.0)
    {
        // Solve for sphere case.
        z = sqrt(radius * radius - dist * dist);
    }
    else
    {
        // Solve for hyperbolic sheet case.
        double t = radius / kUnitSphereRadius2D;
        z = t * t / dist;
    }

    return z;
}

/******************************************************************************/
