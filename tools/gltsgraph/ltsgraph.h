#ifndef LTSGRAPH_H
#define LTSGRAPH_H

#ifndef MAINFRAME_H
  #include "mainframe.h"
#else
  //Forward declaration
  class MainFrame;
#endif

#include "graph.h"
#include "glcanvas.h"
#include "visualizer.h"
#include "layoutalgo.h"

#include <wx/app.h>

class LTSGraph : public wxApp
{
  private:
    Graph *graph; // The labeled transition system (graph) that we work on

    GLCanvas *glCanvas;  // GLcanvas for visualisation
    Visualizer *visualizer; // Visualizer of objects
    MainFrame *mainFrame;  // Mainframe/central GUI.
    std::vector<LayoutAlgorithm*> algorithms; // The layout algorithms loaded.

    void printHelp(std::string const &appName);

    State* selectedState;
    Transition* selectedTransition;

  public:
    virtual int OnExit();   
    virtual bool OnInit();
    
    void openFile(std::string const &path);
    void display();

    void moveObject(double x, double y);
    void lockObject();
    void deselect();
    void selectState(size_t state);
    void selectTransition(size_t state, size_t trans);
    void selectSelfLoop(size_t state, size_t trans);

    // Setters
    void setLTSInfo(int is, int ns, int nt, int nl);
    
    // Getters
    Graph* getGraph();
    size_t getNumberOfAlgorithms() const;
    LayoutAlgorithm* getAlgorithm(size_t i) const;
    size_t getNumberOfObjects();

};


#endif //LTSGRAPH_H
