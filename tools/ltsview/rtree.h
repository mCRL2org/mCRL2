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

class RNode;

class RTree
{
  public:
    RTree(RNode* root_): root(root_) {}
    ~RTree();
    void deleteNode(RNode* node);
    RNode* findNearestNeighbour(const std::vector< float >& point);
    RNode* findFarthestNeighbour(const std::vector< float >& point);

  private:
    RNode* root;
};


class PackedRTreeBuilder
{
  public:
    PackedRTreeBuilder();
    ~PackedRTreeBuilder();
    void addPoint(std::vector< float >* point) { points.push_back(point); }
    RTree buildTree();

  private:
    std::vector< std::vector< float >* > points;
};

#endif
