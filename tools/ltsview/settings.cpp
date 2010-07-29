// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file settings.cpp
/// \brief Implements the settings store

#include "wx.hpp" // precompiled headers

#include <map>
#include <vector>
#include "rgb_color.h"
#include "settings.h"

using namespace std;

class AbsSetting
{
  private:
    SettingID id;
    vector< Subscriber* > subscribers;
  protected:
    void notify_all();
  public:
    AbsSetting(SettingID i): id(i) {}
    virtual ~AbsSetting() {}
    void subscribe(Subscriber* ss) { subscribers.push_back(ss); }
    virtual void reset() {}
};

void AbsSetting::notify_all()
{
  for (vector<Subscriber*>::iterator sub_it = subscribers.begin();
       sub_it != subscribers.end(); ++sub_it)
  {
    (**sub_it).notify(id);
  }
}

/* -------------------------------------------------------------------------- */

template< class T > class Setting: public AbsSetting
{
  private:
    T def_value;
    T value;
  public:
    Setting(SettingID i, T dv): AbsSetting(i), def_value(dv), value(dv) {}

    ~Setting() {}
    
    T getValue() { return value; }
    
    void setValue(T v)
    {
      if (value != v)
      {
        value = v;
        notify_all();
      }
    }
    
    void reset() { setValue(def_value); }
};

/* -------------------------------------------------------------------------- */

Settings::Settings() { }

void Settings::loadDefaults()
{ 
  settings[StateSize] = new Setting<float>(StateSize, 0.1f);
  settings[BranchRotation] = new Setting<int>(BranchRotation, 0);
  settings[ClusterHeight] = new Setting<float>(ClusterHeight, 0.0f);
  settings[BranchTilt] = new Setting<int>(BranchTilt, 30);
  settings[Quality] = new Setting<int>(Quality, 12);
  settings[TransitionAttraction] = new Setting<float>(TransitionAttraction, 1.0f);
  settings[TransitionLength] = new Setting<float>(TransitionLength, 1.0f);
  settings[StateRepulsion] = new Setting<float>(StateRepulsion, 1.0f);
  settings[Alpha] = new Setting<int>(Alpha, 178);
  settings[LongInterpolation] = new Setting<bool>(LongInterpolation, false);
  settings[DisplayBackpointers] = new Setting<bool>(DisplayBackpointers, false);
  settings[DisplayStates] = new Setting<bool>(DisplayStates, false);
  settings[DisplayTransitions] = new Setting<bool>(DisplayTransitions, false);
  settings[DisplayWireframe] = new Setting<bool>(DisplayWireframe, false);
  settings[NavShowBackpointers] = new Setting<bool>(NavShowBackpointers, true);
  settings[NavShowStates] = new Setting<bool>(NavShowStates, true);
  settings[NavShowTransitions] = new Setting<bool>(NavShowTransitions, true);
  settings[NavSmoothShading] = new Setting<bool>(NavSmoothShading, true);
  settings[NavLighting] = new Setting<bool>(NavLighting, true);
  settings[NavTransparency] = new Setting<bool>(NavTransparency, true);
  settings[DownEdgeColor] = new Setting<RGB_Color>(DownEdgeColor, RGB_Color(255, 255, 255));
  settings[UpEdgeColor] = new Setting<RGB_Color>(UpEdgeColor, RGB_Color(0, 0, 255));
  settings[InterpolateColor1] = new Setting<RGB_Color>(InterpolateColor1, RGB_Color(255, 255, 255));
  settings[InterpolateColor2] = new Setting<RGB_Color>(InterpolateColor2, RGB_Color(0, 0, 255));
  settings[MarkedColor] = new Setting<RGB_Color>(MarkedColor, RGB_Color(255, 0, 0));
  settings[StateColor] = new Setting<RGB_Color>(StateColor, RGB_Color(255, 255, 255));
  settings[SimPrevColor] = new Setting<RGB_Color>(SimPrevColor, RGB_Color(255, 0, 255));
  settings[SimCurrColor] = new Setting<RGB_Color>(SimCurrColor, RGB_Color(0, 255, 0));
  settings[SimSelColor] = new Setting<RGB_Color>(SimSelColor, RGB_Color(255, 255, 0));
  settings[SimPosColor] = new Setting<RGB_Color>(SimPosColor, RGB_Color(0, 255, 255));
  settings[BackgroundColor] = new Setting<RGB_Color>(BackgroundColor, RGB_Color(150, 150, 150));
}

Settings::~Settings()
{
  for (map<SettingID, AbsSetting*>::iterator i = settings.begin();
      i != settings.end(); ++i)
  {
    delete i->second;
  }
}

void Settings::subscribe(SettingID st,Subscriber* ss)
{
  settings[st]->subscribe(ss);
}

int Settings::getInt(SettingID s)
{
  return static_cast<Setting<int>*>(settings[s])->getValue();
}

bool Settings::getBool(SettingID s)
{
  return static_cast<Setting<bool>*>(settings[s])->getValue();
}

float Settings::getFloat(SettingID s)
{
  return static_cast<Setting<float>*>(settings[s])->getValue();
}

RGB_Color Settings::getRGB(SettingID s) {
  return static_cast<Setting<RGB_Color>*>(settings[s])->getValue();
}

void Settings::setInt(SettingID s,int v)
{
  static_cast<Setting<int>*>(settings[s])->setValue(v);
}

void Settings::setBool(SettingID s,bool v)
{
  static_cast<Setting<bool>*>(settings[s])->setValue(v);
}

void Settings::setFloat(SettingID s,float v)
{
  static_cast<Setting<float>*>(settings[s])->setValue(v);
}

void Settings::setRGB(SettingID s,RGB_Color v)
{
  static_cast<Setting<RGB_Color>*>(settings[s])->setValue(v);
}
