// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settings.h
/// \brief Add your file description here.

#ifndef SETTINGS_H
#define SETTINGS_H
#include <vector>
#include "utils.h"

enum SettingID {
  NodeSize,
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
  StateColor,
  SimPrevColor,
  SimCurrColor,
  SimSelColor,
  SimPosColor,
  BackgroundColor,
  NUM_OF_SETTINGID /* should always be last! */
};

/* abstract base class for subscribers */
class Subscriber {
  public:
    virtual ~Subscriber() {}
    virtual void notify(SettingID s) = 0;
};

class Setting; /* forward declaration */

class Settings {
  private:
    std::vector< Setting* > settings;
  public:
    Settings();
    ~Settings();
    void subscribe(SettingID st,Subscriber* ss);
    int  getInt(SettingID s);
    bool getBool(SettingID s);
    float getFloat(SettingID s);
    unsigned char getUByte(SettingID s);
    Utils::RGB_Color getRGB(SettingID s);
    void setInt(SettingID s,int v);
    void setBool(SettingID s,bool v);
    void setFloat(SettingID s,float v);
    void setUByte(SettingID s,unsigned char v);
    void setRGB(SettingID s,Utils::RGB_Color v);
};
#endif
