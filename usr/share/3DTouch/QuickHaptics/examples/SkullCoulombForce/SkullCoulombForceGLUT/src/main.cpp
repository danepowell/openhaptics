/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - Coulomb Force Demo
//SensAble Technologies, Woburn, MA
//Movember 11, 2008
//Programmers: Hari Vasudevan & Venkatraghavan Gourishankar
//
// This demo uses QuickHaptics to set up a HD servoloop callback. The skull
// location defines a Coulomb Force for the Phantom.

// Note that when the Proxy Position is obtained within the servo loop,
// all coordinates are in the "device space" of the Phantom. Thus,
// matrix transformations must be performed from Device Space to World Space
// and vice versa.
//////////////////////////////////////////////////////////////////////////////

#include <QHHeadersGLUT.h>//Include all necessary headers
#include <HDU/hduMath.h>
#include <HDU/hduMatrix.h>

class DataTransportClass//This class carried data into the ServoLoop thread
{
public:
	TriMesh* Model;//Trimesh pointer to hold mesh data
	Sphere* cursorSphere;//Sphere pointer. for the Sphere which replaces the cursor in this demo
	Cylinder* forceArrow;//The bar on the Sphere showing the magnitude and direction of force generated
    Cone* forceArrowTip;// The bar tip that points in the force direction.
	Cursor* deviceCursor;//Pointer to hold the cursor data
    Text* descriptionText;
};

double chargeRadius = 3;//This variable defines the radius around the charge when the inverse square law changes to a spring force law.
hduMatrix WorldToDevice;//This matrix contains the World Space to DeviceSpace Transformation
hduVector3Dd forceVec;//This variable contains the force vector.

void GraphicsCallback(void);//Graphics callback routine
void HLCALLBACK computeForceCB(HDdouble force[3], HLcache *cache, void *userdata);//Servo loop callback
void HLCALLBACK startEffectCB(HLcache *cache, void *userdata);//Servo Loop callback
void HLCALLBACK stopEffectCB(HLcache *cache, void *userdata);//Servo Loop callback
hduVector3Dd forceField(hduVector3Dd Pos1, hduVector3Dd Pos2, HDdouble Multiplier, HLdouble Radius);//This function computer the force beween the Model and the particle based on the positions


int main(int argc, char *argv[])
{
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window
	DataTransportClass dataObject;//Initialize an Object to transport data into the servoloop callback
 
    DeviceSpace* OmniSpace = new DeviceSpace;//Find the default Phantom Device
    DisplayObject->setName("Coulomb Field Demo");//Give the window a title
    DisplayObject->tell(OmniSpace);//Tell quickHaptics about the device space object    

    dataObject.cursorSphere = new Sphere(chargeRadius,15);//Initialise a Sphere
	dataObject.cursorSphere->setName("cursorSphere");//Give it a name
    dataObject.cursorSphere->setShapeColor(0.8,0.2,0.2);//Give it a color
    dataObject.cursorSphere->setHapticVisibility(false);//Make the Sphere haptically invisible. this sphere replaces the cursor hence it must be haptically invisible or the proxy will keep colliding with the sphere
    DisplayObject->tell(dataObject.cursorSphere);//Tell QuickHaptics
    
    dataObject.forceArrow = new Cylinder(chargeRadius/4,1,15);//Initialise a cylinder
    dataObject.forceArrow->setShapeColor(0.2,0.7,0.2);//Give it a color
    dataObject.forceArrow->setHapticVisibility(false);//Make it haptictically invisible
    dataObject.forceArrow->setName("forceArrow");//Give it a name
    DisplayObject->tell(dataObject.forceArrow);//tell Quickhaptics
        
    dataObject.forceArrowTip = new Cone(2,4,15);//Initialise a cone
    dataObject.forceArrowTip->setShapeColor(1.0,0.0,0.0);//Give it a color
    dataObject.forceArrowTip->setHapticVisibility(false);//Make it haptictically invisible
    dataObject.forceArrowTip->setName("forceArrowTip");//Give it a name
    DisplayObject->tell(dataObject.forceArrowTip);//tell Quickhaptics

    dataObject.Model = new TriMesh("Models/skull.obj");//Load a Skull  Model for the Mesh
    dataObject.Model->setName("Skull");//Give it a name
    dataObject.Model->setHapticVisibility(false);//Make it haptically invisible
	dataObject.Model->setShapeColor(0.35,0.15,0.75);//Make to color of the skull purple
	dataObject.Model->setScale( .2 ); // make the skull smaller, about the same size as the sphere
    DisplayObject->tell(dataObject.Model);//Tell QuickHaptics about it.
    
    dataObject.deviceCursor= new Cursor();//Get a new cursor
    dataObject.deviceCursor->setName("devCursor");//Give it a name
    dataObject.deviceCursor->setCursorGraphicallyVisible(false);//Make it graphically invisible
    DisplayObject->tell(dataObject.deviceCursor);//Tell Quickhaptics about it.

    dataObject.descriptionText = new Text(20.0,"This example demonstrates Coulomb Forces between two dynamic charges",
        0.1,0.9);
    dataObject.descriptionText->setShapeColor(0.7,0.0,0.4);
    DisplayObject->tell(dataObject.descriptionText);

    DisplayObject->preDrawCallback(GraphicsCallback);//Register the graphics callback
    OmniSpace->startServoLoopCallback(startEffectCB, computeForceCB, stopEffectCB,&dataObject);//Register the servoloop callback


	//
	// Change the default camera, first set the Default Camera, 
	// then read back the fov, eye point etc.
	//
	DisplayObject->setDefaultCamera();
	
	float fov, nearplane, farplane;
	hduVector3Dd eyepoint, lookat, up;
	DisplayObject->getCamera( &fov, &nearplane, &farplane, &eyepoint, &lookat, &up );

	eyepoint[2] += 100.;	// pull back by 100
	nearplane += 80.;		// recenter the haptic workspace (adjust by 20) 
	farplane += 80.;
	DisplayObject->setCamera( fov+15., nearplane, farplane,eyepoint, lookat, up );


	//Set everything in motion
    qhStart();
	return 0;
}


//
// The Graphics Callback runs in the application "client thread" (qhStart) and sets the transformations
// for the Red Sphere and Green Line of the Cursor. Also, this callback sets the WorldToDevice matrix
// for use in the ServoLoopCallback.
//
void GraphicsCallback(void)
{
    QHGLUT* localDisplayObject = QHGLUT::searchWindow("Coulomb Field Demo");//Get a Pointer to the display object
    Cursor* localDeviceCursor = Cursor::searchCursor("devCursor");//Get a pointer to the cursor
    Cylinder* localForceArrow = Cylinder::searchCylinder("forceArrow");//get a pointer to the cylinder
    Cone* localForceArrowTip = Cone::searchCone("forceArrowTip");//get a pointer to the cylinder
	Sphere* localCursorSphere = Sphere::searchSphere("cursorSphere");//get a pointer top the Sphere

	if( localDisplayObject == NULL || localDeviceCursor == NULL || localForceArrow == NULL || localCursorSphere == NULL)
		return;

	hduMatrix CylinderTransform;//Transformation for the Cylinder. This transform makes it point toward the Model
	hduVector3Dd localCursorPosition;
	hduVector3Dd DirectionVecX;
	hduVector3Dd PointOnPlane;
	hduVector3Dd DirectionVecY;
	hduVector3Dd DirectionVecZ;

	//Compute the world to device transform
    WorldToDevice = localDisplayObject->getWorldToDeviceTransform();

	// Set transform for Red Sphere
    localCursorPosition = localDeviceCursor->getPosition();//Get the local cursor position in World Space
	
	hduVector3Dd localCursorSpherePos = localCursorSphere->getTranslation();
	localCursorSphere->setTranslation(-localCursorSpherePos);
	localCursorSphere->setTranslation(localCursorPosition);//Set the position of the Sphere the same as the cursor
    
	////////////////////////////////////////////////////////////////////////////////////////////
	//Code to calculate the transform of the green cylinder to point along the force direction
	////////////////////////////////////////////////////////////////////////////////////////////
	hduMatrix DeviceToWorld = WorldToDevice.getInverse();
	HDdouble ForceMagnitude = forceVec.magnitude();
	DeviceToWorld[3][0] = 0.0;			   
	DeviceToWorld[3][1] = 0.0;			   
	DeviceToWorld[3][2] = 0.0;
	DirectionVecX = forceVec * DeviceToWorld;
    DirectionVecX.normalize();
    PointOnPlane.set(0.0,0.0,(DirectionVecX[0]*localCursorPosition[0] + DirectionVecX[1]*localCursorPosition[1] + DirectionVecX[2]*localCursorPosition[2])/DirectionVecX[2]);
    DirectionVecY = PointOnPlane  - localCursorPosition;
    DirectionVecY.normalize();

    DirectionVecZ = -DirectionVecY.crossProduct(DirectionVecX);

    CylinderTransform[0][0] = DirectionVecZ[0]; CylinderTransform[0][1] = DirectionVecZ[1]; CylinderTransform[0][2] = DirectionVecZ[2]; CylinderTransform[0][3] = 0.0;
    CylinderTransform[1][0] = DirectionVecX[0]; CylinderTransform[1][1] = DirectionVecX[1]; CylinderTransform[1][2] = DirectionVecX[2]; CylinderTransform[1][3] = 0.0;
    CylinderTransform[2][0] = DirectionVecY[0]; CylinderTransform[2][1] = DirectionVecY[1]; CylinderTransform[2][2] = DirectionVecY[2]; CylinderTransform[2][3] = 0.0;
    CylinderTransform[3][0] = 0.0             ; CylinderTransform[3][1] = 0.0             ; CylinderTransform[3][2] = 0.0             ; CylinderTransform[3][3] = 1.0;
    CylinderTransform = CylinderTransform * hduMatrix::createTranslation(localCursorPosition[0], localCursorPosition[1], localCursorPosition[2]);
    
    localForceArrow->update(chargeRadius/4, ForceMagnitude*50, 15);
    localForceArrow->setTranslation(localCursorPosition);
    localForceArrow->setTransform(CylinderTransform);

     hduMatrix ConeTransform = CylinderTransform * hduMatrix::createTranslation(DirectionVecX[0]
     * ForceMagnitude*50,DirectionVecX[1] * ForceMagnitude*50,DirectionVecX[2] * ForceMagnitude*50 );

    localForceArrowTip->setTransform(ConeTransform);
	/////////////////////////////////////////////
}


/***************************************************************************************
 Servo loop thread callback.  Computes a force effect. This callback defines the motion
 of the purple skull and calculates the force based on the "real-time" Proxy position
 in Device space.
****************************************************************************************/
void HLCALLBACK computeForceCB(HDdouble force[3], HLcache *cache, void *userdata)
{
    DataTransportClass *localdataObject = (DataTransportClass *) userdata;//Typecast the pointer passed in appropriately
    hduVector3Dd skullPositionDS;//Position of the skull (Moving sphere) in Device Space.
	hduVector3Dd proxyPosition;//Position of the proxy in device space
	HDdouble instRate = 0.0;
	HDdouble deltaT = 0.0;
	static float counter = 0.0;
	float degInRad = 0.0;
	static int counter1 = 0;

    // Get the time delta since the last update.
    hdGetDoublev(HD_INSTANTANEOUS_UPDATE_RATE, &instRate);
    deltaT = 1.0 / instRate;
    counter+=deltaT;
    degInRad = counter*20*3.14159/180;
    
	hduVector3Dd ModelPos = localdataObject->Model->getTranslation();
	localdataObject->Model->setTranslation(-ModelPos);
	localdataObject->Model->setTranslation(cos(degInRad)*64.0, sin(degInRad)*64.0,5.0);//Move the skull aroubnd in a circle

    WorldToDevice.multVecMatrix(localdataObject->Model->getTranslation(),skullPositionDS);//Convert the position of the sphere from world space to device space
 
    hlCacheGetDoublev(cache, HL_PROXY_POSITION, proxyPosition);//Get the position of the proxy in Device Coordinates (All HL commands in the servo loop callback fetch values in device coordinates)
    forceVec = forceField(proxyPosition, skullPositionDS, 40.0, 5.0);//Calculate the force

    counter1++;
    if(counter1>2000)//Make the force start after 2 seconds of program start. This is because the servo loop thread executes before the graphics thread. 
		//Hence global variables set in the graphics thread will not be valid for sometime in the begining og the program
    {
        force[0] = forceVec[0];
        force[1] = forceVec[1];
        force[2] = forceVec[2];
        counter1 = 2001;
    }
    else
    {
        force[0] = 0.0;
        force[1] = 0.0;
        force[2] = 0.0;
    }
}


/******************************************************************************
 Servo loop thread callback called when the effect is started.
******************************************************************************/
void HLCALLBACK startEffectCB(HLcache *cache, void *userdata)
{
    DataTransportClass *localdataObject = (DataTransportClass *) userdata;
    printf("Custom effect started\n");
}


/******************************************************************************
 Servo loop thread callback called when the effect is stopped.
******************************************************************************/
void HLCALLBACK stopEffectCB(HLcache *cache, void *userdata)
{
    printf("Custom effect stopped\n");
}


/*******************************************************************************
 Given the position of the two charges in space,
 calculates the (modified) coulomb force.
*******************************************************************************/
hduVector3Dd forceField(hduVector3Dd Pos1, hduVector3Dd Pos2, HDdouble Multiplier, HLdouble Radius)
{
    hduVector3Dd diffVec = Pos2 - Pos1 ;//Find the difference in position
    double dist = 0.0;
    hduVector3Dd forceVec(0,0,0);
	
    HDdouble nominalMaxContinuousForce;
    hdGetDoublev(HD_NOMINAL_MAX_CONTINUOUS_FORCE, &nominalMaxContinuousForce);//Find the max continuous for that the device is capable of

    dist = diffVec.magnitude();

	if(dist < Radius*2.0) //Spring force (when the model and cursor are within a 'sphere of influence'
    {
        diffVec.normalize();
        forceVec =  (Multiplier) * diffVec * dist /(4.0 * Radius * Radius);
        static int i=0;
    }
    else //Inverse square attraction
    {
        forceVec = Multiplier * diffVec/(dist*dist);
    }

    for(int i=0;i<3;i++)//Limit force calculated to Max continuouis. This is very important because force values exceeding this value can damage the device motors.
    {
        if(forceVec[i]>nominalMaxContinuousForce)
            forceVec[i] = nominalMaxContinuousForce;

        if(forceVec[i]<-nominalMaxContinuousForce)
            forceVec[i] = -nominalMaxContinuousForce;
    }

	return forceVec;
}
