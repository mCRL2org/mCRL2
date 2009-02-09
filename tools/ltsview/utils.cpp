// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utils.cpp
/// \brief Source file for Utils namespace

#include "wx.hpp" // precompiled headers

#include <algorithm> // for std::min/std::max
#include "utils.h"
#include <cmath>

namespace Utils
{

bool operator==(RGB_Color c1,RGB_Color c2) {
  return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b;
}

bool operator!=(RGB_Color c1,RGB_Color c2) {
  return !(c1 == c2);
}

Utils::Point3D operator+(Point3D p1,Point3D p2) {
  Point3D result = { p1.x+p2.x, p1.y+p2.y, p1.z+p2.z };
  return result;
}

Utils::Point3D operator-(Point3D p1,Point3D p2) {
  Point3D result = { p1.x-p2.x, p1.y-p2.y, p1.z-p2.z };
  return result;
}

Utils::Point3D operator*(float s,Point3D p) {
  Point3D result = { s*p.x, s*p.y, s*p.z };
  return result;
}

Utils::Vect operator+(Vect v1, Vect v2) {
  Vect result = { v1.x + v2.x, v1.y + v2.y };
  return result;
}

Utils::Vect operator*(float s,Vect v) {
  Vect result = { s*v.x, s*v.y };
  return result;
}

float length(Point3D p) {
  return sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
}

void normalize(Point3D &p) {
  float len = length( p );
  if (len != 0.0f) {
    p = (1.0f / len) * p;
  }
}

float dot_product(Point3D p1,Point3D p2) {
  return (p1.x*p2.x + p1.y*p2.y + p1.z*p2.z);
}

Utils::Point3D cross_product(Point3D p1,Point3D p2) {
  Point3D result = { p1.y*p2.z - p1.z*p2.y,
                     p1.z*p2.x - p1.x*p2.z,
                     p1.x*p2.y - p1.y*p2.x };
  return result;
}

Utils::HSV_Color operator+(HSV_Color c1,HSV_Color c2) {
  HSV_Color result = { c1.h+c2.h, c1.s+c2.s, c1.v+c2.v };
  while (result.h < 0)    result.h += 360;
  while (result.h >= 360) result.h -= 360;
  if (result.s < 0)       result.s = 0;
  if (result.s > 100)     result.s = 100;
  if (result.v < 0)       result.v = 0;
  if (result.v > 100)     result.v = 100;
  return result;
}

Utils::RGB_Color blend_RGB(RGB_Color c1, RGB_Color c2, float factor)
{
  RGB_Color result = {static_cast<unsigned char>(
                        static_cast<float>(c1.r * factor) +
                        static_cast<float>(c2.r * (1 - factor))
                      ),
                      static_cast<unsigned char> (
                        static_cast<float>(c1.g * factor) +
                        static_cast<float>(c2.g * (1 - factor))
                      ),
                      static_cast<unsigned char> (
                        static_cast<float>(c1.b * factor) +
                        static_cast<float>(c2.b * (1 - factor))
                      )};

  return result;
}


float deg_to_rad(float deg) {
  return deg * Utils::PI / 180.0f;
}

float rad_to_deg(float rad) {
  return rad * 180.0f / Utils::PI;
}

Utils::HSV_Color RGB_to_HSV(RGB_Color c) {
  unsigned char MIN = (std::min)(c.r,(std::min)(c.g,c.b));
  unsigned char MAX = (std::max)(c.r,(std::max)(c.g,c.b));

  HSV_Color result;
  if (MAX == MIN) {
    result.h = 0;
  }
  else if (MAX == c.r) {
    if (c.g >= c.b) {
      result.h = round_to_int(60.0 * double(c.g-c.b)/double(MAX-MIN));
    }
    else {
      result.h = round_to_int(60.0 * double(c.g-c.b)/double(MAX-MIN)) + 360;
    }
  }
  else if (MAX == c.g) {
    result.h = round_to_int(60.0 * double(c.b-c.r)/double(MAX-MIN)) + 120;
  }
  else {
    result.h = round_to_int(60.0 * double(c.r-c.g)/double(MAX-MIN)) + 240;
  }

  if (MAX == 0) {
    result.s = 0;
  } else {
    result.s = round_to_int(100 * (1.0 - double(MIN)/double(MAX)));
  }
  result.v = round_to_int(double(MAX)/2.55);
  return result;
}

Utils::RGB_Color HSV_to_RGB(HSV_Color c) {
  int hi = (c.h/60) % 6;
  double H = double(c.h);
  double S = double(c.s) / 100;
  double V = double(c.v) / 100;
  double f = H / 60.0 - hi;
  double r,g,b;
  switch (hi) {
    case 0:
      r = V;
      g = V*(1-(1-f)*S);
      b = V*(1-S);
      break;
    case 1:
      r = V*(1-f*S);
      g = V;
      b = V*(1-S);
      break;
    case 2:
      r = V*(1-S);
      g = V;
      b = V*(1-(1-f)*S);
      break;
    case 3:
      r = V*(1-S);
      g = V*(1-f*S);
      b = V;
      break;
    case 4:
      r = V*(1-(1-f)*S);
      g = V*(1-S);
      b = V;
      break;
    case 5:
      r = V;
      g = V*(1-S);
      b = V*(1-f*S);
      break;
    default:
      r = 0;
      g = 0;
      b = 0;
      break;
  }
  RGB_Color result = {
    static_cast<unsigned char>(r*255),
    static_cast<unsigned char>(g*255),
    static_cast<unsigned char>(b*255) };
  return result;
}

int round_to_int(double f) {
  return static_cast< int > (f+0.5);
}

float truncate_float(float f) {
  return float(int(f * 1000000.0f)) / 1000000.0f;
}

float vec_to_deg(Utils::Vect v) {
  float r = rad_to_deg(atan2(v.y,v.x));
  if (r < 0.0f) {
    r += 360.0f;
  }
  return r;
}

Utils::Vect deg_to_vec(float deg) {
  Vect v;
  float r = deg_to_rad(deg);
  v.x = cos(r);
  v.y = sin(r);
  return v;
}

float vec_length(Vect v) {
  return sqrt(v.x*v.x + v.y*v.y);
}

Interpolater::Interpolater(RGB_Color rgb1,RGB_Color rgb2,int n,bool l) {
  base = RGB_to_HSV(rgb1);
  HSV_Color col2 = RGB_to_HSV(rgb2);
  if (rgb1.r == rgb1.g  &&  rgb1.g == rgb1.b) {
    base.h = col2.h;
  }
  if (rgb2.r == rgb2.g  &&  rgb2.g == rgb2.b) {
    col2.h = base.h;
  }
  delta_h1 = col2.h - base.h;
  delta_h2 = 360 - abs(delta_h1);
  if (delta_h1 >= 0) {
    delta_h2 = delta_h1 - 360;
  }
  else {
    delta_h2 = delta_h1 + 360;
  }
  N = n-1;
  is_long = l;
  delta_s = col2.s - base.s;
  delta_v = col2.v - base.v;
}

Utils::RGB_Color Interpolater::getColor(int i) {
  float r = float(i);
  if (N > 1) {
    r /= float(N);
  }
  HSV_Color result;
  if (is_long) {
    if (abs(delta_h1) < abs(delta_h2)) {
      result.h = round_to_int(base.h + r*delta_h2);
    } else {
      result.h = round_to_int(base.h + r*delta_h1);
    }
  } else {
    if (fabs(float(delta_h1)) < fabs(float(delta_h2))) {
      result.h = round_to_int(base.h + r*delta_h1);
    } else {
      result.h = round_to_int(base.h + r*delta_h2);
    }
  }
  while (result.h < 0) {
    result.h += 360;
  }
  while (result.h >= 360) {
    result.h -= 360;
  }
  result.s = round_to_int(base.s + r*delta_s);
  result.v = round_to_int(base.v + r*delta_v);
  return HSV_to_RGB(result);
}

wxColour RGB_to_wxC(RGB_Color c) {
  return wxColour(c.r,c.g,c.b);
}

Utils::RGB_Color wxC_to_RGB(wxColour c) {
  RGB_Color result = {c.Red(),c.Green(),c.Blue()};
  return result;
}

}
