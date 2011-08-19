// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include "enums.h"
#include "settings.h"

class Mediator;
class PrimitiveFactory;
class LTS;
class VisObjectFactory;
class State;
class Cluster;
class Transition;
class Vector3D;

class Visualizer: public Subscriber
{
  public:
    Visualizer(Mediator* owner,Settings* ss);
    ~Visualizer();

    void computeBoundsInfo(float& bcw,float& bch);
    float getHalfStructureHeight() const;
    void notify(SettingID s);
    void setLTS(LTS* l,bool compute_ratio);
    void notifyMarkStyleChanged();
    void notifyStatePositionsChanged();
    void notifyVisStyleChanged();

    void drawStates(bool simulating);
    void drawSimStates(std::vector<State*> historicStates, State*
                       currState, Transition* chosenTrans);

    void drawTransitions(bool draw_fp,bool draw_bp);
    void drawSimTransitions(bool draw_fp, bool draw_bp,
                            std::vector<Transition*> historicTrans, std::vector<Transition*>
                            posTrans, Transition* chosenTrans);

    void drawStructure();
    void sortClusters(Vector3D viewpoint);
    void exportToText(std::string filename);

  private:
    float cos_obt;
    float sin_obt;
    LTS* lts;
    VisObjectFactory* visObjectFactory;
    Mediator* mediator;
    PrimitiveFactory* primitiveFactory;
    Settings* settings;
    VisStyle visStyle;
    bool create_objects;
    bool update_colors;
    bool update_matrices;
    bool update_abs;

    void computeAbsPos();
    void computeStateAbsPos(Cluster* root,int rot);
    void computeSubtreeBounds(Cluster* root,float& boundWidth,
                              float& boundHeight);
    void drawBackPointer(State* startState, const RGB_Color& startColor, State* endState, const RGB_Color& endColor);
    void drawForwardPointer(State* startState,State* endState);
    void drawLoop(State* state);
    void drawStates(Cluster* root,bool simulating);
    void drawTransitions(Cluster* root,bool disp_fp,bool disp_bp);

    void traverseTree(bool co);
    void traverseTreeC(Cluster* root, bool topClosed, int rot);
    void traverseTreeT(Cluster* root, bool topClosed, int rot);
    void updateColors();
};

#endif
