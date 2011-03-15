// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "wx.hpp" // precompiled headers

#include "settingsdialog.h"
#include <wx/notebook.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>
#include <wx/statline.h>
#include "mcrl2/utilities/colorbutton.h"
#include "enums.h"
#include "glcanvas.h"
#include "settings.h"
#include "mathutils.h"
#include "ids.h"
#include "rgb_color.h"

using mcrl2::utilities::wx::wxColorButton;

using namespace IDs;

BEGIN_EVENT_TABLE(SettingsDialog,wxDialog)
  EVT_BUTTON(myID_BACKGROUND_CLR,SettingsDialog::onBackgroundClrButton)
  EVT_BUTTON(myID_DOWN_EDGE_CLR,SettingsDialog::onDownEdgeClrButton)
  EVT_BUTTON(myID_INTERPOLATE_CLR_1,SettingsDialog::onInterpolateClr1Button)
  EVT_BUTTON(myID_INTERPOLATE_CLR_2,SettingsDialog::onInterpolateClr2Button)
  EVT_BUTTON(myID_MARK_CLR,SettingsDialog::onMarkClrButton)
  EVT_BUTTON(myID_STATE_CLR,SettingsDialog::onStateClrButton)
  EVT_BUTTON(myID_UP_EDGE_CLR,SettingsDialog::onUpEdgeClrButton)
  EVT_BUTTON(myID_SIM_CURR_CLR,SettingsDialog::onSimCurrClrButton)
  EVT_BUTTON(myID_SIM_POS_CLR,SettingsDialog::onSimPosClrButton)
  EVT_BUTTON(myID_SIM_SEL_CLR,SettingsDialog::onSimSelClrButton)
  EVT_BUTTON(myID_SIM_PREV_CLR,SettingsDialog::onSimPrevClrButton)
  EVT_SPINCTRL(myID_BRANCH_ROTATION,SettingsDialog::onBranchRotationSpin)
  EVT_SPINCTRL(myID_STATE_SIZE,SettingsDialog::onStateSizeSpin)
  EVT_SPINCTRL(myID_CLUSTER_HEIGHT,SettingsDialog::onClusterHeightSpin)
  EVT_SPINCTRL(myID_BRANCH_TILT,SettingsDialog::onBranchTiltSpin)
  EVT_SPINCTRL(myID_QUALITY,SettingsDialog::onQualitySpin)
  EVT_SPINCTRL(myID_TRANSPARENCY,SettingsDialog::onTransparencySpin)
  EVT_CHECKBOX(myID_LONG_INTERPOLATION,SettingsDialog::onLongInterpolationCheck)
  EVT_CHECKBOX(myID_NAV_SHOW_BACKPOINTERS,
               SettingsDialog::onNavShowBackpointersCheck)
  EVT_CHECKBOX(myID_NAV_SHOW_STATES,SettingsDialog::onNavShowStatesCheck)
  EVT_CHECKBOX(myID_NAV_SHOW_TRANSITIONS,
               SettingsDialog::onNavShowTransitionsCheck)
  EVT_CHECKBOX(myID_NAV_SMOOTH_SHADING,SettingsDialog::onNavSmoothShadingCheck)
  EVT_CHECKBOX(myID_NAV_LIGHTING,SettingsDialog::onNavLightingCheck)
  EVT_CHECKBOX(myID_NAV_TRANSPARENCY,SettingsDialog::onNavTransparencyCheck)
  EVT_RADIOBUTTON(myID_ITERATIVE, SettingsDialog::onIterativeRadio)
  EVT_RADIOBUTTON(myID_CYCLIC, SettingsDialog::onCyclicRadio)
  EVT_RADIOBUTTON(myID_CONES_STYLE, SettingsDialog::onConesRadio)
  EVT_RADIOBUTTON(myID_TUBES_STYLE, SettingsDialog::onTubesRadio)
  EVT_CHECKBOX(myID_FSM_STYLE,SettingsDialog::onFsmStyleCheck)
  EVT_RADIOBUTTON(myID_SP_STATEPOS, SettingsDialog::onSinglePassRadio)
  EVT_RADIOBUTTON(myID_MP_STATEPOS, SettingsDialog::onMultiPassRadio)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent,GLCanvas* glc,Settings* ss)
  : wxDialog(parent,wxID_ANY,wxT("Settings"),wxDefaultPosition)
{
  glCanvas = glc;
  settings = ss;

  settings->subscribe(ClusterHeight,this);

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxNotebook* nb = new wxNotebook(this,wxID_ANY);
  wxPanel* parPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* clrPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* simPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* pfmPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* algPanel = new wxPanel(nb,wxID_ANY);

  setupParametersPanel(parPanel);
  setupColourPanel(clrPanel);
  setupSimulationPanel(simPanel);
  setupAlgorithmsPanel(algPanel);
  setupPerformancePanel(pfmPanel);

  nb->AddPage(parPanel,wxT("Parameters"), true);
  nb->AddPage(clrPanel,wxT("Colours"), false);
  nb->AddPage(simPanel,wxT("Simulation"), false);
  nb->AddPage(algPanel,wxT("Algorithms"), false);
  nb->AddPage(pfmPanel,wxT("Performance"), false);

  sizer->Add(nb,0,wxEXPAND|wxALL,5);
  SetSizerAndFit(sizer);
  Layout();
  SetSize(wxSize(400,-1));
}

void SettingsDialog::setupParametersPanel(wxPanel* panel)
{
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int rf = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL;
  int bd = 5;
  wxSize spinSize(65,-1);
  wxSize sliderSize(200,-1);
  long spinStyle = wxSP_ARROW_KEYS;

  wxFlexGridSizer* sizer = new wxFlexGridSizer(6,2,0,0);
  sizer->AddGrowableCol(0);
  sizer->AddGrowableRow(5);

  wxSpinCtrl* ssSpin = new wxSpinCtrl(panel,myID_STATE_SIZE,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle,0,1000,
                                      int(10*settings->getFloat(StateSize)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("State size:")),0,lf,bd);
  sizer->Add(ssSpin,0,rf,bd);

  wxSpinCtrl* chSpin = new wxSpinCtrl(panel,myID_CLUSTER_HEIGHT,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle,0,1000000,
                                      int(10*settings->getFloat(ClusterHeight)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Cluster height:")),0,lf,bd);
  sizer->Add(chSpin,0,rf,bd);

  wxSpinCtrl* brSpin = new wxSpinCtrl(panel,myID_BRANCH_ROTATION,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle|wxSP_WRAP,0,359,
                                      settings->getInt(BranchRotation));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Branch rotation:")),0,lf,bd);
  sizer->Add(brSpin,0,rf,bd);

  wxSpinCtrl* btSpin = new wxSpinCtrl(panel,myID_BRANCH_TILT,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle,0,90,settings->getInt(BranchTilt));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Branch tilt:")),0,lf,bd);
  sizer->Add(btSpin,0,rf,bd);

  wxSpinCtrl* qlSpin = new wxSpinCtrl(panel,myID_QUALITY,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle,2,50,settings->getInt(Quality)/2);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Accuracy:")),0,lf,bd);
  sizer->Add(qlSpin,0,rf,bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupColourPanel(wxPanel* panel)
{
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int rf = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL;
  int bd = 5;
  wxSize spinSize(65,-1);
  long spinStyle = wxSP_ARROW_KEYS;
  wxSize btnSize(25,25);

  wxFlexGridSizer* sizer = new wxFlexGridSizer(9,3,0,0);
  sizer->AddGrowableCol(0);
  sizer->AddGrowableRow(8);

  wxSpinCtrl* trSpin = new wxSpinCtrl(panel,myID_TRANSPARENCY,wxEmptyString,
                                      wxDefaultPosition,spinSize,spinStyle,0,100,
                                      static_cast<int>((255-settings->getInt(Alpha))/2.55f));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transparency:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(trSpin,0,rf,bd);

  wxColorButton* bgButton = new wxColorButton(panel,this,myID_BACKGROUND_CLR,
      wxDefaultPosition,btnSize);
  bgButton->SetBackgroundColour(settings->getRGB(BackgroundColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Background:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(bgButton,0,rf,bd);

  wxColorButton* ndButton = new wxColorButton(panel,this,myID_STATE_CLR,
      wxDefaultPosition,btnSize);
  ndButton->SetBackgroundColour(settings->getRGB(StateColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("State:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(ndButton,0,rf,bd);

  wxColorButton* deButton = new wxColorButton(panel,this,myID_DOWN_EDGE_CLR,
      wxDefaultPosition,btnSize);
  deButton->SetBackgroundColour(settings->getRGB(DownEdgeColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transition:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(deButton,0,rf,bd);

  wxColorButton* ueButton = new wxColorButton(panel,this,myID_UP_EDGE_CLR,
      wxDefaultPosition,btnSize);
  ueButton->SetBackgroundColour(settings->getRGB(UpEdgeColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Backpointer:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(ueButton,0,rf,bd);

  wxColorButton* mkButton = new wxColorButton(panel,this,myID_MARK_CLR,
      wxDefaultPosition,btnSize);
  mkButton->SetBackgroundColour(settings->getRGB(MarkedColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Mark:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(mkButton,0,rf,bd);

  wxColorButton* i1Btn = new wxColorButton(panel,this,myID_INTERPOLATE_CLR_1,
      wxDefaultPosition,btnSize);
  wxColorButton* i2Btn = new wxColorButton(panel,this,myID_INTERPOLATE_CLR_2,
      wxDefaultPosition,btnSize);
  i1Btn->SetBackgroundColour(settings->getRGB(InterpolateColor1).toWxColour());
  i2Btn->SetBackgroundColour(settings->getRGB(InterpolateColor2).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Interpolate:")),0,lf,bd);
  sizer->Add(i1Btn,0,rf,bd);
  sizer->Add(i2Btn,0,rf,bd);

  wxCheckBox* liCheck = new wxCheckBox(panel,myID_LONG_INTERPOLATION,
                                       wxT("Long interpolation"));
  liCheck->SetValue(settings->getBool(LongInterpolation));
  sizer->Add(liCheck,0,lf,bd);
  sizer->AddSpacer(0);
  sizer->AddSpacer(0);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupSimulationPanel(wxPanel* panel)
{
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int rf = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL;
  int bd = 5;

  wxSize btnSize(25,25);
  wxFlexGridSizer* sizer = new wxFlexGridSizer(5,2,0,0);
  sizer->AddGrowableCol(0);
  sizer->AddGrowableRow(4);

  wxColorButton* shButton = new wxColorButton(panel,this,myID_SIM_PREV_CLR,
      wxDefaultPosition,btnSize);
  shButton->SetBackgroundColour(settings->getRGB(SimPrevColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,
                              wxT("Previous states/transitions:")),0,lf,bd);
  sizer->Add(shButton,0,rf,bd);

  wxColorButton* scButton = new wxColorButton(panel,this,myID_SIM_CURR_CLR,
      wxDefaultPosition,btnSize);
  scButton->SetBackgroundColour(settings->getRGB(SimCurrColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Current state:")),0,lf,bd);
  sizer->Add(scButton,0,rf,bd);

  wxColorButton* ssButton = new wxColorButton(panel,this,myID_SIM_SEL_CLR,
      wxDefaultPosition,btnSize);
  ssButton->SetBackgroundColour(settings->getRGB(SimSelColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Selected state/transition:")),
             0,lf,bd);
  sizer->Add(ssButton,0,rf,bd);

  wxColorButton* spButton = new wxColorButton(panel,this,myID_SIM_POS_CLR,
      wxDefaultPosition,btnSize);
  spButton->SetBackgroundColour(settings->getRGB(SimPosColor).toWxColour());
  sizer->Add(new wxStaticText(panel,wxID_ANY,
                              wxT("Possible states/transitions:")),0,lf,bd);
  sizer->Add(spButton,0,rf,bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupAlgorithmsPanel(wxPanel* panel)
{
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int bd = 5;

  wxFlexGridSizer* sizer = new wxFlexGridSizer(10,1,0,0);
  sizer->AddGrowableCol(0);
  sizer->AddGrowableRow(9);

  wxRadioButton* irsRadio = new wxRadioButton(panel, myID_ITERATIVE,
      wxT("Iterative state ranking"), wxDefaultPosition, wxDefaultSize,
      wxRB_GROUP);
  wxRadioButton* crsRadio = new wxRadioButton(panel, myID_CYCLIC,
      wxT("Cyclic state ranking"));
  sizer->Add(irsRadio, 0, lf, bd);
  sizer->Add(crsRadio, 0, lf, bd);

  sizer->Add(new wxStaticLine(panel), 0, lf, bd);

  wxRadioButton* cvsRadio = new wxRadioButton(panel, myID_CONES_STYLE,
      wxT("Cones visualization"), wxDefaultPosition, wxDefaultSize,
      wxRB_GROUP);
  wxRadioButton* tvsRadio = new wxRadioButton(panel, myID_TUBES_STYLE,
      wxT("Tubes visualization"));
  wxCheckBox* fsmCheck = new wxCheckBox(panel, myID_FSM_STYLE,
                                        wxT("FSMView style"));
  sizer->Add(cvsRadio, 0, lf, bd);
  sizer->Add(tvsRadio, 0, lf, bd);
  sizer->Add(fsmCheck, 0, lf, bd);

  sizer->Add(new wxStaticLine(panel), 0, lf, bd);

  wxRadioButton* spsRadio = new wxRadioButton(panel, myID_SP_STATEPOS,
      wxT("Single-pass state positioning"), wxDefaultPosition, wxDefaultSize,
      wxRB_GROUP);
  wxRadioButton* mpsRadio = new wxRadioButton(panel, myID_MP_STATEPOS,
      wxT("Multi-pass state positioning"));
  sizer->Add(spsRadio, 0, lf, bd);
  sizer->Add(mpsRadio, 0, lf, bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupPerformancePanel(wxPanel* panel)
{
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL;
  int bd = 5;

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(new wxStaticText(panel,wxID_ANY,
                              wxT("While zooming/panning/rotating:")),0,lf,bd);

  wxCheckBox* nsbCheck = new wxCheckBox(panel,myID_NAV_SHOW_BACKPOINTERS,
                                        wxT("Show backpointers"));
  nsbCheck->SetValue(settings->getBool(NavShowBackpointers));
  sizer->Add(nsbCheck,0,lf,bd);

  wxCheckBox* nssCheck = new wxCheckBox(panel,myID_NAV_SHOW_STATES,
                                        wxT("Show states"));
  nssCheck->SetValue(settings->getBool(NavShowStates));
  sizer->Add(nssCheck,0,lf,bd);

  wxCheckBox* nstCheck = new wxCheckBox(panel,myID_NAV_SHOW_TRANSITIONS,
                                        wxT("Show transitions"));
  nstCheck->SetValue(settings->getBool(NavShowTransitions));
  sizer->Add(nstCheck,0,lf,bd);

  wxCheckBox* nshCheck = new wxCheckBox(panel,myID_NAV_SMOOTH_SHADING,
                                        wxT("Enable smooth shading"));
  nshCheck->SetValue(settings->getBool(NavSmoothShading));
  sizer->Add(nshCheck,0,lf,bd);

  wxCheckBox* nliCheck = new wxCheckBox(panel,myID_NAV_LIGHTING,
                                        wxT("Enable lighting"));
  nliCheck->SetValue(settings->getBool(NavLighting));
  sizer->Add(nliCheck,0,lf,bd);

  wxCheckBox* ntrCheck = new wxCheckBox(panel,myID_NAV_TRANSPARENCY,
                                        wxT("Render transparent objects correctly"));
  ntrCheck->SetValue(settings->getBool(NavTransparency));
  sizer->Add(ntrCheck,0,lf,bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::notify(SettingID s)
{
  switch (s)
  {
    case ClusterHeight:
    {
      wxSpinCtrl* chSpin = static_cast<wxSpinCtrl*>(
                             FindWindowById(myID_CLUSTER_HEIGHT,this));
      chSpin->SetValue(MathUtils::round_to_int(10*settings->getFloat(ClusterHeight)));
      break;
    }
    default:
      break;
  }
}

void SettingsDialog::onBackgroundClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(BackgroundColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onDownEdgeClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(DownEdgeColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onInterpolateClr1Button(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(InterpolateColor1, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onInterpolateClr2Button(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(InterpolateColor2, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onMarkClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(MarkedColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onStateClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(StateColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onUpEdgeClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(UpEdgeColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onSimCurrClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(SimCurrColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onSimPosClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(SimPosColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onSimSelClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(SimSelColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onSimPrevClrButton(wxCommandEvent& event)
{
  wxColorButton* btn = (wxColorButton*)event.GetEventObject();
  settings->setRGB(SimPrevColor, RGB_Color(btn->GetBackgroundColour()));
  glCanvas->display();
}

void SettingsDialog::onLongInterpolationCheck(wxCommandEvent& event)
{
  settings->setBool(LongInterpolation,event.IsChecked());
  glCanvas->display();
}

void SettingsDialog::onNavShowBackpointersCheck(wxCommandEvent& event)
{
  settings->setBool(NavShowBackpointers,event.IsChecked());
}

void SettingsDialog::onNavShowStatesCheck(wxCommandEvent& event)
{
  settings->setBool(NavShowStates,event.IsChecked());
}

void SettingsDialog::onNavShowTransitionsCheck(wxCommandEvent& event)
{
  settings->setBool(NavShowTransitions,event.IsChecked());
}

void SettingsDialog::onNavSmoothShadingCheck(wxCommandEvent& event)
{
  settings->setBool(NavSmoothShading,event.IsChecked());
}

void SettingsDialog::onNavLightingCheck(wxCommandEvent& event)
{
  settings->setBool(NavLighting,event.IsChecked());
}

void SettingsDialog::onNavTransparencyCheck(wxCommandEvent& event)
{
  settings->setBool(NavTransparency,event.IsChecked());
}

void SettingsDialog::onBranchRotationSpin(wxSpinEvent& event)
{
  settings->setInt(BranchRotation,event.GetPosition());
  glCanvas->display();
}

void SettingsDialog::onStateSizeSpin(wxSpinEvent& event)
{
  settings->setFloat(StateSize,event.GetPosition()/10.0f);
  glCanvas->display();
}

void SettingsDialog::onClusterHeightSpin(wxSpinEvent& event)
{
  settings->setFloat(ClusterHeight,event.GetPosition()/10.0f);
  glCanvas->display();
}

void SettingsDialog::onBranchTiltSpin(wxSpinEvent& event)
{
  settings->setInt(BranchTilt,event.GetPosition());
  glCanvas->display();
}

void SettingsDialog::onQualitySpin(wxSpinEvent& event)
{
  settings->setInt(Quality,2*event.GetPosition());
  glCanvas->display();
}

void SettingsDialog::onTransparencySpin(wxSpinEvent& event)
{
  settings->setInt(Alpha,
                   static_cast<int>((100-event.GetPosition())*2.55f));
  glCanvas->display();
}

void SettingsDialog::onIterativeRadio(wxCommandEvent&)
{
  settings->setInt(StateRankStyle, ITERATIVE);
}

void SettingsDialog::onCyclicRadio(wxCommandEvent&)
{
  settings->setInt(StateRankStyle, CYCLIC);
}

void SettingsDialog::onConesRadio(wxCommandEvent&)
{
  settings->setInt(ClusterVisStyle, CONES);
}

void SettingsDialog::onTubesRadio(wxCommandEvent&)
{
  settings->setInt(ClusterVisStyle, TUBES);
}

void SettingsDialog::onFsmStyleCheck(wxCommandEvent& event)
{
  settings->setBool(FsmStyle, event.IsChecked());
}

void SettingsDialog::onSinglePassRadio(wxCommandEvent&)
{
  settings->setInt(StatePosStyle, SINGLE_PASS);
}

void SettingsDialog::onMultiPassRadio(wxCommandEvent&)
{
  settings->setInt(StatePosStyle, MULTI_PASS);
}
