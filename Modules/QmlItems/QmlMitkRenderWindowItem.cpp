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

#include "QmlMitkRenderWindowItem.h"

#include <vtkOpenGLExtensionManager.h>

#include <QVTKInteractor.h>
#include<vtkEventQtSlotConnect.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKInteractorAdapter.h>

// MITK event types
#include "mitkTouchEvent.h"
#include "mitkMousePressEvent.h"
#include "mitkMouseMoveEvent.h"
#include "mitkMouseDoubleClickEvent.h"
#include "mitkMouseReleaseEvent.h"
#include "mitkInteractionKeyEvent.h"
#include "mitkMouseWheelEvent.h"
#include "mitkInternalEvent.h"
#include "mitkPlaneGeometryDataMapper2D.h"

#include "QmlMitkBigRenderLock.h"

#define INTERACTION_LEGACY // TODO: remove INTERACTION_LEGACY!

#if defined INTERACTION_LEGACY
  #include "InteractionLegacy/QmitkEventAdapter.h"
  #include "mitkGlobalInteraction.h"
#endif

QmlMitkRenderWindowItem* QmlMitkRenderWindowItem::GetInstanceForVTKRenderWindow( vtkRenderWindow* rw )
{
  return GetInstances()[rw];
}

QMap<vtkRenderWindow*, QmlMitkRenderWindowItem*>& QmlMitkRenderWindowItem::GetInstances()
{
  static QMap<vtkRenderWindow*, QmlMitkRenderWindowItem*> s_Instances;
  return s_Instances;
}


QmlMitkRenderWindowItem
::QmlMitkRenderWindowItem(QQuickItem* parent,
                          const QString& name,
                          mitk::VtkPropRenderer* /*renderer*/,
                          mitk::RenderingManager* renderingManager)
: QVTKQuickItem(parent)
{
  mitk::RenderWindowBase::Initialize( renderingManager, name.toStdString().c_str() );

  /* from QmitkRenderWindow. Required?
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  */

  GetInstances()[QVTKQuickItem::GetRenderWindow()] = this;
}

// called from QVTKQuickItem when window is painted for the first time!
void QmlMitkRenderWindowItem::init()
{
  QVTKQuickItem::init();

  mitk::DataStorage::Pointer m_DataStorage = mitk::RenderWindowBase::GetRenderer()->GetDataStorage();
  if (m_DataStorage.IsNotNull())
  {
    mitk::RenderingManager::GetInstance()->InitializeViews( m_DataStorage->ComputeBoundingGeometry3D(m_DataStorage->GetAll()) );
  }

  // TODO the following code needs to be moved to a multi-widget item
  if ( mitk::RenderWindowBase::GetRenderer()->GetMapperID() == mitk::BaseRenderer::Standard2D )
  {
    this->SetCrossHairPositioningOnClick(true);
  }

  if ( mitk::RenderWindowBase::GetRenderer()->GetMapperID() == mitk::BaseRenderer::Standard2D
      && m_DataStorage.IsNotNull() )
  {

    mitk::DataNode::Pointer planeNode = mitk::RenderWindowBase::GetRenderer()->GetCurrentWorldGeometry2DNode();
    switch ( mitk::RenderWindowBase::GetRenderer()->GetSliceNavigationController()->GetDefaultViewDirection() )
    {
      case mitk::SliceNavigationController::Axial:
        planeNode->SetColor(1.0,0.0,0.0);
        break;
      case mitk::SliceNavigationController::Sagittal:
        planeNode->SetColor(0.0,1.0,0.0);
        break;
      case mitk::SliceNavigationController::Frontal:
        planeNode->SetColor(0.0,0.0,1.0);
        break;
      default:
        planeNode->SetColor(1.0,1.0,0.0);
    }
    planeNode->SetProperty("layer", mitk::IntProperty::New(1000) );
    planeNode->SetProperty("visible", mitk::BoolProperty::New(true) );
    planeNode->SetProperty("helper object", mitk::BoolProperty::New(true) );

    mitk::PlaneGeometryDataMapper2D::Pointer mapper = mitk::PlaneGeometryDataMapper2D::New();
    mapper->SetDatastorageAndGeometryBaseNode( m_DataStorage, m_PlaneNodeParent );
    planeNode->SetMapper( mitk::BaseRenderer::Standard2D, mapper );

    m_DataStorage->Add( planeNode, m_PlaneNodeParent );
  }

  m_GestureManager.SetRenderer(this->GetRenderer());
}

void QmlMitkRenderWindowItem::InitView( mitk::BaseRenderer::MapperSlotId mapperID,
                                        mitk::SliceNavigationController::ViewDirection viewDirection )
{
  m_MapperID = mapperID;
  m_ViewDirection = viewDirection;
}


void QmlMitkRenderWindowItem::SetDataStorage(mitk::DataStorage::Pointer storage)
{
  m_DataStorage = storage;
}

mitk::Point2D QmlMitkRenderWindowItem::GetMousePosition(QMouseEvent* me) const
{
  mitk::Point2D point;
  point[0] = me->x();
  point[1] = me->y();
  m_Renderer->GetDisplayGeometry()->ULDisplayToDisplay(point, point);
  return point;
}

mitk::Point2D QmlMitkRenderWindowItem::GetMousePosition(QWheelEvent* we) const
{
  mitk::Point2D point;
  point[0] = we->x();
  point[1] = we->y();
  m_Renderer->GetDisplayGeometry()->ULDisplayToDisplay(point, point);
  return point;
}

mitk::InteractionEvent::MouseButtons QmlMitkRenderWindowItem::GetEventButton(QMouseEvent* me) const
{
  mitk::InteractionEvent::MouseButtons eventButton;
  switch (me->button())
  {
  case Qt::LeftButton:
    eventButton = mitk::InteractionEvent::LeftMouseButton;
    break;
  case Qt::RightButton:
    eventButton = mitk::InteractionEvent::RightMouseButton;
    break;
  case Qt::MidButton:
    eventButton = mitk::InteractionEvent::MiddleMouseButton;
    break;
  default:
    eventButton = mitk::InteractionEvent::NoButton;
    break;
  }
  return eventButton;
}

mitk::InteractionEvent::MouseButtons QmlMitkRenderWindowItem::GetButtonState(QMouseEvent* me) const
{
  mitk::InteractionEvent::MouseButtons buttonState = mitk::InteractionEvent::NoButton;

  if (me->buttons() & Qt::LeftButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::LeftMouseButton;
  }
  if (me->buttons() & Qt::RightButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::RightMouseButton;
  }
  if (me->buttons() & Qt::MidButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::MiddleMouseButton;
  }
  return buttonState;
}

mitk::InteractionEvent::ModifierKeys QmlMitkRenderWindowItem::GetModifiers(QInputEvent* me) const
{
  mitk::InteractionEvent::ModifierKeys modifiers = mitk::InteractionEvent::NoKey;

  if (me->modifiers() & Qt::ALT)
  {
    modifiers = modifiers | mitk::InteractionEvent::AltKey;
  }
  if (me->modifiers() & Qt::CTRL)
  {
    modifiers = modifiers | mitk::InteractionEvent::ControlKey;
  }
  if (me->modifiers() & Qt::SHIFT)
  {
    modifiers = modifiers | mitk::InteractionEvent::ShiftKey;
  }
  return modifiers;
}

mitk::InteractionEvent::MouseButtons QmlMitkRenderWindowItem::GetButtonState(QWheelEvent* we) const
{
  mitk::InteractionEvent::MouseButtons buttonState = mitk::InteractionEvent::NoButton;

  if (we->buttons() & Qt::LeftButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::LeftMouseButton;
  }
  if (we->buttons() & Qt::RightButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::RightMouseButton;
  }
  if (we->buttons() & Qt::MidButton)
  {
    buttonState = buttonState | mitk::InteractionEvent::MiddleMouseButton;
  }
  return buttonState;
}

mitk::InteractionEvent::EventState QmlMitkRenderWindowItem::GetEventState(QTouchEvent* te) const
{
  mitk::InteractionEvent::EventState eventType;
  switch (te->type())
  {
  case QEvent::Type::TouchBegin:
    eventType = mitk::InteractionEvent::EventState::Begin;
    break;
  case QEvent::Type::TouchUpdate:
    eventType = mitk::InteractionEvent::EventState::Update;
    break;
  case QEvent::Type::TouchEnd:
    eventType = mitk::InteractionEvent::EventState::End;
    break;
  default:
    eventType = mitk::InteractionEvent::EventState::Begin;
    break;
  }
  return eventType;
}

mitk::InteractionEvent::TouchDeviceType QmlMitkRenderWindowItem::GetTouchDeviceType(QTouchEvent* te) const
{
  mitk::InteractionEvent::TouchDeviceType devType;
  switch (te->device()->type())
  {
  case QTouchDevice::TouchScreen:
    devType = mitk::InteractionEvent::TouchDeviceType::TouchScreen;
    break;
  case QTouchDevice::TouchPad:
    devType = mitk::InteractionEvent::TouchDeviceType::TouchPad;
    break;
  default:
    devType = mitk::InteractionEvent::TouchDeviceType::TouchScreen;
    break;
  }
  return devType;
}

mitk::Point2D QmlMitkRenderWindowItem::GetTouchPointPosition(QTouchEvent::TouchPoint* tp) const
{
  mitk::Point2D point;
  point[0] = tp->pos().x();
  point[1] = tp->pos().y();
  m_Renderer->GetDisplayGeometry()->ULDisplayToDisplay(point, point);
  return point;
}

std::list<mitk::Point2D> QmlMitkRenderWindowItem::GetTouchPointPositions(QTouchEvent* te) const
{
  std::list<mitk::Point2D> positions;

  const QList<QTouchEvent::TouchPoint>& touchPoints = te->touchPoints();
  Q_FOREACH(QTouchEvent::TouchPoint tp, touchPoints)
  {
    positions.push_back(this->GetTouchPointPosition(&tp));
  }
  return positions;
}

mitk::InteractionEvent::TouchPointState QmlMitkRenderWindowItem::GetTouchPointState(QTouchEvent::TouchPoint* tp) const
{
  mitk::InteractionEvent::TouchPointState tpState;
  switch (tp->state())
  {
  case Qt::TouchPointPressed:
    tpState = mitk::InteractionEvent::TouchPointState::Pressed;
    break;
  case Qt::TouchPointMoved:
    tpState = mitk::InteractionEvent::TouchPointState::Moved;
    break;
  case Qt::TouchPointStationary:
    tpState = mitk::InteractionEvent::TouchPointState::Stationary;
    break;
  case Qt::TouchPointReleased:
    tpState = mitk::InteractionEvent::TouchPointState::Released;
    break;
  default:
    tpState = mitk::InteractionEvent::TouchPointState::Pressed;
    break;
  }
  return tpState;
}

std::list<mitk::InteractionEvent::TouchPointState> QmlMitkRenderWindowItem::GetTouchPointStates(QTouchEvent* te) const
{
  std::list<mitk::InteractionEvent::TouchPointState> states;

  const QList<QTouchEvent::TouchPoint>& touchPoints = te->touchPoints();
  Q_FOREACH(QTouchEvent::TouchPoint tp, touchPoints)
  {
    states.push_back(this->GetTouchPointState(&tp));
  }
  return states;
}


void QmlMitkRenderWindowItem::mousePressEvent(QMouseEvent* me)
{
  mitk::Point2D mousePosition = GetMousePosition(me);
  mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
  mitk::MousePressEvent::Pointer mPressEvent =
    mitk::MousePressEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(me), GetModifiers(me), GetEventButton(me));

#if defined INTERACTION_LEGACY
  bool modernInteractorHandledEvent =
#endif
  mitk::RenderWindowBase::HandleEvent(mPressEvent.GetPointer());
#if defined INTERACTION_LEGACY
  if (!modernInteractorHandledEvent)
  {
    mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
    mitk::RenderWindowBase::mousePressMitkEvent(&myevent);
  }
#endif

  QVTKQuickItem::mousePressEvent(me);

//  if (m_ResendQtEvents)
//    me->ignore();
}

void QmlMitkRenderWindowItem::mouseReleaseEvent(QMouseEvent* me)
{
  mitk::Point2D mousePosition = GetMousePosition(me);
  mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
  mitk::MouseReleaseEvent::Pointer mReleaseEvent =
    mitk::MouseReleaseEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(me), GetModifiers(me), GetEventButton(me));

#if defined INTERACTION_LEGACY
  bool modernInteractorHandledEvent =
#endif
  mitk::RenderWindowBase::HandleEvent(mReleaseEvent.GetPointer());
#if defined INTERACTION_LEGACY
  if (!modernInteractorHandledEvent)
  {
    mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
    mitk::RenderWindowBase::mouseReleaseMitkEvent(&myevent);
  }
#endif

  QVTKQuickItem::mouseReleaseEvent(me);

//  if (m_ResendQtEvents)
//    me->ignore();
}

void QmlMitkRenderWindowItem::mouseMoveEvent(QMouseEvent* me)
{
  mitk::Point2D mousePosition = GetMousePosition(me);
  mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
  mitk::MouseMoveEvent::Pointer mMoveEvent =
    mitk::MouseMoveEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(me), GetModifiers(me));

#if defined INTERACTION_LEGACY
  bool modernInteractorHandledEvent =
#endif
  mitk::RenderWindowBase::HandleEvent(mMoveEvent.GetPointer());
#if defined INTERACTION_LEGACY
  if (!modernInteractorHandledEvent)
  {
    mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
    mitk::RenderWindowBase::mouseMoveMitkEvent(&myevent);
  }
#endif

  QVTKQuickItem::mouseMoveEvent(me);

// TODO: why was this not put here initially? What is special about mouse move?
//  if (m_ResendQtEvents)
//    me->ignore();
}

void QmlMitkRenderWindowItem::wheelEvent(QWheelEvent *we)
{
  mitk::Point2D mousePosition = GetMousePosition(we);
  mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
  mitk::MouseWheelEvent::Pointer mWheelEvent =
    mitk::MouseWheelEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(we), GetModifiers(we), we->delta());

#if defined INTERACTION_LEGACY
  bool modernInteractorHandledEvent =
#endif
  mitk::RenderWindowBase::HandleEvent(mWheelEvent.GetPointer());
#if defined INTERACTION_LEGACY
  if (!modernInteractorHandledEvent)
  { // TODO: INTERACTION_LEGACY
    mitk::WheelEvent myevent(QmitkEventAdapter::AdaptWheelEvent(mitk::RenderWindowBase::GetRenderer(), we));
    mitk::RenderWindowBase::wheelMitkEvent(&myevent);
  }
#endif

  QVTKQuickItem::wheelEvent(we);

//  if (m_ResendQtEvents)
//    we->ignore();
}

void QmlMitkRenderWindowItem::touchEvent(QTouchEvent* te)
{
  mitk::TouchEvent::Pointer touchEvent =
    mitk::TouchEvent::New(mitk::RenderWindowBase::GetRenderer(), GetEventState(te),
    GetTouchDeviceType(te), GetTouchPointPositions(te), GetTouchPointStates(te));

  mitk::GestureEvent::Pointer gestureEvent = m_GestureManager.CheckForGesture(touchEvent);
  if (gestureEvent.IsNotNull())
  {
    mitk::RenderWindowBase::HandleEvent(gestureEvent);
  }

  QVTKQuickItem::touchEvent(te);
}

void QmlMitkRenderWindowItem::prepareForRender()
{
//  Adjust camera is kaputt wenn nicht der renderingmanager dem vtkprop bescheid sagt!
//  this is just a workaround
  QmlMitkBigRenderLock::GetMutex().lock();
  mitk::RenderWindowBase::GetRenderer()->ForceImmediateUpdate();
}

void QmlMitkRenderWindowItem::cleanupAfterRender()
{
  QmlMitkBigRenderLock::GetMutex().unlock();
}

void QmlMitkRenderWindowItem::SetCrossHairPositioningOnClick(bool enabled)
{
  if (enabled)
  {
    mitk::GlobalInteraction::GetInstance()->AddListener( mitk::RenderWindowBase::GetSliceNavigationController() );
  }
  else
  {
    mitk::GlobalInteraction::GetInstance()->RemoveListener( mitk::RenderWindowBase::GetSliceNavigationController() );
  }
}

void QmlMitkRenderWindowItem::SetPlaneNodeParent( mitk::DataNode::Pointer node )
{
  m_PlaneNodeParent = node;
}

void QmlMitkRenderWindowItem::geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry)
{
  QVTKQuickItem::geometryChanged(newGeometry, oldGeometry);

  this->resizeMitkEvent( newGeometry.width(), newGeometry.height() );
}

vtkRenderWindow* QmlMitkRenderWindowItem::GetVtkRenderWindow()
{
  return QVTKQuickItem::GetRenderWindow();
}

vtkRenderWindowInteractor* QmlMitkRenderWindowItem::GetVtkRenderWindowInteractor()
{
  return QVTKQuickItem::GetInteractor();
}


