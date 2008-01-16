// Author(s): Bas Ploeger and Carst Tankink
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
#include <list>
#include <map>
#include "mediator.h"
#include "state.h"
#include "transition.h"
#include "cluster.h"
#include "utils.h"
#include "simreader.h"

class LTS {
  public:

    LTS( Mediator* owner );
    ~LTS();
    void activateMarkRule(const int index,const bool activate);
    int  addLabel(std::string label);
    void addMarkRule(Utils::MarkRule* r,int index=-1);
    int  addParameter(std::string parname,std::string partype);
    void addParameterValue(int parindex,std::string parvalue);
    void addCluster(Cluster* c);
    void addClusterAndBelow(Cluster* c);
    void addState(State* s);
    void addTransition(Transition* t);
    void clearStatePositions();
    void clusterStates(Utils::RankStyle rs);
    void computeClusterInfo();
    void getActionLabels(std::vector<std::string> &ls) const;
    Cluster* getClusterAtRank(int r, int c) const;
    State* getInitialState() const;
    std::string getLabel(int labindex);
    int  getMatchAnyMarkRule() const;
    Utils::MarkRule* getMarkRule(const int index) const;
    int  getNumClusters() const;
    int  getNumClustersAtRank(int r) const;
    int  getNumDeadlocks();
    int  getNumMarkedStates() const;
    int  getNumMarkedTransitions() const;
    int  getNumRanks() const;
    int  getMaxRanks() const;  // Returns the maximal rank of the structure, 
                               // that is, the highest rank a cluster would have
                               // in a non-zoomed in structure
    int  getNumStates() const;
    int  getNumTransitions() const;
    int  getNumParameters() const;
    int  getNumParameterValues(int parindex) const;
    
    std::string getParameterName(int parindex) ;
    std::string getParameterType(int parindex) ;
    std::string getParameterValue(int parindex,int valindex);
    State*      selectStateByID(int id);
    void        selectCluster(const int rank, const int pos);
    void        deselect();
    void markAction(std::string label);
    void markClusters();
    void positionClusters(bool fsmstyle);
    void positionStates();
    void rankStates(Utils::RankStyle rs);
    void removeMarkRule(const int index);
    void replaceMarkRule(int index, Utils::MarkRule* mr);
    void setInitialState(State* s);
    void setMatchAnyMarkRule(int i);
    void unmarkAction(std::string label);
    
    void trim();

    int         getZoomLevel() const;
    void        setZoomLevel(const int level);
    // Zooming procedures.
    // Zooms into the structure starting from the initial cluster/state and upto
    // selectedCluster, if any. 
    LTS*        zoomIntoAbove(); 

    // Zooms into the structure starting from the selectedCluster, upto the
    // end of the structure. 
    LTS*        zoomIntoBelow();

    // Zooms out to the previous level.
    LTS*        zoomOut();
    void        setLastCluster(Cluster* c);
    void        setPreviousLevel(LTS* prev);
    LTS*        getPreviousLevel() const;
    void        fromAbove();

    // Method for simulation
    Simulation* getSimulation() const;


    // Methods getting information from parents while zooming in
    Cluster* getSelectedCluster() const;
    std::vector<Utils::MarkRule*> getMarkRules() const; 
    bool* getActionMarked(const int i) const;

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
    int matchAny;
    int zoomLevel;
    int deadlockCount;
    int transitionCount;
    int markedTransitionCount;
    int countMarkedTransitions();
    LTS*        previousLevel;
    State*     initialState;
    State*      selectedState;
    Cluster*    selectedCluster;
    Cluster*    lastCluster;
    std::list< State* > markedStates;
    std::list< State* > unmarkedStates;
    std::vector< Utils::MarkRule* > markRules;
    std::vector< std::vector< Cluster* > > clustersInRank;

    // State vector info
    std::vector< std::string > parameterNames;
    std::vector< std::string > parameterTypes;
    std::vector< std::vector< std::string > > valueTable;

    // Labels
    std::vector< std::string > labels;
    std::map< std::string,int > labels_inv;
    std::vector< bool* > label_marks;

    // Used for translating free variables from a trace to their instantiation 
    // in the LTS
    std::map<std::string, std::string> freeVars;

    // Methods
    void clearRanksAndClusters();
    void clusterTree(State* s,Cluster *c,bool cyclic);
    void processAddedMarkRule( Utils::MarkRule* r );
    void processRemovedMarkRule( Utils::MarkRule* r );

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
};

#endif // LTS_H
