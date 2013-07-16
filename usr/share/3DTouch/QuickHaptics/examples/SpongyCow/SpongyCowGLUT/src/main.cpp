/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - Spongy Cow
//SensAble Technologies, Woburn, MA
//October 15, 2008
//Programmer: Hari Vasudevan
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//This demo is Experimental!!!!
//////////////////////////////////////////////////////////////////////////////



#include <QHHeadersGLUT.h>//Include all necessary headers

void MotionCallback(unsigned int ShapeID);
void GraphicsCallback(void);


int main(int argc, char *argv[])
{
	
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window
    DeviceSpace* OmniSpace = new DeviceSpace;//Find the default Phantom device 
    DisplayObject->tell(OmniSpace);//Tell QuickHaptics about it

    TriMesh* Cow = new TriMesh("Models/cow.3DS");//Load a cow model
    Cow->setName("Cow");//give it a name
    Cow->setTexture("Models/cow.jpg");
	
	Cow->dynamic(true);//make the cow deformable
	Cow->setGravity(false);//Turn off gravity
	Cow->setSpringStiffness(0.2);//Parameters to play around with - Spring Stiffness
	Cow->setSpringDamping(0.05);//Parameters to play around with - Damping
	Cow->setMass(0.01);////Parameters to play around with - Mass of each particle

    
    DisplayObject->tell(Cow);//Tell Quickhaptics about the cow

    Text* descriptionText1 = new Text(20.0, "This example shows how to build a simple Deformable 3D model", 0.2,0.9);
    descriptionText1->setShapeColor(0.0,0.35,0.7);
    DisplayObject->tell(descriptionText1);
    Text* descriptionText2 = new Text(20.0, "This feature is experimental!!", 0.4,0.85);
    descriptionText2->setShapeColor(0.0,0.7,0.35);
    DisplayObject->tell(descriptionText2);

    Cursor* OmniCursor = new Cursor;//Declare a new cursor
    OmniCursor->setName("OmniCursor");
    DisplayObject->tell(OmniCursor);//tell QuickHaptics about the cursor

    DisplayObject->preDrawCallback(GraphicsCallback);//set the graphics callback
    OmniSpace->motionCallback(MotionCallback, Cow);//set the movement callback

    
    
    qhStart();//Set everything in motion
    return 0;
}

void GraphicsCallback(void)
{
    TriMesh* CowPointer = TriMesh::searchTriMesh("Cow");//Find the cube and return the pointer to it.
	CowPointer->deformationFunction();//Calculate the deformation of the mesh
}

void MotionCallback(unsigned int ShapeID)
{
    TriMesh* CowPointer = TriMesh::searchTriMesh("Cow");//Find the cube and return a pointer to it.
	Cursor* OmniCursorPointer = Cursor::searchCursor("OmniCursor");//Search for the cursor and return a pointer to it.
	DeviceSpace* SpacePointer = DeviceSpace::searchSpace("Default PHANToM");//Search for the haptic device and return a pointer to it.

	hduVector3Dd CPosition;
	CPosition = OmniCursorPointer->getPosition();//Get the current position of the haptic device
	
	hduVector3Dd Force = SpacePointer->getForce();//Get the current force being exerted by the haptic device.
	CowPointer->deformationFunction(CPosition,Force);//calculate the deformation because of the force at the cursor position
}



