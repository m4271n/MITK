/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef MITKUSTerasonDevice_H_HEADER_INCLUDED_
#define MITKUSTerasonDevice_H_HEADER_INCLUDED_

#include "mitkUSDevice.h"
#include "mitkUSTerasonImageSource.h"
//#include "mitkUSTerasonScanConverterPlugin.h"
//#include "mitkUSTerasonProbesControls.h"
//#include "mitkUSTerasonBModeControls.h"
//#include "mitkUSTerasonDopplerControls.h"
//
//#include "mitkUSTerasonSDKHeader.h"
#include "MitkUSHardwareTerasonExports.h"

#include "us_engine.h"

namespace mitk {
  /**
    * \brief Implementation of mitk::USDevice for Terason API devices.
    * Connects to a Terason API device through its COM library interface.
    *
    * This class handles all API communications and creates interfaces for
    * b mode, doppler and probes controls.
    * Images given by the device are put into an object of
    * mitk::USTerasonImageSource.
    *
    * It implements IUsgDeviceChangeSink of the Terason API to be notified
    * of changes to beamformer device or probes (e.g. probe change).
    */
  class MITKUSHARDWARETERASON_EXPORT USTerasonDevice : public USDevice
  {
  public:
    mitkClassMacro(USTerasonDevice, mitk::USDevice);
    mitkNewMacro2Param(Self, std::string, std::string);

    /**
    * \brief Returns the class of the device.
    */
    virtual std::string GetDeviceClass();

    //virtual USControlInterfaceBMode::Pointer GetControlInterfaceBMode();
    //virtual USControlInterfaceProbes::Pointer GetControlInterfaceProbes();
    //virtual USControlInterfaceDoppler::Pointer GetControlInterfaceDoppler();

    /**
      * \brief Is called during the initialization process.
      * There is nothing done on the initialization of a mik::USTerasonDevive object.
      *
      * \return always true
      */
    virtual bool OnInitialization();

    /**
      * \brief Is called during the connection process.
      * Connect to the Terason API and try to get available probes from the device.
      *
      * \return true if successfull, false if no device is connected to the pc
      * \throws mitk::Exception if something goes wrong at the API calls
      */
    virtual bool OnConnection();

    /**
      * \brief Is called during the disconnection process.
      * Deactivate and remove all Terason API controls. A disconnect from the
      * Terason API is not possible for which reason the hardware stays in connected
      * state even after calling this method.
      *
      * \return always true
      * \throws mitk::Exception if something goes wrong at the API calls
      */
    virtual bool OnDisconnection();

    /**
      * \brief Is called during the activation process.
      * After this method is finished, the device is generating images in b mode.
      * Changing scanning mode is possible afterwards by using the appropriate
      * control interfaces.
      *
      * \return always true
      * \throws mitk::Exception if something goes wrong at the API calls
      */
    virtual bool OnActivation();

    /**
      * \brief Is called during the deactivation process.
      * After a call to this method the device is connected, but not producing images anymore.
      *
      * \return always true
      * \throws mitk::Exception if something goes wrong at the API calls
      */
    virtual bool OnDeactivation();

    /**
      * \brief Changes scan state of the device if freeze is toggeled in mitk::USDevice.
      */
    virtual void OnFreeze(bool freeze);

    /** @return Returns the current image source of this device. */
    USImageSource::Pointer GetUSImageSource( );

    ///**
    //  * \brief Getter for main Terason API object.
    //  * This method is for being called by Terason control interfaces.
    //  */
    //Usgfw2Lib::IUsgfw2* GetUsgMainInterface();

    ///**
    //  * \brief Changes active IUsgDataView of the device.
    //  * This method is for being called by Terason control interfaces.
    //  */
    //void SetActiveDataView(Usgfw2Lib::IUsgDataView*);

    //// Methods implemented for IUsgDeviceChangeSink
    //virtual HRESULT __stdcall raw_OnProbeArrive(IUnknown *pUsgProbe, ULONG *reserved);
    //virtual HRESULT __stdcall raw_OnBeamformerArrive(IUnknown *pUsgBeamformer, ULONG *reserved);
    //virtual HRESULT __stdcall raw_OnProbeRemove(IUnknown *pUsgProbe, ULONG *reserved);
    //virtual HRESULT __stdcall raw_OnBeamformerRemove(IUnknown *pUsgBeamformer, ULONG *reserved);
    //virtual HRESULT __stdcall raw_OnProbeStateChanged(IUnknown *pUsgProbe, ULONG *reserved) { return S_OK; };
    //virtual HRESULT __stdcall raw_OnBeamformerStateChanged(IUnknown *pUsgBeamformer, ULONG *reserved) { return S_OK; };

    //// Methods implemented for IUnknown (necessary for IUsgDeviceChangeSink)
    //STDMETHODIMP_(ULONG) AddRef();
    //STDMETHODIMP_(ULONG) Release();
    //STDMETHODIMP QueryInterface(REFIID riid, void** ppv);

    //// Methods implemented for IDispatch (necessary for IUsgDeviceChangeSink)
    //virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT *pctinfo);
    //virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT itinfo, LCID lcid, ITypeInfo** pptinfo);
    //virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(const IID &riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid);
    //virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, const IID &riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

  protected:
    /**
      * Constructs a mitk::USTerasonDevice object by given manufacturer
      * and model string. These strings are just for labeling the device
      * in the micro service.
      *
      * Control interfaces and image source are available directly after
      * construction. Registration at the micro service happens not before
      * initialization method was called.
      */
    USTerasonDevice(std::string manufacturer, std::string model);
    virtual ~USTerasonDevice();

    void ReleaseUsgControls( );

    void ConnectDeviceChangeSink( );

    /**
      * \brief Stop ultrasound scanning by Terason API call.
      *
      * \throw mitk::Exception if API call returned with an error
      */
    void StopScanning( );

    //USTerasonProbesControls::Pointer    m_ControlsProbes;
    //USTerasonBModeControls::Pointer     m_ControlsBMode;
    //USTerasonDopplerControls::Pointer   m_ControlsDoppler;

    USEngine_C                          m_USEngine;

    USTerasonImageSource::Pointer       m_ImageSource;

    //Usgfw2Lib::IUsgfw2*                 m_UsgMainInterface;
    //Usgfw2Lib::IProbe*                  m_Probe;
    //Usgfw2Lib::IUsgDataView*            m_UsgDataView;
    //Usgfw2Lib::IUsgCollection*          m_ProbesCollection;

    //ULONG                               m_RefCount;
    //IConnectionPoint*                   m_UsgDeviceChangeCpnt;
    //DWORD                               m_UsgDeviceChangeCpntCookie;
  };
} // namespace mitk

#endif // MITKUSTerasonDevice_H_HEADER_INCLUDED_