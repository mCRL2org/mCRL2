// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
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

#include <QColor>

/*
 * Unfortunately, MOC doesn't understand templates or macros, or this would be a whole lot cleaner.
 */
class SettingInt : public QObject
{
  Q_OBJECT
  public:
    SettingInt(int value): m_value(value), m_default(value) {}
    int value() { return m_value; }
  public slots:
    void reset() { setValue(m_default); }
    void setValue(int value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(int value);
  private:
    int m_value, m_default;
};

class SettingBool : public QObject
{
  Q_OBJECT
  public:
    SettingBool(bool value): m_value(value), m_default(value) {}
    bool value() { return m_value; }
  public slots:
    void reset() { setValue(m_default); }
    void setValue(bool value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(bool value);
  private:
    bool m_value, m_default;
};

class SettingFloat : public QObject
{
  Q_OBJECT
  public:
    SettingFloat(float value): m_value(value), m_default(value) {}
    float value() { return m_value; }
  public slots:
    void reset() { setValue(m_default); }
    void setValue(float value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(float value);
  private:
    float m_value, m_default;
};

class SettingColor : public QObject
{
  Q_OBJECT
  public:
    SettingColor(QColor value): m_value(value), m_default(value) {}
    QColor value() { return m_value; }
  public slots:
    void reset() { setValue(m_default); }
    void setValue(QColor value) { if (value != m_value) { m_value = value; emit changed(value); } }
  signals:
    void changed(QColor value);
  private:
    QColor m_value, m_default;
};

struct Settings
{
  SettingFloat stateSize;
  SettingFloat clusterHeight;
  SettingInt branchRotation;
  SettingInt branchTilt;
  SettingInt quality;
  SettingInt transparency;
  SettingColor backgroundColor;
  SettingColor stateColor;
  SettingColor downEdgeColor;
  SettingColor upEdgeColor;
  SettingColor markedColor;
  SettingColor clusterColorTop;
  SettingColor clusterColorBottom;
  SettingBool longInterpolation;
  SettingColor simPrevColor;
  SettingColor simCurrColor;
  SettingColor simSelColor;
  SettingColor simPosColor;
  SettingBool stateRankStyleCyclic;
  SettingBool fsmStyle;
  SettingBool statePosStyleMultiPass;
  SettingBool clusterVisStyleTubes;
  SettingBool navShowBackpointers;
  SettingBool navShowStates;
  SettingBool navShowTransitions;
  SettingBool navSmoothShading;
  SettingBool navLighting;
  SettingBool navTransparency;
  SettingBool displayBackpointers;
  SettingBool displayStates;
  SettingBool displayTransitions;
  SettingBool displayWireframe;

  Settings():
    stateSize(1.0f),
    clusterHeight(0.0f),
    branchRotation(0),
    branchTilt(30),
    quality(12),
    transparency(30),
    backgroundColor(QColor(150, 150, 150)),
    stateColor(QColor(255, 255, 255)),
    downEdgeColor(QColor(255, 255, 255)),
    upEdgeColor(QColor(0, 0, 255)),
    markedColor(QColor(255, 0, 0)),
    clusterColorTop(QColor(255, 255, 255)),
    clusterColorBottom(QColor(0, 0, 255)),
    longInterpolation(false),
    simPrevColor(QColor(255, 0, 255)),
    simCurrColor(QColor(0, 255, 0)),
    simSelColor(QColor(255, 255, 0)),
    simPosColor(QColor(0, 255, 255)),
    stateRankStyleCyclic(false),
    fsmStyle(false),
    statePosStyleMultiPass(false),
    clusterVisStyleTubes(false),
    navShowBackpointers(true),
    navShowStates(true),
    navShowTransitions(true),
    navSmoothShading(true),
    navLighting(true),
    navTransparency(true),
    displayBackpointers(false),
    displayStates(false),
    displayTransitions(false),
    displayWireframe(false)
  {}
};

#endif
