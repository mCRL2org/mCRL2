#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
#include <cmath>
#include <wx/image.h>
#include "glcanvas.h"
#include "ids.h"
#include "icons/zoom_cursor.xpm"
#include "icons/zoom_cursor_mask.xpm"
#include "icons/pan_cursor.xpm"
#include "icons/pan_cursor_mask.xpm"
#include "icons/rotate_cursor.xpm"
#include "icons/rotate_cursor_mask.xpm"

using namespace Utils;
using namespace IDs;
BEGIN_EVENT_TABLE(GLCanvas,wxGLCanvas)
    EVT_MOTION(GLCanvas::onMouseMove)
    EVT_ENTER_WINDOW(GLCanvas::onMouseEnter)
    EVT_LEFT_DOWN(GLCanvas::onMouseDown)
    EVT_LEFT_UP(GLCanvas::onMouseUp)
    EVT_RIGHT_DOWN(GLCanvas::onMouseDown)
    EVT_RIGHT_UP(GLCanvas::onMouseUp)
    EVT_MIDDLE_DOWN(GLCanvas::onMouseDown)
    EVT_MIDDLE_UP(GLCanvas::onMouseUp)
    EVT_MOUSEWHEEL(GLCanvas::onMouseWheel)
    EVT_PAINT(GLCanvas::onPaint)
    EVT_SIZE(GLCanvas::onSize)
    EVT_ERASE_BACKGROUND(GLCanvas::OnEraseBackground)
END_EVENT_TABLE()

GLCanvas::GLCanvas(Mediator* owner,wxWindow* parent,const wxSize &size,
    int* attribList)
	: wxGLCanvas(parent,wxID_ANY,wxDefaultPosition,size,wxSUNKEN_BORDER,
		     wxT(""),attribList) {
  mediator = owner;
  displayAllowed = true;
  collectingData = false;
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector.x = 0.0f;
  moveVector.y = 0.0f;
  moveVector.z = 0.0f;
  startPosZ = 0.0f;
  startPosZDefault = 0.0f;
  farPlane = 0.0f;
  nearPlane = 1.0f;
  defaultBGColor.r = 100; 
  defaultBGColor.g = 100; 
  defaultBGColor.b = 100; 
  displayBackpointers = false;
  displayStates = false;
  displayTransitions = false;
  displayWireframe = false;
  lightRenderMode = false;

  setActiveTool(myID_SELECT);
}

GLCanvas::~GLCanvas() {
}

void GLCanvas::setVisualizer(Visualizer *vis) {
  visualizer = vis;
}

void GLCanvas::initialize() {
  SetCurrent();

  glDepthFunc(GL_LEQUAL);

  GLfloat gray[] = { 0.35f,0.35f,0.35f,1.0f };
  GLfloat light_pos[] = { 50.0f,50.0f,50.0f,1.0f };
  glLightfv(GL_LIGHT0,GL_AMBIENT,gray);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,gray);
  glLightfv(GL_LIGHT0,GL_POSITION,light_pos);
  
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_COLOR_MATERIAL);
  
  GLfloat light_col[] = { 0.2f,0.2f,0.2f };
  glMaterialfv(GL_FRONT,GL_SPECULAR,light_col);
  glMaterialf(GL_FRONT,GL_SHININESS,8.0f);
  glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  
  glClearColor(defaultBGColor.r/255.0f,defaultBGColor.g/255.0f,
      defaultBGColor.b/255.0f,1.0f);
  glClearDepth(1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SwapBuffers();
}

void GLCanvas::disableDisplay() {
  displayAllowed = false;
}

void GLCanvas::enableDisplay() {
  displayAllowed = true;
}

void GLCanvas::setDefaultPosition(float structWidth,float structHeight) {
  // structWidth is the radius of the smallest cylinder that contains the entire
  // structure; structHeight is the height of that cylinder
  // 0.5 / tan(60) = 0.866
  startPosZDefault = 0.866f*structHeight + structWidth + nearPlane;
  farPlane = startPosZDefault + 2*structWidth;
  reshape();
}

RGB_Color GLCanvas::getBackgroundColor() const {
  GLfloat bgc[4];
  glGetFloatv(GL_COLOR_CLEAR_VALUE,bgc);
  RGB_Color result = {
    static_cast<unsigned char>(bgc[0]*255.0f),
    static_cast<unsigned char>(bgc[1]*255.0f),
    static_cast<unsigned char>(bgc[2]*255.0f) };
  return result;
}

RGB_Color GLCanvas::getDefaultBackgroundColor() const { 
  return defaultBGColor;
}

void GLCanvas::setBackgroundColor(Utils::RGB_Color c) {
  glClearColor(c.r/255.0f,c.g/255.0f,c.b/255.0f,1.0f);
}

void GLCanvas::getMaxViewportDims(int *w,int* h) {
  GLint dims[2];
  glGetIntegerv(GL_MAX_VIEWPORT_DIMS,dims);
  *w = int(dims[0]);
  *h = int(dims[1]);
}

void GLCanvas::resetView() {
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector.x = 0.0f;
  moveVector.y = 0.0f;
  moveVector.z = 0.0f;
  startPosZ = startPosZDefault;
  reshape();
  display();
}

void GLCanvas::setActiveTool(int t) {
  activeTool = t;
  currentTool = t;
  setMouseCursor();
}

void GLCanvas::display(bool coll_caller) {
  // coll_caller indicates whether the caller of display() is the 
  // getPictureData() method. While collecting data, only this method is allowed
  // to call display(); else the collected data may be corrupted.
  if (collectingData && !coll_caller) {
    return;
  }
  
  // next check is for preventing infinite recursive calls to display(), which 
  // happened on the Mac during startup of the application
  if (displayAllowed) {
    displayAllowed = false;
    mediator->notifyRenderingStarted();
    SetCurrent();
    glPushMatrix();
      glLoadIdentity(); 
        
      if (lightRenderMode) {
        glDisable(GL_NORMALIZE);
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glShadeModel(GL_FLAT);
        //glDisable(GL_DEPTH_TEST);
        //glDisable(GL_COLOR_MATERIAL);
      }
      else {
        glEnable(GL_NORMALIZE);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glShadeModel(GL_SMOOTH);
        //glEnable(GL_DEPTH_TEST);
        //glEnable(GL_COLOR_MATERIAL);
      }
      if (displayWireframe) {
        glPolygonMode(GL_FRONT,GL_LINE);
      }
      else {
        glPolygonMode(GL_FRONT,GL_FILL);
      }
    
      glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
     
      // apply panning, zooming and rotating transformations
      glTranslatef(moveVector.x,moveVector.y,moveVector.z - startPosZ);
      glRotatef(angleY,1.0f,0.0f,0.0f);
      glRotatef(angleX,0.0f,1.0f,0.0f);

      // structure will be drawn around the positive z-axis starting from the
      // origin, so rotate to make the z-axis point downwards
      glRotatef(90.0f,1.0f,0.0f,0.0f);

      // translate along the z-axis to make the vertical center of the structure
      // end up in the current origin
      float halfHeight = visualizer->getHalfStructureHeight();
      glTranslatef(0.0f,0.0f,-halfHeight);
      
      if (!lightRenderMode && displayStates) {
        visualizer->drawStates();
      }
      
      if (!lightRenderMode && (displayTransitions || displayBackpointers)) {
        visualizer->drawTransitions(displayTransitions,displayBackpointers);
      }
      
      if (!lightRenderMode) {
        // determine current viewpoint in world coordinates
        glPushMatrix();
          glLoadIdentity();
          glTranslatef(0.0f,0.0f,halfHeight);
          glRotatef(-90.0f ,1.0f,0.0f,0.0f);
          glRotatef(-angleX,0.0f,1.0f,0.0f);
          glRotatef(-angleY,1.0f,0.0f,0.0f);
          glTranslatef(-moveVector.x,-moveVector.y,-moveVector.z + startPosZ);
          GLfloat M[16];
          glGetFloatv(GL_MODELVIEW_MATRIX,M);
          Point3D viewpoint = { M[12],M[13],M[14] };
        glPopMatrix();
        // sort clusters on distance to viewpoint
        visualizer->sortClusters(viewpoint);
      }

      // draw the structure
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
      glDepthMask(GL_FALSE);
      visualizer->drawStructure();
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
      
      // do not show the picture in the canvas if we are collecting data
      if (!collectingData) {
        SwapBuffers();
      }
    glPopMatrix();
    mediator->notifyRenderingFinished();
    displayAllowed = true;
  }
}

void GLCanvas::reshape() {
  int width,height;
  GetClientSize(&width,&height);
  glViewport(0,0,width,height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f,(GLfloat)(width)/(GLfloat)(height),nearPlane,farPlane);
  glMatrixMode(GL_MODELVIEW);
}

void GLCanvas::onPaint(wxPaintEvent& /*event*/) {
  wxPaintDC dc(this);
  display();
}

void GLCanvas::onSize(wxSizeEvent& /*event*/) {
  reshape();
}

void GLCanvas::OnEraseBackground(wxEraseEvent& /*event*/) {
}

// Mouse event handlers

void GLCanvas::determineCurrentTool(wxMouseEvent& event) {
  if (event.MiddleIsDown() || (event.LeftIsDown() && event.RightIsDown())) {
    currentTool = myID_ZOOM;
  }
  else if (event.RightIsDown()) {
    currentTool = myID_ROTATE;
  }
  else {
    currentTool = activeTool;
  }
  setMouseCursor();
}

void GLCanvas::setMouseCursor() {
  wxImage img;
  wxCursor cursor;
  bool ok = true;
  switch (currentTool) {
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

  if (ok) {
    SetCursor(cursor);
  }
}

void GLCanvas::onMouseEnter(wxMouseEvent& /*event*/) {
  this->SetFocus();
}

void GLCanvas::onMouseDown(wxMouseEvent& event) {
  lightRenderMode = true;
  determineCurrentTool(event);
  if (currentTool==myID_ZOOM || currentTool==myID_PAN ||
      currentTool==myID_ROTATE) {
    oldMouseX = event.GetX();
    oldMouseY = event.GetY();
  }
  display();
}

void GLCanvas::onMouseUp(wxMouseEvent& event) {
  lightRenderMode = false;
  determineCurrentTool(event);
  display();
}

void GLCanvas::onMouseMove(wxMouseEvent& event) {
  if (event.Dragging()) {
    // mouse is moving with some button(s) pressed
    int newMouseX = (int)event.GetX();
    int newMouseY = (int)event.GetY();
    switch (currentTool) {
      case myID_ZOOM :
	      moveVector.z += 0.01f*(startPosZ-moveVector.z)*(oldMouseY-newMouseY);
	      oldMouseY = newMouseY;
        display();
	      break;
	
      case myID_PAN :
	      moveVector.x -= 0.0015f*(startPosZ-moveVector.z)*(oldMouseX-newMouseX);
	      moveVector.y += 0.0015f*(startPosZ-moveVector.z)*(oldMouseY-newMouseY);
	      oldMouseX = newMouseX;
	      oldMouseY = newMouseY;
	      display();
	      break;
	
      case myID_ROTATE :
	      angleX -= 0.5f*(oldMouseX-newMouseX);
	      angleY -= 0.5f*(oldMouseY-newMouseY);
	      if (angleX >= 360.0f) angleX -= 360.0f;
	      if (angleY >= 360.0f) angleY -= 360.0f;
      	if (angleX < 0.0f) angleX += 360.0f;
      	if (angleY < 0.0f) angleY += 360.0f;
	      oldMouseX = newMouseX;
	      oldMouseY = newMouseY;
	      display();
	      break;
	
      default : break;
    }
  }
  else {
    event.Skip();
  }
}

void GLCanvas::onMouseWheel(wxMouseEvent& event) {
  moveVector.z += 0.001f*(startPosZ-moveVector.z)*event.GetWheelRotation();
  display();
}

unsigned char* GLCanvas::getPictureData(int w_res,int h_res) {
  /* collect the contents of the GLCanvas in an array of bytes; every byte is
   * the R-, G-, or B-value of a pixel; order of returned data is from
   * bottomleft to topright corner of canvas, row major.
   *
   * The user wants a picture of w_res * h_res pixels, but the canvas is w_block
   * * h_block pixels and we can only read the contents of the canvas. So, we
   * resize the viewport to w_res * h_res and read the viewport data in blocks
   * of size w_block * h_block. Because in general (w_res MOD w_block) and
   * (h_res MOD h_block) need not be 0, we also have to read a few remaining strips
   * of viewport data. Afterwards, we "stitch" all these blocks of data together
   * to obtain one big picture.
   */
  int w_block,h_block;
  GetClientSize(&w_block,&h_block);

  int W = w_res / w_block;     /* number of blocks in X direction */
  int H = h_res / h_block;     /* number of blocks in Y direction */
  int w_rem = w_res % w_block; /* number of pixels remaining in X direction */
  int h_rem = h_res % h_block; /* number of pixels remaining in Y direction */

  int M = W;		       /* number of blocks in X incl.remaining strip */
  if (w_rem > 0) ++M;
  int N = H;		       /* number of blocks in Y incl.remaining strip */
  if (h_rem > 0) ++N;
  unsigned char* pixel_ptrs[M][N]; /* pointers to the blocks of data */

  glReadBuffer(GL_BACK);
  glPixelStorei(GL_PACK_ALIGNMENT,1);

  /* COLLECT ALL DATA */
  
  // calling display() first seems to solve the problem that sometimes the
  // collected data gets corrupted because of a pending repaint of the part of 
  // the canvas that was underneath the Save Picture dialog window...
  //display();
  
  // ... still, just to be sure, it's wise to apply mutual exclusion here
  collectingData = true;
  int bx,by; /* x and y coordinate of lower left corner of current block */
  int bw,bh; /* width and height of current block */
  by = 0;
  bh = h_block;
  for (int j=0; j<N; ++j) {
    if (j==H) bh = h_rem;
    bx = 0;
    bw = w_block;
    for (int i=0; i<M; ++i) {
      if (i==W) bw = w_rem;
      pixel_ptrs[i][j] = (unsigned char*)malloc(3*bw*bh*sizeof(unsigned char));
      glViewport(-bx,-by,w_res,h_res);
      // we do not want other methods to call display() while collecting data, 
      // so we call display(true): 'true' indicates that we are the method that 
      // is collecting data and are thus allowed to call display() 
      display(true);
      glReadPixels(0,0,bw,bh,GL_RGB,GL_UNSIGNED_BYTE,pixel_ptrs[i][j]);
      bx += w_block;
    }
    by += h_block;
  }
  collectingData = false;
  
  /* RESET VIEW */

  glViewport(0,0,w_block,h_block);
  display();

  /* STITCH COLLECTED DATA TOGETHER */
  
  unsigned char* pixels = 
    (unsigned char*)malloc(3*w_res*h_res*sizeof(unsigned char));
  
  int offset = 0;
  bh = h_block;
  for (int j=0; j<N; ++j) {
    if (j==H) bh = h_rem;
    for (int r=0; r<bh; ++r) {
      bw = w_block;
      for (int i=0; i<M; ++i) {
	      if (i==W) bw = w_rem;
        memcpy(pixels+offset,pixel_ptrs[i][j]+3*r*bw,3*bw);
        offset += 3*bw;
      }
    }
  }
  
  /* CLEAN UP */

  for (int j=0; j<N; ++j) {
    for (int i=0; i<M; ++i) {
      free(pixel_ptrs[i][j]);
    }
  }

  return pixels;
}

void GLCanvas::setDisplayWireframe(bool b) {
  displayWireframe = b;
}

void GLCanvas::setDisplayBackpointers(bool b) {
  displayBackpointers = b;
}

void GLCanvas::setDisplayStates(bool b) {
  displayStates = b;
}

void GLCanvas::setDisplayTransitions(bool b) {
  displayTransitions = b;
}
