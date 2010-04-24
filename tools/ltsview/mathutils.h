// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mathutils.h
/// \brief Header file for MathUtils namespace

#ifndef MATHUTILS_H
#define MATHUTILS_H

namespace MathUtils
{
  const float PI = 3.14159265359;
  int round_to_int(float f);
  float deg_to_rad(float deg);
  float rad_to_deg(float rad);
}
#endif
