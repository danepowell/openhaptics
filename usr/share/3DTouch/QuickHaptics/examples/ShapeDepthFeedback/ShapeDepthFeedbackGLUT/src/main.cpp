/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - Depth and Feedback Buffer Example
//SensAble Technologies, Woburn, MA
//September 03, 2008
//Programmer: Hari Vasudevan
//////////////////////////////////////////////////////////////////////////////



#include <QHHeadersGLUT.h>//Include all necessary headers


void glutMenuFunction(int MenuID);



int main(int argc, char *argv[])
{
	
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window
    	DeviceSpace* OmniSpace = new DeviceSpace;//get the default device
	DisplayObject->tell(OmniSpace);//Tell QuickHaptics about it

	TriMesh* Bunny = new TriMesh("Models/BunnyRep.ply");//Load the Bunny Models
	Bunny->setName("Bunny");//give it a name
	Bunny->setTranslation(0.25,-1.0,0.0);//Position the model
	Bunny->setScale(10.0);//make the model 2 times as large
	Bunny->setStiffness(0.5);
	Bunny->setDamping(0.3);
	Bunny->setFriction(0.3, 0.5);//Give the Bunny some friction on the surface
	Bunny->setShapeColor(205.0/255.0,133.0/255.0,63.0/255.0);//Set a brown color for the bunny
	DisplayObject->tell(Bunny);//Tell Quickhaptics about the bunny


	TriMesh* WheelLowRes = new TriMesh("Models/wheel-lo.obj");//Load the low resolution Wheel model
	WheelLowRes->setName("WheelLowRes");//give it a name
	WheelLowRes->setScale(1/12.0);//This model is too big compared to the bunnt model.. So we have to scale it down
	WheelLowRes->setStiffness(1.0);
	WheelLowRes->setFriction(0.5,0.3);//Give the Wheel some friction on the surface
	WheelLowRes->setShapeColor(0.65,0.65,0.65);//Give the Wheel a green color

	DisplayObject->tell(WheelLowRes);//Tell Quickhaptics about the low resolution Wheel

	TriMesh* WheelHighRes = new TriMesh("Models/wheel-hi.obj");//Load the High resolution Wheel model
        WheelHighRes->setName("WheelHighRes");//give it a name
	WheelHighRes->setScale(1/12.0);//Scale the Wheel model
        WheelHighRes->setStiffness(1.0);
	WheelHighRes->setFriction(0.5,0.3);//Give the Wheel some friction on the surface
	WheelHighRes->setRenderModeDepth();//Set the rendering mode to Depth Buffer. This is because the High resolution mode contains more than 65536 vertices
	WheelHighRes->setShapeColor(0.65,0.65,0.65);//Set the color of the shape to green
	DisplayObject->tell(WheelHighRes);//Tell Quickhaptics about the WheelHighRes

	Text* RenderModeMsg = new Text(30, "Render Mode: Feedback Buffer", 0.0, 0.95);//Set the message to be displayed on screen, with it's position in 
	//normalised coordinates. (0,0) is the lower left corner of the screen and (1,1) is the upper right corner.
	RenderModeMsg->setShapeColor(0.0,0.0,0.0);//Set the color as black.
	RenderModeMsg->setName("RenderModemessage");//Give the message a name
	DisplayObject->tell(RenderModeMsg);//Tell QuickHaptics about the text message

	Text* ModelStatMsg = new Text(24, "Stanford Bunny: 35,947 vertices",0.0, 0.875);//Create a text message and position it.
	ModelStatMsg->setShapeColor(0.0,0.0,0.0);//Set the color as black.
	ModelStatMsg->setName("ModelStatMessage");//Give the message a name
	DisplayObject->tell(ModelStatMsg);//Tell QuickHaptics about the text message

	Text* InstMsg = new Text(24, "Right click on screen to bring up the menu",0.0, 0.05);
	InstMsg->setShapeColor(0.0,0.0,0.0);//Set the color as black.
	InstMsg->setName("ModelStatMessage");//Tell QuickHaptics about the text message
	DisplayObject->tell(InstMsg);//Tell QuickHaptics about the text message

        Cursor* OmniCursor = new Cursor("Models/pencil.3DS");//Declare a new cursor
	OmniCursor->scaleCursor(0.002);//Scale the cursor because it is too large
	TriMesh* ModelTriMeshPointer = OmniCursor->getTriMeshPointer();
	ModelTriMeshPointer->setTexture("Models/pencil.JPG");
	DisplayObject->tell(OmniCursor);//tell QuickHaptics about the cursor
	
	
	//Make the The Hight and Low Res Wheels both haptically and graphically invisible
	/////////////////////////////////////////////////////////////////////////////////
	WheelLowRes->setHapticVisibility(false);
	WheelLowRes->setGraphicVisibility(false);
	WheelHighRes->setHapticVisibility(false);
	WheelHighRes->setGraphicVisibility(false);
	/////////////////////////////////////////////////////////////////////////////////


	
	//Create the GLUT menu
	glutCreateMenu(glutMenuFunction);
        //Add entries
	glutAddMenuEntry("Stanford Bunny - Feedback Buffer", 0);
	glutAddMenuEntry("Stanford Bunny - Depth Buffer", 1);
	glutAddMenuEntry("Wheel Low Resolution - Feedback Buffer", 2);
	glutAddMenuEntry("Wheel Low Resolution - Depth Buffer", 3);
	glutAddMenuEntry("Wheel High Resolution - Depth Buffer", 4);
	//Attach the menu to the right mouse button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
  
   
    
         qhStart();//Set everything in motion
         return 0;
}

void glutMenuFunction(int MenuID)
{
	static TriMesh* BunnyPointer = TriMesh::searchTriMesh("Bunny");//Search for a Pointer to the model
	static TriMesh* WheelLowRes = TriMesh::searchTriMesh("WheelLowRes");//Search for a Pointer to the model
	static TriMesh* WheelHighRes = TriMesh::searchTriMesh("WheelHighRes");//Search for a Pointer to the model
	static Text* RenderModeMsgPointer = Text::searchText("RenderModemessage");//Search for a Pointer to the Text
	static Text* ModelStatMsgPointer = Text::searchText("ModelStatMessage");//Search for a Pointer to the Text

	if(!(BunnyPointer && WheelLowRes && WheelHighRes && RenderModeMsgPointer && ModelStatMsgPointer))//If any of the models cannot be found then return
		return;
	

	//////////////////////////
	//////////////////////////
	if(MenuID == 0)//If the Bunny is clicked on
	{
		BunnyPointer->setHapticVisibility(true);//Make the Bunny Haptically visible
		BunnyPointer->setGraphicVisibility(true);//Make the Bunny Graphically visible

		////////////////////////////////////////////////////////////
		//Make the other models graphically and haptically invisible
		////////////////////////////////////////////////////////////
		WheelLowRes->setHapticVisibility(false);
		WheelLowRes->setGraphicVisibility(false);

		WheelHighRes->setHapticVisibility(false);
		WheelHighRes->setGraphicVisibility(false);
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		BunnyPointer->setRenderModeFeedback();
		WheelLowRes->setRenderModeFeedback();
		WheelHighRes->setRenderModeDepth();

		RenderModeMsgPointer->setText("Render Mode: Feedback Buffer");//For any other model change the message to feedback buffer
		ModelStatMsgPointer->setText("Stanford Bunny: 35,947 vertices");//Display message
		///////////////////////
		///////////////////////
	}
	else if(MenuID == 1)//If the low resolution Wheel is clicked on
	{
		BunnyPointer->setHapticVisibility(true);//Make the Bunny Haptically visible
		BunnyPointer->setGraphicVisibility(true);//Make the Bunny Graphically visible

		////////////////////////////////////////////////////////////
		//Make the other models graphically and haptically invisible
		////////////////////////////////////////////////////////////
		WheelLowRes->setHapticVisibility(false);
		WheelLowRes->setGraphicVisibility(false);

		WheelHighRes->setHapticVisibility(false);
		WheelHighRes->setGraphicVisibility(false);
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		BunnyPointer->setRenderModeDepth();
		WheelLowRes->setRenderModeDepth();
		WheelHighRes->setRenderModeDepth();

		RenderModeMsgPointer->setText("Render Mode: Depth Buffer");
		ModelStatMsgPointer->setText("Stanford Bunny: 35,947 vertices");//Display message
		///////////////////////
		///////////////////////
	}
	else if(MenuID == 2)//If the high resolution Wheel is clicked on 
	{
		WheelLowRes->setHapticVisibility(true);//Make the Low Res Wheel Haptically visible
		WheelLowRes->setGraphicVisibility(true);//Make the Low Res Wheel Graphically visible

		////////////////////////////////////////////////////////////
		//Make the other models graphically and haptically invisible
		////////////////////////////////////////////////////////////
		BunnyPointer->setHapticVisibility(false);
		BunnyPointer->setGraphicVisibility(false);

		WheelHighRes->setHapticVisibility(false);
		WheelHighRes->setGraphicVisibility(false);
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		BunnyPointer->setRenderModeFeedback();
		WheelLowRes->setRenderModeFeedback();
		WheelHighRes->setRenderModeDepth();

		
		RenderModeMsgPointer->setText("Render Mode: Feedback Buffer");
		ModelStatMsgPointer->setText("Wheel - Low Resolution: 49,989 vertices");
	}
	else if(MenuID == 3)
	{
		WheelLowRes->setHapticVisibility(true);//Make the Low Res Wheel Haptically visible;
		WheelLowRes->setGraphicVisibility(true);//Make the Low Res Wheel Graphically visible

		////////////////////////////////////////////////////////////
		//Make the other models graphically and haptically invisible
		////////////////////////////////////////////////////////////
		BunnyPointer->setHapticVisibility(false);
		BunnyPointer->setGraphicVisibility(false);

		WheelHighRes->setHapticVisibility(false);
		WheelHighRes->setGraphicVisibility(false);
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		BunnyPointer->setRenderModeDepth();
		WheelLowRes->setRenderModeDepth();
		WheelHighRes->setRenderModeDepth();

		RenderModeMsgPointer->setText("Render Mode: Depth Buffer");
		ModelStatMsgPointer->setText("Wheel - Low Resolution: 49,989 vertices");
	}

	else if(MenuID == 4)
	{
		
		WheelHighRes->setHapticVisibility(true);//Make the High Res Wheel Haptically visible;
		WheelHighRes->setGraphicVisibility(true);//Make the High Res Wheel Graphically visible;

		

		////////////////////////////////////////////////////////////
		//Make the other models graphically and haptically invisible
		////////////////////////////////////////////////////////////
		BunnyPointer->setHapticVisibility(false);
		BunnyPointer->setGraphicVisibility(false);
		
		
		WheelLowRes->setHapticVisibility(false);
		WheelLowRes->setGraphicVisibility(false);
		////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////
		BunnyPointer->setRenderModeDepth();
		WheelLowRes->setRenderModeDepth();
		WheelHighRes->setRenderModeDepth();

		RenderModeMsgPointer->setText("Render Mode: Depth Buffer");
		ModelStatMsgPointer->setText("Wheel - High Resolution: 147,489 vertices");
	}
}


	
	




