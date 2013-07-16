/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  StatsSampler.h

Description: 

  Manages statistics.

*******************************************************************************/

#ifndef StatsSampler_H_
#define StatsSampler_H_

#include <cassert>
#include <ostream>
#include <cmath>

/* Abstraction used to determine types as a function of other types. */
template <class T>
class TypeMapper
{};

/* TypeMapper specialized for floats. */
template <> class TypeMapper<float>
{
public:
    typedef double SumType;
};

/* TypeMapper specialized for doubles. */
template <> class TypeMapper<double>
{
public:
    typedef double SumType;
};

/* TypeMapper specialized for ints. */
template <> class TypeMapper<int>
{
public:
    typedef long SumType;
};
      
/* TypeMapper specialized for uchar. */
template <> class TypeMapper<unsigned char>
{
public:
    typedef int SumType;
};

template <class T>
class StatsSampler 
{
public:
    /* Default constructor. */
    StatsSampler();
    /* Destructor. */
    ~StatsSampler();

    /* Reset all stats. */
    void clear();
    /* Add a single sample to the collection. */
    void sample(const T& x);
    /* Add a sample multiple times. */
    void sampleMulti(const T& x, int m = 1);
    /* Remove a sample from the collection. */

    void updateMinMax(const T& x);

    T getMin() const
    {
        assert(n()>0);
        return m_min;
    }

    T getMax() const
    {
        assert(n()>0);
        return m_max;
    }

    /* Number of samples. */
    int n() const { return m_n; }

    /* Sum of all samples. */
    typename TypeMapper<T>::SumType sum() const { return m_sx; } 

    /* Sum of square of all samples. */
    typename TypeMapper<T>::SumType sumsqr() const { return m_ssx; }

    /* Mean. */
    double u() const;
    /* Variance. */
    double var() const;
    /* Standard deviation. */
    double stdev() const;

private:
    int m_n;
    T m_min;
    T m_max;
    typename TypeMapper<T>::SumType m_sx, m_ssx;
};

typedef StatsSampler<double> StatsSamplerd;
typedef StatsSampler<float> StatsSamplerf;
typedef StatsSampler<unsigned char> StatsSampleru;
typedef StatsSampler<int> StatsSampleri;

template <class T>
StatsSampler<T>::StatsSampler(): m_n(0), m_sx(0), m_ssx(0), m_min(0), m_max(0)
{}

template <class T>
StatsSampler<T>::~StatsSampler() {}

template <class T>
void StatsSampler<T>::clear() { m_n = 0; m_sx = 0, m_ssx = 0; }

template <class T>
inline void StatsSampler<T>::updateMinMax(const T& x)
{
    if (m_n==0)
    {
        m_min = x;
        m_max = x;
    }
    else if (x<m_min)
    {
        m_min = x;
    }
    else if (x>m_max)
    {
        m_max = x;
    }
}

template <class T>
inline void StatsSampler<T>::sample(const T& x)
{ 
    updateMinMax(x);
    m_sx += x; 
    m_ssx += x*x; 
    m_n++; 
}

template <class T>
inline void StatsSampler<T>::sampleMulti(const T& x, int m) 
{ 
    updateMinMax(x);
    m_sx += m*x; 
    m_ssx += m*x*x; 
    m_n += m; 
}

template <class T>
double StatsSampler<T>::u() const  
{ 
    /* Must have some samples to average! */
    assert(m_n>0);
    if (m_n<=0)
        return 0.0;
    return 1.0*m_sx/n();
}

template <class T>
double StatsSampler<T>::var() const
{
    /* Must have at least 2 samples to avoid division by zero. */
    assert(m_n>1);
    if (m_n<=1)
        return 0.0;
    return 1.0 * (m_ssx  - (m_n * u() * u()))/(m_n-1);
}

template <class T>
double StatsSampler<T>::stdev() const
{
    double dvar = var();
    if (dvar < 0.0) return 0.0; /* Underflow, presumably. */
    return sqrt(dvar);
}

template <class T>
inline std::ostream& operator<<(std::ostream &os, const StatsSampler<T>& samp)
{ 
    int n = samp.n();
    os << "n " << samp.n() << " ";

    if (n>0)
        os << "mean " << samp.u() << " " ;
    else
        os << "mean UNDEF " ;

    if (n>1)
        os << "stdev " << samp.stdev() << " ";
    else
        os << "stdev UNDEF ";

    if (n>0)
        os << "min " << samp.getMin() << " ";
    else
        os << "min UNDEF ";

    if (n>0)
        os << "max " << samp.getMax();
    else
        os << "max UNDEF";

    return os;
}

#endif /* StatsSampler_H_ */

/******************************************************************************/
