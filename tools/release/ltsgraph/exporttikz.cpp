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
  QRegExp rx("[#$%_&{}^]");
  for (QChar x : str) {
    if (rx.indexIn(x) != -1) {
      escaped.append('\\');
    }
    escaped.append(x);
  }
  return escaped;
}

inline QString tikzNode(const Export::Node& node, float aspectRatio)
{
  QVector3D line(node.color());

  QString ret = "\\definecolor{currentcolor}{rgb}{%1,%2,%3}\n\\node at (%4pt, %5pt) [fill=currentcolor, %6state%8] (state%7) {%7};\n";

  ret = ret.arg(line.x(), 0, 'f', 3).arg(line.y(), 0, 'f', 3).arg(line.z(), 0, 'f', 3);
  ret = ret.arg(node.pos().x() / 10.0f * aspectRatio, 6, 'f').arg(node.pos().y() / 10.0f, 6, 'f');
  ret = ret.arg(node.initial() ? "init" : "");
  ret = ret.arg(node.id());
  ret = ret.arg(!node.active() ? ", dashed" : "");

  return ret;
}

inline QString tikzEdge(const Export::Edge& edge, float aspectRatio)
{
  const std::array<QVector3D, 4> ctrl = edge.quadraticCurve();

  QString ret = "\\draw [transition] (state%1) .. node[auto] {%3} controls (%4pt, %5pt) and (%6pt, %7pt) .. (state%2);\n";
  ret = ret.arg(edge.from().id()).arg(edge.to().id());
  ret = ret.arg(escapeLatex(edge.label()));
  ret = ret.arg(ctrl[1].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[1].y() / 10.0f, 6, 'f');
  ret = ret.arg(ctrl[2].x() / 10.0f * aspectRatio, 6, 'f').arg(ctrl[2].y() / 10.0f, 6, 'f');

  return ret;
}

/** @brief Exports the currently displayed graph as tikz output to the specified file. */
template <> void GLWidget::saveVector<Export::Tikz>(const QString& filename)
{
  Export::Exporter exporter(m_graph, filename);
  if (exporter)
  {
    float aspectRatio = width() / height();

    exporter +=
      "\\documentclass[10pt, a4paper]{article}\n\n"
      "\\usepackage{tikz}\n"
      "\\usetikzlibrary{arrows}\n\n"

      "\\begin{document}\n"
      "\\begin{tikzpicture}\n"
      "  [scale=5]\n\n"
      "   \\tikzstyle{state}=[circle, draw]\n"
      "   \\tikzstyle{initstate}=[state,fill=green]\n"
      "   \\tikzstyle{transition}=[->,>=stealth']\n";

    for (std::size_t i = 0; i < exporter.nodeCount(); ++i)
    {
      exporter += tikzNode(exporter.node(i), aspectRatio);
    }

    for (std::size_t i = 0; i < exporter.edgeCount(); ++i)
    {
      exporter += tikzEdge(exporter.edge(i), aspectRatio);
    }

    exporter +=
      "\n\\end{tikzpicture}\n"
      "\\end{document}\n";
  }
}
