/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include "QmitkStdMultiWidgetEditor.h"

#include <berryUIException.h>
#include <berryIWorkbenchPage.h>
#include <berryIPreferencesService.h>

#include <QWidget>

#include <mitkGlobalInteraction.h>

#include <mitkDataStorageEditorInput.h>

const std::string QmitkStdMultiWidgetEditor::EDITOR_ID = "org.mitk.editors.stdmultiwidget";

QmitkStdMultiWidgetEditor::QmitkStdMultiWidgetEditor()
 : m_StdMultiWidget(0)
{

}

QmitkStdMultiWidgetEditor::QmitkStdMultiWidgetEditor(const QmitkStdMultiWidgetEditor& other)
{
  Q_UNUSED(other)
  throw std::runtime_error("Copy constructor not implemented");
}

QmitkStdMultiWidgetEditor::~QmitkStdMultiWidgetEditor()
{
  // we need to wrap the RemovePartListener call inside a
  // register/unregister block to prevent infinite recursion
  // due to the destruction of temporary smartpointer to this
  this->Register();
  this->GetSite()->GetPage()->RemovePartListener(berry::IPartListener::Pointer(this));
  this->UnRegister(false);
}

QmitkStdMultiWidget* QmitkStdMultiWidgetEditor::GetStdMultiWidget()
{
  return m_StdMultiWidget;
}

void QmitkStdMultiWidgetEditor::Init(berry::IEditorSite::Pointer site, berry::IEditorInput::Pointer input)
{
  if (input.Cast<mitk::DataStorageEditorInput>().IsNull())
     throw berry::PartInitException("Invalid Input: Must be IFileEditorInput");

  this->SetSite(site);
  this->SetInput(input);
}

void QmitkStdMultiWidgetEditor::CreateQtPartControl(QWidget* parent)
{
  if (m_StdMultiWidget == 0)
  {
    m_DndFrameWidget = new QmitkDnDFrameWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout(parent);
    layout->addWidget(m_DndFrameWidget);
    layout->setContentsMargins(0,0,0,0); 

    m_StdMultiWidget = new QmitkStdMultiWidget(m_DndFrameWidget);
    QVBoxLayout* layout2 = new QVBoxLayout(m_DndFrameWidget);
    layout2->addWidget(m_StdMultiWidget);
    layout2->setContentsMargins(0,0,0,0); 

    mitk::DataStorage::Pointer ds = this->GetEditorInput().Cast<mitk::DataStorageEditorInput>()
      ->GetDataStorageReference()->GetDataStorage();

    // Tell the multiWidget which (part of) the tree to render
    m_StdMultiWidget->SetDataStorage(ds);

    // Initialize views as transversal, sagittal, coronar to all data objects in DataStorage
    // (from top-left to bottom)
    mitk::TimeSlicedGeometry::Pointer geo = ds->ComputeBoundingGeometry3D(ds->GetAll());
    mitk::RenderingManager::GetInstance()->InitializeViews(geo);

    // Initialize bottom-right view as 3D view
    m_StdMultiWidget->GetRenderWindow4()->GetRenderer()->SetMapperID(
      mitk::BaseRenderer::Standard3D );

    // Enable standard handler for levelwindow-slider
    m_StdMultiWidget->EnableStandardLevelWindow();

    // Add the displayed views to the tree to see their positions
    // in 2D and 3D
    m_StdMultiWidget->AddDisplayPlaneSubTree();

    m_StdMultiWidget->EnableNavigationControllerEventListening();

    mitk::GlobalInteraction::GetInstance()->AddListener(
        m_StdMultiWidget->GetMoveAndZoomInteractor()
      );
    this->GetSite()->GetPage()->AddPartListener(berry::IPartListener::Pointer(this));

    // enable change of logo
    berry::IPreferencesService::Pointer prefService
      = berry::Platform::GetServiceRegistry()
      .GetServiceById<berry::IPreferencesService>(berry::IPreferencesService::ID);

    berry::IPreferences::Pointer logoPref = prefService->GetSystemPreferences()->Node("DepartmentLogo");
    std::string departmentLogoLocation = logoPref->Get("DepartmentLogo","");


    //# Preferences

    berry::IBerryPreferences::Pointer prefs
        = (prefService->GetSystemPreferences()->Node(EDITOR_ID))
          .Cast<berry::IBerryPreferences>();
    assert( prefs );

    prefs->OnChanged.AddListener( berry::MessageDelegate1<QmitkStdMultiWidgetEditor
      , const berry::IBerryPreferences*>( this
        , &QmitkStdMultiWidgetEditor::OnPreferencesChanged ) );

    bool constrainedZooming = prefs->GetBool("Use constrained zooming and padding", false);

    mitk::RenderingManager::GetInstance()->SetConstrainedPaddingZooming(constrainedZooming);

    mitk::RenderingManager::GetInstance()->RequestUpdateAll();

    m_StdMultiWidget->SetDepartmentLogoPath(departmentLogoLocation.c_str());
    m_StdMultiWidget->DisableDepartmentLogo();
    m_StdMultiWidget->EnableDepartmentLogo();
  }
}

void QmitkStdMultiWidgetEditor::OnPreferencesChanged(const berry::IBerryPreferences* prefs)
{

  // Set preferences respecting zooming and padding
  bool constrainedZooming = prefs->GetBool("Use constrained zooming and padding", false);

  //prefs->Flush();

  mitk::RenderingManager::GetInstance()->SetConstrainedPaddingZooming(constrainedZooming);

  mitk::RenderingManager::GetInstance()->RequestUpdateAll();

  //this->GlobalReinit();


}

berry::IPartListener::Events::Types QmitkStdMultiWidgetEditor::GetPartEventTypes() const
{
  return Events::CLOSED | Events::HIDDEN | Events::VISIBLE;
}

void QmitkStdMultiWidgetEditor::PartClosed( berry::IWorkbenchPartReference::Pointer partRef )
{
  if (partRef->GetId() == QmitkStdMultiWidgetEditor::EDITOR_ID)
  {
    QmitkStdMultiWidgetEditor::Pointer stdMultiWidgetEditor = partRef->GetPart(false).Cast<QmitkStdMultiWidgetEditor>();

    if (m_StdMultiWidget == stdMultiWidgetEditor->GetStdMultiWidget())
    {
      m_StdMultiWidget->RemovePlanesFromDataStorage();
    }
  }
}

void QmitkStdMultiWidgetEditor::PartVisible( berry::IWorkbenchPartReference::Pointer partRef )
{
  if (partRef->GetId() == QmitkStdMultiWidgetEditor::EDITOR_ID)
  {
    QmitkStdMultiWidgetEditor::Pointer stdMultiWidgetEditor = partRef->GetPart(false).Cast<QmitkStdMultiWidgetEditor>();

    if (m_StdMultiWidget == stdMultiWidgetEditor->GetStdMultiWidget())
    {
      m_StdMultiWidget->AddPlanesToDataStorage();
    }
  }
}

void QmitkStdMultiWidgetEditor::PartHidden( berry::IWorkbenchPartReference::Pointer partRef )
{
  if (partRef->GetId() == QmitkStdMultiWidgetEditor::EDITOR_ID)
  {
    QmitkStdMultiWidgetEditor::Pointer stdMultiWidgetEditor = partRef->GetPart(false).Cast<QmitkStdMultiWidgetEditor>();

    if (m_StdMultiWidget == stdMultiWidgetEditor->GetStdMultiWidget())
    {
      m_StdMultiWidget->RemovePlanesFromDataStorage();
    }
  }
}

void QmitkStdMultiWidgetEditor::SetFocus()
{
  if (m_StdMultiWidget != 0)
    m_StdMultiWidget->setFocus();
}
