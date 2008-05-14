//  Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  LICENSE_1_0.txt)
//
/// \file ./export_svg.cpp

#include "export_svg.h"

#include <fstream>
#include <wx/textfile.h> 
#include <wx/dc.h>
#include <math.h>
#include <boost/format.hpp>

#define PI 3.14159265
const std::string default_border_colour = "black";
const std::string init_border_colour = "red";

std::string make_wx_spline(wxPoint *points)
{
    double cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
    double x1, y1, x2, y2;
    std::string r;

    x1 = points[0].x;
    y1 = points[0].y;
    x2 = points[1].x;
    y2 = points[1].y;
    cx1 = (double)((x1 + x2) / 2);
    cy1 = (double)((y1 + y2) / 2);
    cx2 = (double)((cx1 + x2) / 2);
    cy2 = (double)((cy1 + y2) / 2);

    boost::format p1("M %1% %2% L %3% %4%");
    p1 % points[0].x % points[0].y % cx1 % cy1;
    r = boost::str(p1);

    x1 = x2;
    y1 = y2;
    x2 = points[2].x;
    y2 = points[2].y;
    cx4 = (double)(x1 + x2) / 2;
    cy4 = (double)(y1 + y2) / 2;
    cx3 = (double)(x1 + cx4) / 2;
    cy3 = (double)(y1 + cy4) / 2;
    boost::format p2(" C %1% %2% %3% %4% %5% %6%");
    p2 % cx2 % cy2 % cx3 % cy3 % cx4 % cy4;
    r += boost::str(p2);

    cx1 = cx4;
    cy1 = cy4;
    cx2 = (double)(cx1 + x2) / 2;
    cy2 = (double)(cy1 + y2) / 2;
    x1 = x2;
    y1 = y2;
    x2 = points[3].x;
    y2 = points[3].y;
    cx4 = (double)(x1 + x2) / 2;
    cy4 = (double)(y1 + y2) / 2;
    cx3 = (double)(x1 + cx4) / 2;
    cy3 = (double)(y1 + cy4) / 2;
    boost::format p3(" C %1% %2% %3% %4% %5% %6%");
    p3 % cx2 % cy2 % cx3 % cy3 % cx4 % cy4;
    r += boost::str(p3);

    boost::format p4(" L %1% %2%");
    p4 % points[3].x % points[3].y;
    r += boost::str(p4);

    return r;
}

export_to_svg::export_to_svg(wxString _filename, vector<node_svg> _nodes, vector<edge_svg> _edges, double _height, double _width) :
  filename(_filename) , nodes(_nodes), edges(_edges), height(_height), width(_width) {
}

bool export_to_svg::generate() {

  //Create the svg header
  svg_code  = "<?xml version=\"1.0\" standalone=\"no\"?>\n\n";
  svg_code += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n\n";

  boost::format header("<svg viewBox=\"0 0 %1% %2%\" width=\"100%%\" height=\"100%%\" version=\"1.1.\" \n");;
  header%width
        %height;

  svg_code += boost::str(header);
  svg_code += "xmlns=\"http://www.w3.org/2000/svg\">\n\n";

  // Draw edges first (body and arrow heads)
  for (unsigned int i = 0; i < edges.size(); i++) {

    /* Draw the splines that form the edge bodies */
    svg_code += "<path d=\"";
    svg_code += make_wx_spline(edges[i].spline_control_points);
    svg_code += "\" stroke=\"black\" stroke-width=\"1\" fill=\"none\"/>\n";

    /* Draw the arrowheads of the edges */
    boost::format arrow_f("<polygon points=\"%1% %2% %3% %4% %5% %6%\" fill=\"black\" stroke=\"black\"/>\n");
    arrow_f%edges[i].arrow_points[0].x
           %edges[i].arrow_points[0].y
           %edges[i].arrow_points[1].x
           %edges[i].arrow_points[1].y
           %edges[i].arrow_points[2].x
           %edges[i].arrow_points[2].y;



    svg_code += boost::str(arrow_f);
    
 
    /* Draw the edge labels */
    boost::format label_f("<text x =\"%1%\" y=\"%2%\" fill=\"rgb(%3%, %4%, %5%)\" font-size=\"12\">\n");
    label_f%edges[i].lbl_x
           %edges[i].lbl_y
           %edges[i].red
           %edges[i].green
           %edges[i].blue;
    
    svg_code += boost::str(label_f);
    svg_code += edges[i].lbl + "\n";
    svg_code += "</text>\n";
  }
  
  // Draw nodes
  for (unsigned int i = 0; i < nodes.size(); i++) {
    boost::format f("<circle cx=\"%1%\" cy=\"%2%\" r=\"%3%\" stroke=\"%4%\" stroke-width=\"%5%\" fill=\"rgb(%6%, %7%, %8%)\"/>\n");
    
    double node_x = nodes[i].x;
    double node_y = nodes[i].y;
    double radius = nodes[i].radius;
    
    int stroke_width;
    std::string stroke;

    if (nodes[i].num == 0) {
      stroke = init_border_colour;
      stroke_width = 2;
    }
    else {
      stroke = default_border_colour;
      stroke_width = 1;
    }
    int node_red = nodes[i].red;
    int node_green = nodes[i].green;
    int node_blue = nodes[i].blue;
    f%node_x
     %node_y
     %radius
     %stroke
     %stroke_width
     %node_red
     %node_green
     %node_blue;
    svg_code += boost::str(f);

    /* Draw node label onto node */
    string label = nodes[i].label;

    boost::format label_f("<text x=\"%1%\" y=\"%2%\" fill=\"black\" font-size=\"12\">\n");
    label_f%nodes[i].label_x%nodes[i].label_y;
    svg_code += boost::str(label_f);
    svg_code += label + "\n";
    svg_code += "</text> \n";
  }
  // End svg file
  svg_code += "</svg>";
  
  //Create the file
  wxTextFile svg_export(filename);

  if (svg_export.Exists()) {
    if (!svg_export.Open(filename)) {
      return false;
    }
    else {
      svg_export.Clear();
    }
  }
  else {
    if (!svg_export.Create(filename)) {
      return false;
    }
  }

  // Write code to file
  wxString svg_code_wx(svg_code.c_str(), wxConvLocal);
  svg_export.AddLine(svg_code_wx);
  
  svg_export.AddLine(wxEmptyString);

  if (!svg_export.Write()) {
    return false;
  }
  
  if (!svg_export.Close()) {
    return false;
  }

  return true; 
} 
  
