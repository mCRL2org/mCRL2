#ifndef SPRING_LAYOUT_H
#define SPRING_LAYOUT_H
#include "layoutalgo.h"
#include "gltsgraph.h"

// This class defines the "spring layout" algorithm of the original LTSgraph
class SpringLayout : public LayoutAlgorithm 
{
  public:
    SpringLayout(GLTSGraph* owner);
    void setupPane(wxPanel* pane);
    void stop();



  private:
    int edgeStiffness;
    int nodeStrength;
    int naturalLength;
    bool stopOpti;
    bool stopped;

    wxPanel* pane;
    GLTSGraph* app;

    void layoutGraph(Graph* g);

    void onStart(wxCommandEvent& event);
    void onStop(wxCommandEvent& event);
    
    void onNodeStrength(wxScrollEvent& evt);
    void onEdgeStrength(wxScrollEvent& evt);
    void onTransLength(wxScrollEvent& evt);

    DECLARE_EVENT_TABLE()
};
#endif // SPRING_LAYOUT_H
