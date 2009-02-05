// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsgraph.h
/// \brief Main application class.

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

#include "mcrl2/utilities/wx_tool.h"

class LTSGraph : public mcrl2::utilities::wx::tool< LTSGraph >
{
  friend class mcrl2::utilities::wx::tool< LTSGraph >;

  private:
    Graph *graph; // The labeled transition system (graph) that we work on

    GLCanvas *glCanvas;  // GLcanvas for visualisation
    Visualizer *visualizer; // Visualizer of objects
    MainFrame *mainFrame;  // Mainframe/central GUI.
    std::vector<LayoutAlgorithm*> algorithms; // The layout algorithms loaded.

    State* selectedState;
    Transition* selectedTransition;
    Transition* selectedLabel; // Label of transition selected should be moved

    std::string fileName;
    bool colouring;
    wxColour brushColour;

    bool parse_command_line(int argc, wxChar** argv);

  public:
    LTSGraph();

    bool DoInit();

    void openFile(std::string const &path);
    void display();

    void moveObject(double x, double y);
    void lockObject();
    void dragObject();
    void stopDrag();
    void deselect();
    void selectState(size_t state);
    void colourState(size_t state);
    void selectTransition(size_t state, size_t trans);
    void selectSelfLoop(size_t state, size_t trans);
    void selectLabel(size_t state, size_t trans);
    void selectSelfLabel(size_t state, size_t trans);

    // Setters
    void setLTSInfo(int is, int ns, int nt, int nl);
    void setRadius(int radius);
    void setTransLabels(bool value);
    void setStateLabels(bool value);
    void setCurves(bool value);
    void setBrushColour(wxColour colour);
    void setTool(bool isColour);

    // Getters
    Graph* getGraph();
    size_t getNumberOfAlgorithms() const;
    LayoutAlgorithm* getAlgorithm(size_t i) const;
    size_t getNumberOfObjects();
    std::string getFileName() const;
    int getRadius() const;
    double getAspectRatio() const;
    std::string getVersion();
    std::string getRevision();
};


#endif //LTSGRAPH_H
