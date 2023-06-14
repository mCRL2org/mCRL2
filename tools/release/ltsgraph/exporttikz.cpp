// Author(s): Rimco Boudewijns, Sjoerd Cranen and Ferry Timmers
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "glwidget.h"

#include <QtOpenGL>
#include "export.h"

namespace Export
{
  struct Tikz{};
}

inline QString escapeLatex(const QString& str)
{
  QString escaped;
  QRegularExpression rx("[#$%_&{}^]");
  for (QChar x : str) {
    if (rx.globalMatch(x).isValid()) {
      escaped.append('\\');
    }
    escaped.append(x);
  }
  return escaped;
}

inline QString tikzNode(const Export::Node& node, bool exportStateNumbers, bool exportStateLabels)
{
  const QVector3D nodeColorVec(node.color());
  const QVector3D labelColorVec(node.labelColor());

  QString color1 = QString("\\definecolor{currentcolor}{rgb}{%1,%2,%3}\n")
    .arg(nodeColorVec.x(), 0, 'f', 3).arg(nodeColorVec.y(), 0, 'f', 3).arg(nodeColorVec.z(), 0, 'f', 3);
  QString color2 = QString("\\definecolor{currentlabelcolor}{rgb}{%1,%2,%3}\n")
    .arg(labelColorVec.x(), 0, 'f', 3).arg(labelColorVec.y(), 0, 'f', 3).arg(labelColorVec.z(), 0, 'f', 3);
  QString ret = "\\node[label={[color=currentlabelcolor]above:%6}] at (%1pt, %2pt) [fill=currentcolor, %3state%7%8] (state%4) {%5};\n";

  ret = ret.arg(node.pos().x(), 6, 'f').arg(node.pos().y(), 6, 'f');
  ret = ret.arg(node.initial() ? "init" : "");
  ret = ret.arg(node.id());
  ret = ret.arg(exportStateNumbers ? QString::number(node.id()) : "");
  ret = ret.arg(exportStateLabels ? escapeLatex(node.label()) : "");
  ret = ret.arg(!node.active() ? ", inactive" : "");
  ret = ret.arg(node.probabilistic() ? ", prob" : "");

  return color1 + color2 + ret;
}

inline QString tikzEdge(const Export::Edge& edge, bool exportEdgeLabels)
{
  const std::array<QVector3D, 4> ctrl = edge.quadraticCurve();
  const QVector3D colorVec(edge.labelColor());

  QString ret = "\\definecolor{currentcolor}{rgb}{%1,%2,%3}\n\\draw [transition] (state%4) .. node[auto,color=currentcolor] {%6} controls (%7pt, %8pt) and (%9pt, %10pt) .. (state%5);\n";

  ret = ret.arg(colorVec.x(), 0, 'f', 3).arg(colorVec.y(), 0, 'f', 3).arg(colorVec.z(), 0, 'f', 3);
  ret = ret.arg(edge.from().id()).arg(edge.to().id());
  ret = ret.arg(exportEdgeLabels ? escapeLatex(edge.label()) : "");
  ret = ret.arg(ctrl[1].x(), 6, 'f').arg(ctrl[1].y(), 6, 'f');
  ret = ret.arg(ctrl[2].x(), 6, 'f').arg(ctrl[2].y(), 6, 'f');

  return ret;
}

/** @brief Exports the currently displayed graph as tikz output to the specified file. */
template <> void GLWidget::saveVector<Export::Tikz>(const QString& filename)
{
  Export::Exporter exporter(m_graph, filename);
  if (exporter)
  {
    exporter += QString(
      "\\documentclass[10pt]{standalone}\n\n"
      "\\usepackage{tikz}\n"
      "\\usetikzlibrary{arrows.meta}\n\n"

      "\\begin{document}\n"
      "\\begin{tikzpicture}\n"
      "   \\tikzstyle{state}=[circle,draw,minimum width=%1pt,minimum height=%1pt]\n"
      "   \\tikzstyle{initstate}=[state,fill=green]\n"
      "   \\tikzstyle{inactive}=[dashed]\n"
      "   \\tikzstyle{prob}=[line width = 2.5pt]\n"
      "   \\tikzstyle{transition}=[->,arrows={-Stealth[scale=2,inset=1pt]}]\n")
      .arg(m_scene.nodeSizeScaled());

    for (std::size_t i = 0; i < exporter.nodeCount(); ++i)
    {
      exporter += tikzNode(exporter.node(i), m_scene.drawStateNumbers(), m_scene.drawStateLabels());
    }

    for (std::size_t i = 0; i < exporter.edgeCount(); ++i)
    {
      exporter += tikzEdge(exporter.edge(i), m_scene.drawTransitionLabels());
    }

    exporter +=
      "\n\\end{tikzpicture}\n"
      "\\end{document}\n";
  }
}
