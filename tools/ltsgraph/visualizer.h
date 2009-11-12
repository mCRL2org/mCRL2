// Author(s): Carst Tankink and Ali Deniz Aladagli
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

#ifndef LTSGRAPH3D_H
  #include "ltsgraph3d.h"
#else
  class LTSGraph3d;
#endif
#include "state.h"
#include "mcrl2/utilities/font_renderer.h"

#ifndef __glu_h__
#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif
#endif

class Visualizer
{
  public:
    Visualizer(LTSGraph3d* app);
    ~Visualizer();

    void visualize(double width, double height, double pixelSize,
                   bool inSelectMode, bool enabled3D);

    void initFontRenderer();

    // Setters
    void setWidth(double width);
    void setHeight(double height);
    void setPixelSize(double size);
    void setRadius(int radius);
    void setCurves(bool value);
    void setTransLabels(bool value);
    void setStateLabels(bool value);
    void drawCoorSystem();

    // Getters
    int getRadius() const;
  private:
    LTSGraph3d* owner;
    double pixelSize; // Pixel size in world coordinates.
    double width; // Canvas width
    double height; // Canvas height
    double depth;
    int radius;
    bool showHandles;
    bool showTransLabels;
    bool showStateLabels;
    bool showStateVector;
    bool draw3d;

    GLUquadricObj *quadratic;	

    void drawStates(bool inSelectMode);
    // Draws a single state s
    void drawState(State* s);

    // Draws a single transition, from its in to its out transition
    void drawTransition(Transition* t, size_t trid, bool inSelectMode);

  	// Draws the transition label
	  void drawTransLabel(Transition* t, size_t trid, bool inSelectMode);

	  // Draws the state label and vector
	  void drawStateText(State* s);

    // Draws a self-loop, a transition from a state to itself
    void drawSelfLoop(Transition* t, size_t trid, bool inSelectMode);

	  // Draws an arrow head in 3d
    void drawArrowHead3d(double baseLength);

	  // Draws an arrow head
	  void drawArrowHead(double baseLength);

    mcrl2::utilities::wx::font_renderer* fr;
};

#endif //VISUALIZER_/H
