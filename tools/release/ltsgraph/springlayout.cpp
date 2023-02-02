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
#include "settingsmanager.h"

#include <QWidget>
#include <QThread>
#include <cstdlib>
#include <cmath>

#define DEV_DEBUG
#ifdef DEV_DEBUG
// #define LOG_FORCE_MAGNITUDE
// #define LOG_FUNCTION_SETTER
#endif

namespace Graph
{

//
// Debug functions
//

#ifdef DEV_DEBUG
std::string getName(SpringLayout::AttractionCalculation c)
{
  std::string s = "UNKNOWN";
#define PROC_VAL(p)                                                            \
  case (p):                                                                    \
    s = #p;                                                                    \
    break;
  switch (c)
  {
    PROC_VAL(SpringLayout::AttractionCalculation::ltsgraph_attr);
    PROC_VAL(SpringLayout::AttractionCalculation::electricalsprings_attr);
    PROC_VAL(SpringLayout::AttractionCalculation::linearsprings_attr);
  }
#undef PROC_VAL
  return s;
}
std::string getName(SpringLayout::RepulsionCalculation c)
{
  std::string s = "UNKNOWN";
#define PROC_VAL(p)                                                            \
  case (p):                                                                    \
    s = #p;                                                                    \
    break;
  switch (c)
  {
    PROC_VAL(SpringLayout::RepulsionCalculation::ltsgraph_rep);
    PROC_VAL(SpringLayout::RepulsionCalculation::electricalsprings_rep);
    PROC_VAL(SpringLayout::RepulsionCalculation::none_rep);
  }
#undef PROC_VAL
  return s;
}
std::string getName(SpringLayout::ForceApplication c)
{
  std::string s = "UNKNOWN";
#define PROC_VAL(p)                                                            \
  case (p):                                                                    \
    s = #p;                                                                    \
    break;
  switch (c)
  {
    PROC_VAL(SpringLayout::ForceApplication::ltsgraph_appl);
    PROC_VAL(SpringLayout::ForceApplication::force_directed_appl);
    PROC_VAL(SpringLayout::ForceApplication::force_cumulative_appl);
  }
#undef PROC_VAL
  return s;
}
#endif

//
// Utility functions
//

inline float cube(float x)
{
  return x * x * x;
}

inline float smoothstep(float l, float r, float x)
{
  if (x < l)
    return 0;
  if (x > r)
    return 1;

  x = (x - l) / (r - l);

  return x * x * (3 - 2 * x);
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

const float electricalSpringScaling = 1e-2f;

void SimpleAdaptiveSimulatedAnnealing::reset()
{
  m_temperature = m_reset_temperature;
  m_progress = 0;
  m_prev_energy = -1;
  T = m_temperature;
}

bool SimpleAdaptiveSimulatedAnnealing::calculateTemperature(float new_energy)
{
  if (new_energy < m_prev_energy*0.995)
  {
    m_progress++;
    if (m_progress >= m_progress_threshold)
    {
      m_temperature = std::max(m_temperature, m_minimum_temperature);
      m_temperature *= m_heating_factor;
      m_progress = 0;
    }
  }
  else if (new_energy < m_prev_energy)
  {
      // still made some progress possibly, but we do nothing
  }
  else
  {
    m_progress = 0;
    m_temperature *= m_cooling_factor;
  }
  T = std::max(m_minimum_temperature, m_temperature);
  m_prev_energy = new_energy;
  return false; // Simple => no checking for stable configuration
}

struct AttractionFunction
{
  virtual QVector3D operator()(const QVector3D& a, const QVector3D& b,
                               const float ideal) = 0;
  void update(){};
  void reset(){};
};
namespace AttractionFunctions
{
struct LTSGraph : AttractionFunction
{
  QVector3D diff = {0, 0, 0};
  const float scaling = 1e3f;
  float dist = 0.0f;
  float factor = 0.0f;
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float ideal) override
  {
    diff = (a - b);
    dist = (std::max)(diff.length(), 1.0f);
    factor = scaling * std::log(dist / (ideal + 1.0f)) / dist;
    return diff * factor;
  }
};

struct LinearSprings : AttractionFunction
{
  const float spring_constant = 1e-4f;
  const float scaling = 1.0f / 10000;
  QVector3D diff = {0, 0, 0};
  float dist = 0.0f;
  float factor = 0.0f;
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float ideal) override
  {
    diff = (a - b);
    dist = diff.length() - ideal;
    factor = spring_constant * std::max(dist, 0.0f);
    if (dist > 0.0f)
    {
      factor = std::max(factor, 100 / (std::max)(dist * dist / 10000.0f, 0.1f));
    }
    return diff * (factor * scaling);
  }
};

struct ElectricalSprings : AttractionFunction
{
  QVector3D diff = {0, 0, 0};
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float ideal) override
  {
    diff = (a - b);
    return (electricalSpringScaling * diff.length() / std::max(0.0001f, ideal)) *
           diff;
  }
};

struct SimpleSpring : AttractionFunction
{
  QVector3D diff = {0, 0, 0};
  const float spring_constant = 1e-4f;
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float ideal) override
  {
    diff = a - b;
    return spring_constant * (diff.length() - ideal) * diff;
  }
};
}; // namespace AttractionFunctions

struct RepulsionFunction
{
  virtual QVector3D operator()(const QVector3D& a, const QVector3D& b,
                               const float ideal) = 0;
  virtual void update(){};
  virtual void reset(){};
};

namespace RepulsionFunctions
{
struct LTSGraph : RepulsionFunction
{
  QVector3D diff;
  float r;
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float natlength) override
  {
    diff = a - b;
    r = cube(natlength);
    r /= cube((std::max)(diff.length() * 0.5f, natlength * 0.1f));
    diff = diff * r + QVector3D(fast_frand(-0.01f, 0.01f),
                                fast_frand(-0.01f, 0.01f),
                                fast_frand(-0.01f, 0.01f));
    return diff;
  }
};

struct ElectricalSpring : RepulsionFunction
{
  QVector3D diff;
  QVector3D operator()(const QVector3D& a, const QVector3D& b,
                       const float K) override
  {
    diff = a - b;
    return ((electricalSpringScaling * K * K) /
            std::max(diff.lengthSquared(), 0.00001f)) *
           diff;
  }
};

struct None : RepulsionFunction
{
  QVector3D ZERO = {0, 0, 0};
  QVector3D operator()(const QVector3D&, const QVector3D&,
                       const float) override
  {
    return ZERO;
  }
};
}; // namespace RepulsionFunctions

struct ApplicationFunction
{
  float* temperature = nullptr;
  virtual void operator()(QVector3D& pos, const QVector3D& f,
                          const float speed) = 0;
  virtual void update(){};
  virtual void reset(){};
};

namespace ApplicationFunctions
{
struct LTSGraph : ApplicationFunction
{
  const float scaling = 0.01f;
  const float limit = 1e4f;
  void operator()(QVector3D& pos, const QVector3D& f,
                  const float speed) override
  {
    if (scaling * std::abs(f.x()) >= limit ||
        scaling * std::abs(f.y()) >= limit ||
        scaling * std::abs(f.z()) >= limit)
      return;
    pos += f * (speed * scaling * *temperature);
  }
};

struct ForceDirected : ApplicationFunction
{
  const float scaling = 2.0f;
  const float stability_param = 0.01f;
  // precompute
  const float thres = scaling * stability_param;
  // easing such that at threshold translation is 50% of stepsize
  const float ease_width = .1f;
  const float ease_floor =
      1e-06f; // Always keep ease_floor force applied. May cause jitter
  // precompute
  const float one_minus_ease_floor = 1 - ease_floor;
  void operator()(QVector3D& pos, const QVector3D& f,
                  const float speed) override
  {

    float amplitude = speed * scaling * (*temperature);
    float threshold = speed * thres;
    threshold /= *temperature;
    float L = f.length();
    if (L < (1 + ease_width) * threshold)
    {
      // smoothstep the amplitude
      amplitude *= ease_floor + one_minus_ease_floor *
                                    smoothstep(threshold * (1 - ease_width),
                                               threshold * (1 + ease_width), L);
    }
    if (L == 0)
      return;
    pos += (amplitude / L) * f;
  }
};

struct ForceDirectedAnnealing : ApplicationFunction
{
  const float scaling = 2.0f;
  // Annealing parameters
  const int anneal_iterations = 1;
  const float anneal_cooling_factor = 0.98f;
  const float anneal_start_temperature = 100.0f;
  // Annealing variables
  float m_anneal_temperature = 1.0f;
  int iterations = 0;
  void operator()(QVector3D& pos, const QVector3D& f,
                  const float speed) override
  {
    pos += (speed * scaling * m_anneal_temperature) * f.normalized();
  }
  void update() override
  {
    if (++iterations >= anneal_iterations)
    {
      m_anneal_temperature *= anneal_cooling_factor;
      iterations = 0;
    }
  }
  void reset() override
  {
    m_anneal_temperature = anneal_start_temperature;
  }
};

struct ForceCumulative : ApplicationFunction
{
  ForceCumulative()
  {
    //mCRL2log(mcrl2::log::warning)
    //    << "Cumulative force not impemented yet." << std::endl;
  }
  void operator()(QVector3D& pos, const QVector3D& f,
                  const float speed) override
  {
    pos += (speed)*f.normalized();
  }
};
}; // namespace ApplicationFunctions
//
// SpringLayout
//

SpringLayout::SpringLayout(Graph& graph, GLWidget& glwidget)
    : m_node_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_handle_tree(0, {0, 0, 0}, {0, 0, 0}), 
      m_trans_tree(0, {0, 0, 0}, {0, 0, 0}),
      m_node_tree2D(0, {0, 0}, {0, 0}),
      m_handle_tree2D(0, {0, 0}, {0, 0}),
      m_trans_tree2D(0, {0, 0}, {0, 0}),
      m_speed(0.001f), m_attraction(0.13f), m_repulsion(50.0f),
      m_natLength(50.0f), m_controlPointWeight(0.001f), m_graph(graph),
      m_ui(nullptr), m_glwidget(glwidget), 
      attrFuncMap({
          {AttractionCalculation::ltsgraph_attr,
           new AttractionFunctions::LTSGraph()},
          {AttractionCalculation::electricalsprings_attr,
           new AttractionFunctions::ElectricalSprings()},
          {AttractionCalculation::linearsprings_attr,
           new AttractionFunctions::LinearSprings()},
      }),
      m_attrFunc(attrFuncMap[AttractionCalculation::ltsgraph_attr]),
      m_option_attractionCalculation(AttractionCalculation::ltsgraph_attr),
      repFuncMap({
          {RepulsionCalculation::ltsgraph_rep,
           new RepulsionFunctions::LTSGraph()},
          {RepulsionCalculation::electricalsprings_rep,
           new RepulsionFunctions::ElectricalSpring()},
          {RepulsionCalculation::none_rep, new RepulsionFunctions::None()},
      }),
      m_repFunc(repFuncMap[RepulsionCalculation::ltsgraph_rep]),
      m_option_repulsionCalculation(RepulsionCalculation::ltsgraph_rep),
      applFuncMap({
          {ForceApplication::ltsgraph_appl,
           new ApplicationFunctions::LTSGraph()},
          {ForceApplication::force_directed_appl,
           new ApplicationFunctions::ForceDirected()},
          {ForceApplication::force_cumulative_appl,
           new ApplicationFunctions::ForceCumulative()},
      }),
      m_applFunc(applFuncMap[ForceApplication::ltsgraph_appl]),
      m_option_forceApplication(ForceApplication::ltsgraph_appl)
{
  m_graph.gv_debug.addVar("Temperature");
  m_graph.gv_debug.addVar("Energy");
  m_graph.gv_debug.addVar("min energy");
  m_graph.gv_debug.addVar("max energy");
  m_graph.gv_debug.addToPlot(
      0, 0,
      {"Temperature", QBrush(Qt::red, Qt::SolidPattern), QPen(Qt::red, 1)});

  m_graph.gv_debug.addToPlot(
      1, 0,
      {"Energy", QBrush(QColor::fromRgbF(0, 0.5, 1), Qt::SolidPattern),
       QPen(QColor::fromRgbF(0, 0.5, 1), 1)});

  m_graph.gv_debug.addToPlot(
      1, 0,
      {"min energy", QBrush(QColor::fromRgbF(0, 0.25, 0.5), Qt::SolidPattern),
       QPen(QColor::fromRgbF(0, 0.5, 1), 1, Qt::DashLine, Qt::FlatCap)});

  m_graph.gv_debug.addToPlot(
      1, 0,
      {"max energy", QBrush(QColor::fromRgbF(0, 0.25, 0.5), Qt::SolidPattern),
       QPen(QColor::fromRgbF(0, 0.5, 1), 1, Qt::DashLine, Qt::FlatCap)});

  srand(time(nullptr));
  drift_timer.restart();
  m_annealing_temperature = m_useAnnealing ? m_asa.T : m_no_annealing_temperature;
  applFuncMap[ForceApplication::force_directed_appl]->temperature =
      &m_annealing_temperature;
  applFuncMap[ForceApplication::ltsgraph_appl]->temperature =
      &m_annealing_temperature;
  applFuncMap[ForceApplication::force_cumulative_appl]->temperature =
      &m_annealing_temperature;
}

SpringLayout::~SpringLayout()
{
  delete m_ui;
}

SpringLayoutUi* SpringLayout::ui(QAction* advancedDialogAction, CustomQWidget* advancedWidget, QWidget* parent)
{
  if (m_ui == nullptr)
  {
    m_ui = new SpringLayoutUi(*this, advancedWidget, parent);

    SettingsManager::addSettings("SpringLayoutUi");
    Settings* settings = SettingsManager::getSettings("SpringLayoutUi");
    Ui::AdvancedSpringLayoutDialog& advanced_ui = m_ui->m_ui_advanced;
    Ui::DockWidgetLayout& ui = m_ui->m_ui;
    settings->registerVar(ui.sldBalance, unlerp(0.5f, 0, 1));
    settings->registerVar(ui.sldHandleWeight, unlerp(0.1, m_min_controlPointWeight, m_max_controlPointWeight));
    settings->registerVar(ui.sldNatLength, unlerp(20, m_min_natLength, m_max_natLength));

    settings->registerVar(advanced_ui.sld_acc,
                          unlerp(1.2f, m_min_accuracy, m_max_accuracy), true);
    settings->registerVar(advanced_ui.sld_spd,
                          unlerp(m_speed_inverse_scale_func(1.0f),
                                 m_speed_inverse_scale_func(m_min_speed),
                                 m_speed_inverse_scale_func(m_max_speed)),
                          true);

    settings->registerVar(advanced_ui.chk_annealing, true, true);
    settings->registerVar(advanced_ui.chk_debugDraw, false, true);
    settings->registerVar(advanced_ui.chk_enableTree, true, true);

    settings->registerVar(advanced_ui.cmb_appl, (int)ForceApplication::force_directed_appl,
                          true);
    settings->registerVar(advanced_ui.cmb_attr,
                          (int)AttractionCalculation::ltsgraph_attr, true);
    settings->registerVar(advanced_ui.cmb_rep,
                          (int)RepulsionCalculation::ltsgraph_rep, true);

    settings->registerVar(advanced_ui.txt_cooling_factor, QString::number(m_asa.getCoolingFactor()),
                          true);
    settings->registerVar(advanced_ui.txt_heating_factor, QString::number(m_asa.getHeatingFactor()), true);
    settings->registerVar(advanced_ui.txt_progress_threshold, QString::number(m_asa.getProgressThreshold()),
                          true);

    settings->registerVar(advanced_ui.txt_stab_thres, QString::number(m_stabilityThreshold), true);
    settings->registerVar(advanced_ui.txt_stab_iters, QString::number(m_stabilityMaxCount), true);

    m_ui->m_ui.dispHandleWeight->setText(
        QString::number(m_controlPointWeight, 'g', 3));
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
  return m_ui;
}

void SpringLayout::setAttractionCalculation(AttractionCalculation c)
{
  if (attrFuncMap.find(c) == attrFuncMap.end())
  {
    mCRL2log(mcrl2::log::debug)
        << "Unkown attraction calculation selected \"" << getName(c)
        << "\". Cause may be invalid settings were loaded or selected function "
           "is not implemented."
        << std::endl;
    c = SpringLayout::AttractionCalculation::electricalsprings_attr;
    mCRL2log(mcrl2::log::debug) << "Setting default attraction calculation \""
                                << getName(c) << "\"." << std::endl;
  }

  m_option_attractionCalculation = c;
  m_attrFunc = attrFuncMap[c];
  m_attrFunc->reset();
#ifdef LOG_FUNCTION_SETTER
  mCRL2log(mcrl2::log::debug)
      << "Attraction set to: " << getName(c) << std::endl;
  mCRL2log(mcrl2::log::debug)
      << "Attr func: " << typeid(*m_attrFunc).name() << std::endl;
#endif
}

SpringLayout::AttractionCalculation SpringLayout::attractionCalculation()
{
  return m_option_attractionCalculation;
}

void SpringLayout::setRepulsionCalculation(RepulsionCalculation c)
{
  if (repFuncMap.find(c) == repFuncMap.end())
  {
    mCRL2log(mcrl2::log::debug)
        << "Unkown repulsion calculation selected \"" << getName(c)
        << "\". Cause may be invalid settings were loaded or selected function "
           "is not implemented."
        << std::endl;
    c = SpringLayout::RepulsionCalculation::electricalsprings_rep;
    mCRL2log(mcrl2::log::debug) << "Setting default repulsion calculation \""
                                << getName(c) << "\"." << std::endl;
  }

  m_option_repulsionCalculation = c;
  m_repFunc = repFuncMap[c];
  m_repFunc->reset();
#ifdef LOG_FUNCTION_SETTER
  mCRL2log(mcrl2::log::debug)
      << "Repulsion set to: " << getName(c) << std::endl;
  mCRL2log(mcrl2::log::debug)
      << "Rep func: " << typeid(*m_repFunc).name() << std::endl;
#endif
}

SpringLayout::RepulsionCalculation SpringLayout::repulsionCalculation()
{
  return m_option_repulsionCalculation;
}

void SpringLayout::setForceApplication(SpringLayout::ForceApplication c)
{
  if (applFuncMap.find(c) == applFuncMap.end())
  {
    mCRL2log(mcrl2::log::debug)
        << "Unkown force application selected \"" << getName(c)
        << "\". Cause may be invalid settings were loaded or selected function "
           "is not implemented."
        << std::endl;
    c = SpringLayout::ForceApplication::force_directed_appl;
    mCRL2log(mcrl2::log::debug) << "Setting default force application \""
                                << getName(c) << "\"." << std::endl;
  }

  m_option_forceApplication = c;
  m_applFunc = applFuncMap[c];
  m_applFunc->reset();
#ifdef LOG_FUNCTION_SETTER
  mCRL2log(mcrl2::log::debug)
      << "Application set to: " << getName(c) << std::endl;
  mCRL2log(mcrl2::log::debug)
      << "Appl func: " << typeid(*m_applFunc).name() << std::endl;
#endif
}

SpringLayout::ForceApplication SpringLayout::forceApplication()
{
  return m_option_forceApplication;
}

template <>
QVector3D SpringLayout::approxRepulsionForce<Octree>(const QVector3D& a,
                                                     Octree& tree)
{
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes(a);
  for (auto super_node : super_nodes)
  {
    force +=
        super_node->children * (*m_repFunc)(a, super_node->pos, m_natLength);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

template <>
QVector3D SpringLayout::approxRepulsionForce<Quadtree>(const QVector3D& a,
                                                       Quadtree& tree)
{
  QVector3D force(0, 0, 0);
  std::size_t num_nodes = 0;
  auto& super_nodes = tree.getSuperNodes({a.x(), a.y()});
  for (auto super_node : super_nodes)
  {
    force += super_node->children *
             (*m_repFunc)(a, {super_node->pos.x(), super_node->pos.y(), 0},
                          m_natLength);
  }
  force *= m_repulsion;
  num_nodes = super_nodes.size();

  m_max_num_nodes = std::max(m_max_num_nodes, num_nodes);
  m_total_num_nodes += num_nodes;
  return force;
}

const std::size_t max_slice = 50;
/// @brief Takes average of at most @c max_slice values, or recursively computes
/// average by splitting
/// @param i Start index
/// @param j End index (exclusive)
/// This method avoids precision loss due to adding up too much before division
///  Downside: more divisions. Upside: more accuracy.
static QVector3D slicedAverage(Graph& graph, std::size_t i, std::size_t j)
{
  std::size_t n = j - i;
  if (n > max_slice)
  {
    // split
    std::size_t m = i + n / 2;
    double recip = 1.0 / n;
    return (m - i) * recip * slicedAverage(graph, i, m) +
           (j - m) * recip * slicedAverage(graph, m, j);
  }
  else
  {
    double x = 0, y = 0, z = 0;
    auto node = [&graph](std::size_t k) {
      return graph.node(graph.hasExploration() ? graph.explorationNode(k) : k);
    };
    for (std::size_t k = i; k < j; ++k)
    {
      x += node(k).pos().x();
      y += node(k).pos().y();
      z += node(k).pos().z();
    }
    return QVector3D(x / n, y / n, z / n);
  }
}

static float slicedAverageSqrMagnitude(std::vector<QVector3D>& forces,
                                       std::size_t i, std::size_t j)
{
  std::size_t n = j - i;
  if (n > max_slice)
  {
    // split
    std::size_t m = i + n / 2;
    double recip = 1.0 / n;
    return (m - i) * recip * slicedAverageSqrMagnitude(forces, i, m) +
           (j - m) * recip * slicedAverageSqrMagnitude(forces, m, j);
  }
  else
  {
    double sum = 0;
    for (std::size_t k = i; k < j; ++k)
      sum += forces[k].lengthSquared();
    return sum / n;
  }
}

int iterations = 0;

template <>
void SpringLayout::attractionAccumulation<SpringLayout::ThreadingMode::normal>(
    bool sel, std::size_t nodeCount, std::size_t edgeCount)
{
  std::vector<std::size_t> nodeLocations(m_graph.nodeCount());
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    nodeLocations[n] = i;
    m_nforces[i] = {0, 0, 0};
    m_sforces[i] =
        (*m_attrFunc)(m_graph.node(n).pos(), m_graph.stateLabel(n).pos(), 0.0) *
        m_attraction;
  }

  QVector3D f;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    // Variables for repulsion calculations
    std::size_t from = e.from();
    std::size_t to = e.to();

    m_hforces[i] = QVector3D(0, 0, 0);
    m_lforces[i] = QVector3D(0, 0, 0);

    if (e.is_selfloop())
    {
      m_hforces[i] += (*m_repFunc)(m_graph.handle(n).pos(),
                                   m_graph.node(from).pos(), m_natLength) *
                      m_repulsion;
    }

    f = (*m_attrFunc)(m_graph.node(to).pos(), m_graph.node(from).pos(),
                      m_natLength) *
        m_attraction;
    m_nforces[nodeLocations[from]] += f;
    m_nforces[nodeLocations[to]] -= f;

    f = (*m_attrFunc)(
            (m_graph.node(to).pos() + m_graph.node(from).pos()) / 2.0,
            m_graph.handle(n).pos(), 0.0) *
        m_attraction;
    m_hforces[i] += f;

    f = (*m_attrFunc)(m_graph.handle(n).pos(), m_graph.transitionLabel(n).pos(),
                      0.0) *
        m_attraction;
    m_lforces[i] += f;
  }
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::quadtree>(
    bool sel, std::size_t nodeCount, std::size_t edgeCount)
{
  /// TODO: Fix blatant code duplication
  QVector2D node_min = {INFINITY, INFINITY}, node_max = {-INFINITY, -INFINITY},
            handle_min = {INFINITY, INFINITY},
            handle_max = {-INFINITY, -INFINITY},
            trans_min = {INFINITY, INFINITY},
            trans_max = {-INFINITY, -INFINITY};

  // calculate bounding volumes
  {
    for (int i = 0; i < nodeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      node_min.setX(std::min(node_min.x(), m_graph.node(n).pos().x()));
      node_min.setY(std::min(node_min.y(), m_graph.node(n).pos().y()));
      node_max.setX(std::max(node_max.x(), m_graph.node(n).pos().x()));
      node_max.setY(std::max(node_max.y(), m_graph.node(n).pos().y()));
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      handle_min.setX(std::min(handle_min.x(), m_graph.handle(n).pos().x()));
      handle_min.setY(std::min(handle_min.y(), m_graph.handle(n).pos().y()));
      handle_max.setX(std::max(handle_max.x(), m_graph.handle(n).pos().x()));
      handle_max.setY(std::max(handle_max.y(), m_graph.handle(n).pos().y()));
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      trans_min.setX(
          std::min(trans_min.x(), m_graph.transitionLabel(n).pos().x()));
      trans_min.setY(
          std::min(trans_min.y(), m_graph.transitionLabel(n).pos().y()));
      trans_max.setX(
          std::max(trans_max.x(), m_graph.transitionLabel(n).pos().x()));
      trans_max.setY(
          std::max(trans_max.y(), m_graph.transitionLabel(n).pos().y()));
    }
  }
  node_min -= QVector2D(1, 1);
  node_max += QVector2D(1, 1);
  handle_min -= QVector2D(1, 1);
  handle_max += QVector2D(1, 1);
  trans_min -= QVector2D(1, 1);
  trans_max += QVector2D(1, 1);

  // The code commented out below calculates a cube around all points to have a 'perfect' octree

  //QVector2D _extents, cubic_extents;
  //float _width;

  // _extents = node_max - node_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // node_min -= (cubic_extents - _extents)*0.5f;
  // node_max = node_min + cubic_extents;

  // _extents = handle_max - handle_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // handle_min -= (cubic_extents - _extents)*0.5f;
  // handle_max = handle_min + cubic_extents;

  // _extents = trans_max - trans_min;
  // _width = std::max({_extents.x(), _extents.y()});
  // cubic_extents = QVector2D(_width, _width);
  // trans_min -= (cubic_extents - _extents)*0.5f;
  // trans_max = trans_min + cubic_extents;

  m_node_tree2D.setMinBounds(node_min);
  m_node_tree2D.setMaxBounds(node_max);

  m_handle_tree2D.setMinBounds(handle_min);
  m_handle_tree2D.setMaxBounds(handle_max);

  m_trans_tree2D.setMinBounds(trans_min);
  m_trans_tree2D.setMaxBounds(trans_max);

  m_node_tree2D.setTheta(m_accuracy);
  m_handle_tree2D.setTheta(m_accuracy);
  m_trans_tree2D.setTheta(m_accuracy);

  m_node_tree2D.reset();
  m_handle_tree2D.reset();
  m_trans_tree2D.reset();

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;

    m_node_tree2D.insert((QVector2D)m_graph.node(n).pos());
  }

  m_node_tree2D.calculatePositions();

  // approx repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[i] +=
        approxRepulsionForce<Quadtree>(m_graph.node(n).pos(), m_node_tree2D);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;

    Edge e = m_graph.edge(n);
    m_handle_tree2D.insert((QVector2D)m_graph.handle(n).pos());
    m_trans_tree2D.insert((QVector2D)m_graph.transitionLabel(n).pos());
  }

  m_handle_tree2D.calculatePositions();
  m_trans_tree2D.calculatePositions();

  // approximate repulsive forces
  float temp = m_repulsion;
  m_repulsion *= m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[i] += approxRepulsionForce<Quadtree>(m_graph.handle(n).pos(),
                                                   m_handle_tree2D);
    m_lforces[i] += approxRepulsionForce<Quadtree>(
        m_graph.transitionLabel(n).pos(), m_trans_tree2D);
  }
  m_repulsion = temp;
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::octree>(
    bool sel, std::size_t nodeCount, std::size_t edgeCount)
{
  /// TODO: Fix blatant code duplication
  QVector3D node_min = {INFINITY, INFINITY, INFINITY},
            node_max = {-INFINITY, -INFINITY, -INFINITY},
            handle_min = {INFINITY, INFINITY, INFINITY},
            handle_max = {-INFINITY, -INFINITY, -INFINITY},
            trans_min = {INFINITY, INFINITY, INFINITY},
            trans_max = {-INFINITY, -INFINITY, -INFINITY};

  // calculate bounding volumes
  {
    for (int i = 0; i < nodeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;
      node_min.setX(std::min(node_min.x(), m_graph.node(n).pos().x()));
      node_min.setY(std::min(node_min.y(), m_graph.node(n).pos().y()));
      node_min.setZ(std::min(node_min.z(), m_graph.node(n).pos().z()));
      node_max.setX(std::max(node_max.x(), m_graph.node(n).pos().x()));
      node_max.setY(std::max(node_max.y(), m_graph.node(n).pos().y()));
      node_max.setZ(std::max(node_max.z(), m_graph.node(n).pos().z()));
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      handle_min.setX(std::min(handle_min.x(), m_graph.handle(n).pos().x()));
      handle_min.setY(std::min(handle_min.y(), m_graph.handle(n).pos().y()));
      handle_min.setZ(std::min(handle_min.z(), m_graph.handle(n).pos().z()));
      handle_max.setX(std::max(handle_max.x(), m_graph.handle(n).pos().x()));
      handle_max.setY(std::max(handle_max.y(), m_graph.handle(n).pos().y()));
      handle_max.setZ(std::max(handle_max.z(), m_graph.handle(n).pos().z()));
    }

    // calculate bounding volume
    for (int i = 0; i < edgeCount; i++)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;
      trans_min.setX(
          std::min(trans_min.x(), m_graph.transitionLabel(n).pos().x()));
      trans_min.setY(
          std::min(trans_min.y(), m_graph.transitionLabel(n).pos().y()));
      trans_min.setZ(
          std::min(trans_min.z(), m_graph.transitionLabel(n).pos().z()));
      trans_max.setX(
          std::max(trans_max.x(), m_graph.transitionLabel(n).pos().x()));
      trans_max.setY(
          std::max(trans_max.y(), m_graph.transitionLabel(n).pos().y()));
      trans_max.setZ(
          std::max(trans_max.z(), m_graph.transitionLabel(n).pos().z()));
    }
  }
  node_min -= QVector3D(1, 1, 1);
  node_max += QVector3D(1, 1, 1);
  handle_min -= QVector3D(1, 1, 1);
  handle_max += QVector3D(1, 1, 1);
  trans_min -= QVector3D(1, 1, 1);
  trans_max += QVector3D(1, 1, 1);

  //QVector3D _extents, cubic_extents;
  //float _width;

  // _extents = node_max - node_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // node_min -= (cubic_extents - _extents)*0.5f;
  // node_max = node_min + cubic_extents;

  // _extents = handle_max - handle_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // handle_min -= (cubic_extents - _extents)*0.5f;
  // handle_max = handle_min + cubic_extents;

  // _extents = trans_max - trans_min;
  // _width = std::max({_extents.x(), _extents.y(), _extents.z()});
  // cubic_extents = QVector3D(_width, _width, _width);
  // trans_min -= (cubic_extents - _extents)*0.5f;
  // trans_max = trans_min + cubic_extents;

  m_node_tree.setMinBounds(node_min);
  m_node_tree.setMaxBounds(node_max);

  m_handle_tree.setMinBounds(handle_min);
  m_handle_tree.setMaxBounds(handle_max);

  m_trans_tree.setMinBounds(trans_min);
  m_trans_tree.setMaxBounds(trans_max);

  m_node_tree.setTheta(m_accuracy);
  m_handle_tree.setTheta(m_accuracy);
  m_trans_tree.setTheta(m_accuracy);

  m_node_tree.reset();
  m_handle_tree.reset();
  m_trans_tree.reset();

  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;

    m_node_tree.insert(m_graph.node(n).pos());
  }

  m_node_tree.calculatePositions();

  // approx repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    m_nforces[i] +=
        approxRepulsionForce<Octree>(m_graph.node(n).pos(), m_node_tree);
  }

  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_handle_tree.insert(m_graph.handle(n).pos());
    m_trans_tree.insert(m_graph.transitionLabel(n).pos());
  }

  m_handle_tree.calculatePositions();
  m_trans_tree.calculatePositions();

  // approximate repulsive forces
  const float temp = m_repulsion;
  m_repulsion *= m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    m_hforces[i] +=
        approxRepulsionForce<Octree>(m_graph.handle(n).pos(), m_handle_tree);
    m_lforces[i] += approxRepulsionForce<Octree>(
        m_graph.transitionLabel(n).pos(), m_trans_tree);
  }
  m_repulsion = temp;
}

template <>
void SpringLayout::repulsionAccumulation<SpringLayout::TreeMode::none>(
    bool sel, std::size_t nodeCount, std::size_t edgeCount)
{
  // used for storing intermediate results
  QVector3D f;
  // repulsive forces for nodes
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationNode(i) : i;
    for (std::size_t j = i + 1; j < nodeCount; ++j)
    {
      std::size_t m = sel ? m_graph.explorationNode(j) : j;
      f = (*m_repFunc)(m_graph.node(n).pos(), m_graph.node(m).pos(),
                       m_natLength) *
          m_repulsion;
      m_nforces[i] += f;
      m_nforces[j] -= f;
    }
  }

  // repulsive forces
  const float repulsion_force_control_point =
      m_repulsion * m_controlPointWeight;
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = sel ? m_graph.explorationEdge(i) : i;
    for (std::size_t j = i + 1; j < edgeCount; ++j)
    {
      std::size_t m = sel ? m_graph.explorationNode(j) : j;
      f = (*m_repFunc)(m_graph.handle(n).pos(), m_graph.handle(m).pos(),
                       m_natLength) *
          repulsion_force_control_point;
      m_hforces[i] += f;
      m_hforces[j] -= f;

      f = (*m_repFunc)(m_graph.transitionLabel(n).pos(),
                       m_graph.transitionLabel(m).pos(), m_natLength) *
          repulsion_force_control_point;
      m_lforces[i] += f;
      m_lforces[j] -= f;
    }
  }
}

void SpringLayout::forceAccumulation(bool sel, std::size_t nodeCount,
                                     std::size_t edgeCount, TreeMode treeMode,
                                     ThreadingMode threadingMode)
{
  switch (threadingMode)
  {
  case ThreadingMode::normal:
    attractionAccumulation<ThreadingMode::normal>(sel, nodeCount, edgeCount);
    break;
  }
  if (m_option_repulsionCalculation != RepulsionCalculation::none_rep)
  {
    switch (treeMode)
    {
    case TreeMode::none:
      repulsionAccumulation<TreeMode::none>(sel, nodeCount, edgeCount);
      break;
    case TreeMode::quadtree:
      repulsionAccumulation<TreeMode::quadtree>(sel, nodeCount, edgeCount);
      break;
    case TreeMode::octree:
      repulsionAccumulation<TreeMode::octree>(sel, nodeCount, edgeCount);
      break;
    }
  }
}

void SpringLayout::apply()
{
  assert(m_attrFunc);
  assert(m_repFunc);
  assert(m_attrFunc);
  if (!m_graph.stable() || m_graph.hasForcedUpdate())
  {
    m_graph.lock(GRAPH_LOCK_TRACE); // enter critical section
    if (m_graph.hasForcedUpdate())
    {
      m_graph.hasForcedUpdate() = false;
      m_asa.reset();
    }
    if (m_graph.scrambleZ())
    {
      m_graph.scrambleZ() = false;
      float min_x = INFINITY;
      float max_x = -INFINITY;
      float min_y = INFINITY;
      float max_y = -INFINITY;
      for (std::size_t i = 0; i < m_graph.nodeCount(); i++)
      {
        min_x = std::min(min_x, m_graph.node(i).pos().x());
        max_x = std::max(max_x, m_graph.node(i).pos().x());
        min_y = std::min(min_y, m_graph.node(i).pos().y());
        max_y = std::max(max_y, m_graph.node(i).pos().y());
      }
      randomizeZ(0.1f * std::sqrt((max_x - min_x) * (max_x - min_x) +
                                  (max_y - min_y) * (max_y - min_y)));
      
    }
    if (m_graph.resetPositions())
    {
      m_graph.resetPositions() = false;
      resetPositions();
    }
    bool sel = m_graph.hasExploration();
    std::size_t nodeCount =
        sel ? m_graph.explorationNodeCount() : m_graph.nodeCount();
    std::size_t edgeCount =
        sel ? m_graph.explorationEdgeCount() : m_graph.edgeCount();


    m_nforces.resize(nodeCount);
    m_sforces.resize(nodeCount);
    m_hforces.resize(edgeCount);
    m_lforces.resize(edgeCount);
    std::fill(m_nforces.begin(), m_nforces.end(), QVector3D(0, 0, 0));
    std::fill(m_sforces.begin(), m_sforces.end(), QVector3D(0, 0, 0));
    std::fill(m_hforces.begin(), m_hforces.end(), QVector3D(0, 0, 0));
    std::fill(m_lforces.begin(), m_lforces.end(), QVector3D(0, 0, 0));
    if (m_tree_enabled)
    {
      bool is_2D =
          (m_graph.getClipMax().z() - m_graph.getClipMin().z() < 0.001f);
      if (is_2D)
      {
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::quadtree,
                          ThreadingMode::normal);
      }
      else
      {
        forceAccumulation(sel, nodeCount, edgeCount, TreeMode::octree,
                          ThreadingMode::normal);
      }
    }
    else
    {
      forceAccumulation(sel, nodeCount, edgeCount, TreeMode::none,
                        ThreadingMode::normal);
    }

    QVector3D clipmin = m_graph.getClipMin();
    QVector3D clipmax = m_graph.getClipMax();
    float nodeSumForces = 0;
    float edgeSumForces = 0;
    bool new_anchored = false;
    float use_speed = m_speed * std::log2f(nodeCount) * 0.25f;
    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.node(n).anchored())
      {
        (*m_applFunc)(m_graph.node(n).pos_mutable(), m_nforces[i], use_speed);
        nodeSumForces += m_nforces[i].lengthSquared();
        clipVector(m_graph.node(n).pos_mutable(), clipmin, clipmax);
      }
      else
      {
        new_anchored = true;
      }
    }

    float drift_secs = drift_timer.elapsed() * 0.001f; // seconds
    QVector3D center_of_mass = slicedAverage(m_graph, 0, m_graph.hasExploration() ? m_graph.explorationNodeCount() : m_graph.nodeCount());
    if (new_anchored ^ any_anchored)
    {
      // changed
      center_of_mass_offset = center_of_mass;
      any_anchored = new_anchored;
      mCRL2log(mcrl2::log::debug)
          << "Setting new center of mass offset: " << center_of_mass_offset.x()
          << ", " << center_of_mass_offset.y() << ", "
          << center_of_mass_offset.z() << std::endl;
      drift_timer.restart();
      drift_secs = 0;
    }
    if (!any_anchored)
    {
      center_of_mass -=
          smoothstep(0, time_to_center, time_to_center - drift_secs) *
          center_of_mass_offset;
    }
    else
    {
      center_of_mass = QVector3D(0, 0, 0);
    }

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      m_graph.node(n).pos_mutable() -= center_of_mass;
    }

    for (std::size_t i = 0; i < nodeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationNode(i) : i;

      if (!m_graph.stateLabel(n).anchored())
      {
        (*m_applFunc)(m_graph.stateLabel(n).pos_mutable(), m_sforces[i],
                      use_speed);
        m_graph.stateLabel(n).pos_mutable() -= center_of_mass;
        nodeSumForces += m_sforces[i].lengthSquared();
        clipVector(m_graph.stateLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }

    for (std::size_t i = 0; i < edgeCount; ++i)
    {
      std::size_t n = sel ? m_graph.explorationEdge(i) : i;

      if (!m_graph.handle(n).anchored())
      {
        (*m_applFunc)(m_graph.handle(n).pos_mutable(), m_hforces[i], use_speed);
        m_graph.handle(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_hforces[i].lengthSquared();
        clipVector(m_graph.handle(n).pos_mutable(), clipmin, clipmax);
      }
      if (!m_graph.transitionLabel(n).anchored())
      {
        (*m_applFunc)(m_graph.transitionLabel(n).pos_mutable(), m_lforces[i],
                      use_speed);
        m_graph.transitionLabel(n).pos_mutable() -= center_of_mass;
        edgeSumForces += m_lforces[i].lengthSquared();
        clipVector(m_graph.transitionLabel(n).pos_mutable(), clipmin, clipmax);
      }
    }
    double energy = slicedAverageSqrMagnitude(m_nforces, 0, m_nforces.size());
    double min = 1e15;
    double max = -1e15;
    for (auto f : m_nforces)
    {
      double mag = f.lengthSquared();
      min = std::min(min, mag);
      max = std::max(max, mag);
    }
    if (m_glwidget.getDebugDrawGraphs())
    {
      m_graph.gv_debug.logVar("Temperature", m_asa.T);
      m_graph.gv_debug.logVar("Energy", energy);

      m_graph.gv_debug.logVar("min energy", min);
      m_graph.gv_debug.logVar("max energy", max);
    }
    if (m_useAnnealing)
    {
      m_asa.calculateTemperature(energy);
      m_annealing_temperature = m_asa.T;
    }
    if (m_graph.userIsDragging)
    {
      m_asa.reset();
    }
    
    m_max_num_nodes = 0;
    m_total_num_nodes = 0;

    m_applFunc->update();
    m_repFunc->update();
    m_attrFunc->update();

    float stability =
        std::abs((m_previous_energy - energy) / m_previous_energy);
    if (stability <= m_stabilityThreshold && (center_of_mass == QVector3D(0, 0, 0) || any_anchored))
    {
      m_stabilityCounter++;
      if (m_stabilityCounter >= m_stabilityMaxCount)
      {
          m_graph.setStable(true);
          mCRL2log(mcrl2::log::debug) << "The graph is now stable." << std::endl;
          m_ui->m_ui.lblStable->setText("Stable");
      }
    }
    else
    {
      m_stabilityCounter = 0;
      m_ui->m_ui.lblStable->setText("");
    }
    m_previous_energy = energy;

    notifyNewFrame();
    m_graph.unlock(GRAPH_LOCK_TRACE);
  }
}

void SpringLayout::randomizeZ(float z)
{
  m_graph.lock(GRAPH_LOCK_TRACE);
  bool exploration = m_graph.hasExploration();
  std::size_t nodeCount = exploration
                              ? m_graph.explorationNodeCount()
                              : m_graph.nodeCount();
  std::size_t edgeCount = exploration
                              ? m_graph.explorationEdgeCount()
                              : m_graph.edgeCount();
  for (std::size_t i = 0; i < nodeCount; ++i)
  {
    std::size_t n = exploration ? m_graph.explorationNode(i) : i;
    if (!m_graph.node(n).anchored())
    {
      float z_offset = fast_frand(-z, z);
      m_graph.node(n).pos_mutable().setZ(m_graph.node(n).pos().z() +
                                         z_offset);
      m_graph.stateLabel(n).pos_mutable().setZ(m_graph.stateLabel(n).pos().z() +
                                               z_offset);
    }
  }
  for (std::size_t i = 0; i < edgeCount; ++i)
  {
    std::size_t n = exploration ? m_graph.explorationEdge(i) : i;
    if (!m_graph.transitionLabel(n).anchored())
    {
      m_graph.transitionLabel(n).pos_mutable() =
          0.5 * (m_graph.node(m_graph.edge(n).from()).pos() +
                 m_graph.node(m_graph.edge(n).to()).pos());
      m_graph.handle(n).pos_mutable() = m_graph.transitionLabel(n).pos();
    }
  }
  m_asa.reset();
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

void SpringLayout::notifyNewFrame()
{
  m_graph.hasNewFrame(true);
}

void SpringLayout::setTreeEnabled(bool b)
{
  m_tree_enabled = b;
  mCRL2log(mcrl2::log::verbose)
      << (b ? "Enabled" : "Disabled") << " tree acceleration." << std::endl;
}

void SpringLayout::setAnnealingEnabled(bool b)
{
  m_useAnnealing = b;
  if (m_useAnnealing)
  {
    m_annealing_temperature = m_asa.T;
  }
  else
  {
    m_annealing_temperature = m_no_annealing_temperature;
  }
}

void SpringLayout::setSpeed(int v)
{
  m_speed = m_speed_scale_func(lerp(v, m_speed_inverse_scale_func(m_min_speed),
                                    m_speed_inverse_scale_func(m_max_speed)));
  mCRL2log(mcrl2::log::verbose)
      << "Set speed to: " << v << " corresponding to: " << m_speed << std::endl;
  if (this->m_ui)
    m_ui->m_ui_advanced.disp_spd->setText(QString::number(m_speed, 'g', 3));
}

void SpringLayout::setAccuracy(int v)
{
  m_accuracy = lerp(v, m_min_accuracy, m_max_accuracy);
  mCRL2log(mcrl2::log::verbose)
      << "Set accuracy to: " << v << " corresponding to: " << m_accuracy
      << std::endl;
  if (this->m_ui)
    m_ui->m_ui_advanced.disp_acc->setText(QString::number(m_accuracy, 'g', 3));
}

void SpringLayout::setAttraction(int v)
{
  m_attraction = lerp(v, 1, 0);
  if (this->m_ui)
      m_ui->m_ui.lbl_attractRepulse->setText(QString::number(1-m_attraction, 'g', 2));
  mCRL2log(mcrl2::log::verbose)
      << "Set attraction scale to: " << v
      << " corresponding to: " << m_attraction << std::endl;
}

void SpringLayout::setRepulsion(int v)
{
  m_repulsion = lerp(v, 0, 1);
  mCRL2log(mcrl2::log::verbose)
      << "Set repulsion scale to: " << v << " corresponding to: " << m_repulsion
      << std::endl;
}

void SpringLayout::setControlPointWeight(int v)
{
  m_controlPointWeight =
      lerp(v, m_min_controlPointWeight, m_max_controlPointWeight);
  mCRL2log(mcrl2::log::verbose)
      << "Set control point weight to: " << m_controlPointWeight << std::endl;
  if (this->m_ui)
    m_ui->m_ui.dispHandleWeight->setText(
        QString::number(m_controlPointWeight, 'g', 3));
}

void SpringLayout::setNaturalTransitionLength(int v)
{
  m_natLength = lerp(v, m_min_natLength, m_max_natLength);
  mCRL2log(mcrl2::log::verbose)
      << "Set natural length to: " << v << " corresponding to: " << m_natLength
      << std::endl;
  if (this->m_ui)
  {
    m_ui->m_ui.dispNatLength->setText(QString::number(m_natLength, 'g', 3));
  }
}

void SpringLayout::rulesChanged()
{
  m_graph.setStable(false);
  setForceApplication(m_option_forceApplication);
  m_applFunc->reset();
  m_repFunc->reset();
  m_attrFunc->reset();
  m_asa.reset();
}

void SpringLayout::resetPositions()
{
  mCRL2log(mcrl2::log::debug) << "Resetting positions" << std::endl;
  m_graph.lock(GRAPH_LOCK_TRACE);
  bool exploration = m_graph.hasExploration();
  std::size_t n_nodes = exploration ? m_graph.explorationNodeCount() : m_graph.nodeCount();
  std::size_t n_edges = exploration ? m_graph.explorationEdgeCount() : m_graph.edgeCount();
  bool is3D = m_glwidget.get3D();
  float hwidth = 5*std::pow(m_natLength*n_nodes, 1.0f / (is3D ? 3 : 2));
  for (std::size_t i = 0; i < n_nodes; i++)
  {
    std::size_t n = exploration ? m_graph.explorationNode(i) : i;
    if (!m_graph.node(n).anchored())
    {
      m_graph.node(n).pos_mutable().setX(frand(-hwidth, hwidth));
      m_graph.node(n).pos_mutable().setY(frand(-hwidth, hwidth));
      m_graph.node(n).pos_mutable().setZ(is3D ? frand(-hwidth, hwidth) : 0);
      m_graph.stateLabel(n).pos_mutable() = m_graph.node(n).pos();
    }
  }
  for (std::size_t i = 0; i < n_edges; i++)
  {
    std::size_t n = exploration ? m_graph.explorationEdge(i) : i;
    if (!m_graph.transitionLabel(n).anchored())
    {
      m_graph.transitionLabel(n).pos_mutable() =
          0.5 * (m_graph.node(m_graph.edge(n).from()).pos() +
                 m_graph.node(m_graph.edge(n).to()).pos());
      m_graph.handle(n).pos_mutable() = m_graph.transitionLabel(n).pos();
    }
  }
  m_graph.hasNewFrame(true);
  m_graph.setStable(false);
  m_asa.reset();
  m_graph.unlock(GRAPH_LOCK_TRACE);
}

//
// SpringLayoutUi
//

class WorkerThread : public QThread
{
  private:
  bool m_stopped;
  SpringLayout& m_layout;
  int m_counter = 0;
#ifdef DEV_DEBUG
  // By default log ever second
  const int m_debug_log_interval = 1000;
  // if we have a cycle time longer than 100ms we want an extra message
  const int m_debug_max_cycle_time = 100;
  QElapsedTimer m_debug_log_timer;
#endif
  public:
  WorkerThread(SpringLayout& layout, QObject* parent = nullptr)
      : QThread(parent), m_stopped(false), m_layout(layout)
  {
#ifdef DEV_DEBUG
    mCRL2log(mcrl2::log::debug)
        << "Workerthread will output debug messages to this stream."
        << std::endl;
    m_debug_log_timer.start();
#endif
  }

  void stop()
  {
    m_stopped = true;
  }

  void run() override
  {
    while (!m_stopped)
    {
      if (m_layout.isStable())
      {
        msleep(50); // We don't want to keep computing if the layout is stable
      }
      else
      {
          m_layout.apply();
          m_counter++;
          debugLogging();
      }
    }
  }

#ifdef DEV_DEBUG
  /// @brief Only called when a debug configuration is ran.
  void debugLogging()
  {
    int elapsed = m_debug_log_timer.elapsed();
    if (elapsed > m_debug_log_interval)
    {
      mCRL2log(mcrl2::log::debug)
          << "Worker thread performed " << m_counter << " cycles in " << elapsed
          << "ms. ASA temperature: " << m_layout.m_asa.T;
      if ((float)elapsed / m_counter > m_debug_max_cycle_time)
        mCRL2log(mcrl2::log::debug)
            << " - NB: This is longer than the set expected maximum "
            << m_debug_max_cycle_time << "ms per cycle. ";
      mCRL2log(mcrl2::log::debug) << std::endl;
      // reset debugging
      m_debug_log_timer.restart();
      m_counter = 0;
    }
  }
#else
  // This structure is required to be optimised out by every "Release" flagged
  // compiler at least, and most compilers will optimise always
  void debugLogging()
  {
    do
    {
    } while (0);
  }
#endif
};

SpringLayoutUi::SpringLayoutUi(SpringLayout& layout,
                               CustomQWidget* advancedDialogWidget, QWidget* parent)
    : QDockWidget(parent), m_layout(layout), m_ui_advanced_dialog(advancedDialogWidget), m_thread(nullptr)
{
  m_ui.setupUi(this);
  m_ui_advanced.setupUi(m_ui_advanced_dialog);
  m_ui_advanced_dialog->hide();

  m_ui.sldBalance->setValue(m_layout.repulsion());
  m_ui.sldHandleWeight->setValue(m_layout.controlPointWeight());
  m_ui.sldNatLength->setValue(m_layout.naturalTransitionLength());
  m_layout.setTreeEnabled(m_ui_advanced.chk_enableTree->isChecked());
  m_ui_advanced.chk_enableTree->setChecked(false);

  m_ui_advanced.sld_spd->setValue(m_layout.speed());
  connect(m_ui_advanced.sld_spd, SIGNAL(valueChanged(int)), this,
          SLOT(onSpeedChanged(int)));

  m_ui_advanced.sld_acc->setValue(50);
  connect(m_ui_advanced.sld_acc, SIGNAL(valueChanged(int)), this,
          SLOT(onAccuracyChanged(int)));

  m_ui_advanced.cmb_attr->setCurrentIndex(m_layout.attractionCalculation());
  connect(m_ui_advanced.cmb_attr, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onAttractionCalculationChanged(int)));

  m_ui_advanced.cmb_rep->setCurrentIndex(m_layout.repulsionCalculation());
  connect(m_ui_advanced.cmb_rep, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onRepulsionCalculationChanged(int)));

  m_ui_advanced.cmb_appl->setCurrentIndex(m_layout.forceApplication());
  connect(m_ui_advanced.cmb_appl, SIGNAL(currentIndexChanged(int)), this,
          SLOT(onForceApplicationChanged(int)));

  connect(m_ui_advanced.chk_debugDraw, SIGNAL(toggled(bool)),
          &m_layout.m_glwidget, SLOT(toggleDebugDrawGraphs(bool)));
  connect(m_ui_advanced.chk_annealing, SIGNAL(toggled(bool)), this,
          SLOT(onAnnealingToggled(bool)));

  connect(m_ui_advanced.chk_enableTree, SIGNAL(toggled(bool)), this,
          SLOT(onTreeToggled(bool)));

  connect(m_ui_advanced.txt_progress_threshold, &QLineEdit::textChanged, this,
          &SpringLayoutUi::onProgressThresholdChanged);
  connect(m_ui_advanced.txt_heating_factor, &QLineEdit::textChanged, this,
          &SpringLayoutUi::onHeatingFactorChanged);
  connect(m_ui_advanced.txt_cooling_factor, &QLineEdit::textChanged, this,
          &SpringLayoutUi::onCoolingFactorChanged);

  connect(m_ui_advanced.txt_stab_thres, &QLineEdit::textChanged, this,
          &SpringLayoutUi::onStabilityThresholdChanged);
  connect(m_ui_advanced.txt_stab_iters, &QLineEdit::textChanged, this,
          &SpringLayoutUi::onStabilityIterationsChanged);
 
  connect(m_ui_advanced.cmd_reset_positions, &QPushButton::pressed, this,
          &SpringLayoutUi::onResetPositionsPressed);

  //connect(m_ui_advanced_dialog, SIGNAL(finished(int)), this,
  //        SLOT(onAdvancedDialogShow(false)));
  //connect(m_ui_advanced_dialog, SIGNAL(accepted()), this,
  //        SLOT(onAdvancedDialogShow(false)));
  //connect(m_ui_advanced_dialog, SIGNAL(rejected()), this,
  //        SLOT(onAdvancedDialogShow(false)));


}

SpringLayoutUi::~SpringLayoutUi()
{
  if (m_thread != nullptr)
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
  }
}
/// @brief Notifies layout that rules have changed
void SpringLayoutUi::layoutRulesChanged()
{
  m_layout.rulesChanged();
}

QByteArray SpringLayoutUi::settings()
{
  return SettingsManager::getSettings("SpringLayoutUi")->save();
}


void SpringLayoutUi::setSettings(QByteArray state)
{
  SettingsManager::getSettings("SpringLayoutUi")->load(state);
  layoutRulesChanged();
}



void SpringLayoutUi::onProgressThresholdChanged(const QString& text)
{
  bool success;
  int num = text.toInt(&success);
  if (success && num > 0)
  {
    m_layout.m_asa.setProgressThreshold(num);
    mCRL2log(mcrl2::log::debug)
        << "Setting progress threshold to: " << num << std::endl;
  }
}

void SpringLayoutUi::onHeatingFactorChanged(const QString& text)
{
  bool success;
  float num = text.toFloat(&success);
  if (success && num > 0)
  {
    m_layout.m_asa.setHeatingFactor(num);
    mCRL2log(mcrl2::log::debug)
        << "Setting heating factor to: " << num << std::endl;
  }
}

void SpringLayoutUi::onCoolingFactorChanged(const QString& text)
{
  bool success;
  float num = text.toFloat(&success);
  if (success && num > 0)
  {
    m_layout.m_asa.setCoolingFactor(num);
    mCRL2log(mcrl2::log::debug)
        << "Setting cooling factor to: " << num << std::endl;
  }
}

void SpringLayoutUi::onStabilityThresholdChanged(const QString& text)
{
  bool success;
  float num = text.toFloat(&success);
  if (success && num > 0)
  {
    m_layout.m_stabilityThreshold = num;
    mCRL2log(mcrl2::log::debug)
        << "Setting stability threshold to: " << num << std::endl;
  }
}

void SpringLayoutUi::onStabilityIterationsChanged(const QString& text)
{
  bool success;
  float num = text.toInt(&success);
  if (success && num > 0)
  {
    m_layout.m_stabilityMaxCount = num;
    mCRL2log(mcrl2::log::debug)
        << "Setting stability iterations to: " << num << std::endl;

  }
}

void SpringLayoutUi::onResetPositionsPressed()
{
  m_layout.resetPositions();
}

void SpringLayoutUi::onAttractionChanged(int value)
{
  m_layout.setAttraction(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onRepulsionChanged(int value)
{
  m_layout.setRepulsion(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onSpeedChanged(int value)
{
  m_layout.setSpeed(value);
  update();
}

void SpringLayoutUi::onAccuracyChanged(int value)
{
  m_layout.setAccuracy(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onHandleWeightChanged(int value)
{
  m_layout.setControlPointWeight(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onNatLengthChanged(int value)
{
  m_layout.setNaturalTransitionLength(value);
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onAttractionCalculationChanged(int value)
{
  m_layout.setAttractionCalculation(
      static_cast<SpringLayout::AttractionCalculation>(value));
  m_ui_advanced.cmb_attr->setCurrentIndex(
      (int)m_layout.attractionCalculation());
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onRepulsionCalculationChanged(int value)
{
  m_layout.setRepulsionCalculation(
      static_cast<SpringLayout::RepulsionCalculation>(value));
  m_ui_advanced.cmb_rep->setCurrentIndex((int)m_layout.repulsionCalculation());
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onForceApplicationChanged(int value)
{
  m_layout.setForceApplication(
      static_cast<SpringLayout::ForceApplication>(value));
  m_ui_advanced.cmb_appl->setCurrentIndex((int)m_layout.forceApplication());
  layoutRulesChanged();
  update();
}

void SpringLayoutUi::onStarted()
{
  m_ui.btnStartStop->setText("Stop");
  m_ui.btnStartStop->setEnabled(true);
  update();
}

void SpringLayoutUi::onStopped()
{
  m_ui.btnStartStop->setText("Start");
  m_ui.btnStartStop->setEnabled(true);
  runningChanged(false);
  update();
}

void SpringLayoutUi::onTreeToggled(bool b)
{
  m_layout.setTreeEnabled(b);
  update();
}

void SpringLayoutUi::onAnnealingToggled(bool b)
{
  m_layout.setAnnealingEnabled(b);
}

void SpringLayoutUi::onStartStop()
{
  m_ui.btnStartStop->setEnabled(false);
  if (m_thread == nullptr)
  {
    emit runningChanged(true);
    layoutRulesChanged(); // force update
    m_thread = new WorkerThread(m_layout, this);
    m_thread->connect(m_thread, SIGNAL(started()), this, SLOT(onStarted()));
    m_thread->connect(m_thread, SIGNAL(finished()), this, SLOT(onStopped()));
    m_thread->start();
  }
  else
  {
    dynamic_cast<WorkerThread*>(m_thread)->stop();
    m_thread->wait();
    m_thread = nullptr;
  }
  update();
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
  update();
}

void SpringLayoutUi::onAdvancedDialogShow(bool b)
{
  mCRL2log(mcrl2::log::debug) << "OnAdvancedDialogShow called." << std::endl;
  if (b)
  {
    mCRL2log(mcrl2::log::debug) << "Opening advanced dialog." << std::endl;
    m_ui_advanced_dialog->show();
  }
  else
  {
    mCRL2log(mcrl2::log::debug) << "Closing advanced dialog." << std::endl;
    m_ui_advanced_dialog->hide();
  }
}

} // namespace Graph
