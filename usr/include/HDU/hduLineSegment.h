/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduLineSegment.h

Description: 

  A geometric line segment.

*******************************************************************************/

#ifndef hduLineSegment_H_
#define hduLineSegment_H_

#include <HDU/hduLine.h>
#include <HDU/hduVector.h>
#include <HDU/hduMath.h>


//
// explicit typename definition needed for gcc 4.x
//
#ifdef linux
#define REQUIRED_TYPENAME typename
#else
#define REQUIRED_TYPENAME
#endif


/* Implements a 3D line segment defined by two endpoints, p1 and p2. */
template <class T>
class hduLineSegment : public hduLine<T>
{
public:

    /* Constructor defines line segment from p1 to p2.  The parametric 
       representation is then f(t) = (1-t)P1+tP2. */
    explicit hduLineSegment(
        const typename hduLine<T>::VecType &p1 = REQUIRED_TYPENAME hduLine<T>::VecType(),  // Line passes through p1
        const typename hduLine<T>::VecType &p2 = REQUIRED_TYPENAME hduLine<T>::VecType()); // Line passes through p2
           
    hduLineSegment(const hduLineSegment &rhs) :
        hduLine<T>(rhs),
        m_length(rhs.m_length),
        m_unitDirection(rhs.m_unitDirection)
    {
    }

    virtual ~hduLineSegment()
    {
    }
                
    /* Evaluates parametric form of line segment at t and returns point at f(t). */
    virtual typename hduLine<T>::VecType eval(const HDdouble t) const
    {
        return ( hduLine<T>::m_p1 + (t * m_length) * m_unitDirection );
    }

    /* Evaluates parametric form of line segment at t and returns point at f(t).
       p1 returned for values of t<0 and p2 is returned for values of t > 1. */    
    virtual typename hduLine<T>::VecType evalSegment(const HDdouble t) const
    {
        return eval(hduClamp<HDdouble>(t, 0.0, 1.0));
    }

    /* Gives the parametric value t of the line segment for the projection of pt 
       onto the line segment. */
    virtual HDdouble eval(const typename hduLine<T>::VecType &pt) const
    {
        return projectToParametric(pt);
    }
        
    /* Returns length of line segment. */
    virtual HDdouble length() const { return m_length; }
        
    /* Projects pt onto the line segment and returns the parametric value of 
       the projection. */
    virtual HDdouble projectToParametric(const typename hduLine<T>::VecType &pt) const;
    
    /* Returns distance from point to line segment. */
    virtual HDdouble distPointSegment(const typename hduLine<T>::VecType &pt) const;
    
    /* Returns squared distance from point to line segment. */
    virtual HDdouble distSqPointSegment(const typename hduLine<T>::VecType &pt) const;
    
    /* Sets p1 = startPoint. */
    virtual void setStartPoint(const typename hduLine<T>::VecType &startPoint);

    /* Sets p2 = endPoint. */
    virtual void setEndPoint(const typename hduLine<T>::VecType &endPoint);
        
    /* Vector directed along line in the direction of v1 or toward p2 depending on
       constructor used. */
    virtual typename hduLine<T>::VecType direction() const 
    { 
        return m_length * m_unitDirection; 
    }

    /* Vector directed along line in the direction of v1 or toward p2 depending on
       constructor used. */
    virtual typename hduLine<T>::VecType unitDirection() const 
    { 
        return m_unitDirection; 
    }
    
    /* Determines the minimum distance between two line segments. */
    virtual HDdouble distFromSegment(const hduLineSegment &segment,
                                     HDdouble &t, HDdouble &s, 
                                     const HDdouble epsilon) const;

private:
      
    void update();
    
    HDdouble m_length;    
    typename hduLine<T>::VecType m_unitDirection;
};

typedef hduLineSegment<HDdouble> hduLineSegmentd;
typedef hduLineSegment<HDfloat> hduLineSegmentf;

#endif  // hduLineSegment_H_

/*****************************************************************************/
