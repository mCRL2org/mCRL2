// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SETTINGS_H
#define SETTINGS_H

#include "mcrl2/utilities/setting.h"
#include "visutils.h"

struct Settings
{
  typedef mcrl2::utilities::qt::SettingInt SettingInt;
  typedef mcrl2::utilities::qt::SettingFloat SettingFloat;
  typedef mcrl2::utilities::qt::SettingBool SettingBool;
  typedef mcrl2::utilities::qt::SettingColor SettingColor;
  typedef mcrl2::utilities::qt::SettingEnum SettingEnum;
  typedef SettingEnum::Item Item;

  SettingColor backgroundColor;
  SettingColor textColor;
  SettingInt textSize;
  SettingFloat animationSpeed;
  SettingEnum blendType;

  SettingBool showClusters;
  SettingBool showBundles;
  SettingBool showClusterTree;
  SettingBool annotateClusterTree;
  SettingBool showBarTree;
  SettingColor bundleColor;
  SettingFloat arcTransparency;
  SettingEnum clusterTreeColorMap;
  SettingFloat barTreeMagnification;

  Settings():
    backgroundColor(QColor(Qt::white)),
    textColor(QColor(Qt::black)),
    textSize(12),
    animationSpeed(10.0),
    blendType(QList<Item>()
      << Item(VisUtils::BLEND_HARD, "Hard transition")
      << Item(VisUtils::BLEND_LINEAR, "Linear")
      << Item(VisUtils::BLEND_CONCAVE, "Concave")
      << Item(VisUtils::BLEND_CONVEX, "Convex")
      << Item(VisUtils::BLEND_OSCILLATE, "Oscillate")
      ),
    showClusters(true),
    showBundles(true),
    showClusterTree(true),
    annotateClusterTree(true),
    showBarTree(true),
    bundleColor(QColor(Qt::black)),
    arcTransparency(0.3),
    clusterTreeColorMap(QList<Item>()
      << Item(VisUtils::COL_MAP_QUAL_PAST_1, "Pastel 1 (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_PAST_2, "Pastel 2 (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_SET_1, "Set 1 (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_SET_2, "Set 2 (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_SET_3, "Set 3 (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_PAIR, "Paired (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_DARK, "Dark (Qual)")
      << Item(VisUtils::COL_MAP_QUAL_ACCENT, "Accents (Qual)")
      ),
    barTreeMagnification(0.0)
  {}
};

#endif
