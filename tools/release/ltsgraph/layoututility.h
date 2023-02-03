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

#include "utility.h"

#include <QVector2D>
#include <QVector3D>
#include <assert.h>
#include <vector>
#include <mcrl2/utilities/logger.h>
#include <graph.h>

#include <cmath>

#define MAX_DEPTH 20

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif


//
// Utility functions
//

inline float cube(float x)
{
  return x * x * x;
}

inline float smoothstep(float l, float r, float x)
{
  if (x < l)
    return 0;
  if (x > r)
    return 1;

  x = (x - l) / (r - l);

  return x * x * (3 - 2 * x);
}

inline void clip(float& f, float min, float max)
{
  if (f < min)
  {
    f = min;
  }
  else if (f > max)
  {
    f = max;
  }
}

inline float lerp(int value, float targ_min, float targ_max, int _min = 0,
           int _max = 100)
{
  return targ_min +
         (targ_max - targ_min) * (value - _min) / (float)(_max - _min);
}

inline int unlerp(float value, float val_min, float val_max, int targ_min = 0,
           int targ_max = 100)
{
  return targ_min +
         (targ_max - targ_min) * (value - val_min) / (val_max - val_min);
}


const std::size_t max_slice = 50;
/// @brief Takes average of at most @c max_slice values, or recursively computes
/// average by splitting
/// @param i Start index
/// @param j End index (exclusive)
/// This method avoids precision loss due to adding up too much before division
///  Downside: more divisions. Upside: more accuracy.
template < typename ReturnType >
ReturnType _slicedAverage(std::size_t i, std::size_t j,
                          std::function<ReturnType(std::size_t)> f, ReturnType zero)
{
  std::size_t n = j - i;
  if (n > max_slice)
  {
    // split
    std::size_t m = i + n / 2;
    double recip = 1.0 / n;
    return (m - i) * recip * _slicedAverage(i, m, f, zero) +
           (j - m) * recip * _slicedAverage(m, j, f, zero);
  }
  else
  {
    ReturnType accum = zero;
    for (std::size_t k = i; k < j; ++k)
    {
      accum += f(k);
    }
    return accum / (j-i);
  }
}

QVector3D slicedAverage(Graph::Graph& graph);
float slicedAverageSqrMagnitude(std::vector<QVector3D>& forces);

///
/// Tree specifications below
///

template <class T> struct TreeNode
{
  T pos; // if the node has children, pos contains the sum of all inserted
         // subnode positions
  std::size_t offset;
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

template <typename T, int num_children, bool (*equal)(const T&, const T&),
          std::size_t (*get_child_index)(const T&),
          bool (*criterion)(const TreeNode<T>&, const T&, const T&,
                            const float&),
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

  typedef T data_t;
  /**
   * @brief Interface function for inserting position into tree
   *
   * @param node_pos Position to be inserted
   */
  /// TODO: Consider returning boolean on success/fail
  void insert(const T& node_pos)
  {
    // Tree has to exist
    assert(this->m_nodes >= 1);
    assert(!this->m_sub_positions_calculated);

    // call 'recursive' function (implemented iteratively)
    T node_extents = m_maxbounds - m_minbounds;
    T node_minbounds(m_minbounds);
    sub_insert(node_pos, 0, node_minbounds, node_extents);
  }

  /**
   * @brief Fills \property m_super_nodes with nodes relevant to \p node_pos .
   * \property criterion and \property m_theta are used to determine relevancy.
   *
   * @param node_pos Position of node to which super nodes are relative.
   * @return std::vector<TreeNode<T>*>& Reference to \property m_super_nodes.
   *
   * @pre Super node weighted averages have been computed throughout the entire
   * tree.
   */
  /// TODO: Complete asserts i.e. bounds checking
  std::vector<TreeNode<T>*>& getSuperNodes(const T& node_pos)
  {
    assert(this->m_sub_positions_calculated);
    m_super_nodes.resize(0);
    T extents = m_maxbounds - m_minbounds;
    sub_supnodes(node_pos, 0, extents);
    return m_super_nodes;
  }

  /**
   * @brief Completes the insertion phase of the building process. Computes all
   * weighted positions of nodes.
   *
   * @pre Non-leaf node positions are correct if and only if \property
   * m_sub_position_calculated = true
   */
  void calculatePositions()
  {
    if (!m_sub_positions_calculated)
      calc_subpos(0);
    m_sub_positions_calculated = true;
  }

  /// @brief Setter for @property m_minbounds
  /// TODO: Invalidate tree.
  void setMinBounds(const T& bounds)
  {
    m_minbounds = bounds;
  }

  /// @brief Setter for @property m_maxbounds
  /// TODO: Invalidate tree.
  void setMaxBounds(const T& bounds)
  {
    m_maxbounds = bounds;
  }

  /// @brief Setter for @property m_theta
  /// TODO: Consider whether invalidates tree or not. Assumed not.
  void setTheta(const float theta)
  {
    m_theta = theta;
  }

  /// @brief Resets tree. Memory is lazily reused by resetting root node.
  /// TODO: Consider shrinking data vector if unnecessarily large. Assumed
  /// irrelevant.
  void reset()
  {
    m_sub_positions_calculated = false;
    m_nodes = 1;
    m_data[0].children = 0;
  }

  /// @brief Returns number of nodes currently in tree. Includes empty, leaf,
  /// and parent nodes.
  std::size_t size()
  {
    return m_nodes;
  };

  private:
  bool m_sub_positions_calculated =
      false; ///< Indicates whether all non-leaf nodes have correctly calculated
             ///< average position of children

  std::vector<TreeNode<T>> m_data; ///< Contains raw tree data

  float m_theta;       ///< Barnes-Hut opening criterion
  T m_minbounds;       ///< Bounding volume of entire tree base
  T m_maxbounds;       ///< Bounding volume of entire tree other extreme
  std::size_t m_nodes; ///< Number of leaf nodes currently in tree

  std::vector<TreeNode<T>*>
      m_super_nodes; ///< Used to accumulate all super nodes during query
                     ///< \property getSuperNodes

  /**
   * @brief Subroutine for recursively inserting a position.
   *
   * @param node_pos Position to be inserted
   * @param i Index of current node in recursion
   * @param node_minbound Bounding area/volume origin
   * @param node_extents Extents of bounding area/volume
   */
  void sub_insert(const T& node_pos, std::size_t i, T& node_minbound,
                  T& node_extents)
  {
    // Index has to be within m_data range
    assert(i >= 0 && i < this->m_nodes);
    std::size_t depth = 0;
    while (depth++ < MAX_DEPTH)
    {
      assert(depth < 100);
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

        std::size_t child_index =
            i + m_data[i].offset + get_child_index(relative_pos);

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
      std::size_t child_index =
          i + m_data[i].offset + get_child_index(relative_pos);
      node_extents *= 0.5;
      update_minbound(relative_pos, node_extents, node_minbound);

      i = child_index;
    }
  }

  /**
   * @brief Subroutine for recursively finding sup(er)nodes for a given @p
   * node_pos .
   *
   * @param node_pos Relative to this position we want super nodes.
   * @param i Current index in tree structure.
   * @param node_extents Extents of the current bounding area/volume. Used for
   * opening \property criterion.
   */
  /// TODO: Iterative instead of recursive.
  void sub_supnodes(const T& node_pos, std::size_t i, T& node_extents)
  {
    // we should never recurse on an empty subtree
    assert(this->m_data[i].children != TreeNodeTypes::EMPTY_NODE);

    if (m_data[i].children == 1)
    {
      // has to be a supernode
      m_super_nodes.push_back(&m_data[i]);
    }
    else
    {
      // we check criterion
      // if satisfied we stop recursing and add to super_nodes
      if (criterion(m_data[i], node_extents, node_pos, m_theta))
      {
        m_super_nodes.push_back(&m_data[i]);
      }
      else
      {
        // iterate over all children and recurse if necessary
        node_extents *= 0.5f;
        for (std::size_t j = 0; j < num_children; j++)
        {
          std::size_t child_index = i + m_data[i].offset + j;
          if (m_data[child_index].children > 0)
          {
            sub_supnodes(node_pos, child_index, node_extents);
          }
        }
        node_extents *= 2;
      }
    }
  }

  /**
   * @brief Subroutine for recursively calculating average position of node
   *
   * @param i Index of current node in tree.
   */
  /// TODO: Iterative instead of recursive.
  void calc_subpos(std::size_t i)
  {
    if (m_data[i].children <= 1)
      return;
    std::size_t base = i + m_data[i].offset;
    double recip = 1.0 / m_data[i].children;
    m_data[i].pos = T();
    for (std::size_t child = 0; child < num_children; child++)
    {
      calc_subpos(base + child);
      m_data[i].pos +=
          (m_data[base + child].children * recip) * m_data[base + child].pos;
    }
  }

  /**
   * @brief Adds children to the tree. If tree does not have nodes yet they are
   * created, otherwise they are reset.
   *
   */
  void add_children()
  {
    if (m_data.size() < m_nodes + num_children)
    {
      for (std::size_t i = 0; i < num_children; i++)
        m_data.emplace_back(emptyTreeNode<T>());
    }
    else
    {
      for (std::size_t i = 0; i < num_children; i++)
        m_data[m_nodes + i].children = 0;
    }
    m_nodes += num_children;
  }
};

// This template assumes that node.pos = xS, in other words the center of mass of all
// contained nodes
template <class T, float (*norm)(const T&), float (*sqwidth)(const T&)>
bool BarnesHutCriterion(const TreeNode<T>& node, const T& extents, const T& pos,
                        const float& m_theta)
{
  return sqwidth(extents) <= m_theta * m_theta * norm(pos - node.pos);
}

float octreeNorm(const QVector3D&);
float octreeSqWidth(const QVector3D&);
bool octreeEqual(const QVector3D&, const QVector3D&);
std::size_t octreeChildIndex(const QVector3D&);
void octreeUpdateMinbound(const QVector3D&, const QVector3D&, QVector3D&);
using Octree =
    GeometricTree<QVector3D, 8, octreeEqual, octreeChildIndex,
                  BarnesHutCriterion<QVector3D, octreeNorm, octreeSqWidth>,
                  octreeUpdateMinbound>;

float quadtreeNorm(const QVector2D&);
float quadtreeSqWidth(const QVector2D&);
bool quadtreeEqual(const QVector2D&, const QVector2D&);
std::size_t quadtreeChildIndex(const QVector2D&);
void quadtreeUpdateMinbound(const QVector2D&, const QVector2D&, QVector2D&);
using Quadtree =
    GeometricTree<QVector2D, 4, quadtreeEqual, quadtreeChildIndex,
                  BarnesHutCriterion<QVector2D, quadtreeNorm, quadtreeSqWidth>,
                  quadtreeUpdateMinbound>;


#endif