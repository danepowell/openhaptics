/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  main.c

Description: 

  This example demonstrates how to render a custom force effect using the
  callback effect type of HLAPI.  The effect in this example is an inertia
  effect, which simulates a point mass being dragged around by the haptic
  device.

******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <cstdio>
#include <iostream>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <HL/hl.h>
#include <HD/hd.h>

#include <HDU/hduVector.h>
#include <HDU/hduError.h>

/* Point mass structure, represents a draggable mass. */
struct PointMass
{
    hduVector3Dd m_position;
    hduVector3Dd m_velocity;
    HDdouble m_mass; 
    HDdouble m_kStiffness;
    HDdouble m_kDamping;
};


/******************************************************************************
 Servo loop thread callback.  Computes a force effect.
******************************************************************************/
void HLCALLBACK computeForceCB(HDdouble force[3], HLcache *cache, void *userdata)
{
    PointMass *pPointMass = static_cast<PointMass *>(userdata);

    // Get the time delta since the last update.
    HDdouble instRate;
    hdGetDoublev(HD_INSTANTANEOUS_UPDATE_RATE, &instRate);
    HDdouble deltaT = 1.0 / instRate;
    
    // Get the current proxy position from the state cache.
    // Note that the effect state cache is maintained in workspace coordinates,
    // so we don't need to do any transformations in using the proxy
    // position for computing forces.
    hduVector3Dd proxyPos;
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxyPos);
    
    // Compute the inertia force based on pulling the point mass around
    // by a spring.
    hduVector3Dd springForce = pPointMass->m_kStiffness * 
        (proxyPos - pPointMass->m_position);
    hduVector3Dd damperForce = -pPointMass->m_kDamping * pPointMass->m_velocity;
    hduVector3Dd inertiaForce = springForce + damperForce;
        
    // Perform Euler integration of the point mass state.
    hduVector3Dd acceleration = inertiaForce / pPointMass->m_mass;
    pPointMass->m_velocity += acceleration * deltaT;    
    pPointMass->m_position += pPointMass->m_velocity * deltaT;
                                  
    // Send the opposing force to the device.
    force[0] += -inertiaForce[0];
    force[1] += -inertiaForce[1];
    force[2] += -inertiaForce[2];
}


/******************************************************************************
 Servo loop thread callback called when the effect is started.
******************************************************************************/
void HLCALLBACK startEffectCB(HLcache *cache, void *userdata)
{
    PointMass *pPointMass = (PointMass *) userdata;
    
    fprintf(stdout, "Custom effect started\n");

    // Initialize the position of the mass to be at the proxy position.
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, pPointMass->m_position);

    pPointMass->m_velocity.set(0, 0, 0);
}


/******************************************************************************
 Servo loop thread callback called when the effect is stopped.
******************************************************************************/
void HLCALLBACK stopEffectCB(HLcache *cache, void *userdata)
{
    fprintf(stdout, "Custom effect stopped\n");
}


/******************************************************************************
 Initializes the control parameters used for simulating the point mass.
******************************************************************************/
void initPointMass(PointMass *pPointMass)
{
    pPointMass->m_mass = 0.001; // Kg        

    // Query HDAPI for the max spring stiffness and then tune it down to allow
    // for stable force rendering throughout the workspace.
    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &pPointMass->m_kStiffness);
    pPointMass->m_kStiffness *= 0.5;

    // Compute damping constant so that the point mass motion is
    // critically damped.
    pPointMass->m_kDamping = 2 * sqrt(pPointMass->m_mass *
                                      pPointMass->m_kStiffness);
}


/******************************************************************************
 Main function.
******************************************************************************/
int main(int argc, char *argv[])
{
    HHD hHD;
    HHLRC hHLRC;
    HDErrorInfo error;

    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError())) 
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        return -1;
    }
    hdMakeCurrentDevice(hHD);

    hHLRC = hlCreateContext(hHD);
    hlMakeCurrent(hHLRC);
    
    hlDisable(HL_USE_GL_MODELVIEW);

    HLuint effect = hlGenEffects(1);        

    // Initialize the point mass.
    PointMass pointMass;
    initPointMass(&pointMass);

    hlBeginFrame();

    hlCallback(HL_EFFECT_COMPUTE_FORCE, (HLcallbackProc) computeForceCB, &pointMass);
    hlCallback(HL_EFFECT_START, (HLcallbackProc) startEffectCB, &pointMass);
    hlCallback(HL_EFFECT_STOP, (HLcallbackProc) stopEffectCB, &pointMass);

    hlStartEffect(HL_EFFECT_CALLBACK, effect);

    hlEndFrame();

    fprintf(stdout, "Press any key to stop the effect\n");
    while (!_kbhit())
    {
        // Check for any errors.
        HLerror error;
        while (HL_ERROR(error = hlGetError()))
        {
            fprintf(stderr, "HL Error: %s\n", error.errorCode);
            
            if (error.errorCode == HL_DEVICE_ERROR)
            {
                hduPrintError(stderr, &error.errorInfo,
                    "Error during haptic rendering\n");
            }
        }                  
    }

    hlBeginFrame();
    hlStopEffect(effect);
    hlEndFrame();

    fprintf(stdout, "Shutting down...\n");
    getch();

    hlDeleteEffects(effect, 1);

    hlDeleteContext(hHLRC);
    hdDisableDevice(hHD);

    return 0;
}

/******************************************************************************/
