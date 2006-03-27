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

using namespace Utils;

class LTS
{
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	activateMarkRule( const int index, const bool activate );
    void	addMarkRule( MarkRule* r, int index=-1 );
    void	addState( State* s );
    void	addTransition( Transition* t );
    void	applyIterativeRanking();
    void	applyCyclicRanking();
    void	clusterComrades();
    void	computeClusterLabelInfo();
    void	getActionLabels( vector< ATerm> &ls ) const;
    void	getClustersAtRank( unsigned int r, vector< Cluster* > &cs ) const;
    State*	getInitialState() const;
    bool	getMatchAnyMarkRule() const;
    MarkRule*	getMarkRule( const int index ) const;
    int		getNumberOfClusters() const;
    int		getNumberOfDeadlocks();
    int		getNumberOfMarkedStates() const;
    int		getNumberOfMarkedTransitions() const;
    int		getNumberOfRanks() const;
    int		getNumberOfStates() const;
    int		getNumberOfTransitions() const;
    ATermList	getStateVectorSpec();
    void	markAction( string label );
    void	markClusters();
    void	mergeSuperiorClusters();
//    void  	printStructure();
//    void	printClusterSizesPositions();
    void	removeMarkRule( const int index );
    void	replaceMarkRule( int index, MarkRule* mr );
    void	setActionLabels( ATermList labels );
    void	setInitialState( State* s );
    void	setMatchAnyMarkRule( bool b );
    void	setStateVectorSpec( ATermList spec );
    void	unmarkAction( string label );

  private:
    map< ATerm, bool* >		  actionLabelMarkings;
    vector< vector< Cluster* > >  clustersInRank;
    int				  deadlockCount;
    State*			  initialState;
    vector< State* >		  markedStates;
    int				  markedTransitionCount;
    vector< MarkRule* >		  markRules;
    bool			  matchAny;
    Mediator*			  mediator;
    vector< vector< State* > >	  statesInRank;
    ATermList			  stateVectorSpec;
    vector< Transition* >	  transitions;
    vector< State* >		  unmarkedStates;

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
    void processAddedMarkRule( MarkRule* r );
    void processRemovedMarkRule( MarkRule* r );
    void updateMarksAll();
    void updateMarksAny();
};

#endif // LTS_H
