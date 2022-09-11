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
#include <stack>
#include <iterator>

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

template< class T, int num_children, float(*norm)(const T&), float(*width)(const T&) >
struct TreeIterator{
    using iterator_category = std::forward_iterator_tag;
    using value_type = TreeNode<T>;
    using pointer = TreeNode<T>*;
    using reference = TreeNode<T>&; 
    using _Iterator_t = TreeIterator<T, num_children, norm, width>; 

    TreeIterator(pointer ptr, const T& pos, T node_extents, float theta) 
      : m_ptr(ptr), m_pos(pos), m_node_extents(node_extents), m_theta(theta)
    {
      traverse_stack = std::stack<int>({0});
      child_index_stack = std::stack<char>({0});
      if(m_ptr) ++(*this);
    }

    reference operator*() const {return *m_ptr; }
    pointer operator->() {return m_ptr; }
    
    /// @brief Prefix increment
    _Iterator_t& operator++() 
    {
      while (!traverse_stack.empty()){
          while (!child_index_stack.empty() && child_index_stack.top() == num_children){
              // move up the tree
              m_ptr -= traverse_stack.top();
              traverse_stack.pop();
              child_index_stack.pop();
              m_node_extents *= 2;
          }
          if (traverse_stack.empty())
            break;
          int n = traverse_stack.size();
          assert(m_ptr->children > -1);
          if (m_ptr->children == 0){
              child_index_stack.top() = num_children; // marked for going back up next time
              return *this; // found leaf node, cant go further down so it must be a super node
          }else{
              T xS = m_ptr->pos / m_ptr->children;

              //Barnes-Hut criterion
              if (width(m_node_extents) <= m_theta*m_theta*norm(m_pos - xS)){
                child_index_stack.top() = num_children; // mark for going back up next time
                return *this; // this is a supernode since the Barnes Hut citerion holds
              }else{
                  // iterate over all children
                  m_node_extents *= 0.5f;
                  while(child_index_stack.top() < num_children){
                      int child_offset = m_ptr->offset + child_index_stack.top();
                      child_index_stack.top()++;
                      if ((m_ptr + child_offset)->children >= 0){
                        // move down the tree
                        traverse_stack.push(child_offset);
                        child_index_stack.push(0);
                        m_ptr += child_offset;
                        return ++(*this); // recursion is not done yet
                      }
                  } // if this loop finishes; child_index_stack.top() == num_children is true and we are marked to move back up
              }
          }
      }
      m_ptr = nullptr;
      return *this;
    }
    friend bool operator== (const TreeIterator& a, const TreeIterator& b) { return a.m_ptr == b.m_ptr; };
    friend bool operator!= (const TreeIterator& a, const TreeIterator& b) { return a.m_ptr != b.m_ptr; };

    private:
        pointer m_ptr;
        T dvec;
        const T& m_pos;
        T m_node_extents;
        float m_theta;
        std::stack<int> traverse_stack;
        std::stack<char> child_index_stack;
};

template< class T, int num_children, class _Iterator >
class GeometricTree{
  public:
    GeometricTree(float theta, T minbounds, T maxbounds);

    void insert(const T& node_pos);

    std::vector<std::pair<int, T>> getSuperNodes(const T& node_pos);

    std::vector<TreeNode<T>> m_data;

    void setMinBounds(const T& bounds){ m_minbounds = bounds; }
    void setMaxBounds(const T& bounds){ m_maxbounds = bounds; }
    void setTheta(const float theta){ m_theta = theta; }
    void reset(){
      m_nodes = 1;
      m_data[0].children = -1;
    }

    std::size_t size(){ return m_nodes; };

    _Iterator begin(const T& node_pos) { T extents = m_maxbounds - m_minbounds; return _Iterator(&m_data[0],node_pos, extents, m_theta); }
    _Iterator end(const T& node_pos) {  T extents = m_maxbounds - m_minbounds; return _Iterator(nullptr, node_pos, extents, m_theta); }
  private:
    void sub_insert(const T& node_pos, int i, T& node_minbound, T& node_extents);
    void sub_supnodes(const T& node_pos, int i, T& node_extents, std::vector<std::pair<int, T>>& super_nodes);
    void calc_subpos(int i);
    void add_children();
    T m_minbounds;
    T m_maxbounds;
    float m_theta; ///< Barnes-Hut opening criterion
    std::size_t m_nodes;   ///< Number of leaf nodes currently in tree
};

float octreeNorm(const QVector3D&);
float octreeWidth(const QVector3D&);
using OctreeIterator = TreeIterator<QVector3D, 8, octreeNorm, octreeWidth>;
using Octree = GeometricTree<QVector3D, 8, OctreeIterator>;

float quadtreeNorm(const QVector2D&);
float quadtreeWidth(const QVector2D&);
using QuadtreeIterator = TreeIterator<QVector2D, 4, quadtreeNorm, quadtreeWidth> ;
using Quadtree = GeometricTree<QVector2D, 4, QuadtreeIterator>;

#endif