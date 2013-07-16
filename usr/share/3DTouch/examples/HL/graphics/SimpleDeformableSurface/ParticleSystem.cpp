/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  ParticleSystem.cpp

Description:

  Set of particles with springs and other constraints.
*******************************************************************************/

#if defined(WIN32)
# include "windows.h"
#endif

#include <assert.h>
#include <HDU/hduMath.h>
#include "SpringConstraint.h"
#include "DynamicsMath.h"
#include "Constraint.h"
#include "NailConstraint.h"
#include "MouseSpringConstraint.h"
#include "HapticDeviceConstraint.h"
#include "ParticleSystem.h"

#if defined(WIN32) || defined(linux)
# include <GL/gl.h>
#elif defined(__APPLE__)
# include <OpenGL/gl.h>
#endif

#include "UnProjectUtilities.h" // wrapper function for gluUnProject()

// the number of values (double) needed to store the state for one particle
static const int kStateSize = 6;

static const double kGravityDef = -9.8f;

static const double kMaxVelocity = 10;
static const double kMaxAcceleration = 100;

ParticleSystem::ParticleSystem()
{
    restitution = .5;
    drag = .01f;
    gravity = 9.8f; //*5;
    t = 0;
        
    mouseSpring = NULL;
    hapticDeviceConstraint = NULL;
        
    design = true;

    //odeSolver = new OdeSolverEuler();
    odeSolver = new OdeSolverRungeKutta4();
}

ParticleSystem::~ParticleSystem()
{
    ClearSystem();
    delete odeSolver;
}

// Adds a new particle with position (x, y, z) and returns a reference to the
// particle.
// If there is no space left for a new particle, a reference to the last
// particle is returned, but no new particle is created.
Particle& ParticleSystem::AddParticle(double x, double y, double z, double inMass)
{
    assert(design);
        
    Particle* part;
    if (inMass == 0)
        part = new Particle();
    else
        part = new Particle(inMass);
        
    part->x[0] = x;
    part->x[1] = y;
    part->x[2] = z;
        
    particles.push_back(part);
        
    return *part;
}

void ParticleSystem::AddParticle(Particle* p)
{
    assert(design);
        
    particles.push_back(p);
}

void ParticleSystem::AddConstraint(Constraint* c)
{
    assert(design);
        
    constraints.push_back(c);
}

NailConstraint* ParticleSystem::AddNailConstraint(int p)
{
    assert(design);
        
    // rely on FlexToSystem to fix the constraint
    NailConstraint* nc = new NailConstraint(p);
        
    AddConstraint(nc);
        
    return nc;
}

SpringConstraint* ParticleSystem::AddSpringConstraint(int p1, int p2)
{
    assert(design);
        
    // rely on FlexToSystem to fix the constraint
    SpringConstraint* sc = new SpringConstraint(p1, p2);
        
    AddConstraint(sc);
        
    return sc;
}

SpringConstraint* ParticleSystem::AddSpringConstraint(int p1, int p2, float length)
{
    assert(design);
        
    SpringConstraint* sc = new SpringConstraint(p1, p2, length);
        
    AddConstraint(sc);
        
    return sc;
}

void ParticleSystem::DeleteConstraint(Constraint* c)
{
    assert(c != NULL);
        
    constraints.remove(c);
        
    delete c;
}

void ParticleSystem::AddMouseSpringConstraint(void)
{
    assert(design);
        
    mouseSpring = new MouseSpringConstraint;
        
    mouseSpring->SetState(false);
        
    AddConstraint(mouseSpring);
}

void ParticleSystem::AddHapticDeviceConstraint(void)
{
    assert(design);
        
    hapticDeviceConstraint = new HapticDeviceConstraint;
        
    hapticDeviceConstraint->SetState(false);
        
    AddConstraint(hapticDeviceConstraint);
}

// Find the particle nearest to 3D position
int ParticleSystem::GetClosestParticle(const hduVector3Dd& pos)
{
    int closestPart = -1;
    double closestDist, currDist;   
        
    typedef ParticleListT::const_iterator LI; // constant because not modifying list
    LI pi;
    int i;
        
    for (pi = particles.begin(), i = 0; pi != particles.end(); ++pi, i++)
    {
        Particle& p = **pi;
        assert(&p != NULL);
                
        currDist = pos.distanceSqr(p.x);                
        if (closestPart == -1 || (currDist < closestDist))
        {
            closestDist = currDist;
            closestPart = i;
        }
    }
        
    return closestPart;
}

// Find the particle nearest to x, y
int ParticleSystem::GetClosestParticle(int x, int y)
{
    int closestPart = -1;
    double closestDist, currDist;   

    hduVector3Dd mousePos;
    hduVector3Dd mouseNear, mouseFar;

    if (useDepthForMouseZ)
        UnProjectUtilities::GetMousePosition(x, y, mousePos);
    else
        UnProjectUtilities::GetMouseRay(x, y, mouseNear, mouseFar);

        
    typedef ParticleListT::const_iterator LI; // constant because not modifying list
    LI pi;
    int i;
        
    for (pi = particles.begin(), i = 0; pi != particles.end(); ++pi, i++)
    {
        Particle& p = **pi;
        assert(&p != NULL);
                
        if (useDepthForMouseZ)
            currDist = p.x.distance(mousePos);
        else
            currDist = UnProjectUtilities::GetDistanceFromLine(p.x, mouseNear, mouseFar);
                
        if (closestPart == -1 || (currDist < closestDist))
        {
            closestDist = currDist;
            closestPart = i;
        }
    }
        
    if (closestDist < kClosenessThreshold)
        return closestPart;
    else
        return -1;
}

void ParticleSystem::ActivateMouseSpring(int x, int y)
{
    assert(!design);
        
    assert(mouseSpring != NULL);
        
    int closestPart = GetClosestParticle(x, y);
        
    if (closestPart != -1)
    {
        hduVector3Dd pos;
                
        hduVector3Dd mouseNear, mouseFar;
        UnProjectUtilities::GetMouseRay(x, y, mouseNear, mouseFar);
                
        hduVector3Dd particlePos = particles[closestPart]->x;
//      hduVector3Dd projection = dot(particlePos - mouseNear, mouseFar - mouseNear);
                
        hduVector3Dd mouseProjected = UnProjectUtilities::GetLineIntersectPlaneZ(mouseNear, mouseFar, particlePos[2]);

        mouseSpring->SetParticle(closestPart);
        mouseSpring->SetPosition(mouseProjected);
        mouseSpring->SetState(true);
    }
}

void ParticleSystem::DeactivateMouseSpring(void)
{
    mouseSpring->SetState(false);
}

void ParticleSystem::MouseMove(int x, int y)
{
    assert(mouseSpring != NULL);

    hduVector3Dd mouseNear, mouseFar;
    UnProjectUtilities::GetMouseRay(x, y, mouseNear, mouseFar);
        
    hduVector3Dd particlePos = particles[mouseSpring->GetParticle()]->x;
    hduVector3Dd mouseProjected = UnProjectUtilities::GetLineIntersectPlaneZ(mouseNear, mouseFar, particlePos[2]);

    mouseSpring->SetPosition(mouseProjected);

}

void ParticleSystem::ActivateHapticDeviceConstraint(void)
{
    assert(hapticDeviceConstraint != NULL);
    hapticDeviceConstraint->SetForce(hduVector3Dd(0,0,0));
    hapticDeviceConstraint->SetState(true);
}

void ParticleSystem::DeactivateHapticDeviceConstraint(void)
{
    assert(hapticDeviceConstraint != NULL);
    hapticDeviceConstraint->SetState(false);
}


void ParticleSystem::HapticDeviceMove(const hduVector3Dd& pos,
                                      const hduVector3Dd& force)
{
    assert(hapticDeviceConstraint != NULL);

    int closestParticle = GetClosestParticle(pos);

    hapticDeviceConstraint->SetParticle(closestParticle);
    hapticDeviceConstraint->SetForce(force);
}


void ParticleSystem::StartConstructingSystem(void)
{
    // delete the mouse spring if there is one
    if (mouseSpring != NULL)
    {
        constraints.remove(mouseSpring);
        delete mouseSpring;
        mouseSpring = NULL;
    }
        
    // delete the hapticDeviceConstraint if there is one
    if (hapticDeviceConstraint != NULL)
    {
        constraints.remove(hapticDeviceConstraint);
        delete hapticDeviceConstraint;
        hapticDeviceConstraint = NULL;
    }
        
    design = true;
}

void ParticleSystem::FinishConstructingSystem(void)
{
    // make sure we have a mouse spring
    if (mouseSpring == NULL)
        AddMouseSpringConstraint();
                
        // make sure we have a haptic device constraint
    if (hapticDeviceConstraint == NULL)
        AddHapticDeviceConstraint();
                
    x0.resize(particles.size() * kStateSize);
    xFinal.resize(particles.size() * kStateSize);
        
        // clear any velocities
    for (unsigned int i = 0; i < particles.size(); i++)
    {
        particles[i]->v = hduVector3Dd(0,0,0);
    }

    ParticlesStateToArray(&xFinal[0]);

    odeSolver->setSize(particles.size() * kStateSize);

    typedef ConstraintListT::const_iterator LI; // constant because not modifying list
    LI ci;

    for (ci = constraints.begin(); ci != constraints.end(); ++ci)
    {
        Constraint& c = **ci;
        assert(&c != NULL);
                
        c.FlexToSystem(particles);
    }
        
    design = false;
}

void ParticleSystem::ClearSystem(void)
{
    typedef ConstraintListT::iterator CLI; // constant because not modifying list
        
    CLI ci;
        
    while (constraints.size() > 0)
    {
        ci = constraints.begin();
                
        Constraint& c = **ci;
        assert(&c != NULL);
                
        delete *ci;
                
        constraints.erase(ci);
    }
        
    mouseSpring = NULL;
    hapticDeviceConstraint = NULL;

    typedef ParticleListT::iterator PLI; // constant because not modifying list
        
    PLI pi;

    while (particles.size() > 0)
    {
        pi = particles.begin();
                
        Particle& p = **pi;
        assert(&p != NULL);
                
        delete *pi;

        particles.erase(pi);
    }
        
    assert(particles.size() == 0);
    assert(constraints.size() == 0);
}

void ParticleSystem::Draw(void)
{
    typedef ConstraintListT::const_iterator LI; // constant because not modifying list
        
    LI ci;
        
    for (ci = constraints.begin(); ci != constraints.end(); ++ci)
    {
        Constraint& c = **ci;
        assert(&c != NULL);
                
        c.Draw(particles);
    }

    Particle::BeginDraw();
        
    for (unsigned int i = 0; i < particles.size(); i++)
    {
        particles[i]->Draw();
    }
        
    Particle::EndDraw();
        
}

void ParticleSystem::AdvanceSimulation(double tPrev, double tCurr)
{
    // copy xFinal back to x0
    for(unsigned int i=0; i<kStateSize * particles.size(); i++)
    {
        x0[i] = xFinal[i];
    }

    //ode(x0, xFinal, kStateSize * mBodies.size(), tPrev, tCurr, dxdt);
    odeSolver->solve(x0, xFinal, tPrev, tCurr, DxDt, this);
        
    // copy d/dt X(tNext) into state variables
    ParticlesArrayToState(&xFinal[0]);
}

bool ParticleSystem::DxDt(double t, nvectord &x, nvectord &xdot, void *userData)
{
    ParticleSystem *pThis = static_cast<ParticleSystem *>(userData);
    assert(pThis);
        
    // Put data in x[] into particles
    pThis->ParticlesArrayToState(&x[0]);
        
    pThis->ClearForces();

    // evaluate all forces
    pThis->ApplyRegularForces();
    pThis->ApplyDragForces();
    pThis->ApplyConstraintForces();

    pThis->LimitStateChanges();

    pThis->DdtParticlesStateToArray(&xdot[0]);

    return false;
}

void ParticleSystem::ParticlesStateToArray(double *dst)
{
    typedef ParticleListT::const_iterator LI; // constant because not modifying list
        
    LI pi;

    for (pi = particles.begin(); pi != particles.end(); ++pi)
    {
        Particle& p = **pi;
        assert(&p != NULL);

        *(dst++) = p.x[0];
        *(dst++) = p.x[1];
        *(dst++) = p.x[2];
        *(dst++) = p.v[0];
        *(dst++) = p.v[1];
        *(dst++) = p.v[2];
    }
}

void ParticleSystem::ParticlesArrayToState(double *dst)
{
    ParticleListT::const_iterator pi; // constant because not modifying list

    for (pi = particles.begin(); pi != particles.end(); ++pi)
    {
        Particle& p = **pi;
        assert(&p != NULL);
                
        p.x[0] = *(dst++);
        p.x[1] = *(dst++);
        p.x[2] = *(dst++);
        p.v[0] = *(dst++);
        p.v[1] = *(dst++);
        p.v[2] = *(dst++);
    }
}

void ParticleSystem::ClearForces(void)
{
    for (unsigned int i = 0; i < particles.size(); i++)
    {
        particles[i]->f = hduVector3Dd(0,0,0);
    }
}

void ParticleSystem::ApplyRegularForces(void)
{
    Particle *p;

    for (unsigned int i = 0; i < particles.size(); i++)
    {
        p = particles[i];
        p->f[1] -= gravity * p->mass;
        //p->f -= p->v * drag;
    }
}

void ParticleSystem::ApplyDragForces(void)
{
    Particle *p;

    for (unsigned int i = 0; i < particles.size(); i++)
    {
        p = particles[i];
        p->f -= p->v * drag;
    }
}

void ParticleSystem::ApplyConstraintForces(void)
{
    typedef ConstraintListT::const_iterator LI; // constant because not modifying list
        
    LI ci;
        
    for (ci = constraints.begin(); ci != constraints.end(); ++ci)
    {
        Constraint& c = **ci;
        assert(&c != NULL);
                
        c.ApplyConstraint(particles);
    }
}

#if LIMIT_CHANGES_PER_PARTICLE // an experimental alternative
// For each particle, make sure the max v and f aren't exceeded
void ParticleSystem::LimitStateChanges(void)
{
    ParticleListT::const_iterator pi; // constant because not modifying list
    double vMag;
    double fMag;
        
    for (pi = particles.begin(); pi != particles.end(); ++pi)
    {
        Particle& p = **pi;
        assert(&p != NULL);

        vMag = norm(p.v);
        if (vMag > kMaxVelocity)
            p.v *= kMaxVelocity / vMag;

        fMag = norm(p.f);
        if (fMag > kMaxAcceleration * p.mass)
            p.f *= kMaxAcceleration * p.mass / fMag;
    }
}
#endif

// Scale back the v and f of all particles to ensure
// the max values aren't exceeded, effectively 
// reducing the size of the time step taken
void ParticleSystem::LimitStateChanges(void)
{
    ParticleListT::const_iterator pi; // constant because not modifying list
    double vMag, vBiggest = 0;
    double fMag, fBiggest = 0;

    // take advantage of the fact that all particles have the same mass
    double maxForce = kMaxAcceleration * Particle::mass;

    for (pi = particles.begin(); pi != particles.end(); ++pi)
    {
        Particle& p = **pi;
        assert(&p != NULL);

        vMag = p.v.magnitude();
        vBiggest = hduMax(vMag, vBiggest);

        fMag = p.f.magnitude();
        fBiggest = hduMax(fMag, fBiggest);
    }

    if (vBiggest > kMaxVelocity)
    {
        for (pi = particles.begin(); pi != particles.end(); ++pi)
        {
            (*pi)->v *= kMaxVelocity / vBiggest;
        }
    }

    if (fBiggest > maxForce)
    {
        for (pi = particles.begin(); pi != particles.end(); ++pi)
        {
            (*pi)->f *= maxForce / fBiggest;
        }
    }
}

void ParticleSystem::DdtParticlesStateToArray(double *xdot)
{
    ParticleListT::const_iterator pi; // constant because not modifying list

    for (pi = particles.begin(); pi != particles.end(); ++pi)
    {
        Particle& p = **pi;
        assert(&p != NULL);

        int i;

        // copy d/dt x(t) = v(t) into xdot
        for(i=0; i<3; i++)
            *(xdot++) = p.v[i];

        // copy d/dt v(t) = f(t) / m into xdot
        for(i=0; i<3; i++)
            *(xdot++) = p.f[i] / p.mass;
    }
}

void ParticleSystem::SetSpringConstant(double inKS)
{
    SpringConstraint::SetSpringConstant(inKS);
}

void ParticleSystem::SetSpringDampingConstant(double inKD)
{
    SpringConstraint::SetSpringDampingConstant(inKD);
}

double ParticleSystem::GetSpringConstant(void)
{
    return SpringConstraint::GetSpringConstant();
}

double ParticleSystem::GetSpringDampingConstant(void)
{
    return SpringConstraint::GetSpringDampingConstant();
}

/******************************************************************************/
