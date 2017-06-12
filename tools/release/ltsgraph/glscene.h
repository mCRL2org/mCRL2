// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file glscene.h
  @author S. Cranen, R. Boudewijns

  This file contains an interface to the OpenGL renderer used by LTSGraph.

*/

#ifndef GLSCENE_H
#define GLSCENE_H

#include <QColor>
#include <QPainter>

#include <gl2ps.h>

#include "graph.h"

struct Texture
{
  GLuint name;
  size_t width;
  size_t height;
  QVector3D shape[4];
  Texture(size_t width, size_t height, float pixelsize) : width(width), height(height)
  {
    glGenTextures(1, &name);
    resize(pixelsize);
  }
  ~Texture()
  {
    glDeleteTextures(1, &name);
  }
  void resize(float pixelsize)
  {
    const GLfloat w = width;
    const GLfloat h = height;
    shape[0] = QVector3D(-w, -h, 0.0f) * pixelsize / 2.0;
    shape[1] = QVector3D(w, -h, 0.0f) * pixelsize / 2.0;
    shape[2] = QVector3D(w,  h, 0.0f) * pixelsize / 2.0;
    shape[3] = QVector3D(-w,  h, 0.0f) * pixelsize / 2.0;
  }
};

struct VertexData
{
  static const int handleSize = 8;
  static const int arrowheadSize = 12;

  QVector3D* node{nullptr}, *hint{nullptr}, *handle{nullptr}, *arrowhead{nullptr}, *transition_labels, *state_labels, *number_labels;

  ~VertexData() { clear(); }
  void clear();
  void generateVertexData(float handlesize, float nodesize, float arrowheadsize);
};

struct CameraView
{
  QQuaternion rotation;   ///< Rotation of the camera
  QVector3D translation;  ///< Translation of the camera
  QVector3D world;        ///< The size of the box in which the graph lives
  float zoom{1.0};        ///< Zoom specifies by how much the view angle is narrowed. Larger numbers mean narrower angles.
  float pixelsize{1};

  CameraView()
    : rotation(QQuaternion(1, 0, 0, 0)), translation(QVector3D(0, 0, 0)),
    world(QVector3D(1000.0, 1000.0, 1000.0))
  { }

  void viewport(size_t width, size_t height);
  void billboard_spherical(const QVector3D& pos);
  void billboard_cylindrical(const QVector3D& pos);
  void applyTranslation();
  void applyFrustum();
  void applyPickMatrix(GLdouble x, GLdouble y, GLdouble fuzz);
};

struct CameraAnimation : public CameraView
{
  CameraView m_source, m_target;
  size_t m_animation{0};
  size_t m_animation_steps{0};
  bool m_resizing{false};

  void start_animation(size_t steps);
  void operator=(const CameraView& other);
  void interpolate_cam(float pos);
  void interpolate_world(float pos);
  void animate();
  void viewport(size_t width, size_t height);
  bool resizing();
  void setZoom(float factor, size_t animation);
  void setRotation(const QQuaternion& rotation, size_t animation);
  void setTranslation(const QVector3D& translation, size_t animation);
  void setSize(const QVector3D& size, size_t animation);
  bool animationFinished() {
    return m_animation_steps == 0 || m_animation >= m_animation_steps;
  }
};

class GLScene
{
  private:
    QOpenGLWidget& m_glwidget; ///< The widget where this scene is drawn
    Graph::Graph& m_graph;     ///< The graph that is being visualised.
    CameraAnimation m_camera;  ///< Implementation details of the OpenGL camera handling.
    VertexData m_vertexdata;   ///< Implementation details storing pre-calculated vertices.
    float m_device_pixel_ratio;
    QFont m_font;
    QPainter m_renderpainter;
    QPainter m_selectpainter;

    bool m_drawtransitionlabels;   ///< Transition labels are only drawn if this field is true.
    bool m_drawstatelabels;        ///< State labels are only drawn if this field is true.
    bool m_drawstatenumbers;       ///< State numbers are only drawn if this field is true.
    bool m_drawselfloops;          ///< Self loops are only drawn if this field is true.
    bool m_drawinitialmarking;     ///< The initial state is marked if this field is true.
    size_t m_size_node;            ///< Variable node size (radius).
    size_t m_fontsize;             ///< Variable font size

    bool m_drawfog;                ///< Fog is rendered only if this field is true.
    float m_fogdistance;           ///< The distance at which the fog starts

    /**
     * @brief Renders a single edge.
     * @param i The index of the edge to render.
     */
    void renderEdge(size_t i);

    /**
     * @brief Renders a single edge handle.
     * @param i The index of the edge of the handle to render.
     */
    void renderHandle(GLuint i);

    /**
     * @brief Renders a single node.
     * @param i The index of the node to render.
     */
    void renderNode(GLuint i);

    /**
     * @brief Renders a single edge label.
     * @param i The index of the edge of the label to render.
     */
    void renderTransitionLabel(GLuint i);

    /**
     * @brief Renders a single state label.
     * @param i The index of the state of the label to render.
     */
    void renderStateLabel(GLuint i);

    /**
     * @brief Renders a single state number.
     * @param i The index of the state number to render.
     */
    void renderStateNumber(GLuint i);

    /**
     * @brief Renders text, centered around the point at x and y
     */
    QRect drawCenteredText(float x, float y, const QString& text);

    /**
     * @brief Generates TikZ code for a single edge.
     * @param i The index of the edge to render.
     * @param aspectRatio The aspect ratio used for placement.
     */
    QString tikzEdge(size_t i, float aspectRatio);

    /**
     * @brief Generates TikZ code for a single node.
     * @param i The index of the node to render.
     * @param aspectRatio The aspect ratio used for placement.
     */
    QString tikzNode(size_t i, float aspectRatio);

  public:

    /**
     * @brief An enumeration that identifies the types of objects that
              can be selected. The order determines priority during selection.
     */
    enum SelectableObject
    {
      so_none,     ///< Nothing was selected.
      so_edge,     ///< An edge was selected.
      so_label,    ///< An edge label was selected.
      so_slabel,   ///< An state label was selected.
      so_handle,   ///< An edge handle was selected.
      so_node      ///< A node was selected.
    };

    /**
     * @brief A structure that identifies a selectable object from m_graph.
     */
    struct Selection
    {
      SelectableObject selectionType; ///< The type of object.
      size_t index;                   ///< The index of the object in m_graph.
    };

    /**
     * @brief Constructor.
     * @param glwidget The widget where this scene is drawn
     * @param g The graph that is to be visualised by this object.
     */
    GLScene(QOpenGLWidget& glwidget, Graph::Graph& g);

    /**
     * @brief Applies the current fog settings. Call when the
     *        fog settings have changed.
     */
    void updateFog();

    /**
     * @brief Rebuilds the shapes for nodes, handles, arrowheads and labels.
     *        Call whenever the size of a pixel in world coordinates changes.
     */
    void updateShapes();

    /**
     * @brief Initialises the OpenGL context.
     * @param clear The colour to use as a background colour. This colour is
     *              also used for the fog in 3D mode if enabled.
     */
    void init(const QColor& clear);

    /*
     * @brief Initiales the painter and calls painter.begin()
     */
    void startPainter(QPainter& painter);

    /**
     * @brief Render the scene.
     */
    void render();

    /**
     * @brief Resize the OpenGL viewport.
     * @param width The new width for the viewport.
     * @param height The new height for the viewport.
     */
    void resize(size_t width, size_t height);

    /**
     * @brief Zoom in by @e factor.
     * @param factor The amount by which to multiply the current zoom factor.
     */
    void zoom(float factor);

    /**
     * @brief Retrieve the current magnification factor
     */
    float magnificationFactor() const;

    /**
     * @brief Rotate by @e delta.
     * @param delta The rotation quaternion.
     */
    void rotate(const QQuaternion& delta);

    /**
     * @brief Translate by @e amount.
     * @param amount The translation in world coordinates to add to the current
     *               translation.
     */
    void translate(const QVector3D& amount);

    /**
     * @brief Returns true if the world size changed since the last call.
     * @return True if the world size changed since the last call, false
     *              otherwise.
     */
    bool resizing();

    /**
     * @brief Set the zoom factor to @e factor, interpolating to the new value
     *        for a number of frames if desired.
     * @param factor The new zoom factor.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setZoom(float factor, size_t animation = 1);

    /**
     * @brief Set the rotation quaternion to @e rotation, interpolating to the
     *        new value for a number of frames if desired.
     * @param rotation The new rotation.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setRotation(const QQuaternion& rotation, size_t animation = 1);

    /**
     * @brief Set the translation to @e amount, interpolating to the new value
     *        for a number of frames if desired.
     * @param translation The new translation.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setTranslation(const QVector3D& translation, size_t animation = 1);

    /**
     * @brief Set the world size to @e size, interpolating to the new value
     *        for a number of frames if desired.
     * @param size The new world size.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setSize(const QVector3D& size, size_t animation = 1);

    /**
     * @brief Returns the current world size.
     * @return The current world size in world coordinates.
     */
    QVector3D size();

    /**
     * @brief Converts viewport coordinates to world coordinates.
     * @param x The viewport X coordinate.
     * @param y The viewport Y coordinate (warning: the origin is in the
     *          bottom left, not the top right as is usual in GUI programming).
     * @param z The viewport Z coordinate (depth buffer value). If not given, the
     *          value of the depth buffer at (x, y) is used.
     * @return The world coordinates corresponding to the eye coordinates provided.
     */
    QVector3D eyeToWorld(int x, int y, GLfloat z = -1) const;

    /**
     * @brief Converts world coordinates to viewport coordinates.
     * @param world The world coordinates.
     * @returns The eye coordinates, where the origin is at the bottom left. The
     *          Z value is the value of the depth buffer at the requested location.
     */
    QVector3D worldToEye(const QVector3D& world) const;

    /**
     * @brief Retrieve the object at viewport coordinate (x, y).
     * @returns A record that represents the selected object.
     */
    Selection select(int x, int y);

    /**
     * @brief Select an object of type @type
     * @returns true if an object was selected
     */
    bool selectObject(Selection& s, int x, int y, SelectableObject type);

    /**
     * @brief Renders the scene to a file using gl2ps.
     */
    void renderVectorGraphics(const char* filename, GLint format = GL2PS_PDF);


    /**
     * @brief Renders the scene to LaTeX using Tikz.
     */
    void renderLatexGraphics(const QString& filename, float aspectRatio = 1.0f);

    //Getters and setters
    bool drawStateLabels() const {
      return m_drawstatelabels;
    }
    bool drawTransitionLabels() const {
      return m_drawtransitionlabels;
    }
    size_t nodeSize() const {
      return m_size_node;
    }
    size_t fontSize() const {
      return m_fontsize;
    }
    float nodeSizeOnScreen() const {
      return m_size_node * m_camera.pixelsize * m_device_pixel_ratio;
    }
    float handleSizeOnScreen() const {
      return VertexData::handleSize * m_camera.pixelsize * m_device_pixel_ratio;
    }
    float arrowheadSizeOnScreen() const {
      return VertexData::arrowheadSize * m_camera.pixelsize * m_device_pixel_ratio;
    }
    float  fogDistance() const {
      return m_fogdistance;
    }
    void setDrawTransitionLabels(bool drawLabels) {
      m_drawtransitionlabels = drawLabels;
    }
    void setDrawStateLabels(bool drawLabels) {
      m_drawstatelabels = drawLabels;
    }
    void setDrawStateNumbers(bool drawLabels) {
      m_drawstatenumbers = drawLabels;
    }
    void setDrawSelfLoops(bool drawLoops) {
      m_drawselfloops = drawLoops;
    }
    void setDrawInitialMarking(bool drawMark) {
      m_drawinitialmarking = drawMark;
    }
    void setDrawFog(bool drawFog) {
      m_drawfog = drawFog;
      updateFog();
    }
    void setNodeSize(size_t size) {
      m_size_node = size;
    }
    void setFontSize(size_t size) {
      m_font.setPixelSize(m_fontsize = size);
    }
    void setFogDistance(float dist) {
      m_fogdistance = dist;
      updateFog();
    }
    bool animationFinished() {
      return m_camera.animationFinished();
    }
    void setDevicePixelRatio(float device_pixel_ratio) {
      m_device_pixel_ratio = device_pixel_ratio;
    }
};

#endif // GLSCENE_H
