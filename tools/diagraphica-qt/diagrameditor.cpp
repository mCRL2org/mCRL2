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
  Visualizer(parent, g),
  m_dofDialog(this)
{
  //setClearColor( 0.44, 0.59, 0.85 );
  setClearColor(0.65, 0.79, 0.94);

  m_diagram     = new Diagram(this);
  m_editMode    = EDIT_MODE_SELECT;
  drgBegIdx1 = NON_EXISTING;
  drgBegIdx2 = NON_EXISTING;
  m_selectionActive = false;
  lastSelectedShapeId = NON_EXISTING;

  initMouse();

  m_dofDialog.setCurrentGraph(g);
}

// -- set data functions --------------------------------------------


void DiagramEditor::setDiagram(Diagram* dgrm)
{
  delete m_diagram;
  m_diagram = dgrm;
  m_dofDialog.setCurrentShape(0);
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


// -- get functions ---------------------------------------------


Shape *DiagramEditor::selectedShape()
{
  if (m_diagram != 0)
  {
    size_t sizeShapes = m_diagram->shapeCount();
    for (size_t i = 0; i < sizeShapes; ++i)
    {
      if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
      {
        return m_diagram->shape(i);
      }
    }
  }
  return 0;
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


// -- event handlers --------------------------------------------


void DiagramEditor::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  if (e->type() != QEvent::MouseMove)
  {
    if (m_editMode == EDIT_MODE_SELECT ||
        (m_editMode == EDIT_MODE_DOF && e->button() == Qt::LeftButton &&
         (e->type() == QEvent::MouseButtonPress ||
          e->type() == QEvent::MouseButtonRelease)))
    {
      // redraw in select mode
      updateGL(true);
    }
    if (e->button() == Qt::LeftButton && e->type() == QEvent::MouseButtonRelease)
    {
      handleIntersection();
      if (m_editMode != EDIT_MODE_SELECT && m_editMode != EDIT_MODE_DOF)
      {
        double x1, x2, y1, y2;
        double dX, dY;
        double pix;

        pix = pixelSize();

        QPointF start = worldCoordinate(m_mouseDragStart);
        QPointF stop = worldCoordinate(e->posF());

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

        dX = x2-x1;
        dY = y2-y1;

        if (Utils::abs(dX) < Shape::minSzeHnt*pix &&
            Utils::abs(dY) < Shape::minSzeHnt*pix)
        {
          dX = Shape::minSzeHnt*pix;
          dY = Shape::minSzeHnt*pix;
        }

        double xC, yC, xDFC, yDFC;
        xC = x1+0.5*dX;
        yC = y1+0.5*dY;
        xDFC = 0.5*dX;
        yDFC = -0.5*dY;

        QRectF gridCoordinates = m_diagram->gridCoordinates();
        double xLeft = gridCoordinates.left();
        double xRight = gridCoordinates.right();
        double yTop = gridCoordinates.top();
        double yBottom = gridCoordinates.bottom();
        if (xLeft > (xC - xDFC))
        {
          xC = xLeft + xDFC;
        }
        else if (xRight < (xC + xDFC))
        {
          xC = xRight - xDFC;
        }
        if (yBottom > (yC - yDFC))
        {
          yC = yBottom + yDFC;
        }
        else if (yTop < (yC + yDFC))
        {
          yC = yTop - yDFC;
        }

        Shape* s = new Shape(m_diagram, m_diagram->shapeCount(),
                             xC,        yC,
                             0.5*dX,    -0.5*dY,
                             0.0,       Shape::TYPE_RECT,
                             0.0,       0.0);

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
          //mediator->handleNote(s->getIndex() , s->getNote());
        }

        m_diagram->addShape(s);
        s = 0;

        // undo transl & scale here
      }
    }
  }
  else
  {
    if (m_mouseDrag && (m_editMode  == EDIT_MODE_SELECT || m_editMode == EDIT_MODE_DOF))
    {
      qDebug() << "drgBegIdx1" << drgBegIdx1 << "drgBegIdx2" << drgBegIdx1;
      if (drgBegIdx1 == NON_EXISTING && drgBegIdx2 == NON_EXISTING)
      {
        m_selectionActive = true;
        // redraw in select mode
        updateGL(true);
      }
      else
      {
        qDebug() << "drgBegIdx1 exists";
        handleDrag();
      }
    }

    m_lastMousePos = e->pos();
  }
  // redraw in render mode
  updateGL();
}

void DiagramEditor::handleKeyEvent(QKeyEvent* e)
{
  Visualizer::handleKeyEvent(e);
  if (m_editMode == EDIT_MODE_SELECT)
  {
    if (e->type() == QEvent::KeyPress)
    {
      if (e->matches(QKeySequence::SelectAll))
      {
        handleSelectAll();
      }
      else if (e->matches(QKeySequence::Cut))
      {
        handleCut();
      }
      else if (e->matches(QKeySequence::Copy))
      {
        handleCopy();
      }
      else if (e->matches(QKeySequence::Paste))
      {
        handlePaste();
      }
    }
    if (e->type() == QEvent::KeyRelease)
    {
      if (e->matches(QKeySequence::Delete))
      {
        handleDelete();
      }
    }
  }
  // redraw in render mode
  updateGL();
}


void DiagramEditor::handleHits(const vector< int > &ids)
{
  // only diagram was hit
  if (ids.size() == 1)
  {
    lastSelectedShapeId = NON_EXISTING;
    handleHitDiagramOnly();
  }
  // shape was hit
  else if (ids.size() == 2 && !m_selectionActive)
  {
    lastSelectedShapeId = ids[1];
    handleHitShape(ids[1]);
  }
  else if (ids.size() == 3)
  {
    lastSelectedShapeId = ids[1];
    handleHitShapeHandle(ids[1], ids[2]);
  }
}


void DiagramEditor::handleHitDiagramOnly()
{
  //Deselect all and show popup menu
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


          //TODO: Show DOF Dialog


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
        QPointF pos = worldCoordinate(m_lastMouseEvent.posF());
        xPaste = pos.x();
        yPaste = pos.y();
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
        xPaste = pos.x();
        yPaste = pos.y();

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
        //TODO: Show DOF Dialog

        m_editMode = EDIT_MODE_DOF;
        //mediator->handleEditModeDOF(this);
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


//void DiagramEditor::handleShowVariable(const string& variable, const int& variableId)
//{
//  Shape* selectedShape = 0;
//  if (lastSelectedShapeId != NON_EXISTING)
//  {
//    selectedShape = m_diagram->shape(lastSelectedShapeId);
//    selectedShape->setCheckedId(variableId);
//    string variableName = variable;
//    size_t i = variable.find(":",0); // for getting variable's name
//    variableName = variable.substr(0, i);
//    i++;
//    string variableValue = variable;
//    variableValue.erase(0,i);
//    selectedShape->setNote(variableName);
//    selectedShape->setVariableName(variableName);
//    if (variable.length() > variableValue.length())  // If their length is same, it means variable doesn't have a value
//    {
//      selectedShape->setVariableValue(variableValue);
//    }
//    else
//    {
//      selectedShape->setVariableValue("");
//    }
//  }
//}


//void DiagramEditor::handleShowNote(const string& variable, const size_t& shapeId)
//{
//  Shape* selectedShape = 0;
//  selectedShape = m_diagram->shape(shapeId);
//  selectedShape->setNote(variable);
//  if (selectedShape->shapeType() == Shape::TYPE_NOTE)
//  {
//    double letterCount = selectedShape->note().length();
//    double x = letterCount * 0.009;
//    double y = 0.02;
//    selectedShape->setDistance(x, y);
//  }
//}


//void DiagramEditor::handleAddText(string& variable, size_t& shapeId)
//{
//  Shape* selectedShape = 0;
//  shapeId = lastSelectedShapeId;
//  selectedShape = m_diagram->shape(shapeId);
//  variable = selectedShape->note();
//  selectedShape = 0;
//}


//void DiagramEditor::handleTextSize(size_t& textSize, size_t& shapeId)
//{
//  Shape* selectedShape = 0;
//  shapeId = lastSelectedShapeId;
//  selectedShape = m_diagram->shape(shapeId);
//  if (selectedShape != 0)
//  {
//    textSize = selectedShape->textSize();
//    selectedShape = 0;
//  }
//}


//void DiagramEditor::handleSetTextSize(size_t& textSize, size_t& shapeId)
//{
//  Shape* selectedShape = 0;
//  selectedShape = m_diagram->shape(shapeId);
//  selectedShape->setTextSize(textSize);
//}


void DiagramEditor::handleCut()
{
  Shape* origShape = 0;
  Shape* copyShape = 0;
  bool shapeSelected = false;

  // find & copy selected shape
  for (int i = 0; i < m_diagram->shapeCount(); ++i)
  {
    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
    {
      if (!shapeSelected)
      {
        shapeSelected = true;
        clearClipBoard();
      }
      origShape = m_diagram->shape(i);

      // invoke copy constructor
      copyShape = new Shape(*origShape);
      // delete original shape
      m_diagram->removeShape(origShape->index());
      i--; // decrement index; because deletion of the shape decrements the size of the diagram

      clipBoardList.push_back(copyShape);
      origShape = 0;
      copyShape = 0;
    }
  }
}


void DiagramEditor::handleCopy()
{
  Shape* origShape = 0;
  Shape* copyShape = 0;
  bool shapeSelected = false;

  // find & copy selected shape
  for (int i = 0; i < m_diagram->shapeCount(); ++i)
  {
    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
    {
      if (!shapeSelected)
      {
        shapeSelected = true;
        clearClipBoard();
      }
      origShape = m_diagram->shape(i);

      // invoke copy constructor
      copyShape = new Shape(*origShape);

      clipBoardList.push_back(copyShape);
      origShape = 0;
      copyShape = 0;
    }
  }
}


void DiagramEditor::clearClipBoard()
{
  size_t size = clipBoardList.size();
  for (size_t i = 0; i < size; i++)
  {
    clipBoardList[i] = 0;
  }
  clipBoardList.clear();
}


void DiagramEditor::handlePaste()
{
  if (clipBoardList.size() > 0)
  {
    // deselect all other shapes
    for (int i = 0; i < m_diagram->shapeCount(); ++i)
      if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
      {
        m_diagram->shape(i)->setModeNormal();
      }

    size_t size = clipBoardList.size();
    double xC, yC, xCFirst, yCFirst;
    clipBoardList[0]->center(xCFirst, yCFirst);
    for (size_t i = 0; i < size; i++)
    {
      // update index of clipboard shape
      clipBoardList[i]->setIndex(m_diagram->shapeCount());

      // update clipboard shape
      if (i == 0) // Paste the first selected shape to the clicked position
      {
        clipBoardList[i]->setCenter(xPaste, yPaste);
      }
      else // Paste other shapes relative to their position with respect to first shape
      {
        double distanceX, distanceY, x1, x2, y1, y2;
        clipBoardList[0]->center(x1, y1);
        clipBoardList[i]->center(x2, y2);

        // calculate the distance between the first selected shape and the current shape
        distanceX = xCFirst - x2;
        distanceY = yCFirst - y2;
        if (x2 > xCFirst)
        {
          distanceX = x2 - xCFirst;
        }
        if (y2 > yCFirst)
        {
          distanceY = y2 - yCFirst;
        }

        xC = xPaste + distanceX; // Calculate new center of the selected shape according to the distance between the first shape
        yC = yPaste + distanceY;

        clipBoardList[i]->setCenter(xC, yC);
      }
      clipBoardList[i]->setModeEdit();

      // add clipboard shape to diagram
      m_diagram->addShape(clipBoardList[i]);
      // make another copy of clipboard shape
      clipBoardList[i] = new Shape(*clipBoardList[i]);
    }
  }
}


void DiagramEditor::handleDelete()
{
  QList<int> toDelete;
  // get indices to delete
  {
    for (int i = 0; i < m_diagram->shapeCount() ; ++i)
    {
      /*
      if ( diagram->getShape(i)->getMode() == Shape::MODE_EDT_CTR_DFC ||
      diagram->getShape(i)->getMode() == Shape::MODE_EDT_HGE_AGL )
      */
      if (m_diagram->shape(i)->drawMode() == Shape::MODE_EDIT)
      {
        toDelete.append(i);
      }
    }
  }

  // delete shapes
  {
    for (int i = 0; i < toDelete.size(); ++i)
    {
      m_diagram->removeShape(toDelete[i]);
    }
  }
}


void DiagramEditor::handleSelectAll()
{
  for (int i = 0; i < m_diagram->shapeCount() ; ++i)
  {
    m_diagram->shape(i)->setDrawMode(Shape::MODE_EDIT);
  }
}


//void DiagramEditor::handleBringToFront()
//{
//  Shape* s = 0;
//  for (size_t i = 0; i < m_diagram->shapeCount() && s == 0; ++i)
//    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
//    {
//      s = m_diagram->shape(i);
//    }

//  if (s != 0)
//  {
//    m_diagram->moveShapeToBack(s->index());
//  }

//  s = 0;
//}


//void DiagramEditor::handleSendToBack()
//{
//  Shape* s = 0;
//  for (size_t i = 0; i < m_diagram->shapeCount() && s == 0; ++i)
//    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
//    {
//      s = m_diagram->shape(i);
//    }

//  if (s != 0)
//  {
//    m_diagram->moveShapeToFront(s->index());
//  }

//  s = 0;
//}


//void DiagramEditor::handleBringForward()
//{
//  Shape* s = 0;
//  for (size_t i = 0; i < m_diagram->shapeCount() && s == 0; ++i)
//    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
//    {
//      s = m_diagram->shape(i);
//    }

//  if (s != 0)
//  {
//    m_diagram->moveShapeBackward(s->index());
//  }

//  s = 0;
//}


//void DiagramEditor::handleSendBackward()
//{
//  Shape* s = 0;
//  for (size_t i = 0; i < m_diagram->shapeCount() && s == 0; ++i)
//    if (m_diagram->shape(i)->drawMode() != Shape::MODE_NORMAL)
//    {
//      s = m_diagram->shape(i);
//    }

//  if (s != 0)
//  {
//    m_diagram->moveShapeForward(s->index());
//  }

//  s = 0;
//}


void DiagramEditor::handleEditDOF()
{
  Shape* s = 0;
  if (lastSelectedShapeId != NON_EXISTING)
  {
    s = m_diagram->shape(lastSelectedShapeId);
  }
  if (s != 0)
  {
    m_dofDialog.setCurrentShape(s);
    m_dofDialog.show();
  }
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


// -- private utility functions -------------------------------------



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


void DiagramEditor::handleDragCtr(Shape* s, double& xDrag, double& yDrag)
{
  qDebug() << "handleDragCtr";
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
