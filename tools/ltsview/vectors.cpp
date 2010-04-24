// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file vectors.cpp
/// \brief Implementation of the Vector classes

#include <math.h>
#include "mathutils.h"
#include "vectors.h"

Vector2D::Vector2D(float deg)
{
  float r = MathUtils::deg_to_rad(deg);
  _x = cos(r);
  _y = sin(r);
}

float Vector2D::toDegrees()
{
  float r = MathUtils::rad_to_deg(atan2(_y, _x));
  if (r < 0.0f)
  {
    r += 360.0f;
  }
  return r;
}

float Vector2D::length()
{
  return sqrt(_x * _x + _y * _y);
}

Vector2D Vector2D::operator+(Vector2D v)
{
  return Vector2D(_x + v.x(), _y + v.y());
}

Vector2D Vector2D::operator*(float s)
{
  return Vector2D(s * _x, s * _y);
}

float Vector3D::length()
{
  return sqrt(_x * _x + _y * _y + _z * _z);
}

void Vector3D::normalize()
{
  float len = length();
  if (len != 0.0f)
  {
    _x /= len;
    _y /= len;
    _z /= len;
  }
}

Vector3D Vector3D::operator+(Vector3D p)
{
  return Vector3D(_x + p.x(), _y + p.y(), _z + p.z());
}

Vector3D Vector3D::operator-(Vector3D p)
{
  return Vector3D(_x - p.x(), _y - p.y(), _z - p.z());
}

Vector3D Vector3D::operator*(float s)
{
  return Vector3D(s * _x, s * _y, s * _z);
}

float Vector3D::dot_product(Vector3D p)
{
  return _x * p.x() + _y * p.y() + _z * p.z();
}

Vector3D Vector3D::cross_product(Vector3D p)
{
  return Vector3D(
      _y * p.z() - _z * p.y(),
      _z * p.x() - _x * p.z(),
      _x * p.y() - _y * p.x() );
}
