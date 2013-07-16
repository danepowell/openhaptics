/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  OdeSolver.h

Description:

  Ordinary differential equation solver.
*******************************************************************************/


#if !defined(AFX_ODESOLVER_H__CDEE86EE_DBBF_4FDF_B9B0_9A54FEC95521__INCLUDED_)
#define AFX_ODESOLVER_H__CDEE86EE_DBBF_4FDF_B9B0_9A54FEC95521__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DynamicsMath.h"

//
// The derivative function takes x at time time and determines xdot = d/dt x(t)
// So, if  x = { position, velocity }
// then xdot = { velocity, acceleration }
//
// Note that there are two special conditions that may arise during
// the computation of xdot:
//  (1) there may be a discontinuity, and thus x will be modified directly
//  (2) an invalid state may be reached because the time step was too great
// 
// If (1) occurs, x will be modified but no message is currently sent to
// the solver.  OdeSolverEuler deals with this correctly, but 
// OdeSolverMidpoint and OdeSolverRungeKutta4 do not.
//
// If (2) occurs, the function will return false and a smaller step forward
// should be taken.  This is not currently implemented.
//
// Because of these problems, only OdeSolverEuler should currently be used
// with SimpleRigidBodyDynamics.  SimpleDeformableSurface can use any of
// the solvers.
//
typedef bool (*DerivFunc)(double t, nvectord &x, nvectord &xdot, void *userData);

class IOdeSolver  
{
public:
    virtual void setSize(int vecSize) = 0;
    virtual void solve(nvectord &x0, nvectord &xFinal, double t0, double t1, DerivFunc dxdt, void *userData) = 0;
};

class OdeSolverEuler : public IOdeSolver
{
public:
    OdeSolverEuler();
    virtual ~OdeSolverEuler();
        
    virtual void setSize(int vecSize);
    virtual void solve(nvectord &x0, nvectord &xFinal, double t0, double t1, DerivFunc dxdt, void *userData);

private:
    // vector used by the ode solver; avoid continuously re-allocating
    nvectord fStart;
};

class OdeSolverMidpoint : public IOdeSolver
{
public:
    OdeSolverMidpoint();
    virtual ~OdeSolverMidpoint();
        
    virtual void setSize(int vecSize);
    virtual void solve(nvectord &x0, nvectord &xFinal, double t0, double t1, DerivFunc dxdt, void *userData);

private:
    // vectors used by the ode solver; avoid continuously re-allocating
    nvectord fStart;
    nvectord fMid;
};

class OdeSolverRungeKutta4 : public IOdeSolver
{
public:
    OdeSolverRungeKutta4();
    virtual ~OdeSolverRungeKutta4();
        
    virtual void setSize(int vecSize);
    virtual void solve(nvectord &x0, nvectord &xFinal, double t0, double t1, DerivFunc dxdt, void *userData);

private:
    // vectors used by the ode solver; avoid continuously re-allocating
    nvectord k1, k2, k3, k4;
};

#endif // !defined(AFX_ODESOLVER_H__CDEE86EE_DBBF_4FDF_B9B0_9A54FEC95521__INCLUDED_)

/******************************************************************************/
