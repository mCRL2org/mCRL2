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

class LTS
{
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	addState( State* s );
    void	addTransition( Transition* t );
    void	addTransitionLabels( ATermList labels );
    void	applyIterativeRanking();
    void	applyCyclicRanking();
    void	clusterComrades();
    void	getClustersAtRank( int r, vector< Cluster* > &cs ) const;
    State*	getInitialState() const;
    int		getNumberOfClusters() const;
    int		getNumberOfRanks() const;
    int		getNumberOfStates() const;
    int		getNumberOfTransitions() const;
    ATermList	getStateVectorSpec();
    void	mergeSuperiorClusters();
    void  	printStructure();
    void	printClusterSizesPositions();
    void	setInitialState( State* s );
    void	setStateVectorSpec( ATermList spec );

  private:
    vector< vector< Cluster* > >  clustersInRank;
    State*			  initialState;
    Mediator*			  mediator;
    vector< State* >		  states;
    vector< vector< State* > >	  statesInRank;
    ATermList			  stateVectorSpec;
    vector< Transition* >	  transitions;
    ATermList			  transitionLabels;

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
};

#endif // LTS_H
