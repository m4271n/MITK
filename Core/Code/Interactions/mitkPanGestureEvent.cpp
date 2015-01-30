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

#include "mitkException.h"
#include "mitkPanGestureEvent.h"

mitk::PanGestureEvent::PanGestureEvent(mitk::BaseRenderer* baseRenderer, EventState state)
: GestureEvent(baseRenderer, state)
{
  m_Offset.Fill(0.0);
  m_LastOffset.Fill(0.0);
}

mitk::PanGestureEvent::~PanGestureEvent()
{
}

bool mitk::PanGestureEvent::IsEqual(const mitk::InteractionEvent& interactionEvent) const
{
  const mitk::PanGestureEvent& mpe = static_cast<const mitk::PanGestureEvent&>(interactionEvent);
  return Superclass::IsEqual(interactionEvent);
}

bool mitk::PanGestureEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<PanGestureEvent*>(baseClass.GetPointer()) != NULL) ;
}
