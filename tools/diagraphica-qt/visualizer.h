// Author(s): A.J. (Hannes) Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./visualizer.h

#ifndef VISUALIZER_H
#define VISUALIZER_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#endif
#include <cstddef>
#include "colleague.h"
#include "glcanvas.h"
#include "graph.h"
#include "visutils.h"

class Visualizer : public Colleague
{
  public:
    // -- constructors and destructor -------------------------------
    Visualizer(
      Mediator* m,
      Graph* g,
      GLCanvas* c);
    virtual ~Visualizer();

    // -- set functions ---------------------------------------------
    virtual void setClearColor(
      const double& r,
      const double& g,
      const double& b);

    // -- helper functions ------------------------------------------
    virtual void printMouseVariables();

    // -- visualization functions -----------------------------------
    virtual void visualize(const bool& inSelectMode) = 0;
    virtual void setGeomChanged(const bool& flag);
    virtual void setDataChanged(const bool& flag);
    /*
    virtual void animate() = 0;
    */

    // -- event handlers --------------------------------------------
    virtual void handleSizeEvent();

    virtual void handleMouseLftDownEvent(
      const int& x,
      const int& y);
    virtual void handleMouseLftUpEvent(
      const int& x,
      const int& y);
    virtual void handleMouseLftDClickEvent(
      const int& x,
      const int& y);
    virtual void handleMouseRgtDownEvent(
      const int& x,
      const int& y);
    virtual void handleMouseRgtUpEvent(
      const int& x,
      const int& y);
    virtual void handleMouseRgtDClickEvent(
      const int& x,
      const int& y);
    virtual void handleMouseMotionEvent(
      const int& x,
      const int& y);
    virtual void handleMouseWheelIncEvent(
      const int& x,
      const int& y);
    virtual void handleMouseWheelDecEvent(
      const int& x,
      const int& y);
    virtual void handleMouseEnterEvent();
    virtual void handleMouseLeaveEvent();
    virtual void handleKeyDownEvent(const int& keyCode);
    virtual void handleKeyUpEvent(const int& keyCode);

  protected:
    // -- protected utility functions -------------------------------
    virtual void clear();
    virtual void initMouse();

    virtual void startSelectMode(
      GLint hits,
      GLuint selectBuf[],
      double pickWth,
      double pickHgt);
    virtual void finishSelectMode(
      GLint hits,
      GLuint selectBuf[]);

    void genCharTex();
    void genCushTex();

    // -- hit detection ---------------------------------------------
    virtual void processHits(
      GLint hits,
      GLuint buffer[]) = 0;

    // -- mouse -----------------------------------------------------
    enum
    {
      MSE_BUTTON_UP,
      MSE_BUTTON_DOWN,
      MSE_SIDE_LFT,
      MSE_SIDE_MID,
      MSE_SIDE_RGT,
      MSE_CLICK_SINGLE,
      MSE_CLICK_DOUBLE,
      MSE_DRAG_TRUE,
      MSE_DRAG_FALSE
    };
    int mouseButton;
    int mouseSide;
    int mouseClick;
    int mouseDrag;
    double xMouseDragBeg;
    double yMouseDragBeg;
    double xMouseCur;
    double yMouseCur;
    double xMousePrev;
    double yMousePrev;
    int keyCodeDown;

    bool showMenu;

    // -- data members ----------------------------------------------
    ColorRGB  clearColor;

    Graph*    graph;  // association
    GLCanvas* canvas; // association

    // -- flags -----------------------------------------------------
    bool geomChanged; // canvas resized
    bool dataChanged; // data has changed

    // -- character textures ----------------------------------------
    bool    texCharOK;
    GLuint  texCharId[CHARSETSIZE];
    GLubyte texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH];

    // -- cushion texture -------------------------------------------
    bool    texCushOK;
    GLuint  texCushId;
    float   texCush[CUSHSIZE];
};

#endif

// -- end -----------------------------------------------------------
