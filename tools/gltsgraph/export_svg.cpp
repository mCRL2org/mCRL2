//  Copyright 2007 Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./export_svg.cpp

#include "export_svg.h"  
#include <boost/format.hpp>
#include <wx/textfile.h>
#include <iostream>
#include <sstream>
#include <cmath>

#include "workarounds.h" // for M_PI on Windows with MSVC

ExporterSVG::ExporterSVG(Graph* g) : Exporter(g) {};


ExporterSVG::~ExporterSVG() {};


bool ExporterSVG::export_to(wxString filename)
{
  // SVG header
  svg_code  = "<?xml version = \"1.0\" standalone=\"no\"?>\n\n";
  svg_code += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://w3c.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n\n";
  
  // For the size of the viewBox (canvas), we stick to the virtual size of 
  // the graph's model, which is 2000 by 2000.
  svg_code += "<svg viewBox=\"0 0 2000 2000\" width = \"100%\" height=\"100%\" version=\"1.1.\" \n";

  svg_code += "xmlns=\"http://www.w3.org/2000/svg\">\n\n";

  // Add a "marker" definition for arrow heads,
  svg_code += "<defs>\n";
  svg_code += "<marker id = \"Arrowhead\" viewBox = \"0 0 20 10\" \n";
  svg_code += " refX=\"20\" refY = \"5\" \n";
  svg_code += " markerUnits = \"strokeWidth\" orient = \"auto\" \n";
  svg_code += " markerWidth = \"40\" markerHeight = \"40\">\n";
  svg_code += " <polyline points = \"0,0 10,5 0,10 1,5\" fill = \"black\" />\n";
  svg_code += "</marker></defs>\n\n";

  // First put in the transitions, since transitions move from/to the centers
  // of states, instead of borders.
  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    State* from = graph->getState(i);
    for(size_t j = 0; j < from->getNumberOfTransitions(); ++j)
    {
      Transition* tr = from->getTransition(j);
      // Interpolate and draw the path
      drawBezier(tr);
    }
    
    for(size_t j = 0; j < from->getNumberOfSelfLoops(); ++j)
    {
      Transition* tr = from->getSelfLoop(j);
      // draw the loop. We do this in a seperate function to avoid clutter
      drawSelfLoop(tr);
    }
  }

  // Go through all the states again, to make sure that no transitions are seen 
  // on top of them (SVG uses the painter's algorithm to render)
  for(size_t i = 0; i < graph->getNumberOfStates(); ++i)
  {
    State* from = graph->getState(i);
    boost::format f("<circle cx = \"%1%\" cy =\"%2%\" r=\"%3%\" stroke=\"%4%\" stroke-width=\"%5%\" fill=\"rgb(%6%, %7%, %8%)\"/>\n\n");

    double fromX = 1000 + from->getX();
    double fromY = 1000 - from->getY();
    double radius = 20; // TODO

    unsigned int stroke_width;
    std::string stroke;

    if(from->isInitialState())
    {
      stroke = "green";
      stroke_width = 2;
    }
    else
    {
      stroke = "black";
      stroke_width = 1;
    }

    int red = 255;
    int green = 255;
    int blue = 255;

    f%fromX
     %fromY
     %radius
     %stroke
     %stroke_width
     %red
     %green
     %blue;

    svg_code += boost::str(f);

    // Draw state label.
    boost::format label_f("<text x=\"%1%\" y=\"%2%\" fill=\"black\" font-size=\"24\">\n");
    label_f%(fromX - 7)%(fromY + 5);
    svg_code += boost::str(label_f);
    std::stringstream labelstr;
    labelstr << from->getValue();
    svg_code += "  " + labelstr.str() + "\n</text> \n\n";
  }

  

  // End svg code
  svg_code += "</svg>\n";
 
  // Create the file

  wxTextFile svgFile(filename);

  if(svgFile.Exists())
  {
    if(!svgFile.Open(filename))
    {
      return false;
    }
    else
    {
      svgFile.Clear();
    }
  }
  else
  {
    if (!svgFile.Create(filename))
    {
      return false;
    }
  }

  wxString svgCodeWX(svg_code.c_str(), wxConvLocal);
  svgFile.AddLine(svgCodeWX);
  svgFile.AddLine(wxEmptyString);

  if(!svgFile.Write())
  {
    return false;
  }

  if(!svgFile.Close())
  {
    return false;
  }
  
  // TODO: Proper return
  return true;
}

void ExporterSVG::drawBezier(Transition* tr)
{
  State *from, *to;
  double xFrom, yFrom, xTo, yTo, xVirtual, yVirtual, xControl, yControl;
  
  from = tr->getFrom();
  to = tr->getTo();
  
  xFrom = 1000.0 + from->getX();
  yFrom = 1000.0 - from->getY();

  xTo = 1000.0 + to->getX();
  yTo = 1000.0 - to->getY();
  
  tr->getControl(xVirtual, yVirtual);
  
  xVirtual += 1000.0;
  yVirtual = 1000.0 - yVirtual;
  // For a justification of the xControl, yControl computations, see the 
  // drawTransition method in visualizer.cpp
  xControl = 2.0 * xVirtual - .5 * (xFrom + xTo);
  yControl = 2.0 * yVirtual - .5 * (yFrom + yTo);
    
  svg_code += "<path d = \"";
  
  boost::format p1("M%1%,%2% Q%3%,%4% %5%,%6%\"\n");
  p1 % xFrom    % yFrom
     % xControl % yControl
     % xTo      % yTo;

  svg_code += boost::str(p1);

  svg_code += " stroke = \"black\" stroke-width=\"1\" fill=\"none\"\n";
  svg_code += " marker-end=\"url(#Arrowhead)\"/>\n";


  // Draw the transition's label
  std::string label = tr->getLabel();

  boost::format label_f("<text x =\"%1%\" y=\"%2%\" fill=\"black\" font-size=\"24\">\n");
  label_f%xVirtual%(yVirtual - 5);

  svg_code += boost::str(label_f);
  svg_code += "  " + label + "\n</text>\n\n";

}

void ExporterSVG::drawSelfLoop(Transition* tr)
{
  // For a self-loop, t.to == t.from
  State* s = tr->getFrom();
  
  double alpha = tr->getControlAlpha();
  double beta = .25 * M_PI;
  double dist = tr->getControlDist();

  double xState = 1000 + s->getX();
  double yState = 1000 - s->getY();

  double xVirtual = xState + cos(alpha) * dist * 200.0f;
  double yVirtual = yState - sin(alpha) * dist * 200.0f;
  
  double gamma = alpha + beta;
  double delta = alpha - beta;

  double xFactor, yFactor; 
  double xControl1, yControl1;
  double xControl2, yControl2;
  double cosGamma = cos(gamma);
  double cosDelta = cos(delta);
  double sinGamma = sin(gamma);
  double sinDelta = sin(delta);

  if(fabs(cosGamma + cosDelta) > 0.01)
  {
    xFactor = (8 * (xVirtual - xState)) / (3 * (cos(gamma) + cos(delta)));
    xControl1 = xState + xFactor * cosGamma;
    xControl2 = xState + xFactor * cosDelta;
  }
  
  if(fabs(sinGamma + sinDelta) <= 0.01)
  {
    float additive = tan(beta) * (xControl1 - xState);
    yControl1 = yState + additive;
    yControl2 = yState - additive;
  }
  else
  {
    yFactor = (8 * (yVirtual - yState)) / (3 * (sin(gamma) + sin(delta)));
    yControl1 = yState + yFactor * sinGamma;
    yControl2 = yState + yFactor * sinDelta;

    if(fabs(cosGamma + cosDelta) <= .01)
    {
      float additive = tan(beta)* (yControl1 - yState);
      xControl1 = xState - additive;
      xControl2 = xState + additive;
    }
  }

  boost::format f("<path d=\"M%1%,%2% C%3%,%4% %5%,%6% %7%,%8%\"\n");
  f%xState%yState
   %xControl1%yControl1
   %xControl2%yControl2
   %xState%yState;

  svg_code += boost::str(f);
  svg_code += " stroke = \"black\" stroke-width=\"1\" fill=\"none\"\n";
  svg_code += " marker-end=\"url(#Arrowhead)\"/>\n";
  
  // Draw the transition's label
  std::string label = tr->getLabel();

  boost::format label_f("<text x =\"%1%\" y=\"%2%\" fill=\"black\" font-size=\"24\">\n");
  label_f%xVirtual%(yVirtual - 5);

  svg_code += boost::str(label_f);
  svg_code += "  " + label + "\n</text>\n\n";
}

