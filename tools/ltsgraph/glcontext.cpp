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

#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

GLContext::GLContext(wxGLCanvas *canvas)
          : wxGLContext(canvas)
{
    SetCurrent( *canvas );
}

void GLContext::set2DContext(){
	if(prevContext != CONTEXT2D ){
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    // Reset Model perspective
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		prevContext = CONTEXT2D;
	}
}

void GLContext::set3DContext(){
	if(prevContext != CONTEXT3D ){
		glShadeModel(GL_SMOOTH);
		glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
		glClearDepth(1.0);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Setup scene lights */
		double xl, yl, zl;
		xl = 0;
		yl = 0;
		zl = 100;
		GLfloat LightAmbient[]=   { 0.2f, 0.2f, 0.2f, 0.2f };
		GLfloat LightDiffuse[]=   { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat LightPosition[]=  { xl, yl, zl, 0.0f};
		glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);

		glColorMaterial(GL_FRONT,GL_AMBIENT);
		glEnable(GL_COLOR_MATERIAL);

		prevContext = CONTEXT3D;
	}
}
