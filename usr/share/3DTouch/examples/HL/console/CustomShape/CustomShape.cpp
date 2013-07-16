/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name: 

  CustomShape.cpp

Description:
  
  Example of using a custom callback shape.  Renders a haptic sphere.

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
#include <HDU/hduError.h>
#include <HL/hl.h>

#include "HapticSphere.h"
#include "HapticRings.h"

/* Handle to haptic device. */
HHD ghHD = HD_INVALID_HANDLE;

/* Handle to haptic rendering context. */
HHLRC ghHLRC;

/* Global flag for controlling whether to render the haptic rings. */
bool gUseRings = false;

/* Snap distances to use for shape and rings. */
float gShapeSnapDistance = 0;
float gRingSnapDistance = 0;

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

    hlTouchableFace(HL_FRONT);
    hlTouchModel(HL_CONTACT);

    // Compute the snap distances to use based on approximating how much
    // force the user should need to exert to pull off.
    const double kShapeForce = 3.0; // Newtons
    const double kRingForce = 2.0; // Newtons

    HDdouble kStiffness;
    hdGetDoublev(HD_NOMINAL_MAX_STIFFNESS, &kStiffness);

    // We can get a good approximation of the snap distance to use by
    // solving the following simple force formula: 
    // >  F = k * x  <
    // F: Force in Newtons (N).
    // k: Stiffness control coefficient (N/mm).
    // x: Displacement (i.e. snap distance).
    gShapeSnapDistance = kShapeForce / kStiffness;
    gRingSnapDistance = kRingForce / kStiffness;
}

/******************************************************************************
 Renders the custom shapes.                                                                               
******************************************************************************/
void renderCustomShapes(const HapticSphere &sphere, HLuint sphereId,
                        const HapticRings &rings, HLuint ringsId)
{
    // Start haptic frame.
    hlBeginFrame();

    // Setup materials.
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.1f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.2f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.3f);

    hlTouchModelf(HL_SNAP_DISTANCE, gShapeSnapDistance);

    // First render the custom sphere by creating a callback shape and
    // specifying the callback functions to use.
    hlBeginShape(HL_SHAPE_CALLBACK, sphereId);
    hlCallback(HL_SHAPE_INTERSECT_LS, 
        (HLcallbackProc) HapticSphere::intersectSurface, (void *) &sphere);
    hlCallback(HL_SHAPE_CLOSEST_FEATURES, 
        (HLcallbackProc) HapticSphere::closestSurfaceFeatures, (void *) &sphere);
    hlEndShape();

    // If we are rendering the haptic rings, create a second shape
    // that is rendered along with the sphere.
    if (gUseRings)
    {
        // Setup materials.
        hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.0f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.0f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.0f);
     
        // HLAPI doesn't support push/pop attributes yet, so 
        // manually save and restore the touch model setting.
        HLenum nPrevTouchModel;
        hlGetIntegerv(HL_TOUCH_MODEL, (HLint *) &nPrevTouchModel);
        
        // Haptic rings are always rendered as constraints.
        hlTouchModel(HL_CONSTRAINT);
        hlTouchModelf(HL_SNAP_DISTANCE, gRingSnapDistance);

        hlBeginShape(HL_SHAPE_CALLBACK, ringsId);
        hlCallback(HL_SHAPE_CLOSEST_FEATURES, 
            (HLcallbackProc) HapticRings::closestFeatures, (void *) &rings);
        hlEndShape(); 
        
        // Restore the touch model state.
        hlTouchModel(nPrevTouchModel);
    }

    // End the frame.
    hlEndFrame();    
}


/******************************************************************************
 Toggles the touchable face.
******************************************************************************/
void toggleTouchableFace(void)
{
    HLenum nTouchableFace;
    hlGetIntegerv(HL_TOUCHABLE_FACE, (HLint *) &nTouchableFace);

    if (nTouchableFace == HL_FRONT)
    {
        nTouchableFace = HL_BACK;
    }
    else
    {
        nTouchableFace = HL_FRONT;
    }

    hlTouchableFace(nTouchableFace);
    printf("Touchable Face: %s\n", nTouchableFace);
}

/******************************************************************************
 Toggles between constraint and contact modes.
******************************************************************************/
void toggleTouchModel(void)
{
    HLenum nTouchModel;
    hlGetIntegerv(HL_TOUCH_MODEL, (HLint *) &nTouchModel);

    if (nTouchModel == HL_CONTACT)
    {
        nTouchModel = HL_CONSTRAINT;      
    }
    else
    {
        nTouchModel = HL_CONTACT;
    }        

    hlTouchModel(nTouchModel);
    printf("Touch Model: %s\n", nTouchModel);
}

/******************************************************************************
 Toggles whether the sphere rings are active.
******************************************************************************/
void toggleSphereRings(HapticSphere &sphere)
{
    gUseRings = !gUseRings;
    printf("Using sphere rings: %s\n", (gUseRings ? "yes" : "no"));
}

/******************************************************************************
 Prints help.
******************************************************************************/
void printHelp()
{
    static const char help[] = {"\
-----------------------------------------------\n\
      Custom Shape Example Menu Options\n\
===============================================\n\
[F] : Toggle touchable face (front / back )\n\
[T] : Toggle touch model (contact / constraint)\n\
[R] : Toggle sphere constraint rings\n\
[H] : Print help menu\n\
[Q] : Quit"};

    printf("%s\n\n", help);

}

/******************************************************************************
 Main function.
******************************************************************************/
int main(int argc, char* argv[])
{
    initHL();

    // Create a sphere.
    HapticSphere mySphere;
    mySphere.setRadius(40);

    // Create the rings.
    HapticRings myRings;
    myRings.setRadius(mySphere.getRadius());

    // Generate ids for the custom shapes.
    HLuint mySphereId = hlGenShapes(1);
    HLuint myRingsId = hlGenShapes(1);

    // Render the custom shape once to get started.
    renderCustomShapes(mySphere, mySphereId, myRings, myRingsId);
    
    printHelp();
    
    // Loop until the user quits or an error occurs.
    bool bDone = false;
    while (!bDone)
    {
        if (_kbhit())
        {
            int key = toupper(getch());
            
            switch (key)
            {
                case 'F':
                    toggleTouchableFace();
                    renderCustomShapes(mySphere, mySphereId,
                                       myRings, myRingsId);
                    break;

                case 'T':
                    toggleTouchModel();
                    renderCustomShapes(mySphere, mySphereId,
                                       myRings, myRingsId);
                    break;

                case 'R':
                    toggleSphereRings(mySphere);
                    renderCustomShapes(mySphere, mySphereId,
                                       myRings, myRingsId);
                    break;

                case 'Q':
                    bDone = true;
                    break;

                case 'H':
                default:
                    printHelp();
                    break;
            }
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
    
    // Release the shape ids.
    hlDeleteShapes(mySphereId, 1);
    hlDeleteShapes(myRingsId, 1);   

    // Cleanup.
    hlMakeCurrent(NULL);
    hlDeleteContext(ghHLRC);
    hdDisableDevice(ghHD);

    return 0;
}

/******************************************************************************/

