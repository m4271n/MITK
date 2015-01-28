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


mitk::PanGestureRecognizer::PanGestureRecognizer()
: GestureRecognizer()
{
}

mitk::PanGestureRecognizer::~PanGestureRecognizer()
{

}

mitk::GestureEvent::Pointer
mitk::PanGestureRecognizer::Create(mitk::BaseRenderer* renderer)
{
  return static_cast<mitk::GestureEvent*>(
    mitk::PanGestureEvent::New(renderer, mitk::GestureEvent::Ignore));
}

mitk::GestureEvent::GestureRating
mitk::PanGestureRecognizer::Recognize(mitk::InteractionEvent* e, mitk::GestureEvent* g)
{
  mitk::GestureEvent::GestureRating result;

  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);
  mitk::PanGestureEvent* pg = dynamic_cast<mitk::PanGestureEvent*>(g);

  if (te == NULL)
  {
    return mitk::GestureEvent::Ignore;
  }

  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();

  switch (te->GetEventState())
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
    pg->SetLastOffset(m_Offset);
    pg->SetOffset(m_Offset);

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

        pg->SetLastOffset(m_Offset);
        m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
        m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
        pg->SetOffset(m_Offset);
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

      pg->SetLastOffset(m_Offset);
      m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
      m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
      pg->SetOffset(m_Offset);
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
  return result;
}

void mitk::PanGestureRecognizer::Reset()
{

}
