// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rgb_color.cpp
/// \brief Source file for the RGB color class

#include "wx.hpp" // precompiled headers

#include "rgb_color.h"

using namespace std;

unsigned char RGB_Color::blend(unsigned char c1, unsigned char c2, float f)
{
  return static_cast<float>(c1 * f) + static_cast<float>(c2 * (1 - f));
}

void RGB_Color::blendWith(RGB_Color c, float factor)
{
  _red = blend(_red, c.red(), factor);
  _green = blend(_green, c.green(), factor);
  _blue = blend(_blue, c.blue(), factor);
}

bool RGB_Color::operator==(RGB_Color &c)
{
  return _red == c.red() && _green == c.green() && _blue == c.blue();
}

bool RGB_Color::operator!=(RGB_Color &c)
{
  return _red != c.red() || _green != c.green() || _blue != c.blue();
}
