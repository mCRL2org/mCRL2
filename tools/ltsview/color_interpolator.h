// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file color_interpolator.h
/// \brief Header file for the color interpolator class

#ifndef COLOR_INTERPOLATOR_H
#define COLOR_INTERPOLATOR_H

#include <vector>
#include "rgb_color.h"

class ColorInterpolator
{
  private:
    std::vector< RGB_Color > colors;
    static void RGBToHSV(RGB_Color &c, int &h, int &s, int &v);
    static RGB_Color HSVToRGB(int h, int s, int v);
  public:
    ColorInterpolator() {} 
    ~ColorInterpolator() {}
    void computeColors(RGB_Color c1, RGB_Color c2, int n, bool is_long);
    RGB_Color getColor(int i) { return colors[i]; }
};
#endif
