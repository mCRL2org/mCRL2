#include <QtOpenGL>
#include <assert.h>
#include <cstdio>
#include "glscene.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "GL/glu.h" // Needed for compilation on Ubuntu 12.04
#endif

#define RES_ARROWHEAD  30  ///< Amount of segments in arrowhead cone
#define RES_ARC        20  ///< Amount of segments for edge arc
#define RES_NODE_SLICE 16
#define RES_NODE_STACK  4

#define SIZE_HANDLE     8
#define SIZE_NODE      20
#define SIZE_ARROWHEAD 12

typedef Graph::Coord3D Coord3D;

struct GLHitRecord
{
    GLuint stackSize;
    GLuint minDepth;
    GLuint maxDepth;
    GLuint stack[0];
};

struct Color3f
{
    GLfloat r, g, b;
    Color3f() {}
    Color3f(GLfloat r, GLfloat g, GLfloat b) : r(r), g(g), b(b) {}
    Color3f(GLfloat* c) : r(c[0]), g(c[1]), b(c[2]) {}
    operator const GLfloat*() const { return &r; }
};

struct TextureData
{
    size_t* widths;
    size_t* heights;
    GLuint* textures;
    size_t count;

    TextureData()
        : widths(NULL), heights(NULL), textures(NULL), count(0)
    { }

    ~TextureData()
    {
        glDeleteTextures(count, textures);
        delete[] widths;
        delete[] heights;
        delete[] textures;
    }

    void generate(const Graph::Graph& g)
    {
        QFont font;
        QFontMetrics metrics(font);
        QPainter p;


        glDeleteTextures(count, textures);
        delete[] widths;
        delete[] heights;
        delete[] textures;

        count = g.labelCount();
        textures = new GLuint[count];
        widths = new size_t[count];
        heights = new size_t[count];

        glGenTextures(count, textures);

        for (size_t i = 0; i < count; ++i)
        {
            QRect bounds = metrics.boundingRect(0, 0, 0, 0, Qt::AlignLeft, g.labelstring(i));
            QImage label(bounds.width(), bounds.height(), QImage::Format_ARGB32_Premultiplied);
            p.begin(&label);
            p.setFont(font);
            p.setCompositionMode(QPainter::CompositionMode_Clear);
            p.fillRect(bounds, QColor(1, 1, 1, 0));
            p.setCompositionMode(QPainter::CompositionMode_SourceOver);
            p.setPen(QColor(255, 0, 0, 255));
            p.drawText(bounds, g.labelstring(i));
            p.end();
            // Save the original width and height for posterity
            widths[i] = label.width();
            heights[i] = label.height();
            // OpenGL likes its textures to have dimensions that are powers of 2
            int w = 1, h = 1;
            while (w < label.width()) w <<= 1;
            while (h < label.height()) h <<= 1;
            // ... and also wants the alpha component to be the 4th component
            label = QGLWidget::convertToGLFormat(label.scaled(w, h));

            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, label.width(), label.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, label.bits());

            assert(glGetError() == 0);
        }
    }
};

struct VertexData
{
    Coord3D *node, *handle, *arrowhead, *labels;

    VertexData()
        : node(NULL), handle(NULL), arrowhead(NULL), labels(NULL)
    { }

    ~VertexData()
    {
        delete[] node;
        delete[] handle;
        delete[] arrowhead;
        delete[] labels;
    }

    void generate(const TextureData& textures, float pixelsize)
    {
        float handlesize = SIZE_HANDLE * pixelsize,
              nodesize = SIZE_NODE * pixelsize,
              arrowheadsize = SIZE_ARROWHEAD * pixelsize;

        // Delete old data
        delete[] node;
        delete[] handle;
        delete[] arrowhead;
        delete[] labels;

        // Generate vertices for node border (a line loop drawing a circle)
        float slice = 0, sliced = 2.0 * M_PI / (RES_NODE_SLICE - 1),
              stack = 0, stackd = M_PI_2 / RES_NODE_STACK;
        node = new Coord3D[RES_NODE_SLICE - 1 + RES_NODE_SLICE * RES_NODE_STACK * 2];
        for (int i = 0; i < RES_NODE_SLICE - 1; ++i, slice += sliced)
            node[i] = Coord3D(sin(slice), cos(slice), 0.1);
        // Generate vertices for node (a quad strip drawing a half sphere)
        slice = 0;
        size_t n = RES_NODE_SLICE - 1;
        for (int j = 0; j < RES_NODE_STACK; ++j, stack += stackd)
        {
            for (int i = 0; i < RES_NODE_SLICE - 1; ++i, slice += sliced)
            {
                node[n++] = Coord3D( sin(stack + stackd) * sin(slice),
                                     sin(stack + stackd) * cos(slice),
                                     cos(stack + stackd));
                node[n++] = Coord3D( sin(stack) * sin(slice),
                                     sin(stack) * cos(slice),
                                     cos(stack));
            }
            node[n++] = Coord3D( sin(stack + stackd) * sin(0),
                                 sin(stack + stackd) * cos(0),
                                 cos(stack + stackd));
            node[n++] = Coord3D( sin(stack) * sin(0),
                                 sin(stack) * cos(0),
                                 cos(stack));
        }
        for (size_t i = 0; i < n; ++i)
            node[i] *= 0.5 * pixelsize * SIZE_NODE;

        // Generate vertices for handle (border + fill, both squares)
        handle = new Coord3D[4];
        handle[0] = Coord3D(-handlesize/2.0, -handlesize/2.0, 0.0);
        handle[1] = Coord3D( handlesize/2.0, -handlesize/2.0, 0.0);
        handle[2] = Coord3D( handlesize/2.0,  handlesize/2.0, 0.0);
        handle[3] = Coord3D(-handlesize/2.0,  handlesize/2.0, 0.0);

        // Generate vertices for arrowhead (a triangle fan drawing a cone)
        arrowhead = new Coord3D[RES_ARROWHEAD + 1];
        arrowhead[0] = Coord3D(-nodesize / 2.0, 0.0, 0.0);
        float diff = M_PI / 20, t = 0;
        for (int i = 1; i < RES_ARROWHEAD; ++i, t += diff)
            arrowhead[i] = Coord3D(-nodesize / 2.0 - arrowheadsize,
                                   0.3 * arrowheadsize * sin(t),
                                   0.3 * arrowheadsize * cos(t));
        arrowhead[RES_ARROWHEAD] = Coord3D(-nodesize / 2.0 - arrowheadsize,
                                           0.3 * arrowheadsize * sin(0),
                                           0.3 * arrowheadsize * cos(0));

        // Generate quads for labels
        labels = new Coord3D[4 * textures.count];
        n = 0;
        for (size_t i = 0; i < textures.count; ++i)
        {
            labels[n++] = Coord3D(-textures.widths[i], -textures.heights[i], 0.0);
            labels[n++] = Coord3D( textures.widths[i], -textures.heights[i], 0.0);
            labels[n++] = Coord3D( textures.widths[i],  textures.heights[i], 0.0);
            labels[n++] = Coord3D(-textures.widths[i],  textures.heights[i], 0.0);
        }
        for (size_t i = 0; i < n; ++i)
            labels[i] *= pixelsize / 2.0;
    }
};

struct CameraView
{
    Coord3D rotation;    ///< Rotation of the camera around x, y and z axis (performed in that order)
    Coord3D translation; ///< Translation of the camera
    Coord3D world;       ///< The size of the box in which the graph lives
    float zoom;          ///< Zoom specifies by how much the view angle is narrowed. Larger numbers mean narrower angles.
    float pixelsize;

    CameraView()
        : rotation(Coord3D(0, 0, 0)), translation(Coord3D(0, 0, 0)), world(Coord3D(1000.0, 1000.0, 1000.0)),
          zoom(1.0), pixelsize(1)
    { }

    void viewport(size_t width, size_t height)
    {
        glViewport(0, 0, width, height);
        pixelsize = 1000.0 / (width > height ? height : width);
        world.x = width * pixelsize;
        world.y = height * pixelsize;
    }

    /**
     *  Implements "true" billboarding, by moving to @e pos and aligning
     *  the Z-axis to the vector between @e pos and the camera position.
     *  The Z-axis will not be facing the camera, but rather looking away
     *  from it.
     *
     *  @param pos The position of the billboard.
     */
    void billboard_spherical(const Coord3D &pos)
    {
        Coord3D rt, up, lk;
        GLfloat mm[16];

        glGetFloatv(GL_MODELVIEW_MATRIX, mm);
        lk.x = mm[0] * pos.x + mm[4] * pos.y + mm[8] * pos.z + mm[12];
        lk.y = mm[1] * pos.x + mm[5] * pos.y + mm[9] * pos.z + mm[13];
        lk.z = mm[2] * pos.x + mm[6] * pos.y + mm[10] * pos.z + mm[14];

        lk /= lk.size();
        rt = lk.cross(Coord3D(0, 1, 0));
        up = rt.cross(lk);
        GLfloat matrix[16] = {rt.x,	rt.y,	rt.z,	0,
                              up.x,	up.y,	up.z,	0,
                              -lk.x,	-lk.y,	-lk.z,	0,
                              0,        0,      0,	1};
        billboard_cylindrical(pos);
        glMultMatrixf(matrix);
    }

    /**
     *  Implements "fake" billboarding, by moving to @e pos and aligning
     *  the X and Y axes to those of the projection plane.
     *
     *  @param pos The position of the billboard.
     */
    void billboard_cylindrical(const Coord3D &pos)
    {
        glTranslatef(pos.x, pos.y, pos.z);
        glRotatef(-rotation.y, 0, 1, 0);
        glRotatef(-rotation.x, 1, 0, 0);
    }

    void applyTranslation()
    {
        float viewdepth = world.size() + 2 * pixelsize * 10;
        glTranslatef(0, 0, -500.0005 - 0.5 * viewdepth);
        glTranslatef(translation.x, translation.y, translation.z);
    }

    void applyFrustum()
    {
        float viewdepth = world.size() + 2 * pixelsize * 10;
        float f = zoom * (1000.0 + (viewdepth - world.z)) / 1000.0;
        glFrustum(-world.x / (2 * f), world.x / (2 * f), -world.y / (2 * f), world.y / (2 * f), 500, viewdepth + 500.001);
    }

    void applyPickMatrix(GLdouble x, GLdouble y, GLdouble fuzz)
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        // Viewport is always (0, 0, width, height)
        gluPickMatrix(x, viewport[3] - y, fuzz * pixelsize, fuzz * pixelsize, viewport);
    }

    void applyRotation()
    {
        glRotatef(rotation.x, 1, 0, 0);
        glRotatef(rotation.y, 0, 1, 0);
    }
};

struct CameraAnimation : public CameraView
{
    CameraView m_source, m_target;
    size_t m_animation;
    size_t m_animation_steps;
    bool m_resizing;

    CameraAnimation() : m_animation(0), m_animation_steps(0), m_resizing(false) {}

    void start_animation(size_t steps)
    {
        m_source = *this;
        m_animation_steps = steps;
        m_animation = 0;
        if (steps == 0)
            operator=(m_target);
    }

    void operator=(const CameraView& other)
    {
        rotation = other.rotation;
        translation = other.translation;
        zoom = other.zoom;
        world = other.world;
        pixelsize = other.pixelsize;
    }

    void interpolate_cam(float pos)
    {
        if (pos > 0.999)
        {
            rotation = m_target.rotation;
            translation = m_target.translation;
            zoom = m_target.zoom;
        }
        else
        {
            rotation = m_target.rotation * pos + m_source.rotation * (1.0 - pos);
            translation = m_target.translation * pos + m_source.translation * (1.0 - pos);
            zoom = m_target.zoom * pos + m_source.zoom * (1.0 - pos);
        }
    }

    void interpolate_world(float pos)
    {
        m_resizing = true;
        if (pos > 0.999)
        {
            world = m_target.world;
        }
        else
        {
            world.x = m_target.world.x * pos + m_source.world.x * (1.0 - pos);
            world.y = m_target.world.y * pos + m_source.world.y * (1.0 - pos);
            if (m_target.world.z > m_source.world.z)
                world.z = m_target.world.z * sin(M_PI_2 * pos) + m_source.world.z * (1.0 - sin(M_PI_2 * pos));
            else
                world.z = m_target.world.z * (1.0 - cos(M_PI_2 * pos)) + m_source.world.z * cos(M_PI_2 * pos);
        }
    }

    void animate()
    {
        if ((m_target.rotation != rotation || m_target.translation != translation || m_target.zoom != zoom) &&
            (m_target.world != world))
        {
            size_t halfway = m_animation_steps / 2;
            if (m_animation < halfway)
                interpolate_cam((float)(++m_animation) / halfway);
            if (m_animation == halfway)
            {
                m_animation_steps -= halfway;
                m_animation = 0;
            }
        }
        else
        if (m_target.world != world)
        {
            interpolate_world((float)(++m_animation) / m_animation_steps);
        }
        else
            interpolate_cam((float)(++m_animation) / m_animation_steps);
    }

    void viewport(size_t width, size_t height)
    {
        CameraView::viewport(width, height);
        m_target.world.x = world.x;
        m_target.world.y = world.y;
    }

    bool resizing()
    {
        bool temp = m_resizing;
        m_resizing = false;
        return temp;
    }

    void setZoom(float factor, size_t animation)
    {
        m_target.zoom = factor;
        start_animation(animation);
    }

    void setRotation(const Graph::Coord3D& rotation, size_t animation)
    {
        m_target.rotation = rotation;
        start_animation(animation);
    }

    void setTranslation(const Graph::Coord3D& translation, size_t animation)
    {
        m_target.translation = translation;
        start_animation(animation);
    }

    void setSize(const Graph::Coord3D& size, size_t animation)
    {
        m_target.world = size;
        start_animation(animation);
    }
};

//
// Some auxiliary functions that extend OpenGL
//

inline
bool gl2ps()
{
    return (gl2psEnable(GL2PS_BLEND) == GL2PS_SUCCESS);
}

inline
void glStartName(GLuint objectType, GLuint index=0)
{
    glLoadName(objectType);
    glPushName(index);
}

inline
void glEndName()
{
    glPopName();
    glLoadName(GLScene::so_none);
}

inline
GLuint glEyeZ(int eyeX, int eyeY)
{
    GLuint result;
    glReadPixels(eyeX, eyeY, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &result);
    return result;
}

//
// Functions that actually draw primitives by combining vertex data, textures and colors.
//

inline
void drawHandle(const VertexData& data, const Color3f& line, const Color3f& fill)
{
    glVertexPointer(3, GL_FLOAT, 0, data.handle);
    glColor3fv(fill);   glDrawArrays(GL_QUADS, 0, 4);
    glColor3fv(line);   glDrawArrays(GL_LINE_LOOP, 0, 4);
}

inline
void drawNode(const VertexData& data, const Color3f& line, const Color3f& fill)
{
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(2.0);
    gl2psLineWidth(0.25);
    glVertexPointer(3, GL_FLOAT, 0, data.node);
    glColor3fv(fill);   glDrawArrays(GL_TRIANGLE_STRIP, RES_NODE_SLICE - 1, RES_NODE_SLICE * RES_NODE_STACK * 2);
    glDepthMask(GL_FALSE);
    glColor3fv(line);   glDrawArrays(GL_LINE_LOOP, 0, RES_NODE_SLICE - 1);
    glDepthMask(GL_TRUE);
    glPopAttrib();
    gl2psLineWidth(0.25);
}

inline
void drawArrowHead(const VertexData& data)
{
    glVertexPointer(3, GL_FLOAT, 0, data.arrowhead);
    glDrawArrays(GL_TRIANGLE_FAN, 0, RES_ARROWHEAD + 1);
}

inline
void drawArc(const Coord3D* controlpoints)
{
    glDepthMask(GL_FALSE);

    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &controlpoints[0].x);
    glEnable(GL_MAP1_VERTEX_3);
    glMapGrid1f(RES_ARC, 0, 1);
    glEvalMesh1(GL_LINE, 0, RES_ARC);

    glDepthMask(GL_TRUE);
}

inline
void drawLabel(const VertexData& vertices, const TextureData& textures, size_t index)
{
    static GLfloat texCoords[] = { 0.0, 0.0,
                                   1.0, 0.0,
                                   1.0, 1.0,
                                   0.0, 1.0 };
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures.textures[index]);

    glVertexPointer(3, GL_FLOAT, 0, &vertices.labels[4 * index]);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glDrawArrays(GL_QUADS, 0, 4);

    glDisable(GL_TEXTURE_2D);
}

//
// GLScene private methods
//

void GLScene::renderEdge(size_t i)
{
    Graph::Edge edge = m_graph.edge(i);
    Coord3D ctrl[4];
    Coord3D &from = ctrl[0];
    Coord3D &to = ctrl[3];
    Coord3D via = m_graph.handle(i).pos;
    from = m_graph.node(edge.from).pos;
    to = m_graph.node(edge.to).pos;

    // Calculate control points from handle
    ctrl[1] = via * 1.33333 - (from + to) / 6.0;
    ctrl[2] = ctrl[1];

    // For self-loops, ctrl[1] and ctrl[2] need to lie apart, we'll spread
    // them in x-y direction.
    if (edge.from == edge.to)
    {
        Coord3D diff = ctrl[1] - ctrl[0];
        diff = diff.cross(Coord3D(0, 0, 1));
        diff = diff * ((via - from).size() / (diff.size() * 2.0));
        ctrl[1] = ctrl[1] + diff;
        ctrl[2] = ctrl[2] - diff;
    }

    glStartName(so_edge, i);
    glPushMatrix();

    glColor3f(edge.selected, 0.0, 0.0);

    // Draw the arc
    drawArc(ctrl);

    // Go to arrowhead position
    glTranslatef(to.x, to.y, to.z);

    // Rotate to match the orientation of the arc
    Coord3D vec = to - ctrl[2];
    // If ctrl[3] == ctrl[2], then something odd is going on. We'll just
    // make the executive decision not to draw the arrowhead then, as it
    // will just clutter the image.
    if (vec.size() > 0.0)
    {
        vec /= vec.size();
        Coord3D axis = Graph::Coord3D(1, 0, 0).cross(vec);
        float angle = acos(vec.x);
        glRotatef(angle * 180.0 / M_PI, axis.x, axis.y, axis.z);

        // Draw the arrow head
        drawArrowHead(*m_vertexdata);
    }

    glPopMatrix();
    glEndName();
}

void GLScene::renderNode(size_t i)
{
    Graph::NodeNode& node = m_graph.node(i);
    Color3f fill;
    Color3f line(node.color);

    if (node.locked)
        fill = Color3f(1.0 - 0.2 * node.selected, 0.8, 0.8);
    else
        fill = Color3f(1.0, 1.0 - 0.2 * node.selected, 1.0 - 0.2 * node.selected);

    glStartName(so_node, i);
    glPushMatrix();

    m_camera->billboard_spherical(node.pos);
    drawNode(*m_vertexdata, line, fill);

    glPopMatrix();
    glEndName();
}

void GLScene::renderLabel(size_t i)
{
    Graph::LabelNode& label = m_graph.label(i);

    if (gl2ps())
    {
        Coord3D pos = label.pos;
        pos.x -= m_camera->pixelsize * m_texturedata->widths[label.labelindex] / 2;
        pos.y -= m_camera->pixelsize * m_texturedata->heights[label.labelindex] / 2;
        glRasterPos3fv(pos);
        if (!m_graph.isTau(label.labelindex))
            gl2psText(m_graph.labelstring(label.labelindex).toUtf8(), "", 10);
        else
            gl2psText("t", "Symbol", 10);
    }
    else
    {
        glStartName(so_label, i);
        glPushMatrix();

        glColor3f(label.selected, 0.0, 0.0);
        m_camera->billboard_cylindrical(label.pos);
        drawLabel(*m_vertexdata, *m_texturedata, label.labelindex);

        glPopMatrix();
        glEndName();
    }
}

void GLScene::renderHandle(size_t i)
{
    Graph::Node& handle = m_graph.handle(i);
    if (handle.selected > 0.1)
    {
        Color3f line(2 * handle.selected - 1.0, 0.0, 0.0);
        Color3f fill(1.0, 1.0, 1.0);

        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_POLYGON_SMOOTH);
        glStartName(so_handle, i);
        glPushMatrix();

        m_camera->billboard_cylindrical(handle.pos);
        drawHandle(*m_vertexdata, line, fill);

        glPopMatrix();
        glEndName();
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);
    }
}

//
// GLScene public methods
//

GLScene::GLScene(Graph::Graph &g)
    : m_graph(g), m_drawlabels(true)
{
    m_camera = new CameraAnimation();
    m_texturedata = new TextureData;
    m_vertexdata = new VertexData;
}

GLScene::~GLScene()
{
    delete m_vertexdata;
    delete m_texturedata;
    delete m_camera;
}

void GLScene::init(const QColor& clear)
{
    // Set clear color to desired color
    glClearColor(clear.redF(), clear.greenF(), clear.blueF(), 0.0);
    // Enable anti-aliasing for all primitives
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    gl2psEnable(GL2PS_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl2psBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Get some fog in. This should probably be made custumizable for people
    // who don't like fog...
    GLfloat fog_color[4] = {clear.redF(), clear.greenF(), clear.blueF(), 0.0};
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_DENSITY, 1);
    glFogf(GL_FOG_START, 1000.0);
    glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
    glFogf(GL_FOG_END, 2500.0);
    glFogfv(GL_FOG_COLOR, fog_color);
    glEnable(GL_FOG);
    // Enable depth testing, so that we don't have to care too much about
    // rendering in the right order.
    glEnable(GL_DEPTH_TEST);
    // We'll be using a lot of glDrawArrays, and all of them use the vertex
    // array. We enable that feature once and leave it untouched.
    glEnableClientState(GL_VERTEX_ARRAY);
    // Load textures and shapes
    m_texturedata->generate(m_graph);
    m_vertexdata->generate(*m_texturedata, m_camera->pixelsize);
    // Initialise projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_camera->applyFrustum();
}

void GLScene::render()
{
    m_camera->animate();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    m_camera->applyTranslation();
    m_camera->applyRotation();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < m_graph.nodeCount(); ++i)
    {
        renderNode(i);
    }
    for (size_t i = 0; i < m_graph.edgeCount(); ++i)
    {
        renderEdge(i);
    }
    glDepthMask(GL_FALSE);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    for (size_t i = 0; i < m_graph.edgeCount(); ++i)
    {
        if (m_drawlabels)
            renderLabel(i);
        renderHandle(i);
    }
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDepthMask(GL_TRUE);
}

void GLScene::resize(size_t width, size_t height)
{
    m_camera->viewport(width, height);
    updateShapes();
}

void GLScene::updateLabels()
{
    m_texturedata->generate(m_graph);
}

void GLScene::updateShapes()
{
    m_vertexdata->generate(*m_texturedata, m_camera->pixelsize);
}

Coord3D GLScene::eyeToWorld(int x, int y, GLfloat z)
{
    GLint V[4];
    GLdouble P[16];
    GLdouble M[16];
    GLdouble wx, wy, wz;
    glGetDoublev(GL_PROJECTION_MATRIX, P);
    glGetDoublev(GL_MODELVIEW_MATRIX, M);
    glGetIntegerv(GL_VIEWPORT, V);
    if (z < 0)
        glReadPixels(x, V[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
    gluUnProject(x, V[3]-y, z, M, P, V, &wx, &wy, &wz);
    return Coord3D(wx, wy, wz);
}

Coord3D GLScene::worldToEye(const Coord3D& pos)
{
    GLint V[4];
    GLdouble P[16];
    GLdouble M[16];
    GLdouble ex, ey, ez;
    glGetDoublev(GL_PROJECTION_MATRIX, P);
    glGetDoublev(GL_MODELVIEW_MATRIX, M);
    glGetIntegerv(GL_VIEWPORT, V);
    gluProject(pos.x, pos.y, pos.z, M, P, V, &ex, &ey, &ez);
    return Coord3D(ex, V[3] - ey, ez);
}

Coord3D GLScene::size()
{
    return m_camera->world;
}

GLScene::Selection GLScene::select(int x, int y)
{
    Selection result;
    GLuint selectBuf[512];
    GLint  hits = 0;
    GLdouble fuzz = 2.0;

    glSelectBuffer(512, selectBuf);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(so_none);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_camera->applyPickMatrix(x, y, fuzz);
    m_camera->applyFrustum();
    render();
    hits = glRenderMode(GL_RENDER);

    GLuint bestZ = (GLuint)-1;
    result.selectionType = so_none;
    result.index = 0;

    for (size_t index = 0; hits > 0; --hits)
    {
        GLHitRecord& rec = *((GLHitRecord*)&selectBuf[index]);
        index += rec.stackSize + 3;
        if ((rec.stack[0] == (GLuint)result.selectionType && rec.minDepth < bestZ) ||
            (rec.stack[0] >  (GLuint)result.selectionType))
        {
            result.selectionType = static_cast<SelectableObject>(rec.stack[0]);
            result.index = rec.stack[1];
            bestZ = rec.minDepth;
        }
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    m_camera->applyFrustum();

    return result;
}

void GLScene::zoom(float factor)
{
    setZoom(m_camera->zoom * factor, 0);
}

void GLScene::rotate(Graph::Coord3D amount)
{
    amount += m_camera->rotation;
    while (amount.x > 180) amount.x -= 360;
    while (amount.y > 180) amount.y -= 360;
    while (amount.z > 180) amount.z -= 360;
    while (amount.x < -180) amount.x += 360;
    while (amount.y < -180) amount.y += 360;
    while (amount.z < -180) amount.z += 360;
    setRotation(amount, 0);
}

void GLScene::translate(const Graph::Coord3D& amount)
{
    setTranslation(m_camera->translation + amount);
}

bool GLScene::resizing()
{
    return m_camera->resizing();
}

void GLScene::setZoom(float factor, size_t animation)
{
    m_camera->setZoom(factor, animation);
}

void GLScene::setRotation(const Graph::Coord3D& rotation, size_t animation)
{
    m_camera->setRotation(rotation, animation);
}

void GLScene::setTranslation(const Graph::Coord3D& translation, size_t animation)
{
    m_camera->setTranslation(translation, animation);
}

void GLScene::setSize(const Graph::Coord3D& size, size_t animation)
{
    m_camera->setSize(size, animation);
}

void GLScene::renderVectorGraphics(const char* filename, GLint format)
{
    FILE* outfile = fopen(filename, "wb+");
    GLint viewport[4];
    GLint buffersize = 0, state = GL2PS_OVERFLOW;

    while( state == GL2PS_OVERFLOW ){
      buffersize += 1024*1024;
      gl2psBeginPage(filename,
                     "mCRL2 toolset",
                     viewport,
                     format,
                     GL2PS_BSP_SORT,
                     GL2PS_SILENT |
                       GL2PS_USE_CURRENT_VIEWPORT |
                       GL2PS_BEST_ROOT |
                       GL2PS_COMPRESS,
                     GL_RGBA,
                     0,
                     NULL,
                     0, 0, 0,
                     buffersize,
                     outfile,
                     filename
                  );
      render();
      state = gl2psEndPage();
    }
    fclose(outfile);
}
