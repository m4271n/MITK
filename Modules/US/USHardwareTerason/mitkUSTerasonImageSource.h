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

#ifndef MITKUSTerasonImageSource_H_HEADER_INCLUDED_
#define MITKUSTerasonImageSource_H_HEADER_INCLUDED_

#include "mitkUSImageSource.h"
//#include "mitkUSTerasonSDKHeader.h"
//#include "mitkUSTerasonScanConverterPlugin.h"
#include "us_engine.h"
#include "MitkUSHardwareTerasonExports.h"

#include "itkFastMutexLock.h"

namespace mitk {
/**
  * \brief Implementation of mitk::USImageSource for Terason API devices.
  * The method mitk::USImageSource::GetNextRawImage() is implemented for
  * getting images from the Terason API.
  *
  * A method for connecting this ImageSource to the Terason API is
  * implemented (mitk::USTerasonImageSource::CreateAndConnectConverterPlugin()).
  * This method is available for being used by mitk::USTerasonDevice.
  */
class MITKUSHARDWARETERASON_EXPORT USTerasonImageSource : public USImageSource
{
public:
  mitkClassMacro(USTerasonImageSource, USImageSource);
  itkFactorylessNewMacro(Self)
  itkCloneMacro(Self)

  /**
    * Implementation of the superclass method. Returns the pointer
    * to the mitk::Image filled by Terason API callback.
    */
  virtual void GetNextRawImage( mitk::Image::Pointer& );

  mitk::Image::Pointer ConstructImageMessage(USImageData_C* img);

  ///**
  //  * Updates the geometry of the current image from the API.
  //  * Is used internally when a new image is initialized, but
  //  * also needs to be called if, e.g., the zoom factor is changed.
  //  */
  void UpdateImageGeometry(USImageData_C* img);

  ///**
  //  * \brief Connect this object to the Terason API.
  //  * This method is for being used by mitk::USTerasonDevice.
  //  */
  //bool CreateAndConnectConverterPlugin( Usgfw2Lib::IUsgDataView*, Usgfw2Lib::tagScanMode );

  void SetEngine(USEngine_C* engine) { m_USEngine = engine;};

protected:
  USTerasonImageSource( );
  virtual ~USTerasonImageSource( );

  //Usgfw2Lib::IUsgScanConverterPlugin*         m_Plugin;
  //USTerasonScanConverterPlugin*               m_PluginCallback;

  //Usgfw2Lib::IUsgDataView* m_UsgDataView;  // main SDK object for comminucating with the Terason API

  ////API objects for communication, used to get the right geometry
  //Usgfw2Lib::IUsgImageProperties* m_ImageProperties;
  //Usgfw2Lib::IUsgDepth* m_DepthProperties;
  //double m_OldDepth;
  //int m_upDateCounter;

  USEngine_C*                                 m_USEngine;


  mitk::Image::Pointer                        m_Image;
  itk::FastMutexLock::Pointer                 m_ImageMutex;
};
} // namespace mitk

#endif // MITKUSTerasonImageSource_H
