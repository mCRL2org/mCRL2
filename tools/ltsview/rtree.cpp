// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <limits>
#include <priority_queue>
#include <vector>
#include "rtree.h"

class Rectangle
{
  public:
    Rectangle(const Vector2D& lowc, const Vector2D& highc):
      low_corner(lowc), high_corner(highc)
    { }

    bool contains(const Vector2D& point) const;

    float minDistanceTo(const Vector2D& point) const;

    // We assume the following invariant for any rectangle:
    // low_corner.x() <= high_corner.x() && low_corner.y() <= high_corner.y()
    Vector2D low_corner;
    Vector2D high_corner;
};

bool Rectangle::contains(const Vector2D& point) const
{
  return point.x() >= low_corner.x() && high_corner.x() >= point.x() &&
    point.y() >= low_corner.y() && high_corner.y() >= point.y();
}

float Rectangle::minDistanceTo(const Vector2D& point) const
{
  if (this->contains(point))
  {
    return 0.0f;
  }
  Vector2D diff_low = low_corner - point;
  Vector2D diff_high = high_corner - point;
  return std::min(diff_low.x() * diff_low.x(), diff_high.x() * diff_high.x()) +
    std::min(diff_low.y() * diff_low.y(), diff_high.y() * diff_high.y());
}


class RNode
{
  public:
    RNode()
    { }

    virtual ~RNode()
    { }

    Rectangle boundingBox() const
    {
      return bounding_box;
    }

    float centerX() const
    {
      return bounding_box.low_corner.x() + 0.5f * (bounding_box.high_corner.x()
          - bounding_box.low_corner.x());
    }

    float centerY() const
    {
      return bounding_box.low_corner.y() + 0.5f * (bounding_box.high_corner.y()
          - bounding_box.low_corner.y());
    }

    virtual void computeBoundingBox();

    virtual void deletePoint(const Vector2D& point);

    virtual bool hasChildren() const;

  private:
    Rectangle bounding_box;

    static float MIN_BB_SIDE_LENGTH = 1e-6f;

    static void ensureMinBBSize(float& lc_x, float& lc_y, float& hc_x,
        float& hc_y);
};

void RNode::ensureMinBBSize(float& lc_x, float& lc_y, float& hc_x, float& hc_y)
{
  float diff_x = hc_x - lc_x;
  if (diff_x < MIN_BB_SIDE_LENGTH)
  {
    hc_x += 0.5f * (MIN_BB_SIDE_LENGTH - diff_x)
    lc_x -= 0.5f * (MIN_BB_SIDE_LENGTH - diff_x)
  }
  float diff_y = hc_y - lc_y;
  if (diff_y < MIN_BB_SIDE_LENGTH)
  {
    hc_y += 0.5f * (MIN_BB_SIDE_LENGTH - diff_y)
    lc_y -= 0.5f * (MIN_BB_SIDE_LENGTH - diff_y)
  }
}


class RTreeNode: public RNode
{
  public:
    RTreeNode() { }

    ~RTreeNode();

    void deletePoint(const Vector2D& point);

    bool hasChildren() const
    {
      return !children.empty();
    }

    std::list< RNode* > children;
};

void RTreeNode::computeBoundingBox()
{
  float lc_x = numeric_limits< float >::max();
  float lc_y = numeric_limits< float >::max();
  float hc_x = numeric_limits< float >::min();
  float hc_y = numeric_limits< float >::min();
  std::list< RNode* >::iterator ci;
  for (ci = children.begin(); ci != children.end(); ++ci)
  {
    Rectangle child_bb = (**ci).boundingBox();
    lc_x = std::min(lc_x, child_bb.low_corner.x());
    lc_y = std::min(lc_y, child_bb.low_corner.y());
    hc_x = std::max(hc_x, child_bb.high_corner.x());
    hc_y = std::max(hc_y, child_bb.high_corner.y());
  }
  ensureMinBBSize(lc_x, lc_y, hc_x, hc_y);
  bounding_box = Rectangle(Vector2D(lc_x, lc_y), Vector2D(hc_x, hc_y));
}

void RTreeNode::deletePoint(const Vector2D& point)
{
  std::list< RNode* >::iterator ci;
  for (ci = children.begin(); ci != children.end(); ++ci)
  {
    RNode* child = *ci;
    if (child->boundingBox().contains(point))
    {
      child->deletePoint(point);
      if (child->hasChildren())
      {
        child->computeBoundingBox();
      }
      else
      {
        delete ci->child;
        children.remove(ci);
      }
    }
  }
}


class RTreeLeaf: public RNode
{
  public:
    RTreeLeaf(const Vector2D& p):
      point(p)
    { }

    ~RTreeLeaf()
    { }

    bool hasChildren() const
    {
      return false;
    }

    void deletePoint(const Vector2D& p)
    { }

  private:
    Vector2D point;
};

void RTreeLeaf::computeBoundingBox()
{
  float lc_x = point.x();
  float lc_y = point.y();
  float hc_x = point.x();
  float hc_y = point.y();
  ensureMinBBSize(lc_x, lc_y, hc_x, hc_y);
  bounding_box = Rectangle(Vector2D(lc_x, lc_y), Vector2D(hc_x, hc_y));
}


void RTree::deletePoint(const Vector2D& point)
{
  root->deletePoint(point);
  if (!root->hasChildren())
  {
    delete root;
    root = NULL;
  }
}

class QueueElement
{
  public:
    QueueElement(RNode* n, float d):
      node(n), distance(d)
    { }

    bool operator < (const QueueElement& q) const
    {
      return distance < q.distance;
    }

    RNode* node;
    float distance;
};

Vector2D* RTree::nearestNeighbour(const Vector2D& point)
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
      return leaf->point;
    }
    RTreeNode* node = dynamic_cast< RTreeNode* > (top_node);
    std::list< RNode* >::iterator ci;
    for (ci = node->children.begin(); ci != node->children.end(); ++ci)
    {
      RNode* child = *ci;
      queue.push(QueueElement(child, child->boundingBox().minDistanceTo(point)))
    }
  }
  return NULL;
}

bool compareCenterX(RNode* a, RNode* b) const
{
  // See cluster.cpp for an explanation on the a != b clause.
  return a != b && a->centerX() < b->centerX();
}

bool compareCenterY(RNode* a, RNode* b) const
{
  // See cluster.cpp for an explanation on the a != b clause.
  return a != b && a->centerY() < b->centerY();
}

RTree PackedRTreeBuilder::buildTree()
{
  // This algorithm for constructing a packed R-tree from a set of spatial
  // objects (in our case just points), is called Sort-Tile-Recursive and is
  // described in:
  // "STR: a simple and efficient algorithm for R-tree packing", S.T.
  // Leutenegger, M.A. Lopez and J. Edgington, Technical Report TR-97-14,
  // Institute for Computer Applications in Science and Engineering, 1997.
  std::vector< RNode* > roots;
  std::vector< Vector2D >::iterator pi;
  for (pi = points.begin(); pi != points.end(); ++pi)
  {
    RTreeLeaf* leaf = new RTreeLeaf(*i);
    leaf->computeBoundingBox();
    roots.push_back(leaf);
  }
  while (roots.size() > 1)
  {
    std::vector< RNode* > new_roots;
    int slice_size = max_fanout * std::ceil( std::sqrt( std::ceil(
            static_cast< double >(roots.size()) /
            static_cast< double >(max_fanout))));
    sort(roots.begin(), roots.end(), compareCenterX);
    std::vector< RNode* >::iterator slice_begin;
    for (slice_begin = roots.begin(); slice_begin < roots.end();
        slice_begin += slice_size)
    {
      std::vector< RNode* >::iterator slice_end = slice_begin + slice_size;
      if (slice_end > roots.end())
      {
        slice_end = roots.end();
      }
      sort(slice_begin, slice_end, compareCenterY);
      std::vector< RNode* >::iterator children_begin;
      for (children_begin = slice_begin; children_begin < slice_end;
          children_begin += max_fanout)
      {
        std::vector< RNode* >::iterator children_end = slice_begin + max_fanout;
        if (children_end > slice_end)
        {
          children_end = slice_end;
        }
        RTreeNode* node = new RTreeNode();
        node->children.assign(children_begin, children_end);
        node->computeBoundingBox();
        new_roots.push_back(node);
      }
    }
    roots.swap(new_roots);
  }
  return RTree(roots[0]);
}
