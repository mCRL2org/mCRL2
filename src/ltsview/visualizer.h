#ifndef VISUALIZER_H
#define VISUALIZER_H
#include <vector>
#include "mediator.h"
#include "utils.h"
#include "lts.h"
#include "primitivefactory.h"
#include "settings.h"
#include "visobjectfactory.h"

class Visualizer: public Subscriber {
  private:
    float cos_obt;
    float sin_obt;
    LTS* lts;
    VisObjectFactory *visObjectFactory;
    Utils::MarkStyle markStyle;
    Mediator* mediator;
    PrimitiveFactory *primitiveFactory;
    Settings* settings;
    Utils::VisStyle visStyle;
		bool create_objects;
		bool update_colors;
		bool update_matrices;

    void clearDFSStates(State* root);
    void computeStateAbsPos(State* root,int rot,Utils::Point3D init);
    void computeSubtreeBounds(Cluster* root,float &boundWidth,
                              float &boundHeight);
    void drawBackPointer(State* startState,State* endState);
    void drawForwardPointer(State* startState,State* endState);
    void drawStates(Cluster* root,int rot);
    void drawTransitions(State* root,bool disp_fp,bool disp_bp);
    bool isMarked(Cluster* c);
    bool isMarked(State* s);
    bool isMarked(Transition* s);

    void traverseTree(bool co);
    void traverseTreeC(Cluster *root,bool topClosed,int rot);
    void traverseTreeT(Cluster *root,int rot);
		void updateColors();
		float compute_cone_scale_x(float phi,float r,float x);
  
  public:
    Visualizer(Mediator* owner,Settings* ss);
    ~Visualizer();
    
    void computeBoundsInfo(float &bcw,float &bch);
    float	getHalfStructureHeight() const;
    Utils::VisStyle	getVisStyle() const;
    void notify(SettingID s);
    void setLTS(LTS *l);
    void setMarkStyle(Utils::MarkStyle ms);
    void setVisStyle(Utils::VisStyle vs);

    void drawStates();
    void drawTransitions(bool draw_fp,bool draw_bp);
    void drawStructure();
    void sortClusters(Utils::Point3D viewpoint);
};

#endif
