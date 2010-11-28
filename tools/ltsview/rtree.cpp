// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "rtree.h"

class Hypercube
{
  public:
    std::vector< float > low_corner;
    std::vector< float > high_corner;
    bool containsPoint(const std::vector< float >& point);
};

typedef std::vector< Hypercube >::iterator hypercube_iterator;
typedef std::vector< float >::iterator float_iterator;

class RNode
{
  public:
    RNode();
    virtual ~RNode();
    virtual RNode* findNearestNeighbour(const std::vector< float >& point);
};

class RTreeNode: public RNode
{
  public:
    RTreeNode();
    ~RTreeNode();

    std::vector< Hypercube > regions;
    std::vector< RNode* > children;
};

class RTreeLeaf: public RNode
{
  public:
    RTreeLeaf();
    ~RTreeLeaf();

    std::vector< float >* point;
};

bool Hypercube::containsPoint(const std::vector< float >& point)
{
  for (int i = 0; i < point.size(); ++i)
  {
    if (point[i] < low_corner[i] || point[i] > high_corner[i])
    {
      return false;
    }
  }
  return true;
}

RNode* RTree::findNearestNeighbour(const std::vector< float >& point)
{
  return root->findNearestNeighbour(point);
}

RNode* RTreeNode::findNearestNeighbour(const std::vector< float >& point)
{
  for (hypercube_iterator ri = regions.begin(); ri != regions.end(); ++ri)
  {
    if (ri->containsPoint(point))
    {
    }
  }
}
