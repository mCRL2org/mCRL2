#ifndef edge_h
#define edge_h

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wxprec.h"
#include <wx/dcps.h>
#include "node.h"
#include <string>

#define FONT_SIZE 9
#define POS_EDGE_LBL_X -10.0
#define POS_EDGE_LBL_Y -10.0

using namespace std;

class edge
{
public:
  // Constructors
  edge();
  edge(Node* _N1, Node* _N2, wxString _Slbl);

  // Paint event handler
  void on_paint(wxDC * ptrDC);

  // End points of edge (n2 is the end with the arrow
  Node* get_n1();
  Node* get_n2();

  // Returns the transition label
  string get_lbl();
  void set_label_text(wxString new_text);

  // Returns the x and y positions of the nodes and the spline control
  // point
  double get_x_pos1();
  double get_y_pos1();

  double get_x_pos2();
  double get_y_pos2();

  double get_x_control(); // Returns absolute (DC) coordinates of control point.
  double get_y_control();

  // Returns wether or not the edge label is visible
  // TODO (CT): Coding standard
  bool LabelVisible();

  // Toggles wether or not the edge label is visible
  // TODO (CT): Coding standard
  void ShowLabels();
  void HideLabels();

  void set_label_selected(bool selection_value);

  // Allows setting and resetting of control point
  void set_control(double new_x, double new_y);
  void reset_control();

  void set_control_selected(bool selection_value);
  void set_control_visible(bool selection_value);

  wxColour get_label_colour();

  void set_label_colour(wxColour new_colour);

  double get_label_lower_x();
  double get_label_lower_y();
  double get_label_higher_x();
  double get_label_higher_y();

  void set_label_x(double new_value);
  void set_label_y(double new_value);
private:
  // Start and end nodes.
  // TODO (CT): Coding standard
  Node* N1; 
  Node* N2;

  // Edge label
  wxString lbl;

  // Control point for edge spline
  double control_point_alpha; // Relative angle to line N1, N2
  double control_point_dist; // Distance to N1

  // Status of label visibility
  // TODO (CT): Coding standard
  bool labelsVisible;
  bool label_selected;

  wxColour label_colour;

  // Status of control point visibility
  bool control_point_visible;
  bool control_selected;

  double label_x;
  double label_y;
  double label_higher_x;
  double label_higher_y;
    

};

#endif //edge_h
