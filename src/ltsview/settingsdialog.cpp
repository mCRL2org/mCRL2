#include "settingsdialog.h"
#include <wx/notebook.h>
#include "ids.h"
#include "utils.h"

using namespace IDs;
using namespace Utils;

BEGIN_EVENT_TABLE(SettingsDialog,wxDialog)
  EVT_BUTTON(myID_ANY,SettingsDialog::onButton)
  EVT_SPINCTRL(myID_ANY,SettingsDialog::onSpin)
  EVT_CHECKBOX(myID_ANY,SettingsDialog::onCheck)
END_EVENT_TABLE()

SettingsDialog::SettingsDialog(wxWindow* parent,GLCanvas* glc,Settings* ss)
  : wxDialog(parent,wxID_ANY,wxT("Settings"),wxDefaultPosition) {
  glCanvas = glc;
  settings = ss;
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  wxNotebook* nb = new wxNotebook(this,wxID_ANY);
  wxPanel* parPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* clrPanel = new wxPanel(nb,wxID_ANY);
  wxPanel* pfmPanel = new wxPanel(nb,wxID_ANY);
  
  setupParPanel(parPanel);
  setupClrPanel(clrPanel);
  setupPfmPanel(pfmPanel);
  
  nb->AddPage(parPanel,wxT("Parameters"),true);
  nb->AddPage(clrPanel,wxT("Colours"),false);
  nb->AddPage(pfmPanel,wxT("Performance"),false);
  
  sizer->Add(nb,0,wxEXPAND|wxALL,5);
  SetSizerAndFit(sizer);
  Layout();
  SetSize(wxSize(300,-1));
}

void SettingsDialog::setupParPanel(wxPanel* panel) {
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int rf = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int bd = 5;
  wxSize spinSize(50,-1);

  wxFlexGridSizer* sizer = new wxFlexGridSizer(7,2,0,0);
  sizer->AddGrowableCol(0);
  //for (int i=0; i<5; ++i) {
  //  sizer->AddGrowableRow(i);
  //}
  sizer->AddGrowableRow(6);

  nsSpin = new wxSpinCtrl(panel,myID_ANY);
  nsSpin->SetRange(0,1000);
  nsSpin->SetValue(int(10*settings->getFloat(NodeSize)));
  nsSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("State size:")),0,lf,bd);
  sizer->Add(nsSpin,0,rf,bd);
  
  brSpin = new wxSpinCtrl(panel,myID_ANY);
  brSpin->SetRange(0,360);
  brSpin->SetValue(settings->getInt(BranchRotation));
  brSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Branch rotation:")),0,lf,bd);
  sizer->Add(brSpin,0,rf,bd);

  ibSpin = new wxSpinCtrl(panel,myID_ANY);
  ibSpin->SetRange(0,90);
  ibSpin->SetValue(settings->getInt(InnerBranchTilt));
  ibSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Inner branch tilt:")),0,lf,bd);
  sizer->Add(ibSpin,0,rf,bd);

  obSpin = new wxSpinCtrl(panel,myID_ANY);
  obSpin->SetRange(0,90);
  obSpin->SetValue(settings->getInt(OuterBranchTilt));
  obSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Outer branch tilt:")),0,lf,bd);
  sizer->Add(obSpin,0,rf,bd);
  
  qlSpin = new wxSpinCtrl(panel,myID_ANY);
  qlSpin->SetRange(2,50);
  qlSpin->SetValue(settings->getInt(Quality)/2);
  qlSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Accuracy:")),0,lf,bd);
  sizer->Add(qlSpin,0,rf,bd);
  
  elSpin = new wxSpinCtrl(panel,myID_ANY);
  elSpin->SetRange(1,100);
  elSpin->SetValue(int(10*settings->getFloat(EllipsoidThreshold)));
  elSpin->SetSizeHints(spinSize, spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Ellipsoid threshold:")),0,lf,bd);
  sizer->Add(elSpin,0,rf,bd);
  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupClrPanel(wxPanel* panel) {
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int rf = wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int bd = 5;
  wxSize spinSize(50,-1);
  wxSize btnSize(25,25);

  wxFlexGridSizer* sizer = new wxFlexGridSizer(9,3,0,0);
  sizer->AddGrowableCol(0);
  //for (int i=0; i<8 ; ++i) {
  //  sizer->AddGrowableRow(i);
  //}
  sizer->AddGrowableRow(8);
  
  trSpin = new wxSpinCtrl(panel,myID_ANY);
  trSpin->SetRange(0,100);
  trSpin->SetValue(static_cast<int>((255-settings->getUByte(Alpha))/2.55f));
  trSpin->SetSizeHints(spinSize,spinSize);
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transparency:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(trSpin,0,rf,bd);

  bgButton = new wxColorButton(panel,this,myID_ANY);
  bgButton->SetSizeHints(btnSize,btnSize);
  bgButton->SetBackgroundColour(RGB_to_wxC(settings->getRGB(BackgroundColor)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Background:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(bgButton,0,rf,bd);

  ndButton = new wxColorButton(panel,this,myID_ANY);
  ndButton->SetSizeHints(btnSize,btnSize);
  ndButton->SetBackgroundColour(RGB_to_wxC(settings->getRGB(StateColor)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("State:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(ndButton,0,rf,bd);

  deButton = new wxColorButton(panel,this,myID_ANY);
  deButton->SetSizeHints(btnSize,btnSize);
  deButton->SetBackgroundColour(RGB_to_wxC(settings->getRGB(DownEdgeColor)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Transition:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(deButton,0,rf,bd);

  ueButton = new wxColorButton(panel,this,myID_ANY);
  ueButton->SetSizeHints(btnSize,btnSize);
  ueButton->SetBackgroundColour(RGB_to_wxC(settings->getRGB(UpEdgeColor)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Backpointer:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(ueButton,0,rf,bd);

  mkButton = new wxColorButton(panel,this,myID_ANY);
  mkButton->SetSizeHints(btnSize,btnSize);
  mkButton->SetBackgroundColour(RGB_to_wxC(settings->getRGB(MarkedColor)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Mark:")),0,lf,bd);
  sizer->AddSpacer(0);
  sizer->Add(mkButton,0,rf,bd);

  i1Button = new wxColorButton(panel,this,myID_ANY);
  i1Button->SetSizeHints(btnSize,btnSize);
  i1Button->SetBackgroundColour(RGB_to_wxC(settings->getRGB(InterpolateColor1)));
  i2Button = new wxColorButton(panel,this,myID_ANY);
  i2Button->SetSizeHints(btnSize,btnSize);
  i2Button->SetBackgroundColour(RGB_to_wxC(settings->getRGB(InterpolateColor2)));
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("Interpolate:")),0,lf,bd);
  sizer->Add(i1Button,0,rf,bd);
  sizer->Add(i2Button,0,rf,bd);
  
  liCheck = new wxCheckBox(panel,myID_ANY,wxT("Long interpolation"));
  liCheck->SetValue(settings->getBool(LongInterpolation));
  sizer->Add(liCheck,0,lf,bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::setupPfmPanel(wxPanel* panel) {
  int lf = wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL; 
  int bd = 5;

  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL); 
  sizer->Add(new wxStaticText(panel,wxID_ANY,wxT("While zooming/panning/rotating:")),0,lf,bd);
  nsbCheck = new wxCheckBox(panel,myID_ANY,wxT("Show backpointers"));
  nsbCheck->SetValue(settings->getBool(NavShowBackpointers));
  sizer->Add(nsbCheck,0,lf,bd);
  nssCheck = new wxCheckBox(panel,myID_ANY,wxT("Show states"));
  nssCheck->SetValue(settings->getBool(NavShowStates));
  sizer->Add(nssCheck,0,lf,bd);
  nstCheck = new wxCheckBox(panel,myID_ANY,wxT("Show transitions"));
  nstCheck->SetValue(settings->getBool(NavShowTransitions));
  sizer->Add(nstCheck,0,lf,bd);
  nshCheck = new wxCheckBox(panel,myID_ANY,wxT("Enable smooth shading"));
  nshCheck->SetValue(settings->getBool(NavSmoothShading));
  sizer->Add(nshCheck,0,lf,bd);
  nliCheck = new wxCheckBox(panel,myID_ANY,wxT("Enable lighting"));
  nliCheck->SetValue(settings->getBool(NavLighting));
  sizer->Add(nliCheck,0,lf,bd);

  panel->SetSizer(sizer);
  panel->Fit();
  panel->Layout();
}

void SettingsDialog::onButton(wxCommandEvent& event) {
  if (event.GetEventObject() == bgButton) {
    settings->setRGB(BackgroundColor,
      wxC_to_RGB(bgButton->GetBackgroundColour()));
  } else if (event.GetEventObject() == deButton) {
    settings->setRGB(DownEdgeColor,
      wxC_to_RGB(deButton->GetBackgroundColour()));
  } else if (event.GetEventObject() == i1Button) {
    settings->setRGB(InterpolateColor1,
      wxC_to_RGB(i1Button->GetBackgroundColour()));
  } else if (event.GetEventObject() == i2Button) {
    settings->setRGB(InterpolateColor2,
      wxC_to_RGB(i2Button->GetBackgroundColour()));
  } else if (event.GetEventObject() == mkButton) {
    settings->setRGB(MarkedColor,
      wxC_to_RGB(mkButton->GetBackgroundColour()));
  } else if (event.GetEventObject() == ndButton) {
    settings->setRGB(StateColor,
      wxC_to_RGB(ndButton->GetBackgroundColour()));
  } else if (event.GetEventObject() == ueButton) {
    settings->setRGB(UpEdgeColor,
      wxC_to_RGB(ueButton->GetBackgroundColour()));
  } else {
    return;
  }
  glCanvas->display();
}

void SettingsDialog::onCheck(wxCommandEvent& event) {
  if (event.GetEventObject() == liCheck) {
    settings->setBool(LongInterpolation,liCheck->GetValue());
    glCanvas->display();
  } else if (event.GetEventObject() == nsbCheck) {
    settings->setBool(NavShowBackpointers,nsbCheck->GetValue());
  } else if (event.GetEventObject() == nssCheck) {
    settings->setBool(NavShowStates,nssCheck->GetValue());
  } else if (event.GetEventObject() == nstCheck) {
    settings->setBool(NavShowTransitions,nstCheck->GetValue());
  } else if (event.GetEventObject() == nshCheck) {
    settings->setBool(NavSmoothShading,nshCheck->GetValue());
  } else if (event.GetEventObject() == nliCheck) {
    settings->setBool(NavLighting,nliCheck->GetValue());
  }
}

void SettingsDialog::onSpin(wxSpinEvent& event) {
  if (event.GetEventObject() == brSpin) {
    settings->setInt(BranchRotation,brSpin->GetValue());
  } else if (event.GetEventObject() == elSpin) {
    settings->setFloat(EllipsoidThreshold,elSpin->GetValue()/10.0f);
  } else if (event.GetEventObject() == ibSpin) {
    settings->setInt(InnerBranchTilt,ibSpin->GetValue());
  } else if (event.GetEventObject() == nsSpin) {
    settings->setFloat(NodeSize,nsSpin->GetValue()/10.0f);
  } else if (event.GetEventObject() == obSpin) {
    settings->setInt(OuterBranchTilt,obSpin->GetValue());
  } else if (event.GetEventObject() == qlSpin) {
    settings->setInt(Quality,2*qlSpin->GetValue());
  } else if (event.GetEventObject() == trSpin) {
    settings->setUByte(Alpha,
     static_cast<unsigned char>((100-trSpin->GetValue())*2.55f));
  } else {
    return;
  }
  glCanvas->display();
}

wxColour SettingsDialog::RGB_to_wxC(RGB_Color c) const {
  return wxColour(c.r,c.g,c.b);
}

RGB_Color SettingsDialog::wxC_to_RGB(wxColour c) const {
  RGB_Color result = {c.Red(),c.Green(),c.Blue()};
  return result;
}
