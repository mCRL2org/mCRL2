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

  double vecLength(Vect v);
  double angDiff(Vect v1, Vect v2);
  float signum(float f);
  void MultGLMatrices (const double leftm[16], const double rightm[16], double* ans);
  void genRotArbAxs (const double angle, const double x1, const double y1, const double z1, double* rotmtrx);
  void GLUnTransform (const double leftm[16], const double rightm[4], double* ans);

}
#endif
