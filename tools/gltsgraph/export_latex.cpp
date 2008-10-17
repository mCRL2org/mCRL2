//  Copyright 2007 Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./export_latex.cpp

#include "export_latex.h"
#include <iostream>
#include <boost/format.hpp>
#include <cmath>
#include <wx/textfile.h>

#include <workarounds.h> // for M_PI

ExporterLatex::ExporterLatex(Graph* g) : Exporter(g)
{
};

bool ExporterLatex::export_to(wxString filename)
{
  tikz_code  = "\\documentclass[10pt, a4paper]{article}\n\n";
  tikz_code += "\\usepackage{tikz}\n";
  tikz_code += "\\usetikzlibrary{arrows}\n\n";

  tikz_code += "\\begin{document}\n";
  tikz_code += "\\begin{tikzpicture}\n";
  tikz_code += "  [scale=2,\n";
  tikz_code += "   state/.style={circle, draw},\n";
  tikz_code += "   initstate/.style={state,draw=green},\n";
  tikz_code += "   transition/.style={->,>=stealth'}]\n";


  // TODO: Draw states, transitions
  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    State* s = graph->getState(i);

    boost::format node;

    if(s->isInitialState())
    {
      node = 
        boost::format("\\node at (%1%pt, %2%pt) [initstate] (state%3%) {%3%};\n");
    }
    else
    {
      node = boost::format("\\node at (%1%pt, %2%pt) [state] (state%3%) {%3%};\n");
    }

    double x = s->getX() / 10.0;
    double y = s->getY() / 10.0;
    
    node%x%y%i;

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
  boost::format draw("\\draw [transition] (state%1%) .. node[auto] {%5%} controls (%3%pt, %4%pt) .. (state%2%);\n");

  State* from = tr->getFrom();
  State* to = tr->getTo();

  size_t fromState = from->getValue();
  size_t toState = to->getValue();

  double controlX, controlY;
  
  tr->getControl(controlX, controlY);
  controlX /= 10.0;
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
  double alpha = tr->getControlAlpha();
  double dist = tr->getControlDist();

  xState = s->getX() / 10.0;
  yState = s->getY() / 10.0;
  
  xVirtual = xState + cos(alpha) * dist * 20.0f;
  yVirtual = yState + sin(alpha) * dist * 20.0f;

  double gamma = alpha + beta;
  double delta = alpha - beta;

  double xFactor, yFactor;
  double cosGamma = cos(gamma);
  double sinGamma = sin(gamma);
  double cosDelta = cos(delta);
  double sinDelta = sin(delta);

  if(fabs(cosGamma + cosDelta) > 0.01)
  {
    xFactor = (8 * (xVirtual - xState)) / (3 * (cosGamma + cosDelta));
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
    yFactor = (8 *  (yVirtual - yState)) / (3  * sinGamma + sinDelta);
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
