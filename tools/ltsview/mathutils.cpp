// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mathutils.cpp
/// \brief Source file for MathUtils namespace

#include "wx.hpp" // precompiled headers
#include "mathutils.h"

namespace MathUtils
{
  int round_to_int(float f)
  {
    if (f >= 0.0f)
    {
      return static_cast< int > (f + 0.5f);
    }
    else
    {
      return static_cast< int > (f - 0.5f);
    }
  }

  float deg_to_rad(float deg)
  {
    return deg * PI / 180.0f;
  }

  float rad_to_deg(float rad)
  {
    return rad * 180.0f / PI;
  }
}
