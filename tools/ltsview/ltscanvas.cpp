// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/utilities/logger.h"
#include "ltscanvas.h"
#include "tr/tr.h"

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif
#include "mcrl2/utilities/workarounds.h"

#include "icons/zoom_cursor.xpm"
#include "icons/pan_cursor.xpm"
#include "icons/rotate_cursor.xpm"

LtsCanvas::LtsCanvas(QWidget *parent, Settings *settings, LtsManager *ltsManager, MarkManager *markManager):
  QGLWidget(parent),
  m_settings(settings),
  m_ltsManager(ltsManager),
  m_visualizer(new Visualizer(this, settings, ltsManager, markManager)),
  m_width(0),
  m_height(0),
  m_dragging(false)
{
  m_selectCursor = QCursor(Qt::ArrowCursor);
  m_panCursor = QCursor(pan_cursor);
  m_zoomCursor = QCursor(zoom_cursor);
  m_rotateCursor = QCursor(rotate_cursor);

  connect(m_visualizer, SIGNAL(dirtied()), this, SLOT(update()));
  connect(m_ltsManager, SIGNAL(clusterPositionsChanged()), this, SLOT(clusterPositionsChanged()));
  connect(m_ltsManager, SIGNAL(ltsZoomed(LTS *)), this, SLOT(clusterPositionsChanged()));
  connect(m_ltsManager, SIGNAL(ltsZoomed(LTS *)), this, SLOT(resetView()));
  connect(&m_settings->backgroundColor, SIGNAL(changed(QColor)), this, SLOT(update()));
  connect(&m_settings->displayStates, SIGNAL(changed(bool)), this, SLOT(update()));
  connect(&m_settings->displayTransitions, SIGNAL(changed(bool)), this, SLOT(update()));
  connect(&m_settings->displayBackpointers, SIGNAL(changed(bool)), this, SLOT(update()));
  connect(&m_settings->displayWireframe, SIGNAL(changed(bool)), this, SLOT(update()));

  clusterPositionsChanged();
  resetView();
}

void LtsCanvas::resetView()
{
  m_position = Vector3D(0.0f, 0.0f, 0.0f);
  m_rotationAngleX = 0.0f;
  m_rotationAngleY = 0.0f;
  update();
}

void LtsCanvas::clusterPositionsChanged()
{
  float width;
  float height;
  m_visualizer->computeBoundsInfo(width, height);
  m_nearPlane = 1.0f;
  m_baseDepth = 0.866f * height + width + m_nearPlane;
  m_farPlane = 4 * m_baseDepth;
  update();
}

void LtsCanvas::setActiveTool(Tool tool)
{
  m_activeTool = tool;
  if (tool == PanTool)
  {
    setCursor(m_panCursor);
  }
  else if (tool == ZoomTool)
  {
    setCursor(m_zoomCursor);
  }
  else if (tool == RotateTool)
  {
    setCursor(m_rotateCursor);
  }
  else
  {
    setCursor(m_selectCursor);
  }
}

void LtsCanvas::initializeGL()
{
  GLfloat gray[] = { 0.35f, 0.35f, 0.35f, 1.0f };
  GLfloat light_pos[] = { 50.0f, 50.0f, 50.0f, 1.0f };
  glLightfv(GL_LIGHT0, GL_AMBIENT, gray);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, gray);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);

  GLfloat light_col[] = { 0.2f, 0.2f, 0.2f };
  glMaterialfv(GL_FRONT, GL_SPECULAR, light_col);
  glMaterialf(GL_FRONT, GL_SHININESS, 8.0f);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glClearDepth(1.0);
}

void LtsCanvas::resizeGL(int width, int height)
{
  m_width = width;
  m_height = height;

  glViewport(0, 0, m_width, m_height);
}

void LtsCanvas::paintGL()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0f, (GLfloat)m_width / (GLfloat)m_height, m_nearPlane, m_farPlane);

  emit renderingStarted();
  render(m_dragging);
  emit renderingFinished();
}

void LtsCanvas::render(bool light)
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glClearColor(
    m_settings->backgroundColor.value().red() / 255.0,
    m_settings->backgroundColor.value().green() / 255.0,
    m_settings->backgroundColor.value().blue() / 255.0,
    1.0f
  );
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  if (!light || m_settings->navLighting.value())
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

  if (!light || m_settings->navSmoothShading.value())
  {
    glShadeModel(GL_SMOOTH);
  }
  else
  {
    glShadeModel(GL_FLAT);
  }

  if (m_settings->displayWireframe.value())
  {
    glPolygonMode(GL_FRONT,GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT,GL_FILL);
  }

  // apply panning, zooming and rotating transformations
  glTranslatef(m_position.x(), m_position.y(), m_position.z() - m_baseDepth);
  glRotatef(m_rotationAngleY, 1.0f, 0.0f, 0.0f);
  glRotatef(m_rotationAngleX, 0.0f, 1.0f, 0.0f);

  // structure will be drawn around the positive z-axis starting from the
  // origin, so rotate to make the z-axis point downwards
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

  // translate along the z-axis to make the vertical center of the structure
  // end up in the current origin
  float halfHeight = m_visualizer->getHalfStructureHeight();
  glTranslatef(0.0f, 0.0f, -halfHeight);

  if (m_ltsManager->simulationActive())
  {
    glPushName(StateObject);
    m_visualizer->drawSimStates(
      m_ltsManager->simulationStateHistory(),
      m_ltsManager->currentSimulationState(),
      m_ltsManager->currentSimulationTransition()
    );
  }
  if (!light || m_settings->navShowStates.value())
  {
    if (m_settings->displayStates.value())
    {
      // Identify that we are drawing states
      glPushName(StateObject);
      m_visualizer->drawStates(m_ltsManager->simulationActive());
      glPopName();
    }
  }

  // Disable lighting while drawing transitions, otherwise their colours
  // change with the viewpoint
  glDisable(GL_NORMALIZE);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  m_visualizer->drawTransitions(
    m_settings->displayTransitions.value() && (!light || m_settings->navShowTransitions.value()),
    m_settings->displayBackpointers.value() && (!light || m_settings->navShowBackpointers.value())
  );

  if (m_ltsManager->simulationActive())
  {
    // Draw transitions followed during simulation and the possible
    // transitions going out of the current state.
    // Identify that we are drawing selectable sim states in this mode.
    m_visualizer->drawSimTransitions(
      !light || m_settings->navShowTransitions.value(),
      !light || m_settings->navShowBackpointers.value(),
      m_ltsManager->simulationTransitionHistory(),
      m_ltsManager->simulationAvailableTransitions(),
      m_ltsManager->currentSimulationTransition()
    );
  }

  // Enable lighting again, if required
  if (!light || m_settings->navLighting.value())
  {
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
  }

  if (!light || m_settings->navTransparency.value())
  {
    // determine current viewpoint in world coordinates
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, halfHeight);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-m_rotationAngleX, 0.0f, 1.0f, 0.0f);
    glRotatef(-m_rotationAngleY, 1.0f, 0.0f, 0.0f);
    glTranslatef(-m_position.x(), -m_position.y(), -m_position.z() + m_baseDepth);
    GLfloat matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    Vector3D viewpoint = Vector3D(matrix[12], matrix[13], matrix[14]);
    glPopMatrix();
    // sort clusters on distance to viewpoint
    m_visualizer->sortClusters(viewpoint);
  }

  // draw the structure
  glEnable(GL_BLEND);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // Identify that we are drawing clusters
  glPushName(ClusterObject);
  m_visualizer->drawStructure();
  glPopName();
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_BLEND);

  glPopMatrix();

  GLenum error = glGetError();
  if (error != GL_NO_ERROR)
  {
    mCRL2log(mcrl2::log::error) << "OpenGL error: " << gluErrorString(error) << std::endl;
  }
}

void LtsCanvas::mousePressEvent(QMouseEvent *event)
{
  if ((event->modifiers() & Qt::ControlModifier) && (event->modifiers() & Qt::ShiftModifier))
  {
    setActiveTool(ZoomTool);
  }
  else if (event->modifiers() & Qt::ControlModifier)
  {
    setActiveTool(PanTool);
  }
  else if (event->modifiers() & Qt::ShiftModifier)
  {
    setActiveTool(RotateTool);
  }
  else if ((event->buttons() & Qt::MidButton) || ((event->buttons() & Qt::LeftButton) && (event->buttons() & Qt::RightButton)))
  {
    setActiveTool(ZoomTool);
  }
  else if (event->buttons() & Qt::RightButton)
  {
    setActiveTool(RotateTool);
  }
  else
  {
    setActiveTool(m_selectedTool);
  }

  if (m_activeTool == SelectTool)
  {
    if (m_ltsManager->lts())
    {
      Selection selection = selectObject(event->pos());
      if (selection.state)
      {
        m_ltsManager->selectState(selection.state);
      }
      else if (selection.cluster)
      {
        m_ltsManager->selectCluster(selection.cluster);
      }
      else
      {
        m_ltsManager->unselect();
      }
    }
  }
  else
  {
    m_dragging = true;
    m_lastMousePosition = event->pos();
  }
  event->accept();
}

void LtsCanvas::mouseReleaseEvent(QMouseEvent *event)
{
  m_dragging = false;
  setActiveTool(m_selectedTool);
  event->accept();
  repaint();
}

void LtsCanvas::mouseDoubleClickEvent(QMouseEvent *event)
{
  mousePressEvent(event);
  if (m_activeTool == SelectTool && m_ltsManager->simulationActive() && m_ltsManager->selectedState())
  {
    m_ltsManager->simulateState(m_ltsManager->selectedState());
  }
}

void LtsCanvas::mouseMoveEvent(QMouseEvent *event)
{
  QPoint oldPosition = m_lastMousePosition;
  m_lastMousePosition = event->pos();

  if (!event->buttons() || !m_dragging)
  {
    event->ignore();
    return;
  }

  if (m_activeTool == PanTool)
  {
    m_position += Vector3D(
      -0.0015f * (m_baseDepth - m_position.z()) * (oldPosition.x() - event->x()),
       0.0015f * (m_baseDepth - m_position.z()) * (oldPosition.y() - event->y()),
       0.0f
    );
    event->accept();
    repaint();
  }
  else if (m_activeTool == ZoomTool)
  {
    m_position += Vector3D(
      0.0f,
      0.0f,
      0.01f * (m_baseDepth - m_position.z()) * (oldPosition.y() - event->y())
    );
    event->accept();
    repaint();
  }
  else if (m_activeTool == RotateTool)
  {
    m_rotationAngleX -= 0.5f * (oldPosition.x() - event->x());
    m_rotationAngleY -= 0.5f * (oldPosition.y() - event->y());
    if (m_rotationAngleX < 0.0f) m_rotationAngleX += 360.0f;
    if (m_rotationAngleY < 0.0f) m_rotationAngleY += 360.0f;
    if (m_rotationAngleX >= 360.0f) m_rotationAngleX -= 360.0f;
    if (m_rotationAngleY >= 360.0f) m_rotationAngleY -= 360.0f;
    event->accept();
    repaint();
  }
}

void LtsCanvas::wheelEvent(QWheelEvent *event)
{
  m_position += Vector3D(0.0f, 0.0f, 0.001f * (m_baseDepth - m_position.z()) * event->delta());
  event->accept();
  repaint();
}

LtsCanvas::Selection LtsCanvas::selectObject(QPoint position)
{
  // In the worst case, all the objects in the frame are hit.
  // For each hit, the following needs to be recorded:
  // * The number of names on the stack
  // * The minimal depth of the hit object
  // * The maximal depth of the hit object
  // * The identifier of the type of object clicked
  // * Up to two numbers indicating the object selected

  int maxItems = m_ltsManager->lts()->getNumClusters() + m_ltsManager->lts()->getNumStates();
  int bufferSize = maxItems * 6;
  GLuint *selectionBuffer = new GLuint[bufferSize];

  glSelectBuffer(bufferSize, selectionBuffer);
  glRenderMode(GL_SELECT);
  glInitNames();

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  gluPickMatrix((GLdouble)position.x(), (GLdouble)(viewport[3] - position.y()), 3.0, 3.0, viewport);

  gluPerspective(60.0f, (GLfloat)m_width / (GLfloat)m_height, m_nearPlane, m_farPlane);

  render(false);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();

  GLint hits = glRenderMode(GL_RENDER);
  Selection output = parseSelection(selectionBuffer, hits);
  delete[] selectionBuffer;
  return output;
}

LtsCanvas::Selection LtsCanvas::parseSelection(GLuint *selectionBuffer, GLint items)
{
  GLuint *buffer = selectionBuffer;
  // This method selects the object clicked.
  //
  // The buffer content per hit is encoded as follows:
  // buffer[0]: The number of names on the name stack at the moment of the hit
  // buffer[1]: The minimal depth of the object hit
  // buffer[2]: The maximal depth of the object. We are certainly not interested in this.
  // buffer[3]: The type of the object picked
  // buffer[4]: The first identifier of the object picked.
  // (buffer[5]: The second identifier of the object picked.)

  bool stateFound = false;
  float minimumStateDepth;
  int stateID;

  float clusterFound = false;
  float minimumClusterDepth;
  int rank;
  int position;

  for (GLint i = 0; i < items; i++)
  {
    GLuint size = *buffer++;
    assert (size <= 3);

    float minimalDepth = ((float)(*buffer++)) / 0x7fffffff;
    buffer++;

    if (!size)
    {
      continue;
    }

    GLuint itemData[3];
    for (GLuint j = 0; j < size; j++)
    {
      itemData[j] = *buffer++;
    }

    if (itemData[0] == StateObject)
    {
      if (!stateFound || minimalDepth < minimumStateDepth)
      {
        stateFound = true;
        minimumStateDepth = minimalDepth;
        stateID = (int)itemData[1];
      }
    }
    else if (itemData[0] == ClusterObject)
    {
      if (!clusterFound || minimalDepth < minimumClusterDepth)
      {
        clusterFound = true;
        minimumClusterDepth = minimalDepth;
        rank = (int)itemData[1];
        position = (int)itemData[2];
      }
    }
  }

  Selection output;
  output.state = 0;
  output.cluster = 0;

  if (stateFound)
  {
    output.state = m_ltsManager->lts()->state(stateID);
  }
  else if (clusterFound)
  {
    output.cluster = m_ltsManager->lts()->cluster(rank, position);
  }

  return output;
}

QImage LtsCanvas::renderImage(int width, int height)
{
  glReadBuffer(GL_BACK);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  TRcontext *context = trNew();
  trTileSize(context, m_width, m_height, 0);
  trImageSize(context, width, height);

  unsigned char *imageData;
  try
  {
    imageData = new unsigned char[width * height * 4];
  }
  catch (std::bad_alloc exception)
  {
    trDelete(context);
    return QImage();
  }

  trImageBuffer(context, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, imageData);
  trRowOrder(context, TR_TOP_TO_BOTTOM);
  trPerspective(context, 60.0f, (GLfloat)m_width / (GLfloat)m_height, m_nearPlane, m_farPlane);

  emit renderingStarted();
  int more;
  do
  {
    trBeginTile(context);
    render(false);
    more = trEndTile(context);
  }
  while (more);
  trDelete(context);
  emit renderingFinished();

  // RESET VIEW
  glViewport(0, 0, m_width, m_height);

  QImage image = QImage(imageData, width, height, QImage::Format_ARGB32).mirrored();
  delete[] imageData;
  return image;
}
