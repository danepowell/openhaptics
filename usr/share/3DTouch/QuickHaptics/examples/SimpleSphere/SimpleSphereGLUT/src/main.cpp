/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - SimpleSphere example
//SensAble Technologies, Woburn, MA
//September 03, 2008
//Programmer: Hari Vasudevan
//////////////////////////////////////////////////////////////////////////////




#include <QHHeadersGLUT.h>//Include all necessary headers


int main(int argc, char *argv[])
{
	
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window
 
    DeviceSpace* Omni = new DeviceSpace;//Find the default Phantom Device
    DisplayObject->tell(Omni);//Tell Quickhaptics that omni exists
    
    Sphere* SimpleSphere = new Sphere;//Get a sphere
    DisplayObject->tell(SimpleSphere);//Tell Quick Haptics that the sphere exists

    Cursor* OmniCursor = new Cursor;//Create a cursor
    DisplayObject->tell(OmniCursor);//Tell QuickHaptics that a cursor exists
    
  
    qhStart();//Set everything in motion
    return 0;
}


