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
  *  \brief a recognizer class for one finger taps
  */
  class MitkQmlItems_EXPORT TapGestureRecognizer : public GestureRecognizer
  {

  public:
    mitkClassMacro(TapGestureRecognizer, GestureRecognizer);
    mitkNewMacro1Param(Self, BaseRenderer*)

      //virtual GestureEvent::Pointer Create(BaseRenderer* renderer);
      virtual GestureEvent::Pointer Recognize(InteractionEvent* e, GestureState& s);
    virtual void Reset();

  protected:
    TapGestureRecognizer(BaseRenderer*);
    virtual ~TapGestureRecognizer();

  private:
    //Point2D m_StartPos_TP1;
    //Point2D m_StartPos_TP2;
    //Point2D m_LastOffset;
    Point2D m_StartPos;
  };


  /**
  *  \brief
  */
  class MitkQmlItems_EXPORT PanGestureRecognizer : public GestureRecognizer
  {

  public:
    mitkClassMacro(PanGestureRecognizer, GestureRecognizer);
    //itkFactorylessNewMacro(Self)
    mitkNewMacro1Param(Self, BaseRenderer*)

    //virtual GestureEvent::Pointer Create(BaseRenderer* renderer);
    virtual GestureEvent::Pointer Recognize(InteractionEvent* e);
    virtual void Reset();

  protected:
    PanGestureRecognizer(BaseRenderer*);
    virtual ~PanGestureRecognizer();

  private:
    Point2D m_StartPos_TP1;
    Point2D m_StartPos_TP2;
    Point2D m_LastOffset;
    Point2D m_Offset;
  };

}

#endif
