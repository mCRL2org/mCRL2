// Author(s): Johannes Altmanninger
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_UTILITIES_WORKAROUNDS_H
#define MCRL2_UTILITIES_WORKAROUNDS_H

#ifdef WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLKit/GLKMatrix4.h>
#else
#include <GL/glu.h>
#endif

#include <QMatrix4x4>
#include <QRect>
#include <QVector3D>
#include <cmath>

// these 3 macros replace glu functions deprecated on Mac OS with calls to
// Apple specific frameworks, so they should be removed once a better,
// cross-platform solution is adopted
#ifdef __APPLE__
#define gluOrtho2D(left, right, bottom, top)                                   \
  do                                                                           \
  {                                                                            \
    GLKMatrix4 mat =                                                           \
        GLKMatrix4MakeOrtho(left, right, bottom, top, -1.0f, 1.0f);            \
    glLoadMatrixf(mat.m);                                                      \
  } while (0)

#define gluPerspective(fovy, aspect, zNear, zFar)                              \
  do                                                                           \
  {                                                                            \
    GLKMatrix4 mat =                                                           \
        GLKMatrix4MakePerspective(fovy * M_PI / 180, aspect, zNear, zFar);     \
    glLoadMatrixf(mat.m);                                                      \
  } while (0)

#define gluPickMatrix(x, y, delX, delY, viewport)                              \
  do                                                                           \
  {                                                                            \
    glTranslatef((viewport[2] - 2 * (x - viewport[0])) / delX,                 \
                 (viewport[3] - 2 * (y - viewport[1])) / delY, 0);             \
    glScalef(viewport[2] / delX, viewport[3] / delY, 1.0);                     \
  } while (0)

#endif // __APPLE__

namespace mcrl2
{
namespace gui
{

// can be replaced once the required Qt version is >= 5.5
// see http://doc.qt.io/qt-5/qvector3d.html#project
QVector3D project(const QVector3D& obj, const QMatrix4x4& modelView,
                  const QMatrix4x4& projection, const QRect& viewport);
QVector3D unproject(const QVector3D& win, const QMatrix4x4& modelView,
                    const QMatrix4x4& projection, const QRect& viewport);
} // namespace gui
} // namespace mcrl2

#endif // MCRL2_UTILITIES_WORKAROUNDS_H
