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

#include "mitkStandardGestureRecognizer.h"
#include "mitkTouchEvent.h"
#include "mitkPanGestureEvent.h"

mitk::TapGestureRecognizer::TapGestureRecognizer(BaseRenderer* ren)
  : GestureRecognizer(ren)
{
}

mitk::TapGestureRecognizer::~TapGestureRecognizer()
{

}

mitk::GestureEvent::Pointer
mitk::TapGestureRecognizer::Recognize(mitk::InteractionEvent* e, GestureState& state)
{
  //mitk::GestureEvent::GestureRating result = mitk::GestureEvent::CancelGesture;

  //todo add a check for touch type
  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);

  if (te == NULL)
  {
    return NULL;
  }

  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();
  mitk::InteractionEvent::EventState gestureState = te->GetEventState();
  mitk::GestureEvent::Pointer g =
    this->Create(m_Renderer, gestureState, mitk::GestureEvent::Tap);

  switch (gestureState)
  {
  case mitk::InteractionEvent::EventState::Begin:
  {
    if (touchPoints.size() == 1)
    {
      result = mitk::GestureRecognizer::TriggerGesture;
      m_StartPos = touchPoints.front();
      g->SetHotspot(m_StartPos);
    }
    break;
  }
  case mitk::InteractionEvent::EventState::Update:
  {
    double delta =
      (m_StartPos[0] - touchPoints.front()[0]) + (m_StartPos[1] - touchPoints.front()[1]);
    if (delta <= /*tapradius*/ 40)
    {
      result = mitk::GestureRecognizer::TriggerGesture;
    }
    break;
  }
  case mitk::InteractionEvent::EventState::End:
  {
    double delta =
      (m_StartPos[0] - touchPoints.front()[0]) + (m_StartPos[1] - touchPoints.front()[1]);
    if (delta <= /*tapradius*/ 40)
    {
      result = mitk::GestureRecognizer::FinishGesture;
    }
    break;
  }
  default:
    result = mitk::GestureRecognizer::Ignore;
    break;
  }
  g->SetGestureRating(result);
  g->SetGestureState(te->GetEventState());
  return g;
}

void mitk::TapGestureRecognizer::Reset()
{

}

mitk::PanGestureRecognizer::PanGestureRecognizer(BaseRenderer* ren)
: GestureRecognizer(ren)
{
}

mitk::PanGestureRecognizer::~PanGestureRecognizer()
{

}

mitk::GestureEvent::Pointer
mitk::PanGestureRecognizer::Recognize(mitk::InteractionEvent* e)
{
  mitk::GestureEvent::GestureRating result;

  //todo add a check for touch type
  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);
  //mitk::PanGestureEvent* pg = dynamic_cast<mitk::PanGestureEvent*>(g.GetPointer);

  if (te == NULL)
  {
    return NULL;
  }

  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();
  mitk::InteractionEvent::EventState gestureState = te->GetEventState();
  mitk::GestureEvent::Pointer g = 
    this->Create(m_Renderer, gestureState, mitk::GestureEvent::TwoFingerPan);

  switch (gestureState)
  {
  case mitk::InteractionEvent::EventState::Begin:
  {
    result = mitk::GestureEvent::MayBeGesture;

    if (touchPoints.size() == 2)
    {
      m_StartPos_TP1 = touchPoints.front();
      touchPoints.pop_front();
      m_StartPos_TP2 = touchPoints.front();
      touchPoints.pop_front();
    }
    else
    {
      m_StartPos_TP1.Fill(0);
      m_StartPos_TP2.Fill(0);
    }

    m_Offset.Fill(0.0);
    m_LastOffset.Fill(0.0);
    g->SetLastOffset(m_Offset);
    g->SetOffset(m_Offset);

    break;
  }
  case mitk::InteractionEvent::EventState::End:
  {
    //if (q->state() != Qt::NoGesture)
    {
      if (touchPoints.size() == 2)
      {
        mitk::Point2D p1 = touchPoints.front();
        touchPoints.pop_front();
        mitk::Point2D p2 = touchPoints.front();
        touchPoints.pop_front();

        g->SetLastOffset(m_Offset);
        m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
        m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
        g->SetOffset(m_Offset);
      }
      result = mitk::GestureEvent::FinishGesture;
    }
    //else {
    //  result = mitk::GestureEvent::CancelGesture;
    //}
    break;
  }
  case mitk::InteractionEvent::EventState::Update:
  {
    if (touchPoints.size() >= 2)
    {
      mitk::Point2D p1 = touchPoints.front();
      touchPoints.pop_front();
      mitk::Point2D p2 = touchPoints.front();
      touchPoints.pop_front();

      g->SetLastOffset(m_Offset);
      m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
      m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
      g->SetOffset(m_Offset);
      if (m_Offset[0] > 10 || m_Offset[1] > 10 || m_Offset[0] < -10 || m_Offset[1] < -10)
      {
        result = mitk::GestureEvent::TriggerGesture;
      }
      else
      {
        result = mitk::GestureEvent::MayBeGesture;
      }
    }
    else
    {
      result = mitk::GestureEvent::Ignore;
    }
    break;
  }
  default:
    result = mitk::GestureEvent::Ignore;
    break;
  }
  g->SetGestureRating(result);
  g->SetGestureState(te->GetEventState());
  return g;
}

void mitk::PanGestureRecognizer::Reset()
{

}
