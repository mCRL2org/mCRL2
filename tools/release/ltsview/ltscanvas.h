// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LTSCANVAS_H
#define LTSCANVAS_H

#include <QCursor>
#include <QGLWidget>
#include <QImage>
#include <QMouseEvent>
#include <QPoint>
#include <QWheelEvent>
#include <QQuaternion>

#include "ltsmanager.h"
#include "markmanager.h"
#include "settings.h"
#include "vectors.h"
#include "visualizer.h"

class LtsCanvas : public QGLWidget
{
  Q_OBJECT

  private:
    enum Tool { SelectTool, PanTool, ZoomTool, RotateTool };
    enum ObjectType { StateObject, ClusterObject };
    struct Selection { State* state; Cluster* cluster; };

  public:
    LtsCanvas(QWidget* parent, Settings* settings, LtsManager* ltsManager, MarkManager* markManager);
    int viewWidth() const { return m_width; }
    int viewHeight() const { return m_height; }

  public slots:
    void resetView();
    void exportToText(QString filename) { m_visualizer->exportToText(filename.toStdString()); }

  protected slots:
    void clusterPositionsChanged();

  protected:
    void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
    void render(bool light);
    void determineActiveTool(QMouseEvent* event, bool useModifiers);
    void setActiveTool(Tool tool);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    Selection selectObject(QPoint position);
    Selection parseSelection(GLuint* selectionBuffer, GLint items);
    void applyRotation(bool reverse = false);
    Vector3D getArcBallVector(int screenX, int screenY);

  public:
    QImage renderImage(int width, int height);
    void renderVectorStart() { emit renderingStarted(); }
    void renderVectorPage() { render(false); }
    void renderVectorFinish() { emit renderingFinished(); }

  signals:
    void renderingStarted();
    void renderingFinished();

  private:
    Settings* m_settings;
    LtsManager* m_ltsManager;
    Visualizer* m_visualizer;

    QCursor m_selectCursor;
    QCursor m_panCursor;
    QCursor m_zoomCursor;
    QCursor m_rotateCursor;

    int m_width;
    int m_height;
    float m_baseDepth;
    float m_nearPlane;
    float m_farPlane;
    Vector3D m_position;
    QQuaternion m_rotation;
    Tool m_activeTool;
    bool m_dragging;
    QPoint m_lastMousePosition;
};

#endif
