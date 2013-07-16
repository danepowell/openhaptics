/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  OdeSolver.cpp

Description:

  Ordinary differential equation solver.

*******************************************************************************/


#include "SimpleRigidBodyDynamicsAfx.h"
#include "OdeSolver.h"

OdeSolverEuler::OdeSolverEuler()
{
}

OdeSolverEuler::~OdeSolverEuler()
{
}

void OdeSolverEuler::setSize(int vecSize)
{
    fStart.resize(vecSize);
}

// Given x0 at t0, find xFinal at t1 using the derivative function dxdt.
void OdeSolverEuler::solve(nvectord &x0, nvectord &xFinal, 
                           double t0, double t1, 
                           DerivFunc dxdt, void *userData)
{
    bool restartRequired = false;
    double h = t1 - t0;
        
    // Euler's method.
    restartRequired = dxdt(t0, x0, fStart, userData);
    assert(!restartRequired);
    xFinal = x0 + h * fStart;
}

OdeSolverMidpoint::OdeSolverMidpoint()
{

}

OdeSolverMidpoint::~OdeSolverMidpoint()
{

}

void OdeSolverMidpoint::setSize(int vecSize)
{
    fStart.resize(vecSize);
    fMid.resize(vecSize);
}

// Given x0 at t0, find xFinal at t1 using the derivative function dxdt
void OdeSolverMidpoint::solve(nvectord &x0, nvectord &xFinal, 
                              double t0, double t1, 
                              DerivFunc dxdt, void *userData)
{
    bool restartRequired = false;
    double h = t1 - t0;

    // Using the midpoint method.
    // We evaluate f = dxdt at t0 and at the midpoint.
    dxdt(t0, x0, fStart, userData);

    nvectord temp = x0 + h/2 * fStart;
    dxdt(t0 + h/2, temp, fMid, userData);

    xFinal = x0 + h * fMid;
}

OdeSolverRungeKutta4::OdeSolverRungeKutta4()
{
}

OdeSolverRungeKutta4::~OdeSolverRungeKutta4()
{
}

void OdeSolverRungeKutta4::setSize(int vecSize)
{
    k1.resize(vecSize);
    k2.resize(vecSize);
    k3.resize(vecSize);
    k4.resize(vecSize);
}

// Given x0 at t0, find xFinal at t1 using the derivative function dxdt
void OdeSolverRungeKutta4::solve(nvectord &x0, nvectord &xFinal, double t0, double t1, DerivFunc dxdt, void *userData)
{
    bool restartRequired = false;
    double h = t1 - t0;
    nvectord temp;
        
    // Runge-Kutta of order 4.
    dxdt(t0, x0, k1, userData);
    k1 *= h;
    temp = x0 + k1 * (double)0.5;
    dxdt(t0 + h * (double)0.5, temp, k2, userData);
    k2 *= h;
    temp = x0 + k2 * (double)0.5;
    dxdt(t0 + h * (double)0.5, temp, k3, userData);
    k3 *= h;
    temp = x0 + k3;
    dxdt(t0 + h, temp, k4, userData);
    k4 *= h;

    xFinal = x0 + k1 / (double)6 + k2 / (double)3 + k3 / (double)3 + k4 / (double)6;
}

/*****************************************************************************/
