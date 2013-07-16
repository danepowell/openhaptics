/******************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  hduBoundBox.h

Description: 

  Bounding box math routines.

*******************************************************************************/

#ifndef hduBoundBox_H_
#define hduBoundBox_H_

#include <HD/hdDefines.h>

#ifdef __cplusplus

namespace
{

template <class T>
void setEltwiseMin(T &dst, const T &src)
{
    dst[0] = (dst[0] < src[0]) ? dst[0] : src[0];
    dst[1] = (dst[1] < src[1]) ? dst[1] : src[1];
    dst[2] = (dst[2] < src[2]) ? dst[2] : src[2];
}

template <class T>
void setEltwiseMax(T &dst, const T &src)
{
    dst[0] = (dst[0] > src[0]) ? dst[0] : src[0];
    dst[1] = (dst[1] > src[1]) ? dst[1] : src[1];
    dst[2] = (dst[2] > src[2]) ? dst[2] : src[2];
}

/* Is pair correctly formed, s.t. each element of lo
   is <= corresponding element of hi? */
template <class T>
bool isValidPointRange(const T &lo, const T &hi)
{
    if (lo[0]>hi[0]) return false;
    if (lo[1]>hi[1]) return false;
    if (lo[2]>hi[2]) return false;

    return true;    
}

/* Is query point >= corresponding elts in lo and <= corresponding 
   elts in hi? */
template <class T>
bool isBetween(const T& query, const T& lo, const T& hi)
{
    return isValidPointRange(lo,query) &&
           isValidPointRange(query,hi);
}

} /* anonymous namespace */

template <class T>
class hduBoundBox3D
{
public:

    typedef T PointType;
    
    hduBoundBox3D() : m_isEmpty(true)
    {}        

    hduBoundBox3D(const hduBoundBox3D<T> &rhs)
    {
        *this = rhs;
    }

    hduBoundBox3D(const T &lo, const T &hi)     
    {
        m_isEmpty = true;
        Union(lo);
        Union(hi);
    }

    inline hduBoundBox3D &operator =(const hduBoundBox3D<T> &rhs)
    {
        m_isEmpty = rhs.isEmpty();
        m_lo = rhs.lo();
        m_hi = rhs.hi();
        return *this;
    }

    bool isEmpty() const { return m_isEmpty; }
    void setIsEmpty(bool val = true) { m_isEmpty = val; }
    
    void update() { setIsEmpty(!isValidPointRange(lo(), hi())); }

    const T& lo() const { return m_lo; }
    const T& hi() const { return m_hi; }

    T& rLo() { return m_lo; }
    T& rHi() { return m_hi; }
    
    void Union(const T &pt)
    {
        if (isEmpty())
        {
            m_lo = pt;
            m_hi = pt;
        }
        else
        {
            setEltwiseMin(m_lo, pt);
            setEltwiseMax(m_hi, pt);
        }
        update();
    }

    void Union(const hduBoundBox3D<T> &box)
    {
        if (box.isEmpty())
            return;

        if (isEmpty())
        {
            *this = box;
            return;
        }

        setEltwiseMin(m_lo, box.lo());
        setEltwiseMax(m_hi, box.hi());
        update();
    }

    void Intersect(const hduBoundBox3D<T> &box)
    {
        if (isEmpty())
            return;

        if (box.isEmpty())
        {
            setIsEmpty(true);
            return;
        }
        
        setEltwiseMax(m_lo,box.lo());
        setEltwiseMin(m_hi,box.hi());
        update();
    }
    
    /* Determine whether this box contains given box. */
    bool contains(const hduBoundBox3D<T> &box)
    {
        return !isEmpty() && !box.isEmpty() &&
               isBetween(box.lo(),lo(),hi()) &&
               isBetween(box.hi(),lo(),hi());
    }
    
    /* Determine whether this box contains given point. */
    bool contains(const T &pt) const
    {
        if (isEmpty())
            return false;
        else
            return isBetween(pt, lo(), hi());
    }

private:
    bool m_isEmpty;
    T m_lo;
    T m_hi;
};

typedef hduBoundBox3D<hduVector3Df> hduBoundBox3Df;
typedef hduBoundBox3D<hduVector3Dd> hduBoundBox3Dd;

#endif  /* __cplusplus  */

#endif  /* hduBoundBox_H_ */

/*****************************************************************************/
