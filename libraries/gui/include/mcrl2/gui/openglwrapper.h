#ifndef MCRL2_UTILITIES_OPENGLWRAPPER_H
#define MCRL2_UTILITIES_OPENGLWRAPPER_H

#include <cmath>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <GLKit/GLKMatrix4.h>

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

#else
#include <GL/glu.h>
#endif // __APPLE__

#endif // MCRL2_UTILITIES_OPENGLWRAPPER_H
