// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualizer.h
/// \brief Visualization functions.

#ifndef VISUALIZER_H
#define VISUALIZER_H

#ifndef LTSGRAPH_H
  #include "ltsgraph.h"
#else
  class LTSGraph;
#endif
#include "state.h"
#include "mcrl2/utilities/font_renderer.h"
class Visualizer
{
  public:
    Visualizer(LTSGraph* app);
    ~Visualizer();
    
    void visualize(double width, double height, double pixelSize, 
                   bool inSelectMode);

    void initFontRenderer();

    // Setters
    void setWidth(double width); 
    void setHeight(double height); 
    void setPixelSize(double size);
    void setRadius(int radius);
    void setCurves(bool value);
    void setTransLabels(bool value);
    void setStateLabels(bool value);
    // Getters
    int getRadius() const;
  private:
    LTSGraph* owner;
    double pixelSize; // Pixel size in world coordinates.
    double width; // Canvas width
    double height; // Canvas height
    int radius;
    bool showHandles;
    bool showTransLabels;
    bool showStateLabels;


    void drawStates(bool inSelectMode);
    // Draws a single state s
    void drawState(State* s);

    // Draws a single transition, from its in to its out transition
    void drawTransition(Transition* t, size_t trid, bool inSelectMode);

    // Draws an arrow head
    void drawArrowHead(double baseLength);


    // Draws a self-loop, a transition from a state to itself
    void drawSelfLoop(Transition* t, size_t trid, bool inSelectMode);

    mcrl2::utilities::font_renderer* fr;
};

#endif //VISUALIZER_/H
