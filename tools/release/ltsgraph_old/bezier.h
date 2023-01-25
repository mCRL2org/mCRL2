// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_BEZIER_H
#define MCRL2_LTSGRAPH_BEZIER_H

#include <array>
#include <cstddef>
#include <functional>
#include <limits>
#include <utility>
#include <QVector3D>
#include <boost/math/tools/roots.hpp>

namespace Math {

using Scalar = float;
using Vector = QVector3D;

// Utility functions

constexpr Scalar clamp(Scalar t, Scalar min, Scalar max) 
{
  return t < min ? min : t > max ? max : t;
}

constexpr Vector lerp(const Vector& a, const Vector& b, Scalar t)
{
  return (static_cast<Scalar>(1) - t) * a + t * b;
}

constexpr Scalar dot(const Vector& v, const Vector& w)
{
  return v.x() * w.x() + v.y() * w.y() + v.z() * w.z();
}

constexpr Scalar length2(const Vector& v)
{
  return dot(v, v);
}

constexpr Scalar squared(const Scalar& s)
{
  return s * s;
}

/** \brief Representation of a geometric circle (or in 3d, a sphere) */
struct Circle
{
  Vector center;
  Scalar radius;

  /**
   * \brief Finds the closest point to a given point that is on the circle border.
   */
  inline Vector project(const Vector& point) const
  {
    return center + (point - center).normalized() * radius;
  }
};

/** \brief Representation of a bezier curve. */
template <int Order> class Bezier
{
  private:
    using VectorArray = std::array<Vector, Order + 1>;
    VectorArray m_control;

  public:
    constexpr Bezier() {}
    constexpr Bezier(const VectorArray& points) : m_control(points) {}

    constexpr Vector& operator[](int index) { return m_control[index]; }
    constexpr const Vector& operator[](int index) const { return m_control[index]; }
    constexpr operator VectorArray() { return m_control; }

    /** \brief Returns the start-point of the curve */
    constexpr const Vector& front() const { return m_control[0]; }
    /** \brief Returns the end-point of the curve */
    constexpr const Vector& back() const { return m_control[Order]; }
    /**
     * \brief Returns a point on the curve.
     * \pre 0 <= t <= 1
     */
    constexpr Vector at(Scalar t) const;
    /**
     * \brief Returns the slope of a point on the curve.
     * \pre 0 <= t <= 1
     */
    constexpr Vector tangent(Scalar t) const;
    /**
     * \brief Returns a set of points according to De Casteljau's algorithm.
     * \pre 0 <= t <= 1
     */
    constexpr std::array<Vector, Order * 2 + 1> interpolate(Scalar t) const;
    /**
     * \brief Returns a curve describing the portion [t, 1] of the original curve.
     * \pre 0 <= t <= 1
     */
    constexpr Bezier trimFront(Scalar t)
    {
      Bezier bezier;
      for (int i = 0; i < Order + 1; ++i)
        bezier[i] = interpolate(t)[i + Order];
      return bezier;
    }
    /**
     * \brief Returns a curve describing the portion [0, t] of the original curve.
     * \pre 0 <= t <= 1
     */
    constexpr Bezier trimBack(Scalar t)
    {
      Bezier bezier;
      for (int i = 0; i < Order + 1; ++i)
        bezier[i] = interpolate(t)[i];
      return bezier;
    }
};

/** \brief Representation of a 3th order bezier curve */
using CubicBezier = Bezier<3>;
/** \brief Representation of a 2nd order bezier curve */
using QuadraticBezier = Bezier<2>;

/** \brief Representation of an intersection between two geometric entities */
template <typename T, typename S> struct Intersection {};
/** \brief Returns an Intersection object of the given geometric entities */
template <typename T, typename S> constexpr Intersection<T, S>
  make_intersection(const T& entity1, const S& entity2)
{
  return Intersection<T, S>{entity1, entity2};
}

template <int Order> struct Intersection<Circle, Bezier<Order>>
{
  public:
    const Circle circle;
    const Bezier<Order> bezier;

    /** \brief Given an guess 't', returns a pair containing a new guess and tangent for the intersection point. */
    constexpr std::pair<Scalar, Scalar> operator()(const Scalar& t) const
    {
      // f(t) = |B(t) - C|^2 - r^2
      // f'(t) = 2[B(t) - C] * B'(t)
      const Vector v = bezier.at(t) - circle.center;
      const Scalar ft = length2(v) - (circle.radius * circle.radius);
      const Scalar dt = dot(v, bezier.tangent(t)) * static_cast<Scalar>(2);
      return std::make_pair(ft, dt);
    }

    /** \brief Returns a guess for the intersection point near the start-point of the curve. */
    inline Scalar guessNearFront() const
    {
      const Scalar n = length2(bezier[1] - bezier[0]);
      if (qFuzzyIsNull(n)) { return 0; }
      return clamp(circle.radius / std::sqrt(n), 0, 1) / static_cast<Scalar>(3);
    }

    /** \brief Returns a guess for the intersection point near the end-point of the curve. */
    inline Scalar guessNearBack() const
    {
      const Scalar n = length2(bezier[Order - 1] - bezier[Order]);
      if (qFuzzyIsNull(static_cast<Scalar>(1) - n)) { return 1; }
      return static_cast<Scalar>(1) - (clamp(circle.radius / std::sqrt(n), 0, 1) / static_cast<Scalar>(3));
    }

    /**
     * \brief Given a guess 't', returns the intersection point calculated by solving.
     * \throw exception no intersection found
     */
    constexpr Scalar solve(Scalar guess) const
    {
      using namespace boost::math::tools;
      return newton_raphson_iterate(*this, guess,
        static_cast<Scalar>(0), static_cast<Scalar>(1),
        std::numeric_limits<Scalar>::digits * .6);
    }
};

// Implementation for cubic bezier curves

template <> inline constexpr Vector Bezier<3>::at(Scalar t) const
{
  const Scalar t2  = t * t,
               t3  = t2 * t,
               tn  = 1.0f - t,
               tn2 = tn * tn,
               tn3 = tn2 * tn;
  return m_control[0] * tn3
       + m_control[1] * 3.0f * tn2 * t
       + m_control[2] * 3.0f * tn * t2
       + m_control[3] * t3;
}

template <> inline constexpr Vector Bezier<3>::tangent(Scalar t) const
{
  const Scalar t2  = t * t,
               tn  = 1.0f - t,
               tn2 = tn * tn;
  return (m_control[1] - m_control[0]) * 3.0f * tn2
       + (m_control[2] - m_control[1]) * 6.0f * tn * t
       + (m_control[3] - m_control[2]) * 3.0f * t2;
}

template <> inline constexpr std::array<Vector, 7> Bezier<3>::interpolate(Scalar t) const
{
  const QVector3D p01   = lerp(m_control[0], m_control[1], t),
                  p12   = lerp(m_control[1], m_control[2], t),
                  p23   = lerp(m_control[2], m_control[3], t),
                  p012  = lerp(p01, p12, t),
                  p123  = lerp(p12, p23, t),
                  p0123 = lerp(p012, p123, t);
  return {m_control[0], p01, p012, p0123, p123, p23, m_control[3]};
}

// Implementation for quadratic bezier curves

template <> inline constexpr Vector Bezier<2>::at(Scalar t) const
{
  const Scalar t2  = t * t,
               tn  = 1.0f - t,
               tn2 = tn * tn;
  return m_control[0] * tn2
       + m_control[1] * 2.0f * tn * t
       + m_control[2] * t2;
}

template <> inline constexpr Vector Bezier<2>::tangent(Scalar t) const
{
  const Scalar tn  = 1.0f - t;
  return (m_control[1] - m_control[0]) * 2.0f * tn
       + (m_control[2] - m_control[1]) * 2.0f * t;
}

template <> inline constexpr std::array<Vector, 5> Bezier<2>::interpolate(Scalar t) const
{
  const QVector3D p01   = lerp(m_control[0], m_control[1], t),
                  p12   = lerp(m_control[1], m_control[2], t),
                  p012  = lerp(p01, p12, t);
  return {m_control[0], p01, p012, p12, m_control[2]};
}

} // namespace Math

#endif // MCRL2_LTSGRAPH_BEZIER_H
