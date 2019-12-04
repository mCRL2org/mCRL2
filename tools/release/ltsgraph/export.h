// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_EXPORT_H
#define MCRL2_LTSGRAPH_EXPORT_H

#include "graph.h"

#include <QtOpenGL>
#include <QFile>

namespace
{
QString tikzNode(Graph::Graph& graph, std::size_t i, float aspectRatio)
{
  Graph::NodeNode& node = graph.node(i);
  QVector3D line(node.color());

  QString ret = "\\definecolor{currentcolor}{rgb}{%1,%2,%3}\n\\node at (%4pt, %5pt) [fill=currentcolor, %6state%8] (state%7) {%7};\n";

  ret = ret.arg(line.x(), 0, 'f', 3).arg(line.y(), 0, 'f', 3).arg(line.z(), 0, 'f', 3);
  ret = ret.arg(node.pos().x() / 10.0f * aspectRatio, 6, 'f').arg(node.pos().y() / 10.0f, 6, 'f');
  ret = ret.arg(graph.initialState() == i ? "init" : "");
  ret = ret.arg(i);
  ret = ret.arg(node.active() ? "" : ", dashed");

  return ret;
}

QString escapeLatex(const QString& str)
{
  QString escaped;
  QRegExp rx("[#$%_&{}^]");
  for (QChar x : str) {
    if (rx.indexIn(x) != -1) {
      escaped.append('\\');
    }
    escaped.append(x);
  }
  return escaped;
}

QString tikzEdge(Graph::Graph& graph, std::size_t i, float aspectRatio)
{
  Graph::LabelNode& label = graph.transitionLabel(i);
  Graph::Edge edge = graph.edge(i);
  QVector3D ctrl[4];
  QVector3D& from = ctrl[0];
  QVector3D& to = ctrl[3];
  QVector3D via = graph.handle(i).pos();
  from = graph.node(edge.from()).pos();
  to = graph.node(edge.to()).pos();

  // Calculate control points from handle
  ctrl[1] = via * 1.33333f - (from + to) / 6.0f;
  ctrl[2] = ctrl[1];

  QString extraControls("");

  // For self-loops, ctrl[1] and ctrl[2] need to lie apart, we'll spread
  // them in x-y direction.
  if (edge.from() == edge.to())
  {
    QVector3D diff = ctrl[1] - ctrl[0];
    diff = QVector3D::crossProduct(diff, QVector3D(0, 0, 1));
    diff = diff * ((via - from).length() / (diff.length() * 2.0));
    ctrl[1] = ctrl[1] + diff;
    ctrl[2] = ctrl[2] - diff;

    extraControls = QString(" and (%1pt, %2pt)").arg(ctrl[2].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[2].y() / 10.0f, 6, 'f');
  }

  QString ret = "\\draw [transition] (state%1) .. node[auto] {%3} controls (%4pt, %5pt)%6 .. (state%2);\n";
  ret = ret.arg(edge.from()).arg(edge.to());
  ret = ret.arg(escapeLatex(graph.transitionLabelstring(label.labelindex())));
  ret = ret.arg(ctrl[1].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[1].y() / 10.0f, 6, 'f');
  ret = ret.arg(extraControls);

  return ret;
}
} // unnamed namespace

/// \brief Exports the given graph as a tikz output in the given aspect ratio to a file.
inline void export_graph_as_tikz_input(Graph::Graph& graph, const QString& filename, float aspectRatio)
{
  QString tikz_code  = "\\documentclass[10pt, a4paper]{article}\n\n";
  tikz_code += "\\usepackage{tikz}\n";
  tikz_code += "\\usetikzlibrary{arrows}\n\n";

  tikz_code += "\\begin{document}\n";
  tikz_code += "\\begin{tikzpicture}\n";
  tikz_code += "  [scale=2]\n\n";
  tikz_code += "   \\tikzstyle{state}=[circle, draw]\n";
  tikz_code += "   \\tikzstyle{initstate}=[state,fill=green]\n";
  tikz_code += "   \\tikzstyle{transition}=[->,>=stealth']\n";

  graph.lock(GRAPH_LOCK_TRACE);

  bool sel = graph.hasExploration();
  std::size_t nodeCount = sel ? graph.explorationNodeCount() : graph.nodeCount();
  std::size_t edgeCount = sel ? graph.explorationEdgeCount() : graph.edgeCount();

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    tikz_code += tikzNode(graph, sel ? graph.explorationNode(i) : i, aspectRatio);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    tikz_code += tikzEdge(graph, sel ? graph.explorationEdge(i) : i, aspectRatio);
  }

  graph.unlock(GRAPH_LOCK_TRACE);

  tikz_code += "\n\\end{tikzpicture}\n";
  tikz_code += "\\end{document}\n";

  QFile file(filename);

  if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    file.write(tikz_code.toLatin1());
    file.close();
  }
}

#endif // MCRL2_LTSGRAPH_EXPORT_H
