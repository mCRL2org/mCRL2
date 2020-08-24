// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "springlayout.h"
#include "utility.h"

#include <QThread>
#include <cstdlib>

namespace Graph
{
// the maximum distance a node may move in one iteration
static const float MAX_DISPLACEMENT = 10.0f;

//
// Utility functions
//

inline float cube(float x)
{
  return x * x * x;
}

inline void clip(float& f, float min, float max)
{
  if (f < min)
  {
    f = min;
  }
  else if (f > max)
  {
    f = max;
  }
}

//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
    : m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f), m_natLength(50.0f), m_controlPointWeight(0.001f),
    m_graph(graph), m_ui(nullptr), m_forceCalculation(&SpringLayout::forceLTSGraph), m_glwidget(glwidget)
{
  srand(time(nullptr));
}

SpringLayout::~SpringLayout()
{
  delete m_ui;
}

SpringLayoutUi* SpringLayout::ui(QWidget* parent)
{
  if (m_ui == nullptr)
  {
    m_ui = new SpringLayoutUi(*this, parent);
  }
  return m_ui;
}

void SpringLayout::setForceCalculation(ForceCalculation c)
{
  switch (c)
  {
    case ltsgraph:
      m_forceCalculation = &SpringLayout::forceLTSGraph;
      break;
    case linearsprings:
      m_forceCalculation = &SpringLayout::forceLinearSprings;
      break;
  }
}

SpringLayout::ForceCalculation SpringLayout::forceCalculation()
{
  if (m_forceCalculation == &SpringLayout::forceLTSGraph)
  {
    return ltsgraph;
  }
  return linearsprings;
}

QVector3D SpringLayout::forceLTSGraph(const QVector3D& a, const QVector3D& b, float ideal)
{
  QVector3D diff = (a - b);
  float dist = (std::max)(diff.length(), 1.0f);
  float factor = m_attraction * 10000 * std::log(dist / (ideal + 1.0f)) / dist;
  return diff * factor;
}

QVector3D SpringLayout::forceLinearSprings(const QVector3D& a, const QVector3D& b, float ideal)
{
  QVector3D diff = (a - b);
  float dist = diff.length() - ideal;
  float factor = (std::max)(dist, 0.0f) * m_attraction;
  // Let springs attract really strong near their equilibrium
  if (dist > 0.0f)
  {
    factor = (std::max)(factor, 100 * m_attraction / (std::max)(dist * dist / 10000.0f, 0.1f));
  }
  return diff * factor;
}

inline QVector3D repulsionForce(const QVector3D& a, const QVector3D& b, float repulsion, float natlength)
{
  QVector3D diff = a - b;
  float dist = diff.length();

  if (dist < natlength / 1000)
  {
    float natFraction = natlength / 10000;
    return QVector3D(
        fast_frand(-natFraction, natFraction),
        fast_frand(-natFraction, natFraction),
        fast_frand(-natFraction, natFraction)
    );
  } else
  {
    float r = repulsion / cube((std::max)(dist / 2.0f, natlength / 10));
    return diff * r;
  }
}

static QVector3D applyForce(const QVector3D& pos, const QVector3D& force, float speed)
{
  QVector3D displacement = speed * force;

  // limit displacement in one iteration
  if (displacement.length() > MAX_DISPLACEMENT)
  {
    displacement.normalize();
    displacement *= MAX_DISPLACEMENT;
  }

  return pos + displacement;
}

void SpringLayout::apply()
{
  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  if (!m_graph.stable())
  {
    bool sel = m_graph.hasExploration();
    std::size_t nodeCount = sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
    std::size_t edgeCount = sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();

    m_nforces.resize(m_graph.nodeCount()); // Todo: compact this
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.nodeCount());

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      m_nforces[n] = QVector3D(0, 0, 0);
      for (std::size_t j = 0; j < i; ++j)
      {
        std::size_t m = sel ? m_graph.explorationNode(j) : j;

        QVector3D diff = repulsionForce(m_graph.node(n).pos(), m_graph.node(m).pos(), m_repulsion, m_natLength);
        m_nforces[n] += diff;
        m_nforces[m] -= diff;
      }
      m_sforces[n] = (this->*m_forceCalculation)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0);

      // calculate repulsion between all edges of this node.
      // we ignore exploration, causing a small overhead when we check against invisible edges.
      std::size_t nrOfNeighbours = m_graph.nrOfNeighboursOfNode(n);
      for (int j = 0; j < nrOfNeighbours; ++j)
      {
        for (int k = j + 1; k < nrOfNeighbours; ++k)
        {
          QVector3D f;
          std::size_t first = m_graph.edgeOfNode(n, j);
          std::size_t second = m_graph.edgeOfNode(n, k);

          // Handles
          const QVector3D& firstHPos = m_graph.handle(first).pos();
          const QVector3D& secondHPos = m_graph.handle(second).pos();
          f = repulsionForce(firstHPos, secondHPos, m_repulsion * m_controlPointWeight, m_natLength);
          m_hforces[first] += f;
          m_hforces[second] -= f;

          // Labels
          const QVector3D& firstLPos = m_graph.transitionLabel(first).pos();
          const QVector3D& secondLPos = m_graph.transitionLabel(second).pos();
          f = repulsionForce(firstLPos, secondLPos, m_repulsion * m_controlPointWeight, m_natLength);
          m_lforces[first] += f;
          m_lforces[second] -= f;
        }
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      QVector3D f;
      Edge e = m_graph.edge(n);
      Node& handle = m_graph.handle(n);
      NodeNode& node_from = m_graph.node(e.from());
      NodeNode& node_to = m_graph.node(e.to());

      // Variables for repulsion calculations
      m_hforces[n] = QVector3D(0, 0, 0);
      m_lforces[n] = QVector3D(0, 0, 0);

      if (e.is_selfloop())
      {
        m_hforces[n] += repulsionForce(handle.pos(), node_from.pos(), m_repulsion, m_natLength);
      }

      f = (this->*m_forceCalculation)(node_to.pos(), node_from.pos(), m_natLength);
      m_nforces[e.from()] += f;
      m_nforces[e.to()] -= f;

      f = (this->*m_forceCalculation)((node_to.pos() + node_from.pos()) / 2.0, handle.pos(), 0.0);
      m_hforces[n] += f;

      f = (this->*m_forceCalculation)(handle.pos(), m_graph.transitionLabel(n).pos(), 0.0);
      m_lforces[n] += f;
    }

    QVector3D clipmin = m_graph.getClipMin();
    QVector3D clipmax = m_graph.getClipMax();
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.node(n).anchored())
      {
        m_graph.node(n).pos_mutable() = applyForce(m_graph.node(n).pos(), m_nforces[n], m_speed);
        clipVector(m_graph.node(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.stateLabel(n).anchored())
      {
        m_graph.stateLabel(n).pos_mutable() = applyForce(m_graph.stateLabel(n).pos(), m_sforces[n], m_speed);
        clipVector(m_graph.stateLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        m_graph.handle(n).pos_mutable() = applyForce(m_graph.handle(n).pos(), m_hforces[n], m_speed);
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        m_graph.transitionLabel(n).pos_mutable() = applyForce(m_graph.transitionLabel(n).pos(), m_lforces[n], m_speed);
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
  }

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
}

void SpringLayout::randomizeZ(float z)
{
  m_graph.lock(GRAPH_LOCK_TRACE);
  for (std::size_t n = 0; n < m_graph.nodeCount(); ++n)
  {
    if (!m_graph.node(n).anchored())
    {
      m_graph.node(n).pos_mutable().setZ(m_graph.node(n).pos().z() + fast_frand(-z, z));
    }
  }
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

//
// SpringLayoutUi
//

class WorkerThread : public QThread
{
private:
  bool m_stopped;
  QElapsedTimer m_time;
  SpringLayout& m_layout;
  int m_period;

public:
  WorkerThread(SpringLayout& layout, int period, QObject* parent = nullptr)
      : QThread(parent), m_stopped(false), m_layout(layout), m_period(period)
  {}

  void stop()
  {
    m_stopped = true;
  }

  void setPeriod(int period)
  {
      m_period = period;
    }

    int period() const
    {
      return m_period;
    }

    void run() override
    {
      m_time.start();
      int elapsed;
      while (!m_stopped)
      {
        m_layout.apply();
        elapsed = m_time.elapsed();
        m_time.restart();
        if (m_period > elapsed)
        {
          msleep(m_period - elapsed);
        }
      }
      m_layout.m_glwidget.update();
    }
};

SpringLayoutUi::SpringLayoutUi(SpringLayout& layout, QWidget* parent)
    : QDockWidget(parent), m_layout(layout), m_thread(nullptr)
{
  m_ui.setupUi(this);
  m_ui.sldAttraction->setValue(m_layout.attraction());
  m_ui.sldRepulsion->setValue(m_layout.repulsion());
  m_ui.sldSpeed->setValue(m_layout.speed());
  m_ui.sldHandleWeight->setValue(m_layout.controlPointWeight());
  m_ui.sldNatLength->setValue(m_layout.naturalTransitionLength());
  m_ui.cmbForceCalculation->setCurrentIndex(m_layout.forceCalculation());
  connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

SpringLayoutUi::~SpringLayoutUi()
{
  if (m_thread != nullptr)
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
  }
}

QByteArray SpringLayoutUi::settings()
{
  QByteArray result;
  QDataStream out(&result, QIODevice::WriteOnly);

  out << quint32(m_ui.sldAttraction->value()) <<
      quint32(m_ui.sldRepulsion->value()) <<
      quint32(m_ui.sldSpeed->value()) <<
      quint32(m_ui.sldHandleWeight->value()) <<
      quint32(m_ui.sldNatLength->value()) <<
      quint32(m_ui.cmbForceCalculation->currentIndex());

  return result;
}

void SpringLayoutUi::setSettings(QByteArray state)
{
  if (state.isEmpty())
  {
    return;
  }

  QDataStream in(&state, QIODevice::ReadOnly);

  quint32 attraction, repulsion, speed, handleWeight, NatLength, ForceCalculation;
  in >> attraction >> repulsion >> speed >> handleWeight >> NatLength >> ForceCalculation;

  if (in.status() == QDataStream::Ok)
  {
    m_ui.sldAttraction->setValue(attraction);
    m_ui.sldRepulsion->setValue(repulsion);
    m_ui.sldSpeed->setValue(speed);
    m_ui.sldHandleWeight->setValue(handleWeight);
    m_ui.sldNatLength->setValue(NatLength);
    m_ui.cmbForceCalculation->setCurrentIndex(ForceCalculation);
  }

}

void SpringLayoutUi::onAttractionChanged(int value)
{
  m_layout.setAttraction(value);
}

void SpringLayoutUi::onRepulsionChanged(int value)
{
  m_layout.setRepulsion(value);
}

void SpringLayoutUi::onSpeedChanged(int value)
{
//        if (m_thread != nullptr)
//        {
//            dynamic_cast<WorkerThread*>(m_thread)->setPeriod(100 - value);
//        }
  m_layout.setSpeed(value);
}

void SpringLayoutUi::onHandleWeightChanged(int value)
{
  m_layout.setControlPointWeight(value);
}

void SpringLayoutUi::onNatLengthChanged(int value)
{
  m_layout.setNaturalTransitionLength(value);
}

void SpringLayoutUi::onForceCalculationChanged(int value)
{
  switch (value)
  {
    case 0:
      m_layout.setForceCalculation(SpringLayout::ltsgraph);
      break;
    case 1:
      m_layout.setForceCalculation(SpringLayout::linearsprings);
      break;
  }
}

void SpringLayoutUi::onStarted()
{
  m_ui.btnStartStop->setText("Stop");
  m_ui.btnStartStop->setEnabled(true);
}

void SpringLayoutUi::onStopped()
{
  m_ui.btnStartStop->setText("Start");
  m_ui.btnStartStop->setEnabled(true);
  emit runningChanged(false);
}

void SpringLayoutUi::onStartStop()
{
  m_ui.btnStartStop->setEnabled(false);
  if (m_thread == nullptr)
  {
    emit runningChanged(true);
    m_updateTimer.start(40);
    m_thread = new WorkerThread(m_layout, 100 - m_ui.sldSpeed->value(), this);
    m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
    m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
    m_thread->start();
  }
  else
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
    m_updateTimer.stop();
    m_thread = nullptr;
  }
}

void SpringLayoutUi::onTimeout()
{
  m_layout.m_glwidget.update();
}

void SpringLayoutUi::setActive(bool active)
{
  if (active && m_thread == nullptr)
  {
    onStartStop();
  }
  else if (!active && m_thread != nullptr)
  {
    onStartStop();
  }
}

}  // namespace Graph
