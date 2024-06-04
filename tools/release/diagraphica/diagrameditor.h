// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./diagrameditor.h

#ifndef DIAGRAMEDITOR_H
#define DIAGRAMEDITOR_H

#include <QtWidgets>
#include <QEvent>

#include "dofdialog.h"
#include "diagram.h"

class DiagramEditor : public Visualizer
{
  Q_OBJECT

  public:
    // -- constructors and destructor -------------------------------
    DiagramEditor(
      QWidget *parent,
      Graph* g);

    // -- get functions ---------------------------------------------
    Diagram* diagram()        { return m_diagram; }
    int editMode()            { return m_editMode; }
    Shape* selectedShape();
    QList<Shape*> selectedShapes();

    // -- public utility functions ----------------------------------
    void deselectAll();

    // -- visualization functions  ----------------------------------
    void visualize() override;
    void mark() override;
    void generateTextures();

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

    // -- context menu functions ------------------------------------

    void editDof(Shape* shape = nullptr);
    bool editNote(Shape* shape = nullptr);
    void editTextSize();

    void cutShapes();
    void copyShapes();
    void pasteShapes();
    void selectAllShapes();

    void deleteShapes();

    void bringToFront();
    void sendToBack();
    void bringForward();
    void sendBackward();

  protected:
    // -- private utility functions ---------------------------------
    virtual void clear() override;
    double snapIfNeeded(double input);
    QPointF snapIfNeeded(QPointF input);
    double snapAngleIfNeeded(double input);
    QRectF worldRectangle(QPointF start, QPointF stop);
    void translatePoints(double& x1, double& y1, double& x2, double& y2, double givenX1, double givenY1, double givenX2, double givenY2);
    void createShape();

    void initContextMenu();
    void showContextMenu();

    // -- event handlers --------------------------------------------
    void handleMouseEvent(QMouseEvent* e) override;
    void handleKeyEvent(QKeyEvent* e) override;

    void handleHits(const std::vector< int > &ids);

    void handleDragLft(Shape* s);
    void handleDragBot(Shape* s);
    void handleDragRgt(Shape* s);
    void handleDragTop(Shape* s);

    void handleDragRot(Shape* s, bool isTop);

    void handleDragDOFXCtrEnd(Shape* s);
    void handleDragDOFYCtrEnd(Shape* s);
    void handleDragDOFWthEnd(Shape* s);
    void handleDragDOFHgtEnd(Shape* s);
    void handleDragDOFHge(Shape* s);
    void handleDragDOFAglEnd(Shape* s);

    QSize sizeHint() const override { return QSize(600,600); }

    // -- hit detection ---------------------------------------------
    virtual void handleSelection(const Selection&) override;

    // -- data members ----------------------------------------------

    QMenu m_popup;          // Context menu

    Diagram* m_diagram;     // Composition
    int m_editMode;         // Current selected tool
    QRectF m_selection;     // Selection rectangle

    QPoint m_lastMousePos;  // For dragging
    QPointF m_dragDistance; // For dragging
    int m_currentSelectedShapeId;   // If a shape is dragged
    int m_currentSelectedHandleId;  // If a handle is dragged

    QList<Shape*> m_clipBoardList;  // If shapes are cut/copied

    // -- static variables ------------------------------------------
    static int szeTxt;
};

#endif

// -- end -----------------------------------------------------------
