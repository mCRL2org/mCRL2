// Author(s): Jan Friso Groote, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parvalues.h
/// \brief Explore statically which parameter values may be reached

#ifndef MCRL2_DATA_PARVALUES_H
#define MCRL2_DATA_PARVALUES_H

#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/utilities/views.h"

#include <functional>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace mcrl2::data::detail
{

template<class VariableContainer>
std::string ppsort(const VariableContainer& s)
{
  std::string result;
  for (const data::variable& v: s)
  {
    if (!result.empty())
    {
      result = result + ",";
    }
    result = result + pp(v) + ":" + pp(v.sort());
  }
  return result;
}

struct parameter
{
  const core::identifier_string eqn;
  const variable var;

  parameter(const core::identifier_string& e, const variable& v)
    : eqn(e),
      var(v)
  {}

  parameter(const variable& v)
    : eqn(""),
      var(v)
  {}

  bool operator==(const parameter& other) const
  {
    return eqn == other.eqn && var == other.var;
  }

  bool operator<(const parameter& other) const
  {
    return std::tie(eqn, var) < std::tie(other.eqn, other.var);
  }

  operator std::string() const
  {
    return eqn == core::identifier_string("") ? pp(var) : ("(" + pp(eqn) + ", " + pp(var) + ")");
  }
};

inline std::ostream& operator<<(std::ostream& out, const parameter& x)
{
  if (x.eqn == core::identifier_string(""))
  {
    return out << x.var.name() << ": " << x.var.sort();
  }
  return out << "(" << x.eqn << ", " << x.var.name() << ": " << x.var.sort() << ")";
}

struct elements_per_domain
{
  /// \brief Values which have already been propagated
  std::unordered_set<data::data_expression> stable;
  /// \brief Values which are currently being propagated (possibly combined with old values)
  std::unordered_set<data::data_expression> todo;
  /// \brief Values found in the current iterations (are not considered until the next iteration)
  std::unordered_set<data::data_expression> newly_found;
};

class influence_graph
{

public:

  struct edge
  {
    /// \brief The equation whose parameters occur freely in cond and updates.
    core::identifier_string source_eqn;
    /// \brief Variables bound in a quantifier, sum or other operator that influce the update (and possibly condition)
    variable_vector qvars;
    /// \brief Condition under which the updates happen
    data_expression cond;
    /// \brief Update expressions for parameters
    std::vector<std::pair<parameter, data_expression>> updates;

    bool operator<(const edge& other) const
    {
      return std::tie(source_eqn, qvars, cond, updates)
             < std::tie(other.source_eqn, other.qvars, other.cond, other.updates);
    }

    operator std::string() const
    {
      std::string result = "(" + pp(source_eqn) + ") [" + ppsort(qvars) + "]  " + pp(cond) + " -> ";
      for (const auto& [from, to]: updates)
      {
        result += std::string(from) + " := " + pp(to) + "; ";
      }
      return result;
    }
  };

private:

  std::map<parameter, elements_per_domain> m_nodes;
  std::set<edge> m_edges;
  /// \brief Parameters for which enumeration fell short; their reported domains may be incomplete.
  std::set<parameter> m_unbounded;

public:

  const std::set<edge>& edges() const
  {
    return m_edges;
  }

  // Mark that not all values that parameter v can reach could be enumerated.
  void mark_unbounded(const parameter& v)
  {
    m_unbounded.insert(v);
  }

  // Indicates whether the domain of parameter v may be unbounded or incomplete.
  bool is_unbounded(const parameter& v) const
  {
    return m_unbounded.contains(v);
  }

  template<typename VariableContainer, typename UpdateContainer>
  void add_edge(const core::identifier_string& source_eqn,
    const VariableContainer& vars,
    data_expression cond,
    const UpdateContainer& up)
  {
    variable_vector qvars(vars.begin(), vars.end());
    std::vector<std::pair<parameter, data_expression>> updates(up.begin(), up.end());
    m_edges.emplace(source_eqn, qvars, cond, updates);
  }

  const elements_per_domain& at(const parameter& v) const
  {
    assert(m_nodes.contains(v));
    return m_nodes.at(v);
  }

  // Insert a new element e in the domain of variable v.
  // Report true if the element was not yet present.
  bool contains(const parameter& v, const data::data_expression& e) const
  {
    return m_nodes.contains(v)
           && (m_nodes.at(v).stable.contains(e) || m_nodes.at(v).todo.contains(e)
               || m_nodes.at(v).newly_found.contains(e));
  }

  // Make a new parameter v with initial value e.
  void new_parameter(const parameter& v, const data::data_expression& e)
  {
    assert(!m_nodes.contains(v));
    m_nodes[v].newly_found.insert(e);
  }

  // Declare a parameter v without any value yet. Values can be inserted later,
  // e.g., when they are generated by updates of other parameters.
  void declare_parameter(const parameter& v)
  {
    assert(!m_nodes.contains(v));
    m_nodes.emplace(v, elements_per_domain{});
  }

  // Insert a new element e in the domain of variable v.
  // Report true if the element was not yet present.
  bool insert(const parameter& v, const data::data_expression& e)
  {
    if (contains(v, e))
    {
      return false;
    }
    m_nodes.at(v).newly_found.insert(e);
    return true;
  }

  // Checks whether there are domains with values from the previous round related to parameters that follow v.
  bool has_available_next_values_from_the_previous_round(const parameter& v)
  {
    auto find_it = m_nodes.find(v);
    if (find_it == m_nodes.end())
    {
      return false;
    }

    auto todo_available = [](const auto& e)
    {
      return !e.second.todo.empty();
    };
    return std::any_of(++find_it, m_nodes.end(), todo_available);
  }

  // If m_nodes[v].m_elements_added_in_current_round is not emptye for some variable v, the system is not stable.
  bool stable() const
  {
    auto is_stable = [](const auto& e)
    {
      return e.second.newly_found.empty();
    };
    return std::ranges::all_of(m_nodes, is_stable);
  }

  // Add the elements of the previous round to the stable elements, move the current round to the previous round,
  // and clear the current round for all parameters.
  void new_round()
  {
    for (auto& [par, values]: m_nodes)
    {
      values.stable.merge(values.todo);
      assert(values.todo.empty());
      std::swap(values.todo, values.newly_found);
    }
  }

  // Move all values that were found but not yet propagated to the stable
  // elements. Used when exploration is stopped prematurely.
  void flush()
  {
    for (auto& [par, values]: m_nodes)
    {
      values.stable.merge(values.todo);
      assert(values.todo.empty());
      values.stable.merge(values.newly_found);
      values.newly_found.clear();
    }
  }

  // Indicates whether exploration found a parameter whose domain cannot be
  // enumerated completely.
  bool has_unbounded() const
  {
    return !m_unbounded.empty();
  }

  // Provide the product size of the domains for the parameters.
  long double product_size()
  {
    if (!m_unbounded.empty())
    {
      return std::numeric_limits<long double>::infinity();
    }
    long double size = 1;
    for (const auto& [_, elms]: m_nodes)
    {
      const auto& [a, b, c] = elms;
      size = size * std::max(std::size_t(1), a.size() + b.size() + c.size());
    }
    return size;
  }

  // List the elements of the domains per variable. If joined=false they are split in stable and unstable elements;
  std::string report(bool joined = true)
  {
    std::stringstream output;
    for (const auto& elm: m_nodes)
    {
      output << "Parameter " << elm.first << " ("
             << (elm.second.stable.size() + elm.second.todo.size() + elm.second.newly_found.size()) << ")"
             << (!joined ? "\n  Stable elements: {" : ": {");
      bool first = true;
      for (const data::data_expression& e: elm.second.stable)
      {
        output << (first ? " " : ", ") << e;
        first = false;
      }
      if (!joined)
      {
        output << " }\n  Previous round: {";
        first = true;
      }
      for (const data::data_expression& e: elm.second.todo)
      {
        output << (first ? " " : ", ") << e;
        first = false;
      }
      if (!joined)
      {
        output << " }\n  Current round: {";
        first = true;
      }
      for (const data::data_expression& e: elm.second.newly_found)
      {
        output << (first ? " " : ", ") << e;
        first = false;
      }
      output << " }\n";
    }
    if (!m_unbounded.empty())
    {
      output << "Warning: the domains of the following parameter(s) could not be enumerated completely"
                " and may be unbounded:";
      for (const parameter& v: m_unbounded)
      {
        output << " " << std::string(v);
      }
      output << "\n";
    }
    output << "Upperbound on the statespace is " << product_size()
           << "\n---------------------------------------------------------------------\n";
    return output.str();
  }
};

/// \brief Algorithm class that can be used to apply the lps_explore_domains algorithm
///
/// All parameter values of the process parameters are enumerated.
template<typename DataRewriter>
class parvalues_algorithm
{
  using enumerator_element = data::enumerator_list_element_with_substitution<>;

protected:
  /// Rewriter
  DataRewriter m_rewriter;
  const data_specification m_dataspec;
  const std::size_t m_qlimit;
  const std::size_t m_maximal_number_of_rounds;
  data::enumerator_identifier_generator& m_generator;

  detail::influence_graph m_graph;

  void propagate_values_qvars(mutable_indexed_substitution<>& sigma,
    const core::identifier_string& source_eqn,
    const parameter& v,
    const variable_vector& qvars,
    const data_expression& condition,
    const data_expression& update_expr,
    const bool used_new_value)
  {
    // Free variables of the condition and the update belong to the equation
    // source_eqn; their domains are looked up under (source_eqn, variable).
    data_expression rewritten_condition = m_rewriter(condition, sigma);
    std::set<variable> parameters_in_condition = find_free_variables(rewritten_condition);
    for (const variable& qv: qvars)
    {
      parameters_in_condition.erase(qv);
    }

    // Base case: both condition and update do not contain parameters
    // We need to enumerate the remaining sumvars/quantified variables
    if (parameters_in_condition.empty())
    {
      const bool target_has_values = !m_graph.at(v).stable.empty() || !m_graph.at(v).todo.empty();
      if ((!used_new_value && target_has_values) || rewritten_condition == sort_bool::false_())
      {
        // No new values were used to reach this update, and the values that it
        // generates have been generated before.
        return;
      }

      // First split the sumvars in those that occur in rewritten_new_expression and those that do not.
      const std::set<variable> update_fv = find_free_variables(update_expr);
      const std::set<variable> cond_fv   = find_free_variables(rewritten_condition);
      
      auto is_in_update         = [&](const variable& v){ return update_fv.contains(v); };
      auto is_in_cond_or_update = [&](const variable& v){ return update_fv.contains(v) || cond_fv.contains(v); };
      auto is_not_in_update     = [&](const variable& v){ return !update_fv.contains(v) && cond_fv.contains(v); };
      
      const variable_list qvars_in_update        {qvars | std::views::filter(is_in_update)};
      const variable_list qvars_in_cond_or_update{qvars | std::views::filter(is_in_cond_or_update)};
      const variable_list qvars_not_in_update{qvars | std::views::filter(is_not_in_update)};

      // If the condition does not restrict the variables in the update and one
      // of them ranges over an infinite sort, then every value of those
      // variables gives rise to a new value for v. Such a domain cannot be
      // captured by enumeration, so report it as unbounded.
      if (rewritten_condition == sort_bool::true_()
          && std::ranges::any_of(qvars_in_update,
            [&](const variable& w) { return !m_dataspec.is_certainly_finite(w.sort()); }))
      {
        m_graph.mark_unbounded(v);
        mCRL2log(log::warning) << "The domain of parameter " << std::string(v)
                               << " depends on unconstrained variable(s) " << detail::ppsort(qvars_in_update)
                               << " ranging over an infinite sort; it cannot be enumerated and may be unbounded.\n";
        return;
      }

      data_expression quantified_condition;
      make_optimized_exists(quantified_condition, qvars_not_in_update, rewritten_condition);

      mCRL2log(log::debug) << "Enumerate " << detail::ppsort(qvars_in_update) << " in " << quantified_condition << "\n";

      const std::size_t enumeration_limit = m_qlimit;
      enumerator_algorithm<> enumerator(m_rewriter, m_dataspec, m_rewriter, m_generator, false, enumeration_limit);

      /* Create a list to store solutions */
      std::size_t count = enumerator.enumerate(
        enumerator_element(qvars_in_cond_or_update, rewritten_condition),
        sigma,
        [&](const enumerator_element& p)
        {
          p.add_assignments(qvars_in_update, sigma, m_rewriter);
          m_graph.insert(v, m_rewriter(update_expr, sigma));
          assert(find_free_variables(m_rewriter(update_expr, sigma)).empty());
          p.remove_assignments(qvars_in_update, sigma);
          // Stop if the condition is true and no variables occur in the new_expression.
          return update_fv.empty() && p.expression() == sort_bool::true_();
        },
        // Ignore the following solution.
        [&](const data_expression& d) -> bool
        {
          if (find_free_variables(d).empty() && d != sort_bool::true_() && d != sort_bool::false_())
          {
            mCRL2log(log::warning) << "The expression " << d
                                   << " does not rewrite to true or false. It is assumed to be true.\n";
          }
          return d == sort_bool::false_();
        },
        [](const data_expression& /*d*/) -> bool { return false; });
      if (count >= enumeration_limit)
      {
        if (update_fv.empty())
        {
          m_graph.insert(v, update_expr);
        }
        else
        {
          throw mcrl2::runtime_error("Cannot enumerate " + detail::ppsort(qvars_in_cond_or_update) + " in " + pp(rewritten_condition) + ". Using the flag --qlimit with a higher value may help. \n");
        }
      }

      return;
    }

    variable curr_var = *parameters_in_condition.begin();
    const parameter curr_par{source_eqn, curr_var};
    {
      for (const data_expression& e: m_graph.at(curr_par).stable)
      {
        sigma[curr_var] = e;
        propagate_values_qvars(sigma, source_eqn, v, qvars, rewritten_condition, update_expr, used_new_value);
      }
    }

    for (const data_expression& e: m_graph.at(curr_par).todo)
    {
      sigma[curr_var] = e;
      propagate_values_qvars(sigma, source_eqn, v, qvars, rewritten_condition, update_expr, true);
    }
    sigma[curr_var] = curr_var;

    if (m_graph.is_unbounded(curr_par))
    {
      m_graph.mark_unbounded(v);
    }

    return;
  }

  void propagate_values(mutable_indexed_substitution<>& sigma,
    const core::identifier_string& source_eqn,
    const parameter& v,
    const variable_vector& qvars,
    const data_expression& condition,
    const data_expression& update_expr,
    const bool used_new_value)
  {
    // We consider the free variables in the new expression relevant as they determine the new value parameter v can
    // get.
    const data_expression update_expr_rewritten = m_rewriter(update_expr, sigma);
    std::set<variable> relevant_parameters = find_free_variables(update_expr_rewritten);
    bool update_is_closed = relevant_parameters.empty();
    // Split the relevant variables in those that are parameters and those that occur in the sum variables
    for (const variable& qv: qvars)
    {
      relevant_parameters.erase(qv);
    }

    // Rewritten update is closed, we can return in some cases
    if (relevant_parameters.empty())
    {
      const data_expression rewritten_condition = m_rewriter(condition, sigma);
      if (rewritten_condition == sort_bool::false_())
      {
        return;
      }

      if (update_is_closed && m_graph.contains(v, update_expr_rewritten))
      {
        return;
      }
      if (update_is_closed && rewritten_condition == sort_bool::true_())
      {
        m_graph.insert(v, update_expr_rewritten);
        return;
      }

      propagate_values_qvars(sigma, source_eqn, v, qvars, rewritten_condition, update_expr_rewritten, used_new_value);
      return;
    }

    // Pick one relevant variable and iterate over its possible values.
    // It belongs to the equation source_eqn.
    const variable curr_var = *relevant_parameters.begin();
    const parameter curr_par{source_eqn, curr_var};
    if (true || m_graph.has_available_next_values_from_the_previous_round(curr_par)) // Zoek uit of dit altijd moet....
    {
      for (const data_expression& e: m_graph.at(curr_par).stable)
      {
        sigma[curr_var] = e;
        propagate_values(sigma, source_eqn, v, qvars, condition, update_expr_rewritten, used_new_value);
      }
    }

    for (const data_expression& e: m_graph.at(curr_par).todo)
    {
      sigma[curr_var] = e;
      propagate_values(sigma, source_eqn, v, qvars, condition, update_expr_rewritten, true);
    }
    sigma[curr_var] = curr_var;

    // Updates induced by valuations of curr_var outside its known domain are
    // not covered, hence inherit its potentially unbounded status.
    if (m_graph.is_unbounded(curr_par))
    {
      m_graph.mark_unbounded(v);
    }

    return;
  }

public:
  /// \brief Constructor for lps_explore_domains algorithm
  /// \param spec Specification to which the algorithm should be applied
  /// \param r a rewriter for data
  parvalues_algorithm(DataRewriter& r,
    const data_specification& dataspec,
    const std::size_t qlimit,
    const std::size_t maximal_number_of_rounds)

    : m_rewriter(r),
      m_dataspec(dataspec),
      m_qlimit(qlimit),
      m_maximal_number_of_rounds(maximal_number_of_rounds),
      m_generator(const_cast<data::enumerator_identifier_generator&>(r.identifier_generator()))
  {}

  /// \brief Apply the algorithm to the specification passed in the
  ///        constructor
  void run()
  {
    mCRL2log(log::debug) << "Start to explore parameter domains" << std::endl;

    std::size_t round = 0;
    bool stopped_on_unbounded = false;
    while (round < m_maximal_number_of_rounds && !m_graph.stable())
    {
      mCRL2log(log::verbose) << "Parameter instantiation round " << round
                             << " (estimated upperbound on the state space: " << m_graph.product_size() << ").\n";
      mCRL2log(log::debug) << m_graph.report(true);

      round++;
      m_graph.new_round();

      for (const influence_graph::edge& edge: m_graph.edges())
      {
        mCRL2log(log::debug)
          << "Process edge (round " << round << ") " << std::string(edge)
          << "\n==================================================================================\n";

        for (const auto& [par, expr]: edge.updates)
        {
          if (par.var == expr)
          {
            continue;
          }

          data::mutable_indexed_substitution<> sigma;
          propagate_values(sigma, edge.source_eqn, par, edge.qvars, edge.cond, expr, false);
        }
      }

      // As soon as a domain is found to be unbounded, the upperbound on the
      // state space is infinite and further rounds cannot repair that.
      if (m_graph.has_unbounded())
      {
        stopped_on_unbounded = true;
        break;
      }
    }
    if (stopped_on_unbounded)
    {
      mCRL2log(log::warning) << "Exploration stopped after round " << round
                             << ": at least one parameter domain is unbounded,"
                             << " hence the state space is infinite. The reported domains are incomplete.\n";
      m_graph.flush();
    }
    else if (round == m_maximal_number_of_rounds)
    {
      mCRL2log(log::warning)
        << "The maximal number of rounds (" << round << ") has been reached. "
        << "Exploration is stopped prematurely. The domains and the upperbound of the state space can be too low.\n";
      m_graph.new_round();
    }
    else
    {
      m_graph.new_round();
    }

    mCRL2log(log::verbose) << m_graph.report(true);
    mCRL2log(log::info) << "This process has at most " << m_graph.product_size() << " states.\n";
  }
};

} // namespace mcrl2::data::detail

#endif
