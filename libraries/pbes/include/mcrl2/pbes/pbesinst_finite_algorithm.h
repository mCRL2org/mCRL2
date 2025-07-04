// Author(s): Wieger Wesselink; Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_finite_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H
#define MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"

namespace mcrl2::pbes_system
{

/// \brief Data structure for storing the indices of the variables that should be expanded by the finite pbesinst algorithm.
using pbesinst_index_map = std::map<core::identifier_string, std::vector<std::size_t>>;

/// \brief Data structure for storing the variables that should be expanded by the finite pbesinst algorithm.
using pbesinst_variable_map = std::map<core::identifier_string, std::vector<data::variable>>;

/// \brief Function object for renaming a propositional variable instantiation
struct pbesinst_finite_rename
{
  protected:
    mutable std::unordered_map<propositional_variable_instantiation, core::identifier_string> m;
    mutable data::set_identifier_generator id_generator;

    core::identifier_string rename(const core::identifier_string& name, const data::data_expression_list& parameters) const
    {
      std::ostringstream out;
      out << std::string(name);
      for (const data::data_expression& param: parameters)
      {
        out << "_" << data::pp(param);
      }
      return core::identifier_string(out.str());
    }

  public:
    /// \brief Renames the propositional variable x.
    core::identifier_string operator()(const core::identifier_string& name, const data::data_expression_list& parameters) const
    {
      propositional_variable_instantiation P(name, parameters);
      auto i = m.find(P);
      if (i == m.end())
      {
        core::identifier_string dest = rename(name, parameters);
        if (id_generator.has_identifier(dest))
        {
          dest = id_generator(dest);
        }
        else
        {
          id_generator.add_identifier(dest);
        }
        m[P] = dest;
        return dest;
      }
      else
      {
        return i->second;
      }
    }
};

/// \brief Exception that is used to signal an empty parameter selection
struct empty_parameter_selection: public mcrl2::runtime_error
{
  explicit empty_parameter_selection(const std::string& msg)
    : mcrl2::runtime_error(msg)
  {}
};

namespace detail
{

/// \brief Computes the subset with variables of finite sort and infinite.
/// \param X A propositional variable instantiation
/// \param index_map a container storing the indices of the variables that
///        should be expanded by the finite pbesinst algorithm.
/// \param finite A sequence of data expressions
/// \param infinite A sequence of data expressions
template <typename PropositionalVariable, typename Parameter>
void split_parameters(const PropositionalVariable& X,
                      const pbesinst_index_map& index_map,
                      std::vector<Parameter>& finite,
                      std::vector<Parameter>& infinite
                     )
{
  auto pi = index_map.find(X.name());
  assert(pi != index_map.end());
  const std::vector<std::size_t>& v = pi->second;
  auto i = X.parameters().begin();
  std::size_t index = 0;
  auto j = v.begin();
  for (; i != X.parameters().end(); ++i, ++index)
  {
    if (j != v.end() && index == *j)
    {
      finite.push_back(*i);
      ++j;
    }
    else
    {
      infinite.push_back(*i);
    }
  }
}

/// \brief Visitor that applies a propositional variable substitution to a pbes expression.
template <typename DataRewriter, typename SubstitutionFunction>
struct pbesinst_finite_builder: public pbes_system::detail::data_rewriter_builder<pbesinst_finite_builder<DataRewriter, SubstitutionFunction>, DataRewriter, SubstitutionFunction>
{
  using super = pbes_system::detail::data_rewriter_builder<pbesinst_finite_builder, DataRewriter, SubstitutionFunction>;
  using super::apply;
  using super::sigma;

  const pbesinst_finite_rename& m_rename;
  const data::data_specification& m_data_spec;
  const pbesinst_index_map& m_index_map;
  const pbesinst_variable_map& m_variable_map;

  pbesinst_finite_builder(const DataRewriter& R,
                          SubstitutionFunction& sigma,
                          const pbesinst_finite_rename& rho,
                          const data::data_specification& data_spec,
                          const pbesinst_index_map& index_map,
                          const pbesinst_variable_map& variable_map
                         )
    : super(R, sigma),
      m_rename(rho),
      m_data_spec(data_spec),
      m_index_map(index_map),
      m_variable_map(variable_map)
  {}

  std::string print_parameters(const std::vector<data::data_expression>& finite_parameters,
                               const std::vector<data::data_expression>& infinite_parameters
                              ) const
  {
    std::ostringstream out;
    out << "<finite>";
    for (const data::data_expression& e: finite_parameters)
    {
      out << e << " ";
    }
    out << "<infinite>";
    for (const data::data_expression& e: infinite_parameters)
    {
      out << e << " ";
    }
    out << std::endl;
    return out.str();
  }

  /// \brief Computes the condition 'for all i: variables[i] == expressions[i]'.
  template <typename VariableContainer, typename ExpressionContainer>
  data::data_expression make_condition(const VariableContainer& variables, const ExpressionContainer& expressions) const
  {
    assert(variables.size() == expressions.size());
    if (variables.empty())
    {
      return data::true_();
    }
    auto vi = variables.begin();
    auto ei = expressions.begin();
    data::data_expression result = data::equal_to(*vi, *ei);
    ++vi;
    ++ei;
    for (; vi != variables.end(); ++vi, ++ei)
    {
      result = data::and_(result, data::equal_to(*vi, *ei));
    }
    return result;
  }

  template <typename DataExpressionContainer>
  data::data_expression_list rewrite_container(const DataExpressionContainer& v, const data::rewriter& rewr)
  {
    return data::data_expression_list(v.begin(), v.end(), [&](const data::data_expression& x) { return rewr(x); });
  }

  template <typename DataExpressionContainer>
  data::data_expression_list rewrite_container(const DataExpressionContainer& v, const data::rewriter& rewr, const data::mutable_indexed_substitution<>& sigma)
  {
    return data::data_expression_list(v.begin(), v.end(), [&](const data::data_expression& x) { return rewr(x, sigma); });
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    using enumerator_element = data::enumerator_list_element_with_substitution<>;

    // TODO: this code contains too much conversion between vectors and aterm lists
    std::vector<data::data_expression> finite_parameters;
    std::vector<data::data_expression> infinite_parameters;
    split_parameters(x, m_index_map, finite_parameters, infinite_parameters);
    mCRL2log(log::debug) << print_parameters(finite_parameters, infinite_parameters);
    data::data_expression_list d(finite_parameters.begin(), finite_parameters.end());
    data::data_expression_list e(infinite_parameters.begin(), infinite_parameters.end());
    const core::identifier_string& Xi = x.name();
    // x = Xi(d,e)

    auto vi = m_variable_map.find(Xi);
    std::vector<data::variable> di;
    if (vi != m_variable_map.end())
    {
      di = vi->second;
    }

    std::set<pbes_expression> result_set;
    bool accept_solutions_with_variables = false;
    data::enumerator_identifier_generator id_generator;
    data::enumerator_algorithm<> E(super::R, m_data_spec, super::R, id_generator, accept_solutions_with_variables);
    const data::variable_list di_list(di.begin(), di.end());
    data::mutable_indexed_substitution<> local_sigma;
    E.enumerate(enumerator_element(di_list, data::true_()),
                local_sigma,
                [&](const enumerator_element& p) {
                    data::mutable_indexed_substitution<> sigma_i;
                    p.add_assignments(di_list, sigma_i, super::R);
                    data::data_expression_list d_copy = rewrite_container(d, super::R, sigma);
                    data::data_expression_list e_copy = rewrite_container(e, super::R, sigma);
                    data::data_expression_list di_copy(di_list);
                    di_copy = data::replace_free_variables(di_copy, sigma_i);
                    data::data_expression c = make_condition(di_copy, d_copy);
                    core::identifier_string Y = m_rename(Xi, di_copy);
                    result_set.insert(and_(c, propositional_variable_instantiation(Y, e_copy)));
                    return false;
                }
    );
    result = join_or(result_set.begin(), result_set.end());
  }

  /// \return Visits the initial state
  propositional_variable_instantiation visit_initial_state(const propositional_variable_instantiation& init)
  {
    std::vector<data::data_expression> finite_parameters_vector;
    std::vector<data::data_expression> infinite_parameters_vector;
    split_parameters(init, m_index_map, finite_parameters_vector, infinite_parameters_vector);

    data::data_expression_list finite_parameters = rewrite_container(finite_parameters_vector, super::R);
    data::data_expression_list infinite_parameters = rewrite_container(infinite_parameters_vector, super::R);
    core::identifier_string X = m_rename(init.name(), finite_parameters);
    return propositional_variable_instantiation(X, infinite_parameters);
  }
};

} // namespace detail

/// \brief Algorithm class for the finite pbesinst algorithm.
class pbesinst_finite_algorithm
{
  protected:
    /// \brief The strategy of the data rewriter.
    data::rewriter::strategy m_rewriter_strategy;

    /// \brief The number of generated equations.
    std::size_t m_equation_count = 0;

    /// \brief Identifier generator for the enumerator
    data::enumerator_identifier_generator m_id_generator;

    /// \brief Returns true if the container contains the given element
    void compute_index_map(const std::vector<pbes_equation>& equations,
                           const pbesinst_variable_map& variable_map,
                           pbesinst_index_map& index_map)
    {
      using utilities::detail::contains;
      for (const pbes_equation& eqn: equations)
      {
        const core::identifier_string& name = eqn.variable().name();
        const data::variable_list& parameters = eqn.variable().parameters();

        std::vector<std::size_t> v;
        auto j = variable_map.find(name);
        if (j != variable_map.end())
        {
          std::size_t index = 0;
          for (auto k = parameters.begin(); k != parameters.end(); ++k, ++index)
          {
            if (contains(j->second, *k))
            {
              v.push_back(index);
            }
          }
        }
        index_map[name] = v;
      }
    }

    /// \brief Prints a message for every 1000-th equation
    std::string print_equation_count(std::size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

  public:

    /// \brief Constructor.
    /// \param rewriter_strategy Strategy to be used for the data rewriter.
    explicit pbesinst_finite_algorithm(data::rewriter::strategy rewriter_strategy = data::jitty)
      : m_rewriter_strategy(rewriter_strategy)
    {}

    /// \brief Runs the algorithm.
    /// \param pbesspec A PBES
    /// \param variable_map A map containing the finite parameters that should be expanded by the algorithm.
    void run(pbes& pbesspec, const pbesinst_variable_map& variable_map)
    {
      pbes_system::algorithms::instantiate_global_variables(pbesspec);
      pbesinst_finite_rename rename;
      m_equation_count = 0;

      // compute index map corresponding to the variable map
      pbesinst_index_map index_map;
      compute_index_map(pbesspec.equations(), variable_map, index_map);

      data::rewriter rewr(pbesspec.data(), m_rewriter_strategy);

      // compute new equations
      std::vector<pbes_equation> equations;
      for (const pbes_equation& eqn: pbesspec.equations())
      {
        std::vector<data::variable> finite_parameters;
        std::vector<data::variable> infinite_parameters;
        detail::split_parameters(eqn.variable(), index_map, finite_parameters, infinite_parameters);
        data::variable_list infinite(infinite_parameters.begin(), infinite_parameters.end());

        using enumerator_element = data::enumerator_list_element_with_substitution<>;
        bool accept_solutions_with_variables = false;
        data::enumerator_algorithm<> E(rewr, pbesspec.data(), rewr, m_id_generator, accept_solutions_with_variables);
        data::variable_list finite_parameter_list(finite_parameters.begin(), finite_parameters.end());
        data::mutable_indexed_substitution<> sigma;
        E.enumerate(enumerator_element(finite_parameter_list, data::true_()),
                    sigma,
                    [&](const enumerator_element& p) {
                      data::mutable_indexed_substitution<> sigma_j;
                      p.add_assignments(finite_parameter_list, sigma_j, rewr);
                      std::vector<data::data_expression> finite;
                      for (const data::variable& v: finite_parameters)
                      {
                        finite.push_back(sigma_j(v));
                      }
                      core::identifier_string name = rename(eqn.variable().name(), data::data_expression_list(finite.begin(), finite.end()));
                      propositional_variable X(name, infinite);
                      detail::pbesinst_finite_builder<data::rewriter, data::mutable_indexed_substitution<>> visitor(rewr, sigma_j, rename, pbesspec.data(), index_map, variable_map);
                      pbes_expression formula;
                      visitor.apply(formula, eqn.formula());
                      equations.emplace_back(eqn.symbol(), X, formula);
                      mCRL2log(log::debug) << print_equation_count(++m_equation_count);
                      mCRL2log(log::debug) << "Added equation " << pbes_system::pp(eqn) << "\n";
                      return false;
                    }
        );
      }

      // compute new initial state
      data::no_substitution sigma;
      detail::pbesinst_finite_builder<data::rewriter, data::no_substitution> visitor(rewr, sigma, rename, pbesspec.data(), index_map, variable_map);
      propositional_variable_instantiation initial_state = visitor.visit_initial_state(pbesspec.initial_state());

      // assign the result
      pbesspec.equations() = equations;
      pbesspec.initial_state() = initial_state;
    }

    /// \brief Runs the algorithm.
    /// \param p A PBES
    void run(pbes& p)
    {
      // put all finite variables in a variable map
      pbesinst_variable_map variable_map;
      for (const pbes_equation& eqn: p.equations())
      {
        for (const data::variable& v: eqn.variable().parameters())
        {
          if (p.data().is_certainly_finite(v.sort()))
          {
            variable_map[eqn.variable().name()].push_back(v);
          }
        }
      }

      run(p, variable_map);
    }
};

inline
void pbesinst_finite(pbes& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection)
{
  if (finite_parameter_selection.empty())
  {
    throw empty_parameter_selection("no finite parameters were selected!");
  }
  pbesinst_finite_algorithm algorithm(rewrite_strategy);
  pbes_system::detail::pbes_parameter_map parameter_map = pbes_system::detail::parse_pbes_parameter_map(p, finite_parameter_selection);

  bool is_empty = true;
  for (auto& i: parameter_map)
  {
    if (!((i.second).empty()))
    {
      is_empty = false;
      break;
    }
  }
  if (is_empty)
  {
    mCRL2log(log::verbose) << "Warning: no parameters were found that match the string \"" + finite_parameter_selection + "\"" << std::endl;
  }
  else
  {
    algorithm.run(p, parameter_map);
  }
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H
