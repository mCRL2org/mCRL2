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

using namespace std;


// -- init static variables -----------------------------------------
int DiagramEditor::szeTxt = 12;


// -- constructors and destructor -----------------------------------


DiagramEditor::DiagramEditor(
  QWidget *parent,
  Mediator* m,
  Graph* g)
  : Visualizer(parent, m, g)
{
  //setClearColor( 0.44, 0.59, 0.85 );
  setClearColor(0.65, 0.79, 0.94);

  diagram     = new Diagram(m/*, c*/);
  editMode    = EDIT_MODE_SELECT;
  drgBegIdx1 = -1;
  drgBegIdx2 = -1;
  selectedX1 = -1;
  selectedX2 = -1;
  selectedY1 = -1;
  selectedY2 = -1;
  selection = false;
  lastSelectedShapeId = NON_EXISTING;

  initMouse();

  clipBoardShape = NULL;
}


DiagramEditor::~DiagramEditor()
{
  if (diagram != NULL)
  {
    delete diagram;
    diagram = NULL;
  }

  if (clipBoardShape != NULL)
  {
    delete clipBoardShape;
    clipBoardShape = NULL;
  }
}


// -- set data functions --------------------------------------------


void DiagramEditor::setDiagram(Diagram* dgrm)
{
  delete diagram;
  diagram = dgrm;
}


void DiagramEditor::setEditModeSelect()
{
  editMode = EDIT_MODE_SELECT;

  for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
  {
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      diagram->getShape(i)->setModeEdit();
    }
  }

  update();
}


void DiagramEditor::setEditModeNote()
{
  editMode = EDIT_MODE_NOTE;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeDOF()
{
  editMode = EDIT_MODE_DOF;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeRect()
{
  editMode = EDIT_MODE_RECT;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeEllipse()
{
  editMode = EDIT_MODE_ELLIPSE;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeLine()
{
  editMode = EDIT_MODE_LINE;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeArrow()
{
  editMode = EDIT_MODE_ARROW;
  deselectAll();
  update();
}


void DiagramEditor::setEditModeDArrow()
{
  editMode = EDIT_MODE_DARROW;
  deselectAll();
  update();
}


void DiagramEditor::setShowGrid(const bool& flag)
{
  diagram->setShowGrid(flag);
  update();
}


void DiagramEditor::setSnapGrid(const bool& flag)
{
  diagram->setSnapGrid(flag);
  update();
}


void DiagramEditor::setFillCol()
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->setFillColor(mediator->getColor(s->getFillColor()));

    update();
  }

  s = NULL;
}


void DiagramEditor::setLineCol()
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->setLineColor(mediator->getColor(s->getLineColor()));

    update();
  }

  s = NULL;
}


void DiagramEditor::handleIntersection()
{
  size_t shapeCount = diagram->getSizeShapes();
  Shape* s = NULL;
  if (!isAnyShapeSelected())  // If not dragging shape, look for intersections
  {
    for (size_t i = 0; i < shapeCount; i++)
    {
      s = diagram->getShape(i);
      double sX1 = s->getXCtr() - s->getXDFC();
      double sY1 = s->getYCtr() + s->getYDFC();
      double sX2 = s->getXCtr() + s->getXDFC();
      double sY2 = s->getYCtr() - s->getYDFC();
      double x1 = selectedX1 - selectedX2;
      double y1 = selectedY1 + selectedY2;
      double x2 = selectedX1 + selectedX2;
      double y2 = selectedY1 - selectedY2;
      translatePoints(sX1, sY1, sX2, sY2, s->getXCtr(), s->getYCtr(), s->getXDFC(), s->getYDFC());
      translatePoints(x1, y1, x2, y2, selectedX1, selectedY1, selectedX2, selectedY2);
      if (!(x1 >= sX2 || x2 <= sX1 || y1 <= sY2 || y2 >= sY1))
      {
        s->setMode(Shape::MODE_EDIT);
      }
      s = NULL;
    }
  }
  selection = false;
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


bool DiagramEditor::isAnyShapeSelected()
{
  if (diagram != NULL)
  {
    size_t sizeShapes = diagram->getSizeShapes();
    for (size_t i = 0; i < sizeShapes; ++i)
    {
      if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT)
      {
        return true;
      }
    }
    return false;
  }
  return false;
}


void DiagramEditor::handleDOFSel(const size_t& DOFIdx)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    if (DOFIdx == s->getDOFXCtr()->getIndex())
    {
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaDeactivate();
      s->setModeEdtDOFXCtr();
    }
    else if (DOFIdx == s->getDOFYCtr()->getIndex())
    {
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaDeactivate();
      s->setModeEdtDOFYCtr();
    }
    else if (DOFIdx == s->getDOFHgt()->getIndex())
    {
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaDeactivate();
      s->setModeEdtDOFHgt();
    }
    else if (DOFIdx == s->getDOFWth()->getIndex())
    {
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaDeactivate();
      s->setModeEdtDOFWth();
    }
    else if (DOFIdx == s->getDOFAgl()->getIndex())
    {
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaDeactivate();
      s->setModeEdtDOFAgl();
    }
    else if (DOFIdx == s->getDOFCol()->getIndex())
    {
      s->setModeEdtDOFCol();
      mediator->handleDOFColActivate();
      mediator->handleDOFOpaDeactivate();
    }
    else if (DOFIdx == s->getDOFOpa()->getIndex())
    {
      s->setModeEdtDOFOpa();
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaActivate();
    }
    else if (DOFIdx == s->getDOFText()->getIndex())
    {
      s->setModeEdtDOFText();
      mediator->handleDOFColDeactivate();
      mediator->handleDOFOpaActivate();
    }
    s = NULL;

    update();
  }
}


void DiagramEditor::handleDOFSetTextStatus(
  const size_t& DOFIdx,
  const int& status)
{
  Shape* s = NULL;
  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    if (DOFIdx == s->getDOFXCtr()->getIndex())
    {
      s->getDOFXCtr()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFYCtr()->getIndex())
    {
      s->getDOFYCtr()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFHgt()->getIndex())
    {
      s->getDOFHgt()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFWth()->getIndex())
    {
      s->getDOFWth()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFAgl()->getIndex())
    {
      s->getDOFAgl()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFCol()->getIndex())
    {
      s->getDOFCol()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFOpa()->getIndex())
    {
      s->getDOFOpa()->setTextStatus(status);
    }
    else if (DOFIdx == s->getDOFText()->getIndex())
    {
      s->getDOFText()->setTextStatus(status);
    }
    s = NULL;
  }
}


int DiagramEditor::handleDOFGetTextStatus(const size_t& DOFIdx)
{
  int result = -1;

  Shape* s = NULL;
  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    if (DOFIdx == s->getDOFXCtr()->getIndex())
    {
      result = s->getDOFXCtr()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFYCtr()->getIndex())
    {
      result = s->getDOFYCtr()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFHgt()->getIndex())
    {
      result = s->getDOFHgt()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFWth()->getIndex())
    {
      result = s->getDOFWth()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFAgl()->getIndex())
    {
      result = s->getDOFAgl()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFCol()->getIndex())
    {
      result = s->getDOFCol()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFOpa()->getIndex())
    {
      result = s->getDOFOpa()->getTextStatus();
    }
    else if (DOFIdx == s->getDOFText()->getIndex())
    {
      result = s->getDOFText()->getTextStatus();
    }
  }

  return result;
}


void DiagramEditor::handleDOFColAdd(
  const double& hue,
  const double& y)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->setModeEdtDOFCol();
    mediator->handleDOFColActivate();
    mediator->handleDOFOpaDeactivate();
    s->getDOFCol()->addValue(hue);
    s->addDOFColYValue(y);
  }

  s = NULL;
}


void DiagramEditor::handleDOFColUpdate(
  const size_t& idx,
  const double& hue,
  const double& y)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->getDOFCol()->setValue(idx, hue);
    s->setDOFColYValue(idx, y);
  }

  s = NULL;
}


void DiagramEditor::handleDOFColClear(
  const size_t& idx)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    if (s->getDOFCol()->getSizeValues() > 2)
    {
      s->getDOFCol()->clearValue(idx);
      s->clearDOFColYValue(idx);

      vector< double > hues;
      vector< double > yVals;

      s->getDOFCol()->getValues(hues);
      s->getDOFColYValues(yVals);

      mediator->handleDOFColSetValuesEdt(hues, yVals);
    }
  }

  s = NULL;
}


void DiagramEditor::handleDOFOpaAdd(
  const double& hue,
  const double& y)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->setModeEdtDOFOpa();
    mediator->handleDOFColDeactivate();
    mediator->handleDOFOpaActivate();
    s->getDOFOpa()->addValue(hue);
    s->addDOFOpaYValue(y);
  }

  s = NULL;
}


void DiagramEditor::handleDOFOpaUpdate(
  const size_t& idx,
  const double& opa,
  const double& y)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    s->getDOFOpa()->setValue(idx, opa);
    s->setDOFOpaYValue(idx, y);
  }

  s = NULL;
}


void DiagramEditor::handleDOFOpaClear(
  const size_t& idx)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    if (s->getDOFOpa()->getSizeValues() > 2)
    {
      s->getDOFOpa()->clearValue(idx);
      s->clearDOFOpaYValue(idx);

      vector< double > opas;
      vector< double > yVals;

      s->getDOFOpa()->getValues(opas);
      s->getDOFOpaYValues(yVals);

      mediator->handleDOFOpaSetValuesEdt(opas, yVals);
    }
  }

  s = NULL;
}


void DiagramEditor::setLinkDOFAttr(
  const size_t& DOFIdx,
  const size_t& attrIdx)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    Attribute* a = graph->getAttribute(attrIdx);
    DOF* dof = NULL;

    if (DOFIdx == s->getDOFXCtr()->getIndex())
    {
      dof = s->getDOFXCtr();
    }
    else if (DOFIdx == s->getDOFYCtr()->getIndex())
    {
      dof = s->getDOFYCtr();
    }
    else if (DOFIdx == s->getDOFWth()->getIndex())
    {
      dof = s->getDOFWth();
    }
    else if (DOFIdx == s->getDOFHgt()->getIndex())
    {
      dof = s->getDOFHgt();
    }
    else if (DOFIdx == s->getDOFAgl()->getIndex())
    {
      dof = s->getDOFAgl();
    }
    else if (DOFIdx == s->getDOFCol()->getIndex())
    {
      dof = s->getDOFCol();
    }
    else if (DOFIdx == s->getDOFOpa()->getIndex())
    {
      dof = s->getDOFOpa();
    }
    else if (DOFIdx == s->getDOFText()->getIndex())
    {
      dof = s->getDOFText();
      if (a->getSizeCurValues() > 0)
      {
        s->setVariable(a->getCurValue(0)->getValue());
        s->setNote("");
      }
      else
      {
        s->setNote(a->getName());
        s->setVariable("");
      }

    }

    if (dof != NULL)
    {
      dof->setAttribute(a);
    }

    displDOFInfo(s);

    a   = NULL;
    dof = NULL;
  }

  s = NULL;
}


void DiagramEditor::clearLinkDOFAttr(const size_t& DOFIdx)
{
  Shape* s = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_XCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_YCTR ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_HGT  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_WTH  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_AGL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_COL  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_OPA  ||
        diagram->getShape(i)->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      s = diagram->getShape(i);
      break;
    }
  }

  if (s != NULL)
  {
    DOF* dof = NULL;

    if (DOFIdx == s->getDOFXCtr()->getIndex())
    {
      dof = s->getDOFXCtr();
    }
    else if (DOFIdx == s->getDOFYCtr()->getIndex())
    {
      dof = s->getDOFYCtr();
    }
    else if (DOFIdx == s->getDOFWth()->getIndex())
    {
      dof = s->getDOFWth();
    }
    else if (DOFIdx == s->getDOFHgt()->getIndex())
    {
      dof = s->getDOFHgt();
    }
    else if (DOFIdx == s->getDOFAgl()->getIndex())
    {
      dof = s->getDOFAgl();
    }
    else if (DOFIdx == s->getDOFCol()->getIndex())
    {
      dof = s->getDOFCol();
    }
    else if (DOFIdx == s->getDOFOpa()->getIndex())
    {
      dof = s->getDOFOpa();
    }
    else if (DOFIdx == s->getDOFText()->getIndex())
    {
      dof = s->getDOFText();
      s->setVariable("");
      s->setVariableName("");
      s->setNote("");
    }

    if (dof != NULL)
    {
      dof->setAttribute(NULL);
    }

    displDOFInfo(s);

    dof = NULL;
  }

  s = NULL;
}


void DiagramEditor::clearLinkAttrDOF(const size_t& attrIdx)
{
  Shape* s        = NULL;
  DOF* dof        = NULL;
  Attribute* attr = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    s = diagram->getShape(i);

    dof  = s->getDOFXCtr();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFYCtr();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFWth();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFHgt();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFAgl();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFCol();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFOpa();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }

    dof  = s->getDOFText();
    attr = dof->getAttribute();
    if (attr != NULL && attr->getIndex() == attrIdx)
    {
      dof->setAttribute(NULL);
    }
    s->setVariable("");
  }

  s    = NULL;
  dof  = NULL;
  attr = NULL;
}


// -- get functions ---------------------------------------------


Diagram* DiagramEditor::getDiagram()
{
  return diagram;
}


int DiagramEditor::getEditMode()
{
  return editMode;
}


// -- visualization functions  ----------------------------------


void DiagramEditor::visualize(const bool& inSelectMode)
{
  clear();

  if (inSelectMode == true)
  {
    if (editMode == EDIT_MODE_SELECT || editMode == EDIT_MODE_DOF || editMode == EDIT_MODE_NOTE)
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
      diagram->visualize(inSelectMode, pixelSize());
      glPopName();

      // finish up picking
      finishSelectMode(
        hits,
        selectBuf);
    }
  }
  else
  {
    diagram->visualize(inSelectMode, pixelSize());

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

      if (diagram->getSnapGrid() == true)
      {
        double intv = diagram->getGridInterval(pixelSize());

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
      if (editMode == EDIT_MODE_SELECT)
      {
        selectedX1 = xC;
        selectedY1 = yC;
        selectedX2 = 0.5*dX;
        selectedY2 = -0.5*dY;
        if (!isAnyShapeSelected())
        {
          VisUtils::drawRect(x1, x2, y1, y2);
        }
      }
      else
      {
        selectedX1 = -1;
        selectedX2 = -1;
        selectedY1 = -1;
        selectedY2 = -1;
        if (editMode == EDIT_MODE_RECT)
        {
          VisUtils::drawRect(x1, x2, y1, y2);
        }
        else if (editMode == EDIT_MODE_ELLIPSE)
        {
          VisUtils::drawEllipse(xC, yC, 0.5*dX, 0.5*dY, Shape::segNumHnt);
        }
        else if (editMode == EDIT_MODE_LINE)
        {
          VisUtils::drawLine(x1, x2, y1, y2);
        }
        else if (editMode == EDIT_MODE_ARROW)
        {
          VisUtils::drawArrow(x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix);
        }
        else if (editMode == EDIT_MODE_DARROW)
        {
          VisUtils::drawDArrow(x1, x2, y1, y2, Shape::hdlSzeHnt*pix, 2.0*Shape::hdlSzeHnt*pix);
        }
      }
    }
  }
}

void DiagramEditor::reGenText()
{
  Shape* s        = NULL;

  size_t sizeShapes = diagram->getSizeShapes();
  for (size_t i = 0; i < sizeShapes; ++i)
  {
    s = diagram->getShape(i);
    s->setTextures(false);
  }
}


// -- event handlers --------------------------------------------


void DiagramEditor::handleMouseEvent(QMouseEvent* e)
{
  Visualizer::handleMouseEvent(e);

  if (e->type() != QEvent::MouseMove)
  {
    if (editMode == EDIT_MODE_SELECT ||
        (editMode == EDIT_MODE_DOF && e->button() == Qt::LeftButton &&
         (e->type() == QEvent::MouseButtonPress ||
          e->type() == QEvent::MouseButtonRelease)))
    {
      visualize(true);
    }
    if (e->button() == Qt::LeftButton && e->type() == QEvent::MouseButtonRelease)
    {
      handleIntersection();
    }
    if (editMode != EDIT_MODE_SELECT && editMode != EDIT_MODE_DOF)
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

      if (diagram->getSnapGrid() == true)
      {
        double intv = diagram->getGridInterval(pixelSize());

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

      double xLeft, xRight, yTop, yBottom;
      diagram->getGridCoordinates(xLeft, xRight, yTop, yBottom);
      if (!(xLeft <= (xC - xDFC) && xRight >= (xC + xDFC) && yBottom <= (yC - yDFC) && yTop >= (yC + yDFC)))
      {
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
      }

      Shape* s = new Shape(
        diagram,
        diagram->getSizeShapes(),
        xC,     yC,
        0.5*dX, -0.5*dY,
        0.0,    Shape::TYPE_RECT);

      if (editMode == EDIT_MODE_RECT)
      {
        s->setTypeRect();
      }
      else if (editMode == EDIT_MODE_ELLIPSE)
      {
        s->setTypeEllipse();
      }
      else if (editMode == EDIT_MODE_LINE)
      {
        s->setTypeLine();
      }
      else if (editMode == EDIT_MODE_ARROW)
      {
        s->setTypeArrow();
      }
      else if (editMode == EDIT_MODE_DARROW)
      {
        s->setTypeDArrow();
      }
      else if (editMode == EDIT_MODE_NOTE)
      {
        s->setTypeNote();
        mediator->handleNote(s->getIndex() , s->getNote());
      }

      diagram->addShape(s);
      s = NULL;

      // undo transl & scale here
    }
  }
  else
  {
    if (m_mouseDrag && (editMode  == EDIT_MODE_SELECT || editMode  == EDIT_MODE_DOF))
    {
      if (drgBegIdx1 == NON_EXISTING && drgBegIdx2 == NON_EXISTING)
      {
        selection = true;
        visualize(true);      // select mode
      }
      else
      {
        handleDrag();
      }
    }

    m_lastMousePos = e->pos();
  }
}

void DiagramEditor::handleKeyEvent(QKeyEvent* e)
{
  Visualizer::handleKeyEvent(e);
  if (editMode == EDIT_MODE_SELECT)
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
  else if (ids.size() == 2 && !selection)
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
  if (drgBegIdx1 == NON_EXISTING && drgBegIdx2 == NON_EXISTING)
  {
    deselectAll();
    mediator->handleDOFDeselect();
  }
  else
  {
    drgBegIdx1 = NON_EXISTING;
    drgBegIdx2 = NON_EXISTING;
  }

  if (m_lastMouseEvent.type() == QEvent::MouseButtonPress &&
      m_lastMouseEvent.button() == Qt::RightButton)
  {
    QPointF pos = worldCoordinate(m_lastMouseEvent.posF());
    xPaste = pos.x();
    yPaste = pos.y();

    //focus change prohibits mouseup event, simulate it:
    QMouseEvent event(QEvent::MouseButtonRelease, m_lastMouseEvent.pos(), Qt::RightButton, Qt::NoButton, m_lastMouseEvent.modifiers());
    handleMouseEvent(&event);

    bool pasteFlag = false;
    int checkedItem = -1;
    if (clipBoardShape != NULL || clipBoardList.size() > 0)
    {
      pasteFlag = true;
    }

    Shape* selectedShape = NULL;
    //find the selected shape
    for (size_t i = 0; i < diagram->getSizeShapes() && selectedShape == NULL; ++i)
    {
      if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
      {
        selectedShape = diagram->getShape(i);
        checkedItem = selectedShape->getCheckedId();
      }
    }

    mediator->handleEditShape(
      false,     // cut
      false,     // copy
      pasteFlag, // paste
      false,     // delete
      false,     // bring to front
      false,     // send to back
      false,     // bring forward
      false,     // send backward
      false,     // edit DOF
      checkedItem);  // id of the variable displayed on the shape
  }
}

void DiagramEditor::handleHitShape(const size_t& shapeIdx)
{
  size_t sizeShapes = 0;
  if (diagram != NULL)
  {
    sizeShapes = diagram->getSizeShapes();
  }

  if (shapeIdx != NON_EXISTING && shapeIdx < sizeShapes)
  {
    Shape* s = diagram->getShape(shapeIdx);

    if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
    {
      if (m_lastMouseEvent.button() == Qt::LeftButton)
      {
        if (editMode == EDIT_MODE_SELECT)   // mode
        {
          if (s->getMode() == Shape::MODE_EDIT)
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
              diagram->getShape(i)->setModeNormal();
            }
        }
        else if (editMode == EDIT_MODE_DOF)
        {
          if (s->getMode() != Shape::MODE_EDT_DOF_XCTR &&
              s->getMode() != Shape::MODE_EDT_DOF_YCTR &&
              s->getMode() != Shape::MODE_EDT_DOF_WTH  &&
              s->getMode() != Shape::MODE_EDT_DOF_HGT  &&
              s->getMode() != Shape::MODE_EDT_DOF_AGL)
          {
            s->setMode(Shape::MODE_EDT_DOF_XCTR);
            drgBegIdx1 = -1;
            drgBegIdx2 = -1;
          }
          displDOFInfo(s);


          for (size_t i = 0; i < sizeShapes; ++i)
            if (i != shapeIdx)
            {
              diagram->getShape(i)->setModeNormal();
            }
        }
        else if (editMode == EDIT_MODE_NOTE)
        {
          //mediator->handleNote( shapeIdx, s->getNote() );
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
          if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
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

        displShapeEdtOptions(s);
      } // side
    } // click

    update();
    s = NULL;
  }
}


void DiagramEditor::handleHitShapeHandle(
  const size_t& shapeIdx,
  const size_t& handleId)
{
  size_t sizeShapes = 0;
  if (diagram != NULL)
  {
    sizeShapes = diagram->getSizeShapes();
  }

  if (shapeIdx != NON_EXISTING && shapeIdx < sizeShapes)
  {
    Shape* s = diagram->getShape(shapeIdx);


    if (m_lastMouseEvent.type() == QEvent::MouseButtonPress)
    {
      if (m_lastMouseEvent.button() == Qt::LeftButton)
      {
        if (editMode != EDIT_MODE_NOTE)
        {
          if (m_mouseDrag)
          {
            drgBegIdx1 = shapeIdx;
            drgBegIdx2 = handleId;
            xDrgDist   = 0;
            yDrgDist   = 0;
          }
          else
          {
            drgBegIdx1 = -1;
            drgBegIdx2 = -1;
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

        displShapeEdtOptions(s);
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
      if (s->getMode() != Shape::MODE_EDT_DOF_XCTR &&
          s->getMode() != Shape::MODE_EDT_DOF_YCTR &&
          s->getMode() != Shape::MODE_EDT_DOF_WTH  &&
          s->getMode() != Shape::MODE_EDT_DOF_HGT  &&
          s->getMode() != Shape::MODE_EDT_DOF_AGL)
      {
        s->setMode(Shape::MODE_EDT_DOF_XCTR);
        drgBegIdx1 = -1;
        drgBegIdx2 = -1;
      }
      if (s->getType() != Shape::TYPE_NOTE)
      {
        displDOFInfo(s);
        editMode = EDIT_MODE_DOF;
        mediator->handleEditModeDOF(this);
      }
      for (size_t i = 0; i < sizeShapes; ++i)
        if (i != shapeIdx)
        {
          diagram->getShape(i)->setModeNormal();
        }
    }
    s = NULL;
  }
}


void DiagramEditor::handleDrag()
{
  size_t sizeShapes = 0;
  if (diagram != NULL)
  {
    sizeShapes = diagram->getSizeShapes();
  }

  if (drgBegIdx1 != NON_EXISTING && drgBegIdx1 < sizeShapes)
  {
    // do transl & scale here
    Shape* s = diagram->getShape(drgBegIdx1);

    if (s->getMode() == Shape::MODE_EDIT)
    {
      if (drgBegIdx2 == Shape::ID_HDL_CTR)
      {
        double xDrag, yDrag;
        handleDragCtr(s, xDrag, yDrag);
        size_t i;
        for (i = 0; i < sizeShapes; i++)
        {
          Shape* otherSelectedShape = diagram->getShape(i);
          if (drgBegIdx1 != i && otherSelectedShape->getMode() == Shape::MODE_EDIT)
          {
            double xCtr, yCtr;
            otherSelectedShape->getCenter(xCtr, yCtr);
            xCtr += xDrag;
            yCtr += yDrag;
            otherSelectedShape ->setCenter(xCtr, yCtr);
          }
          otherSelectedShape = NULL;
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
    else if (s->getMode() == Shape::MODE_EDT_DOF_XCTR)
    {
      /*
      if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
        handleDragDOFXCtrBeg( s );
      else
      */
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFXCtrEnd(s);
      }
    }
    else if (s->getMode() == Shape::MODE_EDT_DOF_YCTR)
    {
      /*
      if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
        handleDragDOFYCtrBeg( s );
      else
      */
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFYCtrEnd(s);
      }
    }
    else if (s->getMode() == Shape::MODE_EDT_DOF_WTH)
    {
      /*
      if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
        handleDragDOFWthBeg( s );
      else
      */
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFWthEnd(s);
      }
    }
    else if (s->getMode() == Shape::MODE_EDT_DOF_HGT)
    {
      /*
      if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
        handleDragDOFHgtBeg( s );
      else
      */
      if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFHgtEnd(s);
      }
    }
    else if (s->getMode() == Shape::MODE_EDT_DOF_AGL)
    {
      if (drgBegIdx2 == Shape::ID_HDL_HGE)
      {
        handleDragHge(s);
      }
      /*
      else if ( drgBegIdx2 == Shape::ID_HDL_DOF_BEG )
        handleDragDOFAglBeg( s );
      */
      else if (drgBegIdx2 == Shape::ID_HDL_DOF_END)
      {
        handleDragDOFAglEnd(s);
      }
    }
    // undo transl & scale here

    update();
    s = NULL;
  }
}


void DiagramEditor::handleShowVariable(const string& variable, const int& variableId)
{
  Shape* selectedShape = NULL;
  if (lastSelectedShapeId != NON_EXISTING)
  {
    selectedShape = diagram->getShape(lastSelectedShapeId);
    selectedShape->setCheckedId(variableId);
    string variableName = variable;
    size_t i = variable.find(":",0); // for getting variable's name
    variableName = variable.substr(0, i);
    i++;
    string variableValue = variable;
    variableValue.erase(0,i);
    selectedShape->setNote(variableName);
    selectedShape->setVariableName(variableName);
    if (variable.length() > variableValue.length())  // If their length is same, it means variable doesn't have a value
    {
      selectedShape->setVariable(variableValue);
    }
    else
    {
      selectedShape->setVariable("");
    }
  }
}


void DiagramEditor::handleShowNote(const string& variable, const size_t& shapeId)
{
  Shape* selectedShape = NULL;
  selectedShape = diagram->getShape(shapeId);
  selectedShape->setNote(variable);
  if (selectedShape->getType() == Shape::TYPE_NOTE)
  {
    double letterCount = selectedShape->getNote().length();
    double x = letterCount * 0.009;
    double y = 0.02;
    selectedShape->setDFC(x, y);
  }
}


void DiagramEditor::handleAddText(string& variable, size_t& shapeId)
{
  Shape* selectedShape = NULL;
  shapeId = lastSelectedShapeId;
  selectedShape = diagram->getShape(shapeId);
  variable = selectedShape->getNote();
  selectedShape = NULL;
}


void DiagramEditor::handleTextSize(size_t& textSize, size_t& shapeId)
{
  Shape* selectedShape = NULL;
  shapeId = lastSelectedShapeId;
  selectedShape = diagram->getShape(shapeId);
  if (selectedShape != NULL)
  {
    textSize = selectedShape->getTextSize();
    selectedShape = NULL;
  }
}


void DiagramEditor::handleSetTextSize(size_t& textSize, size_t& shapeId)
{

  Shape* selectedShape = NULL;
  selectedShape = diagram->getShape(shapeId);
  selectedShape->setTextSize(textSize);
}


void DiagramEditor::handleCut()
{
  Shape* origShape = NULL;
  Shape* copyShape = NULL;
  bool shapeSelected = false;

  // find & copy selected shape
  for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
  {
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      if (!shapeSelected)
      {
        shapeSelected = true;
        clearClipBoard();
      }
      origShape = diagram->getShape(i);

      // invoke copy constructor
      copyShape = new Shape(*origShape);
      // delete original shape
      diagram->deleteShape(origShape->getIndex());
      i--; // decrement index; because deletion of the shape decrements the size of the diagram

      clipBoardList.push_back(copyShape);
      origShape = NULL;
      copyShape = NULL;
    }
  }
}


void DiagramEditor::handleCopy()
{
  Shape* origShape = NULL;
  Shape* copyShape = NULL;
  bool shapeSelected = false;

  // find & copy selected shape
  for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
  {
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      if (!shapeSelected)
      {
        shapeSelected = true;
        clearClipBoard();
      }
      origShape = diagram->getShape(i);

      // invoke copy constructor
      copyShape = new Shape(*origShape);

      clipBoardList.push_back(copyShape);
      origShape = NULL;
      copyShape = NULL;
    }
  }
}


void DiagramEditor::clearClipBoard()
{
  size_t size = clipBoardList.size();
  for (size_t i = 0; i < size; i++)
  {
    clipBoardList[i] = NULL;
  }
  clipBoardList.clear();
}


void DiagramEditor::handlePaste()
{
  if (clipBoardList.size() > 0)
  {
    // deselect all other shapes
    for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
      if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
      {
        diagram->getShape(i)->setModeNormal();
      }

    size_t size = clipBoardList.size();
    double xC, yC, xCFirst, yCFirst;
    clipBoardList[0]->getCenter(xCFirst, yCFirst);
    for (size_t i = 0; i < size; i++)
    {
      // update index of clipboard shape
      clipBoardList[i]->setIndex(diagram->getSizeShapes());

      // update clipboard shape
      if (i == 0) // Paste the first selected shape to the clicked position
      {
        clipBoardList[i]->setCenter(xPaste, yPaste);
      }
      else // Paste other shapes relative to their position with respect to first shape
      {
        double distanceX, distanceY, x1, x2, y1, y2;
        clipBoardList[0]->getCenter(x1, y1);
        clipBoardList[i]->getCenter(x2, y2);

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
      diagram->addShape(clipBoardList[i]);
      // make another copy of clipboard shape
      clipBoardList[i] = new Shape(*clipBoardList[i]);
    }
  }
}


void DiagramEditor::handleDelete()
{
  vector< size_t > toDelete;
  // get indices to delete
  {
    for (size_t i = 0; i < diagram->getSizeShapes() ; ++i)
    {
      /*
      if ( diagram->getShape(i)->getMode() == Shape::MODE_EDT_CTR_DFC ||
      diagram->getShape(i)->getMode() == Shape::MODE_EDT_HGE_AGL )
      */
      if (diagram->getShape(i)->getMode() == Shape::MODE_EDIT)
      {
        toDelete.push_back(i);
      }
    }
  }

  // delete shapes
  {
    for (size_t i = 0; i < toDelete.size(); ++i)
    {
      diagram->deleteShape(toDelete[i]-i);
    }
  }
}


void DiagramEditor::handleSelectAll()
{
  for (size_t i = 0; i < diagram->getSizeShapes() ; ++i)
  {
    diagram->getShape(i)->setMode(Shape::MODE_EDIT);
  }
}


void DiagramEditor::handleBringToFront()
{
  Shape* s = NULL;
  for (size_t i = 0; i < diagram->getSizeShapes() && s == NULL; ++i)
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      s = diagram->getShape(i);
    }

  if (s != NULL)
  {
    diagram->moveShapeToBack(s->getIndex());
  }

  s = NULL;
}


void DiagramEditor::handleSendToBack()
{
  Shape* s = NULL;
  for (size_t i = 0; i < diagram->getSizeShapes() && s == NULL; ++i)
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      s = diagram->getShape(i);
    }

  if (s != NULL)
  {
    diagram->moveShapeToFront(s->getIndex());
  }

  s = NULL;
}


void DiagramEditor::handleBringForward()
{
  Shape* s = NULL;
  for (size_t i = 0; i < diagram->getSizeShapes() && s == NULL; ++i)
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      s = diagram->getShape(i);
    }

  if (s != NULL)
  {
    diagram->moveShapeBackward(s->getIndex());
  }

  s = NULL;
}


void DiagramEditor::handleSendBackward()
{
  Shape* s = NULL;
  for (size_t i = 0; i < diagram->getSizeShapes() && s == NULL; ++i)
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      s = diagram->getShape(i);
    }

  if (s != NULL)
  {
    diagram->moveShapeForward(s->getIndex());
  }

  s = NULL;
}


void DiagramEditor::handleEditDOF()
{
  Shape* s = NULL;
  if (lastSelectedShapeId != NON_EXISTING)
  {
    s = diagram->getShape(lastSelectedShapeId);
  }
  if (s != NULL)
  {
    s->setModeEdtDOFXCtr();
    displDOFInfo(s);
    editMode = EDIT_MODE_DOF;
    mediator->
    handleEditModeDOF(this);
    for (size_t i = 0; i < diagram->getSizeShapes(); ++i)
      if (i != s->getIndex())
      {
        diagram->getShape(i)->setModeNormal();
      }
    s = NULL;
  }
}


void DiagramEditor::handleSetDOF(const size_t& attrIdx)   // Link Attribute to the Text DOF of the selected Shape
{
  Shape* s = NULL;

  if (lastSelectedShapeId != NON_EXISTING)
  {
    s = diagram->getShape(lastSelectedShapeId);
  }
  if (s != NULL)
  {
    DOF* dof = NULL;
    if (attrIdx == NON_EXISTING) // NON_EXISTING Indicates, removing attribute from the Text DOF of the selected shape
    {
      dof = s->getDOFText();
      s->setVariable("");
      dof->setAttribute(NULL);
    }
    else
    {
      Attribute* a = graph->getAttribute(attrIdx);
      dof = s->getDOFText();
      dof->setAttribute(a);
      a   = NULL;
    }
    dof = NULL;
  }
  s = NULL;
}


void DiagramEditor::handleCheckedVariable(const size_t& idDOF, const int& variableId)
{
  Shape* selectedShape = NULL;

  //find the selected shape
  for (size_t i = 0; i < diagram->getSizeShapes() && selectedShape == NULL; ++i)
  {
    if (diagram->getShape(i)->getMode() != Shape::MODE_NORMAL)
    {
      selectedShape = diagram->getShape(i);
      if (idDOF == selectedShape->getDOFText()->getIndex())  // Check the Variable if TextDOF is modified
      {
        selectedShape->setCheckedId(variableId);
      }
    }
  }
}


// -- public utility functions --------------------------------------


void DiagramEditor::deselectAll()
{
  size_t sizeShapes = 0;
  if (diagram != NULL)
  {
    sizeShapes = diagram->getSizeShapes();
  }

  for (size_t i = 0; i < sizeShapes; ++i)
  {
    diagram->getShape(i)->setModeNormal();
  }

  drgBegIdx1 = -1;
  drgBegIdx2 = -1;
}


// -- private utility functions -------------------------------------


void DiagramEditor::displShapeEdtOptions(Shape* s)
{
  if (s != NULL)
  {
    bool editDOF = true;
    bool pasteFlag = false;
    int checkedId = s->getCheckedId();
    if (clipBoardShape != NULL || clipBoardList.size() > 0)
    {
      pasteFlag = true;
    }

    if (s->getType() == Shape::TYPE_NOTE) // If the shape is a note, don't display the text options
    {
      editDOF = false;
    }

    mediator->handleEditShape(
      true,      // cut
      true,      // copy
      pasteFlag, // paste
      true,      // delete
      true,      // bring to front
      true,      // send to back
      true,      // bring forward
      true,      // send backward
      editDOF,     // edit DOF
      checkedId);  // id of the variable displayed on the shape
  }
}


void DiagramEditor::displDOFInfo(Shape* s)
{
  if (s != NULL)
  {
    vector< size_t > indcs;
    vector< string > dofs;
    vector< size_t > attrIdcs;
    size_t selIdx = NON_EXISTING;

    indcs.push_back(s->getDOFXCtr()->getIndex());
    dofs.push_back(s->getDOFXCtr()->getLabel());
    if (s->getDOFXCtr()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFXCtr()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_XCTR)
    {
      selIdx = s->getDOFXCtr()->getIndex();
    }

    indcs.push_back(s->getDOFYCtr()->getIndex());
    dofs.push_back(s->getDOFYCtr()->getLabel());
    if (s->getDOFYCtr()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFYCtr()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_YCTR)
    {
      selIdx = s->getDOFYCtr()->getIndex();
    }

    indcs.push_back(s->getDOFWth()->getIndex());
    dofs.push_back(s->getDOFWth()->getLabel());
    if (s->getDOFWth()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFWth()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_WTH)
    {
      selIdx = s->getDOFWth()->getIndex();
    }

    indcs.push_back(s->getDOFHgt()->getIndex());
    dofs.push_back(s->getDOFHgt()->getLabel());
    if (s->getDOFHgt()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFHgt()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_HGT)
    {
      selIdx = s->getDOFHgt()->getIndex();
    }

    indcs.push_back(s->getDOFAgl()->getIndex());
    dofs.push_back(s->getDOFAgl()->getLabel());
    if (s->getDOFAgl()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFAgl()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_AGL)
    {
      selIdx = s->getDOFAgl()->getIndex();
    }

    indcs.push_back(s->getDOFCol()->getIndex());
    dofs.push_back(s->getDOFCol()->getLabel());
    if (s->getDOFCol()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFCol()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_COL)
    {
      selIdx = s->getDOFCol()->getIndex();
    }

    indcs.push_back(s->getDOFOpa()->getIndex());
    dofs.push_back(s->getDOFOpa()->getLabel());
    if (s->getDOFOpa()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFOpa()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_OPA)
    {
      selIdx = s->getDOFOpa()->getIndex();
    }

    indcs.push_back(s->getDOFText()->getIndex());
    dofs.push_back(s->getDOFText()->getLabel());
    if (s->getDOFText()->getAttribute() == NULL)
    {
      attrIdcs.push_back(NON_EXISTING);
    }
    else
    {
      attrIdcs.push_back(s->getDOFText()->getAttribute()->getIndex());
    }
    if (s->getMode() == Shape::MODE_EDT_DOF_TEXT)
    {
      selIdx = s->getDOFText()->getIndex();
    }

    mediator->handleEditDOF(
      indcs,
      dofs,
      attrIdcs,
      selIdx);

    vector< double > vals;
    vector< double > yVals;

    s->getDOFCol()->getValues(vals);
    s->getDOFColYValues(yVals);
    mediator->handleDOFColSetValuesEdt(vals, yVals);

    vals.clear();
    s->getDOFOpa()->getValues(vals);
    s->getDOFOpaYValues(yVals);
    mediator->handleDOFOpaSetValuesEdt(vals, yVals);
  }
}


void DiagramEditor::handleDragCtr(Shape* s, double& xDrag, double& yDrag)
{
  double xCtr, yCtr;
  double xDFC, yDFC;
  double x,    y;

  QPointF pos = worldCoordinate(m_lastMousePos);
  QPointF eventPos = worldCoordinate(m_lastMouseEvent.posF());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  x = xCtr;
  y = yCtr;

  xDrgDist += eventPos.x()-pos.x();
  yDrgDist += eventPos.y()-pos.y();

  if (diagram->getSnapGrid() == true)
  {
    x = Utils::rndToNearestMult(x+xDrgDist, diagram->getGridInterval(pixelSize()));
    y = Utils::rndToNearestMult(y+yDrgDist, diagram->getGridInterval(pixelSize()));
    double x1 = Utils::rndToNearestMult(x - xDFC, diagram->getGridInterval(pixelSize()));
    double y1 = Utils::rndToNearestMult(y - yDFC, diagram->getGridInterval(pixelSize()));
    x = x1 + xDFC;
    y = y1 + yDFC;

    if (x != xCtr)
    {
      xDrgDist = eventPos.x()-x;
    }
    if (y != yCtr)
    {
      yDrgDist = eventPos.y()-y;
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getGridInterval(pixelSize());
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

  s->setDFC(xDFC+hypX,      yDFC+hypY);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = 0.5*(-xDFC-xS);
  adj = hyp*cos(angl);   // x (-)
  opp = hyp*sin(angl);   // y (-)

  s->setDFC(xDFC+hyp, yDFC);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getGridInterval(pixelSize());
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

  s->setDFC(xDFC+hypX,      yDFC+hypY);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = 0.5*(-yDFC-yS);
  adj = hyp*cos(angl);   // y (-)
  opp = hyp*sin(angl);   // x (+)

  s->setDFC(xDFC,     yDFC+hyp);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getGridInterval(pixelSize());
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

  s->setDFC(xDFC+hypX,      yDFC+hypY);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = 0.5*(xS-xDFC);
  adj = hyp*cos(angl);   // x (+)
  opp = hyp*sin(angl);   // y (+)

  s->setDFC(xDFC+hyp, yDFC);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getGridInterval(pixelSize());
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

  s->setDFC(xDFC+hypX,      yDFC+hypY);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = 0.5*(yS-yDFC);
  adj = hyp*cos(angl);   // y (+)
  opp = hyp*sin(angl);   // x (-)

  s->setDFC(xDFC,     yDFC+hyp);
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
  aglDg = s->getAngleCtr();

  aglRd = Utils::degrToRad(aglDg);
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

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

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getAngleInterval();
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
  s->setAngleCtr(aglDg);
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
  aglDg = s->getAngleCtr();
  aglRd = Utils::degrToRad(aglDg);
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

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

  if (diagram->getSnapGrid() == true)
  {
    double itv = diagram->getAngleInterval();
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
  s->setAngleCtr(aglDg);
}


void DiagramEditor::handleDragDOFXCtrBeg(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double x0;            // x position after translating to shape's center

  // get shape's geometry
  s->getCenter(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    x0 = Utils::rndToNearestMult(xCtr+x0, itv)-xCtr;
  }

  s->getDOFXCtr()->setMin(x0);
}


void DiagramEditor::handleDragDOFXCtrEnd(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double x0;            // x position after translating to shape's center

  // get shape's geometry
  s->getCenter(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    x0 = Utils::rndToNearestMult(xCtr+x0, itv)-xCtr;
  }

  s->getDOFXCtr()->setMax(x0);
}


void DiagramEditor::handleDragDOFYCtrBeg(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double y0;            // y position after translating to shape's center

  // get shape's geometry
  s->getCenter(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  y0 = yCur-yCtr;

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    y0 = Utils::rndToNearestMult(yCtr+y0, itv)-yCtr;
  }

  s->getDOFYCtr()->setMin(y0);
}


void DiagramEditor::handleDragDOFYCtrEnd(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double y0;            // y position after translating to shape's center

  // get shape's geometry
  s->getCenter(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  y0 = yCur-yCtr;

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    y0 = Utils::rndToNearestMult(yCtr+y0, itv)-yCtr;
  }

  s->getDOFYCtr()->setMax(y0);
}


void DiagramEditor::handleDragDOFWthBeg(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double xS;      // position after rotating to shape's angle
  double hyp;           // hypotenuse, for movement parallel to shape's x-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = xS-xDFC;
  s->getDOFWth()->setMin(hyp);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;

  // rotate to 'normal' orientation, find x & y
  xS = x0*cos(-angl) - y0*sin(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(xCtr+xS*cos(angl), itv)-xCtr;
    xS = a/cos(angl);
  }

  hyp = xS-xDFC;
  s->getDOFWth()->setMax(hyp);
}


void DiagramEditor::handleDragDOFHgtBeg(Shape* s)
{
  double angl;          // shape's rotation angle in radians
  double xCtr, yCtr;    // center of shape
  double xDFC, yDFC;    // deltas of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // position after translating to shape's center
  double yS;      // position after rotating to shape's angle
  double hyp;           // hypotenuse, for movement parallel to shape's y-axis

  // get shape's geometry
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = yS-yDFC;
  s->getDOFHgt()->setMin(hyp);
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
  angl = Utils::degrToRad(s->getAngleCtr());
  s->getCenter(xCtr, yCtr);
  s->getDFC(xDFC, yDFC);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to xCtr, the 'origin'
  x0 = xCur-xCtr;
  y0 = yCur-yCtr;
  // rotate to 'normal' orientation, find x & y
  yS = x0*sin(-angl) + y0*cos(-angl);

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    double a = Utils::rndToNearestMult(yCtr+yS*cos(angl), itv)-yCtr;
    yS = a/cos(angl);
  }

  hyp = yS-yDFC;
  s->getDOFHgt()->setMax(hyp);
}


void DiagramEditor::handleDragHge(Shape* s)
{
  double xCtr, yCtr;    // center of shape
  double xCur, yCur;    // current mouse position
  double x0,   y0;      // y position after translating to shape's center

  // get shape's geometry
  s->getCenter(xCtr, yCtr);

  // get mouse info
  xCur = worldCoordinate(m_lastMouseEvent.posF()).x();
  yCur = worldCoordinate(m_lastMouseEvent.posF()).y();

  // translate to center, the 'origin'
  y0 = yCur-yCtr;
  x0 = xCur-xCtr;

  if (diagram->getSnapGrid() == true)
  {
    double itv  = diagram->getGridInterval(pixelSize());
    x0 = Utils::rndToNearestMult(xCtr+x0, itv)-xCtr;
    y0 = Utils::rndToNearestMult(yCtr+y0, itv)-yCtr;
  }

  s->setHinge(x0, y0);
}


void DiagramEditor::handleDragDOFAglBeg(Shape* s)
{
  double xHge, yHge, xCtr, yCtr;
  double dstHgeCtr;
  double aglRef, aglTot;
  double xCur, yCur, xRelHge, yRelHge;

  // distance from hinge to center
  s->getHinge(xHge, yHge);
  s->getCenter(xCtr, yCtr);
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

  aglTot = Utils::calcAngleDg(xRelHge, yRelHge);

  if (diagram->getSnapGrid() == true)
  {
    double itvAgl  = diagram->getAngleInterval();
    aglTot = Utils::rndToNearestMult(aglTot, itvAgl);
  }

  s->getDOFAgl()->setMin(aglTot-aglRef);
}


void DiagramEditor::handleDragDOFAglEnd(Shape* s)
{
  double xHge, yHge, xCtr, yCtr;
  double dstHgeCtr;
  double aglRef, aglTot;
  double xCur, yCur, xRelHge, yRelHge;

  // distance from hinge to center
  s->getHinge(xHge, yHge);
  s->getCenter(xCtr, yCtr);
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

  if (diagram->getSnapGrid() == true)
  {
    double itvAgl  = diagram->getAngleInterval();
    aglTot = Utils::rndToNearestMult(aglTot, itvAgl);
  }

  s->getDOFAgl()->setMax(aglTot-aglRef);
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

  ptr = NULL;
}


// -- end -----------------------------------------------------------
