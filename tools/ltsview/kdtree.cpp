// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <stdlib.h>
#include "kdtree.h"

struct KdNode
{
  KdNode(std::vector<float>* p, int d):
    point(p), dimension(d), left(NULL), right(NULL), parent(NULL)
  {}

  ~KdNode()
  {
    if (left != NULL)
    {
      delete left;
    }
    if (right != NULL)
    {
      delete right;
    }
    if (point != NULL)
    {
      delete point;
    }
  }

  std::vector<float>* point;
  int dimension;
  KdNode* left;
  KdNode* parent;
  KdNode* right;

  void swapContentsWith(KdNode* n)
  {
    std::vector<float>* point_tmp = point;
    int dimension_tmp = dimension;
    point = n->point;
    dimension = n->dimension;
    n->point = point_tmp;
    n->dimension = dimension_tmp;
  }
};

class NeighbourFinder
{
  public:
    NeighbourFinder(const std::vector<float>& p): 
      reference_point(p), best_node(NULL), best_distance(0.0f)
    {}

    virtual ~NeighbourFinder {};
    virtual KdNode* find(KdNode* node);

  protected:
    std::vector<float> reference_point;
    KdNode* best_node;
    float best_distance;

    float squaredDistanceTo(const std::vector<float>& p);
};

class NearestNeighbourFinder: public NeighbourFinder
{
  public:
    NearestNeighbourFinder(const std::vector<float>& p):
      NeighbourFinder(p)
    {}

    ~NearestNeighbourFinder() {}
    KdNode* find(KdNode* node);
    void findSub(KdNode* node);
};

float NeighbourFinder::squaredDistanceTo(std::vector<float>* p)
{
  float result = 0.0f;
  for (int i = 0; i < reference_point.size(); ++i)
  {
    float diff_i = reference_point[i] - *p[i];
    result += diff_i * diff_i;
  }
  return result;
}

KdNode* NearestNeighbourFinder::find(KdNode* node)
{
  if (node != NULL)
  {
    best_node = node;
    best_distance = squaredDistanceTo(node->point);
  }
  findSub(node);
  return best_node;
}

void NearestNeighbourFinder::findSub(KdNode* node)
{
  if (node == NULL)
  {
    return;
  }

  // check if this node is nearer than the nearest node so far
  float distance = squaredDistanceTo(node->point);
  if (distance < best_distance)
  {
    best_node = node;
    best_distance = distance;
  }

  // orthogonal distance of reference point to this node
  distance = reference_point[node->dimension] - *(node->point)[node->dimension];

  findSub(distance < 0 ? node->left : node->right);

  // if reference point is within range, we have to check the other branch as
  // well
  if (distance < best_distance)
  {
    findSub(distance >= 0 ? node->left : node->right);
  }
}


KdTree::KdTree(KdNode* r):
  root(r)
{ }

KdTree::~KdTree()
{
  clear();
}

void KdTree::clear()
{
  if (root != NULL)
  {
    delete root;
    root = NULL;
  }
}

KdNode* KdTree::findNearestNode(const std::vector<float>& reference_point)
{
  NearestNeighbourFinder nn_finder = NearestNeighbourFinder(reference_point);
  return nn_finder.find(root);
}

void KdTree::deleteNode(KdNode* node)
{
  while (node->left != NULL || node->right != NULL)
  {
    MinimumFinder min_finder = MinimumFinder(node->dimension);
    KdNode* minimum = min_finder.minimum(min_finder.find(node->left),
        min_finder.find(node->right));
    node->swapContentsWith(minimum);
    node = minimum;
  }
  if (node == node->parent->left)
  {
    node->parent->left = NULL;
  }
  else
  {
    node->parent->right = NULL;
  }
  delete node;
}

class MinimumFinder
{
  public:
    MinimumFinder(int dim): dimension(dim) {}
    ~MinimumFinder() {}
    KdNode* minimum(KdNode* node1, KdNode* node2);
    KdNode* find(KdNode* root);
  private:
    int dimension;
};

KdNode* MinimumFinder::minimum(KdNode* node1, KdNode* node2)
{
  if (node1 == NULL)
  {
    return node2;
  }
  if (node2 == NULL)
  {
    return node1;
  }
  if (node1->point[dimension] < node2->point[dimension])
  {
    return node1;
  }
  return node2;
}

KdNode* MinimumFinder::find(KdNode* root)
{
  if (root == NULL)
  {
    return NULL;
  }
  if (root->dimension == dimension)
  {
    if (root->left == NULL)
    {
      return root;
    }
    return find(root->left);
  }
  return minimum(root, minimum(find(root->left), find(root->right)));
}


KdTreeBuilder::KdTreeBuilder(int dims):
  dimensions(dims)
{ }

KdTreeBuilder::~KdTreeBuilder()
{ }

void KdTreeBuilder::addPoint(std::vector<float>* point)
{
  points.push_back(point);
}

KdTree KdTreeBuilder::build()
{
  return KdTree(buildSub(0, 0, points.size() - 1));
}

KdNode* KdTreeBuilder::buildSub(int dimension, int begin_index, int end_index)
{
  if (end_index < begin_index)
  {
    return NULL;
  }
  int median_index = (begin_index + end_index) / 2;
  putCorrectPointAtIndex(median_index, dimension, begin_index, end_index);
  KdNode* node = new KdNode(dimension, points[median_index]);
  dimension = (dimension + 1) % dimensions;
  node->left = buildSub(dimension, begin_index, median_index - 1);
  if (node->left != NULL)
  {
    node->left->parent = node;
  }
  node->right = buildSub(dimension, median_index + 1, end_index);
  if (node->right != NULL)
  {
    node->right->parent = node;
  }
  return node;
}

void KdTreeBuilder::putCorrectPointAtIndex(int target_index, int begin_index, int
    end_index, int dimension)
{
  while (true)
  {
    int pivot_index = static_cast<int>( static_cast<double>(rand()) / RAND_MAX *
        (end_index - begin_index) ) + begin_index;
    float pivot_value = (*(points[pivot_index]))[dimension];
    swapPoints(pivot_index, end_index);
    int store_index = begin_index;
    for (int i = begin_index; i < end_index; ++i)
    {
      if (*(points[i])[dimension] < pivot_value)
      {
        swapPoints(i, store_index);
        ++store_index;
      }
    }
    swapPoints(store_index, end_index);
    if (store_index == target_index)
    {
      return;
    }
    else if (store_index > target_index)
    {
      end_index = store_index - 1;
    }
    else
    {
      begin_index = store_index + 1;
    }
  }
}

void KdTreeBuilder::swapPoints(int i, int j)
{
  std::vector<float>* p_i = points[i];
  points[i] = points[j];
  points[j] = p_i;
}
