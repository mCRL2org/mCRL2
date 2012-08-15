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
  drgBegIdx1 = NON_EXISTING;
  drgBegIdx2 = NON_EXISTING;
  m_selectionActive = false;
  m_lastSelectedShapeId = -1;

  initMouse();
  initContextMenu();
}


// -- get functions ---------------------------------------------


Shape* DiagramEditor::selectedShape()
{
  if (m_diagram != 0)
  {
    // Return last selected if possible
    if (m_lastSelectedShapeId != -1 && m_lastSelectedShapeId < m_diagram->shapeCount())
    {
      return m_diagram->shape(m_lastSelectedShapeId);
    }

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

  drgBegIdx1 = NON_EXISTING;
  drgBegIdx2 = NON_EXISTING;
}


// -- visualization functions  ----------------------------------


void DiagramEditor::visualize(const bool& inSelectMode)
{
  qDebug() << "visualize(" << inSelectMode << ")";
  if (inSelectMode == true)
  {
    if (m_editMode == EDIT_MODE_SELECT || m_editMode == EDIT_MODE_DOF || m_editMode == EDIT_MODE_NOTE)
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
  }
  else
  {
    clear();
    m_diagram->visualize(inSelectMode, pixelSize());

    if (m_mouseDrag && m_lastMouseEvent.buttons() == Qt::LeftButton)
    {
      double x1, y1;
      double x2, y2;
      double pix;

      QPointF start = worldCoordinate(m_mouseDragStart);
      QPointF stop = worldCoordinate(m_lastMouseEvent.posF());

      x1 = start.x();
      y1 = start.y();
      x2 = stop.x();
      y2 = stop.y();

      if (m_diagram->snapGrid() == true)
      {
        double intv = m_diagram->gridInterval(pixelSize());

        x1 = Utils::rndToNearestMult(x1, intv);
        y1 = Utils::rndToNearestMult(y1, intv);
        x2 = Utils::rndToNearestMult(x2, intv);
        y2 = Utils::rndToNearestMult(y2, intv);
      }

      pix = pixelSize();

      double dX, dY;
      double xC, yC;

      dX = x2-x1;
      dY = y2-y1;

      xC = x1+0.5*dX;
      yC = y1+0.5*dY;

      VisUtils::setColor(VisUtils::darkGray);
      if (m_editMode == EDIT_MODE_SELECT)
      {
        m_selection.setCoords(xC, yC, 0.5*dX, -0.5*dY);
        if (!isAnyShapeSelected())
        {
          VisUtils::drawRect(x1, x2, y1, y2);
        }
      }
      else
      {
        m_selection.setCoords(-1, -1, -1, -1);
        if (m_editMode == EDIT_MODE_RECT)
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


void DiagramEditor::editDof()
{
  Shape* s = selectedShape();
  if (s != 0)
  {
    DofDialog* dofDialog = new DofDialog(m_graph, s, this);
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
    }
  }
}


void DiagramEditor::cutShapes()
{
  // find & cut selected shapes
  QList<Shape*> shapes = selectedShapes();

  if (!shapes.isEmpty()) {
    m_clipBoardList.clear();
    for (int i = 0; i < shapes.size(); i++)
    {
      m_clipBoardList.append(new Shape(*shapes[i]));
      m_diagram->removeShape(shapes[i]->index());
    }
  }
}


void DiagramEditor::copyShapes()
{
  // find & copy selected shapes
  QList<Shape*> shapes = selectedShapes();

  if (!shapes.isEmpty()) {
    m_clipBoardList.clear();
    for (int i = 0; i < shapes.size(); i++)
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



    for (int i = 1; i < m_clipBoardList.size(); i++)
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
}

void DiagramEditor::selectAllShapes()
{
  for (int i = 0; i < m_diagram->shapeCount() ; ++i)
  {
    m_diagram->shape(i)->setDrawMode(Shape::MODE_EDIT);
  }
}

void DiagramEditor::deleteShapes()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; ++i)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->removeShape(i);
    }
  }
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
}

void DiagramEditor::sendToBack()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; ++i)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeToBack(i);
    }
  }
}

void DiagramEditor::bringForward()
{
  for (int i = m_diagram->shapeCount()-1 ; i >= 0 ; ++i)
  {
    if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
    {
      m_diagram->moveShapeForward(i);
    }
  }
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
}


// -- private utility functions ---------------------------------


void DiagramEditor::handleIntersection()
{
  qDebug() << "handleIntersection";
  Shape* s = 0;
  if (!isAnyShapeSelected())  // If not dragging shape, look for intersections
  {
    for (int i = 0; i < m_diagram->shapeCount(); i++)
    {
      s = m_diagram->shape(i);
      double sX1, sY1, sX2, sY2, x1, x2, y1, y2;
      translatePoints(sX1, sY1, sX2, sY2, s->xCenter(), s->yCenter(), s->xDistance(), s->yDistance());
      translatePoints(x1, y1, x2, y2, m_selection.left(), m_selection.top(), m_selection.right(), m_selection.bottom());
      if (!(x1 >= sX2 || x2 <= sX1 || y1 <= sY2 || y2 >= sY1))
      {
        s->setDrawMode(Shape::MODE_EDIT);
      }
      s = 0;
    }
  }
  m_selectionActive = false;
}


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

void DiagramEditor::initContextMenu()
{
  m_popup.clear();
  m_popup.addAction("Edit DOF", this, SLOT(editDof()));
  m_popup.addAction("Edit Note", this, SLOT(editNote()));
  m_popup.addAction("Text Size", this, SLOT(editTextSize()));
  m_popup.addSeparator();
  m_popup.addAction("Cut", this, SLOT(cutShapes()), QKeySequence::Cut);
  m_popup.addAction("Copy", this, SLOT(copyShapes()), QKeySequence::Copy);
  m_popup.addAction("Paste", this, SLOT(copyShapes()), QKeySequence::Paste);
  m_popup.addAction("Select All", this, SLOT(pasteShapes()), QKeySequence::SelectAll);
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
    enabledOptions << "Edit DOF" << "Edit Note" << "Text Size";
  }
  if (shapes.size() > 0)
  {
    enabledOptions << "Cut" << "Copy" << "Delete" <<
                      "Bring to front" << "Send to back" <<
                      "Bring forward" << "Send backward";
  }

  QList<QAction*> actions = m_popup.actions();
  for (int i = 0; i < actions.size(); i++)
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
}


void DiagramEditor::handleHits(const vector< int > &ids)
{
  qDebug() << "handleHits";

  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::LeftButton)
  {
    if (ids.size() == 1)
    {
      deselectAll();
    }
  }
  if (m_lastMouseEvent.type() == QEvent::MouseButtonRelease && m_lastMouseEvent.button() == Qt::LeftButton)
  {

  }
  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress && m_lastMouseEvent.button() == Qt::RightButton)
  {
    if (ids.size() == 1)
    {
      deselectAll();
    }
  }
  if (m_lastMouseEvent.type() == QEvent::MouseButtonRelease && m_lastMouseEvent.button() == Qt::RightButton)
  {
    showContextMenu();
  }
}


void DiagramEditor::handleHitDiagramOnly()
{
  deselectAll();
}

void DiagramEditor::handleHitShape(const size_t& shapeIdx)
{
  size_t sizeShapes = 0;
  if (m_diagram != 0)
  {
    sizeShapes = m_diagram->shapeCount();
  }

  if (shapeIdx != NON_EXISTING && shapeIdx < sizeShapes)
  {
    Shape* s = m_diagram->shape(shapeIdx);

    if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
    {
      if (m_lastMouseEvent.button() == Qt::LeftButton)
      {
        if (m_editMode == EDIT_MODE_SELECT)   // mode
        {
          if (s->drawMode() == Shape::MODE_EDIT)
          {
            if (drgBegIdx1 == NON_EXISTING && drgBegIdx2 == NON_EXISTING)
            {
              s->setModeNormal();
            }
            else
            {
              s->setModeEdit();
              drgBegIdx1 = NON_EXISTING;
              drgBegIdx2 = NON_EXISTING;
            }
          }
          else
          {
            s->setModeEdit();
          }

          for (size_t i = 0; i < sizeShapes; ++i)
            if (i != shapeIdx)
            {
              m_diagram->shape(i)->setModeNormal();
            }
        }
        else if (m_editMode == EDIT_MODE_DOF)
        {
          if (s->drawMode() != Shape::MODE_EDIT_DOF_XCTR &&
              s->drawMode() != Shape::MODE_EDIT_DOF_YCTR &&
              s->drawMode() != Shape::MODE_EDIT_DOF_WTH  &&
              s->drawMode() != Shape::MODE_EDIT_DOF_HGT  &&
              s->drawMode() != Shape::MODE_EDIT_DOF_AGL)
          {
            s->setDrawMode(Shape::MODE_EDIT_DOF_XCTR);
            drgBegIdx1 = NON_EXISTING;
            drgBegIdx2 = NON_EXISTING;
          }


          editDof();


          for (size_t i = 0; i < sizeShapes; ++i)
            if (i != shapeIdx)
            {
              m_diagram->shape(i)->setModeNormal();
            }
        }
        else if (m_editMode == EDIT_MODE_NOTE)
        {
          ////mediator->handleNote( shapeIdx, s->getNote() );
        } // mode
      } // side
      else if (m_lastMouseEvent.button() == Qt::RightButton)
      {
        s->setModeEdit();
        int countSelectedShapes = 0;
        for (size_t i = 0; i < sizeShapes; i++)
        {
          if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
          {
            countSelectedShapes++;
          }
        }
        if (countSelectedShapes > 0)
        {
          deselectAll();
        }
        s->setModeEdit();

        //focus change prohibits mouseup event, simulate it:
        QMouseEvent event(QEvent::MouseButtonRelease, m_lastMouseEvent.pos(), Qt::RightButton, Qt::NoButton, m_lastMouseEvent.modifiers());
        handleMouseEvent(&event);

        // TODO: popup menu
      } // side
    } // click

    update();
    s = 0;
  }
}


void DiagramEditor::handleHitShapeHandle(
    const size_t& shapeIdx,
    const size_t& handleId)
{
  size_t sizeShapes = 0;
  if (m_diagram != 0)
  {
    sizeShapes = m_diagram->shapeCount();
  }

  if (shapeIdx != NON_EXISTING && shapeIdx < sizeShapes)
  {
    Shape* s = m_diagram->shape(shapeIdx);


    if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
    {
      if (m_lastMouseEvent.button() == Qt::LeftButton)
      {
        if (m_editMode != EDIT_MODE_NOTE)
        {
          if (m_mouseDrag)
          {
            drgBegIdx1 = shapeIdx;
            drgBegIdx2 = handleId;
            m_dragDistance = QPoint(0, 0);
          }
          else
          {
            drgBegIdx1 = NON_EXISTING;
            drgBegIdx2 = NON_EXISTING;
          }
        }
      }
      else if (m_lastMouseEvent.button() == Qt::RightButton)
      {
        QPointF pos = worldCoordinate(m_lastMouseEvent.posF());

        /*for ( int i = 0; i < sizeShapes; ++i )
            if ( i != s->getIndex() )
                diagram->getShape(i)->setModeNormal();*/

        //focus change prohibits mouseup event, simulate it:
        QMouseEvent event(QEvent::MouseButtonRelease, m_lastMouseEvent.pos(), Qt::RightButton, Qt::NoButton, m_lastMouseEvent.modifiers());
        handleMouseEvent(&event);

        // TODO: popup menu
      }
    }
    else if (m_lastMouseEvent.type() == QEvent::MouseButtonRelease)
    {
      drgBegIdx1 = NON_EXISTING;
      drgBegIdx2 = NON_EXISTING;

      s->handleHit(handleId);
    }
    else if (m_lastMouseEvent.type() == QEvent::MouseButtonDblClick && m_lastMouseEvent.button() == Qt::LeftButton)
    {
      if (s->drawMode() != Shape::MODE_EDIT_DOF_XCTR &&
          s->drawMode() != Shape::MODE_EDIT_DOF_YCTR &&
          s->drawMode() != Shape::MODE_EDIT_DOF_WTH  &&
          s->drawMode() != Shape::MODE_EDIT_DOF_HGT  &&
          s->drawMode() != Shape::MODE_EDIT_DOF_AGL)
      {
        s->setDrawMode(Shape::MODE_EDIT_DOF_XCTR);
        drgBegIdx1 = NON_EXISTING;
        drgBegIdx2 = NON_EXISTING;
      }
      if (s->shapeType() != Shape::TYPE_NOTE)
      {
        m_editMode = EDIT_MODE_DOF;
        editDof();
      }
      for (size_t i = 0; i < sizeShapes; ++i)
        if (i != shapeIdx)
        {
          m_diagram->shape(i)->setModeNormal();
        }
    }
    s = 0;
  }
}


void DiagramEditor::handleDrag()
{
  qDebug() << "handleDrag";
  size_t sizeShapes = 0;
  if (m_diagram != 0)
  {
    sizeShapes = m_diagram->shapeCount();
  }

  if (drgBegIdx1 != NON_EXISTING && drgBegIdx1 < sizeShapes)
  {
    // do transl & scale here
    Shape* s = m_diagram->shape(drgBegIdx1);

    if (s->drawMode() == Shape::MODE_EDIT)
    {
      if (drgBegIdx2 == Shape::ID_HDL_CTR)
      {
        double xDrag, yDrag;
        handleDragCtr(s, xDrag, yDrag);
        size_t i;
        for (i = 0; i < sizeShapes; i++)
        {
          Shape* otherSelectedShape = m_diagram->shape(i);
          if (drgBegIdx1 != i && otherSelectedShape->drawMode() == Shape::MODE_EDIT)
          {
            double xCtr, yCtr;
            otherSelectedShape->center(xCtr, yCtr);
            xCtr += xDrag;
            yCtr += yDrag;
            otherSelectedShape ->setCenter(xCtr, yCtr);
          }
          otherSelectedShape = 0;
        }
      }
      else if (drgBegIdx2 == Shape::ID_HDL_TOP_LFT)
      {
        handleDragTopLft(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_LFT)
      {
        handleDragLft(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_BOT_LFT)
      {
        handleDragBotLft(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_BOT)
      {
        handleDragBot(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_BOT_RGT)
      {
        handleDragBotRgt(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_RGT)
      {
        handleDragRgt(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_TOP_RGT)
      {
        handleDragTopRgt(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_TOP)
      {
        handleDragTop(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_ROT_RGT)
      {
        handleDragRotRgt(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_ROT_TOP)
      {
        handleDragRotTop(s);
      }
    }
    else if (s->drawMode() == Shape::MODE_EDIT_DOF_XCTR)
    {
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFXCtrEnd(s);
      }
    }
    else if (s->drawMode() == Shape::MODE_EDIT_DOF_YCTR)
    {
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFYCtrEnd(s);
      }
    }
    else if (s->drawMode() == Shape::MODE_EDIT_DOF_WTH)
    {
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFWthEnd(s);
      }
    }
    else if (s->drawMode() == Shape::MODE_EDIT_DOF_HGT)
    {
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFHgtEnd(s);
      }
    }
    else if (s->drawMode() == Shape::MODE_EDIT_DOF_AGL)
    {
      if (drgBegIdx2 == Shape::ID_HDL_HGE)
      {
        handleDragDOFHge(s);
      }
      else if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFAglEnd(s);
      }
    }
    // undo transl & scale here

    update();
    s = 0;
  }
}


void DiagramEditor::handleDragCtr(Shape* s, double& xDrag, double& yDrag)
{
  double xCtr, yCtr;
  double xDFC, yDFC;
  double x,    y;

  QPointF pos = worldCoordinate(m_lastMousePos);
  QPointF eventPos = worldCoordinate(m_lastMouseEvent.posF());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  x = xCtr;
  y = yCtr;

  m_dragDistance += eventPos-pos;

  if (m_diagram->snapGrid() == true)
  {
    x = Utils::rndToNearestMult(x+m_dragDistance.x(), m_diagram->gridInterval(pixelSize()));
    y = Utils::rndToNearestMult(y+m_dragDistance.y(), m_diagram->gridInterval(pixelSize()));
    double x1 = Utils::rndToNearestMult(x - xDFC, m_diagram->gridInterval(pixelSize()));
    double y1 = Utils::rndToNearestMult(y - yDFC, m_diagram->gridInterval(pixelSize()));
    x = x1 + xDFC;
    y = y1 + yDFC;

    if (x != xCtr)
    {
      m_dragDistance.setX(eventPos.x()-x);
    }
    if (y != yCtr)
    {
      m_dragDistance.setY(eventPos.y()-y);
    }
  }
  else
  {
    x += eventPos.x()-pos.x();
    y += eventPos.y()-pos.y();
  }
  xDrag = x - xCtr;
  yDrag = y - yCtr;

  s->setCenter(x, y);
}


void DiagramEditor::handleDragTopLft(Shape* s)
{
  double angl;             // shape's rotation angle in radians
  double xCtr, yCtr;       // center of shape
  double xDFC, yDFC;       // deltas of shape
  double xCur, yCur;       // current mouse position
  double x0,   y0;         // position after translating to shape's center
  double xS,   yS;         // position after rotating to shape's angle
  double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis
  double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->gridInterval(pixelSize());
    xCur = Utils::rndToNearestMult(xCur, itv);
    yCur = Utils::rndToNearestMult(yCur, itv);
  }

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);
  yS = x0*sin(-angl) + y0*cos(-angl);

  hypX = 0.5*(-xDFC-xS);
  adjX = hypX*cos(angl);   // x (-)
  oppX = hypX*sin(angl);   // y (-)

  hypY = 0.5*(yS-yDFC);
  adjY = hypY*cos(angl);   // y (+)
  oppY = hypY*sin(angl);   // x (-)

  s->setDistance(xDFC+hypX,      yDFC+hypY);
  s->setCenter(xCtr-adjX-oppY, yCtr-oppX+adjY);
}


void DiagramEditor::handleDragLft(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double xS;      // position after rotating to shape's angle
  double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = 0.5*(-xDFC-xS);
  adj = hyp*cos(angl);   // x (-)
  opp = hyp*sin(angl);   // y (-)

  s->setDistance(xDFC+hyp, yDFC);
  s->setCenter(xCtr-adj, yCtr-opp);

}


void DiagramEditor::handleDragBotLft(Shape* s)
{
  double angl;             // shape's rotation angle in radians
  double xCtr, yCtr;       // center of shape
  double xDFC, yDFC;       // deltas of shape
  double xCur, yCur;       // current mouse position
  double x0,   y0;         // position after translating to shape's center
  double xS,   yS;         // position after rotating to shape's angle
  double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis
  double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->gridInterval(pixelSize());
    xCur = Utils::rndToNearestMult(xCur, itv);
    yCur = Utils::rndToNearestMult(yCur, itv);
  }

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);
  yS = x0*sin(-angl) + y0*cos(-angl);

  hypX = 0.5*(-xDFC-xS);
  adjX = hypX*cos(angl);   // x (-)
  oppX = hypX*sin(angl);   // y (-)

  hypY = 0.5*(-yDFC-yS);
  adjY = hypY*cos(angl);   // y (-)
  oppY = hypY*sin(angl);   // x (+)

  s->setDistance(xDFC+hypX,      yDFC+hypY);
  s->setCenter(xCtr-adjX+oppY, yCtr-oppX-adjY);

}


void DiagramEditor::handleDragBot(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double yS;      // position after rotating to shape's angle
  double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = 0.5*(-yDFC-yS);
  adj = hyp*cos(angl);   // y (-)
  opp = hyp*sin(angl);   // x (+)

  s->setDistance(xDFC,     yDFC+hyp);
  s->setCenter(xCtr+opp, yCtr-adj);
}


void DiagramEditor::handleDragBotRgt(Shape* s)
{
  double angl;             // shape's rotation angle in radians
  double xCtr, yCtr;       // center of shape
  double xDFC, yDFC;       // deltas of shape
  double xCur, yCur;       // current mouse position
  double x0,   y0;         // position after translating to shape's center
  double xS,   yS;         // position after rotating to shape's angle
  double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis
  double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->gridInterval(pixelSize());
    xCur = Utils::rndToNearestMult(xCur, itv);
    yCur = Utils::rndToNearestMult(yCur, itv);
  }

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);
  yS = x0*sin(-angl) + y0*cos(-angl);

  hypX = 0.5*(xS-xDFC);
  adjX = hypX*cos(angl);   // x (+)
  oppX = hypX*sin(angl);   // y (+)

  hypY = 0.5*(-yDFC-yS);
  adjY = hypY*cos(angl);   // y (-)
  oppY = hypY*sin(angl);   // x (+)

  s->setDistance(xDFC+hypX,      yDFC+hypY);
  s->setCenter(xCtr+adjX+oppY, yCtr+oppX-adjY);
}


void DiagramEditor::handleDragRgt(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double xS;      // position after rotating to shape's angle
  double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = 0.5*(xS-xDFC);
  adj = hyp*cos(angl);   // x (+)
  opp = hyp*sin(angl);   // y (+)

  s->setDistance(xDFC+hyp, yDFC);
  s->setCenter(xCtr+adj, yCtr+opp);
}


void DiagramEditor::handleDragTopRgt(Shape* s)
{
  double angl;             // shape's rotation angle in radians
  double xCtr, yCtr;       // center of shape
  double xDFC, yDFC;       // deltas of shape
  double xCur, yCur;       // current mouse position
  double x0,   y0;         // position after translating to shape's center
  double xS,   yS;         // position after rotating to shape's angle
  double hypX, adjX, oppX; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's x-axis
  double hypY, adjY, oppY; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->gridInterval(pixelSize());
    xCur = Utils::rndToNearestMult(xCur, itv);
    yCur = Utils::rndToNearestMult(yCur, itv);
  }

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);
  yS = x0*sin(-angl) + y0*cos(-angl);

  hypX = 0.5*(xS-xDFC);
  adjX = hypX*cos(angl);   // x (+)
  oppX = hypX*sin(angl);   // y (+)

  hypY = 0.5*(yS-yDFC);
  adjY = hypY*cos(angl);   // y (+)
  oppY = hypY*sin(angl);   // x (-)

  s->setDistance(xDFC+hypX,      yDFC+hypY);
  s->setCenter(xCtr+adjX-oppY, yCtr+oppX+adjY);
}


void DiagramEditor::handleDragTop(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double yS;      // position after rotating to shape's angle
  double hyp, adj, opp; // hypotenuse, adjacent & opposite sides
  // for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = 0.5*(yS-yDFC);
  adj = hyp*cos(angl);   // y (+)
  opp = hyp*sin(angl);   // x (-)

  s->setDistance(xDFC,     yDFC+hyp);
  s->setCenter(xCtr-opp, yCtr+adj);
}


void DiagramEditor::handleDragRotRgt(Shape* s)
{
  double aglRd;      // shape's rotation angle in radians
  double aglDg;      // shape's rotation angle in degrees
  double xCtr, yCtr; // center of shape
  double xDFC, yDFC; // deltas of shape
  double xCur, yCur; // current mouse position
  double x0,   y0;   // position after translating to shape's center
  double xS,   yS;   // position after rotating to shape's angle

  // get shape's geometry
  aglDg = s->angle();

  aglRd = Utils::degrToRad(aglDg);
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-aglRd) - y0*sin(-aglRd);
  yS = x0*sin(-aglRd) + y0*cos(-aglRd);

  aglDg = aglDg + Utils::calcAngleDg(xS, yS);

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->angleInterval();
    aglDg = Utils::rndToNearestMult(aglDg, itv);
  }

  if (xDFC < 0)
    // shape reflected about y-axis
  {
    aglDg += 180.0;
  }
  if (aglDg >= 360.0)
  {
    aglDg -= 360.0;
  }

  // update angle
  s->setAngle(aglDg);
}


void DiagramEditor::handleDragRotTop(Shape* s)
{
  double aglRd;      // shape's rotation angle in radians
  double aglDg;      // shape's rotation angle in degrees
  double xCtr, yCtr; // center of shape
  double xDFC, yDFC; // deltas of shape
  double xCur, yCur; // current mouse position
  double x0,   y0;   // position after translating to shape's center
  double xS,   yS;   // position after rotating to shape's angle

  // get shape's geometry
  aglDg = s->angle();
  aglRd = Utils::degrToRad(aglDg);
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-aglRd) - y0*sin(-aglRd);
  yS = x0*sin(-aglRd) + y0*cos(-aglRd);

  aglDg = aglDg + Utils::calcAngleDg(xS, yS) - 90.0;

  if (m_diagram->snapGrid() == true)
  {
    double itv = m_diagram->angleInterval();
    aglDg = Utils::rndToNearestMult(aglDg, itv);
  }

  if (yDFC < 0)
    // shape reflected about y-axis
  {
    aglDg += 180.0;
  }
  if (aglDg >= 360.0)
  {
    aglDg -= 360.0;
  }

  // update angle
  s->setAngle(aglDg);
}


void DiagramEditor::handleDragDOFXCtrEnd(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double x0;            // x position after translating to shape's center

  // get shape's geometry
  s->center(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    x0 = Utils::rndToNearestMult(xCtr+x0, itv)-xCtr;
  }

  s->xCenterDOF()->setMax(x0);
}


void DiagramEditor::handleDragDOFYCtrEnd(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double y0;            // y position after translating to shape's center

  // get shape's geometry
  s->center(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  y0 = yCur-yCtr;

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    y0 = Utils::rndToNearestMult(yCtr+y0, itv)-yCtr;
  }

  s->yCenterDOF()->setMax(y0);
}


void DiagramEditor::handleDragDOFWthEnd(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double xS;      // position after rotating to shape's angle
  double hyp;           // hypotenuse, for movement parallel to shape's x-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = xS-xDFC;
  s->widthDOF()->setMax(hyp);
}


void DiagramEditor::handleDragDOFHgtEnd(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double yS;      // position after rotating to shape's angle
  double hyp;           // hypotenuse, for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->angle());
  s->center(xCtr, yCtr);
  s->distance(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = yS-yDFC;
  s->heightDOF()->setMax(hyp);
}


void DiagramEditor::handleDragDOFHge(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // y position after translating to shape's center

  // get shape's geometry
  s->center(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  y0 = yCur-yCtr;
  x0 = xCur-xCtr;

  if (m_diagram->snapGrid() == true)
  {
    double itv  = m_diagram->gridInterval(pixelSize());
    x0 = Utils::rndToNearestMult(xCtr+x0, itv)-xCtr;
    y0 = Utils::rndToNearestMult(yCtr+y0, itv)-yCtr;
  }

  s->setHinge(x0, y0);
}


void DiagramEditor::handleDragDOFAglEnd(Shape* s)
{
  double xHge, yHge, xCtr, yCtr;
  double dstHgeCtr;
  double aglRef, aglTot;
  double xCur, yCur, xRelHge, yRelHge;

  // distance from hinge to center
  s->hinge(xHge, yHge);
  s->center(xCtr, yCtr);
  dstHgeCtr = Utils::dist(xCtr+xHge, yCtr+yHge, xCtr, yCtr);
  // angle center relative to hinge
  if (dstHgeCtr == 0)
  {
    aglRef = 0;
  }
  else
  {
    aglRef = Utils::calcAngleDg(-xHge, -yHge);
  }

  // mouse position relative to hinge
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();


  xRelHge = xCur-(xCtr+xHge);
  yRelHge = yCur-(yCtr+yHge);

  /*
  // rotate to 'normal' orientation, find x & y
  double xTmp, yTmp;
  xS = x0*cos( -angl ) - y0*sin( -angl );
  yS = x0*sin( -angl ) + y0*cos( -angl );
  */

  aglTot = Utils::calcAngleDg(xRelHge, yRelHge);

  if (m_diagram->snapGrid() == true)
  {
    double itvAgl  = m_diagram->angleInterval();
    aglTot = Utils::rndToNearestMult(aglTot, itvAgl);
  }

  s->angleDOF()->setMax(aglTot-aglRef);
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
