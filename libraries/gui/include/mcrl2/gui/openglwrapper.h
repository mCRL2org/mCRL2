#ifndef MCRL2_UTILITIES_OPENGLWRAPPER_H
#define MCRL2_UTILITIES_OPENGLWRAPPER_H

namespace OpenGLWrapper {

#ifdef WIN32
#  include <windows.h>
#endif
#ifdef __APPLE__
#  include <OpenGL/glu.h>
#include <GLKit/GLKMatrix4.h>
#else
#  include <GL/glu.h>
#endif

void gluOrtho2D(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top)
{
#ifdef __APPLE__
  GLKMatrix4 mat = GLKMatrix4MakeOrtho(left, right, bottom, top, -1.0f, 1.0f);
  glLoadMatrixf(mat.m);
#else
  ::gluOrtho2D(left, right, bottom, top);
#endif
}

void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble
        zFar)
{
#ifdef __APPLE__
  GLKMatrix4 mat = GLKMatrix4MakePerspective(fovy, aspect, zNear, zFar);
  glLoadMatrixf(mat.m);
#else
  ::gluPerspective(fovy, aspect, zNear, zFar);
#endif
}

void gluPickMatrix(GLdouble x, GLdouble y, GLdouble delX, GLdouble delY,
        GLint viewport[4])
{
#ifdef __APPLE__
  glTranslatef((viewport[2] - 2 * (x - viewport[0])) / delX,
               (viewport[3] - 2 * (y - viewport[1])) / delY,
               0);
  glScalef(viewport[2] / delX, viewport[3] / delY, 1.0);
#else
  ::gluPickMatrix(x, y, delX, delY, viewport);
#endif
}

} // namespace OpenGLWrapper

#endif // MCRL2_UTILITIES_OPENGLWRAPPER_H
