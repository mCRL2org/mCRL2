// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file glcontext.cpp
/// \brief Implementation of context for OpenGL rendering canvas.

#include "wx/wx.h"
#include "glcontext.h"
#include "rgb_color.h"
#include "settings.h"


#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

GLContext::GLContext(wxGLCanvas *canvas, Settings *settings)
          : wxGLContext(canvas)
{
    SetCurrent( *canvas );

    /* Setup scene parameters */
    GLfloat gray[] = { 0.35f,0.35f,0.35f,1.0f };
    GLfloat light_pos[] = { 50.0f,50.0f,50.0f,1.0f };
    glLightfv(GL_LIGHT0,GL_AMBIENT,gray);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,gray);
    glLightfv(GL_LIGHT0,GL_POSITION,light_pos);

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);

    GLfloat light_col[] = { 0.2f,0.2f,0.2f };
    glMaterialfv(GL_FRONT,GL_SPECULAR,light_col);
    glMaterialf(GL_FRONT,GL_SHININESS,8.0f);
    glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    RGB_Color c = settings->getRGB(BackgroundColor);
    glClearColor(c.red() / 255.0f, c.green() / 255.0f, c.blue() / 255.0f, 1.0f);
    glClearDepth(1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


}
