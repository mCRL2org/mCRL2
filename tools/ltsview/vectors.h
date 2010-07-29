// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file vectors.h
/// \brief Header file for Vector classes

#ifndef VECTORS_H
#define VECTORS_H

class Vector2D
{
  private:
    float _x;
    float _y;

  public:
    Vector2D() {}
    Vector2D(float x, float y): _x(x), _y(y) {}
    Vector2D(float deg);
    ~Vector2D() {}

    float x() { return _x; }
    float y() { return _y; }
    float length();
    float toDegrees();

    Vector2D operator+=(Vector2D v);
    Vector2D operator+(Vector2D v);
    Vector2D operator-(Vector2D v);
    Vector2D operator*(float s);
};

class Vector3D
{
  private:
    float _x;
    float _y;
    float _z;

  public:
    Vector3D() {}
    Vector3D(float x, float y, float z): _x(x), _y(y), _z(z) {}
    ~Vector3D() {}
    float x() { return _x; }
    float y() { return _y; }
    float z() { return _z; }
    float length();
    void normalize();
    float dot_product(Vector3D p);
    Vector3D cross_product(Vector3D p);
    Vector3D operator+(Vector3D p);
    Vector3D operator-(Vector3D p);
    Vector3D operator*(float s);
};


#endif
