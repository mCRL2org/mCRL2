// Author(s): Ruben Vink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file layoututility.cpp
  @author R. Vink

  This file contains implementation of functions for classes facilitating layout algorithms through data structures

*/

#include "layoututility.h"

float octreeNorm(const QVector3D& vec) { return vec.lengthSquared(); }
float octreeWidth(const QVector3D& vec) { return vec.lengthSquared()*0.333333333333333; }
bool octreeEqual(const QVector3D& u, const QVector3D& v) { return (u-v).lengthSquared() < FLT_EPSILON ;}

int octreeChildIndex(const QVector3D& rel_pos){
    return ((rel_pos.z() > 0.5) << 2) |
           ((rel_pos.y() > 0.5) << 1) |
            (rel_pos.x() > 0.5);
}


void octreeUpdateMinbound(const QVector3D& rel_pos, const QVector3D& extents, QVector3D& minbound){
    if (rel_pos.x() > 0.5) minbound.setX(minbound.x() + extents.x());
    if (rel_pos.y() > 0.5) minbound.setY(minbound.y() + extents.y());
    if (rel_pos.z() > 0.5) minbound.setZ(minbound.z() + extents.z());
}

float quadtreeNorm(const QVector2D& vec) { return vec.lengthSquared(); }
float quadtreeWidth(const QVector2D& vec) { return vec.lengthSquared() * 0.5f; }
