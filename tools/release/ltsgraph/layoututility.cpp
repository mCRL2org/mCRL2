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
float octreeSqWidth(const QVector3D& vec) { return vec.lengthSquared() * 0.333333333f; }
bool octreeEqual(const QVector3D& u, const QVector3D& v) { return (u-v).lengthSquared() < FLT_EPSILON ;}

float quadtreeNorm(const QVector2D& vec) { return vec.lengthSquared(); }
float quadtreeSqWidth(const QVector2D& vec) { return vec.x()*vec.x(); }
bool quadtreeEqual(const QVector2D& u, const QVector2D& v) { return (u-v).lengthSquared() < FLT_EPSILON ;}

/// @brief Encode x, y, z in least significant bits in order from least to most 
int octreeChildIndex(const QVector3D& rel_pos){
    return ((rel_pos.z() > 0.5f) << 2) |
           ((rel_pos.y() > 0.5f) << 1) |
            (rel_pos.x() > 0.5f);
}

/// @brief Encode x, y in least significant bits in order from least to most
int quadtreeChildIndex(const QVector2D& rel_pos){
    return ((rel_pos.y() > 0.5f) << 1) |
            (rel_pos.x() > 0.5f);
}

/// @brief When recursing down the tree updates the bounds appropriately
void octreeUpdateMinbound(const QVector3D& rel_pos, const QVector3D& extents, QVector3D& minbound){
    if (rel_pos.x() > 0.5) minbound.setX(minbound.x() + extents.x());
    if (rel_pos.y() > 0.5) minbound.setY(minbound.y() + extents.y());
    if (rel_pos.z() > 0.5) minbound.setZ(minbound.z() + extents.z());
}

void quadtreeUpdateMinbound(const QVector2D& rel_pos, const QVector2D& extents, QVector2D& minbound){
    if (rel_pos.x() > 0.5) minbound.setX(minbound.x() + extents.x());
    if (rel_pos.y() > 0.5) minbound.setY(minbound.y() + extents.y());
}
