// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file glcontext.h
/// \brief Implementation of context for OpenGL rendering canvas.

#ifndef GLCONTEXT_H_
#define GLCONTEXT_H_

#include <wx/glcanvas.h>

enum PrevContext { EMPTY, CONTEXT2D, CONTEXT3D };

class GLContext : public wxGLContext
{
  public:
    GLContext(wxGLCanvas *canvas);
    void set2DContext();
    void set3DContext();
  private:
    PrevContext prevContext;
};

#endif /* GLCONTEXT_H_ */
