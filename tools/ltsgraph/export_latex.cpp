// Author(s): Carst Tankink and Ali Deniz Aladagli
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file export_latex.cpp
/// \brief Implementation of LaTeX exporter.

#include "wx.hpp" // precompiled headers

#include "export_latex.h"
#include <iostream>
#include "ltsgraph.h"
#include <boost/format.hpp>
#include <cmath>
#include <wx/textfile.h>

#include <workarounds.h> // for M_PI

ExporterLatex::ExporterLatex(Graph* g, LTSGraph* owner) : Exporter(g)
{
  this->owner = owner;
}

bool ExporterLatex::export_to(wxString filename)
{
  tikz_code  = "\\documentclass[10pt, a4paper]{article}\n\n";
  tikz_code += "\\usepackage{tikz}\n";
  tikz_code += "\\usetikzlibrary{arrows}\n\n";

  tikz_code += "\\begin{document}\n";
  tikz_code += "\\begin{tikzpicture}\n";
  tikz_code += "  [scale=2]\n\n";
  tikz_code += "   \\tikzstyle{state}=[circle, draw]\n";
  tikz_code += "   \\tikzstyle{initstate}=[state,draw=green]\n";
  tikz_code += "   \\tikzstyle{transition}=[->,>=stealth']\n";


  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    State* s = graph->getState(i);

    boost::format node;

    if(s->isInitialState())
    {
      node =
        boost::format("\\definecolor{currentcolor}{rgb}{%4%,%5%,%6%}\n\\node at (%1%pt, %2%pt) [initstate, fill=currentcolor] (state%3%) {%3%};\n");
    }
    else
    {
      node = boost::format("\\definecolor{currentcolor}{rgb}{%4%, %5%, %6%}\n\\node at (%1%pt, %2%pt) [state, fill=currentcolor] (state%3%) {%3%};\n");
    }

    double aspect = owner->getAspectRatio();
    double x = s->getX() / 10.0 * aspect;
    double y = s->getY() / 10.0;

    wxColour c = s->getColour();
    double r = static_cast<double>(c.Red()) / 255.0;
    double g = static_cast<double>(c.Green()) / 255.0;
    double b = static_cast<double>(c.Blue()) / 255.0;

    node%x%y%i%r%g%b;

    tikz_code += boost::str(node);
  }
  tikz_code += "\n";

  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    State* s = graph->getState(i);
    for(size_t j = 0; j < s->getNumberOfTransitions(); ++j)
    {
      Transition* tr = s->getTransition(j);
      drawBezier(tr);
    }

    for(size_t j = 0; j < s->getNumberOfSelfLoops(); ++j)
    {
      Transition* sl = s->getSelfLoop(j);
      drawSelfLoop(sl);
    }
  }

  tikz_code += "\n\\end{tikzpicture}\n";
  tikz_code += "\\end{document}\n";

  wxTextFile tikzFile(filename);

  // Open file if it exists.
  if(tikzFile.Exists())
  {
    if(!tikzFile.Open(filename))
    {
      return false;
    }
    else
    {
      tikzFile.Clear();
    }
  }

  // Convert code to wxString
  wxString tikzCodeWX(tikz_code.c_str(), wxConvLocal);
  tikzFile.AddLine(tikzCodeWX);
  tikzFile.AddLine(wxEmptyString);

  // Write to file
  if(!tikzFile.Write())
  {
    return false;
  }

  // Close file
  if(!tikzFile.Close())
  {
    return false;
  }

  // If we came this far, all went well
  return true;

}

void ExporterLatex::drawBezier(Transition* tr)
{
  boost::format draw("\\draw [transition] (state%1%) .. node[auto] {%6%} controls (%3%pt, %4%pt) .. (state%2%);\n");

  State* from = tr->getFrom();
  State* to = tr->getTo();

  size_t fromState = from->getValue();
  size_t toState = to->getValue();

  double controlX, controlY, controlZ;
  double aspect = owner->getAspectRatio();
  tr->getControl(controlX, controlY, controlZ);
  controlX /= 10.0;
  controlX = controlX * aspect;
  controlY /= 10.0;

  draw%fromState%toState
      %controlX%controlY
      %tr->getLabel();
  tikz_code += boost::str(draw);

}

void ExporterLatex::drawSelfLoop(Transition* tr)
{
  boost::format draw("\\draw [transition] (state%1%) .. node[auto] {%2%} controls (%3%pt, %4%pt) and (%5%pt, %6%pt) .. (state%1%);\n");

  State* s = tr->getFrom();

  size_t stateNo = s->getValue();

  // Calculate control points for the self loop.
  double beta = .25 * M_PI;

  double xState,    yState;
  double xVirtual,  yVirtual;
  double xControl1, yControl1;
  double xControl2, yControl2;
  double alpha = tr->getControlBeta();
  double dist = tr->getControlDist();

  xState = s->getX() / 10.0;
  yState = s->getY() / 10.0;

  xVirtual = xState + cos(alpha) * dist * 20.0f;
  yVirtual = yState + sin(alpha) * dist * 20.0f;

  double gamma = alpha + beta;
  double delta = alpha - beta;

  double cosGamma = cos(gamma);
  double sinGamma = sin(gamma);
  double cosDelta = cos(delta);
  double sinDelta = sin(delta);

  if(fabs(cosGamma + cosDelta) > 0.01)
  {
    double xFactor = (8 * (xVirtual - xState)) / (3 * (cosGamma + cosDelta));
    xControl1 = xState + xFactor * cosGamma;
    xControl2 = xState + xFactor * cosDelta;
  }

  if(fabs(sinGamma + sinDelta) <= .01)
  {
    float additive = tan(beta) * (xControl1 - xState);
    yControl1 = yState + additive;
    yControl2 = yState - additive;
  }
  else
  {
    double yFactor = (8 *  (yVirtual - yState)) / (3  * sinGamma + sinDelta);
    yControl1 = yState + yFactor * sinGamma;
    yControl2 = yState + yFactor * sinDelta;

    if(fabs(cosGamma + cosDelta) <= .01)
    {
      float additive = tan(beta) * (yControl1 - yState);
      xControl1 = xState - additive;
      xControl2 = xState + additive;
    }
  }

  draw%stateNo%tr->getLabel()
      %xControl1%yControl1
      %xControl2%yControl2;

  tikz_code += boost::str(draw);
}
