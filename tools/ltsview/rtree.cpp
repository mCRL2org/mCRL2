// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <priority_queue>
#include <vector>
#include "rtree.h"

class Hypercube
{
  public:
    std::vector< float > low_corner;
    std::vector< float > high_corner;

    float smallestDistanceTo(const std::vector< float >& point) const;
};

float Hypercube::smallestDistanceTo(const std::vector< float >& point) const
{
  bool pointInsideCube = true;
  float distance = 0.0f;
  for (int dim = 0; dim < point.size(); ++dim)
  {
    float low_dist = low_corner[dim] - point[dim];
    float high_dist = point[dim] - high_corner[dim];
    if (low_dist >= 0.0f || high_dist >= 0.0f)
    {
      pointInsideCube = false;
    }
    low_dist *= low_dist;
    high_dist *= high_dist;
    distance += std::min(low_dist, high_dist);
  }
  if (pointInsideCube)
  {
    return 0.0f;
  }
  return distance;
}

class RNode
{
  public:
    RNode();
    virtual ~RNode();
};

class RTreeNode: public RNode
{
  public:
    RTreeNode();
    ~RTreeNode();
    std::vector< Hypercube > bounding_boxes;
    std::vector< RNode* > children;
};

class RTreeLeaf: public RNode
{
  public:
    RTreeLeaf(const std::vector< float >* p): point(p) {}
    ~RTreeLeaf();
    std::vector< float >* point;
};

class QueueElement
{
  public:
    QueueElement(RNode* n, float d):
      node(n), distance(d) {}

    bool operator < (const QueueElement& q) const
    {
      return distance < q.distance;
    }

    RNode* node;
    float distance;
};

RNode* RTree::findNearestNeighbour(const std::vector< float >& point)
{
  // This algorithm for finding nearest neighbours in an R-tree using a priority
  // queue is described in:
  // "Distance Browsing in Spatial Databases", G.R. Hjaltason and H. Samet, ACM
  // Transactions on Database Systems, 24:2, pp. 265-318, 1999.
  std::priority_queue< QueueElement > queue;
  queue.push(QueueElement(root, 0.0f));
  while (!queue.empty())
  {
    RNode* top_node = queue.top().node;
    queue.pop();
    RTreeLeaf* leaf = dynamic_cast< RTreeLeaf* > (top_node);
    if (leaf != NULL)
    {
      return leaf;
    }
    RTreeNode* node = dynamic_cast< RTreeNode* > (top_node);
    for (int i = 0; i < node->children.size(); ++i)
    {
      queue.push(QueueElement(node->children[i],
            node->bounding_boxes[i].smallestDistanceTo(point)))
    }
  }
}

RTree PackedRTreeBuilder::buildTree()
{
  // This algorithm for constructing a packed R-tree from a set of spatial
  // objects (in our case just points), is called Sort-Tile-Recursive and is
  // described in:
  // "STR: a simple and efficient algorithm for R-tree packing", S.T.
  // Leutenegger, M.A. Lopez and J. Edgington, Technical Report TR-97-14,
  // Institute for Computer Applications in Science and Engineering, 1997.
  
}
