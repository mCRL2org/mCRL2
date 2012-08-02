// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./glcanvas.cpp

#include "wx.hpp" // precompiled headers

#include "glcanvas.h"

using namespace std;

int attribList[11] =
{
  WX_GL_RGBA,
  WX_GL_DOUBLEBUFFER,
  WX_GL_MIN_ALPHA,    8,
  WX_GL_MIN_RED,      8,
  WX_GL_MIN_BLUE,     8,
  WX_GL_MIN_GREEN,    8, 0
};


/// TODO: find out why this is necessary
#ifdef KeyPress
#undef KeyPress
#endif
#ifdef KeyRelease
#undef KeyRelease
#endif
#ifdef None
#undef None
#endif

// -- constructors and desctructor ----------------------------------


GLCanvas::GLCanvas(
  Mediator* m,
  wxWindow* parent,
  wxWindowID id)
  : wxGLCanvas(
    parent,
    id,
    wxDefaultPosition,
    wxDefaultSize,
    wxNO_FULL_REPAINT_ON_RESIZE |
    wxCLIP_CHILDREN,
    wxString(wxT("")),
    attribList),
  Colleague(m)
{
  // want to intercept arrow keys
  // IMPORTANT can only set style once, otherwise its overridden
  // I.e. when a new style is set somewhere else,
  //      arrow keys are NO LONGER intercepted
  SetWindowStyle(wxWANTS_CHARS /*| wxSUNKEN_BORDER*/);

  wxToolTip::Enable(true);
  wxToolTip::SetDelay(1);

  scaleFactor  = 1.0;
  xTranslation = 0.0;
  yTranslation = 0.0;

  handleMouseMotion = true;
}


GLCanvas::~GLCanvas()
{}


// -- set functions -------------------------------------------------


void GLCanvas::setScaleFactor(const double& f)
// Size(viewport)  = Size(world)*scaleFactor
// So, Size(world) = Size(viewport)/scaleFactor
{
  scaleFactor = f;
}


void GLCanvas::setXTranslation(const double& x)
{
  xTranslation = x;
}


void GLCanvas::setYTranslation(const double& y)
{
  yTranslation = y;
}


void GLCanvas::setTranslation(
  const double& x,
  const double& y)
{
  setXTranslation(x);
  setYTranslation(y);
}


void GLCanvas::enableMouseMotion()
{
  handleMouseMotion = true;
}


// -- Get functions -------------------------------------------------


double GLCanvas::getScaleFactor()
{
  return scaleFactor;
}


double GLCanvas::getXTranslation()
{
  return xTranslation;
}


double GLCanvas::getYTranslation()
{
  return yTranslation;
}


void GLCanvas::getTranslation(
  double& x,
  double& y)
{
  x = getXTranslation();
  y = getYTranslation();
}



void GLCanvas::disableMouseMotion()
{
  handleMouseMotion = false;
}


double GLCanvas::getWidth()
// Return viewport width in WORLD coordinates.  For more details, see
// GLCanvas::getSize().
{
  double result = 0;
  double width;
  double height;

  getSize(
    width,
    height);
  result = width;

  return result;
}


double GLCanvas::getHeight()
// Return viewport height in WORLD coordinates. For more details, see
// GLCanvas::getSize().
{
  double result;
  double width;
  double height;

  getSize(
    width,
    height);
  result = height;

  return result;
}


void GLCanvas::getSize(
  double& width,
  double& height)
// Return viewport width and height in WORLD coordinates. Before
// scaling, the viewport is set up such that the shortest side has
// length 2 in world coordinates. Let ratio = width/height be the
// aspect ratio of the window and keep in mind that:
//     Size(viewport)   = Size(world)*scaleFactor
//     So, Size(world)  = Size(viewport)/ScaleFactor
//     So, Size(world)  = 2/ScaleFactor
// There are 2 cases:
// (1) If aspect > 1, the viewport is wider than tall
//     So, the starting height was 2:
//     world width      = ( aspect*2 ) / scaleFactor
//     world height     = 2 / scaleFactor
// (2) If aspect <= 1, the viewport is taller than wide
//     So, the starting width was 2:
//     world width      = 2 / scaleFactor;
//     world height     = ( aspect*2 ) / scaleFactor
{
  int widthViewPort;
  int heightViewPort;
  double aspect;

  // get pixel (device) sizes
  GetClientSize(
    &widthViewPort,
    &heightViewPort);

  // calc aspect ratio
  aspect = (double)widthViewPort / (double)heightViewPort;

  // calc result
  if (aspect > 1)
  {
    // width > height, so starting height = 2
    width = (aspect*2.0) / (double)scaleFactor;
    height = 2.0 / (double)scaleFactor;
  }
  else
  {
    // height >= width, so starting width = 2
    width = 2.0 / (double)scaleFactor;
    height = ((1/aspect)*2.0) / (double)scaleFactor;
  }
}


double GLCanvas::getPixelSize()
// Return distance in WORLD coordinates of 1 pixel.
// Now, d(1 pixel in world)/w(world) = d(1 pixel)/w(in pixels)
// So,  d(1 pixel in world)          = w(world)*
//                                     ( d(1 pixel)/w(in pixels) )
{
  double result = 0.0;

  // get pixel (device) sizes
  int widthPixels;
  int heightPixels;
  GetClientSize(
    &widthPixels,
    &heightPixels);

  // get world sizes
  double widthWorld;
  double heightWorld;
  getSize(
    widthWorld,
    heightWorld);

  result = widthWorld * (1 / (double)widthPixels);

  return result;
}


void GLCanvas::getWorldCoords(
  const double& deviceX,
  const double& deviceY,
  double& worldX,
  double& worldY)
{
  if (IsShown() &&
      GetParent()->IsShown())
  {
    // this is current context
    SetCurrent();

    GLint viewport[4];
    GLdouble modelviewMatrix[16];
    GLdouble projMatrix[16];
    GLdouble tempZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);

    gluUnProject(
      (GLdouble)deviceX,                  // window x
      (GLdouble)(viewport[3] - deviceY),  // window y
      0.0,                                // window z
      modelviewMatrix,
      projMatrix,
      viewport,
      &worldX,
      &worldY,
      &tempZ);
  }
}


// -- drawing and interaction functions -----------------------------


void GLCanvas::clear()
// Default clear function, called by mediator if no visualizer has
// been assigned to the canvas.
{
  if (IsShown() &&
      GetParent()->IsShown() && GetContext())
  {
    // this is current context
    SetCurrent();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}


void GLCanvas::display()
// Set up and draw to the OpenGL canvas. The viewport window is set
// up such that the origin (0, 0) is at the center and such that the
// shortest side has a length of 2 in WORLD coordinates.
// Let ratio = width/height be the aspect ratio of the window.
// There are 2 cases:
// (1) If aspect > 1, the viewport should be wider than tall:
//     So, the height should be 2, and the width = aspect*2
//     E.g. if aspect = 1.5, height = 2 and width = 1.5*2 = 3
//     This implies: left   = aspect*(-1)
//                   right  = aspect*1
//                   bottom = -1
//                   top    = 1
// (2) If aspect <= 1, the viewport should be taller than wide:
//     So, the width should be 2, and the height = (1/aspect)*2
//     E.g. if aspect = 0.5, width = 2 and
//     height = (1/aspect)*2 = 2*2 = 4
//     This implies: left   = -1
//                   right  = 1
//                   bottom = (1/aspect)*(-1)
//                   top    = (1/apsect)*1
{
  if (!GetContext())
  {
    return;
  }
  // get current size of canvas
  int width, height;
  GetClientSize(&width, &height);

  // this is current context
  SetCurrent();

  // set up viewing volume
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // casting to GLdouble ensures smooth transitions
  GLdouble aspect = (GLdouble)width / (GLdouble)height;

  // specify clipping rectangle ( left, right, bottom, top )
  if (aspect > 1)
    // width > height
  {
    gluOrtho2D(aspect*(-1), aspect*1, -1, 1);
  }
  else
    // height >= width
  {
    gluOrtho2D(-1, 1, (1/aspect)*(-1), (1/aspect)*1);
  }

  // switch back to the default matrix mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // and set up viewport
  glViewport(0, 0, width, height);

  // draw
  mediator->handlePaintEvent(this);

  // swap buffers
  glFinish();
  SwapBuffers();

  // check for errors
  int error = glGetError();
  if (error != GL_NO_ERROR)
  {
    *mediator << "OpenGL error.\n";
    string s = string((char*)gluErrorString(error));
    *mediator << s;
    *mediator << "\n";
  }
}


void GLCanvas::showToolTip(const string& msg)
{
  tooltip = wxString(msg.c_str(), wxConvLocal);
}


void GLCanvas::clearToolTip()
{
  if (tooltip.IsEmpty() != true)
  {
    tooltip.Clear();
  }
}


void GLCanvas::onEvtPaint(wxPaintEvent& /*event*/)
// Get the device context for that part of the screen that needs to
// be repainted and redraw.
{
  if (IsShown() && GetContext())
  {
    // this is current context
    SetCurrent();
    wxPaintDC dc(this);
    display();

    SetToolTip(tooltip);
  }
}


void GLCanvas::onEvtSize(wxSizeEvent& /*event*/)
// Ensure that the viewport is adjusted to match the canvas when it
// is resized.
{
  // set this as current context
  if (GetParent()->IsShown())
  {
    // update visualizers using this canvas
    mediator->handleSizeEvent(this);

    // call to GetContext() necessary for some Linux distributions
    if (GetContext())
    {
      // get size of canvas
      int width, height;
      SetCurrent();

      // set up viewport to match canvas size
      GetClientSize(&width, &height);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glViewport(0, 0, width, height);
    }
  }
}


void GLCanvas::OnEraseBackground(wxEraseEvent& /*event*/)
// This function inherited from 'wxGLCanvas' must be overridden to
// allow flicker free repaints of the canvas.
{}


void GLCanvas::onMouseEvent(wxMouseEvent& event)
{
  QEvent::Type type = QEvent::None;
  if (event.Moving()) type = QEvent::MouseMove;
  if (event.Dragging()) type = QEvent::MouseMove;
  if (event.ButtonDown()) type = QEvent::MouseButtonPress;
  if (event.ButtonUp()) type = QEvent::MouseButtonRelease;
  if (event.ButtonDClick()) type = QEvent::MouseButtonDblClick;

  Qt::MouseButton button = Qt::NoButton;
  if (event.ButtonDown())
  {
    if (event.LeftDown()) button = Qt::LeftButton;
    if (event.MiddleDown()) button = Qt::MidButton;
    if (event.RightDown()) button = Qt::RightButton;
  }
  if (event.ButtonUp())
  {
    if (event.LeftUp()) button = Qt::LeftButton;
    if (event.MiddleUp()) button = Qt::MidButton;
    if (event.RightUp()) button = Qt::RightButton;
  }

  Qt::MouseButtons buttons = Qt::NoButton;
  if (event.LeftIsDown()) buttons = buttons | Qt::LeftButton;
  if (event.MiddleIsDown()) buttons = buttons | Qt::MidButton;
  if (event.RightIsDown()) buttons = buttons | Qt::RightButton;

  Qt::KeyboardModifiers modifiers = Qt::NoModifier;
  if (event.ShiftDown()) modifiers = modifiers | Qt::ShiftModifier;
  if (event.ControlDown()) modifiers = modifiers | Qt::ControlModifier;
  if (event.AltDown()) modifiers = modifiers | Qt::AltModifier;
  if (event.MetaDown()) modifiers = modifiers | Qt::MetaModifier;

  QMouseEvent mouseEvent(type, QPoint(event.GetX(), event.GetY()), button, buttons, modifiers);

  if (buttons != Qt::NoButton || handleMouseMotion)
  {
    SetCurrent();
    mediator->handleMouseEvent(this, &mouseEvent);
    Refresh();

    if (button == Qt::LeftButton && (type == QEvent::MouseButtonPress || type == QEvent::MouseButtonDblClick))
    {
      event.Skip();
    }
  } else {
    event.Skip();
  }
}

void GLCanvas::onWheelEvent(wxMouseEvent& event)
{

  Qt::MouseButtons buttons = Qt::NoButton;
  if (event.LeftIsDown()) buttons = buttons | Qt::LeftButton;
  if (event.MiddleIsDown()) buttons = buttons | Qt::MidButton;
  if (event.RightIsDown()) buttons = buttons | Qt::RightButton;

  Qt::KeyboardModifiers modifiers = Qt::NoModifier;
  if (event.ShiftDown()) modifiers = modifiers | Qt::ShiftModifier;
  if (event.ControlDown()) modifiers = modifiers | Qt::ControlModifier;
  if (event.AltDown()) modifiers = modifiers | Qt::AltModifier;
  if (event.MetaDown()) modifiers = modifiers | Qt::MetaModifier;

  QWheelEvent wheelEvent(QPoint(event.GetX(), event.GetY()), event.GetWheelRotation(), buttons, modifiers);

  SetCurrent();
  mediator->handleWheelEvent(this, &wheelEvent);
  Refresh();
}

void GLCanvas:: onEnterMouse(wxMouseEvent& /*event*/)
// This function simply sets the focus to this canvas. This allows key
// events to be caught and handled.
{
  // this is current context
  SetCurrent();
  SetFocus();
  mediator->handleMouseEnterEvent(this);
  Refresh();
}


void GLCanvas:: onLeaveMouse(wxMouseEvent& /*event*/)
// This function simply sets resets cursor to the default icon.
{
  // this is current context
  SetCurrent();
  mediator->handleMouseLeaveEvent(this);
  Refresh();
}


void GLCanvas::onKeyEvent(wxKeyEvent& event, bool down)
// In order to catch key events, this canvas must have the focus.
// This is achieved by having 'Canvas::onEnterMouse()' first setting
// the focus to this canvas.
{
  Qt::KeyboardModifiers modifiers = Qt::NoModifier;
  if (event.ShiftDown()) modifiers = modifiers | Qt::ShiftModifier;
  if (event.ControlDown()) modifiers = modifiers | Qt::ControlModifier;
  if (event.AltDown()) modifiers = modifiers | Qt::AltModifier;
  if (event.MetaDown()) modifiers = modifiers | Qt::MetaModifier;

  QEvent::Type type = (down ? QEvent::KeyPress : QEvent::KeyRelease);

  int key = event.GetKeyCode();

  QKeyEvent keyEvent(type, key, modifiers);

  SetCurrent();
  mediator->handleKeyEvent(this, &keyEvent);
  event.Skip();

  Refresh();
}

void GLCanvas::onKeyDown(wxKeyEvent& event)
{
  onKeyEvent(event, true);
}

void GLCanvas::onKeyUp(wxKeyEvent& event)
{
  onKeyEvent(event, false);
}


// -- implement event table -----------------------------------------


BEGIN_EVENT_TABLE(GLCanvas, wxGLCanvas)
  EVT_PAINT(GLCanvas::onEvtPaint)
  EVT_SIZE(GLCanvas::onEvtSize)

  // prevent flicker on repaint
  EVT_ERASE_BACKGROUND(GLCanvas::OnEraseBackground)

  // mouse interaction
  EVT_LEFT_DOWN(GLCanvas::onMouseEvent)
  EVT_LEFT_UP(GLCanvas::onMouseEvent)
  EVT_LEFT_DCLICK(GLCanvas::onMouseEvent)
  EVT_RIGHT_DOWN(GLCanvas::onMouseEvent)
  EVT_RIGHT_UP(GLCanvas::onMouseEvent)
  EVT_RIGHT_DCLICK(GLCanvas::onMouseEvent)
  EVT_MOTION(GLCanvas::onMouseEvent)
  EVT_MOUSEWHEEL(GLCanvas::onWheelEvent)
  EVT_ENTER_WINDOW(GLCanvas::onEnterMouse)
  EVT_LEAVE_WINDOW(GLCanvas::onLeaveMouse)

  // keyboard interaction
  EVT_KEY_DOWN(GLCanvas::onKeyDown)
  EVT_KEY_UP(GLCanvas::onKeyUp)
END_EVENT_TABLE()


// -- end -----------------------------------------------------------
