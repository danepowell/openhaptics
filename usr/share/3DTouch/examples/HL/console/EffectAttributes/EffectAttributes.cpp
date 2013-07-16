/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  EffectAttributes.cpp

Description:
  
  Example of various effects and updating effects in place.

*******************************************************************************/
#ifdef  _WIN64
#pragma warning (disable:4996)
#endif

#if defined(WIN32)
# include <conio.h>
#else
# include "conio.h"
#endif

#include <stdlib.h>

#include <HD/hd.h>
#include <HL/hl.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

/* Handle to haptic device. */
HHD ghHD = HD_INVALID_HANDLE;

/* Handle to haptic rendering context. */
HHLRC ghHLRC;

/* Effect ID */
HLuint gEffect;

/* Effect properties */
float gGain = .2f;
float gMagnitude = .5f;

/******************************************************************************
 Initializes haptics.
******************************************************************************/
void initHL(void)
{
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "\nPress any key to quit.\n");
        getch();
        exit(-1);    
    }   
    
    // Create a haptic context for the device.  The haptic context maintains 
    // the state that persists between frame intervals and is used for
    // haptic rendering.
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);
    
    hlDisable(HL_USE_GL_MODELVIEW);

    gEffect = hlGenEffects(1);

}

/******************************************************************************
 Updates the effect with new properties.                                                                               
******************************************************************************/
void updateEffect()
{
    // Update the gain and magnitude of the current effect.
    // The effect must be active.  
    // Note that not all effects will use all of these 
    // properties.
    hlBeginFrame();
    HLboolean bActive = false;
    hlGetEffectbv(gEffect, HL_EFFECT_PROPERTY_ACTIVE, &bActive);
    if (bActive)
    {
        hlEffectd(HL_EFFECT_PROPERTY_GAIN, gGain);
        hlEffectd(HL_EFFECT_PROPERTY_MAGNITUDE, gMagnitude);    
        hlUpdateEffect(gEffect);
        // To get the effect type, use the appropriate effect 
        // query to get the integer representation of the effect,
        // then cast that as an HLenum to get the string name.
        printf("Effect properties for ");
        int nType;
        hlGetEffectiv(gEffect, HL_EFFECT_PROPERTY_TYPE, (HLint*)&nType);
        printf("%s\n",(HLenum)nType);
    }
    else
    {
        printf("No effect active\n");
    }
    printf("  Magnitude: %.1f\n",gMagnitude);
    printf("  Gain: %.1f\n",gGain);
    printf("\n");
    hlEndFrame();
}

/******************************************************************************
 Stops the effect.
******************************************************************************/
void stopEffect()
{
    hlBeginFrame();
    // Only necessary to stop the effect if it's active (i.e. on).
    HLboolean bActive = false;
    hlGetEffectbv(gEffect, HL_EFFECT_PROPERTY_ACTIVE, &bActive);
    if (bActive)
    {
        hlStopEffect(gEffect);
    }
    hlEndFrame();
    
}

/******************************************************************************
 Starts an effect type.
******************************************************************************/
void startEffectType(HLenum newEffectType)
{
    // First stop the current effect.
    stopEffect();
    
    // Start the new effect, set whatever appropriate unique 
    // properties are necessary for each effect type.
    hlBeginFrame();
    if (newEffectType == HL_EFFECT_SPRING)
    {
        hduVector3Dd position;
        hlGetDoublev(HL_DEVICE_POSITION,position);
        hlEffectdv(HL_EFFECT_PROPERTY_POSITION, position);
    }
    else if (newEffectType == HL_EFFECT_CONSTANT)
    {
        hlEffectd(HL_EFFECT_PROPERTY_DURATION, 100);
        hlEffectdv(HL_EFFECT_PROPERTY_DIRECTION, hduVector3Dd(0,0,1));
    }

    hlStartEffect(newEffectType, gEffect);
    hlEndFrame();
    updateEffect();
}


/******************************************************************************
 Prints help.
******************************************************************************/
void printHelp()
{
    static const char help[] = {"\
-----------------------------------------------\n\
      Effect Attributes Example Menu Options\n\
===============================================\n\
Effect Types;\n\
[F] : Switch to friction effect\n\
[S] : Switch to spring effect\n\
[C] : Switch to constant effect\n\
[V] : Switch to viscous effect\n\
[N] : Switch to no effect\n\
\n\
Effect Properties:\n\
[-] : Decrease effect magnitude\n\
[+] : Increase effect magnitude\n\
[[] : Decrease effect gain\n\
[]] : Increase effect gain\n\
\n\
Miscellaneous:\n\
[H] : Print help menu\n\
[Q] : Quit\n\
\n\
(note: Setting gain/magnitude at high values may cause\n\
instability in some force effects and devices.)\n"};

    printf("%s\n\n", help);

}

/******************************************************************************
 Main function.
******************************************************************************/
int main(int argc, char* argv[])
{
    initHL();

    printHelp();
    
    // Loop until the user quits or an error occurs.
    bool bDone = false;
    while (!bDone)
    {
        if (_kbhit())
        {
            int key = toupper(getch());
            bool bNeedsUpdate = false;
            
            switch (key)
            {
                case 'F':
                    startEffectType(HL_EFFECT_FRICTION);
                    break;
                case 'S':
                    startEffectType(HL_EFFECT_SPRING);
                    break;
                case 'C':
                    startEffectType(HL_EFFECT_CONSTANT);
                    break;
                case 'V':
                    startEffectType(HL_EFFECT_VISCOUS);
                    break;
                case 'N':
                    stopEffect();
                    break;

                case ']':
                    gGain += 0.1f;
                    if (gGain > 1.0f)
                        gGain = 1.0f;
                    bNeedsUpdate = true;
                    break;
                case '[':
                    gGain -= 0.1f;
                    if (gGain < 0.0f)
                        gGain = 0.0f;
                    bNeedsUpdate = true;
                    break;
                case '+':
                case '=':
                    gMagnitude += 0.1f;
                    if (gMagnitude > 1.0f)
                        gMagnitude = 1.0f;
                    bNeedsUpdate = true;
                    break;
                case '-':
                case '_':
                    gMagnitude -= 0.1f;
                    if (gMagnitude < 0.0f)
                        gMagnitude = 0.0f;
                    bNeedsUpdate = true;
                    break;

                case 'Q':
                    bDone = true;
                    break;
                case 'H':
                default:
                    printHelp();
                    break;
            }

            if (bNeedsUpdate)
            {
                updateEffect();
            }

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
    }
    
    // Release the effect id.
    hlDeleteEffects(gEffect, 1);

    // Cleanup.
    hlMakeCurrent(NULL);
    hlDeleteContext(ghHLRC);
    hdDisableDevice(ghHD);

    return 0;
}

/******************************************************************************/

