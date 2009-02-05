// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./partitionframe.h

#ifndef PARTITIONFRAME_H
#define PARTITIONFRAME_H

#include <wx/wx.h>
#include <wx/spinctrl.h>
#include "attribute.h"
#include "glcanvas.h"
#include "popupframe.h"

class PartitionFrame : public PopupFrame
{
public:
    // -- constructors and desctructor ------------------------------
    PartitionFrame(
        Mediator* m,
        wxWindow* parent,
        wxWindowID id,
        wxString title,
        wxPoint position,
        wxSize size,
        wxString attrName,
        int minParts,
        int maxParts,
        int curParts );
    virtual ~PartitionFrame();

    // -- set functions ---------------------------------------------
    // INSERT

protected:
    // -- event handlers --------------------------------------------
    void onButton( wxCommandEvent &e );
    void onComboBox( wxCommandEvent &e );
    void onSpinCtrl( wxSpinEvent &e );
    void onSpinCtrlText( wxCommandEvent &e );

private:
    // -- utility functions -----------------------------------------
    // INSERT

    // -- GUI initialization ----------------------------------------
    void initFrame(
        wxString attrName,
        int minParts,
        int maxParts,
        int curParts );
    void initWidgets(
        wxString attrName,
        int minParts,
        int maxParts,
        int curParts );
    void initCanvas();
    void initButtons();

    wxBoxSizer*       sizerFrame;
    wxScrolledWindow* panelFrame;

    wxBoxSizer*       sizerPanel;
    wxFlexGridSizer*  sizerTop;

    wxSpinCtrl*       spinCtrlNumber;
    wxComboBox*       comboBoxMethod;
    GLCanvas*         canvas;
    wxButton*         buttonCancel;
    wxButton*         buttonOK;

    // -- ID's ------------------------------------------------------
    enum
    {
        ID_SPIN_CTRL_NUMBER,
        ID_COMBO_BOX_METHOD,
        ID_BUTTON_CANCEL,
        ID_BUTTON_OK
    };

    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
