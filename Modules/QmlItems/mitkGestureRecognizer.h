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

#ifndef MITKGestureRecognizer_h
#define MITKGestureRecognizer_h

#include "MitkQmlItemsExports.h"
#include "mitkCommon.h"
#include <list>
#include "mitkGestureEvent.h"

namespace mitk
{
  /**
  *  \brief
  */
  class MitkQmlItems_EXPORT GestureRecognizer : public itk::Object
  {

  public:
    mitkClassMacro(GestureRecognizer, itk::Object);
    //mitkNewMacro1Param(Self, BaseRenderer*)
    //itkFactorylessNewMacro(Self)
    itkGetMacro(Event, GestureEvent::Pointer);

    /**
    * The state of the gesture
    */
    enum GestureState
    {
      Ignore = 0x0001,

      MayBeGesture = 0x0002,
      TriggerGesture = 0x0004,
      FinishGesture = 0x0008,
      CancelGesture = 0x0010,

      NoGesture = 0x0020,

      ResultState_Mask = 0x00ff,

      ConsumeEventHint = 0x0100,

      ResultHint_Mask = 0xff00
    };

    void SetBaseRenderer(BaseRenderer* ren){ m_Renderer = ren; };

    GestureEvent::Pointer Create(BaseRenderer* ren,
      GestureEvent::EventState state, GestureEvent::GestureType type)
    {
      switch (state)
      {
      case mitk::InteractionEvent::Begin:
        return m_Event = static_cast<mitk::GestureEvent*>(
          mitk::GestureBeginEvent::New(ren, state, type));
        break;
      case mitk::InteractionEvent::Update:
        return m_Event = static_cast<mitk::GestureEvent*>(
          mitk::GestureUpdateEvent::New(ren, state, type));
        break;
      case mitk::InteractionEvent::End:
        return m_Event = static_cast<mitk::GestureEvent*>(
          mitk::GestureEndEvent::New(ren, state, type));
        break;
      default:
        return m_Event = NULL;
        break;
      }
    }
    virtual void Recognize(InteractionEvent* e, GestureState& s) = 0;
    virtual void Reset() = 0;

  protected:
    GestureRecognizer(BaseRenderer* ren){ m_Renderer = ren; };
    virtual ~GestureRecognizer(){};

    BaseRenderer* m_Renderer;
    GestureEvent::Pointer m_Event;
  };

}

#endif
