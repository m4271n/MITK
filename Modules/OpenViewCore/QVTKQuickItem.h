/*========================================================================
  OpenView -- http://openview.kitware.com

  Copyright 2012 Kitware, Inc.

  Licensed under the BSD license. See LICENSE file for details.
 ========================================================================*/
#ifndef QVTKQuickItem_h
#define QVTKQuickItem_h

#include <QQuickFramebufferObject>

#include <QOpenGLShaderProgram>

#include "vtkSmartPointer.h"
#include "vtkNew.h"

#include <QMutex>

#include <MitkOpenViewCoreExports.h>

class QOpenGLContext;
class QOpenGLFramebufferObject;
class QVTKInteractorAdapter;
class QVTKInteractor;
class QVTKFramebufferObjectRenderer;
class vtkEventQtSlotConnect;
class vtkOpenGLRenderWindow;
class vtkObject;
class vtkContextView;

class MITKOPENVIEWCORE_EXPORT QVTKQuickItem : public QQuickFramebufferObject
{
  Q_OBJECT
public:
  QVTKQuickItem(QQuickItem* parent = nullptr);

  // Description:
  // destructor
  ~QVTKQuickItem();

  //void itemChange(ItemChange change, const ItemChangeData &) override;
  Renderer *createRenderer() const;

  // Description:
  // get the render window used with this item
  vtkOpenGLRenderWindow* GetRenderWindow() const;

  // Description:
  // get the render window interactor used with this item
  // this item enforces its own interactor
  QVTKInteractor* GetInteractor() const;

protected slots:
  // slot called when vtk wants to know if the context is current
  virtual void IsCurrent(vtkObject* caller, unsigned long vtk_event, void* client_data, void* call_data);
  // slot called when vtk wants to know if a window is direct
  virtual void IsDirect(vtkObject* caller, unsigned long vtk_event, void* client_data, void* call_data);
  // slot called when vtk wants to know if a window supports OpenGL
  virtual void SupportsOpenGL(vtkObject* caller, unsigned long vtk_event, void* client_data, void* call_data);
  void onTextureFollowsItemSizeChanged(bool follows);

protected:
  // Called ONCE from the render thread before the FBO is first created and while the GUI thread is blocked
  virtual void init();
  // Called from the render thread BEFORE each update while the GUI thread blocked
  virtual bool prepareForRender();
  // Called from the render thread AFTER each update while the GUI thread is NOT blocked
  virtual void cleanupAfterRender();

  // handle item key events
  virtual void keyPressEvent(QKeyEvent* e) override;
  virtual void keyReleaseEvent(QKeyEvent* e) override;

  // handle item mouse events
  virtual void mousePressEvent(QMouseEvent* e) override;
  virtual void mouseReleaseEvent(QMouseEvent* e) override;
  virtual void mouseDoubleClickEvent(QMouseEvent* e) override;
  virtual void mouseMoveEvent(QMouseEvent* e) override;
  virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry) override;
  virtual void wheelEvent(QWheelEvent* e) override;
  virtual void hoverEnterEvent(QHoverEvent* e) override;
  virtual void hoverLeaveEvent(QHoverEvent* e) override;
  virtual void hoverMoveEvent(QHoverEvent* e) override;

  //virtual QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* updatePaintNodeData) override;

  QMutex m_viewLock;

private:
  vtkOpenGLRenderWindow *m_win;
  vtkSmartPointer<QVTKInteractor> m_interactor;
  QVTKInteractorAdapter* m_interactorAdapter;
  vtkSmartPointer<vtkEventQtSlotConnect> m_connect;

  friend class QVTKFramebufferObjectRenderer;
};

#endif
