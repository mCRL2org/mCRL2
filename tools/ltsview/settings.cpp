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

#include <vector>
#include "settings.h"
#include "utils.h"

using namespace std;
using namespace Utils;

class Setting {
  private:
    SettingID id;
    vector< Subscriber* > subscribers;
  protected:
    void notify_all();
  public:
    Setting(SettingID i);
    virtual ~Setting() {}
    void subscribe(Subscriber* ss);
    virtual void reset() {}
};

Setting::Setting(SettingID i) {
  id = i;
}

void Setting::notify_all() {
  for (vector<Subscriber*>::iterator sub_it = subscribers.begin();
       sub_it != subscribers.end(); ++sub_it) {
    (**sub_it).notify(id);
  }
}

inline void Setting::subscribe(Subscriber* ss) { subscribers.push_back(ss); }

/* -------------------------------------------------------------------------- */

class Setting_Int: public Setting {
  private:
    int def_value;
    int value;
  public:
    Setting_Int(SettingID i,int dv);
    ~Setting_Int() {}
    int getValue();
    void setValue(int v);
    void reset();
};

Setting_Int::Setting_Int(SettingID i,int dv): Setting(i) {
  def_value = dv;
  value = dv;
}

inline int Setting_Int::getValue() { return value; }

void Setting_Int::setValue(int v) {
  if (value != v) {
    value = v;
    notify_all();
  }
}

void Setting_Int::reset() {
  if (value != def_value) {
    value = def_value;
    notify_all();
  }
}

/* -------------------------------------------------------------------------- */

class Setting_Bool: public Setting {
  private:
    bool def_value;
    bool value;
  public:
    Setting_Bool(SettingID i,bool dv);
    ~Setting_Bool() {}
    bool getValue();
    void setValue(bool v);
    void reset();
};

Setting_Bool::Setting_Bool(SettingID i,bool dv): Setting(i) {
  def_value = dv;
  value = dv;
}

inline bool Setting_Bool::getValue() { return value; }

void Setting_Bool::setValue(bool v) {
  if (value != v) {
    value = v;
    notify_all();
  }
}

void Setting_Bool::reset() {
  if (value != def_value) {
    value = def_value;
    notify_all();
  }
}

/* -------------------------------------------------------------------------- */

class Setting_Float: public Setting {
  private:
    float def_value;
    float value;
  public:
    Setting_Float(SettingID i,float dv);
    ~Setting_Float() {}
    float getValue();
    void setValue(float v);
    void reset();
};

Setting_Float::Setting_Float(SettingID i,float dv): Setting(i) {
  def_value = dv;
  value = dv;
}

inline float Setting_Float::getValue() { return value; }

void Setting_Float::setValue(float v) {
  if (value != v) {
    value = v;
    notify_all();
  }
}

void Setting_Float::reset() {
  if (value != def_value) {
    value = def_value;
    notify_all();
  }
}

/* -------------------------------------------------------------------------- */

class Setting_UByte: public Setting {
  private:
    unsigned char def_value;
    unsigned char value;
  public:
    Setting_UByte(SettingID i,unsigned char dv);
    ~Setting_UByte() {}
    unsigned char getValue();
    void setValue(unsigned char v);
    void reset();
};

Setting_UByte::Setting_UByte(SettingID i,unsigned char dv): Setting(i) {
  def_value = dv;
  value = dv;
}

inline unsigned char Setting_UByte::getValue() { return value; }

void Setting_UByte::setValue(unsigned char v) {
  if (value != v) {
    value = v;
    notify_all();
  }
}

void Setting_UByte::reset() {
  if (value != def_value) {
    value = def_value;
    notify_all();
  }
}

/* -------------------------------------------------------------------------- */

class Setting_RGB: public Setting {
  private:
    RGB_Color def_value;
    RGB_Color value;
  public:
    Setting_RGB(SettingID i,RGB_Color dv);
    ~Setting_RGB() {}
    RGB_Color getValue();
    void setValue(RGB_Color v);
    void reset();
};

Setting_RGB::Setting_RGB(SettingID i,RGB_Color dv): Setting(i) {
  def_value = dv;
  value = dv;
}

inline RGB_Color Setting_RGB::getValue() { return value; }

void Setting_RGB::setValue(RGB_Color v) {
  if (value != v) {
    value = v;
    notify_all();
  }
}

void Setting_RGB::reset() {
  if (value != def_value) {
    value = def_value;
    notify_all();
  }
}

/* -------------------------------------------------------------------------- */

Settings::Settings() {
  settings.assign(NUM_OF_SETTINGID,NULL);
  RGB_Color prev = {255, 0, 255};
  RGB_Color cur = {0, 255, 0};
  RGB_Color sel = {255, 255, 0};
  RGB_Color pos = {0, 255, 255};
  RGB_Color bg = {150,150,150};
  settings[StateSize]           = new Setting_Float(StateSize,0.1f);
  settings[BranchRotation]      = new Setting_Int(BranchRotation,0);
  settings[ClusterHeight]       = new Setting_Float(ClusterHeight,0.0f);
  settings[BranchTilt]          = new Setting_Int(BranchTilt,30);
  settings[Quality]             = new Setting_Int(Quality,12);
  settings[TransitionAttraction]= new Setting_Float(TransitionAttraction,1.0f);
  settings[TransitionLength]    = new Setting_Float(TransitionLength,1.0f);
  settings[StateRepulsion]      = new Setting_Float(StateRepulsion,1.0f);
  settings[Alpha]               = new Setting_UByte(Alpha,178);
  settings[LongInterpolation]   = new Setting_Bool(LongInterpolation,false);
  settings[DisplayBackpointers] = new Setting_Bool(DisplayBackpointers,false);
  settings[DisplayStates]       = new Setting_Bool(DisplayStates,false);
  settings[DisplayTransitions]  = new Setting_Bool(DisplayTransitions,false);
  settings[DisplayWireframe]    = new Setting_Bool(DisplayWireframe,false);
  settings[NavShowBackpointers] = new Setting_Bool(NavShowBackpointers,true);
  settings[NavShowStates]       = new Setting_Bool(NavShowStates,true);
  settings[NavShowTransitions]  = new Setting_Bool(NavShowTransitions,true);
  settings[NavSmoothShading]    = new Setting_Bool(NavSmoothShading,true);
  settings[NavLighting]         = new Setting_Bool(NavLighting,true);
  settings[NavTransparency]     = new Setting_Bool(NavTransparency,true);
  settings[DownEdgeColor]       = new Setting_RGB(DownEdgeColor,RGB_WHITE);
  settings[UpEdgeColor]         = new Setting_RGB(UpEdgeColor,RGB_BLUE);
  settings[InterpolateColor1]   = new Setting_RGB(InterpolateColor1,RGB_WHITE);
  settings[InterpolateColor2]   = new Setting_RGB(InterpolateColor2,RGB_BLUE);
  settings[MarkedColor]         = new Setting_RGB(MarkedColor,RGB_RED);
  settings[StateColor]          = new Setting_RGB(StateColor,RGB_WHITE);
  settings[SimPrevColor]        = new Setting_RGB(SimPrevColor,prev);
  settings[SimCurrColor]        = new Setting_RGB(SimCurrColor,cur);
  settings[SimSelColor]         = new Setting_RGB(SimSelColor,sel);
  settings[SimPosColor]         = new Setting_RGB(SimPosColor,pos);
  settings[BackgroundColor]     = new Setting_RGB(BackgroundColor,bg);
}

Settings::~Settings() {
  for (unsigned int i=0; i < settings.size(); ++i) {
    delete settings[i];
  }
  settings.clear();
}

void Settings::subscribe(SettingID st,Subscriber* ss) {
  settings[st]->subscribe(ss);
}

int Settings::getInt(SettingID s) {
  return static_cast<Setting_Int*>(settings[s])->getValue();
}

bool Settings::getBool(SettingID s) {
  return static_cast<Setting_Bool*>(settings[s])->getValue();
}

float Settings::getFloat(SettingID s) {
  return static_cast<Setting_Float*>(settings[s])->getValue();
}

unsigned char Settings::getUByte(SettingID s) {
  return static_cast<Setting_UByte*>(settings[s])->getValue();
}

RGB_Color Settings::getRGB(SettingID s) {
  return static_cast<Setting_RGB*>(settings[s])->getValue();
}

void Settings::setInt(SettingID s,int v) {
  static_cast<Setting_Int*>(settings[s])->setValue(v);
}

void Settings::setBool(SettingID s,bool v) {
  static_cast<Setting_Bool*>(settings[s])->setValue(v);
}

void Settings::setFloat(SettingID s,float v) {
  static_cast<Setting_Float*>(settings[s])->setValue(v);
}

void Settings::setUByte(SettingID s,unsigned char v) {
  static_cast<Setting_UByte*>(settings[s])->setValue(v);
}

void Settings::setRGB(SettingID s,RGB_Color v) {
  static_cast<Setting_RGB*>(settings[s])->setValue(v);
}
