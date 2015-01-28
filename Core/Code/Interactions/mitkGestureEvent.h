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

#ifndef MITKGestureEvent_H_
#define MITKGestureEvent_H_

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "mitkCommon.h"
#include "mitkInteractionEventConst.h"
#include "mitkInteractionEvent.h"
#include "mitkBaseRenderer.h"
#include "mitkInteractionEvent.h"

#include <MitkCoreExports.h>

namespace mitk
{

  class MITK_CORE_EXPORT GestureEvent: public InteractionEvent
  {
  public:
    /**
    * The state of the gesture
    */
    enum GestureRating
    {
      Ignore = 0x0001,

      MayBeGesture = 0x0002,
      TriggerGesture = 0x0004,
      FinishGesture = 0x0008,
      CancelGesture = 0x0010,

      ResultState_Mask = 0x00ff,

      ConsumeEventHint = 0x0100,

      ResultHint_Mask = 0xff00
    };

    mitkClassMacro(GestureEvent, InteractionEvent)
    mitkNewMacro2Param(Self, BaseRenderer*, GestureRating)
    mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)
    itkGetMacro(GestureState, EventState);

    GestureRating GetGestureRating() const;
    void SetGestureRating(GestureRating rating);
    void SetGestureState(EventState state){ m_GestureState = state; };

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    GestureEvent(BaseRenderer*, GestureRating rating = Ignore, EventState state = Begin);
    virtual ~GestureEvent();

    friend MITK_CORE_EXPORT bool operator==(const GestureEvent&, const GestureEvent&);
    virtual bool IsEqual(const InteractionEvent&) const;

  private:
    GestureRating m_GestureRating;
    InteractionEvent::EventState m_GestureState;
  };

  /**
  * Implementation of equality for event classes.
  * Equality does \b not mean an exact copy or pointer equality.
  *
  * A match is determined by agreement in all attributes that are necessary to describe
  * the event for a state machine transition.
  * E.g. for a mouse event press event, it is important which modifiers are used,
  * which mouse button was used to triggered the event, but the mouse position is irrelevant.
  */
  MITK_CORE_EXPORT bool operator==(const GestureEvent& a, const GestureEvent& b);
  MITK_CORE_EXPORT bool operator!=(const GestureEvent& a, const GestureEvent& b);

  /*
  * Allow bitwise OR operation on enums.
  */
  inline GestureEvent::GestureRating operator|(GestureEvent::GestureRating a, GestureEvent::GestureRating b)
  {
    return static_cast<GestureEvent::GestureRating>(static_cast<int>(a) | static_cast<int>(b));
  }

  inline GestureEvent::GestureRating& operator|=(GestureEvent::GestureRating& a, GestureEvent::GestureRating& b)
  {
    a = static_cast<GestureEvent::GestureRating>(static_cast<int>(a) | static_cast<int>(b));
    return a;
  }
} /* namespace mitk */

#endif /* MITKGestureEvent_H_ */
