// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./utils.h

#ifndef UTILS_H
#define UTILS_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <string>
#include <map>
#include <set>
#include <vector>

const double PI = 3.14159265;
const double E  = 2.71828183;

class Utils
{
  public:
    // -- std::string conversion functions -------------------------------
    static std::string dblToStr(const double& d);
    static std::string intToStr(const int& i);
    static std::string size_tToStr(const size_t& i);
    static double strToDbl(const std::string& s);
    static int strToInt(const std::string& s);

    // -- trig functions --------------------------------------------
    static double radToDegr(const double& r);
    static double degrToRad(const double& d);

    static double calcAngleDg(
      const double& x,
      const double& y);
    static double calcAngleRd(
      const double& x,
      const double& y);

    static double distLinePoint(
      const QPointF& lineStart,
      const QPointF& lineEnd,
      const QPointF& point);

    // -- math functions --------------------------------------------
    static int rndToInt(const double& f);
    static double rndToNearestMult(
      const double& value,
      const double& factor);

    static double dist(
      const double& x1, const double& y1,
      const double& x2, const double& y2);
    static double abs(const double& val);
    static double maxx(
      const double& d0,
      const double& d1);
    static double minn(
      const double& d0,
      const double& d1);

    static double perc(
      const int& numr,
      const int& denm);
    static double perc(
      const double& numr,
      const double& denm);

    static double fishEye(
      const double& distortion,
      const double& value);

    // -- statistics functions --------------------------------------
    static double mean(const std::vector< double > vals);
    static double variance(const std::vector< double > vals);
    static double stdDev(const std::vector< double > vals);
};

#endif

// -- end -----------------------------------------------------------
