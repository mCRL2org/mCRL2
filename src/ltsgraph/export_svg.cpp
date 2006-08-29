#include "export_svg.h"

#include <fstream>
#include <iostream>

#include <boost/format.hpp>
const std::string default_border_colour = "black";
const std::string init_border_colour = "red";

export_to_svg::export_to_svg(const char* _filename, vector<node_svg> _nodes, vector<edge_svg> _edges) :
  filename(_filename) , nodes(_nodes), edges(_edges) {
}

bool export_to_svg::generate() {

  //Create the svg header
  svg_code  = "<?xml version=\"1.0\" standalone=\"no\"?>\n\n";
  svg_code += "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n\n";
  svg_code += "<svg width=\"100%\" height=\"100%\" version=\"1.1\" \n";
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
  }
  // End svg file
  svg_code += "</svg>";
  
  //Create the file
  ofstream file(filename);

  if (!file) {
    return false;
  }
 
  // Write code to file
  file << svg_code;

  if (!file) {
    return false;
  }

  return true;
} 
  
