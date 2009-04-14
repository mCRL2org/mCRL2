// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./settingsframe.cpp

#include "wx.hpp" // precompiled headers

#include "settingsframe.h"


// -- constructors and desctructor ----------------------------------


// --------------------------
SettingsFrame::SettingsFrame(
    Mediator* m,
    wxWindow* parent,
    wxWindowID id,
    wxString title,
    wxPoint position,
    wxSize size )
    : PopupFrame(
        m,
        parent,
        id,
        title,
        position,
        size )
// --------------------------
{
    SetMinSize( size );
    SetMaxSize( size );

    initFrame();
}


// ----------------------------
SettingsFrame::~SettingsFrame()
// ----------------------------
{}


// -- GUI initialization --------------------------------------------


// ----------------------------
void SettingsFrame::initFrame()
// ----------------------------
{
    sizerFrame = new wxBoxSizer( wxVERTICAL );
    this->SetSizer( sizerFrame );

    panelNotebook = new wxScrolledWindow( this, wxID_ANY );
    panelNotebook->SetWindowStyle( wxSUNKEN_BORDER );
    sizerFrame->Add( panelNotebook, 1 , wxEXPAND );

    sizerNotebook = new wxBoxSizer( wxVERTICAL );
    panelNotebook->SetSizer( sizerNotebook );

    notebook = new wxNotebook(
        panelNotebook,
        wxID_ANY );
    sizerNotebook->Add(
        notebook,
        1,
        wxEXPAND );

    submitButton = new wxButton(panelNotebook, ID_BUTTON_SUBMIT, wxString( wxT( "Submit")), wxDefaultPosition, wxDefaultSize );
    sizerNotebook->Add(submitButton, 0);

    initPanelGeneral();
    initPanelClustTree();
    initPanelBarTree();
    initPanelArcDiagram();
    initPanelSimulator();
    initPanelTrace();
    initPanelDgrmEditor();
}


// -----------------------------------
void SettingsFrame::initPanelGeneral()
// -----------------------------------
{
    // get settings
    wxColour colBG;
    wxColour colTxt;
    int szeTxt;
    wxString sizeVal;
    double animSpd;
    mediator->getSettingsGeneral( colBG, colTxt, szeTxt, animSpd );
    sizeVal = wxString(Utils::intToStr( szeTxt ).c_str(), wxConvLocal);

    // init panel
    panelGeneral = new wxScrolledWindow(
        notebook,
        ID_PANEL_GENERAL );
    panelGeneral->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelGeneral,
        wxString( wxT( "General") ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelGeneral->SetSizer( sizer );

    // background colour
    buttonColorBG = new wxColorButton(
        panelGeneral,
        this,
        ID_BUTTON_COL_BG );
    buttonColorBG->SetSizeHints(
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ),
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ) );
    buttonColorBG->SetWindowStyle( wxSIMPLE_BORDER );
    buttonColorBG->SetBackgroundColour( colBG );
    sizer->Add(
        new wxStaticText(
            panelGeneral,
            wxID_ANY,
            wxString( wxT( "Background color") ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        buttonColorBG,
        0,
        wxEXPAND |
        wxALL,
        10 );

    // text color
    buttonColorTxt = new wxColorButton(
        panelGeneral,
        this,
        ID_BUTTON_COL_TXT );
    buttonColorTxt->SetSizeHints(
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ),
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ) );
    buttonColorTxt->SetWindowStyle( wxSIMPLE_BORDER );
    buttonColorTxt->SetBackgroundColour( colTxt );
    sizer->Add(
        new wxStaticText(
            panelGeneral,
            wxID_ANY,
            wxString( wxT( "Text color")  ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        buttonColorTxt,
        0,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    // text size
    wxArrayString sizeVals;
    sizeVals.Add( wxString( wxT( "8" ) ) );
    sizeVals.Add( wxString( wxT( "10" ) ) );
    sizeVals.Add( wxString( wxT( "12" ) ) );
    sizeVals.Add( wxString( wxT( "14" ) ) );
    sizeVals.Add( wxString( wxT( "16" ) ) );
    sizeVals.Add( wxString( wxT( "18" ) ) );
    sizeVals.Add( wxString( wxT( "20" ) ) );
    sizeVals.Add( wxString( wxT( "24" ) ) );
    sizeVals.Add( wxString( wxT( "28" ) ) );
    sizeVals.Add( wxString( wxT( "30" ) ) );
    sizeVals.Add( wxString( wxT( "32" ) ) );
    comboBoxSizeTxt = new wxComboBox(
        panelGeneral,
        ID_COMBO_BOX_SIZE_TXT,
        wxString( wxT("") ),
        wxDefaultPosition,
        wxDefaultSize,
        sizeVals );
    comboBoxSizeTxt->SetValue( sizeVal );
    sizer->Add(
        new wxStaticText(
            panelGeneral,
            wxID_ANY,
            wxString( wxT("Text size") ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        comboBoxSizeTxt,
        0,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    // animation speed
    spinCtrlAnimSpd = new wxSpinCtrlFloat(
        panelGeneral,
        ID_SPIN_CTRL_ANIM_SPD,
        1.0,
        40.0,
        0.5,
        animSpd );
    spinCtrlAnimSpd->SetSizeHints(
        wxButton::GetDefaultSize(),
        wxButton::GetDefaultSize() );
    sizer->Add(
        new wxStaticText(
            panelGeneral,
            wxID_ANY,
            wxString( wxT( "Animation speed")  ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        spinCtrlAnimSpd,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    sizer = NULL;
}


// -------------------------------------
void SettingsFrame::initPanelClustTree()
// -------------------------------------
{
    // get settings
    bool show;
    bool annotate;
    int  colMap;
    wxString colVal;
    mediator->getSettingsClustTree( show, annotate, colMap );
    if ( colMap == VisUtils::COL_MAP_QUAL_PAST_1 )
        colVal = wxT( "Pastel 1 (Qual)" );
    else if ( colMap == VisUtils::COL_MAP_QUAL_PAST_2 )
        colVal = wxT( "Pastel 2 (Qual)" );
    else if ( colMap == VisUtils::COL_MAP_QUAL_SET_1 )
        colVal = wxT( "Set 1 (Qual)");
    else if ( colMap == VisUtils::COL_MAP_QUAL_SET_2 )
        colVal = wxT( "Set 2 (Qual)");
    else if ( colMap == VisUtils::COL_MAP_QUAL_SET_3 )
        colVal = wxT( "Set 3 (Qual)");
    else if ( colMap == VisUtils::COL_MAP_QUAL_PAIR )
        colVal = wxT( "Paired (Qual)");
    else if ( colMap == VisUtils::COL_MAP_QUAL_DARK )
        colVal = wxT( "Dark (Qual)");
    else if ( colMap == VisUtils::COL_MAP_QUAL_ACCENT )
        colVal = wxT( "Accents (Qual)");

    // init panel
    panelClustTree = new wxScrolledWindow(
        notebook,
        ID_PANEL_CLUST_TREE );
    panelClustTree->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelClustTree,
        wxString( wxT( "Cluster tree" ) ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelClustTree->SetSizer( sizer );

    // show
    checkBoxShowCT = new wxCheckBox(
        panelClustTree,
        ID_CHECK_BOX_SHOW_CT,
        wxString( wxT("") ) );
    checkBoxShowCT->SetValue( show );
    sizer->Add(
        new wxStaticText(
            panelClustTree,
            wxID_ANY,
            wxString( wxT( "Show") ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        checkBoxShowCT,
        1,
        wxEXPAND  |
        wxALL,
        10 );

    // annotate
    checkBoxAnnotateCT = new wxCheckBox(
        panelClustTree,
        ID_CHECK_BOX_ANNOTATE_CT,
        wxString( wxT("") ) );
    checkBoxAnnotateCT->SetValue( annotate );
    sizer->Add(
        new wxStaticText(
            panelClustTree,
            wxID_ANY,
            wxString( wxT( "Annotate" ) ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        checkBoxAnnotateCT,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    // color map
    wxArrayString colVals;
    colVals.Add( wxString( wxT( "Pastel 1 (Qual)") ) );
    colVals.Add( wxString( wxT( "Pastel 2 (Qual)") ) );
    colVals.Add( wxString( wxT( "Set 1 (Qual)") ) );
    colVals.Add( wxString( wxT( "Set 2 (Qual)") ) );
    colVals.Add( wxString( wxT( "Set 3 (Qual)") ) );
    colVals.Add( wxString( wxT( "Paired (Qual)") ) );
    colVals.Add( wxString( wxT( "Dark (Qual)") ) );
    colVals.Add( wxString( wxT( "Accents (Qual)") ) );
    comboBoxColMap = new wxComboBox(
        panelClustTree,
        ID_COMBO_BOX_COL_MAP,
        wxString( wxT( "") ),
        wxDefaultPosition,
        wxDefaultSize,
        colVals );
    comboBoxColMap->SetValue( colVal );
    sizer->Add(
        new wxStaticText(
            panelClustTree,
            wxID_ANY,
            wxString( wxT("Color mapping" ) ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        comboBoxColMap,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    sizer = NULL;
}


// -----------------------------------
void SettingsFrame::initPanelBarTree()
// ------------------------------------
{
    // get settings
    bool   show;
    double magn;
    mediator->getSettingsBarTree( show, magn );

    // init panel
    panelBarTree = new wxScrolledWindow(
        notebook,
        ID_PANEL_BAR_TREE );
    panelBarTree->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelBarTree,
        wxString( wxT( "Bar tree" ) ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelBarTree->SetSizer( sizer );

    // show
    checkBoxShowBT = new wxCheckBox(
        panelBarTree,
        ID_CHECK_BOX_SHOW_BT,
        wxString( wxT("") ) );
    checkBoxShowBT->SetValue( show );
    sizer->Add(
        new wxStaticText(
            panelBarTree,
            wxID_ANY,
            wxString( wxT( "Show" ) ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        checkBoxShowBT,
        1,
        wxEXPAND  |
        wxALL,
        10 );

    // magnification
    spinCtrlMagnBT = new wxSpinCtrlFloat(
        panelBarTree,
        ID_SPIN_CTRL_MAGN_BT,
        0.0,
        40.0,
        0.5,
        magn );
    spinCtrlMagnBT->SetSizeHints(
        wxButton::GetDefaultSize(),
        wxButton::GetDefaultSize() );
    sizer->Add(
        new wxStaticText(
            panelBarTree,
            wxID_ANY,
            wxString( wxT( "Magnification" ) ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        spinCtrlMagnBT,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    sizer = NULL;
}


// --------------------------------------
void SettingsFrame::initPanelArcDiagram()
// --------------------------------------
{
    // get settings
    bool     showNodes;
    bool     showArcs;
    wxColour colBndl;
    double   trspBndl;
    mediator->getSettingsArcDiagram(
        showNodes,
        showArcs,
        colBndl,
        trspBndl );

    // init panel
    panelArcDiagram = new wxScrolledWindow(
        notebook,
        ID_PANEL_ARC_DIAGRAM );
    panelArcDiagram->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelArcDiagram,
        wxString( wxT( "Arc diagram") ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelArcDiagram->SetSizer( sizer );

    // show nodes
    checkBoxShowNodes = new wxCheckBox(
        panelArcDiagram,
        ID_CHECK_BOX_SHOW_NODES,
        wxString( wxT( "" ) ) );
    checkBoxShowNodes->SetValue( showNodes );
    sizer->Add(
        new wxStaticText(
            panelArcDiagram,
            wxID_ANY,
            wxString( wxT( "Show nodes" ) ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        checkBoxShowNodes,
        1,
        wxEXPAND  |
        wxALL,
        10 );

    // show arcs
    checkBoxShowArcs = new wxCheckBox(
        panelArcDiagram,
        ID_CHECK_BOX_SHOW_ARCS,
        wxString( wxT( "" ) ) );
    checkBoxShowArcs->SetValue( showArcs );
    sizer->Add(
        new wxStaticText(
            panelArcDiagram,
            wxID_ANY,
            wxString( wxT( "Show arcs") ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        checkBoxShowArcs,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    // bundle color
    buttonColorArcs = new wxColorButton(
        panelArcDiagram,
        this,
        ID_BUTTON_COL_ARCS );
    buttonColorArcs->SetSizeHints(
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ),
        wxSize(
            wxButton::GetDefaultSize().GetHeight(),
		    wxButton::GetDefaultSize().GetHeight() ) );
    buttonColorArcs->SetWindowStyle( wxSIMPLE_BORDER );
    buttonColorArcs->SetBackgroundColour( colBndl );
    sizer->Add(
        new wxStaticText(
            panelArcDiagram,
            wxID_ANY,
            wxString( wxT("Arc color") ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        buttonColorArcs,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    spinCtrlTrspArcs = new wxSpinCtrlFloat(
        panelArcDiagram,
        ID_SPIN_CTRL_TRSP_ARCS,
        0.0,
        1.0,
        0.1,
        trspBndl );
    spinCtrlTrspArcs->SetSizeHints(
        wxButton::GetDefaultSize(),
        wxButton::GetDefaultSize() );
    sizer->Add(
        new wxStaticText(
            panelArcDiagram,
            wxID_ANY,
            wxString( wxT("Arc transparency") ) ),
        1,
        wxEXPAND |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );
    sizer->Add(
        spinCtrlTrspArcs,
        1,
        wxEXPAND  |
        wxLEFT | wxRIGHT | wxBOTTOM,
        10 );

    sizer = NULL;

}


// -------------------------------------
void SettingsFrame::initPanelSimulator()
// -------------------------------------
{
    int blendType;
    wxString blendVal;

    mediator->getSettingsSimulator( blendType );
    if ( blendType == VisUtils::BLEND_HARD )
        blendVal = wxT("Hard transition");
    else if ( blendType == VisUtils::BLEND_LINEAR )
        blendVal = wxT("Linear");
    else if ( blendType == VisUtils::BLEND_CONCAVE )
        blendVal = wxT("Concave");
    else if ( blendType == VisUtils::BLEND_CONVEX )
        blendVal = wxT("Convex");
    else if ( blendType == VisUtils::BLEND_OSCILLATE )
        blendVal = wxT("Oscillate");

    // init panel
    panelSimulator = new wxScrolledWindow(
        notebook,
        ID_PANEL_SIMULATOR );
    panelSimulator->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelSimulator,
        wxString( wxT( "Simulation" ) ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelSimulator->SetSizer( sizer );

    // blend mode
    wxArrayString blendVals;
    blendVals.Add( wxString( wxT( "Hard transition" ) ) );
    blendVals.Add( wxString( wxT( "Linear" ) ) );
    blendVals.Add( wxString( wxT( "Concave" ) ) );
    blendVals.Add( wxString( wxT( "Convex") ) );
    blendVals.Add( wxString( wxT( "Oscillate") ) );

    comboBoxBlendType = new wxComboBox(
        panelSimulator,
        ID_COMBO_BOX_BLEND_TYPE,
        wxString( wxT ("") ),
        wxDefaultPosition,
        wxDefaultSize,
        blendVals );
    comboBoxBlendType->SetValue( blendVal );
    sizer->Add(
        new wxStaticText(
            panelSimulator,
            wxID_ANY,
            wxString( wxT( "Blend type") ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        comboBoxBlendType,
        1,
        wxEXPAND  |
        wxALL,
        10 );

    sizer = NULL;
}


// ---------------------------------
void SettingsFrame::initPanelTrace()
// ---------------------------------
{
    bool useShading;
    mediator->getSettingsTrace( useShading );

    panelTrace = new wxScrolledWindow(
        notebook,
        ID_PANEL_TRACE );
    panelTrace->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelTrace,
        wxString( wxT( "Trace" ) ) );

    wxFlexGridSizer* sizer = new wxFlexGridSizer( 2 );
    sizer->AddGrowableCol( 1 );
    panelTrace->SetSizer( sizer );

    // use shading
    /*
    checkBoxUseShading = new wxCheckBox(
        panelTrace,
        ID_CHECK_BOX_USE_SHADING,
        wxString( "" ) );
    checkBoxUseShading->SetValue( useShading );
    sizer->Add(
        new wxStaticText(
            panelTrace,
            wxID_ANY,
            wxString( "Use shading" ) ),
        1,
        wxEXPAND |
        wxALL,
        10 );
    sizer->Add(
        checkBoxUseShading,
        1,
        wxEXPAND  |
        wxALL,
        10 );
    */
}


// --------------------------------------
void SettingsFrame::initPanelDgrmEditor()
// --------------------------------------
{
    panelDgrmEditor = new wxScrolledWindow(
        notebook,
        ID_PANEL_DGRM_EDITOR );
    panelDgrmEditor->SetScrollRate( 10, 10 );
    notebook->AddPage(
        panelDgrmEditor,
        wxString( wxT( "Diagram editor" ) ) );
}


// -- set functions -------------------------------------------------


// -----------------------------
void SettingsFrame::setGeneral()
// -----------------------------
{
    notebook->SetSelection( 0 );
}


// -------------------------------
void SettingsFrame::setClustTree()
// -------------------------------
{
    notebook->SetSelection( 1 );
}


// -----------------------------
void SettingsFrame::setBarTree()
// -----------------------------
{
    notebook->SetSelection( 2 );
}


// --------------------------------
void SettingsFrame::setArcDiagram()
// --------------------------------
{
    notebook->SetSelection( 3 );
}


// -------------------------------
void SettingsFrame::setSimulator()
// -------------------------------
{
    notebook->SetSelection( 4 );
}


// ---------------------------
void SettingsFrame::setTrace()
// ---------------------------
{
    notebook->SetSelection( 5 );
}


// --------------------------------
void SettingsFrame::setDgrmEditor()
// --------------------------------
{
    notebook->SetSelection( 6 );
}


// -- event handlers ------------------------------------------------


// ----------------------------------------------
void SettingsFrame::onButton( wxCommandEvent &e )
// ----------------------------------------------
{
    if( e.GetId() == ID_BUTTON_SUBMIT)
    {
	updateSettingsGeneral();
	updateSettingsClustTree();
	updateSettingsBarTree();
	updateSettingsArcDiagram();
	updateSettingsTrace();
	updateSettingsSimulator();
	updateSettingsDgrmEditor();
	this->Close( false );
    }
    /*else if ( e.GetId() == ID_BUTTON_COL_BG )
        updateSettingsGeneral();
    else if ( e.GetId() == ID_BUTTON_COL_TXT )
        updateSettingsGeneral();
    else if ( e.GetId() == ID_BUTTON_COL_ARCS )
        updateSettingsArcDiagram();*/
}


// ------------------------------------------------
void SettingsFrame::onCheckBox( wxCommandEvent &e )
// ------------------------------------------------
{
    /*if ( e.GetId() == ID_CHECK_BOX_SHOW_CT )
        updateSettingsClustTree();
    if ( e.GetId() == ID_CHECK_BOX_ANNOTATE_CT )
        updateSettingsClustTree();
    else if ( e.GetId() == ID_CHECK_BOX_SHOW_BT )
        updateSettingsBarTree();
    else if ( e.GetId() == ID_CHECK_BOX_SHOW_ARCS )
        updateSettingsArcDiagram();
    else if ( e.GetId() == ID_CHECK_BOX_SHOW_NODES )
        updateSettingsArcDiagram();
    else if ( e.GetId() == ID_CHECK_BOX_USE_SHADING )
        updateSettingsTrace();*/
}


// ------------------------------------------------
void SettingsFrame::onComboBox( wxCommandEvent &e )
// ------------------------------------------------
{
    /*if ( e.GetId() == ID_COMBO_BOX_SIZE_TXT )
        updateSettingsGeneral();
    else if ( e.GetId() == ID_COMBO_BOX_COL_MAP )
        updateSettingsClustTree();
    else if ( e.GetId() == ID_COMBO_BOX_BLEND_TYPE )
        updateSettingsSimulator();*/
}


// ---------------------------------------------
void SettingsFrame::onSpinCtrl( wxSpinEvent &e )
// ---------------------------------------------
{
    /*if ( e.GetId() == ID_SPIN_CTRL_ANIM_SPD )
        updateSettingsGeneral();
    else if ( e.GetId() == ID_SPIN_CTRL_MAGN_BT )
        updateSettingsBarTree();
    else if ( e.GetId() == ID_SPIN_CTRL_TRSP_ARCS )
        updateSettingsArcDiagram();*/
}


// -- utility functions -----------------------------------------


// ----------------------------------------
void SettingsFrame::updateSettingsGeneral()
// ----------------------------------------
{
    wxColour colClr = buttonColorBG->GetBackgroundColour();
    wxColour colTxt = buttonColorTxt->GetBackgroundColour();
    wxString szeVal = comboBoxSizeTxt->GetValue();
    int szeTxt      = Utils::strToInt( std::string(szeVal.fn_str()) );
    double spdAnim  = spinCtrlAnimSpd->GetValue();
    mediator->setSettingsGeneral( colClr, colTxt, szeTxt, spdAnim );
}


// ------------------------------------------
void SettingsFrame::updateSettingsClustTree()
// ------------------------------------------
{
    bool     show     = checkBoxShowCT->GetValue();
    bool     annotate = checkBoxAnnotateCT->GetValue();
    wxString colVal   = comboBoxColMap->GetValue();
    int      colMap   = 0;

    if ( colVal == wxT( "Pastel 1 (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_PAST_1;
    else if ( colVal == wxT( "Pastel 2 (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_PAST_2;
    else if ( colVal == wxT( "Set 1 (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_SET_1;
    else if ( colVal == wxT( "Set 2 (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_SET_2;
    else if ( colVal == wxT( "Set 3 (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_SET_3;
    else if ( colVal == wxT( "Paired (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_PAIR;
    else if ( colVal == wxT( "Dark (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_DARK;
    else if ( colVal == wxT( "Accents (Qual)") )
        colMap = VisUtils::COL_MAP_QUAL_ACCENT;

    mediator->setSettingsClustTree( show, annotate, colMap );
}


// ----------------------------------------
void SettingsFrame::updateSettingsBarTree()
// ----------------------------------------
{
    bool   show = checkBoxShowBT->GetValue();
    double magn = spinCtrlMagnBT->GetValue();

    mediator->setSettingsBarTree( show, magn );
}


// -------------------------------------------
void SettingsFrame::updateSettingsArcDiagram()
// -------------------------------------------
{
    bool     showNodes = checkBoxShowNodes->GetValue();
    bool     showArcs  = checkBoxShowArcs->GetValue();
    wxColour colArcs   = buttonColorArcs->GetBackgroundColour();
    double   trspArcs  = spinCtrlTrspArcs->GetValue();

    mediator->setSettingsArcDiagram(
        showNodes,
        showArcs,
        colArcs,
        trspArcs );
}


// ------------------------------------------
void SettingsFrame::updateSettingsSimulator()
// ------------------------------------------
{
    wxString blendVal;
    int blendType;

    blendVal   = comboBoxBlendType->GetValue();

    if ( blendVal == wxT("Hard transition") )
        blendType = VisUtils::BLEND_HARD;
    else if ( blendVal == wxT( "Linear") )
        blendType = VisUtils::BLEND_LINEAR;
    else if ( blendVal == wxT( "Concave" ) )
        blendType = VisUtils::BLEND_CONCAVE;
    else if ( blendVal == wxT ("Convex" ) )
        blendType = VisUtils::BLEND_CONVEX;
    else if ( blendVal == wxT( "Oscillate" ) )
        blendType = VisUtils::BLEND_OSCILLATE;

    mediator->setSettingsSimulator( blendType );
}


// --------------------------------------
void SettingsFrame::updateSettingsTrace()
// --------------------------------------
{
    /*bool useShading;
    useShading = checkBoxUseShading->GetValue();
    mediator->setSettingsTrace( useShading );*/
}


// -------------------------------------------
void SettingsFrame::updateSettingsDgrmEditor()
// -------------------------------------------
{}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE( SettingsFrame, PopupFrame )
    EVT_BUTTON( ID_BUTTON_SUBMIT, SettingsFrame::onButton )
    EVT_BUTTON( ID_BUTTON_COL_BG, SettingsFrame::onButton )
    EVT_BUTTON( ID_BUTTON_COL_TXT, SettingsFrame::onButton )
    EVT_COMBOBOX( ID_COMBO_BOX_SIZE_TXT, SettingsFrame::onComboBox )
    EVT_SPINCTRL( ID_SPIN_CTRL_ANIM_SPD, SettingsFrame::onSpinCtrl )
    EVT_CHECKBOX( ID_CHECK_BOX_SHOW_CT, SettingsFrame::onCheckBox )
    EVT_CHECKBOX( ID_CHECK_BOX_ANNOTATE_CT, SettingsFrame::onCheckBox )
    EVT_COMBOBOX( ID_COMBO_BOX_COL_MAP, SettingsFrame::onComboBox )
    EVT_CHECKBOX( ID_CHECK_BOX_SHOW_BT, SettingsFrame::onCheckBox )
    EVT_SPINCTRL( ID_SPIN_CTRL_MAGN_BT, SettingsFrame::onSpinCtrl )
    EVT_CHECKBOX( ID_CHECK_BOX_SHOW_NODES, SettingsFrame::onCheckBox )
    EVT_CHECKBOX( ID_CHECK_BOX_SHOW_ARCS, SettingsFrame::onCheckBox )
    EVT_BUTTON( ID_BUTTON_COL_ARCS, SettingsFrame::onButton )
    EVT_SPINCTRL( ID_SPIN_CTRL_TRSP_ARCS, SettingsFrame::onSpinCtrl )
    EVT_CHECKBOX( ID_CHECK_BOX_USE_SHADING, SettingsFrame::onCheckBox )
    EVT_COMBOBOX( ID_COMBO_BOX_BLEND_TYPE, SettingsFrame::onComboBox )
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
