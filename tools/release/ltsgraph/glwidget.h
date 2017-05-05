// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file glwidget.h
  @author S. Cranen, R. Boudewijns

  This file contains an Qt OpenGL widget which uses the GLScene interface to render the contents of a graph.

*/

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QColorDialog>
#include <QVector2D>
#include "ui_glwidget.h"

#include "mcrl2/lts/lts.h"
#include "mcrl2/gui/workarounds.h"

#include "graph.h"
#include "glscene.h"

class GLWidgetImpl;

class GLWidgetUi;

struct MoveRecord;

class GLWidget : public QGLWidget
{
    Q_OBJECT
  public:

    /**
     * @brief An enumeration that identifies the type operation during drag.
     */
    enum DragMode
    {
      dm_none,          ///< No operation is active.
      dm_dragnode,      ///< A node is dragged.
      dm_rotate,        ///< 3D rotation is active.
      dm_rotate_2d,     ///< 2D rotation is active.
      dm_translate,     ///< World translation is active.
      dm_zoom,          ///< Zooming is active.
      dm_paint          ///< Painting is active.
    };
  private:
    GLWidgetUi* m_ui;           ///< The user interface of the class.
    Graph::Graph& m_graph;      ///< The current graph.
    GLScene::Selection m_hover; ///< The current object (if any) which is pointed at.
    DragMode m_dragmode;        ///< The current drag mode.
    MoveRecord* m_dragnode;     ///< The current node (if any) which is being dragged.
    QPoint m_dragstart;         ///< The coordinate at which the dragging started.
    QVector2D m_draglength;     ///< The accumulated distance reached while dragging.
    QColor m_paintcolor;        ///< The color of the paint operation.
    bool m_painting;            ///< Boolean indicating if painting is enabled.
    bool m_paused;
    GLScene* m_scene;           ///< The GLScene which is used to render the contents.
    std::list<GLScene::Selection> m_selections; ///< A list of the objects under the cursor.


    /**
     * @brief Renders a single edge.
     * @param i The index of the edge to render.
     */
    void drawEdge(size_t i);

    /**
     * @brief Renders a single Node.
     * @param i The index of the node to render.
     */
    void drawNode(size_t i);

    /**
     * @brief Renders a single label.
     * @param i The index of the label to render.
     */
    void drawLabel(size_t i);

    /**
     * @brief Renders a single handle.
     * @param i The index of the handle to render.
     */
    void drawHandle(size_t i);

    /**
     * @brief Renders the scene.
     */
    void drawScene();

    /**
     * @brief Updates the selected value for all nodes.
     */
    void updateSelection();
  public:

    /**
     * @brief Constructor.
     * @param graph The graph that is to be visualised by this object.
     * @param parent The parent widget for the user interface.
     */
    explicit GLWidget(Graph::Graph& graph, QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~GLWidget() override;


    /**
     * @brief Initialises the OpenGL context.
     */
    void initializeGL() override;

    /**
     * @brief Paints the OpenGL context.
     */
    void paintGL() override;

    /**
     * @brief Pauses painting and clears the selection. Used to make the
     *        GLWidget wait while a new graph is loaded.
     */
    void pause();

    /**
     * @brief Resumes painting after a call to pause().
     */
    void resume();

    /**
     * @brief Resize the OpenGL viewport.
     * @param width The new width for the viewport.
     * @param height The new height for the viewport.
     */
    void resizeGL(int width, int height) override;

    /**
     * @brief Processes mouse click events.
     * @param e The mouse event.
     */
    void mousePressEvent(QMouseEvent* e) override;

    /**
     * @brief Processes mouse click release events.
     * @param e The mouse event.
     */
    void mouseReleaseEvent(QMouseEvent* e) override;

    /**
     * @brief Processes mouse move events.
     * @param e The mouse event.
     */
    void mouseMoveEvent(QMouseEvent* e) override;

    /**
     * @brief Processes mouse scroll/wheel events.
     * @param e The mouse event.
     */
    void wheelEvent(QWheelEvent* e) override;


    /**
     * @brief Sets the depth of the scene using an optional animation.
     * @param depth The new depth.
     * @param animation The number of animation steps to use.
     */
    void setDepth(float depth, size_t animation = 1);

    /**
     * @brief Updates all shapes and labels.
     */
    void rebuild();

    /**
     * @brief Renders the current visualisation to a bitmap file.
     * @param filename The filename for the output (extension determines file type).
     * @param w The desired width of the image.
     * @param h The desired height of the image.
     */
    void savePixmap(const QString& filename, int w = 1024, int h = 768);

    /**
         * @brief Renders the current visualisation to a vector graphics file using gl2ps.
         * @param filename The filename for the output (extension determines file type).
         */
    void saveVector(const QString& filename);

    /**
         * @brief Renders the current visualisation to a LaTeX Tikz image.
         * @param filename The filename for the output.
         */
    void saveTikz(const QString& filename, float aspectRatio);

    /**
     * @brief Sets the paint color.
     * @param color The desired color.
     */
    void setPaint(const QColor& color);

    /**
     * @brief Enables painting with the color set by @fn setPaint.
     */
    void startPaint();

    /**
     * @brief Disables painting.
     */
    void endPaint();

    /**
     * @brief Returns the world size in 3D coordinates.
     */
    QVector3D size3();

    /**
     * @brief Returns the user interface object. If no user interface is available,
     *        one is created using the provided @e parent.
     * @param The parent of the user inferface in the case none exists yet.
     */
    GLWidgetUi* ui(QWidget* parent = nullptr);


    //Getters and setters
    size_t nodeSize() {
      return m_scene->nodeSize();
    }
    float fogDistance() {
      return m_scene->fogDistance();
    }
  signals:
    void widgetResized(const QVector3D& newsize);
  public slots:

    /**
     * @brief Resets the viewpoint of the camera to the original position.
     * @param animation The desired number of animation steps to use.
     */
    void resetViewpoint(size_t animation = 1);

    //Getters and setters
    void toggleTransitionLabels(bool show) {
      m_scene->setDrawTransitionLabels(show);
    }
    void toggleStateLabels(bool show) {
      m_scene->setDrawStateLabels(show);
    }
    void toggleStateNumbers(bool show) {
      m_scene->setDrawStateNumbers(show);
    }
    void toggleSelfLoops(bool show) {
      m_scene->setDrawSelfLoops(show);
    }
    void toggleInitialMarking(bool show) {
      m_scene->setDrawInitialMarking(show);
    }
    void toggleFog(bool show) {
      m_scene->setDrawFog(show);
    }
    void setNodeSize(int size) {
      m_scene->setNodeSize(size);
      m_scene->updateShapes();
    }
    void setFogDistance(int dist) {
      m_scene->setFogDistance(dist);
    }
};

class GLWidgetUi : public QDockWidget
{
    Q_OBJECT
  private:
    GLWidget& m_widget;             ///< The corresponding GLWidget for this user interface.
    Ui::GLWidget m_ui;              ///< The user interface generated by Qt.
    QColorDialog* m_colordialog;    ///< The color dialog used to pick a paint color.
  public:

    /**
     * @brief Constructor.
     * @param widget The GLWidget object this user interface corresponds to.
     * @param parent The parent widget for this user interface.
     */
    GLWidgetUi(GLWidget& widget, QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~GLWidgetUi() override;
  public slots:

    /**
     * @brief Slot which processes the @e color when selected by the color dialog.
     * @param The selected color.
     */
    void selectColor(const QColor& color);

    /**
     * @brief Slot which processes the selection or deselection of the paint button.
     * @param paint Indicates the painting should be enabled if true, disabled otherwise.
     */
    void togglePaintMode(bool paint);
};

#endif // GLWIDGET_H
