// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "springlayout.h"
#include "ui_springlayout.h"
#include <QThread>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "mcrl2/gui/arcball.h"

namespace Graph
{

//
// Utility functions
//

inline float frand(float min, float max)
{
  //return ((float)qrand() / RAND_MAX) * (max - min) + min;
  // Fast pseudo rand, source: http://www.musicdsp.org/showone.php?id=273
  static int32_t seed = 1;
  seed *= 16807;
  return ((((float)seed) * 4.6566129e-010f) + 1.0) * (max - min) / 2.0 + min;
}

inline float cube(float x)
{
  return x * x * x;
}

inline void clip(float& f, float min, float max)
{
  if (f < min) {
    f = min;
  }
  else if (f > max) {
    f = max;
  }
}

//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
  : m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f), m_natLength(50.0f), m_controlPointWeight(0.001f),
    m_clipMin(QVector3D(0.0f, 0.0f, 0.0f)), m_clipMax(QVector3D(1000.0f, 1000.0f, 1000.0f)),
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
  if (m_ui == nullptr) {
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
  if (m_forceCalculation == &SpringLayout::forceLTSGraph) {
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
  return diff *  factor;
}

inline
QVector3D repulsionForce(const QVector3D& a, const QVector3D& b, float repulsion, float natlength)
{
  QVector3D diff = a - b;
  float r = repulsion;
  r /= cube((std::max)(diff.length() / 2.0f, natlength / 10));
  diff = diff * r + QVector3D(frand(-0.01f, 0.01f), frand(-0.01f, 0.01f), frand(-0.01f, 0.01f));
  return diff;
}

static QVector3D applyForce(const QVector3D& pos, const QVector3D& force, float speed)
{
  return pos + speed * force;
}

void SpringLayout::apply()
{
  m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section

  if (!m_graph.stable())
  {
    bool sel = m_graph.hasSelection();
    size_t nodeCount = sel ? m_graph.selectionNodeCount() : m_graph.nodeCount();
    size_t edgeCount = sel ? m_graph.selectionEdgeCount() : m_graph.edgeCount();

    m_nforces.resize(m_graph.nodeCount()); // Todo: compact this
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.nodeCount());

    for (size_t i = 0; i < nodeCount; ++i)
    {
      size_t n = sel ? m_graph.selectionNode(i) : i;

      m_nforces[n] = QVector3D(0, 0, 0);
      for (size_t j = 0; j < i; ++j)
      {
        size_t m = sel ? m_graph.selectionNode(j) : j;

        QVector3D diff = repulsionForce(m_graph.node(n).pos(), m_graph.node(m).pos(), m_repulsion, m_natLength);
        m_nforces[n] += diff;
        m_nforces[m] -= diff;
      }
      m_sforces[n] = (this->*m_forceCalculation)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0);
    }

    for (size_t i = 0; i < edgeCount; ++i)
    {
      size_t n = sel ? m_graph.selectionEdge(i) : i;

      Edge e = m_graph.edge(n);
      QVector3D f;
      // Variables for repulsion calculations

      m_hforces[n] = QVector3D(0, 0, 0);
      m_lforces[n] = QVector3D(0, 0, 0);

      if (e.from() == e.to())
      {
        m_hforces[n] += repulsionForce(m_graph.handle(n).pos(), m_graph.node(e.from()).pos(), m_repulsion, m_natLength);
      }

      f = (this->*m_forceCalculation)(m_graph.node(e.to()).pos(), m_graph.node(e.from()).pos(), m_natLength);
      m_nforces[e.from()] += f;
      m_nforces[e.to()] -= f;

      f = (this->*m_forceCalculation)((m_graph.node(e.to()).pos() + m_graph.node(e.from()).pos()) / 2.0, m_graph.handle(n).pos(), 0.0);
      m_hforces[n] += f;

      f = (this->*m_forceCalculation)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(), 0.0);
      m_lforces[n] += f;

      for (size_t j = 0; j < i; ++j)
      {
        size_t m = sel ? m_graph.selectionEdge(j) : j;

        // Handles
        f = repulsionForce(m_graph.handle(n).pos(), m_graph.handle(m).pos(), m_repulsion * m_controlPointWeight, m_natLength);
        m_hforces[n] += f;
        m_hforces[m] -= f;

        // Labels
        f = repulsionForce(m_graph.transitionLabel(n).pos(), m_graph.transitionLabel(m).pos(), m_repulsion * m_controlPointWeight, m_natLength);
        m_lforces[n] += f;
        m_lforces[m] -= f;
      }
    }

    for (size_t i = 0; i < nodeCount; ++i)
    {
      size_t n = sel ? m_graph.selectionNode(i) : i;

      if (!m_graph.node(n).anchored())
      {
        m_graph.node(n).pos_mutable() = applyForce(m_graph.node(n).pos(), m_nforces[n], m_speed);
        mcrl2::gui::clipVector(m_graph.node(n).pos_mutable(), m_clipMin, m_clipMax);
      }
      if (!m_graph.stateLabel(n).anchored())
      {
        m_graph.stateLabel(n).pos_mutable() = applyForce(m_graph.stateLabel(n).pos(), m_sforces[n], m_speed);
        mcrl2::gui::clipVector(m_graph.stateLabel(n).pos_mutable(), m_clipMin, m_clipMax);
      }
    }

    for (size_t i = 0; i < edgeCount; ++i)
    {
      size_t n = sel ? m_graph.selectionEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        m_graph.handle(n).pos_mutable() = applyForce(m_graph.handle(n).pos(), m_hforces[n], m_speed);
        mcrl2::gui::clipVector(m_graph.handle(n).pos_mutable(), m_clipMin, m_clipMax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        m_graph.transitionLabel(n).pos_mutable() = applyForce(m_graph.transitionLabel(n).pos(), m_lforces[n], m_speed);
        mcrl2::gui::clipVector(m_graph.transitionLabel(n).pos_mutable(), m_clipMin, m_clipMax);
      }
    }
  }

  m_graph.unlock(GRAPH_LOCK_TRACE); // exit critical section
}

void SpringLayout::setClipRegion(const QVector3D& min, const QVector3D& max, float zchange)
{
  if (min.z() < m_clipMin.z() || max.z() > m_clipMax.z()) //Depth is increased, add random z values to improve spring movement in z direction
  {
    m_graph.lock(GRAPH_LOCK_TRACE);
    for (size_t n = 0; n < m_graph.nodeCount(); ++n)
    {
      if (!m_graph.node(n).anchored())
      {
        m_graph.node(n).pos_mutable().setZ(m_graph.node(n).pos().z() + frand(-zchange, zchange));
      }
    }
    m_graph.unlock(GRAPH_LOCK_TRACE);
  }

  m_clipMin = min;
  m_clipMax = max;
}

//
// SpringLayoutUi
//

class WorkerThread : public QThread
{
  private:
    bool m_stopped;
    QTime m_time;
    SpringLayout& m_layout;
    int m_period;
  public:
    WorkerThread(SpringLayout& layout, int period, QObject* parent=nullptr)
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
  if (state.isEmpty()) {
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
  if (m_thread != nullptr) {
    dynamic_cast<WorkerThread*>(m_thread)->setPeriod(100 - value);
  }
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
  if (active && m_thread == nullptr) {
    onStartStop();
  }
  else if (!active && m_thread != nullptr) {
    onStartStop();
  }
}

}  // namespace Graph
