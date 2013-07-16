/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  PointManager.cpp

Description:
  A manager for points in the scene. This manager knows how to create a
  random cloud of points, draw them in different modes, and provides
  accessors for modifying the points.

  A composite snap constraint of a plane with two axial snaps and a point at
  the center.

*******************************************************************************/

#include "PointManipulationAfx.h"

#include "PointManager.h"
#include "PointManipulationUtil.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cfloat>
#include <cassert>

#include <HDU/hduMatrix.h>

#include <vector>

using namespace std;

namespace
{

static const int kNumPoints = 10;

static const double kRegularSize = 10;
static const double kHighlightedSize = 10;
static const double kSelectedSize = 10;

static const hduVector3Dd kRegularColor(1.0, 1.0, 1.0);
static const hduVector3Dd kHighlightedColor(0.0, 1.0, 0.0);
static const hduVector3Dd kSelectedColor(1.0, 0.0, 0.0);

/******************************************************************************
 PointManager Implementation of IPointManager Interface
******************************************************************************/
class PointManager : public IPointManager
{
public:

    enum PointState
    {
        REGULAR     =    (0 << 0),
        SELECTED    =    (1 << 0),
        HIGHLIGHTED =    (1 << 1)
    };    

    struct PointStruct
    {
        hduVector3Dd m_position;
        unsigned m_state;
    };

    typedef vector<PointStruct> PointList;
    
    PointManager();
    ~PointManager();

    void setup();
    void cleanup();
	
	// ArrAy added explicit cast 
    int getNumPoints() const { return (int) m_pointList.size(); }

    const hduVector3Dd &getPointPosition(int nIndex) const;
    hduVector3Dd &getPointPosition(int nIndex);
    void setPointPosition(int nIndex, const hduVector3Dd &position);
   
    void drawPoints();

    void updatePointSize();

    void setPointHighlighted(int nIndex, bool bHighlighted);
    bool isPointHighlighted(int nIndex) const;

    void setPointSelected(int nIndex, bool bSelected);
    bool isPointSelected(int nIndex) const;

private:

    PointList m_pointList;

    double m_unitPointScaleWC;
    GLuint m_nPointDisplayList;
};

} 

/******************************************************************************
 Factory method for creating an instance of PointManager
******************************************************************************/
IPointManager *IPointManager::create()
{
    return new PointManager;
}

/*******************************************************************************
 Factory method for destroying an instance of PointManager
*******************************************************************************/
void IPointManager::destroy(IPointManager *&pInterface)
{
    if (pInterface)
    {
        PointManager *pImp = static_cast<PointManager *>(pInterface);
        delete pImp;
        pInterface = 0;
    }
}

/*******************************************************************************
 PointManager Constructor
*******************************************************************************/
PointManager::PointManager() : m_nPointDisplayList(0)
{
}

/*******************************************************************************
 PointManager Destructor
*******************************************************************************/
PointManager::~PointManager()
{
}

/*******************************************************************************
 Creates a random cloud of points within a canonical volume. Also assigns
 random colors.
*******************************************************************************/
void PointManager::setup()
{
    m_pointList.reserve(kNumPoints);

    /* Create a random set of points in a canonical volume. */
    srand(time(NULL));
    for (int i = 0; i < kNumPoints; i++)
    {
        PointStruct point;

        point.m_position[0] = ((HDdouble) rand() / RAND_MAX - 0.5) * 2;
        point.m_position[1] = ((HDdouble) rand() / RAND_MAX - 0.5) * 2;
        point.m_position[2] = ((HDdouble) rand() / RAND_MAX - 0.5) * 2;
        point.m_state = REGULAR;
            
        m_pointList.push_back(point);
    }    
}

/*******************************************************************************
 Clear the main container for the points
*******************************************************************************/
void PointManager::cleanup()
{
    m_pointList.clear();
}

/*******************************************************************************
 Returns a const reference to a point position given a valid index
 Point positions are in world coordinates
*******************************************************************************/
const hduVector3Dd &PointManager::getPointPosition(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    return m_pointList[nIndex].m_position;
}

/*******************************************************************************
 Returns a non-const reference to a point position given a valid index
 Point positions are in world coordinates
*******************************************************************************/
hduVector3Dd &PointManager::getPointPosition(int nIndex)
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    return m_pointList[nIndex].m_position;
}

/*******************************************************************************
 Sets the position of a point given a valid index
 Point positions are in world coordinates
*******************************************************************************/
void PointManager::setPointPosition(int nIndex, const hduVector3Dd &position)
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    m_pointList[nIndex].m_position = position;
}

/*******************************************************************************
 Controls the highlighted state of a point. Any point can optionally be
 highlighted.
*******************************************************************************/
void PointManager::setPointHighlighted(int nIndex, bool bHighlighted)
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    if (bHighlighted)
    {
        m_pointList[nIndex].m_state |= HIGHLIGHTED;
    }
    else
    {
        m_pointList[nIndex].m_state &= ~HIGHLIGHTED;
    }
}

/*******************************************************************************
 Queries if a point is highlighted. Highlighting is an optional (or'd) state
 of the point.
*******************************************************************************/
bool PointManager::isPointHighlighted(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    return (m_pointList[nIndex].m_state & HIGHLIGHTED) != 0;
}

/*******************************************************************************
 Controls the selected state of a point
*******************************************************************************/
void PointManager::setPointSelected(int nIndex, bool bSelected)
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());    
    if (bSelected)
    {
        m_pointList[nIndex].m_state |= SELECTED;
    }
    else
    {
        m_pointList[nIndex].m_state &= ~SELECTED;
    }
}

/*******************************************************************************
 Queries if a point is selected
*******************************************************************************/
bool PointManager::isPointSelected(int nIndex) const
{
    assert(nIndex >= 0 && nIndex < m_pointList.size());
    return (m_pointList[nIndex].m_state & SELECTED) != 0;
}

/*******************************************************************************
 Determines a scale factor from world to screen for drawing points in
 pixel dimensions
*******************************************************************************/
void PointManager::updatePointSize()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    m_unitPointScaleWC = computePixelToWorldScale(
        modelview, projection, viewport);
}

/*******************************************************************************
 The main draw routine for displaying points. Points can be drawn in different
 colors based on their highlighted/selected state. Each point is drawn as a
 small lit sphere.
*******************************************************************************/
void PointManager::drawPoints()
{
    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
    
    if (!m_nPointDisplayList)
    {
        m_nPointDisplayList = glGenLists(1);
        glNewList(m_nPointDisplayList, GL_COMPILE);
        GLUquadricObj *qobj = gluNewQuadric();
        gluSphere(qobj, 0.5, 10, 10);
        gluDeleteQuadric(qobj);
        glEndList();        
    }

    glEnable(GL_COLOR_MATERIAL);
            
    /* Draw the line segments that connect the points. */
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_LIGHTING);
    
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    int nPoints = getNumPoints();
    int i;
    for (i = 0; i < nPoints; i++)
    {
        glVertex3dv(getPointPosition(i));        
    }
    glEnd();

    glPopAttrib();

    /* Draw each of the points as lit sphere. */
    GLdouble dPointSize;
    nPoints = getNumPoints();
    for (i = 0; i < nPoints; i++)
    {
        if (isPointHighlighted(i))
        {
            dPointSize = kHighlightedSize;
            glColor3dv(kHighlightedColor);
        }
        else if (isPointSelected(i))
        {
            dPointSize = kSelectedSize;
            glColor3dv(kSelectedColor);
        }
        else
        {
            dPointSize = kRegularSize;
            glColor3dv(kRegularColor);
        }
        
        glPushMatrix();

        const hduVector3Dd &pt = getPointPosition(i);
        glTranslated(pt[0], pt[1], pt[2]);

        double dPointScale = dPointSize * m_unitPointScaleWC;
        glScaled(dPointScale, dPointScale, dPointScale);        
        
        glCallList(m_nPointDisplayList);

        glPopMatrix();
    }

    glPopAttrib();
}

/******************************************************************************/
