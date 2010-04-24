// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settingsdialog.h
/// \brief Header file for the settings dialog

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <wx/wx.h>
#include "settings.h"

class GLCanvas;
class wxSpinEvent;

class SettingsDialog: public wxDialog, public Subscriber
{
  public:
    SettingsDialog(wxWindow* parent,GLCanvas* glc,Settings* ss);
    void notify(SettingID s);
    void onBackgroundClrButton(wxCommandEvent& event);
    void onDownEdgeClrButton(wxCommandEvent& event);
    void onInterpolateClr1Button(wxCommandEvent& event);
    void onInterpolateClr2Button(wxCommandEvent& event);
    void onMarkClrButton(wxCommandEvent& event);
    void onStateClrButton(wxCommandEvent& event);
    void onUpEdgeClrButton(wxCommandEvent& event);
    void onSimCurrClrButton(wxCommandEvent& event);
    void onSimPosClrButton(wxCommandEvent& event);
    void onSimSelClrButton(wxCommandEvent& event);
    void onSimPrevClrButton(wxCommandEvent& event);
    void onLongInterpolationCheck(wxCommandEvent& event);
    void onNavShowBackpointersCheck(wxCommandEvent& event);
    void onNavShowStatesCheck(wxCommandEvent& event);
    void onNavShowTransitionsCheck(wxCommandEvent& event);
    void onNavSmoothShadingCheck(wxCommandEvent& event);
    void onNavLightingCheck(wxCommandEvent& event);
    void onNavTransparencyCheck(wxCommandEvent& event);
    void onBranchRotationSpin(wxSpinEvent& event);
    void onStateSizeSpin(wxSpinEvent& event);
    void onClusterHeightSpin(wxSpinEvent& event);
    void onBranchTiltSpin(wxSpinEvent& event);
    void onQualitySpin(wxSpinEvent& event);
    void onTransparencySpin(wxSpinEvent& event);
    void onTransitionAttractionSlider(wxScrollEvent& event);
    void onTransitionLengthSlider(wxScrollEvent& event);
    void onStateRepulsionSlider(wxScrollEvent& event);
  private:
    GLCanvas* glCanvas;
    Settings* settings;
    void setupClrPanel(wxPanel* panel);
    void setupParPanel(wxPanel* panel);
    void setupSimPanel(wxPanel* panel);
    void setupPfmPanel(wxPanel* panel);
    DECLARE_EVENT_TABLE()
};

#endif
