#ifndef LTS_H
#define LTS_H

#include <iostream>
#include <vector>
#include <set>
#include <map>
#include "aterm/aterm1.h"
#include "mediator.h"
#include "state.h"
#include "action.h"
#include "transition.h"
//#include "datatype.h"
#include "cluster.h"

class LTS
{
  public:
    LTS( Mediator* owner );
    ~LTS();
    void	addAction( Action* a );
    // void    addDataType( DataType* dt );
    void	addState( State* s );
    void	addTransition( Transition* t );
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
    vector< Action* >		  actions;
    vector< vector< Cluster* > >  clustersInRank;
    //vector< DataType* >		  dataTypes;
    State*			  initialState;
    Mediator*			  mediator;
    vector< State* >		  states;
    vector< vector< State* > >	  statesInRank;
    ATermList			  stateVectorSpec;
    vector< Transition* >	  transitions;

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
};

#endif // LTS_H
