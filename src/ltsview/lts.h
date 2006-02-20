#ifndef LTS_H
#define LTS_H

#include <iostream>
#include <vector>
#include <map>
#include "aterm/aterm2.h"
#include "liblowlevel.h"
#include "mediator.h"
#include "state.h"
#include "transition.h"
#include "cluster.h"
#include "utils.h"

using namespace Utils;

class LTS
{
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	addMarkRule( MarkRule* r );
    void	addState( State* s );
    void	addTransition( Transition* t );
    void	addTransitionLabels( ATermList labels );
    void	applyIterativeRanking();
    void	applyCyclicRanking();
    void	clusterComrades();
    void	getClustersAtRank( unsigned int r, vector< Cluster* > &cs ) const;
    State*	getInitialState() const;
    bool	getMatchAnyMarkRule() const;
    MarkRule*	getMarkRule( const int index ) const;
    int		getNumberOfClusters() const;
    int		getNumberOfDeadlocks();
    int		getNumberOfMarkedStates() const;
    int		getNumberOfRanks() const;
    int		getNumberOfStates() const;
    int		getNumberOfTransitions() const;
    ATermList	getStateVectorSpec();
    void	markStates();
    void	markTransitions();
    void	mergeSuperiorClusters();
    void  	printStructure();
    void	printClusterSizesPositions();
    void	removeMarkRule( const int index );
    void	replaceMarkRule( int index, MarkRule* mr );
    void	setInitialState( State* s );
    void	setMatchAnyMarkRule( bool b );
    void	setStateVectorSpec( ATermList spec );

  private:
    vector< vector< Cluster* > >  clustersInRank;
    int				  deadlockCount;
    State*			  initialState;
    int				  markedStatesCount;
    vector< MarkRule* >		  markRules;
    bool			  matchAny;
    Mediator*			  mediator;
    vector< State* >		  states;
    vector< vector< State* > >	  statesInRank;
    ATermList			  stateVectorSpec;
    vector< Transition* >	  transitions;
    ATermList			  transitionLabels;

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
    void updateMarksAll();
    void updateMarksAny();
};

#endif // LTS_H
