// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file visualizer.h
/// \brief Header file for the visualizer

#ifndef VISUALIZER_H
#define VISUALIZER_H
#include <vector>
#include "settings.h"
#include "utils.h"

class Mediator;
class PrimitiveFactory;
class LTS;
class VisObjectFactory;
class State;
class Cluster;
class Transition;

class Visualizer: public Subscriber {
  private:
    float cos_obt;
    float sin_obt;
    LTS* lts;
    VisObjectFactory *visObjectFactory;
    Mediator* mediator;
    PrimitiveFactory *primitiveFactory;
    Settings* settings;
    Utils::VisStyle visStyle;
    bool create_objects;
    bool update_colors;
    bool update_matrices;
    bool update_abs;

    void computeAbsPos();
    void computeStateAbsPos(Cluster* root,int rot);
    void computeSubtreeBounds(Cluster* root,float &boundWidth,
                              float &boundHeight);
    void drawBackPointer(State* startState,State* endState);
    void drawForwardPointer(State* startState,State* endState);
    void drawLoop(State* state);
    void drawStates(Cluster* root,bool simulating);
    void drawTransitions(Cluster* root,bool disp_fp,bool disp_bp);

    void traverseTree(bool co);
    void traverseTreeC(Cluster *root, bool topClosed, int rot);
    void traverseTreeT(Cluster *root, bool topClosed, int rot);
    void updateColors();
    float compute_cone_scale_x(float phi,float r,float x);
    void computeForces(Cluster* root);
    void applyForces(Cluster* root, int rot);
    void resetForces(Cluster* root);
    void resetVelocities(Cluster* root);
    void forceDirectedInitPos(Cluster* root);

  public:
    Visualizer(Mediator* owner,Settings* ss);
    ~Visualizer();

    void computeBoundsInfo(float &bcw,float &bch);
    float getHalfStructureHeight() const;
    Utils::VisStyle getVisStyle() const;
    void notify(SettingID s);
    void setLTS(LTS *l,bool compute_ratio);
    void notifyMarkStyleChanged();
    void setVisStyle(Utils::VisStyle vs);

    void drawStates(bool simulating);
    void drawSimStates(std::vector<State*> historicStates, State* currState,
                       Transition* chosenTrans);

    void drawTransitions(bool draw_fp,bool draw_bp);
    void drawSimTransitions(bool draw_fp, bool draw_bp,
                            std::vector<Transition*> historicTrans,
                            std::vector<Transition*> posTrans,
                            Transition* chosenTrans);

    void drawStructure();
    void sortClusters(Utils::Point3D viewpoint);
    void forceDirectedInit();
    void forceDirectedStep();
    void resetStatePositions();
    void exportToText(std::string filename);
};

#endif
