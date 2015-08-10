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

#include "mitkUSTerasonActivator.h"

mitk::USTerasonActivator::USTerasonActivator()
{
  MITK_INFO << "USTerasonActivator created";
}

mitk::USTerasonActivator::~USTerasonActivator()
{
}

void mitk::USTerasonActivator::Load(us::ModuleContext* context)
{
  // create a new device
  m_Device = mitk::USTerasonDevice::New("Terason", "Ultrasound System");
  m_Device->Initialize();
}

void mitk::USTerasonActivator::Unload(us::ModuleContext* context)
{
  // set smart pointer to null (device will be unregistered from
  // micro service in it's destrcutor)
  m_Device = 0;
}