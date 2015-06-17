/*===================================================================

BlueBerry Platform

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef BERRYHELPCONTENTSHANDLER_H
#define BERRYHELPCONTENTSHANDLER_H

#include <berryAbstractHandler.h>

namespace berry {

class HelpContentsHandler : public AbstractHandler
{
  Q_OBJECT

public:

  Object::Pointer Execute(const SmartPointer<const ExecutionEvent>& event);

  bool IsEnabled() const;
};

}

#endif // BERRYHELPCONTENTSHANDLER_H
