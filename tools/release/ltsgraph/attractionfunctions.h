// Author: Ruben Vink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file attractionfunctions.h


#ifndef ATTRACTIONFUNCTIONS_H
#define ATTRACTIONFUNCTIONS_H

#include <layoutfunctions.h>
/**
 * @brief An enumeration that identifies the types of spring types which can
 * be selected.
 */
enum AttractionFunctionID
{
  ltsgraph_attr,          ///< LTSGraph implementation.
  electricalsprings_attr, ///< LTSGraph implementation using approximated
                          ///< repulsion forces.
  linearsprings_attr,     ///< Linear spring implementation.
                            // simplespring_attr,
};

namespace AttractionFunctions 
{
  struct LTSGraph : AttractionFunction
  {
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float ideal) override
    {
      QVector3D diff=(a-b);
      float factor = 2-ideal/diff.length();
      return std::max(-1000.0f,factor)*diff;
    }
  };

  struct LinearSprings : AttractionFunction
  {
    const float spring_constant = 1e-4f;
    const float scaling = 1.0f / 10000;
    QVector3D diff = { 0, 0, 0 };
    float dist = 0.0f;
    float factor = 0.0f;
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float ideal) override
    {
      diff = (a - b);
      dist = diff.length() - ideal;
      factor = spring_constant * std::max(dist, 0.0f);
      if (dist > 0.0f)
      {
          factor = std::max(factor, 100 / (std::max)(dist * dist / 10000.0f, 0.1f));
      }
      return diff * (factor * scaling);
    }
  };

  struct ElectricalSprings : AttractionFunction
  {
    QVector3D diff = { 0, 0, 0 };
    const float scaling = 1e-2f;
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float ideal) override
    {
        diff = (a - b);
        return (scaling * diff.length() / std::max(0.0001f, ideal)) * diff;
    }
  };

  struct SimpleSpring : AttractionFunction
  {
    QVector3D diff = { 0, 0, 0 };
    const float spring_constant = 1e-4f;
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float ideal) override
    {
        diff = a - b;
        return spring_constant * (diff.length() - ideal) * diff;
    }
  };
}; // namespace AttractionFunctions

#endif
