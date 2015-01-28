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
, GestureRating rating
, EventState state)
: InteractionEvent(baseRenderer)
, m_GestureRating(rating)
, m_GestureState(state)
{
}

mitk::GestureEvent::GestureRating  mitk::GestureEvent::GetGestureRating() const
{
   return m_GestureRating;
}

void  mitk::GestureEvent::SetGestureRating(mitk::GestureEvent::GestureRating state)
{
  m_GestureRating = state;
}

mitk::GestureEvent::~GestureEvent()
{
}

bool mitk::GestureEvent::IsEqual(const mitk::InteractionEvent& interactionEvent) const
{
  const mitk::GestureEvent& mpe = static_cast<const mitk::GestureEvent&>(interactionEvent);
  return (this->GetGestureRating() == mpe.GetGestureRating() &&
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
