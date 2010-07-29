// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mediator.h
/// \brief Header file for Mediator class

#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <string>
#include "enums.h"
#include "rgb_color.h"

class State;
class Cluster;
class Transition;

class Mediator {
  public:
    virtual ~Mediator() {}
    virtual void activateMarkRule(int index,bool activate) = 0;
    virtual void addMarkRule() = 0;
    virtual void editMarkRule(int index) = 0;
    virtual void exportToText(std::string filename) = 0;
    virtual std::string getVersionString() = 0;
    virtual RGB_Color getMarkRuleColor(int mr) = 0;
    virtual RGB_Color getNewRuleColour() = 0;
    virtual void setActionMark(int label,bool b) = 0;
    virtual void notifyRenderingFinished() = 0;
    virtual void notifyRenderingStarted() = 0;
    virtual void openFile(std::string fileName) = 0;
    virtual void removeMarkRule(int index) = 0;
    virtual void setMarkStyle(MarkStyle ms) = 0;
    virtual void setMatchStyle(MatchStyle ms) = 0;
    virtual void setMatchStyleClusters(MatchStyle ms) = 0;
    virtual void setRankStyle(RankStyle rs) = 0;
    virtual void setVisStyle(VisStyle vs) = 0;
    virtual void setFSMStyle(bool b) = 0;
    virtual void selectStateByID(const int id) = 0;
    virtual void selectCluster(const int rank, const int pos) = 0;
    virtual void deselect() = 0;
    virtual int getNumberOfObjects() = 0;
    virtual void zoomInBelow() = 0;
    virtual void zoomInAbove() = 0;
    virtual void zoomOutTillTop() = 0;
    virtual void zoomOut() = 0;

    virtual MatchStyle getMatchStyle() = 0;
    virtual MarkStyle getMarkStyle() = 0;
    virtual bool isMarked(State *s) = 0;
    virtual bool isMarked(Cluster *c) = 0;
    virtual bool isMarked(Transition *t) = 0;

    // Reports an error to the user through a message box.
    virtual void reportError(std::string const& error) = 0;


    // Loads a trace from path
    virtual void loadTrace(std::string const& path) = 0;


    // Simulation start.
    virtual void startSim() = 0;

    // Returns action label of transition with index i
    virtual std::string getActionLabel(const int i) const = 0;

    // Returns number of parameters
    virtual int getNumberOfParams() const = 0;

    // Returns the name of parameter with index i
    virtual std::string getParName(const int i) const = 0;

    // Returns the value of the parameter with index i in the state with index
    // j.
    virtual std::string getParValue(State* s, const int j) const = 0;


    virtual void generateBackTrace() = 0;

};

#endif
