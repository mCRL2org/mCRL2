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

class LTS;
class State;
class Cluster;

class Cluster_iterator
{
  public:
    Cluster_iterator(LTS* l);
    virtual ~Cluster_iterator() {}
    void operator++();
    void operator++(int) { ++*this; }
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
    void operator++(int) { ++*this; }
    State* operator*();
    bool is_end();
  private:
    std::vector<State*>::iterator state_it;
    LTS* lts;
};

class LTS
{
  public:
    LTS();
    ~LTS();

    State *state(int id) const { return states[id]; }
    Cluster *cluster(int rank, int position) const { return clustersInRank[rank][position]; }

    void addCluster(Cluster* cluster);
    void addClusterAndBelow(Cluster* cluster);
    void clearStatePositions();
    void clusterStates(bool cyclic);
    void computeClusterInfo();
    State* getInitialState() const;
    std::string getActionLabel(int labindex) const;
    Cluster_iterator getClusterIterator();
    Reverse_cluster_iterator getReverseClusterIterator();
    State_iterator getStateIterator();
    int getNumClusters() const;
    int getNumDeadlocks();
    int getNumActionLabels() const;
    int getNumRanks() const;
    int getNumStates() const;
    int getNumTransitions() const;

    bool hasStateInfo() const { return lts_hasStateInfo; }
    size_t getNumParameters() const;
    const static std::vector<std::string> v;
    const std::vector<std::string>& getParameterDomain(size_t parindex) const { return lts_stateElementValues[parindex]; }
    std::string getParameterName(size_t parindex) const;
    size_t getStateParameterValue(State* state,size_t param) const;
    std::string getStateParameterValueStr(State* state,
                                          size_t param) const;
    std::set<std::string> getClusterParameterValues(Cluster* c, size_t param) const;

    void positionClusters(bool fsmstyle);
    void positionStates(bool multiPass);
    void rankStates(bool cyclic);

    bool readFromFile(const std::string& filename);

    int getZoomLevel() const;
    void setZoomLevel(const int level);
    // Zooms into the structure starting from the initial cluster/state and upto target.
    LTS* zoomIntoAbove(Cluster *target);
    // Zooms into the structure starting from the target, upto the end of the structure.
    LTS* zoomIntoBelow(Cluster *target);
    // Zooms out to the previous level.
    LTS* zoomOut();
    // Returns the maximal rank of the structure, that is, the highest
    // rank a cluster would have in a non-zoomed in structure
    int getMaxRanks() const;
    void setLastCluster(Cluster* c);
    void setPreviousLevel(LTS* prev);
    LTS* getPreviousLevel() const;
    void fromAbove();

  private:
    bool lastWasAbove;
    int zoomLevel;
    int deadlockCount;
    LTS* previousLevel;
    Cluster* lastCluster;
    std::vector< State* > states;
    std::vector< std::vector< Cluster* > > clustersInRank;
    State* initialState;

    bool lts_hasStateInfo;
    size_t lts_numActionLabels;
    size_t lts_numParameters;
    size_t lts_numStateLabels;
    size_t lts_numStates;
    size_t lts_numTransitions;
    std::vector< std::vector< std::string > > lts_stateElementValues;
    std::vector< std::string > lts_parameterNames;
    std::vector< std::vector< size_t > > lts_stateLabels;
    std::vector< std::string > lts_actionLabels;

    // Used for translating free variables from a trace to their
    // instantiation in the LTS
    std::map<std::string, std::string> freeVars;

    // Constructor for zooming
    LTS(LTS* parent, Cluster *target, bool fromAbove);

    void clearRanksAndClusters();
    void clusterTree(State* v,Cluster* c,bool cyclic);

    friend class Cluster_iterator;
    friend class Reverse_cluster_iterator;
    friend class State_iterator;
};

#endif // LTS_H
