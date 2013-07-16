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

  A geometric line.

*******************************************************************************/

#ifndef hduLine_H_
#define hduLine_H_

#include <HDU/hduVector.h>

/* Implements an infinite 3D line. */
template <class T>
class hduLine
{
public:

    typedef T EltType;
    typedef hduVector3D<EltType> VecType;    

    /* Constructor defines line as passing through points p1 and p2.  The 
       parametric representation is then f(t) = (1-t)P1+tP2. */
    explicit hduLine(const VecType &p1 = VecType(),     // Line passes through p1
                     const VecType &p2 = VecType()) :   // Line passes through p2
        m_p1(p1),
        m_p2(p2)
    {
    }

    hduLine(const hduLine &rhs) :
        m_p1(rhs.m_p1),
        m_p2(rhs.m_p2)
    {
    }

    virtual ~hduLine()
    {
    }

    /* VecType directed along line in the direction of v1 or toward p2 depending 
       on constructor used. */
    virtual VecType direction() const { return m_p2 - m_p1; }

    /* Evaluates parametric form of line at t and returns point at f(t). */
    virtual VecType eval(const HDdouble t) const
    {
        return m_p1 + t * direction();
    }

    /* Returns the projection of the input point onto the line. */
    virtual inline VecType project(const VecType &pt) const;

    /* Returns distance from point to infinite line. */
    virtual HDdouble distPointLine(const VecType &pt) const;

    /* Sets p1 = startPoint. */
    virtual void setStartPoint(const VecType &startPoint);

    /* Sets p2 = endPoint. */
    virtual void setEndPoint(const VecType &endPoint);
        
    /* Returns p2. */
    virtual const VecType &getEndPoint() const { return m_p2; }
        
    /* Returns p1. */
    virtual const VecType &getStartPoint() const { return m_p1; }

    /* Returns a point on the line. */
    virtual const VecType & getPointOnLine() const { return m_p1; }

    /* VecType directed along line in the direction of v1 or toward p2 
       depending on constructor used. */
    virtual inline VecType unitDirection() const;
    
    /* Tests whether this line is parallel within epsilon to the passed in 
       line. */
    virtual bool isParallel(const hduLine &line, const HDdouble epsilon) const;

    /* Determines the minimum distance between two infinite lines. */
    virtual HDdouble distFromLine(const hduLine &line,
                                  HDdouble &t, HDdouble &s, 
                                  const HDdouble epsilon) const;

protected:
    VecType m_p1; // f(t)=p1 when t=0.
    VecType m_p2; // f(t)=p2 when t=1.
};

/* Sets p1 = startPoint. */
template <class T>
void hduLine<T>::setStartPoint(const VecType &startPoint)
{
    m_p1 = startPoint;
}

/* Sets p2 = endPoint. */
template <class T>
void hduLine<T>::setEndPoint(const VecType &endPoint)
{
    m_p2 = endPoint;    
}
        
template <class T>
typename hduLine<T>::VecType hduLine<T>::project(const VecType &pt) const
{
    // Project pt on line and return point of projection.
    VecType p0 = getPointOnLine();
    VecType v = pt - p0;
    VecType u = unitDirection();
        
    HDdouble udotv = u.dotProduct(v);
    return p0 + udotv * u;
}

template <class T>
HDdouble hduLine<T>::distPointLine(const VecType &pt) const
{
    return pt.distance(project(pt));
}

template <class T>
typename hduLine<T>::VecType hduLine<T>::unitDirection() const
{
    VecType u = direction();
    u.normalize();
    return u;
}

typedef hduLine<HDdouble> hduLined;
typedef hduLine<HDfloat> hduLinef;

#endif  // hduLine_H_

/*****************************************************************************/

