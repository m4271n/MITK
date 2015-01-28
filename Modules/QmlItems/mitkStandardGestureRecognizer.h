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

#ifndef MITKStandardGestureRecognizer_h
#define MITKStandardGestureRecognizer_h

#include "MitkQmlItemsExports.h"
#include "mitkCommon.h"
#include <list>
#include "mitkGestureRecognizer.h"
#include "mitkInteractionEvent.h"
#include "mitkGestureEvent.h"

namespace mitk
{
  /**
  *  \brief
  */
  class MitkQmlItems_EXPORT PanGestureRecognizer : public GestureRecognizer
  {

  public:
    mitkClassMacro(PanGestureRecognizer, GestureRecognizer);
    itkFactorylessNewMacro(Self)

    virtual GestureEvent::Pointer Create(BaseRenderer* renderer);
    virtual GestureEvent::GestureRating Recognize(InteractionEvent* e, GestureEvent* g);
    virtual void Reset();

  protected:
    PanGestureRecognizer();
    virtual ~PanGestureRecognizer();

  private:
    Point2D m_StartPos_TP1;
    Point2D m_StartPos_TP2;
    Point2D m_LastOffset;
    Point2D m_Offset;
  };

}

#endif
