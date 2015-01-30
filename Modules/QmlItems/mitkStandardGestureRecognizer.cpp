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

//
// PinchGestureRecognizer
//
mitk::PinchGestureRecognizer::PinchGestureRecognizer(BaseRenderer* ren)
  : GestureRecognizer(ren)
{
  this->Reset();
}
//QGesture *PinchGestureRecognizer::create(QObject *target)
//{
//  if (target && target->isWidgetType()) {
//    static_cast<QWidget *>(target)->setAttribute(Qt::WA_AcceptTouchEvents);
//  }
//  return new PinchGesture;
//}
//mitk::GestureEvent::Pointer 
void
mitk::PinchGestureRecognizer::Recognize(InteractionEvent* e, GestureState& s)
{
  //todo add a check for touch type
  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);

  if (te == NULL)
  {
    return;
  }

  //PinchGesture *q = static_cast<PinchGesture *>(state);
  //PinchGesturePrivate *d = q->d_func();
  //const QTouchEvent *ev = static_cast<const QTouchEvent *>(event);
  //QGestureRecognizer::Result result;

  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();
  mitk::InteractionEvent::EventState eventState = te->GetEventState();
  mitk::GestureEvent::Pointer g =
    this->Create(m_Renderer, eventState, mitk::GestureEvent::PinchZoom);

  switch (eventState)
  {
  case mitk::InteractionEvent::Begin:
    s = mitk::GestureRecognizer::MayBeGesture;
    break;
  case mitk::InteractionEvent::Update:
    //d->changeFlags = 0;
    cout << "numTPs: " << touchPoints.size() << endl;
    if (touchPoints.size() == 2) 
    {
      mitk::Point2D p1 = touchPoints.front();
      touchPoints.pop_front();
      mitk::Point2D p2 = touchPoints.front();
      touchPoints.pop_front();
      mitk::Point2D diffP1P2;
      diffP1P2[0] = p1[0] - p2[0];
      diffP1P2[1] = p1[1] - p2[1];

      //d->hotSpot = p1.screenPos();
      //d->isHotSpotSet = true;
      if (m_IsNewSequence) 
      {
        m_StartPosTP1 = p1;
        m_StartPosTP2 = p2;
        m_StartCenter[0] = (p1[0] + p2[0]) / 2.0;
        m_StartCenter[1] = (p1[1] + p2[1]) / 2.0;
        m_LastPosTP1 = p1;
        m_LastPosTP2 = p2;
        m_Center.Fill(0.0);
      }
      m_LastCenter = m_Center;
      m_Center[0] = (p1[0] + p2[0]) / 2.0;
      m_Center[1] = (p1[1] + p2[1]) / 2.0;
      //d->changeFlags |= PinchGesture::CenterPointChanged;
      if (m_IsNewSequence)
      {
        m_ScaleFactor = 1.0;
        m_LastScaleFactor = 1.0;
        m_TotalScaleFactor = 1.0;
      }
      else 
      {
        m_LastScaleFactor = m_ScaleFactor;
        //the scale factor is the difference between the last point difference and the current one
        double curDiff = (diffP1P2[0]) * (diffP1P2[0]) + (diffP1P2[1]) * (diffP1P2[1]);
        double lastDiff = (m_LastPosTP1[0] - m_LastPosTP2[0]) * (m_LastPosTP1[0] - m_LastPosTP2[0])
          + (m_LastPosTP1[1] - m_LastPosTP2[1]) * (m_LastPosTP1[1] - m_LastPosTP2[1]);
        //QLineF line(p1.screenPos(), p2.screenPos());
        //QLineF lastLine(p1.lastScreenPos(), p2.lastScreenPos());
        //d->scaleFactor = line.length() / lastLine.length();
        m_ScaleFactor = std::sqrt ( curDiff / lastDiff);
      }
      m_TotalScaleFactor = m_TotalScaleFactor * m_ScaleFactor;
      //d->changeFlags |= PinchGesture::ScaleFactorChanged;
      double angle = (std::abs(diffP1P2[1]) >= 10e-5) ? std::tan(diffP1P2[0] / diffP1P2[1]) : 0.0;
      angle *= 180 / 3.141;
      if (angle > 180)
      {
        angle -= 360;
      }
      mitk::Point2D diffStartP1P2;
      diffStartP1P2[0] = m_StartPosTP1[0] - m_StartPosTP2[0];
      diffStartP1P2[1] = m_StartPosTP1[1] - m_StartPosTP2[1];
      double startAngle = 
        (std::abs(diffStartP1P2[1]) >= 10e-5) ? std::tan(diffStartP1P2[0] / diffStartP1P2[1]) : 0.0;
      startAngle *= 180 / 3.141;
      if (startAngle > 180)
        startAngle -= 360;
      const double rotationAngle = startAngle - angle;
      //if the rotation angle is too big the user performed a rotation and not a pinch zoom gesture
      //so cancel the gesture in this case
      if (rotationAngle <= 30.0)
      {
        if (m_IsNewSequence)
          m_LastRotationAngle = 0.0;
        else
          m_LastRotationAngle = m_RotationAngle;
        m_RotationAngle = rotationAngle;
        m_TotalRotationAngle += m_RotationAngle - m_LastRotationAngle;
        //d->changeFlags |= PinchGesture::RotationAngleChanged;
        //d->totalChangeFlags |= d->changeFlags;
        m_IsNewSequence = false;
        s = mitk::GestureRecognizer::TriggerGesture;
        m_LastPosTP1 = p1;
        m_LastPosTP2 = p2;
      }
      else
      {
        s = mitk::GestureRecognizer::CancelGesture;
      }
    }
    else {
      m_IsNewSequence = true;
      if (s == mitk::GestureRecognizer::NoGesture)
        s = mitk::GestureRecognizer::Ignore;
      else
        //s = mitk::GestureRecognizer::CancelGesture;
        //s = mitk::GestureRecognizer::MayBeGesture;
      s = mitk::GestureRecognizer::FinishGesture;
    }
    break;
  case mitk::InteractionEvent::End:
    if (s != mitk::GestureRecognizer::NoGesture && s!= mitk::GestureRecognizer::CancelGesture) 
    {
      s = mitk::GestureRecognizer::FinishGesture;
    }
    else 
    {
      s = mitk::GestureRecognizer::CancelGesture;
    }
    break;
  default:
    break;
  }
  g->SetLastScale(m_LastScaleFactor);
  g->SetScale(m_ScaleFactor);
  g->SetTotalScale(m_TotalScaleFactor);
  g->SetStartCenterPosition(m_StartCenter);
}

void mitk::PinchGestureRecognizer::Reset()
{
  m_IsNewSequence = true;
  m_Center.Fill(0.0);
  m_LastCenter.Fill(0.0);
  m_StartPosTP1.Fill(0.0);
  m_StartPosTP2.Fill(0.0);
  m_LastPosTP1.Fill(0.0);
  m_LastPosTP2.Fill(0.0);
  m_StartCenter.Fill(0.0);
  m_ScaleFactor = 1.0;
  m_LastScaleFactor = 1.0;
  m_TotalScaleFactor = 1.0;
  m_RotationAngle = 0.0;
  m_LastRotationAngle = 0.0;
  m_TotalRotationAngle = 0.0;
}
//
//mitk::TapGestureRecognizer::TapGestureRecognizer(BaseRenderer* ren)
//  : GestureRecognizer(ren)
//{
//}
//
//mitk::TapGestureRecognizer::~TapGestureRecognizer()
//{
//
//}
//
//mitk::GestureEvent::Pointer
//mitk::TapGestureRecognizer::Recognize(mitk::InteractionEvent* e, GestureState& state)
//{
//  //mitk::GestureEvent::GestureRating result = mitk::GestureEvent::CancelGesture;
//
//  //todo add a check for touch type
//  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);
//
//  if (te == NULL)
//  {
//    return NULL;
//  }
//
//  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();
//  mitk::InteractionEvent::EventState gestureState = te->GetEventState();
//  mitk::GestureEvent::Pointer g =
//    this->Create(m_Renderer, gestureState, mitk::GestureEvent::Tap);
//
//  switch (gestureState)
//  {
//  case mitk::InteractionEvent::EventState::Begin:
//  {
//    if (touchPoints.size() == 1)
//    {
//      result = mitk::GestureRecognizer::TriggerGesture;
//      m_StartPos = touchPoints.front();
//      g->SetHotspot(m_StartPos);
//    }
//    break;
//  }
//  case mitk::InteractionEvent::EventState::Update:
//  {
//    double delta =
//      (m_StartPos[0] - touchPoints.front()[0]) + (m_StartPos[1] - touchPoints.front()[1]);
//    if (delta <= /*tapradius*/ 40)
//    {
//      result = mitk::GestureRecognizer::TriggerGesture;
//    }
//    break;
//  }
//  case mitk::InteractionEvent::EventState::End:
//  {
//    double delta =
//      (m_StartPos[0] - touchPoints.front()[0]) + (m_StartPos[1] - touchPoints.front()[1]);
//    if (delta <= /*tapradius*/ 40)
//    {
//      result = mitk::GestureRecognizer::FinishGesture;
//    }
//    break;
//  }
//  default:
//    result = mitk::GestureRecognizer::Ignore;
//    break;
//  }
//  g->SetGestureRating(result);
//  g->SetGestureState(te->GetEventState());
//  return g;
//}
//
//void mitk::TapGestureRecognizer::Reset()
//{
//
//}
//
//mitk::PanGestureRecognizer::PanGestureRecognizer(BaseRenderer* ren)
//: GestureRecognizer(ren)
//{
//}
//
//mitk::PanGestureRecognizer::~PanGestureRecognizer()
//{
//
//}
//
//mitk::GestureEvent::Pointer
//mitk::PanGestureRecognizer::Recognize(mitk::InteractionEvent* e)
//{
//  mitk::GestureEvent::GestureRating result;
//
//  //todo add a check for touch type
//  mitk::TouchEvent* te = dynamic_cast<mitk::TouchEvent*>(e);
//  //mitk::PanGestureEvent* pg = dynamic_cast<mitk::PanGestureEvent*>(g.GetPointer);
//
//  if (te == NULL)
//  {
//    return NULL;
//  }
//
//  std::list<mitk::Point2D> touchPoints = te->GetTouchPointPositionsOnScreen();
//  mitk::InteractionEvent::EventState gestureState = te->GetEventState();
//  mitk::GestureEvent::Pointer g = 
//    this->Create(m_Renderer, gestureState, mitk::GestureEvent::TwoFingerPan);
//
//  switch (gestureState)
//  {
//  case mitk::InteractionEvent::EventState::Begin:
//  {
//    result = mitk::GestureEvent::MayBeGesture;
//
//    if (touchPoints.size() == 2)
//    {
//      m_StartPos_TP1 = touchPoints.front();
//      touchPoints.pop_front();
//      m_StartPos_TP2 = touchPoints.front();
//      touchPoints.pop_front();
//    }
//    else
//    {
//      m_StartPos_TP1.Fill(0);
//      m_StartPos_TP2.Fill(0);
//    }
//
//    m_Offset.Fill(0.0);
//    m_LastOffset.Fill(0.0);
//    g->SetLastOffset(m_Offset);
//    g->SetOffset(m_Offset);
//
//    break;
//  }
//  case mitk::InteractionEvent::EventState::End:
//  {
//    //if (q->state() != Qt::NoGesture)
//    {
//      if (touchPoints.size() == 2)
//      {
//        mitk::Point2D p1 = touchPoints.front();
//        touchPoints.pop_front();
//        mitk::Point2D p2 = touchPoints.front();
//        touchPoints.pop_front();
//
//        g->SetLastOffset(m_Offset);
//        m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
//        m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
//        g->SetOffset(m_Offset);
//      }
//      result = mitk::GestureEvent::FinishGesture;
//    }
//    //else {
//    //  result = mitk::GestureEvent::CancelGesture;
//    //}
//    break;
//  }
//  case mitk::InteractionEvent::EventState::Update:
//  {
//    if (touchPoints.size() >= 2)
//    {
//      mitk::Point2D p1 = touchPoints.front();
//      touchPoints.pop_front();
//      mitk::Point2D p2 = touchPoints.front();
//      touchPoints.pop_front();
//
//      g->SetLastOffset(m_Offset);
//      m_Offset[0] = (p1[0] - m_StartPos_TP1[0] + p2[0] - m_StartPos_TP2[0]) / 2.0;
//      m_Offset[1] = (p1[1] - m_StartPos_TP1[1] + p2[1] - m_StartPos_TP2[1]) / 2.0;
//      g->SetOffset(m_Offset);
//      if (m_Offset[0] > 10 || m_Offset[1] > 10 || m_Offset[0] < -10 || m_Offset[1] < -10)
//      {
//        result = mitk::GestureEvent::TriggerGesture;
//      }
//      else
//      {
//        result = mitk::GestureEvent::MayBeGesture;
//      }
//    }
//    else
//    {
//      result = mitk::GestureEvent::Ignore;
//    }
//    break;
//  }
//  default:
//    result = mitk::GestureEvent::Ignore;
//    break;
//  }
//  g->SetGestureRating(result);
//  g->SetGestureState(te->GetEventState());
//  return g;
//}
//
//void mitk::PanGestureRecognizer::Reset()
//{
//
//}
