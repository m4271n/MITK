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
#include "mitkMousePressEvent.h"
#include "mitkMouseMoveEvent.h"
#include "mitkMouseDoubleClickEvent.h"
#include "mitkMouseReleaseEvent.h"
#include "mitkInteractionKeyEvent.h"
#include "mitkMouseWheelEvent.h"
#include "mitkInternalEvent.h"
#include "mitkPlaneGeometryDataMapper2D.h"

#include "QmlFboGeometry.h"
#include <QEvent>
#include <limits>

#define INTERACTION_LEGACY // TODO: remove INTERACTION_LEGACY!

#if defined INTERACTION_LEGACY
  #include "InteractionLegacy/QmitkEventAdapter.h"
  #include "mitkGlobalInteraction.h"
#endif


QMutex QmlMitkRenderWindowItem::s_MitkRendererDataLock(QMutex::Recursive);

class QVTKQuickItemStartedRenderingEvent : public QEvent
{
public:
  static QEvent::Type QVTKQuickItemStartedRendering;
  QVTKQuickItemStartedRenderingEvent() : QEvent( QVTKQuickItemStartedRendering ) {}
};
QEvent::Type QVTKQuickItemStartedRenderingEvent::QVTKQuickItemStartedRendering = static_cast<QEvent::Type>(QEvent::registerEventType());

class QVTKQuickItemFinishedRenderingEvent : public QEvent
{
public:
  static QEvent::Type QVTKQuickItemFinishedRendering;
  QVTKQuickItemFinishedRenderingEvent() : QEvent( QVTKQuickItemFinishedRendering ) {}
};
QEvent::Type QVTKQuickItemFinishedRenderingEvent::QVTKQuickItemFinishedRendering = static_cast<QEvent::Type>(QEvent::registerEventType());


QmlMitkRenderWindowItem* QmlMitkRenderWindowItem::GetInstanceForVTKRenderWindow( vtkRenderWindow* rw )
{
  if (GetInstances().contains(rw))
  {
    return GetInstances()[rw];
  }
  return 0;
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
  QString uniqueName = GetUniqueName(name);
  mitk::RenderWindowBase::Initialize(renderingManager, uniqueName.toStdString().c_str());

  // Replace the default DisplayGeometry with one in which the upper-left
  // corner of the view port is the origin of the "Display" coordinate frame.
  mitk::QmlFboGeometry::Pointer qmlFboGeometry = mitk::QmlFboGeometry::New( GetRenderer()->GetDisplayGeometry() );
  GetRenderer()->SetDisplayGeometry(qmlFboGeometry.GetPointer());

  /* from QmitkRenderWindow. Required?
  setFocusPolicy(Qt::StrongFocus);
  setMouseTracking(true);
  */

  GetInstances()[QVTKQuickItem::GetRenderWindow()] = this;
}

// checks the previously created render window items for their names, in case the name already
// exists it will add a index number at the end of the name in order to make it unique.
// this is just a workaround and shouldnt be necessary, however, caused by the current QML
// architecture the constructor is called with standard parameters (see above). So all instances
// would have the same name. It would be optimal to define a name in the QML file and use this name
// here in the constructor but with the current architecture it is not working. The constructor is
// called automatically in a generic way. A second option would be to first construct the render
// window item and subsequently set the name but as far as I know MITK does not allow the change of
// the name after the construction.
QString QmlMitkRenderWindowItem::GetUniqueName(const QString& name)
{
   static unsigned int nameCounter = 1;
   QString uniqueName = name;

   //check the other instances if they have the same name
   for each (auto instance in GetInstances())
   {
      //check if the name of the current item in the map is the same as the one of the current item
      if (uniqueName.compare(instance->GetRenderer()->GetName()) == 0)
      {
         //they are the same => add an index at the end
         uniqueName.append(QString::number(nameCounter++));
         break;
      }
   }
   return uniqueName;
}

// called from QVTKQuickItem when window is painted for the first time!
void QmlMitkRenderWindowItem::init()
{
  QVTKQuickItem::init();

  mitk::DataStorage::Pointer dataStorage = mitk::RenderWindowBase::GetRenderer()->GetDataStorage();
  if (dataStorage.IsNotNull())
  {
    mitk::RenderingManager::GetInstance()->InitializeViews( dataStorage->ComputeBoundingGeometry3D(dataStorage->GetAll()) );
  }

  // TODO the following code needs to be moved to a multi-widget item
  if ( mitk::RenderWindowBase::GetRenderer()->GetMapperID() == mitk::BaseRenderer::Standard2D )
  {
    this->SetCrossHairPositioningOnClick(true);
  }

  if ( mitk::RenderWindowBase::GetRenderer()->GetMapperID() == mitk::BaseRenderer::Standard2D
      && dataStorage.IsNotNull() )
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
    //mapper->SetDatastorageAndGeometryBaseNode( dataStorage, m_PlaneNodeParent );
    planeNode->SetMapper( mitk::BaseRenderer::Standard2D, mapper );

    dataStorage->Add( planeNode, m_PlaneNodeParent );
  }
}
  


void QmlMitkRenderWindowItem::SetDataStorage(mitk::DataStorage::Pointer storage)
{
  mitk::RenderWindowBase::GetRenderer()->SetDataStorage(storage);
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

//#if defined INTERACTION_LEGACY
//  bool modernInteractorHandledEvent =
//#endif
  bool isHandled = mitk::RenderWindowBase::HandleEvent(mMoveEvent.GetPointer());
//#if defined INTERACTION_LEGACY
//  if (!modernInteractorHandledEvent)
//  {
//    mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
//    mitk::RenderWindowBase::mouseMoveMitkEvent(&myevent);
//  }
//#endif

  if (!isHandled)
  {
     QVTKQuickItem::mouseMoveEvent(me);
  }

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


void QmlMitkRenderWindowItem::keyPressEvent(QKeyEvent * e)
{
//   mitk::Point2D mousePosition = GetMousePosition(me);
//   mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
//   mitk::MousePressEvent::Pointer mPressEvent =
//      mitk::MousePressEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(me), GetModifiers(me), GetEventButton(me));
//
//#if defined INTERACTION_LEGACY
//   bool modernInteractorHandledEvent =
//#endif
//      mitk::RenderWindowBase::HandleEvent(mPressEvent.GetPointer());
//#if defined INTERACTION_LEGACY
//   if (!modernInteractorHandledEvent)
//   {
//      mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
//      mitk::RenderWindowBase::mousePressMitkEvent(&myevent);
//   }
//#endif

   QVTKQuickItem::keyPressEvent(e);

   //  if (m_ResendQtEvents)
   //    me->ignore();
}


void QmlMitkRenderWindowItem::keyReleaseEvent(QKeyEvent * e)
{
   //   mitk::Point2D mousePosition = GetMousePosition(me);
   //   mitk::Point3D worldPosition = mitk::RenderWindowBase::GetRenderer()->Map2DRendererPositionTo3DWorldPosition(mousePosition);
   //   mitk::MousePressEvent::Pointer mPressEvent =
   //      mitk::MousePressEvent::New(mitk::RenderWindowBase::GetRenderer(), mousePosition, worldPosition, GetButtonState(me), GetModifiers(me), GetEventButton(me));
   //
   //#if defined INTERACTION_LEGACY
   //   bool modernInteractorHandledEvent =
   //#endif
   //      mitk::RenderWindowBase::HandleEvent(mPressEvent.GetPointer());
   //#if defined INTERACTION_LEGACY
   //   if (!modernInteractorHandledEvent)
   //   {
   //      mitk::MouseEvent myevent(QmitkEventAdapter::AdaptMouseEvent(mitk::RenderWindowBase::GetRenderer(), me));
   //      mitk::RenderWindowBase::mousePressMitkEvent(&myevent);
   //   }
   //#endif

   QVTKQuickItem::keyReleaseEvent(e);

   //  if (m_ResendQtEvents)
   //    me->ignore();
}


bool QmlMitkRenderWindowItem::prepareForRender()
{
   //  If not able to get the render lock within a timeout (20 millsec), then give up and return false.
  if (!s_MitkRendererDataLock.tryLock())
  {
    // The GUI loop has not had enough time to process events delayed during
    // the previous render update so we return false to indicate that we are
    // not ready to render
    MITK_WARN << "Skipping render update to prevent GUI loop starvation";
    return false;
  }
  QCoreApplication::postEvent(this, new QVTKQuickItemStartedRenderingEvent, INT_MAX);
  mitk::VtkPropRenderer *vPR = dynamic_cast<mitk::VtkPropRenderer*>(mitk::BaseRenderer::GetInstance( this->GetRenderWindow() ));
  if(vPR)
  {
    vPR->PrepareRender();
  }
  return true;
}

void QmlMitkRenderWindowItem::cleanupAfterRender()
{
  // QVTKQuickItemFinishedRenderingEvent to itself with the default
  // priority. Since all instances of QmlMitkRenderWindowItem live in the GUI
  // thread, this event will get added to the GUI event queue and will be
  // processed after all events already in the GUI event queue except for any
  // that were posted with lower than the default priority
  // (e.g. Qt::LowPriorityEvent).
  QCoreApplication::postEvent(this, new QVTKQuickItemFinishedRenderingEvent);
  s_MitkRendererDataLock.unlock();
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

bool QmlMitkRenderWindowItem::event(QEvent *e)
{
  if (e->type() == QVTKQuickItemStartedRenderingEvent::QVTKQuickItemStartedRendering)
  {
    // Rendering started - block GUI loop until ...
    s_MitkRendererDataLock.lock();
    return true;
  }
  else if (e->type() == QVTKQuickItemFinishedRenderingEvent::QVTKQuickItemFinishedRendering)
  {
    // ... rendering is finished
    s_MitkRendererDataLock.unlock();
    return true;
  }
  else
  {
    // Workaround for occasional Event that "slip through" during the brief
    // window of time between when the QSG synchronize phase unblocks the GUI
    // thread and when the QVTKQuickItemStartedRendering event is processed
    // and blocks the GUI thread again.
    /// @todo Exactly how/where does the QSG block the GUI thread??
    s_MitkRendererDataLock.lock();
    s_MitkRendererDataLock.unlock();

    return QVTKQuickItem::event(e);
  }
}
