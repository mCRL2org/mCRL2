// Author(s): Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glwidget.h"

#include "export.h"

namespace Export
{
  struct SVG {};
}

inline QString svgHeader(int width, int height, std::size_t fontSize)
{
  QString ret =
    "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"
    "<svg width=\"%1\" height=\"%2\" viewBox=\"%3 %4 %5 %6\""
      " xmlns=\"http://www.w3.org/2000/svg\">\n"
    "\t<style>\n"
    "\t\t.edge { stroke: black; fill: none; }\n"
    "\t\t.node { stroke: black; fill: white; }\n"
    "\t\t.arrow { stroke: none; fill: black; }\n"
    "\t\t.initial { fill: Lime; }\n"
    "\t\t.inactive { stroke-dasharray: 3.5; }\n"
    "\t\t.label { text-anchor: middle; dominant-baseline: middle;"
      " font-family: sans-serif; font-size: %7px; }\n"
    "\t</style>\n\n";
  return ret
    .arg(width).arg(height)
    .arg(0).arg(0).arg(width).arg(height)
    .arg(fontSize);
}

inline QString svgColor(const QVector3D& color, const QVector3D defaultColor = QVector3D(1, 1, 1))
{
  if (color == defaultColor)
    return "";
  return QString(" style=\"fill:rgb(%1,%2,%3)\"")
    .arg(255 * color.x())
    .arg(255 * color.y())
    .arg(255 * color.z());
}

inline QString svgNode(const Export::Node& node, const GLScene& scene)
{
  QVector3D pos = scene.camera().worldToWindow(node.pos());
  float size = scene.sizeOnScreen(node.pos(), scene.nodeSize()) / 2.0;

  return QString("\t<circle class=\"%5%6node\" cx=\"%1\" cy=\"%2\" r=\"%3\"%4/>\n")
    .arg(pos.x(), 6, 'f').arg(pos.y(), 6, 'f').arg(size)
    .arg(svgColor(node.color()))
    .arg(node.initial() ? "initial " : "")
    .arg(!node.active() ? "inactive " : "");
}

inline QString svgArrowhead(const QVector3D& tip, const QVector3D& top, float size)
{
  // The arrow width is 2/3 the arrow length
  // Technically, |v| can be used to calculate the width, but this method allows
  // arrows of different lengths to be drawn.
  const QVector3D v = (tip - top).normalized();
  const QVector3D w = QVector3D(v.y(), -v.x(), 0.0f) * size * 0.33333f;
  return QString("\t<path class=\"arrow\" d=\"M%1 %2 L%3 %4 L %5 %6 z\" />\n")
    .arg(tip.x(), 6, 'f').arg(tip.y(), 6, 'f')
    .arg((top + w).x(), 6, 'f').arg((top + w).y(), 6, 'f')
    .arg((top - w).x(), 6, 'f').arg((top - w).y(), 6, 'f');
}

inline QString svgEdge(const Export::Edge& edge, const GLScene& scene)
{
  std::array<QVector3D, 4> control = edge.quadraticCurve();
  
  for (QVector3D& point : control)
  {
    point = scene.camera().worldToWindow(point);
  }

  // Calculate arrowhead position
  float nodeSize = scene.sizeOnScreen(edge.to().pos(), scene.nodeSize()) / 2.0;
  float headSize = scene.sizeOnScreen(edge.to().pos(), scene.arrowheadSize);
  const QVector3D v = (control[2] - control[3]).normalized();
  const QVector3D tip = control[3] + v * nodeSize;
  const QVector3D top = control[3] + v * (nodeSize + headSize);

  return QString("\t<path class=\"edge\" d=\"M%1 %2 C%3 %4, %5 %6, %7 %8\" />\n")
    .arg(control[0].x(), 6, 'f').arg(control[0].y(), 6, 'f')
    .arg(control[1].x(), 6, 'f').arg(control[1].y(), 6, 'f')
    .arg(control[2].x(), 6, 'f').arg(control[2].y(), 6, 'f')
    .arg(control[3].x(), 6, 'f').arg(control[3].y(), 6, 'f')
    + svgArrowhead(tip, top, headSize);
}

inline QString svgText(QVector3D pos, const QString& text, QVector3D color)
{
  return QString("\t<text class=\"label\" x=\"%1\" y=\"%2\"%4>%3</text>\n")
    .arg(pos.x(), 6, 'f').arg(pos.y(), 6, 'f')
    .arg(text)
    .arg(svgColor(color, QVector3D(0, 0, 0)));
}

inline QString svgNodeLabel(const Export::Node& node, const GLScene& scene)
{
  const QVector3D pos = scene.camera().worldToWindow(node.labelPos());
  return svgText(pos, node.label(), node.labelColor());
}

inline QString svgEdgeLabel(const Export::Edge& edge, const GLScene& scene)
{
  const QVector3D pos = scene.camera().worldToWindow(edge.labelPos());
  return svgText(pos, edge.label(), edge.labelColor());
}

/** @brief Exports the currently displayed graph as Scalable Vector Graphics (SVG) format to the specified file. */
template <> void GLWidget::saveVector<Export::SVG>(const QString& filename)
{
  Export::Exporter exporter(m_graph, filename);
  if (exporter)
  {
    exporter += svgHeader(width(), height(), m_scene.fontSize());

    for (std::size_t i = 0; i < exporter.edgeCount(); ++i)
    {
      exporter += svgEdge(exporter.edge(i), m_scene);
    }

    for (std::size_t i = 0; i < exporter.nodeCount(); ++i)
    {
      exporter += svgNode(exporter.node(i), m_scene);
    }

    if (m_scene.drawStateLabels())
    {
      for (std::size_t i = 0; i < exporter.nodeCount(); ++i)
      {
        exporter += svgNodeLabel(exporter.node(i), m_scene);
      }
    }

    if (m_scene.drawTransitionLabels())
    {
      for (std::size_t i = 0; i < exporter.edgeCount(); ++i)
      {
        exporter += svgEdgeLabel(exporter.edge(i), m_scene);
      }
    }

    exporter += "</svg>\n";
  }
}
