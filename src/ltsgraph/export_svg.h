#ifndef svg_h
#define svg_h

#include <string>
#include <vector>

using namespace std;

typedef struct {
  unsigned int num; // Identifier of the node

  double x, y; // Position of center
  double radius; //Radius of node

  std::string label; // Node label text

  int red, green, blue; // Node colour RGB values
} node_svg;

typedef struct {
  double start_x, start_y; // The position of the edge start
  double end_x, end_y; // The position of the edge end.
  double control_x, control_y; // The position of the curve control

  std::string lbl; // The edge's label.
  double lbl_x, lbl_y; // The position of the label.
  int red, green, blue; // Label colour RGB values
} edge_svg;


class export_to_svg {
  public:
    export_to_svg(const char* _filename, vector<node_svg> _nodes, 
                vector<edge_svg> _edges);
    bool generate();

  private:
    const char * filename;
    string svg_code;

    vector<node_svg> nodes;
    vector<edge_svg> edges;

};


#endif //svg_h

