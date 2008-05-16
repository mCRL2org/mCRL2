// Author(s): Didier Le Lann, Carst Tankink, Muck van Weerdenburg and Jeroen van der Wulp
// Copyright: see the accompanying file COPYING.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// LICENSE_1_0.txt)
//
/// \file ./export_svg.h

#ifndef svg_h
#define svg_h

#include <string>
#include <vector>
#include <wx/string.h>
#include <wx/gdicmn.h>
using namespace std;

typedef struct {
  unsigned int num; // Identifier of the node

  double x, y; // Position of center
  double radius; //Radius of node


  std::string label; // Node label text
  double label_x, label_y; //Node label position

  int red, green, blue; // Node colour RGB values
} node_svg;

typedef struct {
  double start_x, start_y; // The position of the edge start
  double end_x, end_y; // The position of the edge end.
  double end_radius; // Radius of the node that forms the end of the edge

  wxPoint spline_control_points[8];
  wxPoint arrow_points[6];

  std::string lbl; // The edge's label.
  double lbl_x, lbl_y; // The position of the label.
  int red, green, blue; // Label colour RGB values
} edge_svg;


class export_to_svg {
  public:
    export_to_svg(wxString _filename, vector<node_svg> _nodes, 
                vector<edge_svg> _edges, double _height, double _width);
    bool generate();

  private:
    wxString filename;
    string svg_code;

    vector<node_svg> nodes;
    vector<edge_svg> edges;

    double height;
    double width;
};


#endif //svg_h

