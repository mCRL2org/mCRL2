// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algodialog.cpp
/// \brief Implementation of algorithm dialog

#include "wx.hpp" // precompiled headers

#include "algodialog.h"
#include <wx/sizer.h>
#include <wx/notebook.h>

AlgoDialog::AlgoDialog(LTSGraph3d* owner, wxWindow* parent)
  : wxDialog(parent, wxID_ANY, wxT("Layout optimization"), wxDefaultPosition,
             wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER)

{
  app = owner;

  size_t nrAlgos = app->getNumberOfAlgorithms();

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

  if (nrAlgos > 1)
  {
    wxNotebook* nb = new wxNotebook(
                          this, wxID_ANY, wxDefaultPosition,
                          wxDefaultSize, wxNB_TOP);

    for (size_t i = 0; i < nrAlgos; ++i)
    {
      LayoutAlgorithm* algo = app->getAlgorithm(i);
      wxPanel* algoWindow = new wxPanel(nb, wxID_ANY);

      algo->setupPane(algoWindow);

      //TODO: Get algorithm name
      nb->AddPage(algoWindow, wxT("Spring embedder"));
    }
    sizer->Add(nb, 0, wxEXPAND|wxALL, 5);
  }
  else
  {
    LayoutAlgorithm* algo = app->getAlgorithm(0);

    wxPanel* algoWindow = new wxPanel(this, wxID_ANY);

    algo->setupPane(algoWindow);
    PushEventHandler(algo);

    sizer->Add(algoWindow, 0, wxEXPAND|wxALL, 5);
  }


  SetSizer(sizer);
  Fit();
  Layout();
}
