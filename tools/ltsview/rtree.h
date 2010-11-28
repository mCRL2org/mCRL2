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
      neighbour_found(false)
    { }

    RTree(RNode* root_):
      root(root_),
      neighbour_found(false)
    { }

    ~RTree();
    void deletePoint(const Vector2D& point);
    void findNearestNeighbour(const Vector2D& point);
    void findFarthestNeighbour(const Vector2D& point);

    bool hasFoundNeighbour() const
    {
      return neighbour_found;
    }

    Vector2D foundNeighbour() const
    {
      return neighbour;
    }

  private:
    RNode* root;
    bool neighbour_found;
    Vector2D neighbour;
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

    void buildRTree();

    RTree* getRTree() const
    {
      return rtree;
    }

  private:
    std::vector< Vector2D > points;
    RTree* rtree;
    // The maximum number of children for a node in the RTree
    static const unsigned int MAX_FANOUT;
};

#endif
