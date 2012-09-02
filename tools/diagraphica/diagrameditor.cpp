// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagrameditor.cpp

#include "diagrameditor.h"
#include <iostream>
#include <sstream>
#include <QColorDialog>

using namespace std;


// -- init static variables -----------------------------------------
int DiagramEditor::szeTxt = 12;


// -- constructors and destructor -----------------------------------


DiagramEditor::DiagramEditor(
    QWidget *parent,
    Graph* g):
  Visualizer(parent, g)
{
  //setClearColor( 0.44, 0.59, 0.85 );
  setClearColor(0.65, 0.79, 0.94);


  m_diagram     = new Diagram(this);
  m_editMode    = EDIT_MODE_SELECT;
  m_currentSelectedShapeId = -1;
  m_currentSelectedHandleId = -1;

  initContextMenu();
  setMouseTracking(true); // Needed to paste at the current position with keyboard shortcuts
}


// -- get functions ---------------------------------------------


Shape* DiagramEditor::selectedShape()
{
  if (m_diagram != 0)
  {
    // Find any selected shape
    for (int i = 0; i < m_diagram->shapeCount(); ++i)
    {
      if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
      {
        return m_diagram->shape(i);
      }
    }
  }
  return 0;
}

QList<Shape*> DiagramEditor::selectedShapes()
{
  QList<Shape*> result;
  if (m_diagram != 0)
  {
    for (int i = 0; i < m_diagram->shapeCount(); ++i)
    {
      if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
      {
        result.append(m_diagram->shape(i));
      }
    }
  }
  return result;
}


// -- public utility functions --------------------------------------


void DiagramEditor::deselectAll()
{
  size_t sizeShapes = 0;
  if (m_diagram != 0)
  {
    sizeShapes = m_diagram->shapeCount();
  }

  for (size_t i = 0; i < sizeShapes; ++i)
  {
    m_diagram->shape(i)->setModeNormal();
  }
}


// -- visualization functions  ----------------------------------


void DiagramEditor::visualize(const bool& inSelectMode)
{
  if (inSelectMode)
  {
    // set up picking
    GLint hits = 0;
    GLuint selectBuf[512];
    startSelectMode(
          hits,
          selectBuf,
          2.0,
          2.0);

    // render in select mode
    glPushName(0);
    m_diagram->visualize(inSelectMode, pixelSize());
    glPopName();

    // finish up picking
    finishSelectMode(
          hits,
          selectBuf);
  }
  else
  {
    clear();
    m_diagram->visualize(inSelectMode, pixelSize());

    if (m_mouseDrag && m_lastMouseEvent.buttons() == Qt::LeftButton)
    {
      double pix = pixelSize();

      QPointF start = worldCoordinate(m_mouseDragStart);
      QPointF stop = worldCoordinate(m_lastMouseEvent.pos());

      QRectF gridCoordinates = m_diagram->gridCoordinates();

      double x1 = qMin(start.x(), gridCoordinates.right());
      double y1 = qMin(start.y(), gridCoordinates.top());
      double x2 = qMax(stop.x(), gridCoordinates.left());
      double y2 = qMax(stop.y(), gridCoordinates.bottom());

      x1 = qMax(x1, gridCoordinates.left());
      y1 = qMax(y1, gridCoordinates.bottom());
      x2 = qMin(x2, gridCoordinates.right());
      y2 = qMin(y2, gridCoordinates.top());

      if (m_diagram->snapGrid())
      {
        double intv = m_diagram->gridInterval(pixelSize());

        x1 = Utils::rndToNearestMult(x1, intv);
        y1 = Utils::rndToNearestMult(y1, intv);
        x2 = Utils::rndToNearestMult(x2, intv);
        y2 = Utils::rndToNearestMult(y2, intv);
      }

      double dX = x2-x1;
      double dY = y2-y1;

      double xC = x1+0.5*dX;
      double yC = y1+0.5*dY;

      VisUtils::setColor(VisUtils::darkGray);
      if (m_editMode == EDIT_MODE_SELECT)
      {
        m_selection.setCoords(x1, y1, x2, y2);
        if (m_currentSelectedShapeId == -1)
        {
          VisUtils::drawRect(x1, x2, y1, y2);
        }
      }
      else
      {
        m_selection.setCoords(-1, -1, -1, -1);
        if (m_editMode == EDIT_MODE_RECT || m_editMode == EDIT_MODE_NOTE)
        {
          VisUtils::drawRect(x1, x2, y1, y2);
        }
        else if (m_editMode == EDIT_MODE_ELLIPSE)
        {
          VisUtils::drawEllipse(xC, yC, 0.5*dX, 0.5*dY, Shape::segNumHnt);
        }
        else if (m_editMode == EDIT_MODE_LINE)
        {
          VisUtils::drawLine(x1, x2, y1, y2);
        }
        else if (m_editMode == EDIT_MODE_ARROW)
        {
          VisUtils::drawArrow(x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix);
        }
        else if (m_editMode == EDIT_MODE_DARROW)
        {
          VisUtils::drawDArrow(x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix);
        }
      }
    }
  }
}

void DiagramEditor::generateTextures()
{
  for (int i = 0; i < m_diagram->shapeCount(); ++i)
  {
    m_diagram->shape(i)->generateTextures();
  }
}


// -- set functions ---------------------------------------------


void DiagramEditor::setDiagram(Diagram* dgrm)
{
  delete m_diagram;
  m_diagram = dgrm;
  update();
}

void DiagramEditor::setEditMode(int mode)
{
  m_editMode = mode;
  deselectAll();
  update();
}

void DiagramEditor::setShowGrid(bool show)
{
  m_diagram->setShowGrid(show);
  update();
}


void DiagramEditor::setSnapGrid(bool show)
{
  m_diagram->setSnapGrid(show);
  update();
}


void DiagramEditor::setFillColor()
{
  Shape* s = selectedShape();
  if (s != 0)
  {
    QColor color = QColorDialog::getColor(s->fillColor(), this);
    if (color.isValid())
    {
      s->setFillColor(color);
      update();
    }
  }
}


void DiagramEditor::setLineColor()
{
  Shape* s = selectedShape();
  if (s != 0)
  {
    QColor color = QColorDialog::getColor(s->lineColor(), this);
    if (color.isValid())
    {
      s->setLineColor(color);
      update();
    }
  }
}


// -- context menu functions ------------------------------------


void DiagramEditor::editDof(Shape *shape)
{
  Shape* s = (shape != 0 ? shape : selectedShape());
  if (s != 0)
  {
    if (s->drawMode() == Shape::MODE_NORMAL || s->drawMode() == Shape::MODE_EDIT)
    {
      s->setModeEditDOFXCtr();
    }
    QList<DofDialog*> openDialogs = findChildren<DofDialog*>();
    for (int i = 0; i < openDialogs.size(); ++i)
    {
      if (openDialogs[i]->shape() == s)
      {
        openDialogs[i]->show();
        openDialogs[i]->setFocus();
        return;
      }
    }
    DofDialog* dofDialog = new DofDialog(m_graph, s, this);
    connect(dofDialog, SIGNAL(dofActivated(int)), this, SLOT(update()));
    dofDialog->show();
  }
}

void DiagramEditor::editTextSize()
{
  Shape* s = selectedShape();
  if (s != 0)
  {
    bool ok;
    int size = QInputDialog::getInt(this, "Set Text Size", "Points:", s->textSize(), 6, 72, 1, &ok);
    if (ok)
    {
      s->setTextSize(size);
      updateGL();
    }
  }
}

void DiagramEditor::editNote()
{
  Shape* s = selectedShape();
  if (s != 0)
  {
    bool ok;
    QString note = QInputDialog::getText(this, "Set Note", "Text:", QLineEdit::Normal, s->note(), &ok);
    if (ok)
    {
      s->setNote(note);
      updateGL();
    }
  }
}


void DiagramEditor::cutShapes()
{
  // find & cut selected shapes
  QList<Shape*> shapes = selectedShapes();

  if (!shapes.isEmpty()) {
    m_clipBoardList.clear();
    for (int i = 0; i < shapes.size(); ++i)
    {
      m_clipBoardList.append(new Shape(*shapes[i]));
      m_diagram->removeShape(shapes[i]->index());
    }
  }
  updateGL();
}


void DiagramEditor::copyShapes()
{
  // find & copy selected shapes
  QList<Shape*> shapes = selectedShapes();

  if (!shapes.isEmpty()) {
    m_clipBoardList.clear();
    for (int i = 0; i < shapes.size(); ++i)
    {
      m_clipBoardList.append(new Shape(*shapes[i]));
    }
  }
}

void DiagramEditor::pasteShapes()
{
  if (m_clipBoardList.size() > 0)
  {
    deselectAll();

    double xOriginal = m_clipBoardList[0]->xCenter();
    double yOriginal = m_clipBoardList[0]->yCenter();

    QPointF pos = worldCoordinate(m_lastMouseEvent.posF());
    double xPaste = pos.x();
    double yPaste = pos.y();

    for (int i = 0; i < m_clipBoardList.size(); ++i)
    {
      // update index of clipboard shape
      m_clipBoardList[i]->setIndex(m_diagram->shapeCount());

      // update clipboard shape
      if (i == 0) // Paste the first selected shape to the clicked position
      {
        m_clipBoardList[i]->setCenter(xPaste, yPaste);
      }
      else // Paste other shapes relative to their position with respect to first shape
      {
        double distanceX, distanceY, x1, x2, y1, y2;
        m_clipBoardList[0]->center(x1, y1);
        m_clipBoardList[i]->center(x2, y2);

        // calculate the distance between the first selected shape and the current shape
        distanceX = xOriginal - x2;
        distanceY = yOriginal - y2;
        if (x2 > xOriginal)
        {
          distanceX = x2 - xOriginal;
        }
        if (y2 > yOriginal)
        {
          distanceY = y2 - yOriginal;
        }

        double xC = xPaste + distanceX; // Calculate new center of the selected shape according to the distance between the first shape
        double yC = yPaste + distanceY;

        m_clipBoardList[i]->setCenter(xC, yC);
      }
      m_clipBoardList[i]->setModeEdit();

      // add clipboard shape to diagram
      m_diagram->addShape(m_clipBoardList[i]);
      // make another copy of clipboard shape
      m_clipBoardList[i] = new Shape(*m_clipBoardList[i]);
    }
  }
  updateGL();
}

void DiagramEditor::selectAllShapes()
{
  for (int i = 0; i < m_diagram->shapeCount() ; ++i)
  {
    m_diagram->shape(i)->setDrawMode(Shape::MODE_EDIT);
  }
  updateGL();
}

void DiagramEditor::deleteShapes()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; i--)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->removeShape(i);
    }
  }
  updateGL();
}

void DiagramEditor::bringToFront()
{
  for (int i = 0; i < m_diagram->shapeCount(); ++i)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeToFront(i);
    }
  }
  updateGL();
}

void DiagramEditor::sendToBack()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; i--)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeToBack(i);
    }
  }
  updateGL();
}

void DiagramEditor::bringForward()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; i--)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeForward(i);
    }
  }
  updateGL();
}

void DiagramEditor::sendBackward()
{
  for (int i = 0; i < m_diagram->shapeCount(); ++i)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeBackward(i);
    }
  }
  updateGL();
}

double DiagramEditor::snapIfNeeded(double input)
{
  if (m_diagram->snapGrid())
  {
    double itv = m_diagram->gridInterval(pixelSize());
    return Utils::rndToNearestMult(input, itv);
  }
  return input;
}

QPointF DiagramEditor::snapIfNeeded(QPointF input)
{
  return QPointF(snapIfNeeded(input.x()), snapIfNeeded(input.y()));
}

double DiagramEditor::snapAngleIfNeeded(double input)
{
  if (m_diagram->snapGrid())
  {
    double itv = m_diagram->angleInterval();
    return Utils::rndToNearestMult(input, itv);
  }
  return input;
}

QRectF DiagramEditor::worldRectangle(QPointF start, QPointF stop)
{
  start = worldCoordinate(start);
  stop = worldCoordinate(stop);

  QRectF gridCoordinates = m_diagram->gridCoordinates();

  double x1 = qMin(start.x(), gridCoordinates.right());
  double y1 = qMin(start.y(), gridCoordinates.top());
  double x2 = qMax(stop.x(), gridCoordinates.left());
  double y2 = qMax(stop.y(), gridCoordinates.bottom());

  x1 = snapIfNeeded(qMax(x1, gridCoordinates.left()));
  y1 = snapIfNeeded(qMax(y1, gridCoordinates.bottom()));
  x2 = snapIfNeeded(qMin(x2, gridCoordinates.right()));
  y2 = snapIfNeeded(qMin(y2, gridCoordinates.top()));

  return QRectF(x1, y1, x2-x1, y2-y1);
}


// -- private utility functions ---------------------------------


void DiagramEditor::translatePoints(double& x1, double& y1,
                                    double& x2, double& y2,
                                    double givenX1, double givenY1,
                                    double givenX2, double givenY2)
{
  x1 = givenX1 - givenX2;
  y1 = givenY1 + givenY2;
  x2 = givenX1 + givenX2;
  y2 = givenY1 - givenY2;
  double temp;
  if (x1 > x2)
  {
    temp = x1;
    x1 = x2;
    x2 = temp;
  }
  if (y2 > y1)
  {
    temp = y1;
    y1 = y2;
    y2 = temp;
  }
}

void DiagramEditor::createShape()
{
  deselectAll();

  QRectF rectangle = worldRectangle(m_mouseDragStart, m_lastMouseEvent.pos());
  double dX = rectangle.width();
  double dY = rectangle.height();

  double pix = pixelSize();
  if (Utils::abs(dX) < Shape::minSzeHnt*pix && Utils::abs(dY) < Shape::minSzeHnt*pix)
  {
    dX = Shape::minSzeHnt*pix;
    dY = Shape::minSzeHnt*pix;
  }

  double xC = rectangle.left()+0.5*dX;
  double yC = rectangle.top()+0.5*dY;
  double xDFC = 0.5*dX;
  double yDFC = -0.5*dY;

  Shape* s = new Shape(m_diagram, m_diagram->shapeCount(),
                       xC,        yC,
                       xDFC,      yDFC,
                       0.0,       Shape::TYPE_RECT,
                       0.0,       0.0);

  s->setModeEdit();

  if (m_editMode == EDIT_MODE_RECT)
  {
    s->setTypeRect();
  }
  else if (m_editMode == EDIT_MODE_ELLIPSE)
  {
    s->setTypeEllipse();
  }
  else if (m_editMode == EDIT_MODE_LINE)
  {
    s->setTypeLine();
  }
  else if (m_editMode == EDIT_MODE_ARROW)
  {
    s->setTypeArrow();
  }
  else if (m_editMode == EDIT_MODE_DARROW)
  {
    s->setTypeDArrow();
  }
  else if (m_editMode == EDIT_MODE_NOTE)
  {
    s->setTypeNote();
  }

  m_diagram->addShape(s);
  updateGL();

  if (m_editMode == EDIT_MODE_NOTE)
  {
    editNote();
  }
}

void DiagramEditor::initContextMenu()
{
  m_popup.clear();
  m_popup.addAction("Edit DOF", this, SLOT(editDof()));
  m_popup.addAction("Edit Note", this, SLOT(editNote()));
  m_popup.addAction("Text Size", this, SLOT(editTextSize()));
  m_popup.addSeparator();
  m_popup.addAction("Cut", this, SLOT(cutShapes()), QKeySequence::Cut);
  m_popup.addAction("Copy", this, SLOT(copyShapes()), QKeySequence::Copy);
  m_popup.addAction("Paste", this, SLOT(pasteShapes()), QKeySequence::Paste);
  m_popup.addAction("Select All", this, SLOT(selectAllShapes()), QKeySequence::SelectAll);
  m_popup.addSeparator();
  m_popup.addAction("Delete", this, SLOT(deleteShapes()), QKeySequence::Delete);
  m_popup.addSeparator();
  m_popup.addAction("Bring to front", this, SLOT(bringToFront()));
  m_popup.addAction("Send to back", this, SLOT(sendToBack()));
  m_popup.addAction("Bring forward", this, SLOT(bringForward()));
  m_popup.addAction("Send backward", this, SLOT(sendBackward()));
}

void DiagramEditor::showContextMenu()
{
  QStringList enabledOptions("Select All");

  if (!m_clipBoardList.isEmpty())
  {
    enabledOptions << "Paste";
  }

  QList<Shape*> shapes = selectedShapes();
  if (shapes.size() == 1)
  {
    if (shapes[0]->shapeType() != Shape::TYPE_NOTE)
      enabledOptions << "Edit DOF";
    enabledOptions  << "Edit Note" << "Text Size";
  }
  if (shapes.size() > 0)
  {
    enabledOptions << "Cut" << "Copy" << "Delete" <<
                      "Bring to front" << "Send to back" <<
                      "Bring forward" << "Send backward";
  }

  QList<QAction*> actions = m_popup.actions();
  for (int i = 0; i < actions.size(); ++i)
  {
    actions[i]->setEnabled(enabledOptions.indexOf(actions[i]->text()) != -1);
  }

  m_popup.popup(mapToGlobal(m_lastMouseEvent.pos()));
}


// -- event handlers --------------------------------------------


void DiagramEditor::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  if (e->type() != QEvent::MouseMove || e->buttons() != Qt::NoButton)
  {
    // redraw in select mode
    updateGL(true);
    // redraw in render mode
    updateGL();
  }

  m_lastMousePos = e->pos();
}

void DiagramEditor::handleKeyEvent(QKeyEvent* e)
{
  Visualizer::handleKeyEvent(e);
  if (e->type() == QEvent::KeyPress)
  {
    if (m_editMode == EDIT_MODE_SELECT && e->matches(QKeySequence::SelectAll))
    {
      selectAllShapes();
    }
    else if (e->matches(QKeySequence::Cut))
    {
      cutShapes();
    }
    else if (e->matches(QKeySequence::Copy))
    {
      copyShapes();
    }
    else if (e->matches(QKeySequence::Paste))
    {
      pasteShapes();
    }
  }
  if (e->type() == QEvent::KeyRelease)
  {
    if (e->matches(QKeySequence::Delete))
    {
      deleteShapes();
    }
  }
  // redraw in render mode
  updateGL();
}

void DiagramEditor::handleHits(const vector< int > &ids)
{
  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
  {
    if (ids.size() <= 1 || (m_lastMouseEvent.button() == Qt::LeftButton && m_editMode != EDIT_MODE_SELECT && m_editMode != EDIT_MODE_DOF))
    // Deselect if nothing hit or before shape creation
    {
      deselectAll();
    }
    else if (ids.size() == 2)
    {
      if (m_editMode == EDIT_MODE_SELECT || m_lastMouseEvent.button() == Qt::RightButton)
      // Select the shape if it was not selected
      {
        Shape* s = m_diagram->shape(ids[1]);
        if (s != 0)
        {
          if (s->drawMode() != Shape::MODE_EDIT)
          {
            deselectAll();
            s->setModeEdit();
          }
        }
      }
      if (m_editMode == EDIT_MODE_SELECT && m_lastMouseEvent.button() == Qt::LeftButton)
      // Prepare dragging
      {
        m_currentSelectedShapeId = ids[1];
        m_currentSelectedHandleId = -1;
      }
    }
    else // ids.size() > 2
    {
      if (m_editMode == EDIT_MODE_SELECT || m_editMode == EDIT_MODE_DOF)
      // Prepare dragging of a handle
      {
        m_currentSelectedShapeId = ids[1];
        m_currentSelectedHandleId = ids[2];
      }
    }
  }

  if (ids.size() >= 2 && m_lastMouseEvent.type() == QEvent::MouseButtonRelease && m_lastMouseEvent.button() == Qt::LeftButton)
  {
    Shape* s = m_diagram->shape(ids[1]);
    if (s != 0)
    {
      if (m_editMode == EDIT_MODE_DOF && ids.size() == 2)
      {
        editDof(s);
      }
      if ((m_editMode == EDIT_MODE_SELECT || m_editMode == EDIT_MODE_DOF) && ids.size() == 3)
      {
        if (s->drawMode() == Shape::MODE_EDIT_DOF_AGL && ids[2] == Shape::ID_HDL_DIR)
        {
          s->angleDOF()->setDirection(0-s->angleDOF()->direction());
        }
      }
    }
  }


  if (m_mouseDrag && m_lastMouseEvent.buttons() == Qt::LeftButton)
  {
    if (m_currentSelectedShapeId != -1)
    // Some dragging is possibile
    {
      if (m_editMode == EDIT_MODE_SELECT)
      // Drag the shape or a shape handle
      {
        if (m_currentSelectedHandleId == -1 || m_currentSelectedHandleId == Shape::ID_HDL_CTR)
        // Drag the shape
        {
          QRectF rectangle = worldRectangle(m_lastMousePos, m_lastMouseEvent.pos());
          double dX = rectangle.width();
          double dY = rectangle.height();

          QList<Shape*> shapes = selectedShapes();
          for (int i = 0; i < shapes.size(); ++i)
          {
            Shape* s = shapes[i];
            s->setCenter(s->xCenter()+dX, s->yCenter()+dY);
          }
        }
        else
        // Drag a shape handle
        {
          Shape* s = m_diagram->shape(m_currentSelectedShapeId);
          if (s != 0)
          {
            switch(m_currentSelectedHandleId)
            {
              case Shape::ID_HDL_TOP_LFT:
                handleDragTop(s);
                handleDragLft(s);
                break;
              case Shape::ID_HDL_LFT:
                handleDragLft(s);
                break;
              case Shape::ID_HDL_BOT_LFT:
                handleDragBot(s);
                handleDragLft(s);
                break;
              case Shape::ID_HDL_BOT:
                handleDragBot(s);
                break;
              case Shape::ID_HDL_BOT_RGT:
                handleDragBot(s);
                handleDragRgt(s);
                break;
              case Shape::ID_HDL_RGT:
                handleDragRgt(s);
                break;
              case Shape::ID_HDL_TOP_RGT:
                handleDragTop(s);
                handleDragRgt(s);
                break;
              case Shape::ID_HDL_TOP:
                handleDragTop(s);
                break;
              case Shape::ID_HDL_ROT_RGT:
                handleDragRot(s, false);
                break;
              case Shape::ID_HDL_ROT_TOP:
                handleDragRot(s, true);
                break;
              default:
                break;
            }
          }
        }
      }
      if ((m_editMode == EDIT_MODE_SELECT || m_editMode == EDIT_MODE_DOF) && m_currentSelectedHandleId != -1)
      // Drag a DOF handle
      {
        Shape* s = m_diagram->shape(m_currentSelectedShapeId);
        if (s != 0)
        {
          switch(m_currentSelectedHandleId)
          {
            case Shape::ID_HDL_DOF_END:

              switch(s->drawMode())
              {
                case Shape::MODE_EDIT_DOF_XCTR:
                  handleDragDOFXCtrEnd(s);
                  break;
                case Shape::MODE_EDIT_DOF_YCTR:
                  handleDragDOFYCtrEnd(s);
                  break;
                case Shape::MODE_EDIT_DOF_WTH:
                  handleDragDOFWthEnd(s);
                  break;
                case Shape::MODE_EDIT_DOF_HGT:
                  handleDragDOFHgtEnd(s);
                  break;
                case Shape::MODE_EDIT_DOF_AGL:
                  handleDragDOFAglEnd(s);
                  break;
                default:
                  break;
              }

              break;
            case Shape::ID_HDL_HGE:
              if (s->drawMode() == Shape::MODE_EDIT_DOF_AGL)
              {
                handleDragDOFHge(s);
              }
              break;
            default:
              break;
          }
        }
      }
    }
  }


  if (m_mouseDragReleased && m_lastMouseEvent.button() == Qt::LeftButton && m_currentSelectedShapeId == -1)
  // Drag ended and no shape or handle was dragged
  {
    if (m_editMode == EDIT_MODE_SELECT)
    // We try to select some shapes
    {
      deselectAll();
      for (int i = 0; i < m_diagram->shapeCount(); ++i)
      {
        Shape* s = m_diagram->shape(i);
        QRectF shape(s->xCenter()-s->xDistance(), s->yCenter()-s->yDistance(), 2.0*s->xDistance(), 2.0*s->yDistance());

        if (m_selection.intersects(shape))
        {
          s->setDrawMode(Shape::MODE_EDIT);
        }
      }
    }

    if (m_editMode != EDIT_MODE_SELECT && m_editMode != EDIT_MODE_DOF)
    // We create a shape
    {
      createShape();
    }
  }

  if (m_lastMouseEvent.type() == QEvent::MouseButtonRelease && m_lastMouseEvent.button() == Qt::RightButton)
  {
    showContextMenu();
  }

  if (m_lastMouseEvent.buttons() == Qt::NoButton)
  // Reset the drag information if all buttons are released
  {
    m_currentSelectedShapeId = -1;
    m_currentSelectedHandleId = -1;
  }
}


void DiagramEditor::handleDragLft(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());

  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF rightLocation = QPointF(s->xCenter()+cos(angle)*s->xDistance(), s->yCenter()+sin(angle)*s->xDistance());

  double mouseToRightDistance = snapIfNeeded(Utils::distLinePoint(rightLocation, rightLocation + QPointF(sin(-angle), cos(-angle)), mouseLocation));
  QPointF leftLocation = QPointF(rightLocation.x() + mouseToRightDistance*cos(angle), rightLocation.y() + mouseToRightDistance*sin(angle));

  QPointF newCenter = (leftLocation + rightLocation) / 2;

  s->setCenter(newCenter.x(), newCenter.y());
  s->setDistance(-mouseToRightDistance/2, s->yDistance());
}


void DiagramEditor::handleDragBot(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());

  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF topLocation = QPointF(s->xCenter()-sin(angle)*s->yDistance(), s->yCenter()+cos(angle)*s->yDistance());

  double mouseToTopDistance = snapIfNeeded(Utils::distLinePoint(topLocation, topLocation + QPointF(cos(angle), sin(angle)), mouseLocation));
  QPointF BottomLocation = QPointF(topLocation.x() + mouseToTopDistance*sin(angle), topLocation.y() - mouseToTopDistance*cos(angle));

  QPointF newCenter = (BottomLocation + topLocation) / 2;

  s->setCenter(newCenter.x(), newCenter.y());
  s->setDistance(s->xDistance(), mouseToTopDistance/2);
}


void DiagramEditor::handleDragRgt(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());

  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF leftLocation = QPointF(s->xCenter()-cos(angle)*s->xDistance(), s->yCenter()-sin(angle)*s->xDistance());

  double mouseToLeftDistance = snapIfNeeded(Utils::distLinePoint(leftLocation, leftLocation + QPointF(sin(-angle), cos(-angle)), mouseLocation));
  QPointF rightLocation = QPointF(leftLocation.x() + mouseToLeftDistance*cos(angle), leftLocation.y() + mouseToLeftDistance*sin(angle));

  QPointF newCenter = (rightLocation + leftLocation) / 2;

  s->setCenter(newCenter.x(), newCenter.y());
  s->setDistance(mouseToLeftDistance/2, s->yDistance());
}


void DiagramEditor::handleDragTop(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());

  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF bottomLocation = QPointF(s->xCenter()+sin(angle)*s->yDistance(), s->yCenter()-cos(angle)*s->yDistance());

  double mouseToBottomDistance = snapIfNeeded(Utils::distLinePoint(bottomLocation, bottomLocation + QPointF(cos(angle), sin(angle)), mouseLocation));
  QPointF topLocation = QPointF(bottomLocation.x() + mouseToBottomDistance*sin(angle), bottomLocation.y() - mouseToBottomDistance*cos(angle));

  QPointF newCenter = (topLocation + bottomLocation) / 2;

  s->setCenter(newCenter.x(), newCenter.y());
  s->setDistance(s->xDistance(), -mouseToBottomDistance/2);
}


void DiagramEditor::handleDragRot(Shape* s, bool isTop)
{
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF relativeMouseLocation = mouseLocation - QPointF(s->xCenter(), s->yCenter());
  double newAngle = Utils::calcAngleDg(relativeMouseLocation.x(), relativeMouseLocation.y()) + (isTop ? 270.0 : 0);

  if ((!isTop && s->xDistance() < 0) || (isTop && s->yDistance() < 0))
  {
    newAngle += 180.0;
  }
  while (newAngle >= 360.0)
  {
    newAngle -= 360.0;
  }

  newAngle = snapAngleIfNeeded(newAngle);

  s->setAngle(newAngle);
}


void DiagramEditor::handleDragDOFXCtrEnd(Shape* s)
{
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  double mouseToCenterDistance = snapIfNeeded(mouseLocation.x() - s->xCenter());

  s->xCenterDOF()->setMax(mouseToCenterDistance);
}


void DiagramEditor::handleDragDOFYCtrEnd(Shape* s)
{
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  double mouseToCenterDistance = snapIfNeeded(mouseLocation.y() - s->yCenter());

  s->yCenterDOF()->setMax(mouseToCenterDistance);
}


void DiagramEditor::handleDragDOFWthEnd(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF rightLocation = QPointF(s->xCenter()+cos(angle)*s->xDistance(), s->yCenter()+sin(angle)*s->xDistance());
  double mouseToRightDistance = snapIfNeeded(Utils::distLinePoint(rightLocation, rightLocation + QPointF(sin(-angle), cos(-angle)), mouseLocation));

  s->widthDOF()->setMax(mouseToRightDistance);
}


void DiagramEditor::handleDragDOFHgtEnd(Shape* s)
{
  double angle = Utils::degrToRad(s->angle());
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF topLocation = QPointF(s->xCenter()-sin(angle)*s->yDistance(), s->yCenter()+cos(angle)*s->yDistance());
  double mouseToTopDistance = snapIfNeeded(Utils::distLinePoint(topLocation, topLocation - QPointF(cos(angle), sin(angle)), mouseLocation));

  s->heightDOF()->setMax(mouseToTopDistance);
}


void DiagramEditor::handleDragDOFHge(Shape* s)
{
  QPointF mouseLocation = snapIfNeeded(worldCoordinate(m_lastMouseEvent.posF()));
  QPointF centerLocation = QPointF(s->xCenter(), s->yCenter());
  QPointF relativeLocation = mouseLocation - centerLocation;

  s->setHinge(relativeLocation.x(), relativeLocation.y());
}


void DiagramEditor::handleDragDOFAglEnd(Shape* s)
{
  QPointF mouseLocation = worldCoordinate(m_lastMouseEvent.posF());
  QPointF relativeMouseLocation = mouseLocation - QPointF(s->xCenter()+s->xHinge(), s->yCenter()+s->yHinge());
  double newAngle = Utils::calcAngleDg(relativeMouseLocation.x(), relativeMouseLocation.y());
  double offsetAngle = Utils::calcAngleDg(-s->xHinge(), -s->yHinge());
  newAngle = snapAngleIfNeeded(newAngle);

  s->angleDOF()->setMax(newAngle-offsetAngle);
}


// -- hit detection -------------------------------------------------


void DiagramEditor::processHits(
    GLint hits,
    GLuint buffer[])
{
  GLuint* ptr;
  vector< int > ids;

  ptr = (GLuint*) buffer;

  if (hits > 0)
  {
    // if necassary, advance to closest hit
    if (hits > 1)
    {
      for (int i = 0; i < (hits-1); ++i)
      {
        int number = *ptr;
        ++ptr; // number;
        ++ptr; // z1
        ++ptr; // z2
        for (int j = 0; j < number; ++j)
        {
          ++ptr;  // names
        }
      }
    }

    // last hit
    int number = *ptr;
    ++ptr; // number
    ++ptr; // z1
    ++ptr; // z2

    for (int i = 0; i < number; ++i)
    {
      ids.push_back(*ptr);
      ++ptr;
    }

    handleHits(ids);
  }

  ptr = 0;
}


// -- end -----------------------------------------------------------
