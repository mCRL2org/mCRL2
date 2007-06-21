#include "export_svg.h"

#include <fstream>
#include <wx/textfile.h> 
#include <wx/dc.h>
#include <math.h>
#include <boost/format.hpp>

#define PI 3.14159265
const std::string default_border_colour = "black";
const std::string init_border_colour = "red";
const double triangle_height = 10;
const double triangle_width = 5;

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
    /* Calculate spline control point from the given control point */
    double spline_control_x = (8 * edges[i].control_x - 
                             (edges[i].start_x + edges[i].end_x)) / 6;
    double spline_control_y = (8 * edges[i].control_y - 
                             (edges[i].start_y + edges[i].end_y)) / 6;

    /* Draw the splines that form the edge bodies */
    boost::format f("<path d=\"M %1% %2% Q %3% %4% %5% %6%\" stroke=\"black\" stroke-width=\"1\" fill=\"none\"/>\n");
    f%edges[i].start_x
     %edges[i].start_y
     %spline_control_x
     %spline_control_y
     %edges[i].end_x
     %edges[i].end_y;
    svg_code += boost::str(f);

    /* Draw the arrowheads of the edges */
    /* Calculate angle that the arrow needs to make (in radians) */
    /* Code taken from edge.cpp */
    double end_x = edges[i].end_x;
    double end_y = edges[i].end_y;
    double dist_cp_ed = sqrt( (spline_control_x - end_x) * (spline_control_x - end_x) + (spline_control_y - end_y) * (spline_control_y - end_y));
    double arrow_ratio = 0.0;
    if ( dist_cp_ed != 0) {
      arrow_ratio = edges[i].end_radius / dist_cp_ed;
    }
    else {
      arrow_ratio = 0;
    }
    double triangle_x = end_x - (end_x - spline_control_x) * arrow_ratio;
    double triangle_y = end_y - (end_y - spline_control_y) * arrow_ratio;
    double base_ratio = 0.0;
    if ( dist_cp_ed != 0) {
      base_ratio = (edges[i].end_radius + triangle_height) / dist_cp_ed;
    }
    else {
      base_ratio = 0;
    }
    const double triangle_base = 2.0;
    const double triangle_height = 7.0;
    double base_x = end_x - base_ratio * (end_x - spline_control_x);
    double base_y = end_y - base_ratio * (end_y - spline_control_y);
    double sinus_alpha = end_y;
    double cosinus_alpha = end_x;
    if (triangle_height != 0) {
      sinus_alpha = (base_y - end_y) / triangle_height;
      cosinus_alpha = (base_x - end_x) / triangle_height;
    }
    int arrow_base_high_x = (int) round(end_x + (cosinus_alpha * triangle_height -  .5 * triangle_base * sinus_alpha));
    int arrow_base_high_y = (int) round(end_y + (sinus_alpha * triangle_height + .5 * triangle_base * cosinus_alpha));
    int arrow_base_low_x  = (int) round(end_x + (cosinus_alpha * triangle_height +  .5 * triangle_base * sinus_alpha));
    int arrow_base_low_y  = (int) round(end_y + (sinus_alpha * triangle_height - .5 * triangle_base * cosinus_alpha));
    boost::format arrow_f("<polygon points=\"%1% %2% %3% %4% %5% %6%\" fill=\"black\" stroke=\"black\"/>\n");
    arrow_f%triangle_x
           %triangle_y
           %arrow_base_high_x
           %arrow_base_high_y
           %arrow_base_low_x
           %arrow_base_low_y;


/*    double alpha = 0;
    double end_y = edges[i].start_y;
    double end_x = edges[i].start_x;
    if (spline_control_x != end_x) {
      alpha = atan( (spline_control_y - end_y) / (spline_control_x - end_x));
    }
    else {
      alpha = .5 * PI;
    }
    if (spline_control_x >= end_x) {
      alpha = alpha + PI;
    }*/
    /* Draw the arrowhead, rotated over alpha */
/*    boost::format arrow_f("<polygon points=\"%1% %2% %3% %4% %3% %5%\" fill=\"black\" stroke=\"black\" transform=\"rotate(%6% %7% %8%)\"/>\n");
    arrow_f%(end_x - edges[i].end_radius)
           %end_y
           %(end_x - edges[i].end_radius - triangle_height)
           %(end_y - triangle_width / 2)
           %(end_y + triangle_width / 2)
           %(alpha * (180 / PI)) 
           %end_x
           %end_y;*/

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
  
