#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <vector>
#include "mediator.h"
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
#include "utils.h"
#include "lts.h"
#include "primitivefactory.h"

// class for primitive comparison based on distance
class Distance_greater {
  private:
    Utils::Point3D viewpoint;
  public:
    explicit Distance_greater(const Utils::Point3D vp) : viewpoint(vp) {}
    bool operator()(const Cluster*,const Cluster*) const;
};

class Visualizer {
  private:
    float boundingCylH;
    float boundingCylW;
    float clusterHeight;
    float cos_ibt;
    float cos_obt;
    float sin_ibt;
    float sin_obt;
    LTS* lts;
    std::vector< Cluster* > clusters;
    Utils::MarkStyle markStyle;
    Mediator* mediator;
    PrimitiveFactory *primitiveFactory;
    static Utils::VisSettings	defaultVisSettings;
    Utils::VisSettings visSettings;
    Utils::VisStyle visStyle;
//    std::vector< float > sin_theta1_s;
//    std::vector< float > sin_theta2_s;
//    std::vector< float > cos_theta1_s;
//    std::vector< float > cos_theta2_s;
//    bool refreshStates;
//    bool refreshTransitions;
//    GLuint statesDisplayList;
//    GLuint transDisplayList;
//    Utils::RankStyle rankStyle;
    
    void clearDFSStates(State* root);
    void computeStateAbsPos(State* root,int rot,Utils::Point3D init);
    void computeSubtreeBounds(Cluster* root,float &boundWidth,
                              float &boundHeight);
    void drawBackPointer(State* startState,State* endState);
    void drawForwardPointer(State* startState,State* endState);
    void drawStates(Cluster* root,int rot);
    void drawStatesMark(Cluster* root,int rot);
    void drawTransitions(State* root,bool disp_fp,bool disp_bp);
    bool isMarked(Cluster* c);
    bool isMarked(State* s);
//    void drawSphereState();
//    void drawSubtreeA(Cluster* root,int rot);
//    void drawSubtreeAMark(Cluster* root,int rot);
//    void drawSubtreeT(Cluster* root,Utils::HSV_Color col,int rot);
//    void drawSubtreeTMark(Cluster* root,int rot);
//    void drawTube(float baserad,float toprad,Utils::RGB_Color basecol,
//        Utils::RGB_Color topcol,bool interpolate,Utils::Point3D b1,
//        Utils::Point3D b2,Utils::Point3D b3,Utils::Point3D &center);

    void fillClusters();
    void initClusterData(Cluster *root,bool topClosed,int rot);
    void updateClusterPrimitives();
    void updateClusterMatrices(Cluster *root,int rot);
  
  public:
    Visualizer(Mediator* owner);
    ~Visualizer();
    
    void computeBoundsInfo();
    float	getHalfStructureHeight() const;
    float	getBoundingCylinderHeight() const;
    float	getBoundingCylinderWidth() const;
    Utils::VisStyle	getVisStyle() const;
    Utils::VisSettings getVisSettings() const;
    Utils::VisSettings getDefaultVisSettings() const;
    void setLTS(LTS *l);
    void setMarkStyle(Utils::MarkStyle ms);
    void setVisSettings(Utils::VisSettings vs);
    void setVisStyle(Utils::VisStyle vs);

    void drawStates();
    void drawTransitions(bool disp_fp,bool disp_bp);
    void drawStructure();
    void sortClusters(Utils::Point3D viewpoint);
//    void	drawLTS(Utils::Point3D viewpoint);
//    Utils::RankStyle getRankStyle() const;
//    void setRankStyle(Utils::RankStyle rs);
};
#endif
