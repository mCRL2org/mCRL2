// Author(s): A.J. (Hannes) pretorius
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./shape.h

#ifndef SHAPE_H
#define SHAPE_H

#include <QtCore>
#include <QtGui>

#include <cstddef>
#include <string>
#include <map>
#include <vector>
#include "dof.h"
#include "visutils.h"
#include "visualizer.h"

class Diagram;

class Shape
{
  public:
    // -- constructors and destructor -------------------------------
    Shape(
        Diagram *parentDiagram, size_t index,
        double xCenter,         double yCenter,
        double xDistance,       double yDistance,
        double angle,           int    shapeType,
        double xHinge,          double yHinge);
    Shape(const Shape& shape);
    virtual ~Shape();

    // -- set functions ---------------------------------------------
    void setIndex(size_t index) { m_index = index; }

    void setCenter(double xCenter, double yCenter)        { m_xCenter = xCenter; m_yCenter = yCenter; }
    void setDistance(double xDistance, double yDistance)  { m_xDistance = xDistance; m_yDistance = yDistance; }
    void setAngle(double angle)                           { m_angle = angle; }
    void setHinge(double xHinge, double yHinge)           { m_xHinge = xHinge, m_yHinge = yHinge; }

    void setShapeType(int type) { m_shapeType = type; }
    void setTypeNote()          { setShapeType(TYPE_NOTE); }
    void setTypeLine()          { setShapeType(TYPE_LINE); }
    void setTypeRect()          { setShapeType(TYPE_RECT); }
    void setTypeEllipse()       { setShapeType(TYPE_ELLIPSE); }
    void setTypeArrow()         { setShapeType(TYPE_ARROW); }
    void setTypeDArrow()        { setShapeType(TYPE_DARROW); }

    void setDrawMode(int mode)  { m_drawMode = mode; }
    void setModeNormal()        { setDrawMode(MODE_NORMAL); }
    void setModeEdit()          { setDrawMode(MODE_EDIT); }
    void setModeEditDOFXCtr()   { setDrawMode(MODE_EDIT_DOF_XCTR); }
    void setModeEditDOFYCtr()   { setDrawMode(MODE_EDIT_DOF_YCTR); }
    void setModeEditDOFHgt()    { setDrawMode(MODE_EDIT_DOF_HGT); }
    void setModeEditDOFWth()    { setDrawMode(MODE_EDIT_DOF_WTH); }
    void setModeEditDOFAgl()    { setDrawMode(MODE_EDIT_DOF_AGL); }
    void setModeEditDOFCol()    { setDrawMode(MODE_EDIT_DOF_COL); }
    void setModeEditDOFOpa()    { setDrawMode(MODE_EDIT_DOF_OPA); }
    void setModeEditDOFText()   { setDrawMode(MODE_EDIT_DOF_TEXT); }

    void setTextSize(size_t size) { m_textSize = size; m_texturesGenerated = false; }

    void setLineWidth(double width) { m_lineWidth = width; }
    void setLineColor(QColor color) { m_lineColor = color; }
    void setFillColor(QColor color) { m_fillColor = color; }

    void setVariableValue(QString value)  { m_variableValue = value; m_texturesGenerated = false; }
    void setNote(QString note)            { m_note = note; m_texturesGenerated = false; }

    void generateTextures()  { m_texturesGenerated = false; }

    void addDOFColYValue(double value)  { m_colorYValues.append(value); }
    void setDOFColYValue(int index, double value);
    void removeDOFColYValue(int index);
    void clearDOFColYValues()           { m_colorYValues.clear(); }

    void addDOFOpaYValue(double value)  { m_opacityYValues.append(value); }
    void setDOFOpaYValue(int index, double value);
    void removeDOFOpaYValue(int index);
    void clearDOFOpaYValues()           { m_opacityYValues.clear(); }


    // -- get functions ---------------------------------------------
    size_t index() { return m_index; }

    void center(double& x, double& y)   { x = m_xCenter; y = m_yCenter; }
    double xCenter()                    { return m_xCenter; }
    double yCenter()                    { return m_yCenter; }
    void distance(double& x, double& y) { x = m_xDistance; y = m_yCenter; }
    double xDistance()                  { return m_xDistance; }
    double yDistance()                  { return m_yDistance; }
    double angle()                      { return m_angle; }
    void hinge(double& x, double& y)    { x = m_xHinge; y = m_yHinge; }
    double xHinge()                     { return m_xHinge; }
    double yHinge()                     { return m_yHinge; }

    int shapeType()    { return m_shapeType; }
    int drawMode()     { return m_drawMode; }
    size_t textSize()  { return m_textSize; }

    double lineWidth() { return m_lineWidth; }
    QColor lineColor() { return m_lineColor; }
    QColor fillColor() { return m_fillColor; }

    QString variableValue() { return m_variableValue; }
    QString note()          { return m_note; }

    DOF* xCenterDOF()               { return m_xCenterDOF; }
    DOF* yCenterDOF()               { return m_yCenterDOF; }
    DOF* widthDOF()                 { return m_widthDOF; }
    DOF* heightDOF()                { return m_heightDOF; }
    DOF* angleDOF()                 { return m_angleDOF; }
    DOF* colorDOF()                 { return m_colorDOF; }
    QList<double> colorYValues()    { return m_colorYValues; }
    DOF* opacityDOF()               { return m_opacityDOF; }
    QList<double> opacityYValues()  { return m_opacityYValues; }
    DOF* textDOF()                  { return m_textDOF; }

    // -- visualization ---------------------------------------------
    void visualize(
        const bool& inSelectMode,
        double pixelSize);
    void visualize(
        double pixelSize,
        const double& opacity,
        const std::vector< Attribute* > attrs,
        const std::vector< double > attrValIdcs);

    void setTransf();
    void clrTransf();

    // -- event handlers --------------------------------------------
    void handleHit(const size_t& hdlIdx);

    // -- public constants ------------------------------------------
    enum
    {
      TYPE_LINE,
      TYPE_RECT,
      TYPE_ELLIPSE,
      TYPE_ARROW,
      TYPE_DARROW,
      TYPE_NOTE,

      MODE_NORMAL,
      MODE_EDIT,
      MODE_EDIT_DOF_XCTR,
      MODE_EDIT_DOF_YCTR,
      MODE_EDIT_DOF_HGT,
      MODE_EDIT_DOF_WTH,
      MODE_EDIT_DOF_AGL,
      MODE_EDIT_DOF_COL,
      MODE_EDIT_DOF_OPA,
      MODE_EDIT_DOF_TEXT,

      ID_HDL_CTR,
      ID_HDL_TOP_LFT,
      ID_HDL_LFT,
      ID_HDL_BOT_LFT,
      ID_HDL_BOT,
      ID_HDL_BOT_RGT,
      ID_HDL_RGT,
      ID_HDL_TOP_RGT,
      ID_HDL_TOP,
      ID_HDL_ROT_RGT,
      ID_HDL_ROT_TOP,

      ID_HDL_DOF_BEG,
      ID_HDL_DOF_END,
      ID_HDL_HGE,
      ID_HDL_DIR
    };
    static double hdlSzeHnt;
    static double minSzeHnt;
    static int    segNumHnt;
    static QColor colTxt;

  protected:
    // -- private utility functions ---------------------------------
    void initDOF();
    void clearDOF();

    void handleHitEdtDOFAgl(const size_t& hdlIdx);

    // -- private visualization functions ---------------------------
    void drawNormal(
        const bool& inSelectMode,
        double pixelSize);
    void drawEdit(
        const bool& inSelectMode,
        double pixelSize);
    void drawText(double pixelSize);
    void drawEditDOF(
        const bool& inSelectMode,
        double pixelSize);
    void drawEditDOFXCtr(
        const bool& inSelectMode,
        double pixelSize);
    void drawDOFXCtr(
        const bool& inSelectMode,
        double pixelSize);
    void drawEditDOFYCtr(
        const bool& inSelectMode,
        double pixelSize);
    void drawDOFYCtr(
        const bool& inSelectMode,
        double pixelSize);
    void drawEditDOFWth(
        const bool& inSelectMode,
        double pixelSize);
    void drawDOFWth(
        const bool& inSelectMode,
        double pixelSize);
    void drawEditDOFHgt(
        const bool& inSelectMode,
        double pixelSize);
    void drawDOFHgt(
        const bool& inSelectMode,
        double pixelSize);
    void drawEditDOFAgl(
        const bool& inSelectMode,
        double pixelSize);
    void drawDOFAgl(
        const bool& inSelectMode,
        double pixelSize);

    // -- data members ----------------------------------------------
    Diagram *m_parentDiagram;
    size_t m_index;

    // geometry
    double m_xCenter,   m_yCenter;    // center,             [-1,1]
    double m_xDistance, m_yDistance;  // bound dist from ctr,norm
    double m_angle;                   // rotation & incline, degrees
    double m_xHinge,    m_yHinge;     // hinge point, relative to center

    // properties
    int         m_shapeType;      // type of shape
    int         m_drawMode;       // drawing mode
    size_t      m_textSize;       // font size

    double      m_lineWidth;      // line width in pix
    QColor      m_lineColor;      // line color
    QColor      m_fillColor;      // fill color

    QString     m_variableValue;  // variable shown on the shape
    QString     m_note;           // note shown on the shape

    GLuint      m_texCharId[CHARSETSIZE];                       // resources for drawing text
    GLubyte     m_texChar[CHARSETSIZE][CHARHEIGHT* CHARWIDTH];  // resources for drawing text
    bool        m_texturesGenerated; // check whether textures for drawing text is generated or not

    // degrees of freedom
    DOF* m_xCenterDOF;  // composition
    DOF* m_yCenterDOF;  // composition
    DOF* m_widthDOF;    // composition
    DOF* m_heightDOF;   // composition
    DOF* m_angleDOF;    // composition
    DOF* m_textDOF;     // composition

    DOF* m_colorDOF;    // composition
    QList<double> m_colorYValues;
    DOF* m_opacityDOF;  // composition
    QList<double> m_opacityYValues;
};

#endif

// -- end -----------------------------------------------------------
