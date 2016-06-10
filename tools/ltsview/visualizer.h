// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <QList>
#include <QObject>

#include <string>
#include <vector>
#include "ltsmanager.h"
#include "markmanager.h"
#include "primitivefactory.h"
#include "settings.h"
#include "visobjectfactory.h"

class PrimitiveFactory;
class LTS;
class VisObjectFactory;
class State;
class Cluster;
class Transition;
class Vector3D;

class Visualizer: public QObject
{
  Q_OBJECT

  public:
    Visualizer(QObject *parent, Settings* settings_, LtsManager *ltsManager_, MarkManager* markManager_);

    void computeBoundsInfo(float& bcw,float& bch);
    float getHalfStructureHeight() const;

    void drawStates(bool simulating);
    void drawSimStates(QList<State*> historicStates, State*
                       currState, Transition* chosenTrans);

    void drawTransitions(bool draw_fp,bool draw_bp);
    void drawSimTransitions(bool draw_fp, bool draw_bp,
                            QList<Transition*> historicTrans, QList<Transition*>
                            posTrans, Transition* chosenTrans);

    void drawStructure();
    void sortClusters(const Vector3D& viewpoint);
    void exportToText(std::string filename);

  public slots:
    void setClusterHeight();
    void branchTiltChanged(int value);
    void dirtyObjects() { update_objects = true; emit dirtied(); }
    void dirtyMatrices() { update_matrices = true; emit dirtied(); }
    void dirtyPositions() { update_positions = true; emit dirtied(); }
    void dirtyColors() { update_colors = true; emit dirtied(); }
    void dirtyColorsMark() { if (markManager->markStyle() != NO_MARKS) dirtyColors(); }
    void dirtyColorsNoMark() { if (markManager->markStyle() == NO_MARKS) dirtyColors(); }

  signals:
    void dirtied();

  private:
    Settings* settings;
    LtsManager *ltsManager;
    MarkManager* markManager;
    VisObjectFactory visObjectFactory;
    PrimitiveFactory primitiveFactory;
    float cos_obt;
    float sin_obt;
    bool update_objects;
    bool update_matrices;
    bool update_colors;
    bool update_positions;

    void computeAbsPos();
    void computeStateAbsPos(Cluster* root,int rot);
    void computeSubtreeBounds(Cluster* root,float& bw,
                              float& bh);
    void drawBackPointer(State* startState, const QColor& startColor, State* endState, const QColor& endColor);
    void drawForwardPointer(State* startState,State* endState);
    void drawLoop(State* state);
    void drawStates(Cluster* root,bool simulating);
    void drawTransitions(Cluster* root,bool disp_fp,bool disp_bp);

    void traverseTree();
    void traverseTreeC(Cluster* root, bool topClosed, int rot);
    void traverseTreeT(Cluster* root, bool topClosed, int rot);
    void updateColors();
};

#endif
