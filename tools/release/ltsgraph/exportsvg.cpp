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
#include "bezier.h"

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
    "\t\t.initial { fill: Lime !important; }\n"
    "\t\t.inactive { stroke-dasharray: 3.5; }\n"
    "\t\t.prob { stroke-width: 5px; }\n"
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

  return QString("\t<circle class=\"%5%6%7node\" cx=\"%1\" cy=\"%2\" r=\"%3\"%4/>\n")
    .arg(pos.x(), 6, 'f').arg(pos.y(), 6, 'f').arg(size)
    .arg(svgColor(node.color()))
    .arg(node.initial() ? "initial " : "")
    .arg(!node.active() ? "inactive " : "")
    .arg(node.probabilistic() ? "prob " : "");
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
  using namespace Math;

  // Calculate bezier curve (screen coordinates)
  std::array<QVector3D, 4> control = edge.quadraticCurve();
  for (QVector3D& point : control)
  {
    point = scene.camera().worldToWindow(point);
  }
  const QVector3D from = control[0];
  const QVector3D to = control[3];

  // Calculate node and arrowhead sizes
  const float fromNodeRadius = scene.sizeOnScreen(edge.from().pos(), scene.nodeSize()) / 2.0;
  const float toNodeRadius = scene.sizeOnScreen(edge.to().pos(), scene.nodeSize()) / 2.0;
  const float headLength = scene.sizeOnScreen(edge.to().pos(), scene.arrowheadSize);

  try
  {
    // Subtract the nodes and arrowhead from the curve
    using Ins = Intersection<Circle, CubicBezier>;
    Ins ins1 = make_intersection(Circle{control[0], fromNodeRadius}, CubicBezier(control));
    control = CubicBezier(control).trimFront(ins1.solve(ins1.guessNearFront()));
    Ins ins2 = make_intersection(Circle{control[3], toNodeRadius + headLength}, CubicBezier(control));
    control = CubicBezier(control).trimBack(ins2.solve(ins2.guessNearBack()));

    // Calculate arrow position: find the closest point on the circle to the curve end
    const QVector3D top = control[3];
    const QVector3D tip = Circle{to, toNodeRadius}.project(top);

    return QString("\t<path class=\"edge\" d=\"M%1 %2 C%3 %4, %5 %6, %7 %8\" />\n")
      .arg(control[0].x(), 6, 'f').arg(control[0].y(), 6, 'f')
      .arg(control[1].x(), 6, 'f').arg(control[1].y(), 6, 'f')
      .arg(control[2].x(), 6, 'f').arg(control[2].y(), 6, 'f')
      .arg(control[3].x(), 6, 'f').arg(control[3].y(), 6, 'f')
      + svgArrowhead(tip, top, headLength);
  }
  catch (...)
  {
    // Soving failed, probably the nodes are too close to each other.
    const QVector3D v = to - from;
    if (length2(v) <= squared(fromNodeRadius + headLength + toNodeRadius))
    {
      // Too close to draw something meaningful: draw nothing.
      return QString();
    }
    else
    {
      // Still some space left: draw a straight line.
      const QVector3D src = Circle{from, fromNodeRadius}.project(to);
      const QVector3D tip = Circle{to, toNodeRadius}.project(from);
      const QVector3D top = Circle{to, toNodeRadius + headLength}.project(from);

      return QString("\t<path class=\"edge\" d=\"M%1 %2 L%3 %4\" />\n")
        .arg(src.x(), 6, 'f').arg(src.y(), 6, 'f')
        .arg(top.x(), 6, 'f').arg(top.y(), 6, 'f')
        + svgArrowhead(tip, top, headLength);
    }
  }
}

inline QString escapeXML(const QString& str)
{
  // Based on QString::toHtmlEscaped(), but added unicode escaping
  QString rich;
  const int len = str.length();
  rich.reserve(int(len * 1.1));
  for (int i = 0; i < len; ++i)
  {
    const QChar chr = str.at(i);
    if (chr == QLatin1Char('<'))
      rich += QLatin1String("&lt;");
    else if (chr == QLatin1Char('>'))
      rich += QLatin1String("&gt;");
    else if (chr == QLatin1Char('&'))
      rich += QLatin1String("&amp;");
    else if (chr == QLatin1Char('"'))
      rich += QLatin1String("&quot;");
    else if (chr.unicode() > 255)
      rich += QString("&#%1;").arg(static_cast<int>(chr.unicode()));
    else
      rich += chr;
  }
  rich.squeeze();
  return rich;
}

inline QString svgText(QVector3D pos, const QString& text, QVector3D color)
{
  return QString("\t<text class=\"label\" x=\"%1\" y=\"%2\"%4>%3</text>\n")
    .arg(pos.x(), 6, 'f').arg(pos.y(), 6, 'f')
    .arg(escapeXML(text))
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
