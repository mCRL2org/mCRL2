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
#include "vectors"

class RNode;

class RTree
{
  public:
    RTree(RNode* root_):
      root(root_)
    { }

    ~RTree();
    void deletePoint(const Vector2D& point);
    Vector2D nearestNeighbour(const Vector2D& point);
    Vector2D farthestNeighbour(const Vector2D& point);

  private:
    RNode* root;
};


class PackedRTreeBuilder
{
  public:
    PackedRTreeBuilder(unsigned int fanout):
      max_fanout(fanout)
    { }

    ~PackedRTreeBuilder()
    { }

    void addPoint(const Vector2D& point)
    {
      points.push_back(point);
    }

    RTree buildTree();

  private:
    std::vector< Vector2D > points;
    unsigned int max_fanout;
};

#endif
