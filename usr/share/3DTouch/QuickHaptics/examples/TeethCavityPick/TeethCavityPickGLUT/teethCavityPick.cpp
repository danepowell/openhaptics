/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - TeethCavityPick example
//SensAble Technologies, Woburn, MA
//November 11, 2008
//Programmer: Venkat Gourishankar
//////////////////////////////////////////////////////////////////////////////

#include <QHHeadersGLUT.h>//Include all necessary headers


// Callback functions
void button1DownCallback(unsigned int ShapeID);
void button1UpCallback(unsigned int ShapeID);
void touchCallback(unsigned int ShapeID);

void graphicsCallback(void);


// Global state for directing callback function behavior
bool button1Down;
bool cursorMoving;
bool draggingGumModel;
bool draggingTeethModel;
bool draggingCavityFillModel;
bool draggingCavityModel;

TriMesh* gDentureGum = NULL;
TriMesh* gDentureTeeth = NULL;
TriMesh* gDentureCavityFill = NULL;
TriMesh* gDentureCavity = NULL;

Box* gStartButton = NULL;
Box* gResetButton = NULL;

Text* gStartupMsg = NULL;
Text* gResetMsg = NULL;
Text* gInstructionMsg = NULL;
Text* gSuccessMsg = NULL;



int main(int argc, char *argv[])
{
	QHGLUT* DisplayObject = new QHGLUT(argc,argv);	//create a display window

	DeviceSpace* deviceSpace = new DeviceSpace;		//Find a Phantom device named "Default PHANToM"
	DisplayObject->tell(deviceSpace);				//tell Quickhaptics that Omni exists
	DisplayObject->setBackgroundColor(0.0,0.0,0.6);

	DisplayObject->setHapticWorkspace(hduVector3Dd(-40,-40.0,-17.0), hduVector3Dd(95,45.0,17.0));

	// Load gums model
	TriMesh* tm = new TriMesh("models/TeethCavityPickModels/dentures-gums.obj");	
	gDentureGum = tm;

	tm->setName("dentureGum");
	tm->setShapeColor(1.0,0.5,0.65);
	tm->setRotation(hduVector3Dd(1.0,0.0,0.0), 45.0);
	tm->setStiffness(0.5);
	tm->setDamping(0.6);
	tm->setFriction(0.3,0.0);
	DisplayObject->tell( tm );//Tell quickhaptics that gums exists

	// Load teeth model
	tm = new TriMesh("models/TeethCavityPickModels/dentures-teeth.obj");
	gDentureTeeth = tm;

	tm->setName("dentureTeeth");
	tm->setRotation(hduVector3Dd(1.0,0.0,0.0), 45.0);
	tm->setStiffness(1.0);
	tm->setDamping(0.0);
	tm->setFriction(0.0,0.2);
	DisplayObject->tell(tm);


	// Load cavity model
	tm = new TriMesh("models/TeethCavityPickModels/dentures-cavity fill.obj");
	gDentureCavityFill = tm;
	tm->setName("dentureCavityFill");
	tm->setRotation(hduVector3Dd(1.0,0.0,0.0), 45.0);
	tm->setPopthrough(0.5);
	tm->setStiffness(0.6);
	tm->setDamping(0.3);
	tm->setFriction(0.5,0.4);
	DisplayObject->tell(tm);


	// Load cavity "target"
	tm = new TriMesh("models/TeethCavityPickModels/dentures-marker.obj");
	gDentureCavity = tm;

	tm->setName("dentureCavity");
	tm->setUnDraggable();
	tm->setRotation(hduVector3Dd(1.0,0.0,0.0), 45.0);
	tm->setStiffness(0.2);
	tm->setDamping(0.4);
	tm->setFriction(0.0,0.0);
	DisplayObject->tell(tm);

	// SensAble logo
	Plane* logoBox = new Plane(15,9);
	logoBox->setTranslation(53.0,-27.0,30.0);
	logoBox->setHapticVisibility(false);
	logoBox->setTexture("models/TeethCavityPickModels/sensableLogo.jpg");
	DisplayObject->tell(logoBox);

	// START button
	Box* box = gStartButton = new Box(20,10,10);
	gStartButton = box;

	box->setName("startButton");
	box->setUnDraggable();
	box->setTranslation(60.0,20.0,0.0);
	box->setRotation(hduVector3Dd(0.0,1.0,0.0), -15.0);
	box->setTexture("models/TeethCavityPickModels/start.jpg");
	DisplayObject->tell(box);

	// RESET button
	box = new Box(20,10,10);
	gResetButton = box;

	box->setName("resetButton");
	box->setUnDraggable();
	box->setTranslation(60.0,-5.0,0.0);
	box->setRotation(hduVector3Dd(0.0,1.0,0.0), -15.0);
	box->setTexture("models/TeethCavityPickModels/reset.jpg");
	DisplayObject->tell(box);


	// Startup Message
	Text* text = new Text (20.0,"Please touch START & press button 1 to begin", 0.25, 0.9);
	gStartupMsg = text;

	text->setName("startupMsg");
	text->setShapeColor(0.0,0.5,0.75);
	text->setHapticVisibility(false);
	text->setGraphicVisibility(true);
	DisplayObject->tell(text);

	// Reset Message
	text = new Text (20.0,"Please touch RESET and press button 1 to Reset the demo", 0.2, 0.85);
	gResetMsg = text;

	text->setName("resetMsg");
	text->setShapeColor(0.0,0.5,0.75);
	text->setHapticVisibility(false);
	text->setGraphicVisibility(false);
	DisplayObject->tell(text);

	// Instruction Message
	text = new Text (20.0,"Please locate the cavity  by probing the teeth", 0.25, 0.9);
	gInstructionMsg = text;

	text->setName("instructionMsg");
	text->setShapeColor(0.0,0.5,0.75);
	text->setHapticVisibility(false);
	text->setGraphicVisibility(false);
	DisplayObject->tell(text);

	// Success Message
	text = new Text (20.0,"OUCH!!&*! You have successfully located the cavity", 0.25, 0.9);
	gSuccessMsg = text;

	text->setName("successMsg");
	text->setShapeColor(1.0,0.35,0.5);
	text->setHapticVisibility(false);
	text->setGraphicVisibility(false);
	DisplayObject->tell(text);

	Cursor* OmniCursor = new Cursor("models/TeethCavityPickModels/dentalPick.obj");//Load a cursor that looks like a dental pick
    TriMesh* cursorModel = OmniCursor->getTriMeshPointer();
    cursorModel->setShapeColor(0.35,0.35,0.35);
	OmniCursor->scaleCursor(0.007);
	OmniCursor->setRelativeShapeOrientation(0.0,0.0,1.0,-90.0);

	//    OmniCursor->debugCursor(); //Use this function the view the location of the proxy inside the Cursor mesh
	DisplayObject->tell(OmniCursor);//Tell QuickHaptics that the cursor exists

	DisplayObject->preDrawCallback(graphicsCallback);


	deviceSpace->button1DownCallback(button1DownCallback, gResetButton);
	deviceSpace->button1DownCallback(button1DownCallback, gDentureGum);
	deviceSpace->button1DownCallback(button1DownCallback, gDentureTeeth);
	deviceSpace->button1DownCallback(button1DownCallback, gDentureCavityFill);
	deviceSpace->button1DownCallback(button1DownCallback, gStartButton);
	deviceSpace->touchCallback(touchCallback, gDentureCavity);

	deviceSpace->button1UpCallback(button1UpCallback);

	qhStart();//Set everything in motion
	return 0;
}


void button1DownCallback(unsigned int ShapeID)
{
	TriMesh* modelTouched = TriMesh::searchTriMesh(ShapeID);
	Box* buttonTouched    = Box::searchBox(ShapeID);

	draggingGumModel = false;
	draggingTeethModel = false;
	draggingCavityFillModel = false;
	draggingCavityModel = false;

	if (modelTouched == gDentureGum)
	{
		draggingGumModel = true;
		
		gDentureTeeth->setHapticVisibility(false);
		gDentureCavityFill->setHapticVisibility(false);
		gDentureCavity->setHapticVisibility(false);
	}
	else if (modelTouched == gDentureTeeth)
	{
		draggingTeethModel = true;

		gDentureGum->setHapticVisibility(false);
		gDentureCavityFill->setHapticVisibility(false);
		gDentureCavity->setHapticVisibility(false);	
	}
	else if (modelTouched == gDentureCavityFill)
	{
		draggingCavityFillModel = true;
		
		gDentureTeeth->setHapticVisibility(false);
		gDentureGum->setHapticVisibility(false);
		gDentureCavity->setHapticVisibility(false);
	}


	if (buttonTouched == gStartButton)
	{
		gStartupMsg->setGraphicVisibility(false);
		gInstructionMsg->setGraphicVisibility(true);
		gSuccessMsg->setGraphicVisibility(false);
		gResetMsg->setGraphicVisibility(false);
	}
	else if (buttonTouched == gResetButton)
	{
		gInstructionMsg->setGraphicVisibility(false);
		gSuccessMsg->setGraphicVisibility(false);
		gStartupMsg->setGraphicVisibility(true);
		gResetMsg->setGraphicVisibility(false);
	}
}

void button1UpCallback(unsigned int ShapeID)
{
	draggingGumModel = false;
	draggingTeethModel = false;
	draggingCavityFillModel = false;

	gDentureGum->setHapticVisibility(true);
	gDentureTeeth->setHapticVisibility(true);
	gDentureCavityFill->setHapticVisibility(true);
	gDentureCavity->setHapticVisibility(true);
}

void touchCallback(unsigned int ShapeID)
{
	TriMesh* modelTouched = TriMesh::searchTriMesh(ShapeID);

	if (modelTouched == gDentureCavity)
	{
		gSuccessMsg->setGraphicVisibility(true);
		gStartupMsg->setGraphicVisibility(false);
		gInstructionMsg->setGraphicVisibility(false);
		gResetMsg->setGraphicVisibility(true);

		gDentureCavityFill->setHapticVisibility(false);
	}
	else
	{
		gDentureCavityFill->setHapticVisibility(true);
		gDentureTeeth->setHapticVisibility(true);
		gDentureGum->setHapticVisibility(true);
	}
}

void graphicsCallback()
{
hduMatrix globalDragTransform;

	if (draggingGumModel)
	{
		globalDragTransform = gDentureGum->getTransform();

		gDentureCavity->setTransform(globalDragTransform);
		gDentureTeeth->setTransform(globalDragTransform);
		gDentureCavityFill->setTransform(globalDragTransform);
	}
	else if (draggingTeethModel)
	{
		globalDragTransform = gDentureTeeth->getTransform();

		gDentureCavity->setTransform(globalDragTransform);
		gDentureGum->setTransform(globalDragTransform);
		gDentureCavityFill->setTransform(globalDragTransform);
	}
	else if (draggingCavityFillModel)
	{
		globalDragTransform = gDentureCavityFill->getTransform();

		gDentureCavity->setTransform(globalDragTransform);
		gDentureGum->setTransform(globalDragTransform);
		gDentureTeeth->setTransform(globalDragTransform);
	}
}
