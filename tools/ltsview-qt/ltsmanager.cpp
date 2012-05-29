// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/utilities/logger.h"
#include "ltsmanager.h"
#include "simulation.h"
#include "state.h"
#include "transition.h"

LtsManager::LtsManager(Settings *settings):
  m_settings(settings),
  m_lts(0),
  m_simulation(0),
  m_selectedState(0),
  m_selectedCluster(0)
{
  connect(&settings->stateRankStyleCyclic, SIGNAL(changed(bool)), this, SLOT(clusterStates()));
  connect(&settings->fsmStyle, SIGNAL(changed(bool)), this, SLOT(positionClusters()));
  connect(&settings->statePosStyleMultiPass, SIGNAL(changed(bool)), this, SLOT(positionStates()));
}

State *LtsManager::currentSimulationState() const
{
  if (!m_simulation)
  {
    return 0;
  }
  else
  {
    return m_simulation->currentState();
  }
}

Transition *LtsManager::currentSimulationTransition() const
{
  if (!m_simulation)
  {
    return 0;
  }
  else
  {
    return m_simulation->currentTransition();
  }
}

QList<Transition *> LtsManager::simulationAvailableTransitions() const
{
  if (!m_simulation)
  {
    return QList<Transition *>();
  }
  else
  {
    return m_simulation->availableTransitions();
  }
}

bool LtsManager::openLts(QString filename)
{
  emit loadingLts();

  LTS *lts = new LTS();
  try
  {
    if (!lts->readFromFile(filename.toStdString()))
    {
      delete lts;
      emit errorLoadingLts();
      mCRL2log(mcrl2::log::error) << "Error loading file: " << filename.toStdString();
      return false;
    }
  }
  catch (mcrl2::runtime_error e)
  {
    delete lts;
    emit errorLoadingLts();
    mCRL2log(mcrl2::log::error) << e.what();
    return false;
  }

  clusterStates(lts);

  if (m_simulation)
  {
    delete m_simulation;
  }
  if (m_lts)
  {
    delete m_lts;
  }
  m_lts = lts;
  m_simulation = new Simulation(this, m_lts);

  connect(m_simulation, SIGNAL(changed()), this, SLOT(updateSimulationHistory()));
  connect(m_simulation, SIGNAL(changed()), this, SLOT(unselectNonsimilated()));
  connect(m_simulation, SIGNAL(changed()), this, SIGNAL(simulationChanged()));

  emit ltsChanged(lts);
  emit clustersChanged();
  emit clusterPositionsChanged();
  emit statePositionsChanged();
  emit simulationChanged();

  return true;
}

void LtsManager::zoomInBelow(Cluster *cluster)
{
  unselect();
  m_lts = m_lts->zoomIntoBelow(cluster);
  emit ltsZoomed(m_lts);
}

void LtsManager::zoomInAbove(Cluster *cluster)
{
  unselect();
  m_lts = m_lts->zoomIntoAbove(cluster);
  emit ltsZoomed(m_lts);
}

bool LtsManager::zoomOut()
{
  LTS *lts = m_lts;
  m_lts = lts->zoomOut();
  if (lts != m_lts)
  {
    delete lts;
    emit ltsZoomed(m_lts);
    return true;
  }
  else
  {
    return false;
  }
}

void LtsManager::unselect()
{
  if (m_lts && (m_selectedState || m_selectedCluster))
  {
    m_selectedState = 0;
    m_selectedCluster = 0;
    emit selectionChanged();
  }
}

void LtsManager::selectState(State *state)
{
  if (m_lts)
  {
    m_selectedState = state;
    if (m_simulation->isStarted())
    {
      Transition *transition = transitionTo(state);
      if (transition)
      {
        m_simulation->selectTransition(transition);
      }
    }
    else
    {
      m_simulation->setInitialState(state);
    }
    emit selectionChanged();
  }
}

void LtsManager::selectCluster(Cluster *cluster)
{
  if (m_lts)
  {
    m_selectedCluster = cluster;
    emit selectionChanged();
  }
}

void LtsManager::simulateState(State *state)
{
  if (m_simulation)
  {
    Transition *transition = transitionTo(state);
    if (transition)
    {
      m_simulation->followTransition(transition);
    }
  }
}

void LtsManager::clusterStates()
{
  if (m_lts)
  {
    while (zoomOut())
    {
      // do nothing
    }
    clusterStates(m_lts);
    emit clustersChanged();
    emit clusterPositionsChanged();
    emit statePositionsChanged();
  }
}

void LtsManager::positionClusters()
{
  if (m_lts)
  {
    positionClusters(m_lts);
    emit clusterPositionsChanged();
    emit statePositionsChanged();
  }
}

void LtsManager::positionStates()
{
  if (m_lts)
  {
    positionStates(m_lts);
    emit statePositionsChanged();
  }
}

void LtsManager::clusterStates(LTS *lts)
{
  bool cyclic = m_settings->stateRankStyleCyclic.value();
  emit rankingStates();
  lts->rankStates(cyclic);
  emit clusteringStates();
  lts->clusterStates(cyclic);
  emit computingClusterInfo();
  lts->computeClusterInfo();
  positionClusters(lts);
}

void LtsManager::positionClusters(LTS *lts)
{
  emit positioningClusters();
  lts->positionClusters(m_settings->fsmStyle.value());
  positionStates(lts);
}

void LtsManager::positionStates(LTS *lts)
{
  emit positioningStates();
  lts->positionStates(m_settings->statePosStyleMultiPass.value());
  emit ltsStructured();
}

void LtsManager::updateSimulationHistory()
{
  m_simulationStateHistory.clear();
  m_simulationTransitionHistory.clear();
  if (m_simulation)
  {
    QList<Transition *> history = m_simulation->history();
    for (int i = 0; i < history.size(); i++)
    {
      // TODO: this should only select states that are part of the current zoomed LTS!
      //if (history[i]->getBeginState()->
      if (true)
      {
        m_simulationStateHistory += history[i]->getBeginState();
        m_simulationTransitionHistory += history[i];
      }
    }
  }
}

void LtsManager::unselectNonsimilated()
{
  if (m_simulation->isStarted() == (transitionTo(selectedState()) == 0))
  {
    //unselect();
  }
}

Transition *LtsManager::transitionTo(State *state)
{
  QList<Transition *> transitions = m_simulation->availableTransitions();
  for (int i = 0; i < transitions.size(); i++)
  {
    if (transitions[i]->getEndState() == state)
    {
      return transitions[i];
    }
  }
  return 0;
}
