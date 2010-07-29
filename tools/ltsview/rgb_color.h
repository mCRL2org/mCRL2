// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rgb_color.h
/// \brief Header file for the RGB color class

#ifndef RGB_COLOR_H
#define RGB_COLOR_H
#include <wx/colour.h>

class RGB_Color
{
  private:
    unsigned char _red;
    unsigned char _green;
    unsigned char _blue;

    static unsigned char blend(unsigned char c1, unsigned char c2, float f);

  public:
    /*
    static const RGB_Color BLUE = RGB_Color(0, 0, 255);
    static const RGB_Color ORANGE = RGB_Color(255, 122, 0);
    static const RGB_Color RED = RGB_Color(255, 0, 0);
    static const RGB_Color YELLOW = RGB_Color(255, 255, 0);
    static const RGB_Color WHITE = RGB_Color(255, 255, 255);
    */

    RGB_Color() {}
    RGB_Color(unsigned char r, unsigned char g, unsigned char b)
      : _red(r), _green(g), _blue(b) {}
    RGB_Color(wxColour c) : _red(c.Red()), _green(c.Green()), _blue(c.Blue()) {}
    unsigned char red() { return _red; }
    unsigned char green() { return _green; }
    unsigned char blue() { return _blue; }
    wxColour toWxColour() { return wxColour(_red, _green, _blue); }
    void blendWith(RGB_Color c, float factor);
    bool operator==(RGB_Color &c);
    bool operator!=(RGB_Color &c);
};
#endif
