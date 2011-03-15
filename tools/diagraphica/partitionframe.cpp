// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./partitionframe.cpp

#include "wx.hpp" // precompiled headers

#include "partitionframe.h"


// -- constructors and desctructor ------------------------------


// ----------------------------
PartitionFrame::PartitionFrame(
  Mediator* m,
  wxWindow* parent,
  wxWindowID id,
  wxString title,
  wxPoint position,
  wxSize size,
  wxString attrName,
  size_t minParts,
  size_t maxParts,
  size_t curParts)
  : PopupFrame(
    m,
    parent,
    id,
    title,
    position,
    size)
// ----------------------------
{
  SetMinSize(size);
  SetMaxSize(size);

  initFrame(attrName, minParts, maxParts, curParts);
}


// ------------------------------
PartitionFrame::~PartitionFrame()
// ------------------------------
{}


// -- set functions ---------------------------------------------


// -- event handlers --------------------------------------------


// -----------------------------------------------
void PartitionFrame::onButton(wxCommandEvent& e)
// -----------------------------------------------
{
  if (e.GetId() == ID_BUTTON_CANCEL)
  {
    this->Close();
  }
  else if (e.GetId() == ID_BUTTON_OK)
  {
    int numParts = spinCtrlNumber->GetValue();
    int method   = -1;

    if (comboBoxMethod->GetValue() == wxT("Equal intervals"))
    {
      method = Attribute::PART_METH_EQUAL_INTERVALS;
    }
    else if (comboBoxMethod->GetValue() == wxT("Quantiles"))
    {
      method = Attribute::PART_METH_QUANTILES;
    }
    else if (comboBoxMethod->GetValue() == wxT("Mean-Standard deviation"))
    {
      method = Attribute::PART_METH_MEAN_STANDARD_DEVIATION;
    }

    mediator->handleAttrPartition(numParts, method);
    this->Close();
  }
}


// -------------------------------------------------
void PartitionFrame::onComboBox(wxCommandEvent& e)
// -------------------------------------------------
{
  if (e.GetId() == ID_COMBO_BOX_METHOD)
    {}
}


// ----------------------------------------------
void PartitionFrame::onSpinCtrl(wxSpinEvent& e)
// ----------------------------------------------
{
  if (e.GetId() == ID_SPIN_CTRL_NUMBER)
    {}
}


// -----------------------------------------------------
void PartitionFrame::onSpinCtrlText(wxCommandEvent& e)
// -----------------------------------------------------
{
  if (e.GetId() == ID_SPIN_CTRL_NUMBER)
    {}
}


// -- utility functions -----------------------------------------


// -- GUI initialization ----------------------------------------


// ----------------------------
void PartitionFrame::initFrame(
  wxString attrName,
  size_t minParts,
  size_t maxParts,
  size_t curParts)
// ----------------------------
{
  sizerFrame = new wxBoxSizer(wxVERTICAL);
  this->SetSizer(sizerFrame);

  panelFrame = new wxScrolledWindow(
    this,
    wxID_ANY,
    wxDefaultPosition,
    wxDefaultSize,
    wxHSCROLL |
    wxVSCROLL |
    wxRAISED_BORDER);
  sizerFrame->Add(panelFrame, 1 , wxEXPAND);

  sizerPanel = new wxBoxSizer(wxVERTICAL);
  panelFrame->SetSizer(sizerPanel);

  initWidgets(attrName, minParts, maxParts, curParts);
//    initCanvas();
  initButtons();
}


// ------------------------------
void PartitionFrame::initWidgets(
  wxString attrName,
  size_t minParts,
  size_t maxParts,
  size_t curParts)
// ------------------------------
{
  // init static box
  wxStaticBoxSizer* box = new wxStaticBoxSizer(
    wxVERTICAL,
    panelFrame,
    attrName);
  /*
  sizerPanel->Add(
      box,
      0,        // vert no stretch
      wxEXPAND  // hori stretch
      | wxALL,  // border around
      5 );
  */
  sizerPanel->Add(
    box,
    1,        // vert stretch
    wxEXPAND  // hori stretch
    | wxALL,  // border around
    5);

  // init grid sizer
  wxFlexGridSizer* lblSizer = new wxFlexGridSizer(
    2,        // rows
    2,        // cols
    0,        // vgap
    0);       // hgap
  // add grid sizer
  box->Add(
    lblSizer,
    0,        // vert not stretchable
    wxGROW);  // hori stretchable

  // number of partitions
  wxStaticText* label = new wxStaticText(panelFrame, wxID_ANY, wxT("Number of partitions: "));
  lblSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
  spinCtrlNumber = new wxSpinCtrl(
    panelFrame,
    ID_SPIN_CTRL_NUMBER,
    wxString(Utils::intToStr((int) curParts).c_str(), wxConvUTF8),
    wxDefaultPosition,
    wxDefaultSize,
    wxSP_ARROW_KEYS,
    (int) minParts,
    (int) maxParts,
    (int) curParts);
  lblSizer->Add(spinCtrlNumber, 0, wxALL, 5);

  // method
  label = new wxStaticText(panelFrame, wxID_ANY, wxT("Calculate using: "));
  lblSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

  wxArrayString vals;
  vals.Add(wxString(wxT("Equal intervals")));
  vals.Add(wxString(wxT("Quantiles")));
  vals.Add(wxString(wxT("Mean-Standard deviation")));
  comboBoxMethod = new wxComboBox(
    panelFrame,
    ID_COMBO_BOX_METHOD,
    wxString(wxT("Equal intervals")),
    wxDefaultPosition,
    wxDefaultSize,
    vals);
  lblSizer->Add(comboBoxMethod, 0, wxALL, 5);

  // reset ptrs
  lblSizer = NULL;
  box = NULL;
  label = NULL;
}


// ------------------------------
void PartitionFrame::initCanvas()
// ------------------------------
{
  canvas = new GLCanvas(mediator, panelFrame, wxID_ANY);
  canvas->SetMinSize(wxSize(150, 100));
  sizerPanel->Add(canvas, 1, wxEXPAND | wxALL, 5);
}


// -------------------------------
void PartitionFrame::initButtons()
// -------------------------------
{
  // init grid sizer
  wxFlexGridSizer* lblSizer = new wxFlexGridSizer(
    1,        // rows
    2,        // cols
    0,        // vgap
    0);       // hgap
  lblSizer->AddGrowableCol(0);
  // add grid sizer
  sizerPanel->Add(
    lblSizer,
    0,        // vert not stretchable
    wxGROW);  // hori stretchable

  buttonOK = new wxButton(panelFrame, ID_BUTTON_OK , wxT("OK"));
  lblSizer->Add(buttonOK, 0, wxALIGN_RIGHT | wxALL, 5);

  buttonCancel = new wxButton(panelFrame, ID_BUTTON_CANCEL , wxT("Cancel"));
  lblSizer->Add(buttonCancel, 0, wxALIGN_RIGHT | wxALL, 5);

  // reset ptr
  lblSizer = NULL;
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE(PartitionFrame, PopupFrame)
  EVT_BUTTON(ID_BUTTON_CANCEL, PartitionFrame::onButton)
  EVT_BUTTON(ID_BUTTON_OK, PartitionFrame::onButton)
  EVT_SPINCTRL(ID_SPIN_CTRL_NUMBER, PartitionFrame::onSpinCtrl)
  EVT_TEXT(ID_SPIN_CTRL_NUMBER, PartitionFrame::onSpinCtrlText)
  EVT_COMBOBOX(ID_COMBO_BOX_METHOD, PartitionFrame::onComboBox)
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
