// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.h
/// \brief Add your file description here.

#ifndef LTS_H
#define LTS_H
#include <vector>
#include <map>
#include "mediator.h"
#include "state.h"
#include "transition.h"
#include "cluster.h"
#include "utils.h"
#include "simreader.h"

class LTS;

class Cluster_iterator {
  public:
    Cluster_iterator(LTS *l,bool rev=false);
    ~Cluster_iterator();
    void operator++();
    Cluster* operator*();
    bool is_end();
  private:
    bool reverse;
    int rank;
    unsigned int cluster;
    LTS *lts;
    void next();
    bool is_ok();
};

class State_iterator {
  public:
    State_iterator(LTS *l);
    ~State_iterator();
    void operator++();
    State* operator*();
    bool is_end();
  private:
    std::vector<State*>::iterator state_it;
    LTS *lts;
};

class LTS {
  public:

    LTS( Mediator* owner );
    ~LTS();

    int  addLabel(std::string label);
    void addParameter(std::string parname,std::string partype,
        std::vector<std::string> &parvalues);
    void addCluster(Cluster* c);
    void addClusterAndBelow(Cluster* c);
    void addState(int sid,std::vector<int> &sv);
    void addTransition(int bs,int es,int l);
    void clearStatePositions();
    void clusterStates(Utils::RankStyle rs);
    void computeClusterInfo();
    void getActionLabels(std::vector<std::string> &ls) const;
    State* getInitialState() const;
    std::string getLabel(int labindex);
    int getLabelIndex(std::string label);
    Cluster_iterator getClusterIterator(bool reverse=false);
    State_iterator getStateIterator();
    int getNumClusters() const;
    int getNumDeadlocks();
    int getNumLabels();
    int getNumRanks() const;
    int getNumStates() const;
    int getNumTransitions() const;
    unsigned int getNumParameters() const;
    int getNumParameterValues(int parindex) const;
    int getMaxRanks() const;  // Returns the maximal rank of the structure, 
                               // that is, the highest rank a cluster would have
                               // in a non-zoomed in structure
    
    std::string getParameterName(int parindex) ;
    std::string getParameterType(int parindex) ;
    std::string getParameterValue(int parindex,int valindex);
    State* selectStateByID(int id);
    Cluster* selectCluster(const int rank, const int pos);
    void deselect();
    void positionClusters(bool fsmstyle);
    void positionStates();
    void rankStates(Utils::RankStyle rs);
    
    void trim();

    int getZoomLevel() const;
    void setZoomLevel(const int level);
    // Zooming procedures.
    // Zooms into the structure starting from the initial cluster/state and upto
    // selectedCluster, if any. 
    LTS* zoomIntoAbove(); 
    // Zooms into the structure starting from the selectedCluster, upto the
    // end of the structure. 
    LTS* zoomIntoBelow();
    // Zooms out to the previous level.
    LTS* zoomOut();
    void setLastCluster(Cluster* c);
    void setPreviousLevel(LTS* prev);
    LTS* getPreviousLevel() const;
    void fromAbove();

    // Method for simulation
    Simulation* getSimulation() const;

    // Methods getting information from parents while zooming in
    Cluster* getSelectedCluster() const;

    // Loads a trace stored in location path and constructs a simulation from
    // it.
    void loadTrace(std::string const& path);

    void generateBackTrace();
  private:
    // Constructor for zooming
    LTS(Mediator* owner, LTS* parent, bool fromAbove);

    // Variables
    Mediator* mediator;
    Simulation* simulation;
    bool lastWasAbove;
    int zoomLevel;
    int deadlockCount;
    int transitionCount;
    LTS* previousLevel;
    State* initialState;
    State* selectedState;
    Cluster* selectedCluster;
    Cluster* lastCluster;
    std::vector< State* > states;
    std::vector< std::vector< Cluster* > > clustersInRank;

    // State vector info
    std::vector< std::string > parameterNames;
    std::vector< std::string > parameterTypes;
    std::vector< std::vector< std::string > > valueTable;

    // Labels
    std::vector< std::string > labels;
    std::map< std::string,int > labels_inv;

    // Used for translating free variables from a trace to their instantiation 
    // in the LTS
    std::map<std::string, std::string> freeVars;

    // Methods
    void clearRanksAndClusters();
    void clusterTree(State* s,Cluster *c,bool cyclic);

    // Methods for positioning states based on Frank van Ham's heuristics
    void edgeLengthBottomUp(std::vector< State* > &undecided); 
    //Phase 1: Processes states bottom-up, keeping edges as short as possible.
    //Pre:  statesInRank is correctly sorted by rank. 
    //Post: states in statesInRank are positioned bottom up, keeping edges as 
    //      short as possible, if information is available.
    //Ret:  states that could not be placed in this phase, bottom-up.
    
    void edgeLengthTopDown(std::vector< State* > &ss);
    //Phase 2: Process states top-down, keeping edges as short as possible.
    //Pre:  ss is correctly sorted by rank, bottom-up.
    //Post: states in ss are positioned top-down, keeping edges as short as 
    //      possible.
    //Ret:  states that could not be placed by this phase, sorted top-down.

    void resolveClusterSlots();
    void visit(State* s);

    friend class Cluster_iterator;
    friend class State_iterator;
};

#endif // LTS_H
