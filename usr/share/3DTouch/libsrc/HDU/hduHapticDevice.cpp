/*****************************************************************************

Copyright (c) 2004 SensAble Technologies, Inc. All rights reserved.

OpenHaptics(TM) toolkit. The material embodied in this software and use of
this software is subject to the terms and conditions of the clickthrough
Development License Agreement.

For questions, comments or bug reports, go to forums at: 
    http://dsc.sensable.com
 
Module Name:

  HapticDevice.cpp

Description: 

  Provides a C++ wrapper for easing integration of the HD API. Offers basic 
  support for state synchronization and event handling

*******************************************************************************/

#include "hduAfx.h"
#include <string.h>
#include <HDU/hduHapticDevice.h>

#include <assert.h>
#include <queue>

using namespace std;

class HapticDeviceStateCache : public IHapticDevice::IHapticDeviceState
{
public:
    HapticDeviceStateCache() :
        m_bContact(false),
        m_pContactData(0)
    {
        memset(m_localXform, 0, 16 * sizeof(HDdouble));
        memset(m_proxyXform, 0, 16 * sizeof(HDdouble));
        memset(m_parentTworld, 0, 16 * sizeof(HDdouble));
    }

    HapticDeviceStateCache(const IHapticDevice::IHapticDeviceState &state)
    {
        setPosition(state.getPosition());
        setTransform(state.getTransform());
        setProxyPosition(state.getProxyPosition());
        setProxyTransform(state.getProxyTransform());
        setParentCumulativeTransform(state.getParentCumulativeTransform());
        setIsInContact(state.isInContact());
        setContactData(state.getContactData());
    }

    /* Current positional state in local coordinates. */
    void setPosition(const hduVector3Dd &p) { m_posLC = p; }
    const hduVector3Dd &getPosition() const { return m_posLC; }
    hduVector3Dd &getPosition() { return m_posLC; }
        
    void setTransform(const HDdouble *localXform) { 
        setXform(m_localXform, localXform); }
    const HDdouble *getTransform() const { return m_localXform; }
    HDdouble *getTransform() { return m_localXform; }

    void setProxyPosition(const hduVector3Dd &p) { m_proxyPosLC = p; }
    const hduVector3Dd &getProxyPosition() const { return m_proxyPosLC; }
    hduVector3Dd &getProxyPosition() { return m_proxyPosLC; }   

    void setProxyTransform(const HDdouble *proxyXform) { 
        setXform(m_proxyXform, proxyXform); }
    const HDdouble *getProxyTransform() const { return m_proxyXform; }
    HDdouble *getProxyTransform() { return m_proxyXform; }
    
    void setParentCumulativeTransform(const HDdouble *parentTworld) { 
        setXform(m_parentTworld, parentTworld); }
    const HDdouble *getParentCumulativeTransform() const { 
        return m_parentTworld; }
    HDdouble *getParentCumulativeTransform() { return m_parentTworld; }

    void setIsInContact(bool bContact) { m_bContact = bContact; }
    bool isInContact() const { return m_bContact; }

    void setContactData(void *pContactData) { m_pContactData = pContactData; }
    void *getContactData() const { return m_pContactData; }
    
    /* Error State */
    const HDErrorInfo &getLastError() const { return m_lastError; }
    void setLastError(const HDErrorInfo &rError) { m_lastError = rError; }

private:

    void setXform(HDdouble *dst, const HDdouble *src)
    {
        memcpy(dst, src, 16 * sizeof(HDdouble));
    }

    hduVector3Dd m_posLC, m_proxyPosLC;
    HDdouble m_localXform[16];
    HDdouble m_proxyXform[16];
    HDdouble m_parentTworld[16];

    bool m_bContact;
    void *m_pContactData;

    HDErrorInfo m_lastError;
};

struct HapticDeviceEvent
{
    IHapticDevice::EventType event;
    HapticDeviceStateCache m_state;
};

typedef queue<HapticDeviceEvent *> HapticDeviceEventQueue;

/******************************************************************************/

class HapticDevice : public IHapticDevice
{
public:
    HapticDevice(HHD hHD) : m_hHD(hHD)
    {
        memset(m_aCallbackFunc, 0, NUM_EVENT_TYPES * sizeof(void *));
        memset(m_aCallbackUserData, 0, NUM_EVENT_TYPES * sizeof(void *));
    }

    virtual ~HapticDevice()
    {
    }

    const IHapticDeviceState * const getCurrentState() const { 
        return &m_currentState; }
    IHapticDeviceState * const getCurrentState() { return &m_currentState; }

    const IHapticDeviceState * const getLastState() const { 
        return &m_lastState; }
    IHapticDeviceState * const getLastState() { return &m_lastState; }

    void setCallback(EventType event, 
                     HapticDeviceCallback *pCallback, 
                     void *pUserData)
    {
        if (event >= 0 && event < NUM_EVENT_TYPES)
        {
            m_aCallbackFunc[event] = pCallback;
            m_aCallbackUserData[event] = pUserData;
        }
    }

protected:
    static HDCallbackCode HDCALLBACK syncHapticDeviceState(void *pUserData);

    HHD m_hHD;

    HapticDeviceStateCache m_currentState;
    HapticDeviceStateCache m_lastState;

    HapticDeviceEventQueue m_eventQueue;

    HapticDeviceCallback *m_aCallbackFunc[NUM_EVENT_TYPES];
    void *m_aCallbackUserData[NUM_EVENT_TYPES];    
};

/******************************************************************************/

class HapticDeviceHT : public HapticDevice
{
public:

    HapticDeviceHT(HHD hHD) : HapticDevice(hHD)
    {
    }

    virtual ~HapticDeviceHT()
    {
    }

    InterfaceType getInterfaceType() const { 
        return IHapticDevice::HAPTIC_THREAD_INTERFACE; }

    void beginUpdate(IHapticDevice *pSyncToDevice);
    void endUpdate(IHapticDevice *pSyncToDevice);

protected:

    void handleEvent(EventType event);

};

/******************************************************************************/

class HapticDeviceGT : public HapticDevice
{
public:

    HapticDeviceGT(HHD hHD) : HapticDevice(hHD)
    {
    }

    virtual ~HapticDeviceGT()
    {
    }

    InterfaceType getInterfaceType() const { 
        return IHapticDevice::GRAPHIC_THREAD_INTERFACE; }

    void beginUpdate(IHapticDevice *pSyncToDevice);
    void endUpdate(IHapticDevice *pSyncToDevice);

    void handleEvent(EventType event, const IHapticDeviceState *pState);
};
/******************************************************************************/

IHapticDevice *IHapticDevice::create(IHapticDevice::InterfaceType eType, 
                                     HHD hHD)
{
    if (eType == IHapticDevice::HAPTIC_THREAD_INTERFACE)
    {
        return new HapticDeviceHT(hHD);
    }
    else if (eType == IHapticDevice::GRAPHIC_THREAD_INTERFACE)
    {
        return new HapticDeviceGT(hHD);
    }
    else
    {
        assert(!"Unsupported HapticDevice interface type");
        return 0;
    }
}

void IHapticDevice::destroy(IHapticDevice *&pInterface)
{
    if (pInterface)
    {
        delete pInterface;
        pInterface = 0;
    }
}

/*******************************************************************************
 HapticDevice Common Implementation
*******************************************************************************/
struct DeviceSyncPair
{
    HapticDevice *pSyncSrcDevice;
    HapticDevice *pSyncDstDevice;
};

HDCallbackCode HapticDevice::syncHapticDeviceState(void *pUserData)
{
    DeviceSyncPair *pPair = static_cast<DeviceSyncPair *>(pUserData);
    HapticDevice *pSrcDevice = pPair->pSyncSrcDevice;
    HapticDevice *pDstDevice = pPair->pSyncDstDevice;

    /* Update the device state from the src device. */
    pDstDevice->m_lastState = pDstDevice->m_currentState;
    pDstDevice->m_currentState = pSrcDevice->m_currentState;

    /* Grab all of the pending events from the src device. */
    while (!pSrcDevice->m_eventQueue.empty())
    {
        pDstDevice->m_eventQueue.push(pSrcDevice->m_eventQueue.front());
        pSrcDevice->m_eventQueue.pop();
    }

    return HD_CALLBACK_DONE;
}

/*******************************************************************************
 HapticDevice Haptic Thread Implementation
*******************************************************************************/

void HapticDeviceHT::beginUpdate(IHapticDevice *pSyncToDevice)
{
    hdBeginFrame(m_hHD);
    
    /*Save off the old state as last. */
    m_lastState = m_currentState;

    /* Update the cached position data. */
    hdGetDoublev(HD_CURRENT_POSITION, m_currentState.getPosition());
    hdGetDoublev(HD_CURRENT_TRANSFORM, m_currentState.getTransform());

    /* Check for a stylus switch state change. */
    HDint nCurrentButtonState, nLastButtonState;
    hdGetIntegerv(HD_CURRENT_BUTTONS, &nCurrentButtonState);
    hdGetIntegerv(HD_LAST_BUTTONS, &nLastButtonState);

    if ((nCurrentButtonState & HD_DEVICE_BUTTON_1) != 0 &&
        (nLastButtonState & HD_DEVICE_BUTTON_1) == 0)
    {
        handleEvent(BUTTON_1_DOWN);
    }
    else if ((nCurrentButtonState & HD_DEVICE_BUTTON_1) == 0 &&
             (nLastButtonState & HD_DEVICE_BUTTON_1) != 0)
    {
        handleEvent(BUTTON_1_UP);
    }
    
    if ((nCurrentButtonState & HD_DEVICE_BUTTON_2) != 0 &&
        (nLastButtonState & HD_DEVICE_BUTTON_2) == 0)
    {
        handleEvent(BUTTON_2_DOWN);
    }
    else if ((nCurrentButtonState & HD_DEVICE_BUTTON_2) == 0 &&
             (nLastButtonState & HD_DEVICE_BUTTON_2) != 0)
    {
        handleEvent(BUTTON_2_UP);
    }
    if ((nCurrentButtonState & HD_DEVICE_BUTTON_3) != 0 &&
        (nLastButtonState & HD_DEVICE_BUTTON_3) == 0)
    {
        handleEvent(BUTTON_3_DOWN);
    }
    else if ((nCurrentButtonState & HD_DEVICE_BUTTON_3) == 0 &&
             (nLastButtonState & HD_DEVICE_BUTTON_3) != 0)
    {
        handleEvent(BUTTON_3_UP);
    }

}


void HapticDeviceHT::endUpdate(IHapticDevice *pSyncToDevice)
{
    hdMakeCurrentDevice(m_hHD);

    /* Doing this as part of the endUpdate to give the client a chance to 
       update his simulation and stuff something into the contact state or 
       modify the proxy transform. */

    /* Check for change in contact state. */
    if (m_currentState.isInContact() && !m_lastState.isInContact())
    {
        handleEvent(MADE_CONTACT);
    }
    else if (!m_currentState.isInContact() && m_lastState.isInContact())
    {
        handleEvent(LOST_CONTACT);
    }

    hdEndFrame(m_hHD);

    /* Check if a device error occurred */
    m_currentState.setLastError(hdGetError());
    if (HD_DEVICE_ERROR(m_currentState.getLastError()))
    {        
        handleEvent(DEVICE_ERROR);    
    }
}

void HapticDeviceHT::handleEvent(EventType event)
{
    /* Check if there's a callback to be called. */
    HapticDeviceCallback *pCallback = (HapticDeviceCallback *) m_aCallbackFunc[event];
    if (pCallback)
    {
        pCallback(event, &m_currentState, m_aCallbackUserData[event]);
    }

    HapticDeviceEvent *pEvent = new HapticDeviceEvent;
    pEvent->event = event;
    memcpy(&pEvent->m_state, &m_currentState, sizeof(HapticDeviceStateCache));
    m_eventQueue.push(pEvent); 
}


/*******************************************************************************
 HapticDevice Graphic Thread Implementation
*******************************************************************************/
void HapticDeviceGT::beginUpdate(IHapticDevice *pSyncToDevice)
{
    DeviceSyncPair pair;
    pair.pSyncSrcDevice = static_cast<HapticDevice *>(pSyncToDevice);
    pair.pSyncDstDevice = this;
    assert(pair.pSyncSrcDevice);

    hdScheduleSynchronous(syncHapticDeviceState, &pair, HD_MIN_SCHEDULER_PRIORITY);

    /* Process any events that were added to the queue. */
    while (!m_eventQueue.empty())
    {
        handleEvent(m_eventQueue.front()->event, &m_eventQueue.front()->m_state);

        HapticDeviceEvent *pEvent = m_eventQueue.front();
        m_eventQueue.pop();
        delete pEvent;        
    }
}

void HapticDeviceGT::endUpdate(IHapticDevice *pSyncToDevice)
{
    /* Do nothing for now. */
}

void HapticDeviceGT::handleEvent(EventType event, 
                                 const IHapticDeviceState *pState)
{
    /* Check if there's a callback to be called. */
    HapticDeviceCallback *pCallback = 
        (HapticDeviceCallback *) m_aCallbackFunc[event];
    if (pCallback)
    {
        pCallback(event, pState, m_aCallbackUserData[event]);
    }
}
