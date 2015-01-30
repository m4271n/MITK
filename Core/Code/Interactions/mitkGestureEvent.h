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
    * The type of gesture
    */
    enum GestureType
    {
      Tap             = 0x0001,
      DoubleTap       = 0x0002,
      LongPress       = 0x0004,
      Scroll          = 0x0008,
      Pan             = 0x0010,
      Flick           = 0x0020,
      TwoFingerTap    = 0x0040,
      TwoFingerScroll = 0x0080,
      TwoFingerPan    = 0x0100,
      PinchZoom       = 0x0200,
      Rotate          = 0x0400,
      Undefined       = 0x0800,
      Custom          = 0x1000 //in case someone wants to create another one
    };

    mitkClassMacro(GestureEvent, InteractionEvent)
    mitkNewMacro1Param(Self, BaseRenderer*)
    mitkNewMacro2Param(Self, BaseRenderer*, EventState)
    mitkNewMacro3Param(Self, BaseRenderer*, EventState, GestureType)
    //mitkNewMacro2Param(Self, BaseRenderer*, GestureRating)
    //mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)
    //mitkNewMacro4Param(Self, BaseRenderer*, GestureRating, EventState, GestureType)
    itkGetConstMacro(GestureState, EventState);
    //itkGetConstMacro(GestureRating, GestureRating);
    itkGetConstMacro(Type, GestureType);

    //void SetGestureRating(GestureRating rating){ m_GestureRating = rating; };
    void SetGestureState(EventState state){ m_GestureState = state; };
    void SetType(GestureType type){ m_Type = type; };

    itkGetConstMacro(PointerPosition, Point2D);
    itkGetConstMacro(Offset, Point2D);
    itkGetConstMacro(LastOffset, Point2D);
    itkGetConstMacro(Hotspot, Point2D);
    void SetOffset(Point2D offset) { m_Offset = offset; };
    void SetLastOffset(Point2D offset) { m_LastOffset = offset; };
    void SetHotspot(Point2D hs) { m_Hotspot = hs; };
    void SetPointerPosition(Point2D p) { m_PointerPosition = p; };


    itkGetConstMacro(Scale, double);
    itkGetConstMacro(LastScale, double);
    itkGetConstMacro(TotalScale, double);
    itkGetConstMacro(StartCenterPosition, Point2D);
    void SetScale(double s) { m_Scale = s; };
    void SetLastScale(double s) { m_LastScale = s; };
    void SetTotalScale(double s) { m_TotalScale = s; };
    void SetStartCenterPosition(Point2D c) { m_StartCenterPosition = c; };

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    GestureEvent(BaseRenderer*, /*GestureRating rating = Ignore, */EventState state = Begin, GestureType type = Undefined);
    virtual ~GestureEvent(){};

    friend MITK_CORE_EXPORT bool operator==(const GestureEvent&, const GestureEvent&);
    virtual bool IsEqual(const InteractionEvent&) const;

  private:
    //GestureRating m_GestureRating;
    GestureType m_Type;
    InteractionEvent::EventState m_GestureState;  

    Point2D m_Offset;
    Point2D m_LastOffset;
    Point2D m_Hotspot;

    double m_Scale;
    double m_LastScale;
    double m_TotalScale;
    Point2D m_StartCenterPosition;

    Point2D m_PointerPosition;

  };

  class MITK_CORE_EXPORT GestureBeginEvent : public GestureEvent
  {
  public:
    mitkClassMacro(GestureBeginEvent, GestureEvent);
    mitkNewMacro2Param(Self, BaseRenderer*, EventState)
    mitkNewMacro3Param(Self, BaseRenderer*, EventState, GestureType);
    //mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)
    //mitkNewMacro4Param(Self, BaseRenderer*, GestureRating, EventState, GestureType);

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    GestureBeginEvent(BaseRenderer* ren,
      //GestureRating rating = Ignore,
      EventState state = Begin,
      GestureType type = Undefined) :
      GestureEvent(ren, /*rating, */state, type){};
    virtual ~GestureBeginEvent(){};

    friend MITK_CORE_EXPORT bool operator==(const GestureBeginEvent&, const GestureBeginEvent&);
    //virtual bool IsEqual(const InteractionEvent&) const;
  };

  class MITK_CORE_EXPORT GestureUpdateEvent : public GestureEvent
  {
  public:
    mitkClassMacro(GestureUpdateEvent, GestureEvent);
    mitkNewMacro2Param(Self, BaseRenderer*, EventState)
      mitkNewMacro3Param(Self, BaseRenderer*, EventState, GestureType);
    //mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)
    //mitkNewMacro4Param(Self, BaseRenderer*, GestureRating, EventState, GestureType);

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    GestureUpdateEvent(BaseRenderer* ren,
      EventState state = Begin,
      GestureType type = Undefined) :
      GestureEvent(ren, state, type){};
    virtual ~GestureUpdateEvent(){};

    friend MITK_CORE_EXPORT bool operator==(const GestureUpdateEvent&, const GestureUpdateEvent&);
    //virtual bool IsEqual(const InteractionEvent&) const;
  };

  class MITK_CORE_EXPORT GestureEndEvent : public GestureEvent
  {
  public:
    mitkClassMacro(GestureEndEvent, GestureEvent);
    mitkNewMacro2Param(Self, BaseRenderer*, EventState)
      mitkNewMacro3Param(Self, BaseRenderer*, EventState, GestureType);
    //mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)
    //mitkNewMacro4Param(Self, BaseRenderer*, GestureRating, EventState, GestureType);

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    GestureEndEvent(BaseRenderer* ren,
      EventState state = Begin,
      GestureType type = Undefined) :
      GestureEvent(ren, state, type){};
    virtual ~GestureEndEvent(){};

    friend MITK_CORE_EXPORT bool operator==(const GestureEndEvent&, const GestureEndEvent&);
    //virtual bool IsEqual(const InteractionEvent&) const;
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
  MITK_CORE_EXPORT bool operator==(const GestureBeginEvent& a, const GestureBeginEvent& b);
  MITK_CORE_EXPORT bool operator!=(const GestureBeginEvent& a, const GestureBeginEvent& b);
  MITK_CORE_EXPORT bool operator==(const GestureUpdateEvent& a, const GestureUpdateEvent& b);
  MITK_CORE_EXPORT bool operator!=(const GestureUpdateEvent& a, const GestureUpdateEvent& b);
  MITK_CORE_EXPORT bool operator==(const GestureEndEvent& a, const GestureEndEvent& b);
  MITK_CORE_EXPORT bool operator!=(const GestureEndEvent& a, const GestureEndEvent& b);

  ///*
  //* Allow bitwise OR operation on enums.
  //*/
  //inline GestureEvent::GestureRating operator|(GestureEvent::GestureRating a, GestureEvent::GestureRating b)
  //{
  //  return static_cast<GestureEvent::GestureRating>(static_cast<int>(a) | static_cast<int>(b));
  //}

  //inline GestureEvent::GestureRating& operator|=(GestureEvent::GestureRating& a, GestureEvent::GestureRating& b)
  //{
  //  a = static_cast<GestureEvent::GestureRating>(static_cast<int>(a) | static_cast<int>(b));
  //  return a;
  //}
} /* namespace mitk */

#endif /* MITKGestureEvent_H_ */
