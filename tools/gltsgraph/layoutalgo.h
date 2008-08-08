#ifndef LAYOUT_ALGO_H
#define LAYOUT_ALGO_H

#include <wx/panel.h>
#include <wx/event.h>

#ifndef GRAPH_H
  #include "graph.h"
#else
  class Graph;
#endif


// Defines an interface class for layout algorithms. This should make it easier
// to experiment with different layout algorithms. (e.g. implement a simulated
// annealing type of algorithm

class LayoutAlgorithm : public wxEvtHandler
{
  public:
    virtual ~LayoutAlgorithm() {};

    // setupPane method. All algorithms define their own settings pane, and 
    // manage the appropriate callback functions. 
    virtual void setupPane(wxPanel* pane) = 0;
    virtual void layoutGraph(Graph* graph) = 0;
    virtual void stop() = 0;
};



#endif // LAYOUT_ALGO_H
