// Author(s): Bas Ploeger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

class KdNode;

class KdTree
{
  public:
    KdTree(KdNode* r);
    ~KdTree();

    void deleteNode(KdNode* node);
    KdNode* findNearestNode(const std::vector<float>& reference_point);
    KdNode* findFarthestNode(const std::vector<float>& reference_point);

  private:
    KdNode* root;

    void clear();
};

class KdTreeBuilder
{
  public:
    KdTreeBuilder(int dims);
    ~KdTreeBuilder;
    void addPoint(std::vector<float>* point);
    KdTree build();

  private:
    int dimensions;
    std::vector< std::vector<float>* > points;

    KdNode* buildSub(int dimension, int begin_index, int end_index);
    void putCorrectPointAtIndex(int target_index, int begin_index, int
        end_index, int dimension);
};

#endif
