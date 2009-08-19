// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utils.h
/// \brief Utility classes and functions.

#ifndef UTILS_H
#define UTILS_H

namespace Utils {
  struct Vect {
    double x;
    double y;
	double z;
  };

  Vect operator+(Vect v1, Vect v2);
  Vect operator-(Vect v1, Vect v2);
  Vect operator*(double s, Vect v);
  Vect operator/(Vect v, double s);

  float vecLength(Vect v);
  float angDiff(Vect v1, Vect v2);
  float signum(float f);
  void MultGLMatrices (const float leftm[16], const float rightm[16], float* ans);
  void genRotArbAxs (const float angle, const float x1, const float y1, const float z1, float* rotmtrx);

}
#endif
