// Author(s): Aad Mathijssen and Hannes Pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visutils.h

// ---------------------------------
//
// This function provides utility
//    functions for drawing with
//    OpenGL. Global variables
//    such as background color are
//    also defined. Functions are
//    static, so no objects need
//    to be created to invoke them
//    on.
//
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#ifndef VISUTILS_H
#define VISUTILS_H


#ifdef __APPLE__
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
#else
  #ifdef WIN32
    #include <windows.h>
  #endif
  #include <GL/gl.h>
  #include <GL/glu.h>
#endif


// Used by VisUtils
//	to specify colors
struct Color
{
  double red;
  double green;
  double blue;
  double alpha;
};


class VisUtils
{
public:
  static void clear();
  static void drawLiftNA( int row,
                          int col,
                          int side,
                          int numLevels,
                          int liftLevel );
  static void drawLiftOccupied( int row,
                                int col,
                                int side,
                                int numLevels,
                                int liftLevel );
  static void drawLiftFree( int row,
                            int col,
                            int side,
                            int numLevels,
                            int liftLevel );
  static void drawHalfBayOccupied( int row,
                                   int col,
                                   int side );
  static void drawHalfBayFree( int row,
                               int col,
                               int side );
  static void drawHalfBayNA( int row,
                             int col,
                             int side );
  static void drawTiltedShuttle( int row,
                                 int col,
                                 int side );
  static void drawWalls( int rows,
                         int cols );

  static double getColInterval();
  static double getRowInterval();
private:
  static void drawHalfBay( int row,
                           int col,
                           int side,
                           bool avail );
  static void drawLift( int row,
                        int col,
                        int side,
                        int numLevels,
                        int liftLevel );

  static double colInterval;
  static double rowInterval;
  static Color clearColor;
  static Color occupiedColor;
  static Color freeColor;
  static Color naColor;
  static Color liftOccupiedColor;
  static Color liftFreeColor;
  static Color tiltedShuttleColor;
  static Color gridColor;
};


#endif // VISUTILS_H
