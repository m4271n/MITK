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

#ifndef MITKPanGestureEvent_H_
#define MITKPanGestureEvent_H_

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "mitkCommon.h"
#include "mitkGestureEvent.h"
#include "mitkBaseRenderer.h"
#include "mitkInteractionEvent.h"

#include <MitkCoreExports.h>

namespace mitk
{

  class MITK_CORE_EXPORT PanGestureEvent: public GestureEvent
  {

  public:
    mitkClassMacro(PanGestureEvent, GestureEvent)
    mitkNewMacro2Param(Self, BaseRenderer*, GestureRating)
    mitkNewMacro3Param(Self, BaseRenderer*, GestureRating, EventState)

    itkGetConstMacro(Offset, Point2D)
    itkGetConstMacro(LastOffset, Point2D)
    void SetOffset(Point2D offset) { m_Offset = offset; };
    void SetLastOffset(Point2D offset) { m_LastOffset = offset; };

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    PanGestureEvent(BaseRenderer*, GestureRating rating = Ignore, EventState state = Begin);
    virtual ~PanGestureEvent();

    virtual bool IsEqual(const InteractionEvent&) const;

  private:
    Point2D m_Offset;
    Point2D m_LastOffset;
  };
} /* namespace mitk */

#endif /* MITKGestureEvent_H_ */
