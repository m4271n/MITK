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


mitk::GestureManager::GestureManager(BaseRenderer* ren)
  : itk::Object()
  , m_Renderer(ren)
{
  this->RegisterGestureRecognizer(TapGestureRecognizer::New(ren).GetPointer());
  this->RegisterGestureRecognizer(PanGestureRecognizer::New(ren).GetPointer());
}

mitk::GestureManager::~GestureManager()
{

}

void mitk::GestureManager::RegisterGestureRecognizer(mitk::GestureRecognizer::Pointer recognizer)
{
  this->m_RecognizerStatusPairs.push_back(RecognizerStatePair(recognizer, Ignore));
}

mitk::GestureEvent::Pointer mitk::GestureManager::CheckForGesture(mitk::InteractionEvent* e)
{
  mitk::GestureEvent::Pointer eventToBeSend = NULL;
  mitk::GestureManager::GestureState s;

  RecognizerStatePairs::iterator it = m_RecognizerStatusPairs.begin();
  RecognizerStatePairs::iterator end = m_RecognizerStatusPairs.end();
  for (; it != end; it++)
  {
    //try to recognize the gesture and store all possible gesture events
    (*it).first->Recognize(e, s);
    (*it).second = s;
    m_StatusMap[s]++; //increment the current status to know if a gesture was triggered
  }

  //check the status map to figure out if a gesture was triggered, finished or cancelled
  if (m_StatusMap[mitk::GestureManager::TriggerGesture] == 1 &&
      m_StatusMap[mitk::GestureManager::FinishGesture] == 0 ||
      m_StatusMap[mitk::GestureManager::TriggerGesture] == 0 &&
      m_StatusMap[mitk::GestureManager::FinishGesture] == 1)
  {
    //one event has to be triggered
    //find this event
    it = m_RecognizerStatusPairs.begin();
    for (; it != end; it++)
    {
      if ((*it).second == mitk::GestureManager::TriggerGesture ||
          (*it).second == mitk::GestureManager::FinishGesture)
      {
        //get the event from the recognizer
        eventToBeSend = (*it).first->GetEvent();
        break;
      }
      else
      {
        //all other recognizers can be reset now
        (*it).first->Reset();
      }
    }
  }

  return eventToBeSend;

/*
    if (s == mitk::GestureManager::TriggerGesture)
  {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: gesture triggered: " << s;
    return gestureEvent;
  }
  else if (s == mitk::GestureManager::FinishGesture)
  {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: gesture finished: " << s;
    return gestureEvent;
  }
  else if (s == mitk::GestureManager::MayBeGesture)
  {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: maybe gesture: " << s;
    return gestureEvent;
  }
  else if (s == mitk::GestureManager::CancelGesture)
  {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: not gesture: " << s;
    return gestureEvent;
  }
  else if (s == mitk::GestureManager::Ignore)
  {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: ignored the event: " << s;
  }
  else {
    MITK_DEBUG("GestureManager") << "GestureManager:Recognizer: hm, lets assume the recognizer"
      << "ignored the event: " << s;
  }
  return NULL;*/
}
