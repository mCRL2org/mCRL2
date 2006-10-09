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
    std::map< ATerm, bool* >		  actionLabelMarkings;
    std::vector< std::vector< Cluster* > >  clustersInRank;
    int				  deadlockCount;
    State*			  initialState;
    std::vector< State* >		  markedStates;
    int				  markedTransitionCount;
    std::vector< Utils::MarkRule* >		  markRules;
    bool			  matchAny;
    Mediator*			  mediator;
    std::vector< std::vector< State* > >	  statesInRank;
    ATermList			  stateVectorSpec;
    std::vector< Transition* >	  transitions;
    std::vector< State* >		  unmarkedStates;

    void addComradesToCluster( Cluster* c, State* s );
    void clearRanksAndClusters();
    void processAddedMarkRule( Utils::MarkRule* r );
    void processRemovedMarkRule( Utils::MarkRule* r );
    void updateMarksAll();
    void updateMarksAny();
};

#endif // LTS_H
