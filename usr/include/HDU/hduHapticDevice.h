/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  hduHapticDevice.h

Description: 

  Provides a C++ wrapper for easing integration of the HD API. Offers basic 
  support for state synchronization and event handling

*******************************************************************************/

#ifndef IHapticDevice_H_
#define IHapticDevice_H_

#include <HD/hd.h>
#include <HDU/hduVector.h>

/*******************************************************************************
 IHapticDevice

 IHapticDevice is a convenient mechanism for haptic device state and event
 synchronization beteen graphic and haptic threads.  Typically, new state and
 events are obtained in the haptic thread and can be accessed in that thread
 via event callbacks or scheduler callbacks.  However, it is typical for state
 and event handling to also be processed in the graphics thread. For this
 it is necessary to have accurate snapshots of state that are available in
 the graphics thread in a thread-safe manner. The IHapticDevice interface
 is commonly used by a higher level manager class that mediates the usage
 of the two instantiations.
*******************************************************************************/
class IHapticDevice
{
public:

    struct IHapticDeviceState
    {
        /* Current positional state in local coordinates. */
        virtual void setPosition(const hduVector3Dd &p) = 0;
        virtual const hduVector3Dd &getPosition() const = 0;
        virtual hduVector3Dd &getPosition() = 0;

        virtual void setTransform(const HDdouble *localXform) = 0;
        virtual const HDdouble *getTransform() const = 0;
        virtual HDdouble *getTransform() = 0;
        
        virtual void setProxyPosition(const hduVector3Dd &p) = 0;
        virtual const hduVector3Dd &getProxyPosition() const = 0;
        virtual hduVector3Dd &getProxyPosition() = 0;

        virtual void setProxyTransform(const HDdouble *proxyXform) = 0;
        virtual const HDdouble *getProxyTransform() const = 0;
        virtual HDdouble *getProxyTransform() = 0;

        /* Cumulative transform matrices used for mapping haptic device 
           to scene. */
        virtual void setParentCumulativeTransform(const HDdouble *localTworld) = 0;
        virtual const HDdouble *getParentCumulativeTransform() const = 0;
        virtual HDdouble *getParentCumulativeTransform() = 0;

        /* Contact state. */
        virtual void setIsInContact(bool bContact) = 0;
        virtual bool isInContact() const = 0;

        virtual void setContactData(void *pContactData) = 0;
        virtual void *getContactData() const = 0;        

        /* Error State. */
        virtual const HDErrorInfo &getLastError() const = 0;
    };

    enum InterfaceType
    {
        HAPTIC_THREAD_INTERFACE,
        GRAPHIC_THREAD_INTERFACE
    };

    enum EventType
    {
        BUTTON_1_DOWN = 0,
        BUTTON_1_UP,
        BUTTON_2_DOWN,
        BUTTON_2_UP,
        BUTTON_3_DOWN,
        BUTTON_3_UP,
        MADE_CONTACT,
        LOST_CONTACT,
        DEVICE_ERROR,
        NUM_EVENT_TYPES,
    };

    typedef void (HapticDeviceCallback)(IHapticDevice::EventType event,
                  const IHapticDevice::IHapticDeviceState * const pState,
                  void *pUserData);

    static IHapticDevice *create(InterfaceType eType, HHD hHD);
    static void destroy(IHapticDevice *&pInterface);

    virtual void beginUpdate(IHapticDevice *pSyncToDevice) = 0;
    virtual void endUpdate(IHapticDevice *pSyncToDevice) = 0;
    virtual InterfaceType getInterfaceType() const = 0;

    virtual const IHapticDeviceState * const getCurrentState() const = 0;
    virtual IHapticDeviceState * const getCurrentState() = 0;

    virtual const IHapticDeviceState * const getLastState() const = 0;
    virtual IHapticDeviceState * const getLastState() = 0;

    virtual void setCallback(EventType event, 
                             HapticDeviceCallback *pCallback, 
                             void *pUserData) = 0;

protected:
    IHapticDevice() {}
    virtual ~IHapticDevice() {}
}; /* IHapticDevice */

#endif /* IHapticDevice_H_ */

/*****************************************************************************/
