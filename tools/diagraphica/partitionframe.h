//  Author(s): A.j. (Hannes) pretorius
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./partitionframe.h

// --- partitionframe.h ---------------------------------------------
// (c) 2007  -  A.J. Pretorius  -  Eindhoven University of Technology
// ---------------------------  *  ----------------------------------

#ifndef PARTITIONFRAME_H
#define PARTITIONFRAME_H

#include <vector>
using namespace std;
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
        ID_BUTTON_OK,
    };
    
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
