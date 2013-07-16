/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - ComplexScene example
//SensAble Technologies, Woburn, MA
//September 03, 2008
//Programmer: Hari Vasudevan
//////////////////////////////////////////////////////////////////////////////




#include <QHHeadersGLUT.h>//Include all necessary headers



int main(int argc, char *argv[])
{
	
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window

    DeviceSpace* Omni = new DeviceSpace;//Fina Phantom Device called Omni
    DisplayObject->tell(Omni);//tell Quick haptics that Omni exists
    
    Sphere* SimpleSphere = new Sphere();//get a sphere
    SimpleSphere->setSpin();//make it spin
	SimpleSphere->setFriction();
    SimpleSphere->setTexture("Models/earth.jpg");//load an Earth texture ont he Sphere
    DisplayObject->tell(SimpleSphere);//Tell Quickhaptics that Sphere exists
    

    Box* SimpleBox = new Box();//Get a box
    SimpleBox->setTranslation(0.0,0.8,0.0);//Position it in the workspace
    SimpleBox->setOrbit(HL_CW,0.5,hduVector3Dd(1.0,0.0,0.0),hduVector3Dd(0.0,0.0,0.0));//Make it orbit counter clockwise, with half velocity, axis of revolution is parallel to the x axis, the center of rotation is the origin
    SimpleBox->setSpin(HL_CCW,1.0,hduVector3Dd(0.0,0.0,1.0));//Make it spin counter clockwise with half velocity around the z axis
    SimpleBox->setTexture("Models/box.jpg");//Set a box texture on it
    DisplayObject->tell(SimpleBox);//Tell QuickHaptics that the Box exists

    Cone* SimpleCone = new Cone();//get a cone
    SimpleCone->setTranslation(1.5,0.0,0.0);//Set the position of the Cone
    SimpleCone->setTexture("Models/brick.jpg");//Set a texture on it
    SimpleCone->setSpin(HL_CCW,0.5,hduVector3Dd(0.0,1.0,0.0));//Make it spin
	SimpleCone->setFriction();//Give the cone some friction
    DisplayObject->tell(SimpleCone);//Tell Quick haptics about the cone

    Cylinder* SimpleCylinder = new Cylinder();//get a cylinder
    SimpleCylinder->setTranslation(-1.5,0.0,0.0);//Set it's position
    SimpleCylinder->setTexture("Models/Rusty.jpg");//Load a texture
    SimpleCylinder->setSpin(HL_CW,0.5,hduVector3Dd(0.0,1.0,0.0));//Make it spin
    SimpleCylinder->setMagnetic(true);//Make the Cylinder magnetic so the cursor sticks to it
    DisplayObject->tell(SimpleCylinder);//tell QuickHaptics that Cylinder exists

    Text* SimpleText = new Text(50,"Hello Haptics", "Models/verdana.ttf");//Get some text rendered "hello haptics"
    SimpleText->setTranslation(0.35,0.8,0.0);//Set it's position
    SimpleText->setShapeColor(0.35,0.15,0.7);
    DisplayObject->tell(SimpleText);//Tell QuickHaptics that the text exists


    Cursor* OmniCursor = new Cursor;//Declare a cursor
    DisplayObject->tell(OmniCursor);//Tell QuickHaptics that the cursor exists
    
  
    qhStart();//Set everything in motion

    return 0;
}


