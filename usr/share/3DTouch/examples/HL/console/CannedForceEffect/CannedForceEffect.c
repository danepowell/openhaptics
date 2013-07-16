/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  CannedForceEffect.c

Description: 

  This example demonstrates starting and stopping built-in force effects
  for HLAPI.  Force effects can either persist using hlStartEffect and
  hlStopEffect to control their duration or they can be triggered and
  run for a predefined duration using hlTriggerEffect.  This example also
  shows how to invoke force effects using events.

******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#include <stdio.h>
#include <assert.h>

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <HD/hd.h>
#include <HL/hl.h>

#include <HDU/hduError.h>

void HLCALLBACK buttonCB(HLenum event, HLuint object, HLenum thread, 
                       HLcache *cache, void *userdata);

/*******************************************************************************
 Main function.
*******************************************************************************/
int main(int argc, char *argv[])
{
    HHD hHD;
    HHLRC hHLRC;
    HDErrorInfo error;
    HLuint friction, spring;
    HLerror frameError;

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

    spring = hlGenEffects(1);

    /* Add a callback to handle button down in the collision thread. */
    hlAddEventCallback(HL_EVENT_1BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       buttonCB, &spring);
    hlAddEventCallback(HL_EVENT_1BUTTONUP, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       buttonCB, &spring);
    hlAddEventCallback(HL_EVENT_2BUTTONDOWN, HL_OBJECT_ANY, HL_CLIENT_THREAD, 
                       buttonCB, 0);

    /* Start an ambient friction effect. */
    friction = hlGenEffects(1);

    hlBeginFrame();
    hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.2);
    hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.5);
    hlStartEffect(HL_EFFECT_FRICTION, friction);
    hlEndFrame();

    printf("Move around to feel the ambient stick-slip friction.\n\n");
    printf("Press and hold the primary stylus button to feel the spring effect.\n\n");
    printf("Press the second stylus button to trigger an impulse.\n\n");

    /* Run the main loop. */
    while (!_kbhit())
    {
        hlBeginFrame();

        /* Poll for events.  Note that client thread event callbacks get
           dispatched from within a frame here, so we can safely start/stop
           effects from the event callback directly */
        hlCheckEvents();

        hlEndFrame();

        /* Check for any errors. */
        while (HL_ERROR(frameError = hlGetError()))
        {
            fprintf(stderr, "HL Error: %s\n", frameError.errorCode);
            
            if (frameError.errorCode == HL_DEVICE_ERROR)
            {
                hduPrintError(stderr, &frameError.errorInfo,
                    "Error during haptic rendering\n");
            }
        }
    }

    /* Stop the friction effect. */
    hlBeginFrame();
    hlStopEffect(friction);
    hlEndFrame();

    hlDeleteEffects(friction, 1);
    hlDeleteEffects(spring, 1);

    hlDeleteContext(hHLRC);
    hdDisableDevice(hHD);

    return 0;
}

void HLCALLBACK buttonCB(HLenum event, HLuint object, HLenum thread, 
                         HLcache *cache, void *userdata)
{
    if (event == HL_EVENT_1BUTTONDOWN)
    {
        HLuint spring = *((HLuint *) userdata); 

        HDdouble anchor[3];
        hlCacheGetDoublev(cache, HL_PROXY_POSITION, anchor);

        hlEffectd(HL_EFFECT_PROPERTY_GAIN, 0.8);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 1.0);
        hlEffectdv(HL_EFFECT_PROPERTY_POSITION, anchor);
        hlStartEffect(HL_EFFECT_SPRING, spring);
    }
    else if (event == HL_EVENT_1BUTTONUP)
    {
        HLuint spring = *((HLuint *) userdata); 

        hlStopEffect(spring);    
    }
    else if (event == HL_EVENT_2BUTTONDOWN)
    {
        static const HDdouble direction[3] = { 0, 0, 1 };
        static const HDdouble duration = 100; /* ms */

        /* Trigger an impulse by commanding a force with a
           direction and magnitude for a small duration. */
        hlEffectd(HL_EFFECT_PROPERTY_DURATION, duration);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, 0.8);
        hlEffectdv(HL_EFFECT_PROPERTY_DIRECTION, direction);
        hlTriggerEffect(HL_EFFECT_CONSTANT);
    }
}

/******************************************************************************/
