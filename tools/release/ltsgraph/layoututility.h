// Author(s): Ruben Vink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file layoututility.h
  @author R. Vink

  This file contains classes facilitating layout algorithms through data structures

*/

#ifndef LAYOUTUTILITY_H
#define LAYOUTUTILITY_H

#include <QVector2D>
#include <QVector3D>
#include <vector>
#include <mcrl2/utilities/logger.h>

template< class T >
struct TreeNode{
    T pos; // if the node has children, pos contains the sum of all inserted subnode positions
    int offset;
    // children also encodes information about the node
    //   -1       : empty node
    //    0       : leaf  node
    //  otherwise : number of children in subtrees
    int children;
};

namespace TreeNodeTypes{
    const int EMPTY_NODE = -1;
    const int LEAF_NODE = 0;
}

template< class T, int num_children >
class GeometricTree{
  public:
    GeometricTree(float theta, T minbounds, T maxbounds);

    void insert(const T& node_pos);

    std::vector<std::pair<int, T>> getSuperNodes(const T& node_pos);

    std::vector<TreeNode<T>> m_data;

    void setMinBounds(const T& bounds){ m_minbounds = bounds; }
    void setMaxBounds(const T& bounds){ m_maxbounds = bounds; }
    void setTheta(const float theta){ m_theta = theta; }
    void reset();
  private:
    void sub_insert(const T& node_pos, int i, T& node_minbound, T& node_extents);
    void sub_supnodes(const T& node_pos, int i, T& node_minbound, T& node_extents, std::vector<std::pair<int, T>>& super_nodes);
    void calc_subpos(int i);
    void add_children();
    T m_minbounds;
    T m_maxbounds;
    float m_theta; ///< Barnes-Hut opening criterion
    int m_nodes;   ///< Number of leaf nodes currently in tree
};

using Octree = GeometricTree<QVector3D, 8>;
using Quadtree = GeometricTree<QVector2D, 4>;

#endif