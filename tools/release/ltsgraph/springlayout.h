// Author(s): Rimco Boudewijns and Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

/**

  @file springlayout.h
  @author S. Cranen, R. Boudewijns

  This file contains an implementation and user interface which enables automatic positioning for a graph.

*/

#ifndef SPRINGLAYOUT_H
#define SPRINGLAYOUT_H

#include <QDockWidget>
#include "ui_springlayout.h"
#include <QtOpenGL>

#include "glwidget.h"
#include "layoututility.h"
#include <map>
#include <QElapsedTimer>

namespace Graph
{


class SpringLayoutUi;

struct AttractionFunction;
struct RepulsionFunction;
struct ApplicationFunction;

class SimpleAdaptiveSimulatedAnnealing {
  public:
    float T;
    bool calculateTemperature(float new_energy);
    void reset();
  private:
    // Storage variables
    float m_prev_energy = -1; 
    float m_temperature = m_reset_temperature;
    int m_progress = 0;
    
    // Adaptive variables
    const int m_progress_threshold = 5;
    const float m_cooling_factor = 0.99f;
    const float m_heating_factor = 1 / 0.9f;
    
    // Interactive quality variables
    const float m_minimum_temperature = 1e-6;
    const float m_reset_temperature = 1;

};

class AdaptiveSimulatedAnnealing{
  float m_annealing_factor = 0.995;   ///< Adaptive cooling per progress cycle (multiplicative)
  float m_annealing_term = 0.1;    ///< If the system ever cools completely we need to be able to get out
  float m_energy_smoothing = 0.8; ///< smooths out energy

  float m_relative_change_threshold = 0.001f; ///< We count 'improvement' if it is more than m_relative_change_threshold

  float m_progress = 0; ///< current amount of 'cycles' spent progressing towards a goal
  float m_progress_threshold = 0.5; ///< threshold of 'cycles' spent before heating
  float m_progress_target_per_second = 50; ///< number of 'cycles' per second

  float m_progress_energy = -1;
  
  QElapsedTimer m_reset_timer;
  const float m_reset_duration = 0.4;       ///< in m_reset_duration seconds the temperature is interpolated to m_reset_temperature
  const float m_reset_temperature = 10;      ///< Temperature to reset to
  float m_reset_temperature_floor = 0;      ///< Used to interpolate from current temperature to m_reset_temperature during reset duration
  const float m_minimum_temperature = 1e-4; ///< Always have minimum movement to resolve forces that are left

  QElapsedTimer m_stable_timer; ///< currently disabled
  const float m_stability_energy_threshold = 1e-6; ///< if new_energy \in [prev_energy - energy_threshold, prev_energy + energy_threshold] we say it is stable
  const float m_stability_time_threshold = 2;        ///< how long do we require stability

  float m_previous_energy = -1; ///< Energy of the system, calculation is determined outside of class

  QElapsedTimer m_timer;        
  public:
  AdaptiveSimulatedAnnealing();
  float T;
  float m_temperature = 1; ///< The temperature is a step size multiplier
  /// @brief Calculates temperature for next layout cycle
  /// @param new_energy Energy of the system
  /// @return Boolean value indicating if stabilised
  bool calculateTemperature(float new_energy);
  float getTemperature();
  void reset();
};

class SpringLayout
{
  
  public:

    /**
     * @brief An enumeration that identifies the types of spring types which can be selected.
     */
    enum AttractionCalculation
    {
      ltsgraph_attr,                   ///< LTSGraph implementation.
      linearsprings_attr,              ///< Linear spring implementation.
      electricalsprings_attr,          ///< LTSGraph implementation using approximated repulsion forces.
      simplespring_attr,
    };

    enum RepulsionCalculation
    {
      ltsgraph_rep,
      electricalsprings_rep,
      none_rep,
    };

    enum ForceApplication
    {
      ltsgraph_appl, ///< Treat forces as speed
      force_directed_appl, ///< Treat forces as suggested direction
      force_directed_annealing_appl, ///< Include annealing for speed
      force_cumulative_appl,
    };

    enum TreeMode
    {
      octree,
      quadtree,
      none
    };

    enum ThreadingMode{
      normal,
    };

  private:
    AttractionCalculation m_option_attractionCalculation;
    RepulsionCalculation m_option_repulsionCalculation;
    ForceApplication m_option_forceApplication;

    std::size_t m_max_num_nodes = 0;
    std::size_t m_total_num_nodes = 0;

    Octree m_node_tree;
    Octree m_handle_tree;
    Octree m_trans_tree;
    Quadtree m_node_tree2D;
    Quadtree m_handle_tree2D;
    Quadtree m_trans_tree2D;

    // UI parameters
    const float m_min_speed = 0.00001f;
    const float m_max_speed = 10.0f;
    float m_speed;                   ///< The rate of change each step.
    float m_speed_scale_func(float s) const { return s*s; }
    float m_speed_inverse_scale_func(float s) const { return std::sqrt(s); }
    const float m_min_attraction = 0.0f;
    const float m_max_attraction = 1.0f;
    float m_attraction;              ///< The weight of the attraction of the edges.
    const float m_min_repulsion = 0.0f;
    const float m_max_repulsion = 1.0f;
    float m_repulsion;               ///< The weight of the repulsion of other nodes.
    const float m_min_natLength = 0.0f;   
    const float m_max_natLength = 100.0f;
    float m_natLength;               ///< The natural length of springs.
    const float m_min_controlPointWeight = 0.0f;
    const float m_max_controlPointWeight = 0.1f;
    float m_controlPointWeight;      ///< The handle repulsion wight factor.
    const float m_min_accuracy = 3.0f;
    const float m_max_accuracy = 0.0f;
    float m_accuracy;                ///< Controls the Barnes-Hut criterion in the approximation of repulsive forces
    bool m_tree_enabled;
    bool m_has_new_frame;
    std::vector<QVector3D> m_nforces, m_hforces, m_lforces, m_sforces;  ///< Vector of the calculated forces..
   
    QVector3D center_of_mass_offset; ///< When un-anchoring offset should be kept in mind
    bool any_anchored = false;
    QElapsedTimer drift_timer = QElapsedTimer();        ///< Timing since last anchoring
    float time_to_center = 3; ///< After 1s of nothing anchored we want the center of mass to be back at (0, 0, 0)

    Graph& m_graph;               ///< The graph on which the algorithm is applied.
    SpringLayoutUi* m_ui;         ///< The user interface generated by Qt.

    std::map<AttractionCalculation, AttractionFunction*> attrFuncMap;
    std::map<RepulsionCalculation, RepulsionFunction*> repFuncMap;
    std::map<ForceApplication, ApplicationFunction*> applFuncMap;
    AttractionFunction* m_attrFunc;
    RepulsionFunction* m_repFunc;
    ApplicationFunction* m_applFunc;
public:
    SimpleAdaptiveSimulatedAnnealing m_asa;
private:
    /**
     * @brief Returns approximate accumulation of all repulsive forces from other particles exerted on @e a
     * 
     * @param a Particle
     * @param tree Octree containing all particles
     * @param repulsion Scaling constant
     * @param natlength Other scaling constant
     * @return QVector3D Force exerted by all particles on particle @e a
     */
    template< typename TreeType >
    QVector3D approxRepulsionForce(const QVector3D& a, TreeType& tree);

    void forceAccumulation(bool sel, std::size_t nodeCount, std::size_t edgeCount, TreeMode treeMode, ThreadingMode threadingMode); 

    template< TreeMode mode >
    void repulsionAccumulation(bool sel, std::size_t nodeCount, std::size_t edgeCount);

    template< ThreadingMode mode >
    void attractionAccumulation(bool sel, std::size_t nodeCount, std::size_t edgeCount); 
  public:
    GLWidget& m_glwidget;

    /**
     * @brief Constructor of the algorithm for the given @e graph.
     * @param graph The graph on which the algorithm should be applied.
     */
    SpringLayout(Graph& graph, GLWidget& glwidget);

    /**
     * @brief Destructor.
     */
    virtual ~SpringLayout();

    /**
     * @brief Calculate the forces and update the positions.
     */
    void apply();

    /**
     * @brief Set the type of the force calculation.
     * @param c The desired calculaten 
     */
    void setAttractionCalculation(AttractionCalculation c);

    /**
     * @brief Returns the current force calculation used.
     */
    AttractionCalculation attractionCalculation();

    /**
     * @brief Set the type of the force calculation.
     * @param c The desired calculaten 
     */
    void setRepulsionCalculation(RepulsionCalculation c);

    /**
     * @brief Returns the current repulsion calculation used.
     */
    RepulsionCalculation repulsionCalculation();

    /**
     * @brief Set the type of force application.
     * @param c The desired way force will be applied to the graphs elements
     */
    void setForceApplication(ForceApplication c);

    /**
     * @brief Returns the current force application method.
     */
    ForceApplication forceApplication();

    /**
     * @brief Randomly moves nodes along the Z axis, at most [z] units
     * @param z The maximum distance that nodes are moved
     */
    void randomizeZ(float z);

    /**
     * @brief Pass-through whether graph is stable or not
     */
    const bool& isStable(){ return m_graph.stable(); };

    /**
     * @brief Returns the user interface object. If no user interface is available,
     *        one is created using the provided @e parent.
     * @param The parent of the user inferface in the case none exists yet.
     */
    SpringLayoutUi* ui(QWidget* parent = nullptr);

    //Getters and setters
    float lerp(int value, float targ_min, float targ_max, int _min = 0,
               int _max = 100)
    {
      return targ_min +
             (targ_max - targ_min) * (value - _min) / (float)(_max - _min);
    }
    int unlerp(float value, float val_min, float val_max, int targ_min = 0,
               int targ_max = 100) const
    {
      return targ_min +
             (targ_max - targ_min) * (value - val_min) / (val_max - val_min);
    }
    int speed() const {
      return unlerp(m_speed_inverse_scale_func(m_speed), m_speed_inverse_scale_func(m_min_speed), m_speed_inverse_scale_func(m_max_speed));
    }
    int attraction() const {
      return unlerp(m_attraction, m_min_attraction, m_max_attraction);
    }
    int repulsion() const {
      return unlerp(m_repulsion, m_min_repulsion, m_max_repulsion);
    }
    int controlPointWeight() const {
      return unlerp(m_controlPointWeight, m_min_controlPointWeight, m_max_controlPointWeight);
    }
    int naturalTransitionLength() const {
      return unlerp(m_natLength, m_min_natLength, m_max_natLength);
    }

    bool treeEnabled() const {
      return m_tree_enabled;
    }

    bool hasNewFrame() const {
      return m_has_new_frame;
    }

    void notifyNewFrame();
    void setTreeEnabled(bool b);
    void setSpeed(int v);
    void setAccuracy(int v);
    void setAttraction(int v);
    void setRepulsion(int v);
    void setControlPointWeight(int v);
    void setNaturalTransitionLength(int v);

    /// @brief Used to invalidate graph when settings change (i.e. attraction/repulsion)
    void rulesChanged();
};

class SpringLayoutUi : public QDockWidget
{
    Q_OBJECT
  private:
    SpringLayout& m_layout;     ///< The layout algorithm that corresponds to this user interface.
    QThread* m_thread;          ///< The thread that is used to calculate the new positions.
  public:
    Ui::DockWidgetLayout m_ui;  ///< The user interface generated by Qt.

    /**
     * @brief Constructor.
     * @param layout The layout object this user interface corresponds to.
     * @param parent The parent widget for this user interface.
     */
    SpringLayoutUi(SpringLayout& layout, QWidget* parent=nullptr);

    /**
     * @brief Destructor.
     */
    ~SpringLayoutUi() override;

    /**
     * @brief Get the current state of the settings.
     */
    QByteArray settings();

    /**
     * @brief Restore the settings of the given state.
     * @param state The original state
     */
    void setSettings(QByteArray state);

    /**
     * @brief Indicates that settings changed that influence the layout.
     * 
     */
    void layoutRulesChanged();
  signals:

    /**
     * @brief Indicates that the thread is started or stopped.
     */
    void runningChanged(bool);

  public slots:

    /**
     * @brief Updates the attraction value.
     * @param value The new value.
     */
    void onAttractionChanged(int value);

    /**
     * @brief Updates the repulsion value.
     * @param value The new value.
     */
    void onRepulsionChanged(int value);

    /**
     * @brief Updates the speed value.
     * @param value The new value.
     */
    void onSpeedChanged(int value);

    /**
     * @brief Updates accuracy value.
     * @param value New value.
     */
    void onAccuracyChanged(int value);

    /**
     * @brief Updates the handle weight.
     * @param value The new weight.
     */
    void onHandleWeightChanged(int value);

    /**
     * @brief Updates the natural length value.
     * @param value The new value.
     */
    void onNatLengthChanged(int value);

    /**
     * @brief Updates the attraction force calculation.
     * @param value The new index selected.
     */
    void onAttractionCalculationChanged(int value);

    /**
     * @brief Updates the repulsion force calculation.
     * @param value The new index selected.
     */
    void onRepulsionCalculationChanged(int value);

    /**
     * @brief Updates the force application.
     * @param value The new index selected.
     * 
     */
    void onForceApplicationChanged(int value);

    /**
     * @brief Starts or stops the force calculation depending on the current state.
     */
    void onStartStop();

    /**
     * @brief Starts the force calculation.
     */
    void onStarted();

    /**
     * @brief Stops the force calculation.
     */
    void onStopped();

    /**
     * @brief Enables/disables tree for acceleration
     */
    void onTreeToggled(bool);

    /**
     * @brief Starts or stops the force calculation.
     * @param active The calculation is started if this argument is true.
     */
    void setActive(bool active);
};


}  // namespace Graph

#endif // SPRINGLAYOUT_H
