// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file glcanvas.cpp
/// \brief Implementation of OpenGL rendering canvas.

#include "wx.hpp" // precompiled headers

#include "glcanvas.h"
#include <wx/dcclient.h>
#include <iostream>

#include "ids.h"

#ifdef __APPLE__
  #include <OpenGL/glu.h>
#else
  #include <GL/glu.h>
#endif

BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
  EVT_PAINT(GLCanvas::onPaint)
  EVT_SIZE(GLCanvas::onSize)
  EVT_ERASE_BACKGROUND(GLCanvas::onEraseBackground)

  EVT_ENTER_WINDOW(GLCanvas::onMouseEnter)
  EVT_KILL_FOCUS(GLCanvas::onMouseLeave)
  EVT_LEFT_DOWN(GLCanvas::onMouseLftDown)
  EVT_RIGHT_DOWN(GLCanvas::onMouseRgtDown)
  EVT_LEFT_UP(GLCanvas::onMouseLftUp)
  EVT_RIGHT_UP(GLCanvas::onMouseRgtUp)
  EVT_MOTION(GLCanvas::onMouseMove)
  EVT_MIDDLE_DOWN(GLCanvas::onMouseMidDown)
  EVT_MIDDLE_UP(GLCanvas::onMouseMidUp)
  EVT_MOUSEWHEEL(GLCanvas::onMouseWhl)
  EVT_LEFT_DCLICK(GLCanvas::onMouseDblClck)




END_EVENT_TABLE()

GLCanvas::GLCanvas(LTSGraph3d* app, wxWindow* parent,
                   const wxSize &size, int* attribList)
  : wxGLCanvas(parent, wxID_ANY, wxDefaultPosition, size, wxSUNKEN_BORDER,
               wxEmptyString, attribList)
{
  owner = app;
  displayAllowed = false;
  lookX = 0;
  lookY = 0;
  lookZ = 0;
  rotX = 0;
  rotY = 0;
  scaleFactor = 1.0;  
}

GLCanvas::~GLCanvas()
{
}

void GLCanvas::initialize()
{
  SetCurrent();
  glLoadIdentity();
  glGetFloatv(GL_MODELVIEW_MATRIX, currentModelviewMatrix);
  glShadeModel(GL_SMOOTH);
  glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearDepth(1.0);									
  glDepthFunc(GL_LESS);								
  glEnable(GL_DEPTH_TEST);							
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
  SwapBuffers();
  displayAllowed = true;
  visualizer->initFontRenderer();
}

void GLCanvas::setVisualizer(Visualizer *vis)
{
  visualizer = vis;
}

void GLCanvas::display()
{
  if (GetContext())
  {
    int width, height, depth;

    GetClientSize( &width, &height);
	depth = (width + height) / 2;
    // This is current context
    SetCurrent();

    // Cast to GLdouble for smooth transitions
    GLdouble aspect = (GLdouble)width / (GLdouble)height;

	double wwidth, wheight, wdepth;

    getSize(wwidth, wheight, wdepth);

    // Set up viewing volume
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	double rad = visualizer->getRadius() * getPixelSize() ;

	maxDepth = std::max(std::max((wdepth - 2 * rad), (wheight - 2 * rad)), (wwidth - 2 * rad));

	gluPerspective(45.0f, aspect, 0.1f, 2 * (lookZ + maxDepth + 0.1f));

//	gluLookAt(lookX,lookY,lookZ + 0.1f + maxDepth / 2,0,0,lookZ + 0.1f - maxDepth / 2,0,1,0);

    glMatrixMode( GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, width, height);

//	glTranslatef(lookX,lookY,-lookZ - 0.1f - maxDepth / 2);

	float dumtrx[16];
	Utils::genRotArbAxs(sqrt(rotX * rotX + rotY * rotY), rotX, rotY, 0, dumtrx);
	rotX = 0;
	rotY = 0;
	float dumtrx2[16];
	Utils::MultGLMatrices(dumtrx, currentModelviewMatrix, dumtrx2);

	for ( int i = 0; i < 12; i++)
		currentModelviewMatrix[i] = dumtrx2[i];

	currentModelviewMatrix[12] = lookX;
	currentModelviewMatrix[13] = lookY;
	currentModelviewMatrix[14] = -lookZ - 0.1f - maxDepth / 2;
	currentModelviewMatrix[15] = 1;

	double xl, yl, zl;
	xl = 0;
	yl = 0;
	zl = 100;
	GLfloat LightAmbient[]=		{ 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat LightDiffuse[]=		{ 0.8f, 0.8f, 0.8f, 1.0f };
	GLfloat LightPosition[]=	{ xl, yl, zl, 0.0f};
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);	
	glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);	
	GLfloat LightAmbient2[]=		{ 1.0f, 1.0f, 1.0f, 0.0f };
	GLfloat LightDiffuse2[]=		{ 0.5f, 0.5f, 0.5f, 0.6f };
	GLfloat LightPosition2[]=	{ xl, yl, zl, 0.0f};
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient2);		
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse2);		
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition2);	

	glLoadMatrixf(currentModelviewMatrix);

    double pS = getPixelSize();

    if (visualizer)
    {
      // Draw
      visualizer->visualize(wwidth, wheight, pS, false);
    }

/*	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
    glLoadIdentity();
	gluPerspective(45.0f, 1, 0.1f, 10.0f);
	glMatrixMode( GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();
	glViewport(0, 0, std::max(height, width) / 6, std::max(height, width) / 6);

	if (visualizer)
	{
		visualizer->drawCoorSystem();
	}

	glLoadIdentity();
    glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
*/

    glFinish();
    SwapBuffers();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
      std::cerr << "OpenGL error: " << gluErrorString(error) << std::endl;
    }
  }
}

void GLCanvas::onPaint(wxPaintEvent& /*event*/)
{
  wxPaintDC dc(this);
  display();
}

void GLCanvas::onSize(wxSizeEvent& /*event*/)
{
  reshape();
}

void GLCanvas::reshape()
{
  if(GetContext())
  {
    int width, height;
    SetCurrent();

    GetClientSize(&width, &height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0,0, width, height);
  }
}

void GLCanvas::onEraseBackground(wxEraseEvent& /*event*/)
{
}


void GLCanvas::getSize(
  double &width,
  double &height,
  double &depth)
/* (Based on method of the same name in Diagraphica's GLcanvas, the following
 * description derrives from that tool)
 * Returns viewport width and height in world coordinates.
 *
 * Before scaling, the viewport is set up such that the shortest side has length
 * 2 in world coordinates. Let ratio = width/height be the aspect ration of the
 * window and keep in mind that:
 *
 *  Size(viewport)   = Size(world)*scaleFactor
 *  So, Size(world)  = Size(viewport)/ScaleFactor
 *  So, Size(world)  = 2/ScaleFactor
 * There are 2 cases:
 * (1) If aspect > 1, the viewport is wider than tall
 *     So, the starting height was 2:
 *     world width      = ( aspect*2 ) / scaleFactor
 *     world height     = 2 / scaleFactor
 * (2) If aspect <= 1, the viewport is taller than wide
 *     So, the starting width was 2:
 *     world width      = 2 / scaleFactor;
 *     world height     = ( aspect*2 ) / scaleFactor
 * Depth is just added for ease of use in 3D spaces.
 */

{
  int widthViewPort;
  int heightViewPort;
  int depthViewPort;
  double aspect;

  // get pixel (device) sizes
  GetClientSize(
    &widthViewPort,
    &heightViewPort);
  depthViewPort = (widthViewPort + heightViewPort) / 2;
  aspect = (double) widthViewPort /  (double) heightViewPort;

  if (aspect > 1)
  {
    // width > height, so starting height was 2
    width = (aspect * 2.0) / (double) scaleFactor;
    height = 2.0 / (double) scaleFactor;
  }
  else
  {
    // height >= width, so starting width was 2
    width = 2.0 / (double) scaleFactor;
    height = ((1/aspect) * 2.0) / (double) scaleFactor;
  }
  depth = (width + height) / 2;
}

double GLCanvas::getPixelSize()
// TODO: Comment
{
  double result = 0.0;

  int widthPixels;
  int heightPixels;
  GetClientSize(
    &widthPixels,
    &heightPixels);

  double widthWorld;
  double heightWorld;
  double depthWorld;

  getSize(widthWorld, heightWorld, depthWorld);

  result = widthWorld * (1 / static_cast<double>(widthPixels));
  return result;
}

void GLCanvas::onMouseEnter(wxMouseEvent& /* event */)
{
  this->SetFocus();
}

void GLCanvas::onMouseLeave(wxFocusEvent& /* event */) {
  owner->deselect();
}

void GLCanvas::onMouseLftDown(wxMouseEvent& event)
{
  oldX = event.GetX();
  oldY = event.GetY();
  pickObjects(oldX, oldY, event);
  owner->dragObject();
  display();
}

void GLCanvas::onMouseLftUp(wxMouseEvent& /* evt */)
{
  owner->stopDrag();
  display();
}

void GLCanvas::onMouseRgtDown(wxMouseEvent& event)
{
  oldX = event.GetX();
  oldY = event.GetY();

  pickObjects(oldX, oldY, event);
  owner->lockObject();
  display();
}

void GLCanvas::onMouseRgtUp(wxMouseEvent& /*evt */)
{
}
void GLCanvas::onMouseDblClck(wxMouseEvent& /*evt */)
{
	lookX=0;
	lookY=0;
	lookZ=0;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, currentModelviewMatrix);
}
void GLCanvas::onMouseWhl(wxMouseEvent& event)
{
	lookZ -= float(event.GetWheelRotation())/(2400.0f);
	display();
}
void GLCanvas::onMouseMidDown(wxMouseEvent& event)
{
	oldX = event.GetX();
	oldY = event.GetY();
}
void GLCanvas::onMouseMidUp(wxMouseEvent& /*evt */)
{

}

void GLCanvas::onMouseMove(wxMouseEvent& event)
{
  if(event.Dragging() && (event.LeftIsDown() || event.MiddleIsDown()))
  {
    int x, y;
    GetPosition(&x, &y);
	int newX = static_cast<int>(event.GetX());
    int newY = static_cast<int>(event.GetY());
	if (event.LeftIsDown())
	{
		int width, height;
		GetClientSize(&width, &height);

	    owner->moveObject(oldX, oldY, newX, newY, width, height);

		if ((x < newX) && (newX < x + width)) {
		  oldX = newX;
		}
		if ((y < newY) && (newY < y + height)) {
		  oldY = newY;
		}
	}
	else if (event.MiddleIsDown())
	{
		rotX = 0.5f * (oldX - newX);
	    rotY = -0.5f * (oldY - newY);
	    oldX = newX;
	    oldY = newY;
	}
    display();
  }
  else {
    event.Skip();
  }
}


void GLCanvas::pickObjects(int x, int y, wxMouseEvent const& e)
{
  owner->deselect();


  if(GetContext())
  {
    GLuint selectBuf[512];
    GLint  hits = 0;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glSelectBuffer(512, selectBuf);
    // Swith to selection mode
    (void) glRenderMode(GL_SELECT);

    glInitNames();

    // Create new projection transformation
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Create picking region near cursor location
    gluPickMatrix((GLdouble) x,
                  (GLdouble)  viewport[3] - y,
                  1.0,
                  1.0,
                  viewport);

    // Get current size of canvas
    int width,height,depth;
    GetClientSize(&width,&height);
	depth = (width + height) / 2;

    GLdouble aspect = (GLdouble)width / (GLdouble)height;

    double wwidth, wheight, wdepth;
    getSize(wwidth, wheight, wdepth);

	double rad = visualizer->getRadius() * getPixelSize() ;

	maxDepth = std::max(std::max((wdepth - 2 * rad), (wheight - 2 * rad)), (wwidth - 2 * rad));

	gluPerspective(45.0f, aspect, 0.1f, 2 * (lookZ + maxDepth + 0.1f));

    glMatrixMode( GL_MODELVIEW);

    double pS = getPixelSize();
    visualizer->visualize(wwidth, wheight, pS, true);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);

    processHits(hits, selectBuf, e);
    reshape();
    display();
  }
}

void GLCanvas::processHits(const GLint hits, GLuint *buffer, wxMouseEvent const& e)
{
  // This method selects the object clicked.
  //
  // The buffer content per hit is encoded as follows:
  //  buffer[0]: The number of names on the name stack at the moment of the hit
  //  buffer[1]: The minimal depth of the object hit
  //  buffer[2]: The maximal depth of the object. We are certainly not
  //             interested in this.
  //  buffer[3]: The first identifier of the object picked.
  // (buffer[4]: The second identifier of the object picked.)
  int selectedObject[3]; // Identifier for the object picked
  selectedObject[0] = -1; // None selected
  GLuint names; // The number of names on the stack.
  bool stateSelected = false; // Whether we've hit a state
  bool transSelected = false; // Whether we've hit a transition

  // Choose the objects hit, and store it. Give preference to states, then
  // to transition handles, then to labels
  using namespace IDS;
  for(GLint j = 0; j < hits; ++j) {
    names = *buffer;
    ++buffer; // Buffer points to minimal z value of hit.
    ++buffer; // Buffer points to maximal z value of hit.
    ++buffer; // Buffer points to first name on stack
    GLuint objType = *buffer;

    for(GLuint k = 0; k < names; ++k) {
      if(!(stateSelected || transSelected) || objType == STATE) {
        selectedObject[k] = *buffer;
      }
      ++buffer;
	}

    stateSelected = objType == STATE;
    transSelected = objType == TRANSITION || objType == SELF_LOOP;
  }


  switch(selectedObject[0])
  {
    case TRANSITION: {
      owner->selectTransition(selectedObject[1], selectedObject[2]);
      break;
    }
    case IDS::SELF_LOOP:
    {
      owner->selectSelfLoop(selectedObject[1], selectedObject[2]);
      break;
    }
    case IDS::STATE:
    {
      if(!e.CmdDown()) {
        owner->selectState(selectedObject[1]);
      }
      if (e.Button(wxMOUSE_BTN_LEFT)) {
        owner->colourState(selectedObject[1]);
      }
      else {
        owner->uncolourState(selectedObject[1]);
      }
      break;
    }
    case IDS::LABEL:
    {
      owner->selectLabel(selectedObject[1], selectedObject[2]);
      break;
    }
    case IDS::SELF_LABEL:
    {
      owner->selectSelfLabel(selectedObject[1], selectedObject[2]);
      break;
    }
    default: break;
  }

  buffer = NULL;
}


double GLCanvas::getAspectRatio() const 
{
  int width, height;

  GetClientSize(&width, &height);

  return static_cast<double>(width) / static_cast<double>(height);
}

double GLCanvas::getMaxDepth() const
{
	return maxDepth;
}

void GLCanvas::getMdlvwMtrx(float * mtrx)
{
	for (int i = 0; i < 16; i++)
		mtrx[i] = currentModelviewMatrix[i];
}



