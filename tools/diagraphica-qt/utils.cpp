// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./utils.cpp

#include "utils.h"
#include <iostream>
#include <sstream>
using namespace std;


// -- string conversion functions -----------------------------------


string Utils::dblToStr(const double& d)
{
  ostringstream oss;
  string result;

  oss.precision(3); // set precision
  oss << d;
  result = oss.str();

  return result;
}


string Utils::intToStr(const int& i)
{
  ostringstream oss;
  string result;

  oss << i;
  result = oss.str();

  return result;
}

string Utils::size_tToStr(const size_t& i)
{
  ostringstream oss;
  string result;

  oss << i;
  result = oss.str();

  return result;
}

double Utils::strToDbl(const string& s)
{
  double result;
  result = atof(s.c_str());

  return result;
}


int Utils::strToInt(const string& s)
{
  int result;

  result = atoi(s.c_str());

  return result;
}


// -- trig functions ------------------------------------------------


double Utils::radToDegr(const double& r)
{
  double degrees = 0;
  degrees = r * (180.0 / PI);
  return degrees;
}


double Utils::degrToRad(const double& d)
{
  double radians = 0;
  radians = d * (PI / 180.0);
  return radians;
}


double Utils::calcAngleDg(
  const double& x,
  const double& y)
// This function calculates the angle between the x-axis and the line
// passing through the points (0,0) and (x,y). The result is returned
// in degrees.
{
  return Utils::radToDegr(calcAngleRd(x, y));
}


double Utils::calcAngleRd(
  const double& x,
  const double& y)
// This function calculates the angle between the x-axis and the line
// passing through the points (0,0) and (x,y). The result is returned
// in radians.
{
  double angleRd;

  if (x != 0)
  {
    angleRd = atan(y/x);

    if (x < 0 && y >= 0)
      // 2nd quadrant: angle neg -> add to PI radians
    {
      angleRd = PI + angleRd;
    }
    else if (x < 0 && y < 0)
      // 3rd quadrant, angle pos -> add to PI radians
    {
      angleRd = PI + angleRd;
    }
    else if (x > 0 && y < 0)
      // 4th quadrant, angle neg -> add to 2*PI radians
    {
      angleRd = 2*PI + angleRd;
    }
  }
  else if (y > 0)
    // first undefined angle: (1/2)*PI
  {
    angleRd = 0.5*PI;
  }
  else
    // second undefined angle: (3/2)*PI
  {
    angleRd = 1.5*PI;
  }

  return angleRd;
}


// -- math functions ------------------------------------------------


int Utils::rndToInt(const double& f)
// Code thanks to S.W.C. Ploeger.
{
  double intpart;
  modf(f + 0.5, &intpart);
  return static_cast< int >(intpart);
}


double Utils::rndToNearestMult(
  const double& value,
  const double& factor)
// Round value to the nearest multiple of factor.
{
  double result;

  double intPtVal;
  double dblPtVal;
  dblPtVal = modf(value/factor, &intPtVal);
  if (abs(dblPtVal) < 0.5)
  {
    result = intPtVal*factor;
  }
  else
  {
    if (dblPtVal < 0)
    {
      result = (intPtVal-1)*factor;
    }
    else
    {
      result = (intPtVal+1)*factor;
    }
  }

  return result;
}


double Utils::dist(
  const double& x1, const double& y1,
  const double& x2, const double& y2)
{
  double result;
  result = pow(x1 - x2, 2) + pow(y1 - y2, 2);
  result = sqrt(result);
  return result;
}


double Utils::abs(const double& val)
{
  double result = val;
  if (result < 0.0)
  {
    result = -1.0*result;
  }
  return result;
}


double Utils::maxx(
  const double& d0,
  const double& d1)
{
  double result = d0;
  if (d0 < d1)
  {
    result = d1;
  }
  return result;
}


double Utils::minn(
  const double& d0,
  const double& d1)
{
  double result = d0;
  if (d1 < d0)
  {
    result = d1;
  }
  return result;
}


double Utils::perc(
  const int& numr,
  const int& denm)
{
  return Utils::perc(
           (double)numr,
           (double)denm);
}

double Utils::perc(
  const double& numr,
  const double& denm)
{
  return (numr/denm)*100.0;
}


double Utils::fishEye(
  const double& distortion,
  const double& value)
// Graphical Fisheye Function (Sarkar and Brown,1994)
// In: dist the distortion factor
//     value in [0, 1]
// Out: return value in [0, 1]
// Focus is at 0.
{
  double result;
  result = ((distortion + 1)*value) / (distortion*value + 1);
  return result;
}


// -- statistics functions --------------------------------------


double Utils::mean(const vector< double > vals)
{
  double result = 0;
  if (vals.size() > 0)
  {
    for (size_t i = 0; i < vals.size(); i++)
    {
      result += vals[i];
    }
    result = result/(double)vals.size();
  }
  return result;
}


double Utils::variance(const vector< double > vals)
{
  double result = 0;
  double mean = Utils::mean(vals);

  if (vals.size() > 1)
  {
    for (size_t i = 0; i < vals.size(); i++)
    {
      result += (vals[i]-mean)*(vals[i]-mean);
    }
    result = result/(double)(vals.size()-1);
  }

  return result;
}


double Utils::stdDev(const vector< double > vals)
{
  double result = 0;
  result = sqrt(Utils::variance(vals));
  return result;
}


// -- end -----------------------------------------------------------
