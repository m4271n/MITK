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

#include "mitkUSTerasonImageSource.h"
//#include "mitkUSTerasonSDKHeader.h"
//#include "MITKUSTerasonScanConverterPlugin.h"
#include "mitkImageReadAccessor.h"

mitk::USTerasonImageSource::USTerasonImageSource(  )
  : m_Image(mitk::Image::New()),
    m_ImageMutex(itk::FastMutexLock::New())
{
}

mitk::USTerasonImageSource::~USTerasonImageSource( )
{
}

void mitk::USTerasonImageSource::GetNextRawImage( mitk::Image::Pointer& image)
{
  if ( image.IsNull() ) { image = mitk::Image::New(); }

  USImageData_C* imageData = nullptr;
  if (!m_USEngine->GetRawImageData(imageData))
     return;

  ////if depth changed: update geometry, but 20 frames after the change because it takes this time until geometry adapts
  //if (m_upDateCounter==20 && m_Image->IsInitialized())
  //  {
    UpdateImageGeometry(imageData);
  //  m_upDateCounter++;
  //  }
  //if (m_OldDepth != m_DepthProperties->Current)
  //  {
  //  m_upDateCounter=0;
  //  m_OldDepth =  m_DepthProperties->GetCurrent();
  //  }
  //if (m_upDateCounter<20) m_upDateCounter++;

  //now update image
  if ( m_Image->IsInitialized() )
  {
    m_ImageMutex->Lock();

    // copy contents of the given image into the member variable
    image->Initialize(m_Image->GetPixelType(), m_Image->GetDimension(), m_Image->GetDimensions());
    mitk::ImageReadAccessor inputReadAccessor(m_Image, m_Image->GetSliceData(0,0,0));
    image->SetSlice(inputReadAccessor.GetData());
    image->SetGeometry(m_Image->GetGeometry());

    m_ImageMutex->Unlock();
  }

}

void mitk::USTerasonImageSource::UpdateImageGeometry(USImageData_C* img)
{
   //get the image itself
   const QImage* image = img->GetImage();
   if (image->isNull())
   {
      return;
   }

   mitk::Vector3D spacing;
   spacing[0] = img->GetXPixSize(); 
   spacing[1] = img->GetYPixSize();
   spacing[2] = 1;

   m_ImageMutex->Lock();

   // initialize mitk::Image with given image size on the first time
   if ( ! m_Image->IsInitialized() )
   {
      unsigned int dim[] = {image->width(), image->height()}; // image dimensions

      m_Image->Initialize(mitk::MakeScalarPixelType<BYTE>(), 2, dim);
   }

   // lock the image for writing an copy the given buffer into the image then
   m_Image->SetSlice(image->bits());

   if(m_Image.IsNotNull() && (m_Image->GetGeometry()!=NULL))
   {
      m_Image->GetGeometry()->SetSpacing(spacing);
      m_Image->GetGeometry()->Modified();
   }
   else
   {MITK_WARN << "image or geometry was NULL, can't adapt geometry";}
   m_ImageMutex->Unlock();

  //m_Image->SetRealTimeStamp(itk::RealTimeStamp((unsigned int)img->GetTimestamp(), (unsigned int)(img->GetTimestamp() * 1000.0) % 1000));
}

//void mitk::USTerasonImageSource::UpdateImageGeometry()
//{
//  Usgfw2Lib::tagPixelsOrigin origin = Usgfw2Lib::tagPixelsOrigin();
//  Usgfw2Lib::tagImageResolution resolutionInMeters;
//  m_ImageProperties->GetResolution(&resolutionInMeters,0);
//
//  mitk::Vector3D spacing;
//  spacing[0] = ((double)1 / resolutionInMeters.nXPelsPerUnit) * 1000; //conversion: meters to millimeters
//  spacing[1] = ((double)1 / resolutionInMeters.nXPelsPerUnit) * 1000; //conversion: meters to millimeters
//  spacing[2] = 1;
//
//  m_ImageMutex->Lock();
//  if(m_Image.IsNotNull() && (m_Image->GetGeometry()!=NULL))
//    {
//    m_Image->GetGeometry()->SetSpacing(spacing);
//    m_Image->GetGeometry()->Modified();
//    }
//  else
//    {MITK_WARN << "image or geometry was NULL, can't adapt geometry";}
//  m_ImageMutex->Unlock();
//
//  MITK_DEBUG << "UpdateImageGeometry called!";
//  MITK_DEBUG << "depth: " << m_DepthProperties->GetCurrent();
//  MITK_DEBUG << "new spacing: " << spacing;
//}

//bool mitk::USTerasonImageSource::CreateAndConnectConverterPlugin(Usgfw2Lib::IUsgDataView* usgDataView, Usgfw2Lib::tagScanMode scanMode)
//{
//   IUnknown* tmp_obj = NULL;
//
//   // create control object from Terason API
//   mitk::terason::CreateUsgControl( usgDataView, Usgfw2Lib::IID_IUsgScanConverterPlugin, scanMode, 0, (void**)&tmp_obj );
//   if ( ! tmp_obj )
//   {
//      MITK_ERROR("USImageSource")("USTerasonImageSource") << "Could not create scan converter plugin.";
//      return false;
//   }
//
//   // create the callback object for the scan conversion
//   if ( ! m_PluginCallback )
//   {
//      m_PluginCallback = new USTerasonScanConverterPlugin();
//
//      // current image buffer should be copied to m_Image at every callback
//      m_PluginCallback->SetOutputImage(m_Image.GetPointer(), m_ImageMutex);
//   }
//   else
//   {
//      // make sure that the scan converter plugin is not set
//      // to the plugin callback any longer
//      m_PluginCallback->SetScanConverterPlugin(0);
//   }
//
//   // now the ScanConverterPlugin can be created and set as plugin
//   SAFE_RELEASE(m_Plugin);
//   m_Plugin = (Usgfw2Lib::IUsgScanConverterPlugin*)tmp_obj;
//   m_PluginCallback->SetScanConverterPlugin(m_Plugin);
//
//   //last: create some connections which are needed inside this class for communication with the terason device
//   m_UsgDataView = usgDataView;
//
//   // create terason controls
//   if (!m_DepthProperties) {CREATE_TerasonControl(m_DepthProperties, m_UsgDataView, Usgfw2Lib::IID_IUsgDepth, Usgfw2Lib::IUsgDepth, Usgfw2Lib::SCAN_MODE_B);}
//   if (!m_ImageProperties) {CREATE_TerasonControl(m_ImageProperties, m_UsgDataView, Usgfw2Lib::IID_IUsgImageProperties, Usgfw2Lib::IUsgImageProperties, Usgfw2Lib::SCAN_MODE_B);}
//
//   return true;
//}