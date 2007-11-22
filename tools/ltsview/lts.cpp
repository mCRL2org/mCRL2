// Author(s): Bas Ploeger and Carst Tankink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.cpp
/// \brief Add your file description here.

#include "lts.h"
#include "mcrl2/libtrace.h"
#include "libprint.h"
#include "libstruct.h"
#include <algorithm>

using namespace Utils;
using namespace std;

LTS::LTS(Mediator* owner) {
  mediator = owner;
  initialState = NULL;
  matchAny = 0;
  deadlockCount = -1;
  transitionCount = 0;
  markedTransitionCount = 0;
  simulation = new Simulation();
  selectedState = NULL;
  selectedCluster = NULL;
  lastCluster = NULL;
  previousLevel = NULL;
  lastWasAbove = false;
  zoomLevel = 0;
}

LTS::LTS(Mediator* owner, LTS* parent, bool fromAbove)
{
  // Copied/initial information
  lastWasAbove = fromAbove;
  previousLevel  = parent;
  selectedState = NULL;
  zoomLevel = previousLevel->getZoomLevel() + 1;
  selectedCluster = previousLevel->getSelectedCluster();
  markRules = previousLevel->getMarkRules();
  matchAny = previousLevel->getMatchAnyMarkRule();
  mediator = owner;

  // simulation = new Simulation();
  simulation = previousLevel->getSimulation();

  for (int i = 0; i < previousLevel->getNumParameters(); ++i)
  {
    parameterNames.push_back(previousLevel->getParameterName(i));
    parameterTypes.push_back(previousLevel->getParameterType(i));
    
    vector< string > values;

    for (int j = 0; j < previousLevel->getNumParameterValues(i); ++j)
    {
      values.push_back(previousLevel->getParameterValue(i,j));
    }

    valueTable.push_back(values);
  }
  
  previousLevel->getActionLabels(labels);

  for(size_t i = 0; i < labels.size(); ++i)
  {
    pair<string, int> invLabel(labels[i], i);
    labels_inv.insert(invLabel);

    label_marks.push_back(previousLevel->getActionMarked(i));
  }


  if (lastWasAbove)
  {
    initialState = previousLevel->getInitialState();

    lastCluster = selectedCluster;

    Cluster* child = NULL; 
    Cluster* parent = selectedCluster;

    do
    {
      for (int i = 0; i < parent->getNumDescendants(); ++i)
      {
        if ( child == NULL || parent->getDescendant(i) != child)
        {
          parent->severDescendant(i);
        }
      }

      addCluster(parent);
      child = parent;
      parent = child->getAncestor();
    } while (child != initialState->getCluster());

  }
  else
  {
    initialState = selectedCluster->getState(0);
    addClusterAndBelow(selectedCluster);
  }

  markedTransitionCount = countMarkedTransitions();
}

LTS::~LTS()
{
  if (previousLevel == NULL)
  {
    // This LTS is the top level LTS, so delete all its contents.
    unsigned int i,r;
    list< State* >::iterator li;
    for (li = unmarkedStates.begin(); li != unmarkedStates.end(); ++li) {
      delete *li;
    }
    unmarkedStates.clear();
    for (li = markedStates.begin(); li != markedStates.end(); ++li) {
      delete *li;
    }
    markedStates.clear();
    initialState = NULL;
    
    for (i = 0; i < label_marks.size(); ++i) {
      delete label_marks[i];
    }
    label_marks.clear();
    
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
  else
  {
    unmarkedStates.clear();
    markedStates.clear();
    label_marks.clear();
    clustersInRank.clear();
    markRules.clear();
  }
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

State* LTS::selectStateByID(int id) {
  list< State* >::iterator li;
  State *s;
  for (li = unmarkedStates.begin(); li != unmarkedStates.end(); ++li) {
    s = *li;
    if (s->getID() == id) {
      s->select();
      // For fast deselection
      selectedCluster = NULL;
      selectedState = s;

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
      return selectedState;
    }
  }

  for (li = markedStates.begin(); li != markedStates.end(); ++li) {
    s = *li;
    if (s->getID() == id) {
      s->select();
      // For fast deselection
      selectedState = s;
      selectedCluster = NULL;
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
  return selectedState;
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
    simulation->setInitialState(initialState);
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

int LTS::countMarkedTransitions() 
{
  int result = 0;
  
  for(vector< vector< Cluster* > >::iterator it = clustersInRank.begin();
      it != clustersInRank.end(); ++it)
  {
    for(vector<Cluster*>::iterator it1 = it->begin(); it1 != it->end(); ++it1)
    {
      if (*it1 != NULL)
      {
        result += (*it1)->getMarkedTransitionCount();
      }
    }
  }

  return result;

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
  simulation->setInitialState(s);
}

void LTS::addCluster(Cluster* c)
{
  unsigned int rank = c->getRank();
  unsigned int pos = c->getPositionInRank();

  // Check to see if there is already a rank for this cluster
  if (clustersInRank.size() <= rank)
  {
    clustersInRank.resize(rank + 1);
  }
  
  if(clustersInRank[rank].size() <= pos)
  {
    clustersInRank[rank].resize(pos + 1);
  }

  clustersInRank[rank][pos] = c;

  for (int i = 0; i < c->getNumStates(); ++i)
  {
    State* s = c->getState(i);

    if (s->isMarked())
    {
      markedStates.push_back(s);
    }
    else
    {
      unmarkedStates.push_back(s);
    }

    s->setZoomLevel(zoomLevel);
  }
}

void LTS::addClusterAndBelow(Cluster* c) 
{
  if (c != NULL)
  {
    addCluster(c);

    for (int i = 0; i < c->getNumDescendants(); ++i)
    {
      addClusterAndBelow(c->getDescendant(i));
    }
  }
}


void LTS::addState( State* s )
{
  unmarkedStates.push_back( s );
}

void LTS::addTransition(Transition* t) {
  ++transitionCount;
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
  for (size_t i = 0; i < clustersInRank.size(); ++i) {
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
    list< State* >::iterator state_it;
    for (state_it  = unmarkedStates.begin();
         state_it != unmarkedStates.end(); ++state_it) {
      if ((**state_it).isDeadlock()) {
        ++deadlockCount;
      }
    }
    for (state_it  = markedStates.begin();
         state_it != markedStates.end(); ++state_it) {
      if ((**state_it).isDeadlock()) {
        ++deadlockCount;
      }
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
  return transitionCount;
}

void LTS::clearRanksAndClusters() {
  list< State* >::iterator it;
  for (it = unmarkedStates.begin(); it != unmarkedStates.end(); ++it) {
    (*it)->setRank(-1);
    (*it)->setCluster(NULL);
  }
  for (it = markedStates.begin(); it != markedStates.end(); ++it) {
    (*it)->setRank(-1);
    (*it)->setCluster(NULL);
  }

  for (unsigned int r = 0; r < clustersInRank.size(); ++r) {
    for (unsigned int c = 0; c < clustersInRank[r].size(); ++c) {
      delete clustersInRank[r][c];
    }
  }
  
  vector< vector< Cluster* > > temp2;
  clustersInRank.swap(temp2);
}

void LTS::applyRanking(Utils::RankStyle rs) {
  clearRanksAndClusters();
  
  int rankNumber = 0;

  vector< Cluster* > cluslist;
  vector< State* > nextRank,currRank;
  currRank.push_back(initialState);
  initialState->setRank(rankNumber);

  int t;
  vector< State* >::iterator it;
  State *cs,*ss;
  while (currRank.size() > 0) {
    cluslist.clear();
    nextRank.clear();
    // iterate over the states in this rank
    for (it  = currRank.begin(); it != currRank.end(); ++it) {
      cs = *it;
      if (rs == CYCLIC) {
        // iterate over all in-transitions of cs
        for (t = 0; t < cs->getNumInTransitions(); ++t) {
          ss = cs->getInTransition(t)->getBeginState();
          if (ss->getRank() == -1) {
            ss->setRank(rankNumber+1);
            nextRank.push_back(ss);
          }
        }
      }
      // iterate over all out-transitions of cs
      for (t = 0; t < cs->getNumOutTransitions(); ++t) {
        ss = cs->getOutTransition(t)->getEndState();
        if (ss->getRank() == -1) {
          ss->setRank(rankNumber+1);
          nextRank.push_back(ss);
        }
      }
      if (cs->getCluster() == NULL) {
        Cluster* c = new Cluster(rankNumber);
        //Give cluster a preliminary position in this rank.
        c->setPositionInRank(cluslist.size());
        cluslist.push_back(c);
        addComradesToCluster(c,cs);
      }
    }
    currRank.swap(nextRank);
    clustersInRank.push_back(cluslist);
    ++rankNumber;
  }
}

void LTS::addComradesToCluster(Cluster* c, State* s) {
  if (s->getCluster() == NULL) {
    c->addState(s);
    s->setCluster(c);
    for (int t = 0; t < s->getNumInTransitions(); ++t) {
      if (s->getInTransition(t)->getBeginState()->getRank() == s->getRank()) {
        addComradesToCluster(c,s->getInTransition(t)->getBeginState());
      }
    }
    for (int t = 0; t < s->getNumOutTransitions(); ++t) {
      if (s->getOutTransition(t)->getEndState()->getRank() == s->getRank()) {
        addComradesToCluster(c,s->getOutTransition(t)->getEndState());
      }
    }
  }
}

void LTS::mergeSuperiorClusters(Utils::RankStyle rs) {
  State *s,*sup;
  Cluster *c,*mc;
  set< Cluster* > mergeSet,descSet;
  set< Cluster* >::iterator c_it1;
  vector< Cluster* > *prevRank;
  vector< Cluster* >::iterator c_it;
  int i,r,t;
  // iterate over the ranks in reverse order (bottom-up)
  for (r = clustersInRank.size()-1; r > 0; --r) {
    prevRank = &(clustersInRank[r-1]);
    // iterate over the clusters in this rank
    for (c_it  = clustersInRank[r].begin();
         c_it != clustersInRank[r].end(); ++c_it) {
      c = *c_it;
      mergeSet.clear();
      // iterate over the states in this cluster
      for (i = 0; i < c->getNumStates(); ++i) {
        s = c->getState(i);
        // set deadlock information
        c->setDeadlock(c->hasDeadlock() || s->isDeadlock());
        // iterate over the superiors of s
        if (rs == CYCLIC) {
          for (t = 0; t < s->getNumOutTransitions(); ++t) {
            sup = s->getOutTransition(t)->getEndState();
            if (s->getRank()-1 == sup->getRank()) {
              // add the superior's cluster to the merge set
              mergeSet.insert(sup->getCluster());
            }
          }
        }
        for (t = 0; t < s->getNumInTransitions(); ++t) {
          sup = s->getInTransition(t)->getBeginState();
          if (s->getRank()-1 == sup->getRank()) {
            // add the superior's cluster to the merge set
            mergeSet.insert(sup->getCluster());
          }
        }
      }
      
      if (mergeSet.size() > 1) {
        mc = new Cluster(r-1);
        // Give mc a preliminary positionInRank
        mc->setPositionInRank(prevRank->size());
        descSet.clear();
        // iterate over the clusters in the mergeSet
        for (c_it1 = mergeSet.begin(); c_it1 != mergeSet.end(); ++c_it1) {
          // add the cluster's states to mc
          for (i = 0; i < (**c_it1).getNumStates(); ++i) {
            s = (**c_it1).getState(i);
            mc->addState(s);
            s->setCluster(mc);
          }
          for (i = 0; i < (**c_it1).getNumDescendants(); ++i) {
            descSet.insert((**c_it1).getDescendant(i));
          }
          // delete the cluster
          prevRank->erase(std::find(prevRank->begin(),prevRank->end(),*c_it1));
          delete *c_it1;
        }
        // update hierarchy info
        descSet.insert(c);
        for (c_it1 = descSet.begin(); c_it1 != descSet.end(); ++c_it1) {
          mc->addDescendant(*c_it1);
          (**c_it1).setAncestor(mc);
        }
        prevRank->push_back(mc);
      } else {
        mc = *(mergeSet.begin());
        mc->addDescendant(c);
        c->setAncestor(mc);
      } 
    }

    // This rank is stable, add rank location information to clusters.
    for (size_t i = 0; i < clustersInRank[r].size(); ++i) {
     clustersInRank[r][i]->setPositionInRank(i);
    }
  }
}

void LTS::computeClusterLabelInfo() {
  State* s;
  list< State* >::iterator li;
  int t;
  for (li = markedStates.begin(); li != markedStates.end(); ++li) {
    s = *li;
    for (t = 0; t < s->getNumOutTransitions(); ++t) {
      s->getCluster()->addActionLabel(s->getOutTransition(t)->getLabel());
    }
    for (t = 0; t < s->getNumLoops(); ++t) {
      s->getCluster()->addActionLabel(s->getLoop(t)->getLabel());
    }
  }
  for (li = unmarkedStates.begin(); li != unmarkedStates.end(); ++li) {
    s = *li;
    for (t = 0; t < s->getNumOutTransitions(); ++t) {
      s->getCluster()->addActionLabel(s->getOutTransition(t)->getLabel());
    }
    for (t = 0; t < s->getNumLoops(); ++t) {
      s->getCluster()->addActionLabel(s->getLoop(t)->getLabel());
    }
  }
}

void LTS::positionClusters() {
  // iterate over the ranks in reverse order (bottom-up)
  for (vector< vector< Cluster*> >::reverse_iterator r_it =
      clustersInRank.rbegin(); r_it != clustersInRank.rend(); ++r_it)  {
    // iterate over the clusters in this rank
    for (vector<Cluster*>::iterator c_it = r_it->begin(); c_it != r_it->end();
        ++c_it) {
      // compute the size of this cluster and the positions of its descendants
      (**c_it).computeSizeAndDescendantPositions();
    }
  }
  // position the initial state's cluster
  initialState->getCluster()->center();
}

void LTS::positionStates() {
  vector< State* > undecided;
  edgeLengthBottomUp(undecided);
  edgeLengthTopDown(undecided);
  resolveClusterSlots();
}

void LTS::clearStatePositions() {
  unsigned int i,j;
  int k;
  Cluster *c;
  for (i = 0; i < clustersInRank.size(); ++i) {
    for (j = 0; j < clustersInRank[i].size(); ++j) {
      c = clustersInRank[i][j];
      // center all states
      for (k = 0; k < clustersInRank[i][j]->getNumStates(); ++k) {
        c->getState(k)->center();
      }
      // clear the slots and undecided states
      c->clearSlots();
    }
  }
}

void LTS::edgeLengthBottomUp(vector< State* > &undecided) {
  //Phase 1: Processes states bottom-up, keeping edges as short as possible.
  //Pre:  clustersInRank is correctly sorted by rank. 
  //Post: states are positioned bottom up, keeping edges as 
  //      short as possible, if enough information is available.
  //Ret:  states that could not be placed in this phase, sorted bottom-up
  //
  // The details of this algorithm can be found in  Frank van Ham's master's
  // thesis, pp. 21-29
 
  int r,s,t;
  unsigned int c;
  State *succ, *currState;
  Cluster* currCluster;
  // Iterate over the ranks in reverse order (bottom-up)
  for (r = clustersInRank.size()-1; r >= 0; --r) {
    // Iterate over the clusters in this rank
    for (c = 0; c < clustersInRank[r].size(); ++c) {
      currCluster = clustersInRank[r][c];
      // Iterate over the states in this cluster
      for (s = 0; s < clustersInRank[r][c]->getNumStates(); ++s) {
        currState = currCluster->getState(s);
        // Compute the position of the state, based on number and position of 
        // descendants of its cluster.
        if (currCluster->getNumStates() == 1) {
          currState->center();
          currCluster->occupyCenterSlot(currState);
        }
        else {
          switch (currCluster->getNumDescendants()) {
            case 0:
            {
              // Case three: No descendant clusters.
              // Center the state and mark it as undecided
              currState->center();

              // if the cluster is centered, this state has to be processed in the
              // second pass, otherwise it will skip the second pass and be
              // processed in the third pass.
              if (currCluster->isCentered()) {
                undecided.push_back(currState);
              }
              else {
                currCluster->addUndecidedState(currState);
              }
              break;
            }
            case 1:
            {
              // Case one: One descendant cluster.
              // We know the states in this cluster have a pseudo-correct position
              // (as determined in this pass), since we process the system bottom-up

              // Get the subordinate states. These are all in one cluster, and we 
              // can calculate the position of the current state directly from them.
              
              // Variable that keeps track of whether or not the children are all 
              // centered.
              bool allcentered = true;
              t = 0;
              while (allcentered && t < currState->getNumOutTransitions()) {
                succ = currState->getOutTransition(t)->getEndState();
                if (currState->getRank()+1 == succ->getRank()) {
                  allcentered = succ->isCentered();
                }
                ++t;
              }

              if (allcentered) {
                // Center the state and mark it as undecided
                currState->center();
                // if the cluster is centered, this state has to be processed in the
                // second pass, otherwise it will skip the second pass and be
                // processed in the third pass.
                if (currCluster->isCentered()) {
                  undecided.push_back(currState);
                }
                else {
                  currCluster->addUndecidedState(currState);
                }
              }
              else {
                Vect v = {0,0};
                for (t = 0; t < currState->getNumOutTransitions(); ++t) {
                  succ = currState->getOutTransition(t)->getEndState();
                  if (currState->getRank()+1 == succ->getRank()
                      && !succ->isCentered()) {
                    // convert subordinate position to a vector and add
                    v = v + succ->getPositionRadius() *
                      deg_to_vec(succ->getPositionAngle());
                  }
                }

                unsigned int ring = round_to_int(vec_length(v) * float(NUM_RINGS-1) /
                    currCluster->getTopRadius());
                ring = min(ring,NUM_RINGS-1);

                if (ring == 0) {
                  // v is sufficiently small, center the state.
                  currState->center();
                  currCluster->occupyCenterSlot(currState);
                }
                else {
                  // Transform v into an angle, assign it to state position.
                  float angle = vec_to_deg(v);
                  currState->setPositionAngle(angle);
                  currState->setPositionRadius(currCluster->getTopRadius() *
                      float(ring) / float(NUM_RINGS-1));
                  currCluster->occupySlot(ring,angle,currState);
                }
              }
              break;
            }
            default:
            {
              // Case two: More than one descendant cluster.
              // Iterate over subordinates, calculating sum of vectors.
              Vect v = {0,0};
              for (t = 0; t < currState->getNumOutTransitions(); ++t) {
                succ = currState->getOutTransition(t)->getEndState();
                if (currState->getRank()+1 == succ->getRank()) {
                  if (succ->getCluster()->isCentered()) {
                    if (!succ->isCentered()) {
                      v = v + succ->getPositionRadius() *
                        deg_to_vec(succ->getPositionAngle());
                    }
                  }
                  else {
                    v = v + currCluster->getBaseRadius() *
                      deg_to_vec(succ->getCluster()->getPosition());
                    if (!succ->isCentered()) {
                      v = v + succ->getPositionRadius() *
                        deg_to_vec(succ->getPositionAngle() +
                            succ->getCluster()->getPosition());
                    }
                  }
                }
              }
              
              unsigned int ring = round_to_int(vec_length(v) * float(NUM_RINGS-1)
                  / currCluster->getTopRadius());
              ring = min(ring,NUM_RINGS-1);

              if (ring == 0) {
                // v is sufficiently small, center the state.
                currState->center();
                currCluster->occupyCenterSlot(currState);
              }
              else {
                // Transform v into an angle, assign it to state position.
                float angle = vec_to_deg(v);
                currState->setPositionAngle(angle);
                currState->setPositionRadius(currCluster->getTopRadius() *
                    float(ring) / float(NUM_RINGS-1));
                currCluster->occupySlot(ring,angle,currState);
              }
              break;
            }
          }
        }
      }
    }
  }
}

void LTS::edgeLengthTopDown(vector< State* > &ss) {
  //Phase 2: Process states top-down, keeping edges as short as possible.
  //Pre:  ss is correctly sorted by rank, bottom-up.
  //Post: ss contains the states that could not be placed by this phase, sorted
  //top-down.
  Cluster* currCluster;
  State* pred;
  int t;
  bool allcentered;

  // Iterate over the states in reverse order (top-down)
  for (vector<State*>::reverse_iterator state_it = ss.rbegin(); state_it !=
      ss.rend(); ++state_it) {

    // We know that this state is already centered after the previous pass
    // 
    // Get the cluster belonging to this state
    currCluster = (*state_it)->getCluster();

    // We know there is only one ancestor cluster and that the current cluster
    // is centered below it.
    // We know the states in the above cluster have pseudo-correct
    // positions, as determined in this and the previous pass, so we can
    // proceed placing the states based on this information.
    // The ancestor states are all in one cluster, and we can calculate the
    // position of the current state directly from them.

    // Calculate the sum of the vectors gained from all subordinate angles
    allcentered = true;
    t = 0;
    while (allcentered && t < (**state_it).getNumInTransitions()) {
      pred = (**state_it).getInTransition(t)->getBeginState();
      if ((**state_it).getRank()-1 == pred->getRank()) {
        allcentered = pred->isCentered();
      }
      ++t;
    }
    if (allcentered) {
      currCluster->addUndecidedState(*state_it);
    }
    else {
      Vect v = {0,0};
      for (t = 0; t < (**state_it).getNumInTransitions(); ++t) {
        pred = (**state_it).getInTransition(t)->getBeginState();
        if ((**state_it).getRank()-1 == pred->getRank()
            && !pred->isCentered()) {
          v = v + pred->getPositionRadius() * 
            deg_to_vec(pred->getPositionAngle());
        }
      }

      unsigned int ring = round_to_int(vec_length(v) * float(NUM_RINGS-1) /
          currCluster->getTopRadius());
      ring = min(ring,NUM_RINGS-1);

      if (ring == 0) {
        // v is sufficiently small, center the state.
        (*state_it)->center();
        currCluster->occupyCenterSlot(*state_it);
      }
      else {
        // Transform v into an angle, assign it to state position.
        float angle = vec_to_deg(v);
        (*state_it)->setPositionAngle(angle);
        (*state_it)->setPositionRadius(currCluster->getTopRadius() * float(ring)
            / float(NUM_RINGS-1));
        currCluster->occupySlot(ring,angle,*state_it);
      }
    }
  }
}

void LTS::resolveClusterSlots() {
  //Resolves the slots of each cluster, positioning the states within each slot
  //in such a way that they do not overlap.
  vector< vector< Cluster* > >::iterator rank_it;
  vector< Cluster* >::iterator clus_it;
  for (rank_it = clustersInRank.begin(); rank_it != clustersInRank.end();
      ++rank_it) {
    for (clus_it = rank_it->begin(); clus_it != rank_it->end(); ++clus_it) {
      (*clus_it)->resolveSlots();
    }
  }
}

void LTS::addMarkRule(MarkRule* r,int index) {
  if (index == -1) {
    markRules.push_back(r);
  } 
  else 
  {
    markRules.insert(markRules.begin()+index,r);
  }

  if (r->isActivated) {
    processAddedMarkRule(r);
  }
}

void LTS::processAddedMarkRule(MarkRule* r) {
  // Add r to all clusters' states.
  for (vector< vector<Cluster*> >::iterator r_it = clustersInRank.begin();
       r_it != clustersInRank.end(); ++r_it)
  {
    for(vector<Cluster*>::iterator c_it = (*r_it).begin(); 
        c_it != (*r_it).end(); ++c_it)
    {
      (*c_it)->markState(r);
    }
  }

  std::list<State*>::iterator it = unmarkedStates.begin();

  while (it != unmarkedStates.end())
  {
    if ((*it)->isMarked())
    {
      markedStates.push_back((*it));
      it = unmarkedStates.erase(it);
    }
    else
    {
      ++it;
    }
  }
  
  

  if (matchAny == 1)
  {
    unsigned int activeMarkRulesCount = 0;

    for ( vector< MarkRule* >::iterator mr_it = markRules.begin() ; mr_it !=
          markRules.end() ; ++mr_it )
    {
      if ( (**mr_it).isActivated )
      {
        ++activeMarkRulesCount;
      }
    }

    for(it = markedStates.begin(); it != markedStates.end(); ++it)
    {
      State* s = (*it);
      if(s->nrRulesMatched() == activeMarkRulesCount)
      {
        s->setMarking(true);
        s->getCluster()->setMarking(true);
      }
      else
      {
        s->setMarking(false);
        s->getCluster()->setMarking(false);
      }
    }
  }
    

  /*if (matchAny == 0) {
    // MatchAny is true, so just add the 
    //TODO Inefficient implementation
    li = unmarkedStates.begin();
    while (li != unmarkedStates.end()) {
      s = *li;
      if (r->valueSet[s->getParameterValue(r->paramIndex)]) {
        s->getCluster()->markState(r);
        markedStates.push_back(s);
        li = unmarkedStates.erase(li);
      } else {
        ++li;
      }
    }
  }
  
  else if (matchAny == 1) {
    // TODO: Inefficient implementation
    li = markedStates.begin();
    while (li != markedStates.end()) {
      s = *li;
      if (!r->valueSet[s->getParameterValue(r->paramIndex)]) {
        s->getCluster()->unmarkState(r);
        unmarkedStates.push_back(s);
        li = markedStates.erase(li);
      } else {
        ++li;
      }
    }
  }
  else
  {
    // matchAny == 2
    li = unmarkedStates.begin();
    while (li != unmarkedStates.end())
    {
      s = *li;
      if (r->valueSet[s->getParameterValue(r->paramIndex)])
      {
        s->getCluster()->markState(r);
      }
      ++li;
    }

    li = markedStates.begin();
    while (li != markedStates.end())
    {
      s = *li;
      if (r->valueSet[s->getParameterValue(r->paramIndex)])
      {
        s->getCluster()->markState(r);
      }
      ++li;
    }
  }
  */
  
  // TODO WORKAROUND
  setMatchAnyMarkRule(matchAny);

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
  // Unmark each state in each cluster, which match r
  for(vector<vector<Cluster*> >::iterator it = clustersInRank.begin();
      it != clustersInRank.end(); ++it)
  {
    for(vector<Cluster*>::iterator c_it = (*it).begin(); c_it != (*it).end(); 
        ++c_it)
    {
      (*c_it)->unmarkState(r);
    }
  }

  // Adapt markedStates, unmarkedStates
  list<State*>::iterator s_it = markedStates.begin();

  while (s_it != markedStates.end())
  {
    if ((*s_it)->nrRulesMatched() == 0)
    {
      unmarkedStates.push_back(*s_it);
      s_it = markedStates.erase(s_it);
    }
    else
    {
      ++s_it;
    }
  }

  //TODO WORKAROUND
  setMatchAnyMarkRule(matchAny);
}

MarkRule* LTS::getMarkRule( const int index ) const
{
  return markRules[index];
}

int LTS::getMatchAnyMarkRule() const
{
  return matchAny;
}

void LTS::setMatchAnyMarkRule(int ma) {
  matchAny = ma;

  vector<MarkRule*> activeMarkRules;

  for(vector<MarkRule*>::iterator mr_it = markRules.begin();
      mr_it != markRules.end(); ++mr_it)
  {
    if((*mr_it)->isActivated)
    {
      activeMarkRules.push_back(*mr_it);
    }
  }

  for(list<State*>::iterator s_it = markedStates.begin();
      s_it != markedStates.end(); ++s_it)
  {
    (*s_it)->setMarking(false);
    (*s_it)->getCluster()->setMarking(false);
    (*s_it)->setMarkAllEmpty(false);
    (*s_it)->getCluster()->setMarkAllEmpty(false);
  }

  if (activeMarkRules.size() == 0 && matchAny == 1)
  {
    for(list<State*>::iterator s_it = unmarkedStates.begin();
        s_it != unmarkedStates.end(); ++s_it)
    {
      (*s_it)->setMarking(true);
      (*s_it)->getCluster()->setMarking(true);
      (*s_it)->setMarkAllEmpty(true);
      (*s_it)->getCluster()->setMarkAllEmpty(true);
    }
  }
  else
  {
    
    for(vector<vector<Cluster*> >::iterator r_it = clustersInRank.begin();
        r_it != clustersInRank.end(); ++r_it)
    {
      for(vector<Cluster*>::iterator c_it = (*r_it).begin();
          c_it != (*r_it).end(); ++c_it)
      {
        for(vector<MarkRule*>::iterator it = activeMarkRules.begin(); 
            it != activeMarkRules.end(); ++it)
        {
          (*c_it)->markState(*it);
        }
      }
    }

    if (matchAny == 1)
    {

      for(list<State*>::iterator it = markedStates.begin(); 
          it != markedStates.end(); ++it)
      {
        State* s = (*it);
        if(s->nrRulesMatched() == activeMarkRules.size())
        {
          s->setMarking(true);
          s->getCluster()->setMarking(true);
        }
        else
        {
          s->setMarking(false);
          s->getCluster()->setMarking(false);
        }
      }
    }
  }
  
  list<State*>::iterator s_it = unmarkedStates.begin();

  while(s_it != unmarkedStates.end())
  {
    if((*s_it)->isMarked())
    {
      markedStates.push_back(*s_it);
      s_it = unmarkedStates.erase(s_it);
    }
    else
    {
      ++s_it;
    }
  }

  s_it = markedStates.begin();

  while(s_it != markedStates.end())
  {
    if(!(*s_it)->isMarked())
    {
      unmarkedStates.push_back(*s_it);
      s_it = markedStates.erase(s_it);
    }
    else
    {
      ++s_it;
    }
  }
  /*
  if (matchAny == ma || ma == 2) {
    return;
  }
  
  matchAny = ma;

  vector< MarkRule* > activeMarkRules;
  for (vector< MarkRule* >::iterator mr_it = markRules.begin();
      mr_it != markRules.end(); ++mr_it) {
    if ((**mr_it).isActivated) {
      activeMarkRules.push_back(*mr_it);
    }
  }
  
  if (activeMarkRules.size() == 0) {
    list< State* >::iterator li;
    if (matchAny == 0) {
      for (li = markedStates.begin(); li != markedStates.end(); ++li) {
        for(vector<MarkRule*>::iterator mr_it = markRules.begin();
            mr_it != markRules.end(); ++mr_it)
        {
          // TODO inefficient
          (*li)->getCluster()->unmarkState((*mr_it));
        }
      }
    }
    else if (matchAny == 1) {
      for (li = unmarkedStates.begin(); li != unmarkedStates.end(); ++li) {
        for(vector<MarkRule*>::iterator mr_it = markRules.begin();
            mr_it != markRules.end(); ++mr_it)
        {
          //TODO inefficient
          (*li)->getCluster()->markState((*mr_it));
        }
      }
    }
    else
    {
      // matchAny == 2
      // TODO
    }
    markedStates.swap(unmarkedStates);
  }
  else if (activeMarkRules.size() == 1) {
    return;
  }
  else {
    State* s;
    list< State* >::iterator li;
    unsigned int i;
    if (matchAny == 0) {
      li = unmarkedStates.begin();
      while (li != unmarkedStates.end()) {
        s = *li;
        for (i = 0; i < activeMarkRules.size() && !s->isMarked(); ++i) {
          MarkRule* markRule = activeMarkRules[i];
          if (markRule->valueSet[s->getParameterValue(markRule->paramIndex)])  {
            s->getCluster()->markState(markRule);
            markedStates.push_back(s);
            li = unmarkedStates.erase(li);
          }
        }
        if (!s->isMarked()) {
          ++li;
        }
      }
    }
    else if (matchAny == 1) {
      li = markedStates.begin();
      while (li != markedStates.end()) {
        s = *li;
        for (i = 0; i < activeMarkRules.size() && s->isMarked(); ++i) {
          MarkRule* markRule = activeMarkRules[i];
          if (!markRule->valueSet[s->getParameterValue(markRule->paramIndex)]) {
            s->getCluster()->unmarkState(markRule);
            unmarkedStates.push_back(s);
            li = markedStates.erase(li);
          }
        }
        if (s->isMarked()) {
          ++li;
        }
      }
    }
    else
    {
      // matchAny == 2
      // TODO
    }
  }*/
}

void LTS::markClusters() {
  // recompute the markings of clusters (after applying a different rank style)
  // process marked states
  State* s;
  list< State* >::iterator li;
  for (li = markedStates.begin(); li != markedStates.end(); ++li) {
    s = *li;
    if (s->isMarked()) {
      for(vector<MarkRule*>::iterator it = markRules.begin(); 
          it != markRules.end(); ++it)
      {
        // TODO inefficient
        s->getCluster()->markState((*it));
      }
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
      if (*c_it != NULL)
      {
        markedTransitionCount += (**c_it).markActionLabel(l);
      }
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
      if(*c_it != NULL)
      {
        markedTransitionCount += (**c_it).unmarkActionLabel(l);
      }
    }
  }
}

LTS* LTS::zoomIntoAbove()
{
  if (selectedCluster != NULL)
  {
    LTS* newLTS = new LTS(mediator, this, true);

    return newLTS;
  }
  else {
    return this;
  }
}

LTS* LTS::zoomIntoBelow()
{
  if (selectedCluster != NULL) 
  {
    LTS* newLTS = new LTS(mediator, this, false);

    return newLTS;
  }
  else 
  {
    return this;
  }
}

LTS* LTS::zoomOut()
{
  if (previousLevel != NULL)
  {
    if(lastWasAbove)
    {
        
      Cluster* child = NULL;
      Cluster* parent = lastCluster;

      do {      
        for (int i = 0; i < parent->getNumDescendants(); ++i)
        {
            parent->healSeverance(i);
        }
        child = parent;
        parent = child->getAncestor();
      } while (child != initialState->getCluster());

    }
    list< State* >::iterator li;
    for (li = markedStates.begin(); li != markedStates.end(); ++li)
    {
      (*li)->setZoomLevel(zoomLevel - 1);
    }
    for (li = unmarkedStates.begin(); li != unmarkedStates.end(); ++li)
    {
      (*li)->setZoomLevel(zoomLevel - 1);
    }

    return previousLevel;
  }
  else
  {
    return this;
  }
}

void LTS::setLastCluster(Cluster* c)
{
  lastCluster = c;
}

void LTS::setPreviousLevel(LTS* prev)
{
  previousLevel = prev;
}

LTS* LTS::getPreviousLevel() const
{
  return previousLevel;
}

void LTS::fromAbove()
{
  lastWasAbove = true;
}

Simulation* LTS::getSimulation() const
{
  return simulation;
}

Cluster* LTS::getSelectedCluster() const
{
  return selectedCluster;
}

std::vector<Utils::MarkRule*> LTS::getMarkRules() const
{
  return markRules;
}

bool* LTS::getActionMarked(int i) const
{
  return label_marks[i];
}


int LTS::getZoomLevel() const
{
  return zoomLevel;
}

void LTS::setZoomLevel(const int level)
{
  zoomLevel = level;
}

void LTS::trim() {
  // removes unreachable parts from the LTS
  visit(initialState);
  list<State*>::iterator li = unmarkedStates.begin();
  while (li != unmarkedStates.end()) {
    if ((*li)->isMarkedNoRule()) 
    {
        (*li)->unmark();
        ++li;
    } 
    else 
    {
      delete (*li);
      li = unmarkedStates.erase(li);
    }
  }
}

void LTS::visit(State* s) {
  if (!s->isMarkedNoRule()) 
  {
    s->mark();
    
    for (int i = 0; i < s->getNumOutTransitions(); ++i) 
    {
      visit(s->getOutTransition(i)->getEndState());
    }
  }
}

void LTS::loadTrace(std::string const& path)
{
  Trace tr;
  if (!tr.load(path))
  {
    std::string error = "Error loading file: " + path;
    mediator->reportError(error);
  }
  else 
  {
    // To satisfy pretty printing preconditions.
    gsEnableConstructorFunctions();

    Simulation* sim = new Simulation();
    // Initialize simulation with initial state of the LTS;
    State* initState;
    LTS* topLevel = this;

    // Find the initial state of the entire, zoomed out structure
    while (topLevel->getPreviousLevel() != NULL)
    {
      topLevel = topLevel->getPreviousLevel();
    }
    initState = topLevel->getInitialState();

    sim->setInitialState(initState);
    sim->start();

    // Get the first state of the trace (as an ATermAppl)
    ATermAppl currState = tr.currentState();
    // Now, currState ~ initState. 
    //
    // In currState, free variables can occur, instantiate this with the values
    // of the initial state in the simulation.
    //
    // Assumption: The ith parameter in currState is equal to the ith parameter
    // in initState.
    for(unsigned int i = 0; i < ATgetLength(ATgetArguments(currState)); ++i)
    {

      ATerm currVal = ATgetArgument(currState, i);
      string value = PrintPart_CXX(currVal, ppDefault);

      std::string paramValue = valueTable[i]
                                         [initState->getParameterValue(i)];
    }

    // Load the rest of the trace.

    while (tr.getPosition() != tr.getLength())
    {
      std::string action = PrintPart_CXX(ATgetArgument(
                                          ATgetArgument(tr.getAction(),0),0),
                                         ppDefault);

      std::vector<Transition*> posTrans = sim->getPosTrans();
      int possibilities = 0;
      int toChoose = -1;

      for(size_t i = 0; i < posTrans.size(); ++i)
      { 
        if (action == labels[posTrans[i]->getLabel()])
        {
          ++possibilities;
          toChoose = i;
        }
      }

      if (possibilities > 1)
      {
        // More than one possibility, meaning that choosing on action name is
        // ambiguous. Solve disambiguation by looking at states
        
        currState = tr.currentState();


        // Match is the score keeping track of how well a state matches an LPS
        // state. The (unique) state with the maximum match will be chosen.
        // The value of this match should be the number of variables which have
        // the same value as in the LPS, minus the number of free variables (
        // which are undetectable).
        int maxmatch = -1; 

        for (size_t j = 0; j < posTrans.size(); ++j)
        {
          State* s = posTrans[j]->getEndState();
          int match = 0;

          for(unsigned int i = 0; i < ATgetLength(ATgetArguments(currState));
              ++i)
          {

            std::string currVal = PrintPart_CXX(ATgetArgument(currState, i), 
                                              ppDefault);

            std::map<std::string, std::string>::iterator it;

            if (currVal == valueTable[i][s->getParameterValue(i)])
            {
              ++match;
            }
            
          }

          if (match > maxmatch)
          {
            maxmatch = match;
            toChoose = j;
          }
        }
      }
      else if (possibilities == 1)
      {
        // Exactly one possibility, so skip
      }
      else 
      {
        // This cannot occur, unless there was some mismatch between lps and lts
        std::string error = "Could not regenerate trace, does it belong to the loaded LTS?";
        mediator->reportError(error);
        toChoose = -1;
        return;
      }
      
      sim->chooseTrans(toChoose);
      sim->followTrans();
    }


    // Set simulation to the LTS
    simulation = sim;
  }
}

void LTS::generateBackTrace()
{
  State* initState;
  LTS* topLevel = this;

  // Find the initial state of the entire, zoomed out structure
  while (topLevel->getPreviousLevel() != NULL)
  {
    topLevel = topLevel->getPreviousLevel();
  }
  initState = topLevel->getInitialState();
  
  simulation->traceBack(initState);
}
