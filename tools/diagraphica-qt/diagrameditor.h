// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagrameditor.h

#ifndef DIAGRAMEDITOR_H
#define DIAGRAMEDITOR_H

#include <QtCore>
#include <QtGui>
#include <QEvent>

#include <cstddef>
#include <cstdlib>
#include <cmath>
#include <string>
#include <vector>
#include "colleague.h"
#include "colorchooser.h"
#include "dof.h"
#include "diagram.h"
#include "graph.h"
#include "visualizer.h"

class DiagramEditor : public Visualizer, public Colleague
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    DiagramEditor(
      QWidget *parent,
      Mediator* m,
      Graph* g);

    virtual ~DiagramEditor();

    void handleIntersection();
    void translatePoints(double& x1, double& y1, double& x2, double& y2, double givenX1, double givenY1, double givenX2, double givenY2);
    bool isAnyShapeSelected();
    void handleDOFSel(const size_t& DOFIdx);
    void handleDOFSetTextStatus(
      const size_t& DOFIdx,
      const int& status);
    int handleDOFGetTextStatus(const size_t& DOFIdx);
    void handleDOFColAdd(
      const double& hue,
      const double& y);
    void handleDOFColUpdate(
      const size_t& idx,
      const double& hue,
      const double& y);
    void handleDOFColClear(
      const size_t& idx);
    void handleDOFOpaAdd(
      const double& opa,
      const double& y);
    void handleDOFOpaUpdate(
      const size_t& idx,
      const double& opa,
      const double& y);
    void handleDOFOpaClear(
      const size_t& idx);
    void setLinkDOFAttr(
      const size_t& DOFIdx,
      const size_t& attrIdx);
    void clearLinkDOFAttr(const size_t& DOFIdx);
    void clearLinkAttrDOF(const size_t& attrIdx);

    // -- get functions ---------------------------------------------
    Diagram* getDiagram();
    int getEditMode();

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);
    void reGenText();

    // -- event handlers --------------------------------------------
    void handleMouseEvent(QMouseEvent* e);
    void handleKeyEvent(QKeyEvent* e);

    void handleHits(const std::vector< int > &ids);
    void handleHitDiagramOnly();
    void handleHitShape(const size_t& shapeIdx);
    void handleHitShapeHandle(
      const size_t& shapeIdx,
      const size_t& handleId);

    void handleDrag();
    /*
    void handleDrop(
        const int &x,
        const int &y,
        const vector< int > &data );
    */
    void handleShowVariable(const std::string& variable, const int& variableId);
    void handleShowNote(const std::string& variable, const size_t& shapeId);
    void handleAddText(std::string& variable, size_t& shapeId);
    void handleTextSize(size_t& textSize, size_t& shapeId);
    void handleSetTextSize(size_t& textSize, size_t& shapeId);
    void handleCut();
    void handleCopy();
    void clearClipBoard();
    void handlePaste();
    void handleDelete();
    void handleSelectAll();
    void handleBringToFront();
    void handleSendToBack();
    void handleBringForward();
    void handleSendBackward();
    void handleEditDOF();
    void handleSetDOF(const size_t& attrIdx);
    void handleCheckedVariable(const size_t& idDOF, const int& variableId);

    // -- public utility functions ----------------------------------
    void deselectAll();

    // -- public constants ------------------------------------------
    enum
    {
      EDIT_MODE_SELECT,
      EDIT_MODE_NOTE,
      EDIT_MODE_DOF,
      EDIT_MODE_RECT,
      EDIT_MODE_ELLIPSE,
      EDIT_MODE_LINE,
      EDIT_MODE_ARROW,
      EDIT_MODE_DARROW
    };

  public slots:
    // -- set functions ---------------------------------------------
    void setDiagram(Diagram* dgrm);

    void setEditMode(int mode);

    void setShowGrid(bool flag);
    void setSnapGrid(bool flag);

    void setFillColor();
    void setLineColor();

  protected:
    // -- private utility functions ---------------------------------
    void displShapeEdtOptions(Shape* s);
    void displDOFInfo(Shape* s);

    void handleDragCtr(Shape* s, double& xDrag, double& yDrag);
    void handleDragTopLft(Shape* s);
    void handleDragLft(Shape* s);
    void handleDragBotLft(Shape* s);
    void handleDragBot(Shape* s);
    void handleDragBotRgt(Shape* s);
    void handleDragRgt(Shape* s);
    void handleDragTopRgt(Shape* s);
    void handleDragTop(Shape* s);
    void handleDragRotRgt(Shape* s);
    void handleDragRotTop(Shape* s);

    void handleDragDOFXCtrBeg(Shape* s);
    void handleDragDOFXCtrEnd(Shape* s);
    void handleDragDOFYCtrBeg(Shape* s);
    void handleDragDOFYCtrEnd(Shape* s);
    void handleDragDOFWthBeg(Shape* s);
    void handleDragDOFWthEnd(Shape* s);
    void handleDragDOFHgtBeg(Shape* s);
    void handleDragDOFHgtEnd(Shape* s);
    void handleDragHge(Shape* s);
    void handleDragDOFAglBeg(Shape* s);
    void handleDragDOFAglEnd(Shape* s);

    // -- hit detection ---------------------------------------------
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- data members ----------------------------------------------

    QPoint m_lastMousePos;

    Diagram* m_diagram; // composition
    int m_editMode;
    size_t drgBegIdx1;
    size_t drgBegIdx2;
    size_t lastSelectedShapeId;
    bool selection;

    double xDrgDist;
    double yDrgDist;
    double selectedX1, selectedX2, selectedY1, selectedY2;

    Shape* clipBoardShape; // composition
    std::vector < Shape* > clipBoardList;
    double xPaste, yPaste;

    // -- static variables ------------------------------------------
    static int szeTxt;
};

#endif

// -- end -----------------------------------------------------------
