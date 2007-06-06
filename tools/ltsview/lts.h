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

class LTS {
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	activateMarkRule(const int index,const bool activate);
    int		addLabel(std::string label);
    void	addMarkRule(Utils::MarkRule* r,int index=-1);
    int		addParameter(std::string parname,std::string partype);
    void	addParameterValue(int parindex,std::string parvalue);
    void	addState(State* s);
    void	addTransition(Transition* t);
    void	applyIterativeRanking();
    void	applyCyclicRanking();
    void	clusterComrades();
    void	computeClusterLabelInfo();
    void	getActionLabels(std::vector<std::string> &ls) const;
    Cluster*    getClusterAtRank(int r, int c) const;
    State*	getInitialState() const;
    std::string getLabel(int labindex);
    bool	getMatchAnyMarkRule() const;
    Utils::MarkRule*	getMarkRule(const int index) const;
    State*      getMarkedState(const int index) const;
    int		getNumClusters() const;
    int		getNumClustersAtRank(int r) const;
    int		getNumDeadlocks();
    int		getNumMarkedStates() const;
    int		getNumMarkedTransitions() const;
    int		getNumRanks() const;
    int		getNumStates() const;
    int		getNumTransitions() const;
    int		getNumParameters() const;
    int		getNumParameterValues(int parindex) const;
    std::string getParameterName(int parindex);
    std::string getParameterType(int parindex);
    std::string getParameterValue(int parindex,int valindex);
    void        selectStateByID(int id);
    void        selectCluster(const int rank, const int pos);
    void        deselect();
    void	markAction(std::string label);
    void	markClusters();
    void	mergeSuperiorClusters();
    void	positionClusters();
    void	positionStates();
//    void  	printStructure();
//    void	printClusterSizesPositions();
    void	removeMarkRule(const int index);
    void	replaceMarkRule(int index, Utils::MarkRule* mr);
    void	setInitialState(State* s);
    void	setMatchAnyMarkRule(bool b);
    void	unmarkAction(std::string label);

    // Method for simulation
    Simulation* getSimulation() const;

  private:
    // Variables
    std::vector< std::vector< Cluster* > >	clustersInRank;
    int	deadlockCount;
    State*	initialState;
    State*      selectedState;
    Cluster*    selectedCluster;
    std::vector< State* >	markedStates;
    int	markedTransitionCount;
    std::vector< Utils::MarkRule* >	markRules;
    bool	matchAny;
    Mediator*	mediator;
    std::vector< std::vector< State* > >	statesInRank;
    std::vector< Transition* >	transitions;
    std::vector< State* >	unmarkedStates;
    Simulation*                 simulation;

    // State vector info
    std::vector< std::string > parameterNames;
    std::vector< std::string > parameterTypes;
    std::vector< std::vector< std::string > > valueTable;

    // Labels
    std::vector< std::string > labels;
    std::map< std::string,int > labels_inv;
    std::vector< bool* > label_marks;

    // Methods
    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
    void processAddedMarkRule( Utils::MarkRule* r );
    void processRemovedMarkRule( Utils::MarkRule* r );
    void updateMarksAll();  //Not implemented (?)
    void updateMarksAny();  //Not implemented (?)


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

    void nodeDistanceBottomUp( std::vector < std::vector< State* > > ss );
    //Phase 3: Process states bottom-up, maximizing node distance
    //Pre:  ss is correctly sorted by rank.
    //Post: states in ss are positioned bottom-up, maximizing node distance per
    //      rank.
    void resolveClusterSlots(std::vector< State* > &undecided);
};

#endif // LTS_H
