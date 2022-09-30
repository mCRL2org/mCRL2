#ifndef SCENE_H
#define SCENE_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLExtraFunctions>

#include "glscene.h"
#include <vector>
#include "cluster.h"

#include "glvistree.h"
#include "glscenegraph.h"
#include "glmeshes.h"

#include <map>
#include <tuple>


namespace Primitives{ struct TriangleMesh; }


namespace GlLTSView{
    // Used in the shaders
    struct VertexData{
        int model_index;
        int vertex_index;
    };

    // Used in the scenegraph
    struct NodeData
    {
        int model_id;
        int data_id;
    };

    // A mesh is a resolution and a meshtype
    using Mesh = GlUtil::Meshes::MeshTypes::TriangleMesh;
    using MeshRes = std::pair<int, Mesh>; 

    struct SceneData{
      std::vector<NodeData> nodes = std::vector<NodeData>();
      std::vector<Mesh> meshes = std::vector<Mesh>();
      std::vector<GlUtil::Meshes::AABB> aabbs = std::vector<GlUtil::Meshes::AABB>();
      std::vector<QColor> colors = std::vector<QColor>();
      std::vector<QMatrix4x4> matrices = std::vector<QMatrix4x4>();
    };

    using SGNode = GlUtil::PointerTree::PointerTree<NodeData>;
    using LTSScene = GLScene<NodeData, SceneData>;

    class ConeDB{
        public:
            int getCone(GlUtil::Shapes::TruncatedCone* cone, int resolution, SceneData& sceneData);

        private:
            typedef std::tuple<qreal, qreal, qreal, int> key_t;
            std::map<key_t, int> cones = std::map<key_t, int>(); 
    };

    class SphereDB{
        
        public:
            int getSphere(GlUtil::Shapes::Sphere* sphere, int resolution, SceneData& sceneData);

        private:
            std::map<std::pair<qreal, int>, int> spheres = std::map<std::pair<qreal, int>, int>();
    };

    template < int sphereRes, int coneRes >
    struct SceneGraphFunctor{
        SceneData& sceneData;
        SphereDB sphereDB = SphereDB();
        ConeDB coneDB = ConeDB();
        
        SceneGraphFunctor(SceneData& sceneData) : sceneData(sceneData) {}

        SGNode* operator()(SGNode* parent,
                                    VisTree::VisTreeNode* cluster);
    };


    class Scene : public LTSScene, protected QOpenGLFunctions_4_3_Core{
    public:
        explicit Scene(QOpenGLWidget* glwidget, Cluster* root);
        Cluster *m_clusterRoot;
        VisTree::VisTreeNode *m_vistreeRoot;
        // LTSRenderer m_renderer;

        void initializeScene() override;

        void updateScene() override {};

        void renderScene() override;

        void resizeScene(std::size_t width, std::size_t height) override {};

        void rebuildScene() override;

    private:
        bool built = false;
        bool buffers_exist = false;
        int m_ssbo_count = 0;

        QOpenGLVertexArrayObject m_vao;  // vertex array object

        QOpenGLBuffer m_vbo;  // vertex buffer object contains all vertices as VertexData
        QOpenGLBuffer m_ibo;  // index buffer object contains all triangles

        QOpenGLBuffer m_vertex_ssbo; // shader storage buffer object contains all the mesh vertices
        QOpenGLBuffer m_normal_ssbo; // shader storage buffer object contains all the mesh vertex normals

        QOpenGLBuffer m_color_ssbo;  // shader storage buffer object contains all the model colors
        QOpenGLBuffer m_matrix_ssbo; // shader storage buffer object contains all the model matrices

        GLuint m_view_loc;
        GLuint m_proj_loc;
        GLuint m_alpha_loc;

        // helper functions
        void genBuffers();
        void fillBuffers();
        void bindBufferBases();
        bool reportOpenGLError();
        /// \brief Creates a single SSBO with given parameters and binds to the program associated with the scene
        void createBufferObject(void *data, int num_bytes, QOpenGLBuffer& buff, const char* name, QOpenGLBuffer::UsagePattern drawmode = QOpenGLBuffer::UsagePattern::StaticDraw );

        QOpenGLShaderProgram program;

        std::vector<VertexData> m_vertexData;
        std::vector<QVector3D> m_vertices;
        std::vector<QVector3D> m_normals;
        std::vector<QColor> m_colors;
        std::vector<GlUtil::Triangle> m_triangles;
    };
}
#endif