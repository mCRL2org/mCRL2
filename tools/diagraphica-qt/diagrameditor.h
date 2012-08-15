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
#include "colorchooser.h"
#include "dof.h"
#include "dofdialog.h"
#include "diagram.h"
#include "graph.h"
#include "visualizer.h"

class DiagramEditor : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    DiagramEditor(
      QWidget *parent,
      Graph* g);

    void handleIntersection();

    // -- get functions ---------------------------------------------
    Diagram* diagram()        { return m_diagram; }
    int editMode()            { return m_editMode; }
    Shape* selectedShape();
    bool isAnyShapeSelected() { return (selectedShape() != 0); }

    // -- visualization functions  ----------------------------------
    void visualize(const bool& inSelectMode);
    void generateTextures();

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
    void handleCut();
    void handleCopy();
    void clearClipBoard();

    void handlePaste();
    void handleDelete();
    void handleSelectAll();
    void handleEditDOF();

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

    void setSelectMode() { setEditMode(EDIT_MODE_SELECT); }
    void setNoteMode() { setEditMode(EDIT_MODE_NOTE); }
    void setConfigureMode() { setEditMode(EDIT_MODE_DOF); }
    void setRectangleMode() { setEditMode(EDIT_MODE_RECT); }
    void setEllipseMode() { setEditMode(EDIT_MODE_ELLIPSE); }
    void setLineMode() { setEditMode(EDIT_MODE_LINE); }
    void setArrowMode() { setEditMode(EDIT_MODE_ARROW); }
    void setDoubleArrowMode() { setEditMode(EDIT_MODE_DARROW); }

    void setShowGrid(bool show);
    void setSnapGrid(bool show);

    void setFillColor();
    void setLineColor();

  protected:
    // -- private utility functions ---------------------------------
    void translatePoints(double& x1, double& y1, double& x2, double& y2, double givenX1, double givenY1, double givenX2, double givenY2);

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

    void handleDragDOFXCtrEnd(Shape* s);
    void handleDragDOFYCtrEnd(Shape* s);
    void handleDragDOFWthEnd(Shape* s);
    void handleDragDOFHgtEnd(Shape* s);
    void handleDragDOFHge(Shape* s);
    void handleDragDOFAglEnd(Shape* s);

    // -- hit detection ---------------------------------------------
    void processHits(
      GLint hits,
      GLuint buffer[]);

    // -- data members ----------------------------------------------

    DofDialog m_dofDialog;
    QPoint m_lastMousePos;
    QPointF m_dragDistance;

    Diagram* m_diagram; // composition
    int m_editMode;
    QRectF m_selection;

    size_t drgBegIdx1;
    size_t drgBegIdx2;
    size_t lastSelectedShapeId;
    bool m_selectionActive;

    std::vector < Shape* > clipBoardList;
    double xPaste, yPaste;

    // -- static variables ------------------------------------------
    static int szeTxt;
};

#endif

// -- end -----------------------------------------------------------
