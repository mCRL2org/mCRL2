// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file color_interpolator.cpp
/// \brief Source file for the color interpolator class

#include "wx.hpp" // precompiled headers

#include <algorithm>
#include "color_interpolator.h"
#include "mathutils.h"
#include "rgb_color.h"

using namespace std;
using namespace MathUtils;

void ColorInterpolator::RGBToHSV(RGB_Color &c, int &h, int &s, int &v)
{
  unsigned char rgb_min = min(c.red(), min(c.green(), c.blue()));
  unsigned char rgb_max = max(c.red(), max(c.green(), c.blue()));

  h = 0;
  if (rgb_max != rgb_min)
  {
    float hd = 0;
    if (rgb_max == c.red())
    {
      hd = static_cast<float>(c.green() - c.blue()) / static_cast<float>(rgb_max - rgb_min);
      if (c.green() < c.blue())
      {
        hd += 6.0;
      }
    }
    else if (rgb_max == c.green())
    {
      hd = static_cast<float>(c.blue() - c.red()) / static_cast<float>(rgb_max - rgb_min) + 2.0;
    }
    else // rgb_max == c.blue()
    {
      hd = static_cast<float>(c.red() - c.green()) / static_cast<float>(rgb_max - rgb_min) + 4.0;
    }
    h = round_to_int(60.0 * hd);
  }

  s = 0;
  if (rgb_max != 0)
  {
    s = round_to_int(100.0 * (1.0f - static_cast<float>(rgb_min) / static_cast<float>(rgb_max)));
  }

  v = round_to_int(static_cast<float>(rgb_max) / 2.55f);
}

RGB_Color ColorInterpolator::HSVToRGB(int h, int s, int v)
{
  float sd = static_cast<float>(s) / 100;
  float vd = static_cast<float>(v) / 100;

  int hi = (h / 60) % 6;
  float f = static_cast<float>(h) / 60.0f - hi;
  float r, g, b;
  switch (hi) {
    case 0:
      r = vd;
      g = vd*(1-(1-f)*sd);
      b = vd*(1-sd);
      break;
    case 1:
      r = vd*(1-f*sd);
      g = vd;
      b = vd*(1-sd);
      break;
    case 2:
      r = vd*(1-sd);
      g = vd;
      b = vd*(1-(1-f)*sd);
      break;
    case 3:
      r = vd*(1-sd);
      g = vd*(1-f*sd);
      b = vd;
      break;
    case 4:
      r = vd*(1-(1-f)*sd);
      g = vd*(1-sd);
      b = vd;
      break;
    case 5:
      r = vd;
      g = vd*(1-sd);
      b = vd*(1-f*sd);
      break;
    default:
      r = 0;
      g = 0;
      b = 0;
      break;
  }
  return RGB_Color(
    static_cast<unsigned char>(r * 255),
    static_cast<unsigned char>(g * 255),
    static_cast<unsigned char>(b * 255) );
}

/* Precompute color interpolations between c1 and c2. After calling this
 * function, we have:
 * - getColor(0) == c1 
 * - getColor(n+1) == c2 
 * - and for all i, 1 <= i <= n, getColor(i) is the i'th interpolated
 *   color between c1 and c2.
 * Interpolation is done over the Hue dimension of the HSV color space.
 * The is_long parameter indicates whether we interpolate over the
 * longest possible interval between hue(c1) and hue(c2).
 */
void ColorInterpolator::computeColors(RGB_Color c1, RGB_Color c2, int n, bool is_long)
{
  colors.push_back(c1);
  if (n > 0)
  {
    int h1, s1, v1, h2, s2, v2;

    RGBToHSV(c1, h1, s1, v1);
    RGBToHSV(c2, h2, s2, v2);

    // set hues equal if one of the colors is grey
    if (c1.red() == c1.green() && c1.green() == c1.blue())
    {
      h1 = h2;
    }
    if (c2.red() == c2.green() && c2.green() == c2.blue())
    {
      h2 = h1;
    }
    
    int delta = h2 - h1;
    if ((is_long && abs(delta) < 180) || (!is_long && abs(delta) >= 180))
    {
      if (delta > 0)
      {
        delta -= 360;
      }
      else
      {
        delta += 360;
      }
    }

    float h_i = static_cast<float>(h1);
    float s_i = static_cast<float>(s1);
    float v_i = static_cast<float>(v1);
    float delta_h = static_cast<float>(delta) / static_cast<float>(n + 1);
    float delta_s = static_cast<float>(s2 - s1) /
      static_cast<float>(n + 1);
    float delta_v = static_cast<float>(v2 - v1) /
      static_cast<float>(n + 1);

    for (int i = 0; i < n; ++i)
    {
      h_i += delta_h;
      s_i += delta_s;
      v_i += delta_v;
      if (h_i < 0.0f)
      {
        h_i += 360.0f;
      }
      if (h_i >= 360.0f)
      {
        h_i -= 360.0f;
      }
      colors.push_back(HSVToRGB(round_to_int(h_i), round_to_int(s_i),
            round_to_int(v_i)));
    }
  }
  colors.push_back(c2);
}
