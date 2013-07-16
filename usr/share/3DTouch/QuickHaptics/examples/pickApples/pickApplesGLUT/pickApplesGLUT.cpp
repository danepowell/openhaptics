/////////////////////////////////////////////////////////////////////////////
//OpenHaptics QuickHaptics - Cow Color
//SensAble Technologies, Woburn, MA
//November 20, 2008
//Programmer: Venkat Gourishankar
//Modeler: Venkat Gourishankar
//Models built on solidworks & 3dsmax
//////////////////////////////////////////////////////////////////////////////



#include <QHHeadersGLUT.h>//Include all necessary headers

static const int appleCount = 9;

char* modelFileNames[appleCount] = {
    "models/appleBasket/apple0.3DS",
    "models/appleBasket/apple1.3DS",
    "models/appleBasket/apple2.3DS",
    "models/appleBasket/apple3.3DS",
    "models/appleBasket/apple4.3DS",
    "models/appleBasket/apple5.3DS",
    "models/appleBasket/apple6.3DS",
    "models/appleBasket/apple7.3DS",
    "models/appleBasket/apple8.3DS"
};

HDstring redAppleTex("models/appleBasket/redApple.jpg");
HDstring greenAppleTex("models/appleBasket/greenApple.jpg");
HDstring goldenAppleTex("models/appleBasket/goldenApple.jpg");

HDstring tempTextureState;
TriMesh* tempDroppedApple;

TriMesh* myApples[appleCount];

void touchCallback(unsigned int ShapeID);
void button1DownCallback(unsigned int ShapeID);
void button1UpCallback(unsigned int ShapeID);


int main(int argc, char *argv[])
{

    QHGLUT* DisplayObject = new QHGLUT(argc,argv);//create a display window
    DeviceSpace* OmniSpace = new DeviceSpace;//Find the default Phantom Device
    DisplayObject->tell(OmniSpace);//Tell QuickHaptics about it
    DisplayObject->setBackgroundColor(0.8,0.65,0.4);

    TriMesh* Basket = new TriMesh("models/appleBasket/bowl.3DS");
    Basket->setRotation(hduVector3Dd(1.0,0.0,0.0),45.0);
    Basket->setTexture("models/appleBasket/wood.jpg");
    Basket->setStiffness(0.9);
    Basket->setFriction(0.7,0.5);
    Basket->setUnDraggable();
    DisplayObject->tell(Basket);

    for(int i=0;i<appleCount;i++)
    {
        myApples[i] = new TriMesh(modelFileNames[i]);
        myApples[i]->setTexture(redAppleTex);
        myApples[i]->setRotation(hduVector3Dd(1.0,0.0,0.0),45.0);
		myApples[i]->setStiffness(0.6);
		myApples[i]->setDamping(0.1);
		myApples[i]->setFriction(0.5,0.3);
        DisplayObject->tell(myApples[i]);
    }

    Text* instructionMsg = new Text(20.0, "This example demonstrates Touch & Button Events", 0.25, 0.9 );
    instructionMsg->setShapeColor(0.9,0.9,0.0);
    DisplayObject->tell(instructionMsg);

    Cursor* OmniCursor = new Cursor;//Declare a new cursor
    DisplayObject->tell(OmniCursor);//tell QuickHaptics about the cursor

    for(int i=0;i<appleCount;i++)
    {
        OmniSpace->touchCallback(touchCallback, myApples[i]);
        OmniSpace->button1DownCallback(button1DownCallback, myApples[i]);
    }

    OmniSpace->button1UpCallback(button1UpCallback);    
    qhStart();//Set everything in motion
    return 0;
}

void touchCallback(unsigned int ShapeID)
{
    TriMesh* touchedApple = TriMesh::searchTriMesh(ShapeID);//Find a pointer to the object touched
    if(touchedApple==NULL)//id the pointer is NULl then return
        return;

    HDstring texture = (HDstring) touchedApple->getTextureFilename();

    (texture == redAppleTex) ? texture = greenAppleTex : texture = redAppleTex;

    touchedApple->setTexture(texture);
}

void button1DownCallback(unsigned int ShapeID)
{
    TriMesh* pickedApple = TriMesh::searchTriMesh(ShapeID);//Find a pointer to the object touched
    if(pickedApple==NULL)//id the pointer is NULl then return
        return;

    HDstring texture = (HDstring) pickedApple->getTextureFilename();

    if (texture == redAppleTex || greenAppleTex)
    {
        tempTextureState = texture;
        tempDroppedApple = pickedApple;
        texture = goldenAppleTex;
        pickedApple->setTexture(texture);
    }

}
void button1UpCallback(unsigned int ShapeID)
{
    if(tempDroppedApple==NULL)
        return;

    tempDroppedApple->setTexture(tempTextureState);

}
