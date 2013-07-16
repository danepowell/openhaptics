//---------------------------------------------------------------------
//
// Copyright 2008, SensAble Technologies, Inc.
//
// File: DeviceSpace.h
//
// Author: Hari Vasudevan	
//
// Created: 09/11/2008
//
// Description: Declares the device class that initialises the 
// the Phantom device and manages context switching between devices.
// It also manages effects such as constant force, damping etc...
//
//---------------------------------------------------------------------


#ifndef DEVICE_SPACE_H
#define DEVICE_SPACE_H


#include "ExportDef.h"
#include "Globals.h"
#include "Sphere.h"


#define EFFECT_START 0
#define EFFECT_UPDATE 1

/*! \brief This class describes parameters associated with a device.

This class also initialises/deinitialises the devices and maintains the callback structure. DeviceSpace may
be visualised as the haptic space surrounding a haptic device. The space has many parameters such as m_Damping,
friction and a constant force effect.

Another special callback maintained by this class is the ServoLoopCallback. This callback runs at the scheduler rate 
under HLAPI(by default 1KHz). Advanced users can use this callback to render different force effets. Often there will be regular
high speed computations required. Such computations can be processes in this callback

*/
class QHAPI DeviceSpace
{
		
		friend class QHRenderer;
		friend void HLCALLBACK internalMotionCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalTouchCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalUnTouchCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalButton1DownCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalButton1UpCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalButton2DownCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
		friend void HLCALLBACK internalButton2UpCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);

		///Structure to hold friction parameters
		struct FrictionValues
		{
			HLdouble m_Gain;
			HLdouble m_Magnitude;
			HLuint m_FrictionID;
			bool m_FrictionIDFlag;
			bool m_StartOrUpdate;
			bool m_EffectUpdateFlag;
		};

		///Structure to hold damping parameters
		struct DampingValues
		{
			HLdouble m_Gain;
			HLdouble m_Magnitude;
			HLuint m_DampingID;
			bool m_DampingIDFlag;
			bool m_StartOrUpdate;
			bool m_EffectUpdateFlag;
		};

		///Structure to hold constant force parameters
		struct ConstantForceValues
		{
			hduVector3Dd Direction;
			HLdouble m_Magnitude;
			HLuint m_ConstantForceID;
			bool m_ConstantForceIDFlag;
			bool m_StartOrUpdate;
			bool m_EffectUpdateFlag;
		};

		///Initialise HDAPI	
		void initHD(void);
		///Deinitialise HDAPI
		void uninitHD(void);
		///Initialise HLAPI	
		void initHL(void);
		///Deinitialise HLAPI
		void uninitHL(void);

		///This function removes all the callbacks on program exit
		void removeCallback(void);
		///Function translate from uAPI event names to uAPI event names
		HLenum eventLookup(char* Event);

		///This variable is used for initialisong the haptic device, if the device is the default device.
		int m_NullNumber;

	 	///STL list to keep track of the m_Force Effects that should be rendered
		std::list<HLuint> m_EffectID;
		///STL Iterator for the m_EffectID
		std::list<HLuint>::iterator m_EffectIDIterator;

		///This variable specifies if the exvents have to be updated on the subsequent haptic-graphic frame.
		bool m_EffectUpdateFlag;

		///Function to add a user defined callback
		void addCallback(char* Event, Shape* m_pShapePointer, void (HLCALLBACK* CallbackFunction)(HLenum,HLuint,HLenum,HLcache*,void*));
		
		//Function to remove a userdefined callback
		void removeCallback(char* Event, Shape* m_pShapePointer, void (HLCALLBACK* CallbackFunction)(HLenum,HLuint,HLenum,HLcache*,void*));
		

		
		///This variable holds the current force baeing rendered by the haptic device
		hduVector3Dd m_Force;
		///Variable to hold friction parameters
		FrictionValues m_FrictionEffect;
		///Variable to hold m_Damping parameters
		DampingValues m_DampingEffect;
		///Variable to hold Constant m_Force parameters
		ConstantForceValues m_ConstantForceEffect;

		///String to hold the "name" of the device as set in Phantom Config
		HDstring m_DeviceName;

        ///This variable contains the "name" of the device which will be searchable by the SearchSpace function
        HDstring m_SearchableDeviceName;

		//These pointers are used to manage the simplified callback structure.
		//If the callbacks are not used these function pointer are assigned '0' in the constructor
		//Function pointer for the simplified callback structure (Touchcallback)
		void (*m_pTouchCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (unTouchCallback)
		void (*m_pUnTouchCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (motionCallback)
		void (*m_pMotionCallbackPointer)(HLuint);
		
		//Function pointer for the simplified callback structure (Button1Down)
		void (*m_pButton1DownCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (Button1Up)
		void (*m_pButton1UpCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (Button2Down)
		void (*m_pButton2DownCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (Button2Up)
		void (*m_pButton2UpCallbackPointer)(HLuint);
		//Function pointer for the simplified callback structure (GraphicLoop)
		static void (*m_pGraphicLoopCallbackPointer)(void);

		//Function to remove all force effects at exit
		void internalFnRemoveEffects(void);

		struct ErrorStructure
		{
			char ErrorString[200];
			bool m_ErrorProcessedFlag;
		};
		
		///This is a list of all errors generated in loading the model
		std::list<ErrorStructure> m_ErrorList;
		///This is an iterator user to run through all the errors generated
		std::list<ErrorStructure>::iterator m_ErrorListIterator;

        ///Function to store off errors.
		void InternalFnStoreErrorString(char* ErrorString);
		///Function to retrieve error messages, m_A 0 value means thatthere are no more errors to retrieve.
		char* InternalFnRetrieveErrorString(void);
		///This function is used to capture the force value from the device. This is an internal function, not intended for programmer's use
		void storeForceValue(hduVector3Dd m_Force);
		///This function returns the state of the friction effect flag(which indicates if the effect is active
		///or not. This is an internal function. Not encouraged for programmer use.
		bool getFrictionEffectFlag(void);
		
		///This function returns the state of the damping effect flag(which indicates if the effect is active
		///or not. This is an internal function. Not encouraged for programmer use.
		bool getDampingEffectFlag(void);
		
		///This function returns the state of the constant force effect flag(which indicates if the effect is active
		///or not. This is an internal function. Not encouraged for programmer use.
		bool getConstantForceFlag(void);

		HDstring getInternalDeviceName(void);

		void doDamping(void);
		void doConstantForce(void);
		void doFriction(void);



	public:

		

		
		/*! \brief This function returns the name of the haptic device as defined in Phantom Configuration.

		The name for the  Default PHANToM in the QuickHaptics uAPI is "Default PHANToM".*/
		HDstring getDeviceName(void);
		
		/*! \brief This function returns the force commanded to the haptic device in the previous haptic frame*/
		hduVector3Dd getForce(void);
		

		
	
		/*! \brief Function to schedule the touch callback. If this function is used then the callback is invoked only on contact with the Shape

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->touchCallback(CallBackFn, Sphere);\n
		OmniSpace->touchCallback(CallBackFn, Box);\n
		
		Note: THIS WILL NOT WORK\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->touchCallback(CallBackFn1, Sphere);\n
		OmniSpace->touchCallback(CallBackFn2, BiscuitBox);*/
		void touchCallback(void (*m_pTouchCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a touch callback. This function is invoked when ANY shape is touched.
		void touchCallback(void (*m_pTouchCallbackPointer)(HLuint));

		/*! \brief Function to schedule the untouch callback. If this function is used then the callback is invoked only on discontinuation
		of contact with the Shape primitive

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->untouchCallback(CallBackFn, Sphere);\n
		OmniSpace->untouchCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->untouchCallback(CallBackFn1, Sphere);\n
		OmniSpace->untouchCallback(CallBackFn2, BiscuitBox);*/
		void unTouchCallback(void (*m_pUnTouchCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule an untouch callback. This function is invoked when ANY shape is untouched.
		void unTouchCallback(void (*m_pUnTouchCallbackPointer)(HLuint));

		/*! \brief Function to schedule the motion callback. If this function is used then the callback is invoked only on motion Shape primitive.

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->motionCallback(CallBackFn, Sphere);\n
		OmniSpace->motionCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->motionCallback(CallBackFn1, Sphere);\n
		OmniSpace->motionCallback(CallBackFn2, BiscuitBox);*/
		void motionCallback(void (*m_pMotionCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a motion callback. This function is invoked for any motion of the Phantom
		void motionCallback(void (*m_pMotionCallbackPointer)(HLuint));

		/*! \brief Function to schedule the Button 1 down callback. If this function is used then the callback is invoked only
		if the button 1 is pressed while the proxy is on a Shape primitive.

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button1DownCallback(CallBackFn, Sphere);\n
		OmniSpace->button1DownCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button1DownCallback(CallBackFn1, Sphere);\n
		OmniSpace->button1DownCallback(CallBackFn2, BiscuitBox);*/
		void button1DownCallback(void (*m_pButton1DownCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a Button1 down callback.
		void button1DownCallback(void (*m_pButton1DownCallbackPointer)(HLuint)); 

		/*! \brief Function to schedule the Button1 up callback. If this function is used then the callback is invoked only
		if the button 1 is released while the proxy is on a Shape primitive.

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button1UpCallback(CallBackFn, Sphere);\n
		OmniSpace->button1UpCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button1UpCallback(CallBackFn1, Sphere);\n
		OmniSpace->button1UpCallback(CallBackFn2, BiscuitBox);*/
		void button1UpCallback(void (*m_pButton1UpCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a Button1 up down callback.
		void button1UpCallback(void (*m_pButton1UpCallbackPointer)(HLuint));


		/*! \brief Function to schedule the Button 2 down callback. If this function is used then the callback is invoked only
		if the button 2 is pressed while the proxy is on a Shape primitive.

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button2DownCallback(CallBackFn, Sphere);\n
		OmniSpace->button2DownCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button2DownCallback(CallBackFn1, Sphere);\n
		OmniSpace->button2DownCallback(CallBackFn2, BiscuitBox);*/
		void button2DownCallback(void (*m_pButton2DownCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a Button2 down callback.
		void button2DownCallback(void (*m_pButton2DownCallbackPointer)(HLuint));

		/*! \brief Function to schedule the Button 2 up callback.  If this function is used then the callback is invoked only
		if the button 2 is released while the proxy is on a Shape primitive.

		Use this function call multiple times with pointers to multiple shapes if you want to invoke the callback for multiple shapes.
		For eg:\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button2UpCallback(CallBackFn, Sphere);\n
		OmniSpace->button2UpCallback(CallBackFn, Box);\n
		
		Note: THE CODE BELOW WILL NOT WORK and only CallBackFn2 will be invoked on contact with BiscuitBox\n
		Sphere* FootBall = new Sphere;\n
		Box* BiscuitBox = new Box;\n
		OmniSpace->button2UpCallback(CallBackFn1, Sphere);\n
		OmniSpace->button2UpCallback(CallBackFn2, BiscuitBox);*/
		void button2UpCallback(void (*m_pButton2UpCallbackPointer)(HLuint),Shape* m_pShapePointer);
		///Function to schedule a Button2 down callback.
		void button2UpCallback(void (*m_pButton2UpCallbackPointer)(HLuint));
		/* \brief This function defines a graphic callback that is invoked on every graphic frame.	
		
		There can exist only ONE graphic callback function. Multiple calls to the preDrawCallback function will schedule 
		the function declared last as the Graphics Callback*/
		//void preDrawCallback(void (*m_pGraphicLoopCallbackPointer)(void));
		
		
		

		///Default constructor (Initialises the Default phantom)
		DeviceSpace(void);
		///Constructor to initiliase any other named device (from Phantom Configuration) eg: PHANToM 1
		DeviceSpace(HDstring m_DeviceName);
		//Destructor
		~DeviceSpace(void);

		/*! \brief Default friction

		This function starts the default ambient stick-slip friction effect. The parameters for this effect are:\n
		StaticFriction = 0.5\n
		DynamicFriction = 0.5\n*/
		void setFriction(void);
		/*! \brief This version of the friction function allows the user to set the ambient friction parameters explicitly.

		The parameters for this effect are:\n
		Static Friction = user-defined\n
		Dynamic Friction = user-defined\n
		where\n*
		0.0 <= Static Friction <= 1.0\n
		0.0 <= Dynamic Friction <= 1.0\n/	*/	
		void setFriction(HLdouble m_Gain,HLdouble m_Magnitude);
		
		/*! \brief This function allows the programmer to set an ambient damping effect

		The parameters for this effect are:\n
		Gain = 0.9\n
		Magnitude = 0.6\n
		The above two quantities imply that the force law F = -"Gain"*v is implemented and if F > Magnitude then F = magnitude. Where
		the magnitude is normalised between zero and the max continuous force capability of the device.\n The allowable range of values for 
		Gain and Magnitude are:\n
		0.0 <= Gain <= 1.0\n
		0.0 <= Magnitude <= 1.0*/
		void setDamping(void);
		/*! \brief This function allows the user to set an ambient damping effect with additional parameters

		The parameters for this effect are Gain and Magnitude\n
		The above two quantities imply that the force law F = -"Gain"*v is implemented and if F > Magnitude then F = magnitude. Where
		the magnitude is normalised between zero and the max continuous force capability of the device.\n The allowable range of values for 
		Gain and Magnitude are:\n
		0.0 <= Gain <= 1.0\n
		0.0 <= Magnitude <= 1.0*/
		void setDamping(HLdouble m_Gain, HLdouble m_Magnitude);
		
		/*! \brief Default Constant force effect

		This function maked the haptic device exert a constant force.\n
		The default parameters for this effect are:\n
		Force Direction: Negative Y axis\n
		Force Magnitude: 0.5 \n
		Here the Force magnitude is normalised between zero and the max continuous force capability of the device.*/
		void setConstantForce(void);
		/*! \brief This is the overridden Constant Force effect that allows the programmer to set the direction and magnitude of force.

		This function maked the haptic device exert a constant force.\n
		The default parameters for this effect are:\n
		Force Direction: user-defined vector\n
		0.0 <= Force Magnitude <= 1.0\n
		Here the Force magnitude is normalised between zero and the max continuous force capability of the device.*/
		void setConstantForce(hduVector3Dd Direction, HLdouble m_Magnitude);

	
		/*! \brief This function defines the servoloop callback, The servoloop function will be invoked at the servo loop rate (1000 Hz)

		The servoloop is a high priority thread managed by OpenHaptics. The function runs at the frequency of 
		the servoLoop thread. (usually 1KHz)
		This function accepts three function pointers. For eg:\n

		startServoLoopCallback(StartServoLoopCallback, MainServoLoopCallback, StopServoLoopCallback, userData);\n

		1) StartServoLoopcallBack - This function will be called once before the main body of the loop\n
		2) MainServoLoopCallback - This function will be the called at the servoLooprate\n
		3) StopServoLoopCallback - This function will be called once after the exit of the servoloop\n
		4) userdata - Any void pointer.\n\n
		Note: The servoloop callback along shuts down and starts up at every device error. i.e velocity error, force error. So the functions
		StartServoLoopcallBack and StopServoLoopCallback will be invoked once on every such error.*/
		void startServoLoopCallback(void (HLCALLBACK* startServoLoopCallback)(HLcache*, void*),void (HLCALLBACK* MainServoLoopCallback)(HDdouble force[3], HLcache*, void*), void (HLCALLBACK* StopServoLoopCallback)(HLcache*, void*),void* userData);

		/* \brief Function to search for a Device by "Name".

		The name in this function corresponds to the name assigned to the device in PhantomConfiguration. If the function does not find a 
		device, it will return a NULL. It is the responsibility of the programmer to handle the NULL pointer safely. To search for the 
		default device, search for the string "Default PHANToM" which id the neme set for the default device in  PhantomConfiguration*/
		static DeviceSpace* searchSpace(char* SpaceName);

};

/* \brief m_A pointer to this structure is passed in the callback.

This structure is used to mainitain an STL list of all the callbacks defined by the user. The STL list grows 
longer depending on the number of callbacks defined by the user. The list also is used to remove(deregister?)
all callbacks at exit


*/

struct CallbackMonitor
{
	CallbackMonitor();
	
	//For which event is the callback being registered?
	char* Event;
	//What is the ShapeID the event is registered to?
	HLuint HapticShapeID;
	//Which callback function is being called?
	void (HLCALLBACK* CallbackFunction)(HLenum,HLuint,HLenum,HLcache*,void*);
	//What is the device name? This is used to idetify the context the callback is invoked from
	HDstring m_DeviceName;
};
//Internal callback function............Not for end-user use!!
void HLCALLBACK internalMotionCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);

//Internal callback function............Not for end-user use!!
void HLCALLBACK internalTouchCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
//Internal callback function............Not for end-user use!!
void HLCALLBACK internalUnTouchCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);

//Internal callback function............Not for end-user use!!
void HLCALLBACK internalButton1DownCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
//Internal callback function............Not for end-user use!!
void HLCALLBACK internalButton1UpCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);

//Internal callback function............Not for end-user use!!
void HLCALLBACK internalButton2DownCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);
//Internal callback function............Not for end-user use!!
void HLCALLBACK internalButton2UpCallback(HLenum event,HLuint object,HLenum thread,HLcache *cache,void *userdata);


#endif




