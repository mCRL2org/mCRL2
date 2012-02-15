// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.h
/// \brief Header file of LTS class

#ifndef LTS_H
#define LTS_H

#include <string>
#include <vector>
#include <map>

#include "mcrl2/lts/lts_fsm.h"
#include "enums.h"

class LTS;
class Mediator;
class State;
class Cluster;
class Simulation;

class Cluster_iterator
{
  public:
    Cluster_iterator(LTS* l);
    virtual ~Cluster_iterator() {}
    void operator++();
    Cluster* operator*();
    virtual bool is_end();
  protected:
    int rank;
    size_t cluster;
    LTS* lts;
    bool is_valid();
    virtual void next();
};

class Reverse_cluster_iterator: public Cluster_iterator
{
  public:
    Reverse_cluster_iterator(LTS* l);
    ~Reverse_cluster_iterator() {}
    bool is_end();
  protected:
    void next();
};

class State_iterator
{
  public:
    State_iterator(LTS* l);
    ~State_iterator();
    void operator++();
    State* operator*();
    bool is_end();
  private:
    std::vector<State*>::iterator state_it;
    LTS* lts;
};

class LTS
{
  public:
    LTS(Mediator* owner);
    ~LTS();

    void addCluster(Cluster* c);
    void addClusterAndBelow(Cluster* c);
    void clearStatePositions();
    void clusterStates(RankStyle rs);
    void computeClusterInfo();
    void getActionLabels(std::vector<std::string> &ls) const;
    State* getInitialState() const;
    std::string getLabel(int labindex);
    Cluster_iterator getClusterIterator();
    Reverse_cluster_iterator getReverseClusterIterator();
    State_iterator getStateIterator();
    int getNumClusters() const;
    int getNumDeadlocks();
    int getNumLabels();
    int getNumRanks() const;
    int getNumStates() const;
    int getNumTransitions() const;

    size_t getNumParameters() const;
    std::vector<std::string> getParameterDomain(size_t parindex);
    std::string getParameterName(size_t parindex) ;
    size_t getStateParameterValue(State* state,size_t param);
    std::string getStateParameterValueStr(State* state,
                                          size_t param);
    std::set<std::string> getClusterParameterValues(Cluster* c, size_t param);

    State* selectStateByID(int id);
    Cluster* selectCluster(const int rank, const int pos);
    void deselect();
    void positionClusters(bool fsmstyle);
    void positionStates(StatePositioningStyle s);
    void rankStates(RankStyle rs);

    bool readFromFile(const std::string& filename);

    int getZoomLevel() const;
    void setZoomLevel(const int level);
    // Zooms into the structure starting from the initial cluster/state
    // and upto selectedCluster, if any.
    LTS* zoomIntoAbove();
    // Zooms into the structure starting from the selectedCluster, upto
    // the end of the structure.
    LTS* zoomIntoBelow();
    // Zooms out to the previous level.
    LTS* zoomOut();
    // Returns the maximal rank of the structure, that is, the highest
    // rank a cluster would have in a non-zoomed in structure
    int getMaxRanks() const;
    void setLastCluster(Cluster* c);
    void setPreviousLevel(LTS* prev);
    LTS* getPreviousLevel() const;
    void fromAbove();

    // Method for simulation
    Simulation* getSimulation() const;

    // Methods getting information from parents while zooming in
    Cluster* getSelectedCluster() const;

    // Loads a trace stored in location path and constructs a simulation
    // from it.
    void loadTrace(std::string const& path);

    void generateBackTrace();

  private:
    Mediator* mediator;
    Simulation* simulation;
    mcrl2::lts::lts_fsm_t mcrl2_lts;

    bool lastWasAbove;
    int zoomLevel;
    int deadlockCount;
    LTS* previousLevel;
    State* initialState;
    State* selectedState;
    Cluster* selectedCluster;
    Cluster* lastCluster;
    std::vector< State* > states;
    std::vector< std::vector< Cluster* > > clustersInRank;

    // Used for translating free variables from a trace to their
    // instantiation in the LTS
    std::map<std::string, std::string> freeVars;

    // Constructor for zooming
    LTS(Mediator* owner, LTS* parent, bool fromAbove);

    void clearRanksAndClusters();
    void clusterTree(State* s,Cluster* c,bool cyclic);

    void visit(State* s);

    friend class Cluster_iterator;
    friend class Reverse_cluster_iterator;
    friend class State_iterator;
};

#endif // LTS_H
