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

void Vector2D::toPolar(float& angle, float& radius) const
{
  angle = MathUtils::rad_to_deg(atan2(_y, _x));
  if (angle < 0.0f)
  {
    angle += 360.0f;
  }
  radius = length();
}

float Vector2D::length() const
{
  return sqrt(_x * _x + _y * _y);
}

Vector2D Vector2D::fromPolar(float angle, float radius)
{
  float angle_rad = MathUtils::deg_to_rad(angle);
  return Vector2D(radius * cos(angle_rad), radius * sin(angle_rad));
}

Vector2D Vector2D::operator+=(const Vector2D& v)
{
  _x += v.x();
  _y += v.y();
  return *this;
}

Vector2D Vector2D::operator+(const Vector2D& v) const
{
  return Vector2D(_x + v.x(), _y + v.y());
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
  return Vector2D(_x - v.x(), _y - v.y());
}

Vector2D Vector2D::operator*(float s) const
{
  return Vector2D(s * _x, s * _y);
}

float Vector3D::length() const
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

Vector3D Vector3D::operator+=(const Vector3D& v)
{
  _x += v.x();
  _y += v.y();
  _z += v.z();
  return *this;
}

Vector3D Vector3D::operator+(const Vector3D& w) const
{
  return Vector3D(_x + w.x(), _y + w.y(), _z + w.z());
}

Vector3D Vector3D::operator-(const Vector3D& w) const
{
  return Vector3D(_x - w.x(), _y - w.y(), _z - w.z());
}

Vector3D Vector3D::operator*(float s) const
{
  return Vector3D(s * _x, s * _y, s * _z);
}

float Vector3D::dot_product(const Vector3D& w) const
{
  return _x * w.x() + _y * w.y() + _z * w.z();
}

Vector3D Vector3D::cross_product(const Vector3D& w) const
{
  return Vector3D(
           _y * w.z() - _z * w.y(),
           _z * w.x() - _x * w.z(),
           _x * w.y() - _y * w.x());
}
