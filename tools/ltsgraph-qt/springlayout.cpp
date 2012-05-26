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
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace Graph
{

  //
  // Utility functions
  //

  inline float frand(float min, float max)
  {
    return ((float)qrand() / RAND_MAX) * (max - min) + min;
  }

  inline float cube(float x)
  {
    return x * x * x;
  }

  inline void clip(float& f, float min, float max)
  {
    if (f < min) f = min;
    else if (f > max) f = max;
  }

  //
  // SpringLayout
  //

  SpringLayout::SpringLayout(Graph &graph)
    : m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f), m_natLength(50.0f), m_controlPointWeight(0.001f),
      m_clipMin(Coord3D(0.0f, 0.0f, 0.0f)), m_clipMax(Coord3D(1000.0f, 1000.0f, 1000.0f)),
      m_graph(graph), m_ui(NULL), m_forceCalculation(&SpringLayout::forceLTSGraph)
  {
    srand(time(NULL));
  }

  SpringLayout::~SpringLayout()
  {
    delete m_ui;
  }

  SpringLayoutUi* SpringLayout::ui(QWidget* parent)
  {
    if (m_ui == NULL)
      m_ui = new SpringLayoutUi(*this, parent);
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
      return ltsgraph;
    return linearsprings;
  }

  Coord3D SpringLayout::forceLTSGraph(const Coord3D& a, const Coord3D& b, float ideal)
  {
    Coord3D diff = (a - b);
    float dist = (std::max)(diff.size(), 1.0f);
    float factor = m_attraction * 10000 * log(dist / (ideal + 1.0f)) / dist;
    return diff * factor;
  }

  Coord3D SpringLayout::forceLinearSprings(const Coord3D& a, const Coord3D& b, float ideal)
  {
    Coord3D diff = (a - b);
    float dist = diff.size() - ideal;
    float factor = (std::max)(dist, 0.0f) * m_attraction;
    // Let springs attract really strong near their equilibrium
    if (dist > 0.0f)
    {
      factor = (std::max)(factor, 100 * m_attraction / (std::max)(dist * dist / 10000.0f, 0.1f));
    }
    return diff *  factor;
  }

  inline
  Coord3D repulsionForce(const Coord3D& a, const Coord3D& b, float repulsion, float natlength)
  {
    Coord3D diff = a - b;
    float r = repulsion;
    r /= cube((std::max)(diff.size() / 2.0f, natlength / 10));
    diff = diff * r + Coord3D(frand(-0.01f, 0.01f), frand(-0.01f, 0.01f), frand(-0.01f, 0.01f));
    return diff;
  }

  void SpringLayout::apply()
  {
    m_nforces.resize(m_graph.nodeCount());
    m_hforces.resize(m_graph.edgeCount());
    m_lforces.resize(m_graph.edgeCount());
    m_sforces.resize(m_graph.edgeCount());

    for (size_t n = 0; n < m_graph.nodeCount(); ++n)
    {
      m_nforces[n] = Coord3D(0, 0, 0);
      for (size_t m = 0; m < n; ++m)
      {
        Coord3D diff = repulsionForce(m_graph.node(n).pos, m_graph.node(m).pos, m_repulsion, m_natLength);
        m_nforces[n] += diff;
        m_nforces[m] -= diff;
      }
      m_sforces[n] = (this->*m_forceCalculation)(m_graph.node(n).pos, m_graph.stateLabel(n).pos, 0.0);
    }

    for (size_t n = 0; n < m_graph.edgeCount(); ++n)
    {
      Edge e = m_graph.edge(n);
      Coord3D f;
      // Variables for repulsion calculations

      m_hforces[n] = Coord3D(0, 0, 0);
      m_lforces[n] = Coord3D(0, 0, 0);

      if (e.from == e.to)
      {
        m_hforces[n] += repulsionForce(m_graph.handle(n).pos, m_graph.node(e.from).pos, m_repulsion, m_natLength);
      }

      f = (this->*m_forceCalculation)(m_graph.node(e.to).pos, m_graph.node(e.from).pos, m_natLength);
      m_nforces[e.from] += f;
      m_nforces[e.to] -= f;

      f = (this->*m_forceCalculation)((m_graph.node(e.to).pos + m_graph.node(e.from).pos) / 2.0, m_graph.handle(n).pos, 0.0);
      m_hforces[n] += f;

      f = (this->*m_forceCalculation)(m_graph.handle(n).pos, m_graph.transitionLabel(n).pos, 0.0);
      m_lforces[n] += f;

      for (size_t m = 0; m < n; ++m)
      {
        // Handles
        f = repulsionForce(m_graph.handle(n).pos, m_graph.handle(m).pos, m_repulsion * m_controlPointWeight, m_natLength);
        m_hforces[n] += f;
        m_hforces[m] -= f;

        // Labels
        f = repulsionForce(m_graph.transitionLabel(n).pos, m_graph.transitionLabel(m).pos, m_repulsion * m_controlPointWeight, m_natLength);
        m_lforces[n] += f;
        m_lforces[m] -= f;
      }
    }

    for (size_t n = 0; n < m_graph.nodeCount(); ++n)
    {
      if (!m_graph.node(n).anchored)
      {
        m_graph.node(n).pos = m_graph.node(n).pos + m_nforces[n] * m_speed;
        m_graph.node(n).pos.clip(m_clipMin, m_clipMax);
      }
      if (!m_graph.stateLabel(n).anchored)
      {
        m_graph.stateLabel(n).pos = m_graph.stateLabel(n).pos + m_sforces[n] * m_speed;
        m_graph.stateLabel(n).pos.clip(m_clipMin, m_clipMax);
      }
    }

    for (size_t n = 0; n < m_graph.edgeCount(); ++n)
    {
      if (!m_graph.handle(n).anchored)
      {
        m_graph.handle(n).pos = m_graph.handle(n).pos + m_hforces[n] * m_speed;
        m_graph.handle(n).pos.clip(m_clipMin, m_clipMax);
      }
      if (!m_graph.transitionLabel(n).anchored)
      {
        m_graph.transitionLabel(n).pos = m_graph.transitionLabel(n).pos + m_lforces[n] * m_speed;
        m_graph.transitionLabel(n).pos.clip(m_clipMin, m_clipMax);
      }
    }
  }

  void SpringLayout::setClipRegion(const Coord3D& min, const Coord3D& max)
  {
    if (min.z < m_clipMin.z || max.z > m_clipMax.z) //Depth is increased, add random z values to improve spring movement in z direction
    {
      float change = (std::min)(m_clipMin.z-min.z, max.z-m_clipMax.z)/100.0f; //Add at most 1/100th of the change
      for (size_t n = 0; n < m_graph.nodeCount(); ++n)
      {
        if (!m_graph.node(n).anchored)
        {
          m_graph.node(n).pos.z = m_graph.node(n).pos.z + frand(-change, change);
        }

      }
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
      SpringLayout &m_layout;
      int m_period;
    public:
      WorkerThread(SpringLayout &layout, int period=50, QObject* parent=0)
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

      virtual void run()
      {
        m_time.start();
        int elapsed;
        while (!m_stopped)
        {
          m_layout.apply();
          elapsed = m_time.elapsed();
          m_time.restart();
          if (m_period > elapsed)
            msleep(m_period - elapsed);
        }
      }
  };

  SpringLayoutUi::SpringLayoutUi(SpringLayout &layout, QWidget *parent)
    : QDockWidget(parent), m_layout(layout), m_ui(new Ui::DockWidgetLayout), m_thread(NULL)
  {
    m_ui->setupUi(this);
    m_ui->sldAttraction->setValue(m_layout.attraction());
    m_ui->sldRepulsion->setValue(m_layout.repulsion());
    m_ui->sldSpeed->setValue(m_layout.speed());
    m_ui->sldHandleWeight->setValue(m_layout.controlPointWeight());
    m_ui->sldNatLength->setValue(m_layout.naturalTransitionLength());
    m_ui->cmbForceCalculation->setCurrentIndex(m_layout.forceCalculation());
  }

  SpringLayoutUi::~SpringLayoutUi()
  {
    if (m_thread != NULL)
    {
      static_cast<WorkerThread*>(m_thread)->stop();
      m_thread->wait();
    }
    delete m_ui;
  }

  void SpringLayoutUi::onAttractionChanged(int value)
  {
    m_layout.setAttraction(value);
    m_ui->lblAttraction->setText(QString("Attraction (%0)").arg(m_layout.attraction()));
  }

  void SpringLayoutUi::onRepulsionChanged(int value)
  {
    m_layout.setRepulsion(value);
    m_ui->lblRepulsion->setText(QString("Repulsion (%0)").arg(m_layout.repulsion()));
  }

  void SpringLayoutUi::onSpeedChanged(int value)
  {
    if (m_thread != NULL)
      static_cast<WorkerThread*>(m_thread)->setPeriod(100 - value);
  }

  void SpringLayoutUi::onHandleWeightChanged(int value)
  {
    m_layout.setControlPointWeight(value);
  }

  void SpringLayoutUi::onNatLengthChanged(int value)
  {
    m_layout.setNaturalTransitionLength(value);
    m_ui->lblRepulsion->setText(QString("Repulsion (%0)").arg(m_layout.repulsion()));
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
    m_ui->btnStartStop->setText("Stop");
    m_ui->btnStartStop->setEnabled(true);
  }

  void SpringLayoutUi::onStopped()
  {
    m_ui->btnStartStop->setText("Start");
    m_ui->btnStartStop->setEnabled(true);
    emit runningChanged(false);
  }

  void SpringLayoutUi::onStartStop()
  {
    m_ui->btnStartStop->setEnabled(false);
    if (m_thread == NULL)
    {
      emit runningChanged(true);
      m_thread = new WorkerThread(m_layout, 100 - m_ui->sldSpeed->value(), this);
      m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
      m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
      m_thread->start();
    }
    else
    {
      static_cast<WorkerThread*>(m_thread)->stop();
      m_thread->wait();
      m_thread = NULL;
    }
  }

  void SpringLayoutUi::setActive(bool active)
  {
    if (active && m_thread == NULL)
      onStartStop();
    else
      if (!active && m_thread != NULL)
        onStartStop();
  }

}
