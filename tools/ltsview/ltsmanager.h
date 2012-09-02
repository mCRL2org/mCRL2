// Author(s): Bas Ploeger, Carst Tankink, Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LTSMANAGER_H
#define LTSMANAGER_H

#include <QObject>
#include <QString>

#include "lts.h"
#include "settings.h"
#include "simulation.h"

class LtsManager : public QObject
{
  Q_OBJECT

  public:
    LtsManager(QObject *parent, Settings *settings);
    LTS *lts() const { return m_lts; }

    State *selectedState() const { return m_selectedState; }
    Cluster *selectedCluster() const { return m_selectedCluster; }

    Simulation *simulation() const { return m_simulation; }
    bool simulationActive() const { return m_simulation && m_simulation->isStarted(); }
    State *currentSimulationState() const;
    Transition *currentSimulationTransition() const;
    QList<Transition *> simulationAvailableTransitions() const;
    QList<State *> simulationStateHistory() { return m_simulationStateHistory; }
    QList<Transition *> simulationTransitionHistory() { return m_simulationTransitionHistory; }

  public slots:
    bool openLts(QString filename);
    void zoomInBelow(Cluster *cluster);
    void zoomInBelow() { if (selectedCluster()) zoomInBelow(selectedCluster()); }
    void zoomInAbove(Cluster *cluster);
    void zoomInAbove() { if (selectedCluster()) zoomInAbove(selectedCluster()); }
    bool zoomOut();
    void unselect();
    void selectState(State *state);
    void selectCluster(Cluster *cluster);
    void simulateState(State *state);
    void loadTrace(QString filename) { if (m_simulation) m_simulation->loadTrace(filename); }

  protected slots:
    void clusterStates();
    void positionClusters();
    void positionStates();
    void clusterStates(LTS *lts);
    void positionClusters(LTS *lts);
    void positionStates(LTS *lts);
    void updateSimulationHistory();
    void unselectNonsimilated();

  protected:
    Transition *transitionTo(State *state);

  signals:
    void loadingLts();
    void errorLoadingLts();
    void rankingStates();
    void clusteringStates();
    void computingClusterInfo();
    void positioningClusters();
    void positioningStates();
    void ltsStructured();

    void ltsChanged(LTS *lts);
    void clustersChanged();
    void clusterPositionsChanged();
    void statePositionsChanged();
    void ltsZoomed(LTS *lts);

    void selectionChanged();
    void simulationChanged();

  public:
    Settings *m_settings;
    LTS *m_lts;
    Simulation *m_simulation;
    State *m_selectedState;
    Cluster *m_selectedCluster;
    QList<State *> m_simulationStateHistory;
    QList<Transition *> m_simulationTransitionHistory;

};

#endif
