// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utils.cpp
/// \brief Utility functions.

#include "utils.h"
#include <math.h>

namespace Utils
{

Utils::Vect operator+(Vect v1, Vect v2)
{
  Vect result;
  result.x = v1.x + v2.x;
  result.y = v1.y + v2.y;
  result.z = v1.z + v2.z;
  return result;
}

Utils::Vect operator-(Vect v1, Vect v2)
{
  Vect result = {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
  return result;
}

Utils::Vect operator*(double s, Vect v)
{
  Vect result = {s * v.x, s * v.y, s * v.z};
  return result;
}

Utils::Vect operator/(Vect v, double s)
{
  Vect result = {v.x / s, v.y / s, v.z / s};
  return result;
}

float vecLength(Vect v)
{
  return static_cast< float >(sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

float angDiff(Vect v1, Vect v2)
{
  float dotP = static_cast<float>(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
  float lenP = static_cast<float>(vecLength(v1) * vecLength(v2));

  return acosf(dotP / lenP);

}

float signum(float v)
{
  if (v < 0)
  {
    return -1.0f;
  }
  else
  {
    return 1.0f;
  }
}

//preq: ans is an array of size 16

void MultGLMatrices (const float lhs[16], const float rhs[16], float* ans)
{
	ans[0] = lhs[0] * rhs [0] + lhs[4] * rhs [1] + lhs[8] * rhs [2] + lhs[12] * rhs [3];
	ans[1] = lhs[1] * rhs [0] + lhs[5] * rhs [1] + lhs[9] * rhs [2] + lhs[13] * rhs [3];
	ans[2] = lhs[2] * rhs [0] + lhs[6] * rhs [1] + lhs[10] * rhs [2] + lhs[14] * rhs [3];
	ans[3] = lhs[3] * rhs [0] + lhs[7] * rhs [1] + lhs[11] * rhs [2] + lhs[15] * rhs [3];
	ans[4] = lhs[0] * rhs [4] + lhs[4] * rhs [5] + lhs[8] * rhs [6] + lhs[12] * rhs [7];
	ans[5] = lhs[1] * rhs [4] + lhs[5] * rhs [5] + lhs[9] * rhs [6] + lhs[13] * rhs [7];
	ans[6] = lhs[2] * rhs [4] + lhs[6] * rhs [5] + lhs[10] * rhs [6] + lhs[14] * rhs [7];
	ans[7] = lhs[3] * rhs [4] + lhs[7] * rhs [5] + lhs[11] * rhs [6] + lhs[15] * rhs [7];
	ans[8] = lhs[0] * rhs [8] + lhs[4] * rhs [9] + lhs[8] * rhs [10] + lhs[12] * rhs [11];
	ans[9] = lhs[1] * rhs [8] + lhs[5] * rhs [9] + lhs[9] * rhs [10] + lhs[13] * rhs [11];
	ans[10] = lhs[2] * rhs [8] + lhs[6] * rhs [9] + lhs[10] * rhs [10] + lhs[14] * rhs [11];
	ans[11] = lhs[3] * rhs [8] + lhs[7] * rhs [9] + lhs[11] * rhs [10] + lhs[15] * rhs [11];
	ans[12] = lhs[0] * rhs [12] + lhs[4] * rhs [13] + lhs[8] * rhs [14] + lhs[12] * rhs [15];
	ans[13] = lhs[1] * rhs [12] + lhs[5] * rhs [13] + lhs[9] * rhs [14] + lhs[13] * rhs [15];
	ans[14] = lhs[2] * rhs [12] + lhs[6] * rhs [13] + lhs[10] * rhs [14] + lhs[14] * rhs [15];
	ans[15] = lhs[3] * rhs [12] + lhs[7] * rhs [13] + lhs[11] * rhs [14] + lhs[15] * rhs [15];
}

//preq: the axis is on the z = 0 plane.
//		rotmtrx is an array of size 16

void genRotArbAxs (const float angle, const float x1, const float y1, const float z1, float* rotmtrx)
{
	//TODO: make rotation available around the z-axis
	float angleRad = angle * float(3.1415926535897932384626433832795) / 180.0f;
	float c = cos(angleRad);
	float s = sin(angleRad);
	float t = 1 - cos(angleRad);
	float angleUnitVect = atan2(y1, x1);
	float ax, ay, az;
	ax = -1.0f * sin(angleUnitVect);
	ay = cos(angleUnitVect);
	az = 0;
	rotmtrx[0] = t * ax * ax + c;
	rotmtrx[1] = t * ax * ay - s * az;
	rotmtrx[2] = t * ax * ay + s * ay;
	rotmtrx[3] = 0;
	rotmtrx[4] = t * ax * ay + s * az;
	rotmtrx[5] = t * ay * ay + c;
	rotmtrx[6] = t * ay * az - s * ax;
	rotmtrx[7] = 0;
	rotmtrx[8] = t * ax * az - s * ay;
	rotmtrx[9] = t * ay * az + s * ax;
	rotmtrx[10] = t * az * az + c;
	rotmtrx[11] = 0;
	rotmtrx[12] = 0;
	rotmtrx[13] = 0;
	rotmtrx[14] = 0;
	rotmtrx[15] = 1;
}

//preq: ans is an array of size 4
//		lhs is an only rotation matrix

void GLUnTransform (const float lhs[16], const float rhs[4], float* ans)
{
	ans[0] = lhs[0] * rhs [0] + lhs[1] * rhs [1] + lhs[2] * rhs [2] + lhs[3] * rhs [3];
	ans[1] = lhs[4] * rhs [0] + lhs[5] * rhs [1] + lhs[6] * rhs [2] + lhs[7] * rhs [3];
	ans[2] = lhs[8] * rhs [0] + lhs[9] * rhs [1] + lhs[10] * rhs [2] + lhs[11] * rhs [3];
	ans[3] = lhs[12] * rhs [0] + lhs[13] * rhs [1] + lhs[14] * rhs [2] + lhs[15] * rhs [3];
}


}
