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

#ifndef MITKTouchEvent_H_
#define MITKTouchEvent_H_

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "mitkCommon.h"
#include "mitkInteractionEventConst.h"
#include "mitkInteractionEvent.h"
#include "mitkBaseRenderer.h"
#include "mitkInteractionEvent.h"

#include <MitkCoreExports.h>

namespace mitk
{

  class MITK_CORE_EXPORT TouchEvent: public InteractionEvent
  {

  public:
    mitkClassMacro(TouchEvent,InteractionEvent)
    mitkNewMacro5Param(Self, BaseRenderer*, const std::list<Point2D>&, const std::list<TouchPointState>&, TouchEventType, TouchDeviceType)

    TouchEventType GetEventType() const;
    TouchDeviceType GetDeviceType() const;
    void SetEventType(TouchEventType type);
    void SetDeviceType(TouchDeviceType devType);
    //TouchPointState GetTouchPointState() const;
    void GetTouchPointStates(std::list<TouchPointState>&) const;
    void SetTouchPointStates(const std::list<TouchPointState>& tpStates);
    const std::list<Point2D>& GetTouchPointPositionsOnScreen() const;

    virtual bool IsSuperClassOf(const InteractionEvent::Pointer& baseClass) const;

  protected:
    TouchEvent(BaseRenderer*, const std::list<Point2D>& = std::list<Point2D>(),
      const std::list<TouchPointState>& = std::list<TouchPointState>(),
      TouchEventType eventType = Begin, TouchDeviceType devType = TouchScreen);
    virtual ~TouchEvent();

    virtual bool IsEqual(const InteractionEvent&) const;

  private:
    TouchEventType m_EventType;
    TouchDeviceType m_DeviceType;
    const std::list<TouchPointState>  m_TouchPointStates;
    const std::list<Point2D>  m_TouchPointPositions;
  };
} /* namespace mitk */

#endif /* MITKTouchEvent_H_ */
