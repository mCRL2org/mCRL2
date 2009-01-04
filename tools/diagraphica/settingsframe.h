// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./settingsframe.h

#ifndef SETTINGSFRAME_H
#define SETTINGSFRAME_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "glcanvas.h"
#include "popupframe.h"
#include "mcrl2/utilities/colorbutton.h"
#include "mcrl2/utilities/spinctrlfloat.h"

using mcrl2::utilities::wx::wxColorButton;
using mcrl2::utilities::wx::wxSpinCtrlFloat;

class SettingsFrame : public PopupFrame
{
public:
    // -- constructors and desctructor ------------------------------
    SettingsFrame(
        Mediator* m,
        wxWindow* parent,
        wxWindowID id,
        wxString title,
        wxPoint position,
        wxSize size );
    virtual ~SettingsFrame();

    // -- set functions ---------------------------------------------
    void setGeneral();
    void setClustTree();
    void setBarTree();
    void setArcDiagram();
    void setSimulator();
    void setTrace();
    void setDgrmEditor();

protected:
    // -- event handlers --------------------------------------------
    void onButton( wxCommandEvent &e );
    void onCheckBox( wxCommandEvent &e );
    void onComboBox( wxCommandEvent &e );
    void onSpinCtrl( wxSpinEvent &e );
    
private:
    // -- utility functions -----------------------------------------
    void updateSettingsGeneral();
    void updateSettingsClustTree();
    void updateSettingsBarTree();
    void updateSettingsArcDiagram();
    void updateSettingsSimulator();
    void updateSettingsTrace();
    void updateSettingsDgrmEditor();

    // -- GUI initialization ----------------------------------------
    void initFrame();
    void initPanelGeneral();
    void initPanelClustTree();
    void initPanelBarTree();
    void initPanelArcDiagram();
    void initPanelSimulator();
    void initPanelTrace();
    void initPanelDgrmEditor();

    wxBoxSizer*       sizerFrame;
    wxScrolledWindow* panelNotebook;
    wxBoxSizer*       sizerNotebook;
    wxNotebook*       notebook;
    wxButton*	      submitButton;
    
    wxScrolledWindow* panelGeneral;
    wxColorButton*    buttonColorBG;
    wxColorButton*    buttonColorTxt;
    wxComboBox*       comboBoxSizeTxt;
    wxSpinCtrlFloat*  spinCtrlAnimSpd;
    
    wxScrolledWindow* panelClustTree;
    wxCheckBox*       checkBoxShowCT;
    wxCheckBox*       checkBoxAnnotateCT;
    wxComboBox*       comboBoxColMap;

    wxScrolledWindow* panelBarTree;
    wxCheckBox*       checkBoxShowBT;
    wxSpinCtrlFloat*  spinCtrlMagnBT;
    
    wxScrolledWindow* panelArcDiagram;
    wxCheckBox*       checkBoxShowNodes;
    wxCheckBox*       checkBoxShowArcs;
    wxColorButton*    buttonColorArcs;
    wxSpinCtrlFloat*  spinCtrlTrspArcs;

    wxScrolledWindow* panelSimulator;
    wxComboBox*       comboBoxBlendType;

    wxScrolledWindow* panelTrace;
    wxCheckBox*       checkBoxUseShading;

    wxScrolledWindow* panelDgrmEditor;

    // -- ID's ------------------------------------------------------
    enum
    {
        ID_PANEL_GENERAL,
	ID_BUTTON_SUBMIT,
        ID_BUTTON_COL_BG,
        ID_BUTTON_COL_TXT,
        ID_COMBO_BOX_SIZE_TXT,
        ID_SPIN_CTRL_ANIM_SPD,
        
        ID_PANEL_CLUST_TREE,
        ID_CHECK_BOX_SHOW_CT,
        ID_CHECK_BOX_ANNOTATE_CT,
        ID_COMBO_BOX_COL_MAP,
        
        ID_PANEL_BAR_TREE,
        ID_CHECK_BOX_SHOW_BT,
        ID_SPIN_CTRL_MAGN_BT,
        
        ID_PANEL_ARC_DIAGRAM,
        ID_CHECK_BOX_SHOW_NODES,
        ID_CHECK_BOX_SHOW_ARCS,
        ID_BUTTON_COL_ARCS,
        ID_SPIN_CTRL_TRSP_ARCS,

        ID_PANEL_SIMULATOR,
        ID_COMBO_BOX_BLEND_TYPE,

        ID_PANEL_TRACE,
        ID_CHECK_BOX_USE_SHADING,
        
        ID_PANEL_DGRM_EDITOR
    };
    
    // -- declare event table ---------------------------------------
    DECLARE_EVENT_TABLE()
};

#endif

// -- end -----------------------------------------------------------
