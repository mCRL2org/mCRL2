// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_LTSGRAPH_GLWIDGET_H
#define MCRL2_LTSGRAPH_GLWIDGET_H

#include "glscene.h"

#include <QColorDialog>
#include <QVector2D>
#include <QOpenGLDebugLogger>
#include "ui_glwidget.h"

class GLWidgetImpl;

class GLWidgetUi;

struct MoveRecord;

/// \brief A Qt OpenGL widget which uses the GLScene interface to render the
/// contents of a graph.
class GLWidget : public QOpenGLWidget
{
  Q_OBJECT
  public:
  /**
   * @brief An enumeration that identifies the type operation during drag.
   */
  enum DragMode
  {
    dm_none,      ///< No operation is active.
    dm_dragnode,  ///< A node is dragged.
    dm_rotate,    ///< 3D rotation is active.
    dm_rotate_2d, ///< 2D rotation is active.
    dm_translate, ///< World translation is active.
    dm_zoom,      ///< Zooming is active.
    dm_paint      ///< Painting is active.
  };

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
   * @brief Resize the OpenGL viewport.
   * @param width The new width for the viewport.
   * @param height The new height for the viewport.
   */
  void resizeGL(int width, int height) override;

  /**
   * @brief Processes mouse click events.
   * @param e The mouse event.
   */
  bool event(QEvent* e) override
  {
    mCRL2log(mcrl2::log::debug) << "Processing Qt event " << e->type() << "\n";

    // call super's event. 
    return QOpenGLWidget::event(e);
  } 

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
   * @brief Gets 3D mode
   */
  bool get3D();

  /**
   * @brief Should be called whenever the graph changes.
   */
  void rebuild();

  /**
   * @brief Renders the current visualisation to a bitmap file.
   * @param filename The filename for the output (extension determines file
   * type).
   */
  void saveBitmap(const QString& filename);

  /**
   * @brief Renders the current visualisation to a vector file.
   * @tparam Format The type of vector image format used.
   * @param filename The filename for the output.
   * @note see export{Format}.cpp for the implementation
   */
  template <typename Format> void saveVector(const QString& filename);

  /**
   * @brief Sets the paint color.
   * @param color The desired color.
   */
  void setPaint(const QColor& color);
  
  /**
   * @brief Gets the paint color.
   */
  const QVector3D& getPaint() const;

  /**
   * @brief Enables painting with the color set by @fn setPaint.
   */
  void startPaint();

  /**
   * @brief Disables painting.
   */
  void endPaint();

  bool isPainting() const
  {
    return m_painting;
  }

  /**
   * @brief Returns the user interface object. If no user interface is
   * available, one is created using the provided @e parent.
   * @param The parent of the user inferface in the case none exists yet.
   */
  GLWidgetUi* ui(QWidget* parent = nullptr);

  /**
   * @brief Returns whether to draw debug graphs
   *
   */
  bool getDebugDrawGraphs()
  {
    return m_drawDebugGraphs;
  }

  public slots:

  /**
   * @brief sets 3D mode
   */
  void set3D(bool enabled);

  /**
   * @brief Resets the viewpoint of the camera to the original position.
   * @param animation The desired number of animation steps to use.
   */
  void resetViewpoint(std::size_t animation = 0);

  /// \brief Logs the given message to the mCRL2 output.
  void logMessage(const QOpenGLDebugMessage& debugMessage);

  /// Slots for various UI interactions.
  void setLimitTransLabels(int a)
  {
    m_scene.m_textLimitTransLabels = a;
    rebuild();
  }

  void setLimitStateLabels(int a)
  {
    m_scene.m_textLimitStateLabels = a;
    rebuild();
  }

  void setLimitStateNumbers(int a)
  {
    m_scene.m_textLimitStateNumbers = a;
    rebuild();
  }

  void toggleDebugDrawGraphs(bool show)
  {
    m_drawDebugGraphs = show;
    rebuild();
  }

  void toggleTransitionLabels(bool show)
  {
    m_scene.setDrawTransitionLabels(show);
    rebuild();
  }

  void toggleStateLabels(bool show)
  {
    m_scene.setDrawStateLabels(show);
    rebuild();
  }

  void toggleStateNumbers(bool show)
  {
    m_scene.setDrawStateNumbers(show);
    rebuild();
  }

  void toggleSelfLoops(bool show)
  {
    m_scene.setDrawSelfLoops(show);
    rebuild();
  }

  void toggleInitialMarking(bool show)
  {
    m_scene.setDrawInitialMarking(show);
    rebuild(); 
  }

  void toggleFog(bool enabled)
  {
    m_scene.setDrawFog(enabled);
    rebuild();
  }

  void setNodeSize(int size)
  {
    m_scene.setNodeSize(size);
    rebuild();
  }

  void setFontSize(int size)
  {
    m_scene.setFontSize(size);
    rebuild();
  }

  void setFogDensity(int value)
  {
    m_scene.setFogDistance(value);
    rebuild();
  }

  /**
   * Paints all the deadlocks in the graph with the selected color. 
   */
  void paintDeadlocks();

  private:
  GLWidgetUi* m_ui = nullptr; ///< The user interface of the class.
  Graph::Graph& m_graph;      ///< The current graph.
  GLScene m_scene; ///< The GLScene which is used to render the contents.
  QOpenGLDebugLogger* m_logger; ///< Logs OpenGL debug messages.
  qreal m_current_device_pixel_ratio;
  GLScene::Selection
      m_hover;         ///< The current object (if any) which is pointed at.
  public:              /// TODO: Expose nicely
  DragMode m_dragmode = dm_none; ///< The current drag mode.
  private:
  MoveRecord* m_dragnode = nullptr; ///< The current node (if any) which is being dragged.
  QPoint m_dragstart;     ///< The coordinate at which the dragging started.
  QVector2D m_draglength; ///< The accumulated distance reached while dragging.
  QVector3D m_paintcolor; ///< The color of the paint operation.

  bool m_painting =
      false; ///< Indicates wheter painting node by the used is enabled.
  /* bool m_is_threedimensional = false; ///< Indicates that the scene should be
                                      ///< viewed in 3D, as opposed to 2D. */
  // bool m_has_new_frame = false;  // Does not seem to be used.
  bool m_drawDebugGraphs = false; ///< Drawing debug graphs can be turned on/off
                                  ///< in advanced layout dialog

  std::list<GLScene::Selection>
      m_selections; ///< A list of the objects under the cursor.

  /**
   * @brief Updates the selected value for all nodes.
   */
  void updateSelection();
};

class GLWidgetUi : public QDockWidget
{
  Q_OBJECT
  private:
  GLWidget& m_widget; ///< The corresponding GLWidget for this user interface.
  QColorDialog* m_colordialog; ///< The color dialog used to pick a paint color.
  public:
  Ui::GLWidget m_ui;  ///< The user interface generated by Qt.
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
   * @brief Slot which processes the selection or deselection of the paint
   * button.
   * @param paint Indicates the painting should be enabled if true, disabled
   * otherwise.
   */
  void setPaintMode(bool paint);

  /**
   * @brief Get the current state of the settings.
   */
  QByteArray settings();

  /**
   * @brief Restore the settings of the given state.
   */
  void setSettings(QByteArray state);

  friend class GLWidget;
};

#endif // MCRL2_LTSGRAPH_GLWIDGET_H
