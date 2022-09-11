// Author(s): Ruben Vink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file layoututility.cpp
  @author R. Vink

  This file contains implementation of classes facilitating layout algorithms through data structures

*/

#include "layoututility.h"
#include <cmath>

#ifndef FLT_EPSILON
#define FLT_EPSILON 1.192092896e-07F
#endif

template< class T >
TreeNode<T> emptyTreeNode(){
    TreeNode<T> node;
    node.children = -1;
    return node;
}

template < class T, int num_children, class _Iterator >
GeometricTree<T, num_children, _Iterator>::GeometricTree(float theta, T minbounds, T maxbounds) : m_theta(theta), m_minbounds(minbounds), m_maxbounds(maxbounds), m_nodes(1)
{
    m_data = {emptyTreeNode<T>()};
};

template<>
Octree::GeometricTree(float theta, QVector3D minbounds, QVector3D maxbounds) : m_theta(theta), m_minbounds(minbounds), m_maxbounds(maxbounds), m_nodes(1){
    m_data = {emptyTreeNode<QVector3D>()};
}

template < class T, int num_children, class _Iterator >
void GeometricTree<T, num_children, _Iterator>::add_children(){
    if (m_data.size() < m_nodes + num_children){
        for (int i = 0; i < num_children; i++) m_data.emplace_back(emptyTreeNode<T>());
    }else{
        for (int i = 0; i < num_children; i++) m_data[m_nodes+i].children = -1;
    }
    m_nodes += num_children;
}

template<>
void Octree::sub_insert(const QVector3D& node_pos, int i, QVector3D& node_minbound, QVector3D& node_extents){
    // Index has to be within m_data range
    assert(i >= 0 && i < m_nodes);

    if (m_data[i].children == TreeNodeTypes::EMPTY_NODE){
        // we can insert here
        m_data[i].pos = node_pos;
        m_data[i].children = TreeNodeTypes::LEAF_NODE;
        return; // node has been inserted
    }else if(m_data[i].children == TreeNodeTypes::LEAF_NODE){
      if ((node_pos - m_data[i].pos).lengthSquared() < FLT_EPSILON)
      {
        // trying to insert position that is already in tree
        return;
      }
        // we need to subdivide the current node
        m_data[i].children = 1;
        m_data[i].offset = m_nodes - i;
        add_children();

        // now we have subdivided we need to move the current point down
        QVector3D relative_pos = (m_data[i].pos - node_minbound) / node_extents; 
        int dx = (int)round(relative_pos.x());
        int dy = (int)round(relative_pos.y());
        int dz = (int)round(relative_pos.z());
        // Encode children in x y z order of least significant bit
        int rel_child_index = (dz << 2) | (dy << 1) | dx;
        int child_index = i+m_data[i].offset+rel_child_index;

        // Move node information down
        m_data[child_index].pos = m_data[i].pos;
        m_data[child_index].children = 0; 
    }

    // if we make it this far we know for sure that the node has not been inserted so we need to recurse

    // first we add the node to-be-inserted to the cumulative position and increment the number of children
    m_data[i].pos += node_pos;
    m_data[i].children += 1;

    // then we find what child the to-be-inserted node has to go in
    QVector3D relative_pos = (node_pos - node_minbound) / node_extents; 
    int dx = (int)round(relative_pos.x());
    int dy = (int)round(relative_pos.y());
    int dz = (int)round(relative_pos.z());
    // Encode children in x y z order of least significant bit
    int rel_child_index = (dz << 2) | (dy << 1) | dx;
    int child_index = i+m_data[i].offset+rel_child_index;
    // and lastly we update the node bounds using the calculated dx, dy, dz values
    node_extents *= 0.5;
    node_minbound += QVector3D(dx, dy, dz)*node_extents;
    // recurse
    sub_insert(node_pos, child_index, node_minbound, node_extents);
}

template<>
void Octree::insert(const QVector3D& node_pos){
    // Tree has to exist
    assert(m_nodes >= 1);
    // Attempting to insert node outside of bounds should never happen
    assert(node_pos.x() > m_minbounds.x() && node_pos.y() > m_minbounds.y() && node_pos.z() > m_minbounds.z());
    assert(node_pos.x() < m_maxbounds.x() && node_pos.y() < m_maxbounds.y() && node_pos.z() < m_maxbounds.z());

    // call recursive function
    QVector3D node_extents = m_maxbounds - m_minbounds;
    QVector3D node_minbounds(m_minbounds);
    sub_insert(node_pos, 0, node_minbounds, node_extents);
}

template<>
void Octree::sub_supnodes(const QVector3D& node_pos, int i, QVector3D& node_extents, std::vector<std::pair<int, QVector3D>>& super_nodes){
    // we should never recurse on an empty subtree
    assert(m_data[i].children != -1);

    if (m_data[i].children == 0){
        // has to be a supernode
        super_nodes.push_back({1, m_data[i].pos});
    }else{
        // we check Barnes-Hut criterion
        // if satisfied we stop recursing and add to super_nodes
        QVector3D xS = m_data[i].pos / m_data[i].children;
        if (node_extents.lengthSquared()/3 <= m_theta*m_theta*(node_pos-xS).lengthSquared()){
            super_nodes.push_back({m_data[i].children, xS});
        }else{
            // iterate over all children and recurse if necessary
            node_extents *= 0.5f;
            for (int j = 0; j < 8; j++){
                int child_index = i+m_data[i].offset+j; 
                if (m_data[child_index].children >= 0){
                    sub_supnodes(node_pos, child_index, node_extents, super_nodes);
                }
            }
            node_extents *= 2;
        }
    }
}

template<>
std::vector<std::pair<int, QVector3D>> Octree::getSuperNodes(const QVector3D& node_pos){
    std::vector<std::pair<int, QVector3D>> super_nodes;
    QVector3D extents = m_maxbounds - m_minbounds;
    sub_supnodes(node_pos, 0, extents, super_nodes);
    return super_nodes;
}


float octreeNorm(const QVector3D& vec) { return vec.lengthSquared(); }
float octreeWidth(const QVector3D& vec) { return vec.lengthSquared()*0.333333333333333; }
float quadtreeNorm(const QVector2D& vec) { return vec.lengthSquared(); }
float quadtreeWidth(const QVector2D& vec) { return vec.lengthSquared() * 0.5f; }
