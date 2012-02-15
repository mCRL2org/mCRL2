// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "wx.hpp" // precompiled headers

#include <iostream>
#include <wx/image.h>
#include "mcrl2/utilities/logger.h"
#include "glcanvas.h"
#include "ids.h"
#include "icons/zoom_cursor.xpm"
#include "icons/zoom_cursor_mask.xpm"
#include "icons/pan_cursor.xpm"
#include "icons/pan_cursor_mask.xpm"
#include "icons/rotate_cursor.xpm"
#include "icons/rotate_cursor_mask.xpm"
#include "mediator.h"
#include "settings.h"
#include "tr/tr.h"
#include "visualizer.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

using namespace IDs;

BEGIN_EVENT_TABLE(GLCanvas,wxGLCanvas)
  EVT_MOTION(GLCanvas::onMouseMove)
  EVT_ENTER_WINDOW(GLCanvas::onMouseEnter)
  EVT_LEFT_DOWN(GLCanvas::onMouseDown)
  EVT_LEFT_UP(GLCanvas::onMouseUp)
  EVT_LEFT_DCLICK(GLCanvas::onMouseDClick)
  EVT_RIGHT_DOWN(GLCanvas::onMouseDown)
  EVT_RIGHT_UP(GLCanvas::onMouseUp)
  EVT_MIDDLE_DOWN(GLCanvas::onMouseDown)
  EVT_MIDDLE_UP(GLCanvas::onMouseUp)
  EVT_MOUSEWHEEL(GLCanvas::onMouseWheel)
  EVT_PAINT(GLCanvas::onPaint)
  EVT_SIZE(GLCanvas::onSize)
  EVT_ERASE_BACKGROUND(GLCanvas::onEraseBackground)
END_EVENT_TABLE()

GLCanvas::GLCanvas(Mediator* owner,wxWindow* parent,Settings* ss,
                   const wxSize& size,int* attribList)
  : wxGLCanvas(parent,wxID_ANY,wxDefaultPosition,size,wxSUNKEN_BORDER,
               wxT(""),attribList), simReader(NULL)
{
  mediator = owner;
  settings = ss;
  settings->subscribe(BackgroundColor,this);
  displayAllowed = false;
  collectingData = false;
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector = Vector3D(0.0f, 0.0f, 0.0f);
  startPosZ = 0.0f;
  farPlane = 0.0f;
  nearPlane = 1.0f;
  lightRenderMode = false;
  simulating = false;
  setActiveTool(myID_SELECT);
  selectedType = PICKNONE;
}

void GLCanvas::initialize()
{
  SetCurrent();

  glLoadIdentity();

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
  glClearColor(c.red() / 255.0f, c.green() / 255.0f, c.blue() / 255.0f,
               1.0f);
  glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers();
  displayAllowed = true;
}

void GLCanvas::getMaxViewportDims(int* w,int* h)
{
  GLint dims[2];
  glGetIntegerv(GL_MAX_VIEWPORT_DIMS,dims);
  *w = int(dims[0]);
  *h = int(dims[1]);
}

void GLCanvas::resetView()
{
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector = Vector3D(0.0f, 0.0f, 0.0f);
  if (visualizer != NULL)
  {
    float sw,sh;
    visualizer->computeBoundsInfo(sw,sh);
    startPosZ = 0.866f*sh + sw + nearPlane;
    farPlane = startPosZ + 2*sw;
  }
  reshape();
  display();
}

void GLCanvas::setVisualizer(Visualizer* vis)
{
  visualizer = vis;
}

void GLCanvas::disableDisplay()
{
  displayAllowed = false;
}

void GLCanvas::enableDisplay()
{
  displayAllowed = true;
}

void GLCanvas::notify(SettingID s)
{
  if (s == BackgroundColor)
  {
    RGB_Color c = settings->getRGB(BackgroundColor);
    glClearColor(c.red() / 255.0f, c.green() / 255.0f, c.blue() /
                 255.0f, 1.0f);
  }
}

void GLCanvas::setActiveTool(int t)
{
  activeTool = t;
  currentTool = t;
  setMouseCursor();
}

void GLCanvas::display(bool coll_caller, bool selecting)
{
  // coll_caller indicates whether the caller of display() is the
  // getPictureData() method. While collecting data, only this method is allowed
  // to call display(); else the collected data may be corrupted.
  if (collectingData && !coll_caller)
  {
    return;
  }

  // next check is for preventing infinite recursive calls to display(), which
  // happened on the Mac during startup of the application
  if (displayAllowed)
  {
    displayAllowed = false;
    if (!collectingData)
    {
      mediator->notifyRenderingStarted();
    }

    if (!selecting)
    {
      SetCurrent();
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glLoadIdentity();

    if (!lightRenderMode || settings->getBool(NavLighting))
    {
      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
    }
    else
    {
      glDisable(GL_NORMALIZE);
      glDisable(GL_LIGHTING);
      glDisable(GL_LIGHT0);
    }

    if (!lightRenderMode || settings->getBool(NavSmoothShading))
    {
      glShadeModel(GL_SMOOTH);
    }
    else
    {
      glShadeModel(GL_FLAT);
    }

    if (settings->getBool(DisplayWireframe))
    {
      glPolygonMode(GL_FRONT,GL_LINE);
    }
    else
    {
      glPolygonMode(GL_FRONT,GL_FILL);
    }

    // apply panning, zooming and rotating transformations
    glTranslatef(moveVector.x(), moveVector.y(), moveVector.z() - startPosZ);
    glRotatef(angleY,1.0f,0.0f,0.0f);
    glRotatef(angleX,0.0f,1.0f,0.0f);

    // structure will be drawn around the positive z-axis starting from the
    // origin, so rotate to make the z-axis point downwards
    glRotatef(90.0f,1.0f,0.0f,0.0f);

    // translate along the z-axis to make the vertical center of the structure
    // end up in the current origin
    float halfHeight = visualizer->getHalfStructureHeight();
    glTranslatef(0.0f,0.0f,-halfHeight);

    if (simulating)
    {
      visualizer->drawSimStates(sim->getStateHis(), sim->getCurrState(),
                                sim->getChosenTrans());
    }
    if (!lightRenderMode || settings->getBool(NavShowStates))
    {

      if (settings->getBool(DisplayStates))
      {
        // Identify that we are drawing states
        glPushName(STATE);
        visualizer->drawStates(simulating);
        glPopName();
      }
    }

    // Disable lighting while drawing transitions, otherwise their colours
    // change with the viewpoint
    glDisable(GL_NORMALIZE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);

    visualizer->drawTransitions(
      settings->getBool(DisplayTransitions)
      && (!lightRenderMode || settings->getBool(NavShowTransitions)),
      settings->getBool(DisplayBackpointers)
      && (!lightRenderMode || settings->getBool(NavShowBackpointers)));

    if (simulating)
    {
      // Draw transitions followed during simulation and the possible
      // transitions going out of the current state.
      // Identify that we are drawing selectable sim states in this mode.
      visualizer->drawSimTransitions(
        !lightRenderMode || settings->getBool(NavShowTransitions),
        !lightRenderMode || settings->getBool(NavShowBackpointers),
        sim->getTransHis(), sim->getPosTrans(), sim->getChosenTrans());
    }

    // Enable lighting again, if required
    if (!lightRenderMode || settings->getBool(NavLighting))
    {
      glEnable(GL_NORMALIZE);
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
    }

    if (!lightRenderMode || settings->getBool(NavTransparency))
    {
      // determine current viewpoint in world coordinates
      glPushMatrix();
      glLoadIdentity();
      glTranslatef(0.0f,0.0f,halfHeight);
      glRotatef(-90.0f ,1.0f,0.0f,0.0f);
      glRotatef(-angleX,0.0f,1.0f,0.0f);
      glRotatef(-angleY,1.0f,0.0f,0.0f);
      glTranslatef(-moveVector.x(),-moveVector.y(),-moveVector.z() + startPosZ);
      GLfloat M[16];
      glGetFloatv(GL_MODELVIEW_MATRIX,M);
      Vector3D viewpoint = Vector3D(M[12],M[13],M[14]);
      glPopMatrix();
      // sort clusters on distance to viewpoint
      visualizer->sortClusters(viewpoint);
    }

    // draw the structure
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    // Identify that we are drawing clusters
    glPushName(CLUSTER);
    visualizer->drawStructure();
    glPopName();
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);

    // do not show the picture in the canvas if we are collecting data
    if (!collectingData && !selecting)
    {
      SwapBuffers();
    }
    glPopMatrix();

    GLenum gl_error_code = glGetError();
    if (gl_error_code != GL_NO_ERROR)
    {
      mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(gl_error_code) << std::endl;
    }

    if (!collectingData)
    {
      mediator->notifyRenderingFinished();
    }
    displayAllowed = true;
  }
}

void GLCanvas::reshape()
{
  if (GetContext())
  {
    int width,height;
    GetClientSize(&width, &height);
    SetCurrent();
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, static_cast<GLfloat>(width) /
                   static_cast<GLfloat>(height), nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
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

void GLCanvas::onEraseBackground(wxEraseEvent& /*event*/)
{
}

// Mouse event handlers

void GLCanvas::determineCurrentTool(wxMouseEvent& event)
{
  if (event.MiddleIsDown() || (event.LeftIsDown() && event.RightIsDown()))
  {
    currentTool = myID_ZOOM;
  }
  else if (event.RightIsDown())
  {
    currentTool = myID_ROTATE;
  }
  else
  {
    currentTool = activeTool;
  }
  setMouseCursor();
}

void GLCanvas::setMouseCursor()
{
  wxCursor cursor;
  wxImage img;
  bool ok = true;
  switch (currentTool)
  {
    case myID_SELECT:
      cursor = wxNullCursor;
      break;
    case myID_ZOOM:
      img = wxImage(zoom_cursor);
      img.SetMaskFromImage(wxImage(zoom_cursor_mask),255,0,0);
      cursor = wxCursor(img);
      break;
    case myID_PAN:
      img = wxImage(pan_cursor);
      img.SetMaskFromImage(wxImage(pan_cursor_mask),255,0,0);
      cursor = wxCursor(img);
      break;
    case myID_ROTATE:
      img = wxImage(rotate_cursor);
      img.SetMaskFromImage(wxImage(rotate_cursor_mask),255,0,0);
      cursor = wxCursor(img);
      break;
    default:
      ok = false;
      break;
  }

  if (ok)
  {
    SetCursor(cursor);
  }
}

void GLCanvas::onMouseEnter(wxMouseEvent& /*event*/)
{
  this->SetFocus();
}

void GLCanvas::onMouseDown(wxMouseEvent& event)
{

  lightRenderMode = true;

  determineCurrentTool(event);
  if (currentTool==myID_ZOOM || currentTool==myID_PAN ||
      currentTool==myID_ROTATE)
  {
    // Movement tools
    oldMouseX = event.GetX();
    oldMouseY = event.GetY();
  }
  else if (event.LeftDown())
  {
    // currentTool == myID_SELECT, selection tool
    // Delegate x and y coordinates to picking function.
    pickObjects(event.GetX(), event.GetY(), false);
  }
  display();
}

void GLCanvas::onMouseUp(wxMouseEvent& event)
{
  lightRenderMode = false;
  determineCurrentTool(event);
  display();
}

void GLCanvas::onMouseDClick(wxMouseEvent& event)
{
  lightRenderMode = true;
  if (currentTool == myID_SELECT)
  {
    pickObjects(event.GetX(), event.GetY(), true);
  }
  display();
}

void GLCanvas::onMouseMove(wxMouseEvent& event)
{
  if (event.Dragging())
  {
    // mouse is moving with some button(s) pressed
    int newMouseX = (int)event.GetX();
    int newMouseY = (int)event.GetY();
    Vector3D delta;
    switch (currentTool)
    {
      case myID_ZOOM :
        delta = Vector3D(0.0f, 0.0f, 0.01f * (startPosZ -
                                              moveVector.z()) * (oldMouseY - newMouseY));
        moveVector = moveVector + delta;
        oldMouseY = newMouseY;
        display();
        break;

      case myID_PAN :
        delta = Vector3D(-0.0015f * (startPosZ - moveVector.z()) *
                         (oldMouseX - newMouseX), 0.0015f * (startPosZ -
                             moveVector.z()) * (oldMouseY - newMouseY), 0.0f);
        moveVector = moveVector + delta;
        oldMouseX = newMouseX;
        oldMouseY = newMouseY;
        display();
        break;

      case myID_ROTATE :
        angleX -= 0.5f*(oldMouseX-newMouseX);
        angleY -= 0.5f*(oldMouseY-newMouseY);
        if (angleX >= 360.0f)
        {
          angleX -= 360.0f;
        }
        if (angleY >= 360.0f)
        {
          angleY -= 360.0f;
        }
        if (angleX < 0.0f)
        {
          angleX += 360.0f;
        }
        if (angleY < 0.0f)
        {
          angleY += 360.0f;
        }
        oldMouseX = newMouseX;
        oldMouseY = newMouseY;
        display();
        break;

      default :
        break;
    }
  }
  else
  {
    event.Skip();
  }
}

void GLCanvas::onMouseWheel(wxMouseEvent& event)
{
  Vector3D delta = Vector3D(0.0f, 0.0f,
                            0.001f * (startPosZ - moveVector.z()) * event.GetWheelRotation());
  moveVector = moveVector + delta;
  display();
}

unsigned char* GLCanvas::getPictureData(int w_res,int h_res)
{
  int w_block,h_block;
  GetClientSize(&w_block,&h_block);

  glReadBuffer(GL_BACK);
  glPixelStorei(GL_PACK_ALIGNMENT,1);

  TRcontext* tr_context = trNew();
  trTileSize(tr_context, w_block, h_block, 0);
  trImageSize(tr_context, w_res, h_res);

  unsigned char* pixdata = (unsigned char*)malloc(w_res * h_res * 3 *
                           sizeof(unsigned char));
  if (pixdata == NULL)
  {
    trDelete(tr_context);
    return NULL;
  }

  trImageBuffer(tr_context, GL_RGB, GL_UNSIGNED_BYTE, pixdata);
  trRowOrder(tr_context, TR_TOP_TO_BOTTOM);
  trPerspective(tr_context, 60.0f,
                (GLfloat)(w_block)/(GLfloat)(h_block), nearPlane, farPlane);

  collectingData = true;
  int more = 1;
  while (more)
  {
    trBeginTile(tr_context);
    display(true);
    more = trEndTile(tr_context);
  }
  collectingData = false;

  // RESET VIEW

  glViewport(0,0,w_block,h_block);
  reshape();
  display();

  return pixdata;
}

// Implementation of simulation header
void GLCanvas::refresh()
{
  if (sim != NULL)
  {
    if (sim->getStarted())
    {

      if (selectedType != SIMSTATE)
      {
        // Removed all selections that are not states of the simulation.
        mediator->deselect();
      }

      simulating = true;
      display();
    }
    else
    {
      if (selectedType == SIMSTATE)
      {
        // Remove selections made in simulation
        mediator->deselect();
      }

      simulating = false;
      display();
    }
  }
}

void GLCanvas::selChange()
{
  if (sim != NULL)
  {
    if (sim->getStarted())
    {
      simulating = true;
      display();
    }
    else
    {
      simulating = false;
      display();
    }
  }
}


void GLCanvas::processHits(const GLint hits, GLuint* buffer, bool doubleC)
{
  // This method selects the object clicked.
  //
  // The buffer content per hit is encoded as follows:
  // buffer[0]: The number of names on the name stack at the moment of the hit
  // buffer[1]: The minimal depth of the object hit
  // buffer[2]: The maximal depth of the object. We are certainly not interested
  //            in this.
  // buffer[3]: The type of the object picked
  // buffer[4]: The first identifier of the object picked.
  // (buffer[5]: The second identifier of the object picked.)

  int selectedObject[3];  // Objects are identified by at most 3 integers
  selectedObject[0] = PICKNONE;
  float curMinDepth = 2000000;
  float minDepth = 0;
  GLuint names;
  bool stateSelected = false; // Gives the selection of states precedence over
  // the selection of clusters.

  // Choose the nearest object and store it.
  for (GLint j=0; j < hits; ++j)
  {

    names = *buffer;
    buffer++; // buffer points to the minimal z value of the hit.
    minDepth = static_cast<float>(*buffer)/0x7fffffff;
    buffer++; // skip maximal z value of his (no interest)
    buffer++; // buffer points to the first name on the stack

    GLuint objType = *buffer;



    for (unsigned int k = 0; k < names; k++)
    {
      if (minDepth < curMinDepth && (!stateSelected || objType == STATE ||
                                     objType == SIMSTATE))
      {
        selectedObject[k] = *buffer;
      }
      buffer++;
    }

    if (minDepth < curMinDepth && (!stateSelected || objType == STATE ||
                                   objType == SIMSTATE) && names > 0)
    {
      stateSelected = objType == STATE || objType == SIMSTATE;
      curMinDepth = minDepth;
    }


  }

  selectedType = static_cast<PickState>(selectedObject[0]);

  mediator->deselect();
  switch (selectedType)
  {
    case STATE:
      mediator->selectStateByID(selectedObject[1]);
      break;
    case CLUSTER:
      mediator->selectCluster(selectedObject[1], selectedObject[2]);
      break;

    case SIMSTATE:
      mediator->selectStateByID(selectedObject[1]);
      // As part of selectStateByID, a simulation follow-up state was selected
      // if we caught a double click, follow to this state.
      if (doubleC)
      {
        sim->followTrans();
      }
      break;
    default:
      selectedType = PICKNONE;
      break;
  }
  visualizer->notify(Selection);
}


void GLCanvas::pickObjects(int x, int y, bool doubleC)
{
  // In the worst case, all the objects in the frame are hit. These objects are
  // given by the mediator. For each hit, the following needs to be recorded:
  // * The number of names on the stack
  // * The minimal depth of the hit object
  // * The maximal depth of the hit object
  // * The identifier of the type of object clicked
  // * Up to two numbers indicating the object selected
  GLsizei bufsize = mediator->getNumberOfObjects() * 6;
  if (GetContext())
  {
    GLuint* selectBuf = (GLuint*) malloc(bufsize * sizeof(GLuint));
    GLint  hits;
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glSelectBuffer(bufsize, selectBuf);
    // Swith to selection mode
    glRenderMode(GL_SELECT);

    glInitNames();
    // Create new projection transformation
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    glLoadIdentity();

    // Create 3x3 pixel picking region near cursor location
    gluPickMatrix((GLdouble) x, (GLdouble)  viewport[3] - y,
                  3.0, 3.0, viewport);
    int width,height;
    GetClientSize(&width,&height);
    gluPerspective(60.0f,(GLfloat)(width)/(GLfloat)(height),
                   nearPlane, farPlane);

    glMatrixMode(GL_MODELVIEW); // Switch to Modelview matrix in order to
    // calculate rotations etc.

    display(false, true);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);
    reshape();
    display();
    mediator->deselect();
    processHits(hits, selectBuf, doubleC);
    free(selectBuf);
  }
}
