// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef VECTORS_H
#define VECTORS_H

class Vector2D
{
  public:
    Vector2D(): _x(0.0f), _y(0.0f) {}
    Vector2D(float x, float y): _x(x), _y(y) {}
    ~Vector2D() {}
    float length() const;
    Vector2D operator+=(const Vector2D& v);
    Vector2D operator+(const Vector2D& v) const;
    Vector2D operator-(const Vector2D& v) const;
    Vector2D operator*(float s) const;
    void toPolar(float& angle, float& radius) const;
    float x() const
    {
      return _x;
    }
    float y() const
    {
      return _y;
    }

    static Vector2D fromPolar(float angle, float radius);

  private:
    float _x;
    float _y;
};

class Vector3D
{
  public:
    Vector3D(): _x(0.0f), _y(0.0f), _z(0.0f) {}
    Vector3D(float x, float y, float z): _x(x), _y(y), _z(z) {}
    ~Vector3D() {}
    Vector3D cross_product(const Vector3D& w) const;
    float dot_product(const Vector3D& w) const;
    float length() const;
    void normalize();
    Vector3D operator+=(const Vector3D& v);
    Vector3D operator+(const Vector3D& w) const;
    Vector3D operator-(const Vector3D& w) const;
    Vector3D operator*(float s) const;
    float x() const
    {
      return _x;
    }
    float y() const
    {
      return _y;
    }
    float z() const
    {
      return _z;
    }

  private:
    float _x;
    float _y;
    float _z;
};


#endif
