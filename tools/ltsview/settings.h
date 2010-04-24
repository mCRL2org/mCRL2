// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settings.h
/// \brief Header file for Settings class

#ifndef SETTINGS_H
#define SETTINGS_H

#include <map>

class RGB_Color;

enum SettingID
{
  StateSize,
  BranchRotation,
  ClusterHeight,
  BranchTilt,
  Quality,
  Alpha,
  LongInterpolation,
  DisplayBackpointers,
  DisplayStates,
  DisplayTransitions,
  DisplayWireframe,
  NavShowBackpointers,
  NavShowStates,
  NavShowTransitions,
  NavSmoothShading,
  NavLighting,
  NavTransparency,
  DownEdgeColor,
  UpEdgeColor,
  InterpolateColor1,
  InterpolateColor2,
  MarkedColor,
  Selection,
  StateColor,
  SimPrevColor,
  SimCurrColor,
  SimSelColor,
  SimPosColor,
  BackgroundColor,
  TransitionAttraction,
  TransitionLength,
  StateRepulsion
};

/* abstract base class for subscribers */
class Subscriber
{
  public:
    virtual ~Subscriber() {}
    virtual void notify(SettingID s) = 0;
};

class AbsSetting;

class Settings
{
  private:
    std::map< SettingID, AbsSetting* > settings;
  public:
    Settings();
    ~Settings();
    void subscribe(SettingID st,Subscriber* ss);
    int  getInt(SettingID s);
    bool getBool(SettingID s);
    float getFloat(SettingID s);
    RGB_Color getRGB(SettingID s);
		void loadDefaults();
    void setInt(SettingID s,int v);
    void setBool(SettingID s,bool v);
    void setFloat(SettingID s,float v);
    void setRGB(SettingID s, RGB_Color v);
};
#endif
