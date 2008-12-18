// Author(s): Carst Tankink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file graph.cpp
/// \brief Implementation of graph class

#include "graph.h"
Graph::Graph()
{
  initialState = NULL;

  initial = -1;
  numStates = 0;
  numTrans = 0;
  numLabels = 0;
};

Graph::~Graph()
{
  for(size_t i = 0; i < states.size(); ++i)
  {
    delete states[i];
  }
}

void Graph::addState(State* s)
{
  states.push_back(s);
}

void Graph::setInitialState(State* i)
{
  initialState = i;
}

State* Graph::getInitialState() const
{
  return initialState;
}

size_t Graph::getNumberOfStates()
{
  return states.size();
}


State* Graph::getState(size_t i) const
{
  return states[i];
}

State* Graph::selectState(size_t s)
{
  if (s < states.size())
  {
    State* ss = states[s];
    ss->select();
    return ss;
  }
  else
  {
    return NULL;
  }
}

void Graph::colourState(size_t s, wxColour colour) {
  if(s < states.size())
  {
    states[s]->setColour(colour);
  }
}


void Graph::setInfo(int is, int ns, int nt, int nl)
{
  initial = is;
  numStates = ns;
  numTrans = nt;
  numLabels = nl;
}


int Graph::getInitial() const
{
  return initial;
}

int Graph::getNumStates() const
{
  return numStates;
}

int Graph::getNumTrans() const
{
  return numTrans;
}

int Graph::getNumLabels() const
{
  return numLabels;
}

Transition* Graph::selectTransition(size_t state, size_t transition)
{
  Transition* t = NULL;
  if (state < states.size())
  {
    State* ss = states[state];

    if (transition < ss->getNumberOfTransitions())
    {
      t = ss->getTransition(transition);
      t->select();
    }
  }

  return t;
}

Transition* Graph::selectSelfLoop(size_t state, size_t transition)
{
  Transition* t = NULL;
  if (state < states.size())
  {
    State* ss = states[state];

    if (transition < ss->getNumberOfSelfLoops())
    {
      t = ss->getSelfLoop(transition);
      t->select();
    }
  }

  return t;
}
