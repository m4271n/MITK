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
#include "mitkTouchEvent.h"

mitk::TouchEvent::TouchEvent(mitk::BaseRenderer* baseRenderer,
   const std::list<Point2D>& touchPointPositions,
   const std::list<TouchPointState>& touchPointStates,
   TouchEventType eventType,
   TouchDeviceType devType)
: InteractionEvent(baseRenderer)
, m_DeviceType(devType)
, m_EventType(eventType)
, m_TouchPointPositions( touchPointPositions)
, m_TouchPointStates( touchPointStates)
{
}

mitk::InteractionEvent::TouchEventType  mitk::TouchEvent::GetEventType() const
{
   return m_EventType;
}

mitk::InteractionEvent::TouchDeviceType  mitk::TouchEvent::GetDeviceType() const
{
   return m_DeviceType;
}

void  mitk::TouchEvent::SetEventType(mitk::InteractionEvent::TouchEventType type)
{
   m_EventType = type;
}

void  mitk::TouchEvent::SetDeviceType(mitk::InteractionEvent::TouchDeviceType devType)
{
   m_DeviceType = devType;
}

//mitk::InteractionEvent::TouchPointState  mitk::TouchEvent::GetTouchPointState() const
//{
//   mitk::InteractionEvent::TouchPointState state;
//
//   std::list<TouchPointState>::const_iterator tpIt = m_TouchPointStates.begin();
//   std::list<TouchPointState>::const_iterator tpItEnd = m_TouchPointStates.end();
//   for (; tpIt != tpItEnd; tpIt++)
//   {
//      state = state | *tpIt
//   }
//}

void  mitk::TouchEvent::GetTouchPointStates(std::list<mitk::InteractionEvent::TouchPointState>& states) const
{
   states = m_TouchPointStates;
}

void  mitk::TouchEvent::SetTouchPointStates(const std::list<mitk::InteractionEvent::TouchPointState>& tpStates)
{
   //m_TouchPointStates = tpStates;
}

const std::list<mitk::Point2D>&  mitk::TouchEvent::GetTouchPointPositionsOnScreen() const
{
   return m_TouchPointPositions;
}

mitk::TouchEvent::~TouchEvent()
{
}

bool mitk::TouchEvent::IsEqual(const mitk::InteractionEvent& interactionEvent) const
{
  const mitk::TouchEvent& mpe = static_cast<const mitk::TouchEvent&>(interactionEvent);
  return (this->GetDeviceType() == mpe.GetDeviceType() &&
          this->GetEventType() == mpe.GetEventType() &&
          Superclass::IsEqual(interactionEvent));
}

bool mitk::TouchEvent::IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const
{
  return (dynamic_cast<TouchEvent*>(baseClass.GetPointer()) != NULL) ;
}
