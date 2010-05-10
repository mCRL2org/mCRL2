// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lts.cpp
/// \brief Source file of LTS class

#include "wx.hpp" // precompiled headers
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/trace.h"
#include "mcrl2/core/print.h"
#include "cluster.h"
#include "fsm_state_positioner.h"
#include "lts.h"
#include "mathutils.h"
#include "mediator.h"
#include "simulation.h"
#include "state.h"
#include "transition.h"

using namespace std;
using namespace mcrl2::trace;
using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace MathUtils;

/**************************** Cluster iterators *******************************/

Cluster_iterator::Cluster_iterator(LTS *l)
{
  lts = l;
  rank = 0;
  cluster = 0;
  while (!is_end() && !is_valid())
  {
    next();
  }
}

void Cluster_iterator::operator++()
{
  if (!is_end())
  {
    next();
    while (!is_end() && !is_valid())
    {
      next();
    }
  }
}

Cluster* Cluster_iterator::operator*()
{
  return lts->clustersInRank[rank][cluster];
}

bool Cluster_iterator::is_valid()
{
  return (lts->clustersInRank[rank].size() > 0)
      && (lts->clustersInRank[rank][cluster] != NULL);
}

bool Cluster_iterator::is_end()
{
  return (rank == lts->getMaxRanks());
}

void Cluster_iterator::next()
{
  ++cluster;
  if (cluster >= lts->clustersInRank[rank].size())
  {
    cluster = 0;
    ++rank;
    while (rank < lts->getMaxRanks() &&
        lts->clustersInRank[rank].size() == 0)
    {
      ++rank;
    }
  }
}

Reverse_cluster_iterator::Reverse_cluster_iterator(LTS* l)
  : Cluster_iterator(l)
{
  rank = lts->getMaxRanks() - 1;
}

bool Reverse_cluster_iterator::is_end()
{
  return (rank == -1);
}

void Reverse_cluster_iterator::next()
{
  ++cluster;
  if (cluster >= lts->clustersInRank[rank].size())
  {
    cluster = 0;
    --rank;
    while (rank >= 0 && lts->clustersInRank[rank].size() == 0)
    {
      --rank;
    }
  }
}

/**************************** State_iterator **********************************/

State_iterator::State_iterator(LTS *l)
{
  lts = l;
  state_it = lts->states.begin();
}

State_iterator::~State_iterator()
{
}

void State_iterator::operator++()
{
  if (!is_end())
  {
    ++state_it;
  }
}

State* State_iterator::operator*()
{
  return *state_it;
}

bool State_iterator::is_end()
{
  return (state_it == lts->states.end());
}

/**************************** LTS *********************************************/

LTS::LTS(Mediator* owner)
{
  mediator = owner;
  deadlockCount = -1;
  simulation = new Simulation();
  initialState = NULL;
  selectedState = NULL;
  selectedCluster = NULL;
  lastCluster = NULL;
  previousLevel = NULL;
  lastWasAbove = false;
  zoomLevel = 0;
  mcrl2_lts = NULL;
}

LTS::LTS(Mediator* owner, LTS* parent, bool fromAbove)
{
  // Copied/initial information
  lastWasAbove = fromAbove;
  previousLevel  = parent;
  selectedState = NULL;
  zoomLevel = previousLevel->getZoomLevel() + 1;
  selectedCluster = previousLevel->getSelectedCluster();
  mediator = owner;

  // simulation = new Simulation();
  simulation = previousLevel->getSimulation();

  mcrl2_lts = previousLevel->getmCRL2LTS();

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
}

LTS::~LTS()
{
  if (previousLevel == NULL)
  {
    // This LTS is the top level LTS, so delete all its contents.
    unsigned int i,r;
    vector< State* >::iterator li;
    for (li = states.begin(); li != states.end(); ++li)
    {
      delete *li;
    }
    states.clear();
    initialState = NULL;

    for (r = 0; r < clustersInRank.size(); ++r)
    {
      for (i = 0; i < clustersInRank[r].size(); ++i)
      {
        delete clustersInRank[r][i];
      }
    }
    clustersInRank.clear();

    simulation->stop();
    delete simulation;
    if (mcrl2_lts != NULL)
    {
      delete mcrl2_lts;
      mcrl2_lts = NULL;
    }
  }
  else
  {
    states.clear();
    clustersInRank.clear();
  }
}

Cluster_iterator LTS::getClusterIterator()
{
  return Cluster_iterator(this);
}

Reverse_cluster_iterator LTS::getReverseClusterIterator()
{
  return Reverse_cluster_iterator(this);
}

State_iterator LTS::getStateIterator()
{
  return State_iterator(this);
}

string LTS::getParameterName(int parindex)
{
  if (!mcrl2_lts->has_state_parameters())
  {
    return "";
  }
  return mcrl2_lts->state_parameter_name_str(parindex);
}

atermpp::set<ATerm> LTS::getParameterDomain(int parindex)
{
  if (!mcrl2_lts->has_state_parameters())
  {
    atermpp::set<ATerm> empty_set;
    return empty_set;
  }
  return mcrl2_lts->get_state_parameter_values(parindex);
}

string LTS::prettyPrintParameterValue(ATerm parvalue)
{
  return mcrl2_lts->pretty_print_state_parameter_value(parvalue);
}

ATerm LTS::getStateParameterValue(State* state,unsigned int param)
{
  if (!mcrl2_lts->has_state_parameters())
  {
    return NULL;
  }
  return mcrl2_lts->get_state_parameter_value(state->getID(),param);
}

std::string LTS::getStateParameterValueStr(State* state,
    unsigned int param)
{
  if (!mcrl2_lts->has_state_parameters())
  {
    return "";
  }
  return mcrl2_lts->pretty_print_state_parameter_value(
      mcrl2_lts->get_state_parameter_value(state->getID(),param));
}

atermpp::set<ATerm> LTS::getClusterParameterValues(Cluster* cluster,
    unsigned int param)
{
  if (!mcrl2_lts->has_state_parameters())
  {
    atermpp::set<ATerm> empty_set;
    return empty_set;
  }
  atermpp::set<ATerm> result;
  for (int i = 0; i < cluster->getNumStates(); ++i)
  {
    result.insert(getStateParameterValue(cluster->getState(i),param));
  }
  return result;
}

mcrl2::lts::lts* LTS::getmCRL2LTS()
{
  return mcrl2_lts;
}

bool LTS::readFromFile(std::string filename)
{
  mcrl2_lts = new mcrl2::lts::lts();

  // try to read the file
  if (!mcrl2_lts->read_from(filename,lts_none))
  {
    // bullocks, this file is no good...
    delete mcrl2_lts;
    mcrl2_lts = NULL;
    return false;
  }

  // remove unreachable states
  
  reachability_check(*mcrl2_lts,true);

  states.clear();
  states.reserve(mcrl2_lts->num_states());
  for (unsigned int i = 0; i < mcrl2_lts->num_states(); ++i)
  {
    states.push_back(new State(i));
  }

  initialState = states[mcrl2_lts->initial_state()];
  simulation->setInitialState(initialState);

  transition_iterator ti(mcrl2_lts);
  for ( ; ti.more(); ++ti)
  {
    State *s1 = states[ti.from()];
    State *s2 = states[ti.to()];
    Transition* t = new Transition(s1,s2,ti.label());
    if (s1 != s2)
    {
      s1->addOutTransition(t);
      s2->addInTransition(t);
    }
    else
    {
      s1->addLoop(t);
    }
  }
  return true;
}

State* LTS::selectStateByID(int id)
{
  State *state = states[id];
  if (state)
  {
    state->select();
    // For fast deselection
    selectedCluster = NULL;
    selectedState = state;

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
  }
  return selectedState;
}

Cluster* LTS::selectCluster(const int rank, const int pos)
{
  clustersInRank[rank][pos]->select();

  // For fast deselection.
  selectedState = NULL;
  selectedCluster = clustersInRank[rank][pos];
  return selectedCluster;
}

void LTS::deselect()
{
  if (selectedState != NULL)
  {
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

int LTS::getNumLabels()
{
  return mcrl2_lts->num_labels();
}

unsigned int LTS::getNumParameters() const
{
  if (mcrl2_lts->has_state_parameters())
  {
    return mcrl2_lts->num_state_parameters();
  }
  return 0;
}

string LTS::getLabel(int labindex)
{
  return mcrl2_lts->label_value_str(labindex);
}

void LTS::addCluster(Cluster* cluster)
{
  unsigned int rank = cluster->getRank();
  unsigned int pos = cluster->getPositionInRank();

  // Check to see if there is already a rank for this cluster
  if (clustersInRank.size() <= rank)
  {
    clustersInRank.resize(rank + 1);
  }

  if (clustersInRank[rank].size() <= pos)
  {
    clustersInRank[rank].resize(pos + 1);
  }

  clustersInRank[rank][pos] = cluster;

  for (int i = 0; i < cluster->getNumStates(); ++i)
  {
    State* state = cluster->getState(i);
    unsigned int state_id = state->getID();
    if (states.size() <= state_id)
    {
      states.resize(state_id + 1);
    }

    states[state_id] = state;
    state->setZoomLevel(zoomLevel);
  }
}

void LTS::addClusterAndBelow(Cluster* cluster)
{
  if (cluster != NULL)
  {
    addCluster(cluster);

    for (int i = 0; i < cluster->getNumDescendants(); ++i)
    {
      addClusterAndBelow(cluster->getDescendant(i));
    }
  }
}

void LTS::getActionLabels(vector< string > &ls) const
{
  ls.clear();
  for (unsigned int i = 0; i < mcrl2_lts->num_labels(); ++i)
  {
    ls.push_back(mcrl2_lts->label_value_str(i));
  }
}

State* LTS::getInitialState() const
{
  return initialState;
}

int LTS::getNumRanks() const
{
  int offset = 0;
  while(clustersInRank[offset].empty())
  {
    ++offset;
  }
  return clustersInRank.size() - offset;
}

int LTS::getMaxRanks() const
{
  return clustersInRank.size();
}

int LTS::getNumClusters() const
{
  int result = 0;
  for (size_t i = 0; i < clustersInRank.size(); ++i)
  {
    result += clustersInRank[i].size();
  }
  return result;
}

int LTS::getNumDeadlocks()
{
  if (deadlockCount == -1)
  {
    // a value of -1 indicates that we have to compute it
    deadlockCount = 0;
    vector< State* >::iterator state_it;
    for (state_it  = states.begin(); state_it != states.end(); ++state_it)
    {
      if ((**state_it).isDeadlock())
      {
        ++deadlockCount;
      }
    }
  }
  return deadlockCount;
}

int LTS::getNumStates() const
{
  return mcrl2_lts->num_states();
}

int LTS::getNumTransitions() const
{
  return mcrl2_lts->num_transitions();
}

void LTS::clearRanksAndClusters()
{
  vector< State* >::iterator it;
  for (it = states.begin(); it != states.end(); ++it)
  {
    (*it)->setRank(-1);
    (*it)->setCluster(NULL);
  }

  for (Cluster_iterator ci = getClusterIterator(); !ci.is_end(); ++ci)
  {
    delete *ci;
  }

  vector< vector< Cluster* > > temp2;
  clustersInRank.swap(temp2);
}

void LTS::rankStates(RankStyle rs)
{
  clearRanksAndClusters();
  int rankNumber = 0;

  vector< State* > nextRank,currRank;
  currRank.push_back(initialState);
  initialState->setRank(rankNumber);

  int i;
  vector< State* >::iterator it;
  State *s,*t;
  while (currRank.size() > 0)
  {
    nextRank.clear();
    // iterate over the states in this rank
    for (it  = currRank.begin(); it != currRank.end(); ++it)
    {
      s = *it;
      if (rs == CYCLIC)
      {
        // iterate over all in-transitions of cs
        for (i = 0; i < s->getNumInTransitions(); ++i)
        {
          t = s->getInTransition(i)->getBeginState();
          if (t->getRank() == -1)
          {
            t->setRank(rankNumber+1);
            nextRank.push_back(t);
          }
        }
      }
      // iterate over all out-transitions of cs
      for (i = 0; i < s->getNumOutTransitions(); ++i)
      {
        t = s->getOutTransition(i)->getEndState();
        if (t->getRank() == -1)
        {
          t->setRank(rankNumber+1);
          nextRank.push_back(t);
        }
      }
    }
    currRank.swap(nextRank);
    ++rankNumber;
  }
}

void LTS::clusterStates(RankStyle rs)
{
  Cluster *d = new Cluster(0);
  vector< Cluster* > cs;
  cs.push_back(d);
  d->setPositionInRank(0);
  clustersInRank.push_back(cs);
  clusterTree(initialState,d,rs == CYCLIC);
}

void LTS::clusterTree(State *v,Cluster *c,bool cyclic)
{
  int h,i,j,r;
  State *w, *y;
  c->addState(v);
  v->setCluster(c);
  for (i = 0; i < v->getNumOutTransitions(); ++i)
  {
    w = v->getOutTransition(i)->getEndState();
    if (w->getCluster() == NULL && w->getRank() == v->getRank())
    {
      clusterTree(w,c,cyclic);
    }
  }
  for (i = 0; i < v->getNumInTransitions(); ++i)
  {
    w = v->getInTransition(i)->getBeginState();
    if (w->getCluster() == NULL && w->getRank() == v->getRank())
    {
      clusterTree(w,c,cyclic);
    }
  }
  if (cyclic)
  {
    for (i = 0; i < v->getNumInTransitions(); ++i)
    {
      w = v->getInTransition(i)->getBeginState();
      r = w->getRank();
      if (w->getCluster() == NULL && r == v->getRank()+1)
      {
        Cluster *d = new Cluster(r);
        if ((unsigned int)(r) >= clustersInRank.size())
        {
          vector< Cluster* > cs;
          clustersInRank.push_back(cs);
        }
        d->setPositionInRank(clustersInRank[r].size());
        clustersInRank[r].push_back(d);
        d->setAncestor(c);
        c->addDescendant(d);
        clusterTree(w,d,cyclic);

        for (h = 0; h < d->getNumStates(); ++h)
        {
          y = d->getState(h);
          for (j = 0; j < y->getNumOutTransitions(); ++j)
          {
            w = y->getOutTransition(j)->getEndState();
            if (w->getCluster() == NULL && w->getRank() == v->getRank())
            {
              clusterTree(w,c,cyclic);
            }
          }
          for (j = 0; j < y->getNumInTransitions(); ++j)
          {
            w = y->getInTransition(j)->getBeginState();
            if (w->getCluster() == NULL && w->getRank() == v->getRank())
            {
              clusterTree(w,c,cyclic);
            }
          }
        }
      }
    }
  }
  for (i = 0; i < v->getNumOutTransitions(); ++i)
  {
    w = v->getOutTransition(i)->getEndState();
    r = w->getRank();
    if (w->getCluster() == NULL && r == v->getRank()+1)
    {
      Cluster *d = new Cluster(r);
      if ((unsigned int)(r) >= clustersInRank.size())
      {
        vector< Cluster* > cs;
        clustersInRank.push_back(cs);
      }
      d->setPositionInRank(clustersInRank[r].size());
      clustersInRank[r].push_back(d);
      d->setAncestor(c);
      c->addDescendant(d);
      clusterTree(w,d,cyclic);

      for (h = 0; h < d->getNumStates(); ++h)
      {
        y = d->getState(h);
        if (cyclic)
        {
          for (j = 0; j < y->getNumOutTransitions(); ++j)
          {
            w = y->getOutTransition(j)->getEndState();
            if (w->getCluster() == NULL && w->getRank() == v->getRank())
            {
              clusterTree(w,c,cyclic);
            }
          }
        }
        for (j = 0; j < y->getNumInTransitions(); ++j)
        {
          w = y->getInTransition(j)->getBeginState();
          if (w->getCluster() == NULL && w->getRank() == v->getRank())
          {
            clusterTree(w,c,cyclic);
          }
        }
      }
    }
  }
}

void LTS::computeClusterInfo()
{
  State* s;
  Cluster* c;
  vector< State* >::iterator li;
  int t;
  for (li = states.begin(); li != states.end(); ++li)
  {
    s = *li;
    c = s->getCluster();
    if (s->isDeadlock())
    {
      c->addDeadlock();
    }
    for (t = 0; t < s->getNumOutTransitions(); ++t)
    {
      c->addActionLabel(s->getOutTransition(t)->getLabel());
    }
    for (t = 0; t < s->getNumLoops(); ++t)
    {
      c->addActionLabel(s->getLoop(t)->getLabel());
    }
  }
}

void LTS::positionClusters(bool fsmstyle)
{
  if (fsmstyle)
  {
    initialState->getCluster()->computeSizeAndPositions_FSM();
  }
  else
  {
    initialState->getCluster()->computeSizeAndPositions();
  }
  // position the initial state's cluster
  initialState->getCluster()->center();
}

void LTS::clearStatePositions()
{
  for (State_iterator state_it = getStateIterator(); !state_it.is_end();
      ++state_it)
  {
    (**state_it).center();
  }
}

void LTS::positionStates()
{
  FSMStatePositioner state_positioner(this);
  state_positioner.positionStates();
}

LTS* LTS::zoomIntoAbove()
{
  if (selectedCluster != NULL)
  {
    LTS* newLTS = new LTS(mediator, this, true);
    return newLTS;
  }
  else
  {
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
    if (lastWasAbove)
    {
      Cluster* child = NULL;
      Cluster* parent = lastCluster;
      do
      {
        for (int i = 0; i < parent->getNumDescendants(); ++i)
        {
          parent->healSeverance(i);
        }
        child = parent;
        parent = child->getAncestor();
      } while (child != initialState->getCluster());
    }
    vector< State* >::iterator li;
    for (li = states.begin(); li != states.end(); ++li)
    {
      if (*li)
      {
        (*li)->setZoomLevel(zoomLevel - 1);
      }
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

int LTS::getZoomLevel() const
{
  return zoomLevel;
}

void LTS::setZoomLevel(const int level)
{
  zoomLevel = level;
}

void LTS::visit(State* s)
{
  if (!s->isSelected())
  {
    s->select();
    for (int i = 0; i < s->getNumOutTransitions(); ++i)
    {
      visit(s->getOutTransition(i)->getEndState());
    }
  }
}

void LTS::loadTrace(std::string const& path)
{
  Trace tr;
  try
  {
    tr.load(path);
  }
  catch (...)
  {
    std::string error = "Error loading file: " + path;
    mediator->reportError(error);
    return;
  }

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

      std::string paramValue = getStateParameterValueStr(initState,i);
    }

    // Load the rest of the trace.

    while (tr.getPosition() != tr.getLength())
    {
      std::string action = PrintPart_CXX(ATgetArgument(
                                          ATgetArgument(tr.nextAction(),0),0),
                                         ppDefault);

      std::vector<Transition*> posTrans = sim->getPosTrans();
      int possibilities = 0;
      int toChoose = -1;

      for(size_t i = 0; i < posTrans.size(); ++i)
      {
        if (action == mcrl2_lts->label_value_str(posTrans[i]->getLabel()))
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

            if (currVal == getStateParameterValueStr(s,i))
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
