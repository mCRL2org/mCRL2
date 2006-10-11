#ifndef LTS_H
#define LTS_H

#include <iostream>
#include <vector>
#include <map>
#include "aterm2.h"
#include "liblowlevel.h"
#include "mediator.h"
#include "state.h"
#include "transition.h"
#include "cluster.h"
#include "utils.h"

//using namespace Utils;

class LTS
{
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	activateMarkRule( const int index, const bool activate );
    void	addMarkRule( Utils::MarkRule* r, int index=-1 );
    void	addState( State* s );
    void	addTransition( Transition* t );
    void	applyIterativeRanking();
    void	applyCyclicRanking();
    void	clusterComrades();
    void	computeClusterLabelInfo();
    void	getActionLabels( std::vector< ATerm> &ls ) const;
    State*	getInitialState() const;
    bool	getMatchAnyMarkRule() const;
    Utils::MarkRule*	getMarkRule( const int index ) const;
    int		getNumberOfClusters() const;
    int		getNumberOfDeadlocks();
    int		getNumberOfMarkedStates() const;
    int		getNumberOfMarkedTransitions() const;
    int		getNumberOfRanks() const;
    int		getNumberOfStates() const;
    int		getNumberOfTransitions() const;
    ATermList	getStateVectorSpec();
    void	markAction( std::string label );
    void	markClusters();
    void	mergeSuperiorClusters();
    void	positionClusters();
    void	positionStates();
//    void  	printStructure();
//    void	printClusterSizesPositions();
    void	removeMarkRule( const int index );
    void	replaceMarkRule( int index, Utils::MarkRule* mr );
    void	setActionLabels( ATermList labels );
    void	setInitialState( State* s );
    void	setMatchAnyMarkRule( bool b );
    void	setStateVectorSpec( ATermList spec );
    void	unmarkAction( std::string label );

  private:
    std::map< ATerm, bool* >		    actionLabelMarkings;
    std::vector< std::vector< Cluster* > >  clustersInRank;
    int				            deadlockCount;
    State*			            initialState;
    std::vector< State* >		    markedStates;
    int				            markedTransitionCount;
    std::vector< Utils::MarkRule* >	    markRules;
    bool			            matchAny;
    Mediator*			            mediator;
    std::vector< std::vector< State* > >    statesInRank;
    ATermList			            stateVectorSpec;
    std::vector< Transition* >	            transitions;
    std::vector< State* >		    unmarkedStates;
    float                                   tau; //Treshold for centering nodes

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
    void processAddedMarkRule( Utils::MarkRule* r );
    void processRemovedMarkRule( Utils::MarkRule* r );
    void updateMarksAll();  //Not implemented (?)
    void updateMarksAny();  //Not implemented (?)

    // Functions for positioning states based on Frank van Ham's heuristics
    std::vector< State* > edgeLengthBottomUp(); 
    //Phase 1: Processes states bottom-up, keeping edges as short as possible.
    //Pre:  statesInRank is correctly sorted by rank. 
    //Post: states in statesInRank are positioned bottom up, keeping edges as 
    //      short as possible, if information is available.
    //Ret:  states that could not be placed in this phase, bottom-up.
    
    std::vector< std::vector< State* > > edgeLengthTopDown( 
      std::vector< std::vector< State* > > ss );
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


};

#endif // LTS_H
