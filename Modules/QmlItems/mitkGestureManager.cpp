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
  this->RegisterGestureRecognizer(PinchGestureRecognizer::New(ren).GetPointer());
  //this->RegisterGestureRecognizer(TapGestureRecognizer::New(ren).GetPointer());
  //this->RegisterGestureRecognizer(PanGestureRecognizer::New(ren).GetPointer());
}

mitk::GestureManager::~GestureManager()
{

}

void mitk::GestureManager::RegisterGestureRecognizer(mitk::GestureRecognizer::Pointer recognizer)
{
  m_RecognizerStatusPairs.push_back(RecognizerStatePair(recognizer, GestureRecognizer::NoGesture));
}

mitk::GestureEvent::Pointer mitk::GestureManager::CheckForGesture(mitk::InteractionEvent* e)
{
  mitk::GestureEvent::Pointer eventToBeSend = NULL;
  mitk::GestureRecognizer::GestureState s;    
  RecognizerSet finishedGestures;
  RecognizerSet triggeredGestures;
  RecognizerSet newMaybeGestures;
  RecognizerSet notGestures;

  RecognizerStatePairs::iterator it = m_RecognizerStatusPairs.begin();
  RecognizerStatePairs::iterator end = m_RecognizerStatusPairs.end();
  for (; it != end; it++)
  {
    //try to recognize the gesture and store all possible gesture events
    s = (*it).second;
    (*it).first->Recognize(e, s);
    (*it).second = s;
    switch (s)
    {
    case mitk::GestureRecognizer::Ignore:
      break;
    case mitk::GestureRecognizer::MayBeGesture:
      newMaybeGestures.insert((*it).first);
      break;
    case mitk::GestureRecognizer::TriggerGesture:
      triggeredGestures.insert((*it).first);
      break;
    case mitk::GestureRecognizer::FinishGesture:
      finishedGestures.insert((*it).first);
      break;
    case mitk::GestureRecognizer::CancelGesture:
      notGestures.insert((*it).first);
      break;
    case mitk::GestureRecognizer::NoGesture:
      notGestures.insert((*it).first);
      break;
    default:
      break;
    }
    //m_StatusMap[s]++; //increment the current status to know if a gesture was triggered
  }

  //check if anything was recognized, if not return 
  if (triggeredGestures.empty() && finishedGestures.empty()
    && newMaybeGestures.empty() && notGestures.empty())
    return NULL;

  //get all gestures that are triggered but were not active so far
  RecognizerSet inactiveTriggeredGestures; //startedGestures
  std::set_difference(triggeredGestures.begin(), triggeredGestures.end(), 
    m_ActiveGestures.begin(), m_ActiveGestures.end(),
    std::inserter(inactiveTriggeredGestures, inactiveTriggeredGestures.end()));

  //get all gestures that are triggered AND active
  RecognizerSet activeTriggeredGestures;
  std::set_intersection(triggeredGestures.begin(), triggeredGestures.end(),
    m_ActiveGestures.begin(), m_ActiveGestures.end(),
    std::inserter(activeTriggeredGestures, activeTriggeredGestures.end()));

  // check if a running gesture switched back to maybe state
  RecognizerSet activeToMaybeGestures;
  std::set_intersection(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    newMaybeGestures.begin(), newMaybeGestures.end(),
    std::inserter(activeToMaybeGestures, activeToMaybeGestures.end()));

  // check if a running gesture switched back to not gesture state,
  // i.e. were canceled
  RecognizerSet canceledGestures;
  std::set_intersection(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    notGestures.begin(), notGestures.end(),
    std::inserter(canceledGestures, canceledGestures.end()));

  // start timers for new gestures in maybe state

  // kill timers for gestures that were in maybe state

  assert(!HasCommonItems(inactiveTriggeredGestures, finishedGestures));
  assert(!HasCommonItems(inactiveTriggeredGestures, newMaybeGestures));
  assert(!HasCommonItems(inactiveTriggeredGestures, canceledGestures));
  assert(!HasCommonItems(finishedGestures, newMaybeGestures));
  assert(!HasCommonItems(finishedGestures, canceledGestures));
  assert(!HasCommonItems(canceledGestures, newMaybeGestures));

  //check for gestures that claim to be finished without being active
  RecognizerSet notStarted;
  std::set_difference(finishedGestures.begin(), finishedGestures.end(),
    m_ActiveGestures.begin(), m_ActiveGestures.end(),
    std::inserter(notStarted, notStarted.end()));
  if (notStarted.size())
  {
    //MITK_WARN("GestureManager") << "There are some gestures that claim to be finished, but never "
    //  "started. Probably those are singleshot gestures so we'll fake the started state.";
    cout << "notStarted != 0" << endl;
    //todo handle these gestures
    //here is what qt is doing:
    //foreach(QGesture *gesture, notStarted)
    //  gesture->d_func()->state = Qt::GestureStarted;
    //QSet<QGesture *> undeliveredGestures;
    //deliverEvents(notStarted, &undeliveredGestures);
    //finishedGestures -= undeliveredGestures;

    //for the moment ignore these events
    RecognizerSet finishedAndActiveGestures;
    std::set_difference(finishedGestures.begin(), finishedGestures.end(),
      notStarted.begin(), notStarted.end(),
      std::inserter(finishedAndActiveGestures, finishedAndActiveGestures.end()));
    finishedGestures = finishedAndActiveGestures;
  }

  //add all inactive triggered gestures into the active list
  m_ActiveGestures.insert(inactiveTriggeredGestures.begin(), inactiveTriggeredGestures.end());
  // sanity check: all triggered gestures should already be in active gestures list
  assert(HasCommonItems(m_ActiveGestures, activeTriggeredGestures) == activeTriggeredGestures.size());
  //remove the finished gestures from the active ones
  RecognizerSet activeNonFinishedGestures;
  std::set_difference(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    finishedGestures.begin(), finishedGestures.end(), 
    std::inserter(activeNonFinishedGestures, activeNonFinishedGestures.end()));
  m_ActiveGestures = activeNonFinishedGestures;
  //remove the activeToMaybe gestures from the active ones
  RecognizerSet activeNonMaybeGestures;
  std::set_difference(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    activeToMaybeGestures.begin(), activeToMaybeGestures.end(),
    std::inserter(activeNonMaybeGestures, activeNonMaybeGestures.end()));
  m_ActiveGestures = activeNonMaybeGestures;
  //remove the canceled gestures from the active ones
  RecognizerSet activeNonCanceledGestures;
  std::set_difference(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    canceledGestures.begin(), canceledGestures.end(),
    std::inserter(activeNonCanceledGestures, activeNonCanceledGestures.end()));
  m_ActiveGestures = activeNonCanceledGestures;

  //make a debug outpu
  MITK_INFO("GestureManager") << "activeGestures: " << m_ActiveGestures.size()
    << "maybeGestures: " << newMaybeGestures.size()
    << "started: " << inactiveTriggeredGestures.size()
    << "triggered: " << triggeredGestures.size()
    << "finished: " << finishedGestures.size()
    << "canceled: " << canceledGestures.size();

  //// set the proper gesture state on each gesture
  //foreach(QGesture *gesture, startedGestures)
  //  gesture->d_func()->state = Qt::GestureStarted;
  //foreach(QGesture *gesture, triggeredGestures)
  //  gesture->d_func()->state = Qt::GestureUpdated;
  //foreach(QGesture *gesture, finishedGestures)
  //  gesture->d_func()->state = Qt::GestureFinished;
  //foreach(QGesture *gesture, canceledGestures)
  //  gesture->d_func()->state = Qt::GestureCanceled;
  //foreach(QGesture *gesture, activeToMaybeGestures)
  //  gesture->d_func()->state = Qt::GestureFinished;

  //bundle all gestures that have to be send
  RecognizerSet toBeDeliveredGestures;
  toBeDeliveredGestures.insert(inactiveTriggeredGestures.begin(), inactiveTriggeredGestures.end());
  toBeDeliveredGestures.insert(triggeredGestures.begin(), triggeredGestures.end());
  toBeDeliveredGestures.insert(finishedGestures.begin(), finishedGestures.end());
  toBeDeliveredGestures.insert(canceledGestures.begin(), canceledGestures.end());

  //send the gestures
  RecognizerSet undeliveredGestures;
  //deliverEvents(toBeDeliveredGestures, &undeliveredGestures);
  //the delivering is done by the render window
  //return the gesture event and the render window will handle it
  GestureEventList gesturesToBeDelivered;
  RecognizerSet::iterator dit = toBeDeliveredGestures.begin();
  RecognizerSet::iterator dend = toBeDeliveredGestures.end();
  for (; dit != dend; dit++)
  {
    gesturesToBeDelivered.push_back((*dit)->GetEvent());
  }

  //qt is checking some stuff here
  //foreach(QGesture *g, inactiveTriggeredGestures) {
  //  if (undeliveredGestures.contains(g))
  //    continue;
  //  if (g->gestureCancelPolicy() == QGesture::CancelAllInContext) {
  //    DEBUG() << "lets try to cancel some";
  //    // find gestures in context in Qt::GestureStarted or Qt::GestureUpdated state and cancel them
  //    cancelGesturesForChildren(g);
  //  }
  //}

  //some gestures could not be send but remove them from the avtive list
  RecognizerSet activeUndeliveredGestures;
  std::set_difference(m_ActiveGestures.begin(), m_ActiveGestures.end(),
    undeliveredGestures.begin(), undeliveredGestures.end(),
    std::inserter(activeUndeliveredGestures, activeUndeliveredGestures.end()));
  m_ActiveGestures = activeUndeliveredGestures;

  // reset gestures that ended
  RecognizerSet endedGestures;
  toBeDeliveredGestures.insert(finishedGestures.begin(), finishedGestures.end());
  toBeDeliveredGestures.insert(canceledGestures.begin(), canceledGestures.end());
  toBeDeliveredGestures.insert(undeliveredGestures.begin(), undeliveredGestures.end());

  //reset all recognizer that are finished
  RecognizerSet::iterator eit = endedGestures.begin();
  RecognizerSet::iterator eend = endedGestures.end();
  for (; eit != eend; eit++)
  {
    (*eit)->Reset();
  }

  //return the list with gestures that wait for delivering
  //for the moment just the first one is sent
  if (gesturesToBeDelivered.size())
  {
    return gesturesToBeDelivered.front();
  }
  else
  {
    return NULL;
  }

  ////check the status map to figure out if a gesture was triggered, finished or cancelled
  //if (m_StatusMap[mitk::GestureRecognizer::TriggerGesture] == 1 &&
  //  m_StatusMap[mitk::GestureRecognizer::FinishGesture] == 0 ||
  //  m_StatusMap[mitk::GestureRecognizer::TriggerGesture] == 0 &&
  //  m_StatusMap[mitk::GestureRecognizer::FinishGesture] == 1)
  //{
  //  //one event has to be triggered
  //  //find this event
  //  it = m_RecognizerStatusPairs.begin();
  //  for (; it != end; it++)
  //  {
  //    if ((*it).second == mitk::GestureRecognizer::TriggerGesture ||
  //      (*it).second == mitk::GestureRecognizer::FinishGesture)
  //    {
  //      //get the event from the recognizer
  //      eventToBeSend = (*it).first->GetEvent();
  //      break;
  //    }
  //    else
  //    {
  //      //all other recognizers can be reset now
  //      (*it).first->Reset();
  //    }
  //  }
  //}

  //return eventToBeSend;

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

unsigned int mitk::GestureManager::HasCommonItems(RecognizerSet& a, RecognizerSet& b)
{
  RecognizerSet result;
  std::set_intersection(a.begin(), a.end(),
    b.begin(), b.end(), std::inserter(result, result.begin()));
  return result.size();
}