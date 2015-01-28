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

#ifndef MITKGestureRecognizer_h
#define MITKGestureRecognizer_h

#include "MitkQmlItemsExports.h"
#include "mitkCommon.h"
#include <list>
#include "mitkGestureEvent.h"

namespace mitk
{
  /**
  *  \brief
  */
  class MitkQmlItems_EXPORT GestureRecognizer : public itk::Object
  {

  public:
    mitkClassMacro(GestureRecognizer, itk::Object);
    //itkFactorylessNewMacro(Self)

    virtual GestureEvent::Pointer Create(BaseRenderer*) = 0;
    virtual GestureEvent::GestureRating Recognize(InteractionEvent* e, GestureEvent* g) = 0;
    virtual void Reset() = 0;

  protected:
    GestureRecognizer(){};
    virtual ~GestureRecognizer(){};
  };

}

#endif
