// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algodialog.h
/// \brief Declaration of algorithm dialog

#ifndef ALGO_DIALOG_H
#define ALGO_DIALOG_H

#include <wx/dialog.h>

#include "ltsgraph3d.h"

class AlgoDialog : public wxDialog
{
  public:
    AlgoDialog(LTSGraph3d* owner, wxWindow* parent);

  private:
    LTSGraph3d* app;

};

#endif //ALGO_DIALOG_H
