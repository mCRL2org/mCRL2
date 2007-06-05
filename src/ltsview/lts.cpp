#include "lts.h"
#include <algorithm>

using namespace Utils;
using namespace std;

LTS::LTS(Mediator* owner) {
  mediator = owner;
  initialState = NULL;
  matchAny = true;
  deadlockCount = -1;
  markedTransitionCount = 0;
  simulation = new Simulation();
  selectedState = NULL;
  selectedCluster = NULL;
}

LTS::~LTS()
{
  unsigned int i,r;
  for (i = 0; i < unmarkedStates.size(); ++i) {
    delete unmarkedStates[i];
  }
  unmarkedStates.clear();
  for (i = 0; i < markedStates.size(); ++i) {
    delete markedStates[i];
  }
  markedStates.clear();
  statesInRank.clear();
  initialState = NULL;
  
  for (i = 0; i < label_marks.size(); ++i) {
    delete label_marks[i];
  }
  label_marks.clear();
  
  for (i = 0; i < transitions.size(); ++i) {
    delete transitions[i];
  }
  transitions.clear();

  for (r = 0; r < clustersInRank.size(); ++r) {
    for (i = 0; i < clustersInRank[r].size(); ++i) {
      delete clustersInRank[r][i];
    }
  }
  clustersInRank.clear();

  for (i = 0; i < markRules.size(); ++i) {
    delete markRules[i];
  }
  markRules.clear();
 
  simulation->stop();
  delete simulation;
}

int LTS::addParameter(string parname,string partype) {
	parameterNames.push_back(parname);
	parameterTypes.push_back(partype);
	vector< string > v;
	valueTable.push_back(v);
	return parameterNames.size()-1;
}

void LTS::addParameterValue(int parindex,string parvalue) {
	valueTable[parindex].push_back(parvalue);
}

string LTS::getParameterName(int parindex) {
	return parameterNames[parindex];
}

string LTS::getParameterType(int parindex) {
	return parameterTypes[parindex];
}

string LTS::getParameterValue(int parindex,int valindex) {
	return valueTable[parindex][valindex];
}

void LTS::selectStateByID(int id) {
  // FIXME Naive implementation
  for(size_t i = 0; i < unmarkedStates.size(); ++i) {
    if ( unmarkedStates[i]->getID() == id) {
      unmarkedStates[i]->select();
      // For fast deselection
      selectedCluster = NULL;
      selectedState = unmarkedStates[i];

      // If we are simulating, see if this is a state we can select.
      if ((simulation != NULL) && (simulation->getStarted()))
      { 
        vector< Transition* > posTrans = simulation->getPosTrans();

        for (size_t i = 0; i < posTrans.size(); ++i)
        {
          if (posTrans[i]->getEndState()->getID() == selectedState->getID())
          {
            simulation->chooseTrans(i);
          }
        }
      }

      // Escape from function
      return;
    }
  }

  for(size_t i = 0; i < markedStates.size(); ++i) {
    if (markedStates[i]->getID() == id) {
      markedStates[i]->select();
      // For fast deselection
      selectedState = markedStates[i];

      if ((simulation != NULL) && (simulation->getStarted()))
      { 
        vector< Transition* > posTrans = simulation->getPosTrans();

        for (size_t i = 0; i < posTrans.size(); ++i)
        {
          if (posTrans[i]->getEndState()->getID() == selectedState->getID())
          {
            simulation->chooseTrans(i);
          }
        }
      }
    }
  }
}

void LTS::selectCluster(const int rank, const int pos)
{
  clustersInRank[rank][pos]->select();

  // For fast deselection.
  selectedState = NULL;
  selectedCluster = clustersInRank[rank][pos];
}

void LTS::deselect() {
  if (selectedState != NULL) {
    selectedState->deselect();
    selectedState = NULL;
  }

  if (selectedCluster != NULL)
  {
    selectedCluster->deselect();
    selectedCluster = NULL;
  }
}
int LTS::getNumParameters() const {
	return parameterNames.size();
}

int LTS::getNumParameterValues(int parindex) const {
	return valueTable[parindex].size();
}

string LTS::getLabel(int labindex) {
	return labels[labindex];
}

int LTS::addLabel(string label) {
	labels.push_back(label);
	label_marks.push_back(new bool(false));
	labels_inv[label] = labels.size()-1;
	return labels.size()-1;
}

void LTS::setInitialState(State* s) {
  initialState = s;
}

void LTS::addState( State* s )
{
  unmarkedStates.push_back( s );
}

void LTS::addTransition(Transition* t) {
  transitions.push_back(t);
	t->setMarkedPointer(label_marks[t->getLabel()]);
}

void LTS::getActionLabels(vector< string > &ls) const {
	ls = labels;
}

State* LTS::getInitialState() const {
  return initialState;
}

Cluster* LTS::getClusterAtRank(int r,int i) const {
	return clustersInRank[r][i];
}

int LTS::getNumRanks() const {
  return clustersInRank.size();
}

int LTS::getNumClusters() const {
  int result = 0;
  for (unsigned int i = 0; i < clustersInRank.size(); ++i) {
    result += clustersInRank[i].size();
	}
  return result;
}

int LTS::getNumClustersAtRank(int r) const {
	return clustersInRank[r].size();
}

int LTS::getNumDeadlocks() {
  if (deadlockCount == -1) {
    // a value of -1 indicates that we have to compute it
    deadlockCount = 0;
    for ( vector< State* >::iterator state_it = unmarkedStates.begin() ;
	  state_it != unmarkedStates.end() ; ++state_it )
    {
      if ( (**state_it).isDeadlock() )
	++deadlockCount;
    }
    for ( vector< State* >::iterator state_it = markedStates.begin() ;
	  state_it != markedStates.end() ; ++state_it )
    {
      if ( (**state_it).isDeadlock() )
	++deadlockCount;
    }
  }
  return deadlockCount;
}

int LTS::getNumMarkedStates() const {
  return markedStates.size();
}

int LTS::getNumMarkedTransitions() const {
  return markedTransitionCount;
}

int LTS::getNumStates() const {
  return (unmarkedStates.size() + markedStates.size());
}

int LTS::getNumTransitions() const {
  return transitions.size();
}

void LTS::applyIterativeRanking()
{
  clearRanksAndClusters();
  
  int currRank = 0;
  initialState->setRank( currRank );

  vector< State* > nextRank;
  nextRank.push_back( initialState );
  statesInRank.push_back( nextRank );
  
  while ( statesInRank[ currRank ].size() > 0 )
  {
    nextRank.clear();
    
    // iterate over the states in this rank
    vector< State* >::iterator it;
    for ( it  = statesInRank[ currRank ].begin() ; 
	  it != statesInRank[ currRank ].end()	 ; ++it )
    {
      State* begState = *it;
      
      // iterate over all out-transitions of begState
      vector< Transition* > outTrans;
      begState->getOutTransitions( outTrans );
      vector< Transition* >::iterator tit;
      for ( tit = outTrans.begin() ; tit != outTrans.end() ; ++tit )
      {
	(**tit).setBackpointer( false );
	State* endState = (**tit).getEndState();
	
	if ( endState->getRank() == -1 )
	{
	  endState->setRank( currRank+1 );
	  begState->addSubordinate( endState );
	  endState->addSuperior( begState );
	  nextRank.push_back( endState );
	}
	else if ( endState->getRank() == currRank+1 )
	{
	  begState->addSubordinate( endState );
	  endState->addSuperior( begState );
	}
	else if ( endState->getRank() == currRank )
	{
	  begState->addComrade( endState );
	  endState->addComrade( begState );
	}
	else // 0 <= endState->getRank() < currRank
	{
	  (**tit).setBackpointer( true );
	}
      }
    }
    statesInRank.push_back( nextRank );
      
    ++currRank;
  }
  // last element of statesInRank is an empty vector
  statesInRank.erase( statesInRank.end()-1 );
}

void LTS::applyCyclicRanking()
{
  clearRanksAndClusters();
  
  int currRank = 0;
  initialState->setRank( currRank );

  vector< State* > nextRank;
  nextRank.push_back( initialState );
  statesInRank.push_back( nextRank );
  
  while ( statesInRank[ currRank ].size() > 0 )
  {
    nextRank.clear();
    
    // iterate over the states in this rank
    vector< State* >::iterator it;
    for ( it  = statesInRank[ currRank ].begin() ; 
	  it != statesInRank[ currRank ].end()	 ; ++it )
    {
      State* curState = *it;
      
      // iterate over all in-transitions of curState
      vector< Transition* > trans;
      curState->getInTransitions( trans );
      vector< Transition* >::iterator transit;
      
      for ( transit = trans.begin() ; transit != trans.end() ; ++transit )
      {
	(**transit).setBackpointer( false );
	State* begState = (**transit).getBeginState();
	if ( begState->getRank() == -1 )
	{
	  begState->setRank( currRank+1 );
	  nextRank.push_back( begState );
	  begState->addSuperior( curState );
	  curState->addSubordinate( begState );
	}
	else if ( begState->getRank() == currRank+1 )
	{
	  begState->addSuperior( curState );
	  curState->addSubordinate( begState );
	}
	else if ( begState->getRank() == currRank )
	{
	  begState->addComrade( curState );
	  curState->addComrade( begState );
	}
      }

      // iterate over all out-transitions of curState
      
      curState->getOutTransitions( trans );
      for ( transit = trans.begin() ; transit != trans.end() ; ++transit )
      {
	State* endState = (**transit).getEndState();
	if ( endState->getRank() == -1 )
	{
	  endState->setRank( currRank+1 );
	  nextRank.push_back( endState );
	  endState->addSuperior( curState );
	  curState->addSubordinate( endState );
	}
	else if ( endState->getRank() == currRank+1 )
	{
	  endState->addSuperior( curState );
	  curState->addSubordinate( endState );
	}
	else if ( endState->getRank() == currRank )
	{
	  endState->addComrade( curState );
	  curState->addComrade( endState );
	}
      }
    }
    statesInRank.push_back( nextRank );
    ++currRank;
  }
  // last element of statesInRank is an empty vector
  statesInRank.erase( statesInRank.end()-1 );
}

void LTS::clearRanksAndClusters()
{
  vector< State* >::iterator it;
  for ( it = unmarkedStates.begin() ; it != unmarkedStates.end() ; ++it )
  {
    State* state = *it;
    state->setRank( -1 );
    state->setCluster( NULL );
    state->clearHierarchyInfo();
  }
  for ( it = markedStates.begin() ; it != markedStates.end() ; ++it )
  {
    State* state = *it;
    state->setRank( -1 );
    state->setCluster( NULL );
    state->clearHierarchyInfo();
  }

  for ( unsigned int r = 0 ; r < clustersInRank.size() ; ++r )
    for ( unsigned int c = 0 ; c < clustersInRank[r].size() ; ++c )
      delete clustersInRank[r][c];
  
  vector< vector< State* > > temp1;
  vector< vector< Cluster* > > temp2;
  statesInRank.swap( temp1 );
  clustersInRank.swap( temp2 );
}

void LTS::clusterComrades() {
  for (unsigned int r=0; r<statesInRank.size(); ++r) {
    vector< Cluster* > cluslist;
    vector< State* >::iterator stateit;
    for (stateit=statesInRank[r].begin(); stateit!=statesInRank[r].end();
        ++stateit) {
      if ((**stateit).getCluster() == NULL) {
        Cluster* c = new Cluster(r);
        //Give cluster a preliminary position in this rank.
        c->setPositionInRank(cluslist.size());
        cluslist.push_back(c);
        addComradesToCluster(c,*stateit);
      }
    }
    clustersInRank.push_back(cluslist);
  }
}

void LTS::addComradesToCluster(Cluster* c, State* s) {
  if (s->getCluster() == NULL) {
    c->addState(s);
    s->setCluster(c);
    set< State* > comrades;
    s->getComrades(comrades);
    set< State* >::iterator comit;
    for (comit=comrades.begin(); comit!=comrades.end(); ++comit) {
      addComradesToCluster(c,*comit);
    }
  }
}

void LTS::mergeSuperiorClusters() {
  State *s;
  // iterate over the ranks in reverse order (bottom-up)
  for (int r=clustersInRank.size()-1; r>0; --r) 
  {
    vector< Cluster* > *prevRank = &(clustersInRank[r-1]);
    // iterate over the clusters in this rank
    vector< Cluster* >::iterator clusit;
    for (clusit=clustersInRank[r].begin(); clusit!=clustersInRank[r].end();
        ++clusit) 
    {
      set< Cluster* > mergeSet;
      
      // iterate over the states in this cluster
      for (int i = 0; i < (**clusit).getNumStates(); ++i) 
      {
        s = (**clusit).getState(i);
        // set deadlock information
	(**clusit).setDeadlock((**clusit).hasDeadlock() || s->isDeadlock());
			       
  	// iterate over the superiors of this state
	set< State* > superiors;
	s->getSuperiors(superiors);
	
        set< State* >::iterator superit;
        for (superit=superiors.begin(); superit!=superiors.end(); ++superit) 
        {
	  // add the superior's cluster to the merge set
	  mergeSet.insert((**superit).getCluster());
	}
      }
      
      if (mergeSet.size() > 1) 
      {
        Cluster* c = new Cluster(r-1);
        // Give c a prelminary positionInRank
        c->setPositionInRank(prevRank->size());
        // iterate over the clusters in the mergeSet
        set< Cluster* >::iterator clusit1;
          
        for (clusit1=mergeSet.begin(); clusit1!=mergeSet.end(); ++clusit1) 
        {
          // add the cluster's states to c
          for (int i=0; i < (**clusit1).getNumStates(); ++i) 
          {
            s = (**clusit1).getState(i);
            c->addState(s);
            s->setCluster(c);
          }
          
        // delete the cluster
          prevRank->erase(std::find(prevRank->begin(),
                          prevRank->end(),*clusit1));
          delete *clusit1;
        }
        
        prevRank->push_back(c);
      }
    }
      
    // clusters on previous rank have been merged; compute hierarchy info
    for (clusit=clustersInRank[r].begin(); clusit!=clustersInRank[r].end(); 
        ++clusit) 
    {
      set< State* > superiors;
        
      (**clusit).getState(0)->getSuperiors(superiors);
      Cluster* ancestor = (**(superiors.begin())).getCluster();

      (**clusit).setAncestor(ancestor);
      ancestor->addDescendant(*clusit);
    }

    // This rank is stable, add rank location information to clusters.
    for (size_t i = 0; i < clustersInRank[r].size(); ++i)
    {
     clustersInRank[r][i]->setPositionInRank(i);
    }
  }
}

void LTS::computeClusterLabelInfo()
{
  for ( vector< Transition* >::iterator t_it = transitions.begin() ;
	t_it != transitions.end() ; ++t_it )
    (**t_it).getBeginState()->getCluster()->addActionLabel( (**t_it).getLabel() );
}

void LTS::positionClusters()
{
  // iterate over the ranks in reverse order (bottom-up)
  for ( vector< vector< Cluster* > >::reverse_iterator r_it = clustersInRank.rbegin()
	; r_it != clustersInRank.rend() ; ++r_it )
  {
    // iterate over the clusters in this rank
    for ( vector< Cluster* >::iterator c_it = r_it->begin() ;
	  c_it != r_it->end() ; ++c_it )
    {
      // compute the size of this cluster and the positions of its descendants
      (**c_it).computeSizeAndDescendantPositions();
    }
  }
  // position the initial state's cluster
  initialState->getCluster()->setPosition( -1.0f );
}

void LTS::positionStates()
{
  vector< State* > undecided;
  edgeLengthBottomUp(undecided);
  edgeLengthTopDown(undecided);
  resolveClusterSlots(undecided);
          
  /*
  
  for ( vector< vector< Cluster* > >::iterator r_it = clustersInRank.begin() ;
	r_it != clustersInRank.end() ; ++r_it )
  {
    for ( vector< Cluster* >::iterator c_it = r_it->begin() ;
	  c_it != r_it->end() ; ++c_it )
    {
      vector< State* > c_ss;
      (**c_it).getStates( c_ss );
      if ( c_ss.size() == 1 )
	c_ss[0]->setPosition( -1.0f );
      else
      {
	float pos = 0.0f;
	float delta = 360.0f / float(c_ss.size());
	for ( vector< State* >::iterator s_it = c_ss.begin() ;
	      s_it != c_ss.end() ; ++s_it )
	{
	  (**s_it).setPosition( pos );
	  pos += delta;
	}
      }
    }
  }*/
}
void LTS::resolveClusterSlots(vector< State* > &undecided) 
{
  //Resolves the slots of each cluster, positioning the states within each 
  //slots in such a way that they do not overlap.
  
  // First, we assign each state not yet positioned into a slot. This slot is
  // decided as follows: Consider a node n, with n in undecided
  // 1) Determine the cluster in which the node resides
  // 2) Add the state to a vector of undecided states per cluster.
  // 3) Determine the slots per cluster 
  for( vector< State * >::iterator undecided_it = undecided.begin();
       undecided_it != undecided.end(); ++undecided_it) {
    (*undecided_it)->getCluster()->addUndecidedState(*undecided_it);
  }


  for( vector< vector< Cluster* > >::iterator rank_it = clustersInRank.begin(); 
       rank_it != clustersInRank.end(); ++rank_it) 
  {
    for (vector< Cluster* >::iterator clus_it = rank_it->begin();
         clus_it != rank_it->end(); ++clus_it)
    {
      (*clus_it)->resolveSlots();
    }
  }



}
void LTS::edgeLengthBottomUp(vector< State* > &undecided)
{
  //Phase 1: Processes states bottom-up, keeping edges as short as possible.
  //Pre:  statesInRank is correctly sorted by rank. 
  //Post: states in statesInRank are positioned bottom up, keeping edges as 
  //      short as possible, if enough information is available.
  //Ret:  states that could not be placed in this phase, sorted bottom-up
  //
  // The details of this algorithm can be found in  Frank van Ham's master's
  // thesis, pp. 21-29
 
  // Iterate over the ranks in reverse order (bottom-up)

  vector< vector< State* > >::reverse_iterator rank_it;
  for( rank_it = statesInRank.rbegin();
       rank_it != statesInRank.rend(); ++rank_it) {
    // Iterate over the clusters in this rank
    for( vector< State* >::iterator state_it = rank_it->begin();
         state_it != rank_it->end(); ++state_it) {
      // Get the cluster belonging to this state
      Cluster* currCluster = (*state_it)->getCluster();


      // Compute the position of the states, based on number and position of 
      // descendants of their cluster.
      //
      if (currCluster->getNumStates() == 1) {
        (*state_it)->setPosition(-1.0f);
      }
      else {
        switch (currCluster->getNumDescendants()) {
          case 0:
          {
            // Case three: No descendant clusters.
            // Center the state.
            (*state_it)->setPosition(-1.0f);
            undecided.push_back(*state_it);
            break;
          }
          case 1:
          {
            // Case one: One descendant cluster.
            // We know the states in this cluster have a pseudo-correct position
            // (as determined in this pass), since we process the system bottom-up

            // Get the subordinate states. These are all in one cluster, and we 
            // can calculate the position of the current state directly from them.
            set< State* > subordinates;
            (*state_it)->getSubordinates( subordinates );
            
            // Calculate the sum of the vectors gained from all subordinate angles
            Vect vecSum = {0, 0};

            // Variable that keeps track of whether or not the children are all 
            // centered.
            bool allcentered = true;

            for(set< State* >::iterator sub_it = subordinates.begin();
                sub_it != subordinates.end() && allcentered; ++sub_it) {
              allcentered = allcentered && ((*sub_it)->getPosition() < -0.9f);
            }

            if ( allcentered ) {
              (*state_it)->setPosition(-1.0f);
              undecided.push_back(*state_it);
            }
            else {            
              for(set< State* >::iterator sub_it = subordinates.begin();
                sub_it != subordinates.end(); ++sub_it) {

                // Get the position of subordinate state 
                float sub_position = (*sub_it)->getPosition();
                float position_rad = deg_to_rad(sub_position);

                if (sub_position >= -0.9f) {
                  // This subordinated is not centered
                  allcentered = false;
                }

                // Convert to vectors and add.
                Vect subVec = ang_to_vec(position_rad);
                vecSum = vecSum + subVec;
              }

              if (vec_length(vecSum) < tau) {
                // vecSum is sufficiently small, center the state.
                (*state_it)->setPosition(-1.0f);
                
              }
              else {
                // Transform vecSum into an angle, assign it to state position.
                float pos = rad_to_deg(vec_to_ang(vecSum));

                if (pos < 0) {
                  pos = 360 + pos;
                }
                
                (*state_it)->setPosition(pos);
                int slot = currCluster->occupySlot(pos);
                (*state_it)->setSlot(slot);
              }
            }
            
            break;
          }
          default:
          {
            // Case two: More than one descendant cluster.
            // Iterate over subordinates, calculating sum of vectors.
            Vect vecSum = {0, 0};
            set< State* > subordinates;
            (*state_it)->getSubordinates( subordinates );

            for (set< State* >::iterator sub_it = subordinates.begin();
                 sub_it != subordinates.end(); ++sub_it) {

              float clusterPos = (*sub_it)->getCluster()->getPosition();
              
              if (clusterPos < -0.9f) {
                // Cluster is centered

                if ((*sub_it)->getPosition() >= -0.9f) {
                  float position_rad = deg_to_rad((*sub_it)->getPosition());
                  vecSum = vecSum + ang_to_vec(position_rad);
                }
              }
              else
              {
                float clusterPos_rad = deg_to_rad(clusterPos);
                float statePos = deg_to_rad((*sub_it)->getPosition() - 
                                            clusterPos);

                // Convert angles to Vectors, add
                Vect relPos = ang_to_vec(statePos) + ang_to_vec(clusterPos_rad);
                vecSum = vecSum + relPos;
              }
            }
            
            // Convert sum to angle, assign it to position.
            if (vec_length(vecSum) < tau) {
              // vecSum is sufficiently small, center node.
              (*state_it)->setPosition(-1.0f);
            }
            else {
              float position = rad_to_deg(vec_to_ang(vecSum));

              if (position < 0) {
                position = 360 + position;
              }

              (*state_it)->setPosition(position);
              int slot = currCluster->occupySlot(position);
              (*state_it)->setSlot(slot);
            }
          }
        }
      }
    }
  }
}

void LTS::edgeLengthTopDown(vector< State* > &ss)
{
  //Phase 2: Process states top-down, keeping edges as short as possible.
  //Pre:  ss is correctly sorted by rank, bottom-up.
  //Post: ss contains the states that could not be placed by this phase, sorted
  //top-down.
  
  //To keep the states that could not be placed in this pass.
  vector< State * > undecided; 

  // Iterate over the ranks in reverse order (bottom-up)
  // Iterate over the clusters in this rank
  for( vector< State* >::reverse_iterator state_it = ss.rbegin();
       state_it != ss.rend(); ++state_it) {
    // Get the cluster belonging to this state
    Cluster* currCluster = (*state_it)->getCluster();

    // We know there is only one ancestor cluster.
    // We know the states in the above cluster have pseudo-correct
    // positions, as determined in this and the previous pass, so we can
    // proceed placing the states based on this information.

    // Get the ancestor states. These are all in one cluster, and we
    // can calculate the position of the current state directly from them.

    set< State* > superiors;
    (*state_it)->getSuperiors( superiors );

    // Calculate the sum of the vectors gained from all subordinate angles
    Vect vecSum = {0, 0};
    bool allcentered = true;

    for(set< State* >::iterator sup_it = superiors.begin();
        sup_it != superiors.end() && allcentered; ++sup_it) {
      allcentered = allcentered && ((*sup_it)->getPosition() < -0.9f);
    }

    

    if (allcentered) {
      (*state_it)->setPosition(-1.0f);
      undecided.push_back(*state_it);
    }
    else {
      for(set< State* >::iterator sup_it = superiors.begin();
          sup_it != superiors.end(); ++sup_it) {
        // Get the position of subordinate state 
        float position_rad = deg_to_rad((*sup_it)->getPosition());

        float cluster_position = deg_to_rad(
          (*sup_it)->getCluster()->getPosition());

        // Convert to vectors and add.
        Vect subVec = ang_to_vec(position_rad);
        Vect clusVec = ang_to_vec(cluster_position);
        vecSum = vecSum + subVec + clusVec;
      }

      if (vec_length(vecSum) < tau) {

        // vecSum is sufficiently small, center the state.
        (*state_it)->setPosition(-1.0f);
      }
      else {
        // Transform vecSum into an angle, assign it to state position.
        float position = rad_to_deg(vec_to_ang(vecSum));
        
        if (position < 0) {
          position = 360 + position;
        }

        (*state_it)->setPosition(position);
        int slot = currCluster->occupySlot(position);
        (*state_it)->setSlot(slot);
      }
    }
  }
  
  ss.swap(undecided);
}

void LTS::addMarkRule(MarkRule* r,int index) {
  if (index == -1) {
    markRules.push_back(r);
	} else {
    markRules.insert(markRules.begin()+index,r);
	}
  if (r->isActivated) {
    processAddedMarkRule(r);
  }
}

void LTS::processAddedMarkRule(MarkRule* r) {
  if (matchAny) {
    vector< State* > newunmarked;
    State* s;
    for (vector< State* >::iterator s_it = unmarkedStates.begin();
				s_it != unmarkedStates.end(); ++s_it) {
      s = *s_it;
      if (r->valueSet[s->getParameterValue(r->paramIndex)]) {
				s->mark();
				s->getCluster()->markState();
				markedStates.push_back(s);
      }
      else {
				newunmarked.push_back(s);
      }
    }
    unmarkedStates.swap(newunmarked);
  }
  else {
    vector< State* > newmarked;
    State* s;
    for (vector< State* >::iterator s_it = markedStates.begin();
				s_it != markedStates.end(); ++s_it) {
      s = *s_it;
      if (!r->valueSet[s->getParameterValue(r->paramIndex)]) {
				s->unmark();
				s->getCluster()->unmarkState();
				unmarkedStates.push_back(s);
      }
      else {
				newmarked.push_back(s);
      }
    }
    markedStates.swap(newmarked);
  }
}

void LTS::activateMarkRule( const int index, const bool activate )
{
  markRules[ index ]->isActivated = activate;
  if ( activate )
    processAddedMarkRule( markRules[ index ] );
  else
    processRemovedMarkRule( markRules[ index ] );
}

void LTS::replaceMarkRule( int index, MarkRule* mr )
{
  removeMarkRule( index );
  addMarkRule( mr, index );
}

void LTS::removeMarkRule( const int index )
{
  vector< MarkRule* >::iterator rule_it = markRules.begin() + index;
  MarkRule* r = *rule_it;
  markRules.erase( rule_it );
  if ( r->isActivated )
  {
    processRemovedMarkRule( r );
  }
  delete r;
}

void LTS::processRemovedMarkRule( MarkRule* r )
{
  vector< MarkRule* > activeMarkRules;
  for ( vector< MarkRule* >::iterator mr_it = markRules.begin() ; mr_it !=
      markRules.end() ; ++mr_it )
  {
    if ( (**mr_it).isActivated ) activeMarkRules.push_back( *mr_it );
  }

  if ( matchAny )
  {
    vector< State* > newmarked;
    State* s;
    for ( vector< State* >::iterator s_it = markedStates.begin() ;
	  s_it != markedStates.end() ; ++s_it )
    {
      s = *s_it;
      if ( r->valueSet[ s->getParameterValue( r->paramIndex ) ] )
      {
	unsigned int i = 0;
	s->unmark();
	while ( i < activeMarkRules.size() && !s->isMarked() )
	{
	  MarkRule* r_i = activeMarkRules[i];
	  if ( r_i->valueSet[s->getParameterValue(r_i->paramIndex)] )
	  {
	    s->mark();
	  }
	  ++i;
	}
	if ( !s->isMarked() )
	{
	  s->getCluster()->unmarkState();
	  unmarkedStates.push_back( s );
	}
	else
	{
	  newmarked.push_back( s );
	}
      }
      else
      {
	newmarked.push_back( s );
      }
    }
    markedStates.swap( newmarked );
  }
  else
  {
    vector< State* > newunmarked;
    State* s;
    for ( vector< State* >::iterator s_it = unmarkedStates.begin() ;
	  s_it != unmarkedStates.end() ; ++s_it )
    {
      s = *s_it;
      if ( !r->valueSet[ s->getParameterValue( r->paramIndex ) ] )
      {
	unsigned int i = 0;
	s->mark();
	while ( i < activeMarkRules.size() && s->isMarked() )
	{
	  MarkRule* r_i = activeMarkRules[i];
	  if ( !r_i->valueSet[s->getParameterValue(r_i->paramIndex)] )
	  {
	    s->unmark();
	  }
	  ++i;
	}
	if ( s->isMarked() )
	{
	  s->getCluster()->markState();
	  markedStates.push_back( s );
	}
	else
	{
	  newunmarked.push_back( s );
	}
      }
      else
      {
	newunmarked.push_back( s );
      }
    }
    unmarkedStates.swap( newunmarked );
  }
}

MarkRule* LTS::getMarkRule( const int index ) const
{
  return markRules[ index ];
}

State* LTS::getMarkedState(const int index) const
{
  return markedStates[ index ];
}

bool LTS::getMatchAnyMarkRule() const
{
  return matchAny;
}

void LTS::setMatchAnyMarkRule(bool b) {
  if (matchAny == b) {
		return;
	}
  
  matchAny = b;
  
  vector< MarkRule* > activeMarkRules;
  for (vector< MarkRule* >::iterator mr_it = markRules.begin();
			mr_it != markRules.end(); ++mr_it) {
    if ((**mr_it).isActivated) {
			activeMarkRules.push_back(*mr_it);
		}
  }
  
  if (activeMarkRules.size() == 0) {
    markedStates.swap(unmarkedStates);
    if (matchAny) {
      for (unsigned int r = 0; r < clustersInRank.size(); ++r) {
				for (unsigned int c = 0; c < clustersInRank[r].size(); ++c) {
					clustersInRank[r][c]->unmarkState();
				}
			}
    }
    else {
      for (unsigned int r = 0; r < clustersInRank.size(); ++r) {
				for (unsigned int c = 0; c < clustersInRank[r].size(); ++c) {
					clustersInRank[r][c]->markState();
				}
			}
    }
  }
  else if (activeMarkRules.size() == 1) {
    return;
	}
  else {
    if (matchAny) {
      State* s;
      vector< State* > newunmarked;
      for (vector< State* >::iterator s_it = unmarkedStates.begin();
					s_it != unmarkedStates.end(); ++s_it) {
				s = *s_it;
				unsigned int i = 0;
				while (i < activeMarkRules.size() && !s->isMarked()) {
					MarkRule* markRule = activeMarkRules[i];
					if (markRule->valueSet[s->getParameterValue(markRule->paramIndex)])	{
						s->mark();
						s->getCluster()->markState();
						markedStates.push_back(s);
					}
					++i;
				}
				if (!s->isMarked()) {
					newunmarked.push_back(s);
				}
      }
      unmarkedStates.swap(newunmarked);
    }
    else {
      State* s;
      vector< State* > newmarked;
      for (vector< State* >::iterator s_it = markedStates.begin();
					s_it != markedStates.end(); ++s_it) {
				s = *s_it;
				unsigned int i = 0;
				while (i < activeMarkRules.size() && s->isMarked())	{
					MarkRule* markRule = activeMarkRules[i];
					if (!markRule->valueSet[s->getParameterValue(markRule->paramIndex)]) {
						s->unmark();
						s->getCluster()->unmarkState();
						unmarkedStates.push_back(s);
					}
					++i;
				}
				if (s->isMarked()) {
					newmarked.push_back(s);
				}
      }
      markedStates.swap(newmarked);
    }
  }
}

void LTS::markClusters() {
  // recompute the markings of clusters (after applying a different rank style)
  // process marked states
  State* s;
  for (vector< State* >::iterator s_it = markedStates.begin();
			s_it != markedStates.end(); ++s_it) {
    s = *s_it;
    if (s->isMarked()) {
      s->getCluster()->markState();
    }
  }
}

void LTS::markAction(string label) {
	int l = labels_inv[label];
  *(label_marks[l]) = true;

  markedTransitionCount = 0;
  for (vector< vector< Cluster* > >::iterator cs_it = clustersInRank.begin();
			cs_it != clustersInRank.end(); ++cs_it) {
    for (vector< Cluster* >::iterator c_it = cs_it->begin();
				c_it != cs_it->end(); ++c_it) {
      markedTransitionCount += (**c_it).markActionLabel(l);
		}
  }
}

void LTS::unmarkAction(string label) {
	int l = labels_inv[label];
  *(label_marks[l]) = false;

  markedTransitionCount = 0;
  for (vector< vector< Cluster* > >::iterator cs_it = clustersInRank.begin();
			cs_it != clustersInRank.end(); ++cs_it) {
    for (vector< Cluster* >::iterator c_it = cs_it->begin();
				c_it != cs_it->end(); ++c_it) {
			markedTransitionCount += (**c_it).unmarkActionLabel(l);
		}
  }
}

Simulation* LTS::getSimulation() const
{
  return simulation;
}
