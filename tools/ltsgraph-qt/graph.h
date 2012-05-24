// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file graph.h
  @author S. Cranen, R. Boudewijns

  This file contains an interface to the graph data structure used by LTSGraph.

*/

#ifndef GRAPH_H
#define GRAPH_H

#include "mcrl2/lts/lts.h"

#include <QtOpenGL>

namespace Graph
{

  struct Coord3D
  {
      GLfloat x;
      GLfloat y;
      GLfloat z;
      Coord3D(GLfloat x, GLfloat y, GLfloat z)
        : x(x), y(y), z(z) {}
      Coord3D() : x(0), y(0), z(0) {}
      Coord3D& operator+=(const Coord3D& a)
      {
        x += a.x;
        y += a.y;
        z += a.z;
        return *this;
      }
      Coord3D& operator-=(const Coord3D& a)
      {
        x -= a.x;
        y -= a.y;
        z -= a.z;
        return *this;
      }

      Coord3D& operator*=(float f)
      {
        x *= f;
        y *= f;
        z *= f;
        return *this;
      }

      Coord3D& operator/=(float f)
      {
        x /= f;
        y /= f;
        z /= f;
        return *this;
      }


      Coord3D operator+(const Coord3D& a) const
      {
        return Coord3D(a.x + x, a.y + y, a.z + z);
      }
      Coord3D operator-(const Coord3D& a) const
      {
        return Coord3D(x - a.x, y - a.y, z - a.z);
      }
      Coord3D operator*(GLfloat c) const
      {
        return Coord3D(c * x, c * y, c * z);
      }
      Coord3D operator-() const
      {
        return Coord3D(-x, -y, -z);
      }
      Coord3D operator/(GLfloat c) const
      {
        return Coord3D(x / c, y / c, z / c);
      }
      float size() const
      {
        return sqrt(x * x + y * y + z * z);
      }
      float dot(const Coord3D& a) const
      {
        return x * a.x + y * a.y + z * a.z;
      }
      Coord3D cross(const Coord3D& a) const
      {
        return Coord3D(
              y * a.z - z * a.y,
              z * a.x - x * a.z,
              x * a.y - y * a.x
              );
      }
      void clip(const Coord3D& min, const Coord3D& max)
      {
        if (x < min.x) x = min.x;
        if (x > max.x) x = max.x;
        if (y < min.y) y = min.y;
        if (y > max.y) y = max.y;
        if (z < min.z) z = min.z;
        if (z > max.z) z = max.z;
      }
      operator const GLfloat*() const { return &x; }
      bool operator==(const Coord3D& other) const
      {
        return x == other.x && y == other.y && z == other.z;
      }
      bool operator!=(const Coord3D& other) const
      {
        return !operator==(other);
      }
  };


  /**
   * @brief A structure which contains the information of a single edge.
   */
  struct Edge
  {
      size_t from;      ///< The originating node.
      size_t to;        ///< The node pointed at.
  };


  /**
   * @brief A structure which contains the information of a single node (as in movable object).
   */
  struct Node
  {
      Coord3D pos;            ///< The position of the node.
      unsigned anchored : 1;  ///< Indicates wether this node cannot be moved.
      unsigned locked : 1;    ///< Indicates if anchored should be left true at all times.
      float selected;         ///< Indicates that this node is selected (pointed at). Range 0.0f .. 1.0f.
  };


  /**
   * @brief A structure which contains the information of a single label (with tau indicator).
   */
  struct LabelString
  {
      unsigned isTau : 1;     ///< Indicates that the label is tau.
      QString label;          ///< The string representation of the label.
  };


  /**
   * @brief A structure which contains the information of a single edge.
   */
  struct LabelNode : public Node
  {
      size_t labelindex;      ///< The index of the label (string).
  };


  /**
   * @brief A structure which contains the information of a single graph node.
   */
  struct NodeNode : public Node
  {
      GLfloat color[3];       ///< The (painted) color of the node.
  };

  namespace detail
  {
    class GraphImplBase;
  }

  /**
  @brief: This is the internal data structure that LTSGraph operates on.

    In its implementation it uses the mCRL2 lts classes to represent the graphs,
    and augments it with further information. In particular, positions of labels
    and edge handles are stored as if they were nodes.
*/
  class Graph{
    private:
      detail::GraphImplBase* m_impl;  ///< The internal implementation of the graph used.
      mcrl2::lts::lts_type m_type;    ///< The type of the current graph.
      QString m_empty;                ///< Empty string that is returned as label if none present.


      /**
       * @brief Initialises a graph implementation of the desired type.
       */
      void createImpl(mcrl2::lts::lts_type itype);
    public:

      /**
       * @brief Constructor which initialises a empty graph.
       */
      Graph();

      /**
       * @brief Destructor.
       */
      ~Graph();


      /**
       * @brief Loads a graph with random positioning for the nodes.
       * @param filename The file which contains the graph.
       * @param min The minimum coordinates for any node.
       * @param max The maximum coordinates for any node.
       */
      void load(const QString& filename, const Coord3D& min, const Coord3D& max);

      /**
       * @brief Loads a graph with from a XML file exported using @fn saveXML.
       * @param filename The file which contains the graph.
       */
      void loadXML(const QString& filename);

      /**
       * @brief Saves the current graph with all neccesary information.
       * @param filename The file to which the graph is saved.
       */
      void saveXML(const QString& filename);

      /**
       * @brief Returns the string representation of the transition label with index @e labelindex.
       *        If the index is not valid, an empty string is returned.
       * @param labelindex The index of the label.
       */
      const QString& transitionLabelstring(size_t labelindex) const;

      /**
       * @brief Returns the string representation of the state label with index @e labelindex.
       *        If the index is not valid, an empty string is returned.
       * @param labelindex The index of the label.
       */
      const QString& stateLabelstring(size_t labelindex) const;

      /**
       * @brief Restrains all nodes of the graph between @e min and @e max.
       * @param min The minimum coordinates for any node.
       * @param max The maximum coordinates for any node.
       */
      void clip(const Coord3D& min, const Coord3D& max);

      // Getters and setters
      Edge edge(size_t index) const { return m_impl->edges[index]; }
      NodeNode& node(size_t index) const { return m_impl->nodes[index]; }
      Node& handle(size_t edge) const { return m_impl->handles[edge]; }
      LabelNode& transitionLabel(size_t edge) const { return m_impl->transitionLabelnodes[edge]; }
      LabelNode& stateLabel(size_t edge) const { return m_impl->stateLabelnodes[edge]; }
      bool isTau(size_t labelindex) const { return m_impl->is_tau(labelindex); }

      size_t edgeCount() const { return m_impl->edges.size(); }
      size_t nodeCount() const { return m_impl->nodes.size(); }
      size_t transitionLabelCount() const { return m_impl->transitionLabels.size(); }
      size_t stateLabelCount() const { return m_impl->stateLabels.size(); }
      size_t initialState() const { return m_impl->initialState; }
  };

}

#endif // GRAPH_H
