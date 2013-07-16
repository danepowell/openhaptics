/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ParticleSystem.h

Description:

  Set of particles with springs and other constraints.

*******************************************************************************/

#ifndef ParticleSystem_H_
#define ParticleSystem_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <vector>
#include <iostream>
#include "DynamicsMath.h"
#include "Particle.h"
#include "OdeSolver.h"
#include <HD/hd.h>

class Constraint;
class NailConstraint;
class SpringConstraint;
class MouseSpringConstraint;
class HapticDeviceConstraint;

typedef std::vector<Particle*> ParticleListT;
typedef std::list<Constraint*> ConstraintListT;

const double kClosenessThreshold = 100; // limit for particle selection with mouse
const int kDim = 3; // dimensions of the system

class ParticleSystem
{
public:
    ParticleSystem();
    ~ParticleSystem();

    Particle& AddParticle(double x = 0, double y = 0, double z = 0, 
                          double inMass = 0);
    void AddParticle(Particle* p);

    void AddConstraint(Constraint* c);
    NailConstraint* AddNailConstraint(int p);
    SpringConstraint* AddSpringConstraint(int p1, int p2);
    SpringConstraint* AddSpringConstraint(int p1, int p2, float length);

    void DeleteConstraint(Constraint* c);
        
    int GetClosestParticle(int x, int y);
    int GetClosestParticle(const hduVector3Dd& pos);
    void AddMouseSpringConstraint(void);
    void AddHapticDeviceConstraint(void);
        
    void ActivateMouseSpring(int x, int y);
    void DeactivateMouseSpring(void);
    void MouseMove(int x, int y);
        
    void ActivateHapticDeviceConstraint(void);

    void DeactivateHapticDeviceConstraint(void);

    void HapticDeviceMove(const hduVector3Dd& pos,
                          const hduVector3Dd& force);

    /* For switching modes (design/simulation). */
    void StartConstructingSystem(void);
    void FinishConstructingSystem(void);
    void ClearSystem(void);
        
    void Draw(void);
    void AdvanceSimulation(double tPrev, double tCurr);

    void SetSpringConstant(double inKS);
    void SetSpringDampingConstant(double inKD);
    double GetSpringConstant(void);
    double GetSpringDampingConstant(void);

    ParticleListT particles;
    ConstraintListT constraints;
        
    float t;                // simulation clock
        
    float restitution;
    float drag;
    float gravity;
        
    nvectord x0; // particles.size() * kStateSize
    nvectord xFinal; // particles.size() * kStateSize);

    IOdeSolver *odeSolver;
        
    MouseSpringConstraint* mouseSpring;
    HapticDeviceConstraint* hapticDeviceConstraint;
        
    bool design;    // are we in design mode?
    bool useDepthForMouseZ;

private:
    hduVector3Dd GetLineIntersectPlaneZ(const hduVector3Dd &x1, 
                                        const hduVector3Dd &x2, 
                                        const double z);
    void ParticlesStateToArray(double *dst);
    void ParticlesArrayToState(double *dst);
    static bool DxDt(double t, nvectord &x, nvectord &xdot, void *userData);
    void LimitStateChanges(void);
    void DdtParticlesStateToArray(double *xdot);
    void ClearForces(void);
    void ApplyRegularForces(void);
    void ApplyDragForces(void);
    void ApplyConstraintForces(void);
};

#endif // ParticleSystem_H_

/*****************************************************************************/



