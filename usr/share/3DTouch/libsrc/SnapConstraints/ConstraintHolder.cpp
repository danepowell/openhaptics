/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ConstraintHolder.cpp

Description: 

  Implements a container for SnapConstraints.

******************************************************************************/

#include "SnapConstraintsAfx.h"
#include <SnapConstraints/SnapConstraint.h>
#include <SnapConstraints/ConstraintHolder.h>

#include <list>

using namespace std;

namespace SnapConstraints
{

class ConstraintHolderImp : public ConstraintHolder
{
public:

    typedef list<SnapConstraint *> ConstraintList;

    ConstraintHolderImp() {}
    virtual ~ConstraintHolderImp() { clearConstraints(); }

    void addConstraintFront(SnapConstraint *pConstraint)
    {
        if (pConstraint)
        {
            m_list.push_front(pConstraint);
        }
    }

    void addConstraintBack(SnapConstraint *pConstraint)
    {
        if (pConstraint)
        {
            m_list.push_back(pConstraint);
        }
    }

    void removeConstraint(SnapConstraint *pConstraint)
    { 
        m_list.remove(pConstraint);
    }

    void clearConstraints();

    /* Iteration methods. */
    void begin() { m_it = m_list.begin(); }
    void next() { ++m_it; }
    bool done() const { return m_it == m_list.end(); }
    const SnapConstraint *current() const { return *m_it; }
    SnapConstraint *current() { return *m_it; }

private:

    ConstraintList m_list;
    ConstraintList::const_iterator m_it;
};

/******************************************************************************
 Create
******************************************************************************/
ConstraintHolder *ConstraintHolder::create()
{
    return new ConstraintHolderImp;
}

/******************************************************************************
 Destroy
******************************************************************************/
void ConstraintHolder::destroy(ConstraintHolder *&pInstance)
{
    if (pInstance)
    {
        ConstraintHolderImp *pImp = 
            static_cast<ConstraintHolderImp *>(pInstance);
        delete pImp;
        pInstance = 0;
    }
}

/******************************************************************************
 Clear
******************************************************************************/
void ConstraintHolderImp::clearConstraints()
{
    ConstraintList::iterator it = m_list.begin();
    ConstraintList::iterator iend = m_list.end();
    for (; it != iend; ++it)
    {
        SnapConstraint *pConstraint = *it;
        if (pConstraint && pConstraint->isAutoDelete())
        {
            delete pConstraint;            
        }
    }
    m_list.clear();
    begin();
}

} /* namespace SnapConstraints */

/*****************************************************************************/
