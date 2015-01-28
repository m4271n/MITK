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

  public:
    mitkClassMacro(GestureManager, itk::Object);
    itkFactorylessNewMacro(Self)

    void SetRenderer(mitk::BaseRenderer* renderer){ m_Renderer = renderer;};

    void RegisterGestureRecognizer(GestureRecognizer::Pointer recognizer);

    GestureEvent::Pointer CheckForGesture(InteractionEvent* e);

  public:
    GestureManager();
    virtual ~GestureManager();

  private:
    std::list<GestureRecognizer::Pointer> m_Recognizers;
    mitk::BaseRenderer* m_Renderer;

  };

}

#endif
