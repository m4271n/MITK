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
#include "mitkGestureEvent.h"

mitk::GestureEvent::GestureEvent(
  mitk::BaseRenderer* baseRenderer
//, GestureRating rating
, EventState state
, GestureType type)
: InteractionEvent(baseRenderer)
//, m_GestureRating(rating)
, m_GestureState(state)
, m_Type(type)
{
  m_Offset.Fill(0.0);
  m_LastOffset.Fill(0.0);
  m_Hotspot.Fill(0.0);
}

bool mitk::GestureEvent::IsEqual(const mitk::InteractionEvent& interactionEvent) const
{
  const mitk::GestureEvent& mpe = static_cast<const mitk::GestureEvent&>(interactionEvent);
  return (this->GetType() == mpe.GetType() &&
          this->GetGestureState() == mpe.GetGestureState() &&
          Superclass::IsEqual(interactionEvent));
}

bool mitk::GestureEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<GestureEvent*>(baseClass.GetPointer()) != NULL) ;
}

bool mitk::operator==(const GestureEvent& a, const GestureEvent& b)
{
  return (typeid(a) == typeid(b) && a.IsEqual(b));
}

bool mitk::operator!=(const GestureEvent& a, const GestureEvent& b)
{
  return !(a == b);
}

bool mitk::GestureBeginEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<GestureBeginEvent*>(baseClass.GetPointer()) != NULL);
}

bool mitk::operator==(const GestureBeginEvent& a, const GestureBeginEvent& b)
{
  return (typeid(a) == typeid(b) && a.IsEqual(b));
}

bool mitk::operator!=(const GestureBeginEvent& a, const GestureBeginEvent& b)
{
  return !(a == b);
}

bool mitk::GestureUpdateEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<GestureUpdateEvent*>(baseClass.GetPointer()) != NULL);
}

bool mitk::operator==(const GestureUpdateEvent& a, const GestureUpdateEvent& b)
{
  return (typeid(a) == typeid(b) && a.IsEqual(b));
}

bool mitk::operator!=(const GestureUpdateEvent& a, const GestureUpdateEvent& b)
{
  return !(a == b);
}

bool mitk::GestureEndEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<GestureEndEvent*>(baseClass.GetPointer()) != NULL);
}

bool mitk::operator==(const GestureEndEvent& a, const GestureEndEvent& b)
{
  return (typeid(a) == typeid(b) && a.IsEqual(b));
}

bool mitk::operator!=(const GestureEndEvent& a, const GestureEndEvent& b)
{
  return !(a == b);
}
