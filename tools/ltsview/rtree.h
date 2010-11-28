// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef RTREE_H
#define RTREE_H

#include <vector>
#include "vectors.h"

class RNode;

class RTree
{
  public:
    RTree():
      root(NULL),
      farthest_neighbour_found(false),
      nearest_neighbour_found(false)
    { }

    RTree(RNode* root_):
      root(root_),
      farthest_neighbour_found(false),
      nearest_neighbour_found(false)
    { }

    ~RTree();
    void deletePoint(const Vector2D& point);
    void findNearestNeighbour(const Vector2D& point);
    void findFarthestNeighbour(const Vector2D& point);

    bool farthestNeighbourFound() const
    {
      return farthest_neighbour_found;
    }

    Vector2D farthestNeighbour() const
    {
      return farthest_neighbour;
    }

    bool nearestNeighbourFound() const
    {
      return nearest_neighbour_found;
    }

    Vector2D nearestNeighbour() const
    {
      return nearest_neighbour;
    }

  private:
    RNode* root;
    bool farthest_neighbour_found;
    Vector2D farthest_neighbour;
    bool nearest_neighbour_found;
    Vector2D nearest_neighbour;
};


class PackedRTreeBuilder
{
  public:
    PackedRTreeBuilder():
      rtree(NULL)
    { }

    ~PackedRTreeBuilder()
    { }

    void addPoint(const Vector2D& point)
    {
      points.push_back(point);
    }

    RTree* buildRTree();

  private:
    std::vector< Vector2D > points;
    RTree* rtree;
};

#endif
