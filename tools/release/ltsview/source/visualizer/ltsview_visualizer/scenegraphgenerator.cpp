#include "scenegraphgenerator.h"
#include "cluster.h"
#include "mcrl2/utilities/logger.h"



void ConeConvertFunctor::matrixScale(float x, float y, float z){
    current_matrix.scale(x, y, z);
}

void ConeConvertFunctor::matrixTranslate(float x, float y, float z){
    current_matrix.translate(x, y, z);
}

void ConeConvertFunctor::matrixTranslate(const QVector3D& v){
    current_matrix.translate(v);
}

void ConeConvertFunctor::matrixRotate(float angle, float x, float y, float z){
    current_matrix.rotate(angle, x, y, z);
}


VisTree::VisTreeNode* ConeConvertFunctor::operator()(VisTree::VisTreeNode* parent, Cluster* cluster){
    if (parent == nullptr) current_matrix.setToIdentity(); else current_matrix = parent->shape.matrix;
    if (!cluster->hasDescendants()){
        VisTree::VisTreeNode* node = new VisTree::VisTreeNode();
        node->shape = noChildren(cluster, topClosed, rot);
        return node;
    }

    // move down and to the side (if applicable)
    float sideways = cluster->isCentered() ? 0 : cluster->getAncestor()->getBaseRadius();
    float branchtilt = cluster->isCentered() ? 0 : Settings::instance().branchTilt.value();
    // TODO: Check order
    matrixRotate(cluster->getPosition(), 0, 0, 1);
    matrixRotate(branchtilt, 0, 1, 0);
    matrixTranslate(sideways, 0, Settings::instance().clusterHeight.value());

    VisTree::VisTreeNode* node = new VisTree::VisTreeNode();
    VisTree::Shapes::TruncatedCone cone = VisTree::Shapes::TruncatedCone();
    cone.height = Settings::instance().clusterHeight.value();
    cone.fill_top = !cluster->isCentered() || parent == nullptr;
    cone.fill_bot = cluster->getNumDescendants() > 1;
    cone.matrix = current_matrix;
    cone.radius_top = cluster->getTopRadius();
    cone.radius_bot = cluster->getBaseRadius();
    node->shape = cone;
    std::cout << "Cone at: " << qvec4ToString(current_matrix.column(3)) << " with:" << 
    "\n\t- topclosed: " << std::to_string(cone.fill_top) << 
    "\n\t- botclosed: " << std::to_string(cone.fill_bot) <<
    "\n\t- topradius: " << std::to_string(cone.radius_top) <<
    "\n\t- botradius: " << std::to_string(cone.radius_bot) << std::endl;

    return node;
}

VisTree::Shape ConeConvertFunctor::noChildren(Cluster* cluster, bool topClosed, int rot){
    VisTree::Shapes::Sphere sphere = VisTree::Shapes::Sphere();
    sphere.radius = 1;
    // find shape matrix
    float r = cluster->getTopRadius();
    // move down and to the side (if applicable)
    float sideways = cluster->isCentered() ? 0 : cluster->getAncestor()->getBaseRadius();
    float branchtilt = cluster->isCentered() ? 0 : Settings::instance().branchTilt.value();
    // TODO: Check order
    matrixRotate(cluster->getPosition(), 0, 0, 1);
    matrixRotate(branchtilt, 0, 1, 0);
    matrixTranslate(sideways, 0, Settings::instance().clusterHeight.value());
    matrixScale(r, r, r);
    sphere.matrix = current_matrix;
    
    std::cout << "Sphere at: " << qvec4ToString(current_matrix.column(3)) << " with:" <<
    "\n\t- radius: 1\n\t- r: " << std::to_string(r) << std::endl;
    return sphere;
}

VisTree::VisTreeNode* TubeConvertFunctor::operator()(VisTree::VisTreeNode* parent, Cluster* cluster){
    std::cout << "TubeConvertFunctor: not implemented yet..." << std::endl;
    return nullptr;
}

ClusterChildIterator getClusterChildIterator(Cluster* cluster){
  return {cluster->descendants.begin(), cluster->descendants.end()};
}


std::vector<Cluster*>::iterator ClusterChildIterator::operator++(){ return ++it; }
Cluster* ClusterChildIterator::operator*() { return *it; }

SG* SceneGraphGenerator::generate(Mode mode){
    switch (mode){
        case Mode::TUBES:
            return generateTubes(m_clusterRoot);
            break;
        case Mode::CONES:
            return generateCones(m_clusterRoot);
            break;
        default:
            mCRL2log(mcrl2::log::warning) << "Invalid mode selected in ltsview scenegraphgenerator. Defaulting to tubes." << std::endl;
            return generateCones(m_clusterRoot);
    }
}




