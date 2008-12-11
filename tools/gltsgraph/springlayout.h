// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file springlayout.h
/// \brief Spring embedder algorithm declaration.

#ifndef SPRING_LAYOUT_H
#define SPRING_LAYOUT_H
#include "layoutalgo.h"
#include "ltsgraph.h"

// This class defines the "spring layout" algorithm of the original LTSgraph
class SpringLayout : public LayoutAlgorithm 
{
  public:
    SpringLayout(LTSGraph* owner);
    void setupPane(wxPanel* pane);
    void stop();



  private:
    int edgeStiffness;
    int nodeStrength;
    int naturalLength;
    bool stopOpti;
    bool stopped;

    wxPanel* pane;
    wxButton* optimizeBtn;
    wxButton* stopBtn;
    LTSGraph* app;

    void layoutGraph(Graph* g);

    void onStart(wxCommandEvent& event);
    void onStop(wxCommandEvent& event);
    
    void onNodeStrength(wxScrollEvent& evt);
    void onEdgeStrength(wxScrollEvent& evt);
    void onTransLength(wxScrollEvent& evt);

    DECLARE_EVENT_TABLE()
};
#endif // SPRING_LAYOUT_H
