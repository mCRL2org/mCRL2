/**

  @file glscene.h
  @author S. Cranen

  This file contains an interface to the OpenGL renderer used by LTSGraph.

*/
#ifndef GLSCENE_H
#define GLSCENE_H

#include "graph.h"
#include "gl2ps.h"

struct VertexData;
struct TextureData;
struct CameraView;
struct CameraAnimation;

class GLScene
{
private:
    Graph::Graph& m_graph;      ///< The graph that is being visualised.
    VertexData *m_vertexdata;   ///< Implementation details storing pre-calculated vertices.
    TextureData *m_texturedata; ///< Implementation details storing labels as textures.
    CameraAnimation *m_camera;  ///< Implementation details of the OpenGL camera handling.

    bool m_drawlabels;          ///< Labels are only drawn if this field is true.

    /**
     * @brief Renders a single edge.
     * @param i The index of the edge to render.
     */
    void renderEdge(size_t i);

    /**
     * @brief Renders a single edge handle.
     * @param i The index of the edge of the handle to render.
     */
    void renderHandle(size_t i);

    /**
     * @brief Renders a single node.
     * @param i The index of the node to render.
     */
    void renderNode(size_t i);

    /**
     * @brief Renders a single edge label.
     * @param i The index of the edge of the label to render.
     */
    void renderLabel(size_t i);
public:

    /**
     * @brief An enumeration that identifies the types of objects that
              can be selected.
     */
    enum SelectableObject
    {
        so_none,     ///< Nothing was selected.
        so_edge,     ///< An edge was selected.
        so_label,    ///< An edge label was selected.
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
     * @param g The graph that is to be visualised by this object.
     */
    GLScene(Graph::Graph& g);

    /**
     * @brief Destructor.
     */
    ~GLScene();

    /**
     * @brief Rebuilds the textures used to render labels. Call when the
     *        graph has changed.
     */
    void updateLabels();

    /**
     * @brief Rebuilds the shapes for nodes, handles, arrowheads and labels.
     *        Call whenever the size of a pixel in world coordinates changes.
     * @todo This construction is not very nice, and should be replaced by
     *       a simple scaling transform based on the pixel size.
     */
    void updateShapes();

    /**
     * @brief Initialises the OpenGL context.
     * @param clear The colour to use as a background colour. This colour is
     *              also used for the fog in 3D mode.
     */
    void init(const QColor& clear);

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
     * @brief Rotate by @e amount.
     * @param amount The rotations around the X, Y and Z axes to add to the
     *               current rotations.
     */
    void rotate(Graph::Coord3D amount);

    /**
     * @brief Translate by @e amount.
     * @param amount The translation in world coordinates to add to the current
     *               translation.
     */
    void translate(const Graph::Coord3D& amount);

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
     * @brief Set the rotations to @e amount, interpolating to the new value
     *        for a number of frames if desired.
     * @param amoount The new rotation.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setRotation(const Graph::Coord3D& amount, size_t animation = 1);

    /**
     * @brief Set the translation to @e amount, interpolating to the new value
     *        for a number of frames if desired.
     * @param amount The new translation.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setTranslation(const Graph::Coord3D& amount, size_t animation = 1);

    /**
     * @brief Set the world size to @e size, interpolating to the new value
     *        for a number of frames if desired.
     * @param size The new world size.
     * @param animation The amount of frames to animate. 0 means apply
     *        immediately, 1 applies the change in the next frame. Higher
     *        values cause a gradual change.
     */
    void setSize(const Graph::Coord3D& size, size_t animation = 1);

    /**
     * @brief Returns the current world size.
     * @return The current world size in world coordinates.
     */
    Graph::Coord3D size();

    /**
     * @brief Converts viewport coordinates to world coordinates.
     * @param x The viewport X coordinate.
     * @param y The viewport Y coordinate (warning: the origin is in the
     *          bottom left, not the top right as is usual in GUI programming).
     * @param z The viewport Z coordinate (depth buffer value). If not given, the
     *          value of the depth buffer at (x, y) is used.
     * @return The world coordinates corresponding to the eye coordinates provided.
     */
    Graph::Coord3D eyeToWorld(int x, int y, GLfloat z = -1);

    /**
     * @brief Converts world coordinates to viewport coordinates.
     * @param pos The world coordinates.
     * @returns The eye coordinates, where the origin is at the bottom left. The
     *          Z value is the value of the depth buffer at the requested location.
     */
    Graph::Coord3D worldToEye(const Graph::Coord3D& pos);

    /**
     * @brief Retrieve the object at viewport coordinate (x, y).
     * @returns A record that represents the selected object.
     */
    Selection select(int x, int y);

    /**
     * @brief Renders the scene to a file using gl2ps.
     */
    void renderVectorGraphics(const char* filename, GLint format = GL2PS_PDF);

    bool drawLabels() const { return m_drawlabels; }
    void setDrawLabels(bool drawLabels) { m_drawlabels = drawLabels; }
};

#endif // GLSCENE_H
