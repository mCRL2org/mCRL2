// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utils.h
/// \brief Header file for Utils namespace

#ifndef UTILS_H
#define UTILS_H
#include <wx/colour.h>

namespace Utils {
  const double PI = 3.14159265359;
  const unsigned int NUM_RINGS = 2;

  enum RankStyle {ITERATIVE,CYCLIC};
  enum MarkStyle {NO_MARKS,MARK_DEADLOCKS,MARK_STATES,MARK_TRANSITIONS };
  enum MatchStyle {MATCH_ANY,MATCH_ALL,MATCH_MULTI};
  enum VisStyle {CONES,TUBES};
  // Allows picking processor to identify type of item picked.
  enum PickState {STATE, CLUSTER, SIMSTATE, PICKNONE};

  struct RGB_Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
  };

  const RGB_Color RGB_WHITE = {255,255,255};
  const RGB_Color RGB_BLUE  = {0,0,255};
  const RGB_Color RGB_RED   = {255,0,0};
  const RGB_Color RGB_YELLOW= {255,255,0};
  const RGB_Color RGB_ORANGE= {255,122,0};

  struct HSV_Color {
    int h;
    int s;
    int v;
  };

  struct Point3D {
    float x;
    float y;
    float z;
  };

  struct Vect {
    float x;
    float y;
  };

  int round_to_int(double f);
  float truncate_float(float f); // truncates all digits after the 4th decimal digit of f
  float deg_to_rad(float deg);
  float rad_to_deg(float rad);

  bool operator==(RGB_Color c1,RGB_Color c2);
  bool operator!=(RGB_Color c1,RGB_Color c2);
  HSV_Color operator+(HSV_Color c1,HSV_Color c2);
  HSV_Color RGB_to_HSV(RGB_Color c);
  RGB_Color HSV_to_RGB(HSV_Color c);

  // Blends c1 and c2 according to the formula:
  // result = c1 * factor + c2 *(1 - factor)
  RGB_Color blend_RGB(RGB_Color c1, RGB_Color c2, float factor);

  Point3D operator+(Point3D p1,Point3D p2);
  Point3D operator-(Point3D p1,Point3D p2);
  Point3D operator*(float s,Point3D p);
  Vect operator+(Vect v1,Vect v2);
  Vect operator*(float s,Vect v);
  float length(Point3D p);
  void normalize(Point3D &p);
  float dot_product(Point3D p1,Point3D p2);
  Point3D cross_product(Point3D p1,Point3D p2);
  float vec_to_deg(Vect v);
  Vect deg_to_vec(float deg);
  float vec_length(Vect v);
  wxColour RGB_to_wxC(Utils::RGB_Color c);
  RGB_Color wxC_to_RGB(wxColour c);

  class Interpolater {
    private:
      bool is_long;
      HSV_Color base;
      int delta_h1;
      int delta_h2;
      int delta_s;
      int delta_v;
      int N;
    public:
      Interpolater(RGB_Color hsv1,RGB_Color hsv2,int n,bool l);
      ~Interpolater() {}
      RGB_Color getColor(int i);
  };

}
#endif
