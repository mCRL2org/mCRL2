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

  enum NodeType
  {
    nt_node,
    nt_label,
    nt_handle
  };

  struct Edge
  {
      size_t from;
      size_t to;
  };

  struct Node
  {
      Coord3D pos;
      unsigned anchored : 1;
      unsigned locked : 1;
      float selected;
  };

  struct LabelString
  {
      unsigned isTau : 1;
      QString label;
  };

  struct LabelNode : public Node
  {
      size_t labelindex;
  };

  struct NodeNode : public Node
  {
      GLfloat color[3];
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
      detail::GraphImplBase* m_impl;
      mcrl2::lts::lts_type m_type;
      QString m_empty;
    public:
      Graph();
      ~Graph();
      Edge edge(size_t index) const;
      NodeNode& node(size_t index) const;
      Node& handle(size_t edge) const;
      LabelNode& transitionLabel(size_t edge) const;
      LabelNode& stateLabel(size_t edge) const;
      bool isTau(size_t labelindex) const;
      const QString& transitionLabelstring(size_t labelindex) const;
      const QString& stateLabelstring(size_t labelindex) const;
      void clip(const Coord3D& min, const Coord3D& max);
      size_t edgeCount() const;
      size_t nodeCount() const;
      size_t transitionLabelCount() const;
      size_t stateLabelCount() const;
      size_t initialState() const;
      void createImpl(mcrl2::lts::lts_type itype);
      void load(const QString& filename, const Coord3D& min, const Coord3D& max);
      void loadXML(const QString& filename);
      void saveXML(const QString& filename);
  };

}

#endif // GRAPH_H
