// Author: Ruben Vink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file repulsionfunctions.h

#ifndef REPULSIONFUNCTIONS_H
#define REPULSIONFUNCTIONS_H

#include <layoutfunctions.h>


enum RepulsionFunctionID
{
  ltsgraph_rep,
  electricalsprings_rep,
  none_rep,
};


namespace RepulsionFunctions
{
  struct LTSGraph : RepulsionFunction
  {
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float natlength) override
    {
      const QVector3D ZERO = { 0, 0, 0 };
      QVector3D diff = a - b;
      if (diff==ZERO)
      {
// std::cerr << "R";
        return QVector3D{frand(-1e-2f,1e-2f),frand(-1e-2f,1e-2f),0};
      }
      /* float r = cube(natlength);
      r /= cube(std::max(diff.length() * 0.5f, natlength * 0.1f)); */
      float r = std::max(0.0f,std::min(1000.0f,natlength/diff.lengthSquared()));
      // r /= cube(std::max(diff.length() * 0.5f, natlength * 0.1f));
      // This returned result was randomized by adding a variation of [-0,1,0,1] to the three coordinates.
      // This has been removed in March 2025. It may have served a purpose. 
// std::cerr << "REPULS " << r << "    " << diff.length() * 0.5f << "     " << natlength * 0.1f << "\n";
      return diff * r; 
    }
  };

  struct ElectricalSpring : RepulsionFunction
  {
    QVector3D operator()(const QVector3D& a, 
                         const QVector3D& b,
                         const float K) override
    {
      const QVector3D ZERO = { 0, 0, 0 };
      QVector3D diff = a - b;
      if (diff==ZERO)
      {
        return QVector3D{frand(-1e-2f,1e-2f),frand(-1e-2f,1e-2f),0};
      }
      const float scaling = 1e-2f;
      return ((scaling * K * K) /
            std::max(diff.lengthSquared(), 0.00001f)) * diff;
    }
  };

  struct None : RepulsionFunction
  {
    QVector3D operator()(const QVector3D&, const QVector3D&, const float) override
    {
      const QVector3D ZERO = { 0, 0, 0 };
      return ZERO;
    }
  };
}; // namespace RepulsionFunctions


#endif
