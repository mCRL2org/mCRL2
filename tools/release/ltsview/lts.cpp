// Author(s): Bas Ploeger and Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/trace/trace.h"
#include "cluster.h"
#include "fsm_state_positioner.h"
#include "lts.h"
#include "mathutils.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/utilities/logger.h"
#include "sp_state_positioner.h"
#include "state.h"
#include "transition.h"

using namespace std;
using namespace mcrl2::trace;
using namespace mcrl2::core;
using namespace mcrl2::lts;
using namespace MathUtils;

/**************************** Cluster iterators *******************************/

Cluster_iterator::Cluster_iterator(LTS* l)
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
  return (!lts->clustersInRank[rank].empty())
         && (lts->clustersInRank[rank][cluster] != nullptr);
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
           lts->clustersInRank[rank].empty())
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
    while (rank >= 0 && lts->clustersInRank[rank].empty())
    {
      --rank;
    }
  }
}

/**************************** State_iterator **********************************/

State_iterator::State_iterator(LTS* l)
{
  lts = l;
  state_it = lts->states.begin();
}

State_iterator::~State_iterator()
= default;

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

LTS::LTS()
{
  deadlockCount = -1;
  initialState = nullptr;
  lastCluster = nullptr;
  previousLevel = nullptr;
  lastWasAbove = false;
  zoomLevel = 0;
  mcrl2_lts = nullptr;
}

LTS::LTS(LTS* parent, Cluster *target, bool fromAbove)
{
  // Copied/initial information
  lastWasAbove = fromAbove;
  previousLevel  = parent;
  zoomLevel = previousLevel->getZoomLevel() + 1;

  mcrl2_lts = previousLevel->mcrl2_lts;

  if (lastWasAbove)
  {
    initialState = previousLevel->getInitialState();

    lastCluster = target;

    Cluster* child = nullptr;
    Cluster* parent = target;

    do
    {
      for (int i = 0; i < parent->getNumDescendants(); ++i)
      {
        if (child == nullptr || parent->getDescendant(i) != child)
        {
          parent->severDescendant(i);
        }
      }

      addCluster(parent);
      child = parent;
      parent = child->getAncestor();
    }
    while (child != initialState->getCluster());

  }
  else
  {
    initialState = target->getState(0);
    addClusterAndBelow(target);
  }
}

LTS::~LTS()
{
  if (previousLevel == nullptr)
  {
    // This LTS is the top level LTS, so delete all its contents.
    size_t i,r;
    vector< State* >::iterator li;
    for (li = states.begin(); li != states.end(); ++li)
    {
      delete *li;
    }
    states.clear();
    initialState = nullptr;

    for (r = 0; r < clustersInRank.size(); ++r)
    {
      for (i = 0; i < clustersInRank[r].size(); ++i)
      {
        delete clustersInRank[r][i];
      }
    }
    clustersInRank.clear();

    delete mcrl2_lts;
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

string LTS::getParameterName(size_t parindex)
{
  return mcrl2_lts->process_parameter(parindex).first; // in an .fsm file a parameter is a pair of strings.
}

size_t LTS::getStateParameterValue(State* state,size_t param)
{
  return mcrl2_lts->state_label(state->getID())[param];
}

std::string LTS::getStateParameterValueStr(State* state, size_t param)
{
  using namespace mcrl2::lts::detail;
  return mcrl2_lts->state_element_value(param,(mcrl2_lts->state_label(state->getID()))[param]);
}

std::set<std::string> LTS::getClusterParameterValues(Cluster* cluster, size_t param)
{
  std::set<std::string> result;
  for (int i = 0; i < cluster->getNumStates(); ++i)
  {
    result.insert(getStateParameterValueStr(cluster->getState(i),param));
  }
  return result;
}


bool LTS::readFromFile(const std::string& filename)
{
  assert(!previousLevel);

  delete mcrl2_lts;

  mcrl2_lts = new mcrl2::lts::lts_fsm_t;

  load_lts_as_fsm_file(filename, *mcrl2_lts);

  // remove unreachable states
  reachability_check(*mcrl2_lts, true);

  states.clear();
  states.reserve(mcrl2_lts->num_states());
  for (size_t i = 0; i < mcrl2_lts->num_states(); ++i)
  {
    states.push_back(new State(static_cast<int>(i)));
  }

  initialState = states[mcrl2_lts->initial_state()];

  const std::vector<transition> &trans=mcrl2_lts->get_transitions();
  for (const auto & tran : trans)
  {
    State* s1 = states[tran.from()];
    State* s2 = states[tran.to()];
    auto* t = new Transition(s1,s2,static_cast<int>(tran.label()));
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

int LTS::getNumLabels()
{
  return static_cast<int>(mcrl2_lts->num_action_labels());
}

size_t LTS::getNumParameters() const
{
  return mcrl2_lts->process_parameters().size();
}

string LTS::getLabel(int labindex)
{
  return mcrl2_lts->action_label(labindex);
}

void LTS::addCluster(Cluster* cluster)
{
  size_t rank = cluster->getRank();
  size_t pos = cluster->getPositionInRank();

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
    size_t state_id = state->getID();
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
  if (cluster != nullptr)
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
  for (size_t i = 0; i < mcrl2_lts->num_action_labels(); ++i)
  {
    ls.push_back(mcrl2_lts->action_label(i));
  }
}

State* LTS::getInitialState() const
{
  return initialState;
}

int LTS::getNumRanks() const
{
  int offset = 0;
  while (clustersInRank[offset].empty())
  {
    ++offset;
  }
  return static_cast<int>(clustersInRank.size()) - offset;
}

int LTS::getMaxRanks() const
{
  return static_cast<int>(clustersInRank.size());
}

int LTS::getNumClusters() const
{
  int result = 0;
  for (const auto & i : clustersInRank)
  {
    result += static_cast<int>(i.size());
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
  return static_cast<int>(mcrl2_lts->num_states());
}

int LTS::getNumTransitions() const
{
  return static_cast<int>(mcrl2_lts->num_transitions());
}

void LTS::clearRanksAndClusters()
{
  vector< State* >::iterator it;
  for (it = states.begin(); it != states.end(); ++it)
  {
    (*it)->setRank(-1);
    (*it)->setCluster(nullptr);
  }

  for (Cluster_iterator ci = getClusterIterator(); !ci.is_end(); ++ci)
  {
    delete *ci;
  }

  vector< vector< Cluster* > > temp2;
  clustersInRank.swap(temp2);
}

void LTS::rankStates(bool cyclic)
{
  clearRanksAndClusters();
  int rankNumber = 0;

  vector< State* > nextRank,currRank;
  currRank.push_back(initialState);
  initialState->setRank(rankNumber);

  int i;
  vector< State* >::iterator it;
  State* s,*t;
  while (!currRank.empty())
  {
    nextRank.clear();
    // iterate over the states in this rank
    for (it  = currRank.begin(); it != currRank.end(); ++it)
    {
      s = *it;
      if (cyclic)
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

void LTS::clusterStates(bool cyclic)
{
  auto* d = new Cluster(0);
  vector< Cluster* > cs;
  cs.push_back(d);
  d->setPositionInRank(0);
  clustersInRank.push_back(cs);
  clusterTree(initialState,d,cyclic);
}

void LTS::clusterTree(State* v,Cluster* c,bool cyclic)
{
  int h,i,j,r;
  State* w, *y;
  c->addState(v);
  v->setCluster(c);
  for (i = 0; i < v->getNumOutTransitions(); ++i)
  {
    w = v->getOutTransition(i)->getEndState();
    if (w->getCluster() == nullptr && w->getRank() == v->getRank())
    {
      clusterTree(w,c,cyclic);
    }
  }
  for (i = 0; i < v->getNumInTransitions(); ++i)
  {
    w = v->getInTransition(i)->getBeginState();
    if (w->getCluster() == nullptr && w->getRank() == v->getRank())
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
      if (w->getCluster() == nullptr && r == v->getRank()+1)
      {
        auto* d = new Cluster(r);
        if (static_cast<size_t>(r) >= clustersInRank.size())
        {
          vector< Cluster* > cs;
          clustersInRank.push_back(cs);
        }
        d->setPositionInRank(static_cast<int>(clustersInRank[r].size()));
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
            if (w->getCluster() == nullptr && w->getRank() == v->getRank())
            {
              clusterTree(w,c,cyclic);
            }
          }
          for (j = 0; j < y->getNumInTransitions(); ++j)
          {
            w = y->getInTransition(j)->getBeginState();
            if (w->getCluster() == nullptr && w->getRank() == v->getRank())
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
    if (w->getCluster() == nullptr && r == v->getRank()+1)
    {
      auto* d = new Cluster(r);
      if (static_cast<size_t>(r) >= clustersInRank.size())
      {
        vector< Cluster* > cs;
        clustersInRank.push_back(cs);
      }
      d->setPositionInRank(static_cast<int>(clustersInRank[r].size()));
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
            if (w->getCluster() == nullptr && w->getRank() == v->getRank())
            {
              clusterTree(w,c,cyclic);
            }
          }
        }
        for (j = 0; j < y->getNumInTransitions(); ++j)
        {
          w = y->getInTransition(j)->getBeginState();
          if (w->getCluster() == nullptr && w->getRank() == v->getRank())
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

void LTS::positionStates(bool multiPass)
{
  StatePositioner* state_positioner = nullptr;
  if (multiPass)
  {
    state_positioner = new FSMStatePositioner(this);
  }
  else
  {
    state_positioner = new SinglePassStatePositioner(this);
  }
  state_positioner->positionStates();
  delete state_positioner;
}

LTS* LTS::zoomIntoAbove(Cluster *target)
{
  return new LTS(this, target, true);
}

LTS* LTS::zoomIntoBelow(Cluster *target)
{
  return new LTS(this, target, false);
}

LTS* LTS::zoomOut()
{
  if (previousLevel != nullptr)
  {
    if (lastWasAbove)
    {
      Cluster* child = nullptr;
      Cluster* parent = lastCluster;
      do
      {
        for (int i = 0; i < parent->getNumDescendants(); ++i)
        {
          parent->healSeverance(i);
        }
        child = parent;
        parent = child->getAncestor();
      }
      while (child != initialState->getCluster());
    }
    vector< State* >::iterator li;
    for (li = states.begin(); li != states.end(); ++li)
    {
      if (*li != nullptr)
      {
        (*li)->setZoomLevel(zoomLevel - 1);
      }
    }
    return previousLevel;
  }
  
  
    return this;
  
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

int LTS::getZoomLevel() const
{
  return zoomLevel;
}

void LTS::setZoomLevel(const int level)
{
  zoomLevel = level;
}
