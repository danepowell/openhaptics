/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com

Module Name:

  HelloSphere.cpp

Description: 

This example demonstrates simple haptic rendering of a cloth like surface 
and the interaction of the pinch attachment with it. 
Cloth surface simulation inspired from Mass & Spring model simulation at 
http://freespace.virgin.net/hugo.elias/models/m_cloth.htm
******************************************************************************/

	
#include "Main.h"


const int kMaxStepSizeMs = 10;
double	mManualTimeStep = 0.01;
double mTimeStep = 0;	// size of last time step (in seconds)

//Cloth complexity.
const int gridSize=25;

//2 arrays of mass
int numMass;
Mass * mass1=NULL;
Mass * mass2=NULL;

//Pointers for current mass & next mass
Mass * currentMass=NULL;
Mass * nextMass=NULL;

//Gravity
hduVector3Df gravity(0.0f,-0.0f, 0.0f);

//Values given to each spring
float springConstant=15.0f;
float naturalLength=1.0f;

//Default mass
float mass_default=0.01f;

//Default damping to calculate velocity
float dampFactor=0.9f;

//Array of springs
int numSprings;
Spring * springs=NULL;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(800, 600);
    glutCreateWindow("Simple Cloth");

    // Set glut callback functions.
    glutDisplayFunc(glutDisplay);
    glutReshapeFunc(glutReshape);
    glutIdleFunc(glutIdle);
    
    glutCreateMenu(glutMenu);
    glutAddMenuEntry("Quit", 0);
    glutAttachMenu(GLUT_RIGHT_BUTTON);    
    
    // Provide a cleanup routine for handling application exit.
    atexit(exitHandler);

    initScene();


    glutMainLoop();

    return 0;
}
/*******************************************************************************
 GLUT callback for redrawing the view.
*******************************************************************************/
void glutDisplay()
{   
	drawSceneHaptics();

	drawSceneGraphics();

//	displayInfo();

	glutSwapBuffers();
}

/*******************************************************************************
 GLUT callback for reshaping the window.  This is the main place where the 
 viewing and workspace transforms get initialized.
*******************************************************************************/
void glutReshape(int width, int height)
{
    static const double kPI = 3.1415926535897932384626433832795;
    static const double kFovY = 45;

    double nearDist, farDist, aspect;

    glViewport(0, 0, width, height);

    // Compute the viewing parameters based on a fixed fov and viewing
    // a canonical box centered at the origin.

    nearDist = 1.0 / tan((kFovY / 2.0) * kPI / 180.0);
    farDist = nearDist * gridSize;
    aspect = (double) width / height;
   
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(kFovY, aspect, nearDist, farDist);
//	  gluPerspective(kFovY, aspect, 1, 100);

    // Place the camera down the Z axis looking at the origin.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();            
    gluLookAt(0, gridSize/3, nearDist + gridSize,
              0, 0, 0,
              0, 1, 0);
    
    updateWorkspace();
}

/*******************************************************************************
 GLUT callback for idle state.  Use this as an opportunity to request a redraw.
 Checks for HLAPI errors that have occurred since the last idle check.
*******************************************************************************/
void glutIdle()
{
  
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
//	double t =mManualTimeStep;
	double t =getTime();
	advanceTime(t);

	char title[150];
	sprintf(title, "Simple Cloth Simulation %4.1f fps ", determineFPS());
	glutSetWindowTitle(title);

    glutPostRedisplay();
}

/******************************************************************************
 Popup menu handler.
******************************************************************************/
void glutMenu(int ID)
{
    switch(ID) {
        case 0:
            exit(0);
            break;
	}
}

/*******************************************************************************
 Initializes the scene.  Handles initializing both OpenGL and HL.
*******************************************************************************/
void initScene()
{
    initGL();
	initHL();
	initCloth();
}

/*******************************************************************************
 Sets up general OpenGL rendering properties: lights, depth buffering, etc.
*******************************************************************************/
void initGL()
{
	glClearColor(0.6f, 0.7f, 1.0f, 1.0f);  // Clear the screen to the specified color.
   
 	   // Light properties for the diffuse light, specular light, and light position. //
	float diffuseLight[] = {0.8f, 0.8f, 0.8f, 1.0f};
	float specularLight[] = {1.0f, 1.0f, 1.0f, 1.0f};
//	float LightPosition[] = {1.0f, 1.0f, 490.0f, 1.0f};
	float LightPosition[] = {1.0f, 5.0f, -5.0f, 1.0f};

	glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    
    // Cull back faces.
  //  glCullFace(GL_BACK);
   // glEnable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    // Setup other misc features.
    glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

   // Set up the information for the first light and set it in GL_LIGHT0.
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
   glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
   glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
   glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 130.0f);
   glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 75.0f);
   // Enable the first light.
   glEnable(GL_LIGHT0);

   // Set up the material information for our objects.
   glEnable(GL_COLOR_MATERIAL);
   glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
   glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight);
   glMateriali(GL_FRONT, GL_SHININESS, 128);
   	//Use 2-sided lighting
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);
}

void initHL()
{
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        _getch();
        exit(-1);
    }

    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

    // Enable optimization of the viewing parameters when rendering
    // geometry for OpenHaptics.
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

    // Generate id for the shape.
    clothShapeId = hlGenShapes(1);

    hlTouchableFace(HL_FRONT_AND_BACK);

	hlAddEventCallback(HL_EVENT_TOUCH, clothShapeId, HL_COLLISION_THREAD, 
		hlTouchCB, 0);
	hlAddEventCallback(HL_EVENT_UNTOUCH, clothShapeId, HL_COLLISION_THREAD, 
		hlUnTouchCB, 0);

	hlAddEventCallback(HL_EVENT_MOTION, clothShapeId, HL_COLLISION_THREAD, 
		hlMotionCB, 0);

}

void exitHandler()
{
    // Deallocate the sphere shape id we reserved in initHL.
    hlDeleteShapes(clothShapeId, 1);

    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (ghHLRC != NULL)
    {
        hlDeleteContext(ghHLRC);
    }

    // Free up the haptic device.
    if (ghHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(ghHD);
    }
	
   
	if(mass1)
		delete [] mass1;

     mass1=NULL;

  	if(mass2)
		delete [] mass2;

      mass2=NULL;
	

	if(springs)
		delete [] springs;

     springs=NULL;
	

	
	
}

void updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

//	HLdouble minn[3]={-gridSize/4, -gridSize/(gridSize-1), -gridSize/2};
//	HLdouble maxx[3]={gridSize/4, gridSize/(gridSize-1), gridSize/2};

    hlMatrixMode(HL_TOUCHWORKSPACE);
    hlLoadIdentity();

//    hluFitWorkspaceBox(modelview, minn, maxx);
    // Fit haptic workspace to view volume.
    hluFitWorkspace(projection);

    // Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= CURSOR_SIZE_PIXELS;
}


void drawSceneGraphics()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        

	drawCursor();

	drawCloth();

//	drawMass();

//	drawSprings();

}


void drawSceneHaptics()
{    

    // Start haptic frame.  (Must do this before rendering any haptic shapes.)
    hlBeginFrame();
    // Set material properties for the shapes to be drawn.
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, springConstant/100.0);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, dampFactor);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.2f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.3f);

	hlHintb(HL_SHAPE_DYNAMIC_SURFACE_CHANGE, HL_TRUE);
    // Start a new haptic shape.  Use the feedback buffer to capture OpenGL 
    // geometry for haptic rendering.
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, clothShapeId);

	if (pinchValue > 0.8 && touchedCloth)
	{
		hlTouchModel(HL_CONSTRAINT);
		hlTouchModelf(HL_SNAP_DISTANCE, 50.0);
		glBegin(GL_POINTS);
		{
			glVertex3fv(currentMass[massTouched].position);
		}
		glEnd();
	}
	else
	{
		hlTouchModel(HL_CONTACT);
		drawCloth();
	}

    // End the shape.
    hlEndShape();

    // End the haptic frame.
    hlEndFrame();
}


void drawCursor()
{
    static const double kCursorRadius = 0.5;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;
    HLdouble proxyxform[16];

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!gCursorDisplayList)
    {
        gCursorDisplayList = glGenLists(1);
        glNewList(gCursorDisplayList, GL_COMPILE);
		glutSolidSphere(2.0, 25,25);

        glEndList();
    }
    
    // Get the proxy transform in world coordinates.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
    glMultMatrixd(proxyxform);

    // Apply the local cursor scale factor.
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

	drawPinch();

	glPopMatrix(); 
    glPopAttrib();
}

bool runCalbrationRoutine()
{
	HDErrorInfo error;
	int supportedCalibrationStyles;
    int calibrationStyle;

	printf("Calibration\n");
    printf("Found %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

    /* Choose a calibration style.  Some devices may support multiple types of 
       calibration.  In that case, prefer auto calibration over inkwell 
       calibration, and prefer inkwell calibration over reset encoders. */
    hdGetIntegerv(HD_CALIBRATION_STYLE, &supportedCalibrationStyles);
    if (supportedCalibrationStyles & HD_CALIBRATION_ENCODER_RESET)
    {
        calibrationStyle = HD_CALIBRATION_ENCODER_RESET;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_INKWELL)
    {
        calibrationStyle = HD_CALIBRATION_INKWELL;
    }
    if (supportedCalibrationStyles & HD_CALIBRATION_AUTO)
    {
        calibrationStyle = HD_CALIBRATION_AUTO;
    }

    /* Some haptic devices only support manual encoder calibration via a
       hardware reset. This requires that the endpoint be placed at a known
       physical location when the reset is commanded. For the PHANTOM haptic
       devices, this means positioning the device so that all links are
       orthogonal. Also, this reset is typically performed before the servoloop
       is running, and only technically needs to be performed once after each
       time the device is plugged in. */
    if (calibrationStyle == HD_CALIBRATION_ENCODER_RESET)
    {
        printf("Please prepare for manual calibration by\n");
        printf("placing the device at its reset position.\n\n");
        printf("Press any key to continue...\n");

        _getch();

        hdUpdateCalibration(calibrationStyle);
        if (hdCheckCalibration() == HD_CALIBRATION_OK)
        {
            printf("Calibration complete.\n\n");
			return 1;
        }
        if (HD_DEVICE_ERROR(error = hdGetError()))
        {
            hduPrintError(stderr, &error, "Reset encoders reset failed.");
            return 0;           
        }
    }

}
void initCloth()
{
	//total number of Mass
	numMass=gridSize*gridSize;
		
	//Total number of spings taking care of the edges. 
	numSprings=(gridSize-1)*gridSize*2;

	numSprings+=(gridSize-1)*(gridSize-1)*2;

	numSprings+=(gridSize-2)*gridSize*2;

	//Allocate memory for 
	mass1=new Mass[numMass+1];
	mass2=new Mass[numMass+1];
	springs=new Spring[numSprings+1];

	if(!mass1 || !mass2 || !springs)
	{
	printf("Unable to allocate space");
	exit(0);
	}

	if (resetSimulation)
	{
		
		setupCloth();
		resetSimulation = false;
	}
	else
	{
		setupCloth();
	}
}
void setupCloth()
{
	initAllMass();
	initAllSprings();
}
void initAllMass()
{
	//setup x-z plane
	for(int i=0; i<gridSize; ++i)
	{
		for(int j=0; j<gridSize; ++j)
		{
			mass1[i*gridSize+j].position.set((j)-(gridSize-1)/2,
										 0.0,
										 (i)-(gridSize-1)/2);
			mass1[i*gridSize+j].velocity.set(0.0,0.0,0.0);
			mass1[i*gridSize+j].mass=mass_default;
			mass1[i*gridSize+j].fixed=false;
		}
	}
	//Constrain mass in edges	
	for (int i =0;i <gridSize; i++)
	{
	mass1[i].fixed=true;
	mass1[gridSize*(gridSize-i)-1].fixed=true;
	mass1[gridSize*(gridSize-i)].fixed=true;
	mass1[gridSize*gridSize-i].fixed=true;
	}

	//Constrain corner mass

//	mass1[0].fixed=true;
//	mass1[gridSize-1].fixed=true;

//	mass1[gridSize*(gridSize-1)].fixed=true;
//	mass1[gridSize*gridSize-1].fixed=true;

	for(int i=0; i<numMass; ++i)
		mass2[i]=mass1[i];

	currentMass=mass1;
	nextMass=mass2;

}
void initAllSprings()
{
	//Initialise the springs
	Spring * currentSpring=&springs[0];

	// Now stitch all the mass together using springs

	
	for(int i=0; i<gridSize; ++i)
	{
		for(int j=0; j<gridSize-1; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=i*gridSize+j+1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength;
			
			++currentSpring;
		}
	}
	
	for(int i=0; i<gridSize-1; ++i)
	{
		for(int j=0; j<gridSize; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=(i+1)*gridSize+j;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength;
			
			++currentSpring;
		}
	}
	
	for(int i=0; i<gridSize-1; ++i)
	{
		for(int j=0; j<gridSize-1; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=(i+1)*gridSize+j+1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*sqrt(2.0f);
			
			++currentSpring;
		}
	}
	
	for(int i=0; i<gridSize-1; ++i)
	{
		for(int j=1; j<gridSize; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=(i+1)*gridSize+j-1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*sqrt(2.0f);
			
			++currentSpring;
		}
	}

	for(int i=0; i<gridSize; ++i)
	{
		for(int j=0; j<gridSize-2; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=i*gridSize+j+2;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*2;
			
			++currentSpring;
		}
	}

	for(int i=0; i<gridSize-2; ++i)
	{
		for(int j=0; j<gridSize; ++j)
		{
			currentSpring->imass1=i*gridSize+j;
			currentSpring->imass2=(i+2)*gridSize+j;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*2;
			
			++currentSpring;
		}
	}
}
void updatePhysics(double tPrev, double tCurr)
{
	//set currentTime and timePassed
	static double lastTime=tPrev;
	double currentTime=tCurr;

	double timePassed=currentTime-lastTime;

	//Update the physics 

	//Calculate the tensions in the springs
	for(int i=0; i<numSprings; ++i)
	{
		hduVector3Dd tempLength = (currentMass[springs[i].imass1].position-
			currentMass[springs[i].imass2].position);

		float springLength=tempLength.magnitude();

		float extension=springLength-springs[i].naturalLength;

		springs[i].tension=springs[i].springConstant*extension/springs[i].naturalLength;
	}


	for(int i=0; i<numMass; ++i)
	{
		//Transfer properties which do not change
		nextMass[i].fixed=currentMass[i].fixed;
		nextMass[i].mass=currentMass[i].mass;

		//If the mass is fixed, transfer the position and zero the velocity, otherwise calculate
		//the new values
		if(currentMass[i].fixed)
		{
			nextMass[i].position=nextMass[i].position;
			nextMass[i].velocity.set(0.0,0.0,0.0);
		}
		else
		{
			hduVector3Df currentForce;
			hdGetFloatv(HD_CURRENT_FORCE, currentForce);
			//Calculate the force on this mass
			if (touchedCloth || movingOnCloth)
			{
				if (i == massTouched || i == massTouched-1 || i == massTouched+1 
					|| i ==(massTouched-gridSize) || i ==(massTouched-gridSize)+1 ||i ==(massTouched-gridSize)-1
					|| i ==(massTouched+gridSize) || i ==(massTouched+gridSize)+1 || i ==(massTouched+gridSize)-1)
					force = -currentForce + gravity ;
				else
					force = gravity;
			}
			else
				force = gravity;

			//Loop through springs
			for(int j=0; j<numSprings; ++j)
			{
				//If this mass is "imass1" for this spring, add the tension to the force
				if(springs[j].imass1==i)
				{
					hduVector3Df tensionDirection=	currentMass[springs[j].imass2].position-
						currentMass[i].position;
					tensionDirection.normalize();

					force+=springs[j].tension*tensionDirection;
				}

				//Similarly if the mass is "imass2"
				if(springs[j].imass2==i)
				{
					hduVector3Df tensionDirection=	currentMass[springs[j].imass1].position-
						currentMass[i].position;
					tensionDirection.normalize();

					force+=springs[j].tension*tensionDirection;
				}
			}


			//Calculate the acceleration
			hduVector3Df acceleration=force/currentMass[i].mass;

			//Update velocity
			nextMass[i].velocity=currentMass[i].velocity+acceleration*
				(float)timePassed;

			//Damp the velocity
			nextMass[i].velocity*=dampFactor;

			//Calculate new position
			nextMass[i].position=currentMass[i].position+
				(nextMass[i].velocity+currentMass[i].velocity)*(float)timePassed/2;

		}
	}

	//Swap the currentMass and newMass pointers
	Mass * temp=currentMass;
	currentMass=nextMass;
	nextMass=currentMass;

	//Update the normals of the surface
	updateNormals();

}
void updateNormals()
{
			//Zero the normals on each mass
		for(int i=0; i<numMass; ++i)
			currentMass[i].normal.set(0.0,0.0,0.0);

		//Calculate the normals on the current mass
		for(int i=0; i<gridSize-1; ++i)
		{
			for(int j=0; j<gridSize-1; ++j)
			{
				hduVector3Df & p0=currentMass[i*gridSize+j].position;
				hduVector3Df & p1=currentMass[i*gridSize+j+1].position;
				hduVector3Df & p2=currentMass[(i+1)*gridSize+j].position;
				hduVector3Df & p3=currentMass[(i+1)*gridSize+j+1].position;

				hduVector3Df & n0=currentMass[i*gridSize+j].normal;
				hduVector3Df & n1=currentMass[i*gridSize+j+1].normal;
				hduVector3Df & n2=currentMass[(i+1)*gridSize+j].normal;
				hduVector3Df & n3=currentMass[(i+1)*gridSize+j+1].normal;

				//Calculate the normals for the 2 triangles and add on
				hduVector3Df tpt1, tpt2;
				tpt1 = p1-p0;
				tpt2 = p2-p0;
				hduVector3Df normal= tpt1.crossProduct(tpt2);
	
				n0+=normal;
				n1+=normal;
				n2+=normal;
				
				hduVector3Df tpt3, tpt4;
				tpt3 = p1-p2;
				tpt4 = p3-p2;
				normal= tpt3.crossProduct(tpt4);
	
				n1+=normal;
				n2+=normal;
				n3+=normal;
			}
		}

		//Normalize normals
		for(int i=0; i<numMass; ++i)
			currentMass[i].normal.normalize();
}
void drawCloth()
{
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	{
		for(int i=0; i<gridSize-1; ++i)
		{
			for(int j=0; j<gridSize-1; ++j)
			{
				glNormal3fv(currentMass[i*gridSize+j].normal);
				glVertex3fv(currentMass[i*gridSize+j].position);
				glNormal3fv(currentMass[i*gridSize+j+1].normal);
				glVertex3fv(currentMass[i*gridSize+j+1].position);
				glNormal3fv(currentMass[(i+1)*gridSize+j].normal);
				glVertex3fv(currentMass[(i+1)*gridSize+j].position);

				glNormal3fv(currentMass[(i+1)*gridSize+j].normal);
				glVertex3fv(currentMass[(i+1)*gridSize+j].position);
				glNormal3fv(currentMass[i*gridSize+j+1].normal);
				glVertex3fv(currentMass[i*gridSize+j+1].position);
				glNormal3fv(currentMass[(i+1)*gridSize+j+1].normal);
				glVertex3fv(currentMass[(i+1)*gridSize+j+1].position);
			}
		}
	}
	glEnd();
	glPopMatrix();

}
void drawMass()
{
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glPointSize(10.0f);
	glBegin(GL_POINTS);
	{
		glColor3f(1.0,1.0,0.0);
		for(int i=0; i<numMass; ++i)
			glVertex3fv(currentMass[i].position);
	}
	glEnd();

//	glPopMatrix();
	glPopAttrib();   
}
void drawSprings()
{
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glBegin(GL_LINES);
	{
		glColor3f(1.0,0.0,1.0);
		for(int i=0; i<numSprings; ++i)
		{
			//Check the spring has been initialised and the ball numbers are in bounds
			if(	springs[i].imass1!=-1 && springs[i].imass2!=-1	&&
				springs[i].imass1<numMass && springs[i].imass2<numMass)
			{
				glVertex3fv(currentMass[springs[i].imass1].position);
				glVertex3fv(currentMass[springs[i].imass2].position);
			}
		}
	}
	glEnd();
	
//	glPopMatrix();
	glPopAttrib();  
}
void drawPinch()
{
	//Get current pinch value
	hdGetDoublev(HD_CURRENT_PINCH_VALUE, &pinchValue); 

	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	glTranslatef(0.0,0.0,80.0);
	glBegin(GL_LINES);			// Draw Line for stationary pinch handle
	glColor3f(1.0,1.0,0.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,-80.0);
	glEnd();
	//Calculate pinch angle (Pinch total range:  30 degrees)
	glRotatef(30.0-(30.0*pinchValue),1.0,0.0,0.0);
	glBegin(GL_LINES);			// Draw Line for Moving pinch handle
	glColor3f(1.0,0.0,1.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,-80.0);
	glEnd();
	glPopAttrib(); 
}
double determineFPS(void)
{
	static unsigned int	lastMs = glutGet(GLUT_ELAPSED_TIME);
	unsigned int		currentMs = glutGet(GLUT_ELAPSED_TIME);
	unsigned int		framesMs = currentMs - lastMs;
	static unsigned int	framesCounted = 0;
	static double fps = 0;
	
	framesCounted++;

	#define DURATION_TO_TIME		1000	// duration in miliseconds for calculating fps

	if(framesMs >= DURATION_TO_TIME)
	{
		fps = (double)1000 * framesCounted / framesMs;
		//printf("last: %ld  curr: %ld\n", g.lastClock, currentClock);
		//printf("clocks: %d  cps: %d  FPS: %f\n", clock(), CLOCKS_PER_SEC, fps);
		
		framesCounted = 0;
		//g.framesTime = 0;

		lastMs = currentMs;
	}

	return fps;
}
void advanceTime(double dt)
{
	static double prevTime = 0;
	double currTime = prevTime + dt;

	updatePhysics(prevTime, currTime);

	mTimeStep = dt;
}
double getTime(void)
{
	unsigned int	timeMs;
	unsigned int	timeSinceLastMs;
	static unsigned int timeOldMs = 0;
	
	timeMs = glutGet(GLUT_ELAPSED_TIME);
	timeSinceLastMs = timeMs - timeOldMs;
	timeOldMs = timeMs;
	
	if (timeSinceLastMs > kMaxStepSizeMs)
	{
		timeSinceLastMs = kMaxStepSizeMs;
	}

	return (double)timeSinceLastMs/ 1000;
}

void drawBitmapString(GLfloat x, GLfloat y, void *font, char *format,...)
{
	int len, i;
	va_list args;
	char string[256];

	// special C stuff to interpret a dynamic set of arguments specified by "..."
	va_start(args, format);
	vsprintf(string, format, args);
	va_end(args);

	glRasterPos2f(x, y);
	len = (int) strlen(string);
	
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}

void displayInfo()
{
	glPushMatrix();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

	// switch to 2d orthographic mMode for drawing text
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, winW, winH, 0);
	glMatrixMode(GL_MODELVIEW);
	
	glColor3f(1.0, 1.0, 1.0);

	int textRowDown = 0; // lines of text already drawn downwards from the top
	int textRowUp = 0; // lines of text already drawn upwards from the bottom

	drawBitmapString(5 + 12 * 9, winH - 10 - (textRowUp++) * 15, GLUT_BITMAP_9_BY_15, "Gravity:%5.1f", gravity[1]);

	drawBitmapString(5, winH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "Spring Constant: %4.1f", springConstant);
	drawBitmapString(5 + 12 * 9, winH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "Damping Co-efficient: %5.1f",dampFactor );
	drawBitmapString(5 + 30 * 9, winH - 10 - (textRowUp) * 15, GLUT_BITMAP_9_BY_15, "Mass:   %5.3f", mass_default );
glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
}
int getClosestMass(hduVector3Dd& pos)
{
	int closestPart = -1;
	double closestDist, currDist; 

	for(int i=0; i<numMass; ++i)
	{
		currDist = pos.distanceSqr(currentMass[i].position);  
		if (closestPart == -1 || (currDist < closestDist))
		{
			closestDist = currDist;
			closestPart = i;
		}
	}
	return closestPart;
}
void HLCALLBACK hlTouchCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata)
{
	touchedCloth = true;
	hlGetDoublev(HL_PROXY_POSITION, proxyPosition);
	massTouched = getClosestMass(proxyPosition);
}
void HLCALLBACK hlUnTouchCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata)
{
	touchedCloth = false;
	movingOnCloth = false;
}

void HLCALLBACK hlMotionCB(HLenum event, HLuint object,
						   HLenum thread, HLcache *cache,
						   void *userdata)
{
	movingOnCloth = true;
	hlGetDoublev(HL_PROXY_POSITION, proxyPosition);
	massTouched = getClosestMass(proxyPosition);
}
