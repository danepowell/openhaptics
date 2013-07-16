/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduMath.h

Description: 

  Provides some common math functions needed by HDU.

*******************************************************************************/

#ifndef hduMath_H_
#define hduMath_H_

#ifdef __cplusplus

template <class T>
T hduEvalLine(const T &p0, const T &p1, typename T::EltType t)
{
    return p0 + (p1 - p0) * t;
}

template <class T>
T hduLerp(const T &v0, const T &v1, double t)
{
    return v0 * (1.0 - t) + v1 * t;
}

template <class T>
T hduClamp(const T &val, const T &min_val, const T &max_val)
{
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

template <class T>
T hduRound(const T &val)
{
    return floor(val + 0.5);
}

template <class T>
T hduAbsValue(const T &val)
{
    if (val < 0) return -val;
    return val;
}

template <class T>
bool hduIsEqual(const T &v0, const T &v1, const T &epsilon)
{
    return (hduAbsValue(v0 - v1) <= epsilon);
}

template<class T>
T hduMin(T v0, T v1)
{
    return v0 < v1 ? v0 : v1;
}

template<class T>
T hduMax(T v0, T v1)
{
    return v0 > v1 ? v0 : v1;
}

#endif /* __cplusplus */

#endif /* hduMath_H_ */

/*****************************************************************************/
