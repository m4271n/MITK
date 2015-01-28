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

#include "mitkGestureManager.h"
#include "mitkStandardGestureRecognizer.h"


mitk::GestureManager::GestureManager()
: itk::Object()
{
  this->RegisterGestureRecognizer(PanGestureRecognizer::New().GetPointer());
}

mitk::GestureManager::~GestureManager()
{

}

void mitk::GestureManager::RegisterGestureRecognizer(mitk::GestureRecognizer::Pointer recognizer)
{
  this->m_Recognizers.push_back(recognizer);
}

mitk::GestureEvent::Pointer mitk::GestureManager::CheckForGesture(mitk::InteractionEvent* e)
{
  mitk::GestureEvent::Pointer gestureEvent;
  mitk::GestureEvent::GestureRating result;

  std::list<mitk::GestureRecognizer::Pointer>::iterator it = m_Recognizers.begin();
  std::list<mitk::GestureRecognizer::Pointer>::iterator end = m_Recognizers.end();
  for (; it != end; it++)
  {
    gestureEvent = (*it)->Create(m_Renderer);
    result = (*it)->Recognize(e, gestureEvent);

    if (result == mitk::GestureEvent::TriggerGesture)
    {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: gesture triggered: " << result;
      return gestureEvent;
    }
    else if (result == mitk::GestureEvent::FinishGesture)
    {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: gesture finished: " << result;
      return gestureEvent;
    }
    else if (result == mitk::GestureEvent::MayBeGesture)
    {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: maybe gesture: " << result;
      return gestureEvent;
    }
    else if (result == mitk::GestureEvent::CancelGesture)
    {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: not gesture: " << result;
      return gestureEvent;
    }
    else if (result == mitk::GestureEvent::Ignore)
    {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: ignored the event: " << result;
    }
    else {
      MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: hm, lets assume the recognizer"
        << "ignored the event: " << result;
    }
    //if (result & GestureRecognizer::ConsumeEventHint) {
    //  MITK_DEBUG("GestureManager") << "QGestureManager: we were asked to consume the event: "
    //    << state;
    //  ret = true;
    //}
  }
  return NULL;
}
