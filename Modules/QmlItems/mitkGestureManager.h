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

#ifndef MITKGESTUREMANAGER_h
#define MITKGESTUREMANAGER_h

#include "MitkQmlItemsExports.h"
#include "mitkCommon.h"
#include <list>
#include "mitkGestureRecognizer.h"
#include "mitkInteractionEvent.h"

namespace mitk
{
  /**
  *  \brief
  */
  class MitkQmlItems_EXPORT GestureManager : public itk::Object
  {
    typedef std::pair<GestureRecognizer::Pointer, GestureRecognizer::GestureState> RecognizerStatePair;
    typedef std::list<RecognizerStatePair> RecognizerStatePairs;
    typedef std::set<GestureRecognizer::Pointer> RecognizerSet;
    typedef std::list<GestureEvent::Pointer> GestureEventList;

  public:
    mitkClassMacro(GestureManager, itk::Object);
    mitkNewMacro1Param(Self, BaseRenderer*)


    void SetRenderer(BaseRenderer* renderer){ m_Renderer = renderer;};

    void RegisterGestureRecognizer(GestureRecognizer::Pointer recognizer);

    GestureEvent::Pointer CheckForGesture(InteractionEvent* e);

  protected:
    GestureManager(BaseRenderer* ren);
    virtual ~GestureManager();

    unsigned int HasCommonItems(RecognizerSet& a, RecognizerSet& b);

  private:
    std::map<GestureRecognizer::GestureState, unsigned int> m_StatusMap;
    RecognizerStatePairs m_RecognizerStatusPairs;
    BaseRenderer* m_Renderer;

    RecognizerSet m_ActiveGestures;

  };

}

#endif
