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

  This file contains classes facilitating layout algorithms through data
  structures

*/

#ifndef LAYOUTUTILITY_H
#define LAYOUTUTILITY_H

#include <QVector2D>
#include <QVector3D>
#include <vector>
#include <mcrl2/utilities/logger.h>

#include <cmath>

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif

template <class T> struct TreeNode
{
  T pos; // if the node has children, pos contains the sum of all inserted
         // subnode positions
  int offset;
  // children also encodes information about the node
  //    0       : empty node
  //  otherwise : number of children in subtrees
  int children;
};

template <class T> TreeNode<T> emptyTreeNode()
{
  TreeNode<T> node;
  node.children = 0;
  return node;
}

namespace TreeNodeTypes
{
const int EMPTY_NODE = 0;
const int LEAF_NODE = 1;
} // namespace TreeNodeTypes

template <typename T, int num_children, 
          bool (*equal)(const T&, const T&),
          int (*get_child_index)(const T&),
          bool (*criterion)(const TreeNode<T>&, const T&, const T&, const float&),
          void (*update_minbound)(const T&, const T&, T&)>
class GeometricTree
{
  public:
  GeometricTree(float theta, T minbounds, T maxbounds)
      : m_theta(theta), m_minbounds(minbounds), m_maxbounds(maxbounds),
        m_nodes(1)
  {
    m_data = {emptyTreeNode<T>()};
  };

  void insert(const T& node_pos){
    // Tree has to exist
    assert(m_nodes >= 1);

    // call recursive function
    T node_extents = m_maxbounds - m_minbounds;
    T node_minbounds(m_minbounds);
    sub_insert(node_pos, 0, node_minbounds, node_extents);
  }

  std::vector<TreeNode<T>*>& getSuperNodes(const T& node_pos){
    m_super_nodes.resize(0);
    T extents = m_maxbounds - m_minbounds;
    sub_supnodes(node_pos, 0, extents);
    return m_super_nodes;
  }

  void calculatePositions(){
    if (!m_sub_positions_calculated) calc_subpos(0);
    m_sub_positions_calculated = true;
  }

  std::vector<TreeNode<T>> m_data;

  void setMinBounds(const T& bounds)
  {
    m_minbounds = bounds;
  }
  void setMaxBounds(const T& bounds)
  {
    m_maxbounds = bounds;
  }
  void setTheta(const float theta)
  {
    m_theta = theta;
  }
  void reset()
  {
    m_sub_positions_calculated = false;
    m_nodes = 1;
    m_data[0].children = 0;
  }

  std::size_t size()
  {
    return m_nodes;
  };

  private:
  void sub_insert(const T& node_pos, int i, T& node_minbound, T& node_extents)
  {
    // Index has to be within m_data range
    assert(i >= 0 && i < m_nodes);

    if (m_data[i].children == TreeNodeTypes::EMPTY_NODE)
    {
      // we can insert here
      m_data[i].pos = node_pos;
      m_data[i].children = 1;
      return; // node has been inserted
    }
    else if (m_data[i].children == TreeNodeTypes::LEAF_NODE)
    {
      if (equal(node_pos, m_data[i].pos))
      {
        // trying to insert position that is already in tree
        return;
      }
      // we need to subdivide the current node
      m_data[i].offset = m_nodes - i;
      add_children();

      // now we have subdivided we need to move the current point down
      T relative_pos = (m_data[i].pos - node_minbound) / node_extents;
      
      int child_index = i + m_data[i].offset + get_child_index(relative_pos);

      // Move node information down
      m_data[child_index].pos = m_data[i].pos;
      m_data[child_index].children = TreeNodeTypes::LEAF_NODE;
    }

    // if we make it this far we know for sure that the node has not been
    // inserted so we need to recurse

    // first we add the node to-be-inserted to the cumulative position and
    // increment the number of children
    m_data[i].pos += node_pos;
    m_data[i].children += 1;

    // then we find what child the to-be-inserted node has to go in
    T relative_pos = (node_pos - node_minbound) / node_extents;
    int child_index = i + m_data[i].offset + get_child_index(relative_pos);
    node_extents *= 0.5;
    update_minbound(relative_pos, node_extents, node_minbound);
    // recurse
    sub_insert(node_pos, child_index, node_minbound, node_extents);
  }
  
  void sub_supnodes(const T& node_pos, int i, T& node_extents){
    // we should never recurse on an empty subtree
    assert(m_data[i].children != TreeNodeTypes::EMPTY_NODE);

    if (m_data[i].children == 1){
        // has to be a supernode
        m_super_nodes.push_back(&m_data[i]);
    }else{
        // we check criterion
        // if satisfied we stop recursing and add to super_nodes
        if (criterion(m_data[i], node_extents, node_pos, m_theta)){
            m_super_nodes.push_back(&m_data[i]);
        }else{
            // iterate over all children and recurse if necessary
            node_extents *= 0.5f;
            for (int j = 0; j < num_children; j++){
                int child_index = i+m_data[i].offset+j; 
                if (m_data[child_index].children > 0){
                    sub_supnodes(node_pos, child_index, node_extents);
                }
            }
            node_extents *= 2;
        }
    }
}
  bool m_sub_positions_calculated = false;
  void calc_subpos(int i){
    if (m_data[i].children <= 1) return;
    int base = i + m_data[i].offset;
    double recip = 1.0/m_data[i].children;
    m_data[i].pos = T();
    for (int child = 0; child < num_children; child++){
      calc_subpos(base + child);
      m_data[i].pos += (m_data[base+child].children * recip) * m_data[base+child].pos;
    }
  }

  void add_children()
  {
    if (m_data.size() < m_nodes + num_children)
    {
      for (int i = 0; i < num_children; i++)
        m_data.emplace_back(emptyTreeNode<T>());
    }
    else
    {
      for (int i = 0; i < num_children; i++)
        m_data[m_nodes + i].children = 0;
    }
    m_nodes += num_children;
  }

  T m_minbounds;
  T m_maxbounds;
  float m_theta;       ///< Barnes-Hut opening criterion
  std::size_t m_nodes; ///< Number of leaf nodes currently in tree
  std::vector<TreeNode<T>*> m_super_nodes;
};

// This template assumes that node.pos = xS, aka the center of mass of all contained nodes
template < class T, float(*norm)(const T&), float(*width)(const T&) >
bool BarnesHutCriterion(const TreeNode<T>& node, const T& extents, const T& pos, const float& m_theta){
  return width(extents) <= m_theta*m_theta*norm(pos-node.pos);
}

float octreeNorm(const QVector3D&);
float octreeWidth(const QVector3D&);
bool octreeEqual(const QVector3D&, const QVector3D&);
int octreeChildIndex(const QVector3D&);
void octreeUpdateMinbound(const QVector3D&, const QVector3D&, QVector3D&);
using Octree = GeometricTree<QVector3D, 8, octreeEqual, octreeChildIndex, BarnesHutCriterion<QVector3D, octreeNorm, octreeWidth>, octreeUpdateMinbound>;

// float quadtreeNorm(const QVector2D&);
// float quadtreeWidth(const QVector2D&);
// using Quadtree = GeometricTree<QVector2D, 4>;

#endif