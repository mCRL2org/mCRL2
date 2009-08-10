// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utils.cpp
/// \brief Utility functions.

#include "utils.h"
#include <math.h>

namespace Utils
{

Utils::Vect operator+(Vect v1, Vect v2)
{
  Vect result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  result.z = v1.z + v2.z;
  return result;
}

Utils::Vect operator-(Vect v1, Vect v2)
{
  Vect result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
  return result;
}

Utils::Vect operator*(double s, Vect v)
{
  Vect result = {s * v.x, s * v.y, s * v.z};
  return result;
}

Utils::Vect operator/(Vect v, double s)
{
  Vect result = {v.x / s, v.y / s, v.z / s};
  return result;
}

float vecLength(Vect v)
{
  return static_cast< float >(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

float angDiff(Vect v1, Vect v2)
{
  float dotP = static_cast<float>(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
  float lenP = static_cast<float>(vecLength(v1) * vecLength(v2));

  return acosf(dotP / lenP);

}

float signum(float v)
{
  if (v < 0)
  {
    return -1.0f;
  }
  else
  {
    return 1.0f;
  }
}


}
