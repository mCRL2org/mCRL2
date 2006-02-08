#include "glcanvas.h"
#include "icons/select_cursor.xpm"
#include "icons/select_cursor_mask.xpm"
#include "icons/zoom_cursor.xpm"
#include "icons/zoom_cursor_mask.xpm"
#include "icons/pan_cursor.xpm"
#include "icons/pan_cursor_mask.xpm"
#include "icons/rotate_cursor.xpm"
#include "icons/rotate_cursor_mask.xpm"

BEGIN_EVENT_TABLE( GLCanvas, wxGLCanvas )
    EVT_MOTION( GLCanvas::onMouseMove )
    EVT_LEFT_DOWN( GLCanvas::onMouseDown )
    EVT_LEFT_UP( GLCanvas::onMouseUp )
    EVT_RIGHT_DOWN( GLCanvas::onMouseDown )
    EVT_RIGHT_UP( GLCanvas::onMouseUp )
    EVT_MIDDLE_DOWN( GLCanvas::onMouseDown )
    EVT_MIDDLE_UP( GLCanvas::onMouseUp )
    EVT_MOUSEWHEEL( GLCanvas::onMouseWheel )
    EVT_PAINT( GLCanvas::onPaint )
    EVT_SIZE( GLCanvas::onSize )
    EVT_ERASE_BACKGROUND( GLCanvas::OnEraseBackground )
END_EVENT_TABLE()

GLCanvas::GLCanvas( Mediator* owner, wxWindow* parent, wxWindowID id,
    const wxPoint &pos, const wxSize &size )
        : wxGLCanvas( parent, id, pos, size )
{
  mediator = owner;
}

GLCanvas::~GLCanvas()
{
}

void GLCanvas::initialize()
{
  setActiveTool( myID_SELECT );
  displayAllowed = true;
  
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector.x = 0.0f;
  moveVector.y = 0.0f;
  moveVector.z = 0.0f;
  startPosZ = 0.0f;
  startPosZDefault = 0.0f;
  farClippingPlane = 0.0f;
  farClippingPlaneDefault = 0.0f;
  
  SetCurrent();

  glDepthFunc( GL_LEQUAL );
  glShadeModel( GL_SMOOTH );
  
  GLfloat gray[] = { 0.35f, 0.35f, 0.35f, 1.0f };
  GLfloat light_pos[] = { 50.0f, 50.0f, 50.0f, 1.0f };
  glEnable( GL_NORMALIZE );
  glLightfv( GL_LIGHT0, GL_AMBIENT, gray );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, gray );
  glLightfv( GL_LIGHT0, GL_POSITION, light_pos );
  
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_BLEND );
  
  glClearColor( 0.4, 0.4, 0.4, 1.0 );
  glClearDepth( 1.0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  SwapBuffers();
}

void GLCanvas::disableDisplay()
{
  displayAllowed = false;
}

void GLCanvas::enableDisplay()
{
  displayAllowed = true;
}

void GLCanvas::setDefaultPosition( float structWidth, float structHeight )
{
  // structWidth is the radius of the smallest cylinder that contains the entire
  // structure; structHeight is the height of that cylinder
  float minZ1 = 0.5f * structHeight / float( tan( PI / 6.0 ) ) + structWidth;
  float minZ2 = 0.0f;
  startPosZDefault = maximum( minZ1, minZ2 );
  farClippingPlaneDefault = 2.0f * startPosZDefault;
}

void GLCanvas::resetView()
{
  angleX = 0.0f;
  angleY = 0.0f;
  moveVector.x = 0.0f;
  moveVector.y = 0.0f;
  moveVector.z = 0.0f;
  startPosZ = startPosZDefault;
  farClippingPlane = farClippingPlaneDefault;
  reshape();
  display();
}

void GLCanvas::setActiveTool( int t )
{
  activeTool = t;
  currentTool = t;
  setMouseCursor();
}

void GLCanvas::display()
{
  if ( displayAllowed )
  {
    SetCurrent();
    glPushMatrix();
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      glLoadIdentity(); 
      
      // apply panning and zooming transformations
      glTranslatef( moveVector.x, moveVector.y, moveVector.z - startPosZ );
      
      // apply rotation transformations
      glRotatef( angleY, 1.0f, 0.0f, 0.0f );
      glRotatef( angleX, 0.0f, 1.0f, 0.0f );

      // draw the structure
      mediator->drawLTS();
      
      SwapBuffers();
    glPopMatrix();
  }
}

void GLCanvas::reshape()
{
  int width, height;
  GetSize( &width, &height );
  
  glViewport( 0, 0, width, height );
  glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0f, (GLfloat)(width) / (GLfloat)(height), 0.1f,
	farClippingPlane );
  glMatrixMode( GL_MODELVIEW );
}

void GLCanvas::onPaint( wxPaintEvent& /*event*/ )
{
  wxPaintDC dc( this );
  display();
}

void GLCanvas::onSize( wxSizeEvent& /*event*/ )
{
  reshape();
}

void GLCanvas::OnEraseBackground( wxEraseEvent& /*event*/ )
{
}

// Mouse event handlers

void GLCanvas::determineCurrentTool( wxMouseEvent& event )
{
  if ( event.MiddleIsDown() || ( event.LeftIsDown() && event.RightIsDown() ) )
  {
    currentTool = myID_ZOOM;
  }
  else if ( event.RightIsDown() )
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
  wxImage img;
  bool ok = true;
  switch ( currentTool )
  {
    case myID_SELECT:
      img = wxImage( select_cursor );
      img.SetMaskFromImage( wxImage( select_cursor_mask ), 255, 0, 0 );
      break;
    case myID_ZOOM:
      img = wxImage( zoom_cursor );
      img.SetMaskFromImage( wxImage( zoom_cursor_mask ), 255, 0, 0 );
      break;
    case myID_PAN:
      img = wxImage( pan_cursor );
      img.SetMaskFromImage( wxImage( pan_cursor_mask ), 255, 0, 0 );
      break;
    case myID_ROTATE:
      img = wxImage( rotate_cursor );
      img.SetMaskFromImage( wxImage( rotate_cursor_mask ), 255, 0, 0 );
      break;
    default:
      ok = false;
      break;
  }

  if ( ok )
  {
    SetCursor( img );
  }
}

void GLCanvas::onMouseDown( wxMouseEvent& event )
{
  determineCurrentTool( event );
  if ( currentTool == myID_ZOOM || currentTool == myID_PAN || currentTool == myID_ROTATE )
  {
    oldMouseX = event.GetX();
    oldMouseY = event.GetY();
  }
}

void GLCanvas::onMouseUp( wxMouseEvent& event )
{
  determineCurrentTool( event );
}

void GLCanvas::onMouseMove( wxMouseEvent& event )
{
  if ( event.Dragging() )
  {
    // mouse is moving with some button(s) pressed
    switch ( currentTool )
    {
      case myID_ZOOM :
	moveVector.z += 0.3f * (oldMouseY - (int)(event.GetY()));
	oldMouseY = (int)(event.GetY());
	display();
	break;
	
      case myID_PAN :
	moveVector.x -= 0.03f * (oldMouseX - (int)(event.GetX()));
	moveVector.y += 0.03f * (oldMouseY - (int)(event.GetY()));
	oldMouseX = (int)(event.GetX());
	oldMouseY = (int)(event.GetY());
	display();
	break;
	
      case myID_ROTATE :
	angleX -= 0.5f * (oldMouseX - (int)(event.GetX()));
	angleY -= 0.5f * (oldMouseY - (int)(event.GetY()));
	if ( angleX > 360.0f ) angleX -= 360.0f;
	if ( angleY > 360.0f ) angleY -= 360.0f;
	if ( angleX < 0.0f ) angleX += 360.0f;
	if ( angleY < 0.0f ) angleY += 360.0f;
	oldMouseX = (int)(event.GetX());
	oldMouseY = (int)(event.GetY());
	display();
	break;
	
      default : break;
    }
  }
  else
  {
    event.Skip();
  }
}

void GLCanvas::onMouseWheel( wxMouseEvent& event )
{
  moveVector.z += 0.02f * event.GetWheelRotation();
  display();
}
