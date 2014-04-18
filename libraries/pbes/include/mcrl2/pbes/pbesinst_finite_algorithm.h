// Author(s): Wieger Wesselink; Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_finite_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H
#define MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H

#define MCRL2_PBESINST_DEBUG

#include <algorithm>
#include <vector>
#include <set>
#include <sstream>
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/detail/rewrite_container.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/pbes/detail/pbes_parameter_map.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Data structure for storing the indices of the variables that should be expanded by the finite pbesinst algorithm.
typedef std::map<core::identifier_string, std::vector<size_t> > pbesinst_index_map;

/// \brief Data structure for storing the variables that should be expanded by the finite pbesinst algorithm.
typedef std::map<core::identifier_string, std::vector<data::variable> > pbesinst_variable_map;

/// \brief Function object for renaming a propositional variable instantiation
struct pbesinst_finite_rename
{
  /// \brief Renames the propositional variable x.
  template <typename ExpressionContainer>
  core::identifier_string operator()(const core::identifier_string& name, const ExpressionContainer& parameters) const
  {
    std::ostringstream out;
    out << std::string(name);
    for (typename ExpressionContainer::const_iterator i = parameters.begin(); i != parameters.end(); ++i)
    {
      out << "@" << data::pp(*i);
    }
    return core::identifier_string(out.str());
  }
};

/// \brief Exception that is used to signal an empty parameter selection
struct empty_parameter_selection: public mcrl2::runtime_error
{
  empty_parameter_selection(const std::string& msg)
    : mcrl2::runtime_error(msg)
  {}
};

namespace detail
{

template <typename Function1, typename Function2>
struct compose
{
  typedef typename Function1::result_type result_type;
  typedef typename Function1::argument_type argument_type;
  typedef typename Function1::variable_type variable_type;
  typedef typename Function1::expression_type expression_type;

  const Function1& f1_;
  const Function2& f2_;

  compose(const Function1& f1, const Function2& f2)
    : f1_(f1), f2_(f2)
  {}

  result_type operator()(const argument_type& x)
  {
    return f1_(f2_(x));
  }
};

template <typename Function1, typename Function2>
compose<Function1, Function2> make_compose(const Function1& f1, const Function2& f2)
{
  return compose<Function1, Function2>(f1, f2);
}

/// \brief Computes the subset with variables of finite sort and infinite.
/// \param X A propositional variable instantiation
/// \param index_map a container storing the indices of the variables that
///        should be expanded by the finite pbesinst algorithm.
/// \param finite A sequence of data expressions
/// \param infinite A sequence of data expressions
template <typename PropositionalVariable>
void split_parameters(const PropositionalVariable& X,
                      const pbesinst_index_map& index_map,
                      std::vector<typename PropositionalVariable::parameter_type>& finite,
                      std::vector<typename PropositionalVariable::parameter_type>& infinite
                     )
{
  typedef typename PropositionalVariable::parameter_type parameter_type;
  pbesinst_index_map::const_iterator pi = index_map.find(X.name());
  assert(pi != index_map.end());
  const std::vector<size_t>& v = pi->second;
  typename atermpp::term_list<parameter_type>::const_iterator i = X.parameters().begin();
  size_t index = 0;
  std::vector<size_t>::const_iterator j = v.begin();
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
  typedef pbes_system::detail::data_rewriter_builder<pbesinst_finite_builder, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::sigma;

  typedef core::term_traits<pbes_expression> tr;

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
    for (auto i = finite_parameters.begin(); i != finite_parameters.end(); ++i)
    {
      out << *i << " ";
    }
    out << "<infinite>";
    for (auto i = infinite_parameters.begin(); i != infinite_parameters.end(); ++i)
    {
      out << *i << " ";
    }
    out << std::endl;
    return out.str();
  }

  /// \brief Computes the condition 'for all i: variables[i] == expressions[i]'.
  template <typename VariableContainer, typename ExpressionContainer>
  data::data_expression make_condition(const VariableContainer& variables, const ExpressionContainer& expressions) const
  {
    using namespace data::sort_bool;
    assert(variables.size() == expressions.size());
    if (variables.empty())
    {
      return data::sort_bool::true_();
    }
    typename VariableContainer::const_iterator vi = variables.begin();
    typename ExpressionContainer::const_iterator ei = expressions.begin();
    data::data_expression result = equal_to(*vi, *ei);
    ++vi;
    ++ei;
    for (; vi != variables.end(); ++vi, ++ei)
    {
      result = data::sort_bool::and_(result, equal_to(*vi, *ei));
    }
    return result;
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    // TODO: this code contains too much conversion between vectors and aterm lists
    std::vector<data::data_expression> finite_parameters;
    std::vector<data::data_expression> infinite_parameters;
    split_parameters(x, m_index_map, finite_parameters, infinite_parameters);
    mCRL2log(log::debug, "pbesinst_finite") << print_parameters(finite_parameters, infinite_parameters);
    data::data_expression_list d = atermpp::convert<data::data_expression_list>(finite_parameters);
    data::data_expression_list e = atermpp::convert<data::data_expression_list>(infinite_parameters);
    core::identifier_string Xi = x.name();
    // x = Xi(d,e)

    pbesinst_variable_map::const_iterator vi = m_variable_map.find(Xi);
    std::vector<data::variable> di;
    if (vi != m_variable_map.end())
    {
      di = vi->second;
    }

    std::set<pbes_expression> result;
    data::classic_enumerator<> enumerator(m_data_spec, super::R);
    mcrl2::data::mutable_indexed_substitution<> local_sigma;
    for (auto i = enumerator.begin(data::variable_list(di.begin(), di.end()), data::sort_bool::true_(), local_sigma); i != enumerator.end(); ++i)
    {
      mCRL2log(log::debug1) << "sigma = " << sigma << "\n";
      data::mutable_indexed_substitution<> sigma_i;
      data::data_expression_list::const_iterator k = i->begin();
      for (auto j = di.begin(); j != di.end(); ++j, ++k)
      {
        sigma_i[*j]=*k;
      }
      mCRL2log(log::debug1) << "*i    = " << sigma_i << "\n";
      data::data_expression_list d_copy = d;
      data::detail::rewrite_container(d_copy, super::R, sigma);
      data::data_expression_list e_copy = e;
      data::detail::rewrite_container(e_copy, super::R, sigma);

      data::data_expression_list di_copy = atermpp::convert<data::data_expression_list>(di);
      di_copy = data::replace_free_variables(di_copy, sigma_i);

      data::data_expression c = make_condition(di_copy, d_copy);
      mCRL2log(log::debug1) << "c = " << data::pp(c) << "\n";

      core::identifier_string Y = m_rename(Xi, di_copy);
      result.insert(tr::and_(c, propositional_variable_instantiation(Y, e_copy)));
    }

    pbes_expression result1 = pbes_expr::join_or(result.begin(), result.end());
    mCRL2log(log::debug1) << "result1 = " << pbes_system::pp(result1) << "\n";
    return result1;
  }

  /// \return Visits the initial state
  propositional_variable_instantiation visit_initial_state(const propositional_variable_instantiation& init)
  {
    std::vector<data::data_expression> finite_parameters_vector;
    std::vector<data::data_expression> infinite_parameters_vector;
    split_parameters(init, m_index_map, finite_parameters_vector, infinite_parameters_vector);
    data::data_expression_list finite_parameters = atermpp::convert<data::data_expression_list>(finite_parameters_vector);
    data::data_expression_list infinite_parameters = atermpp::convert<data::data_expression_list>(infinite_parameters_vector);

    data::detail::rewrite_container(finite_parameters, super::R);
    data::detail::rewrite_container(infinite_parameters, super::R);
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
    int m_equation_count;

    /// \brief Returns true if the container contains the given element
    template <typename Container>
    bool has_element(const Container& c, const typename Container::value_type& v) const
    {
      return std::find(c.begin(), c.end(), v) != c.end();
    }

    /// \brief Computes the index map corresponding to the given PBES equations and variable map
    template <typename EquationContainer>
    void compute_index_map(const EquationContainer& equations,
                           const pbesinst_variable_map& variable_map,
                           pbesinst_index_map& index_map)
    {
      for (typename EquationContainer::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        core::identifier_string name = i->variable().name();
        data::variable_list parameters = i->variable().parameters();

        std::vector<size_t> v;
        pbesinst_variable_map::const_iterator j = variable_map.find(name);
        if (j != variable_map.end())
        {
          size_t index = 0;
          for (data::variable_list::const_iterator k = parameters.begin(); k != parameters.end(); ++k, ++index)
          {
            if (has_element(j->second, *k))
            {
              v.push_back(index);
            }
          }
        }
        index_map[name] = v;
      }
    }

    /// \brief Prints a message for every 1000-th equation
    std::string print_equation_count(size_t size) const
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
    pbesinst_finite_algorithm(data::rewriter::strategy rewriter_strategy = data::jitty)
      : m_rewriter_strategy(rewriter_strategy)
    {}

    /// \brief Runs the algorithm.
    /// \param p A PBES
    /// \param variable_map A map containing the finite parameters that should be expanded by the algorithm.
    void run(pbes& p,
             const pbesinst_variable_map& variable_map
            )
    {
      pbes_system::algorithms::instantiate_global_variables(p);
      m_equation_count = 0;

      // compute index map corresponding to the variable map
      pbesinst_index_map index_map;
      compute_index_map(p.equations(), variable_map, index_map);

      data::rewriter rewr(p.data(), m_rewriter_strategy);

      typedef data::classic_enumerator<>::substitution_type substitution_type;

      // compute new equations
      std::vector<pbes_equation> equations;
      for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        std::vector<data::variable> finite_parameters;
        std::vector<data::variable> infinite_parameters;
        detail::split_parameters(i->variable(), index_map, finite_parameters, infinite_parameters);
        data::variable_list infinite = atermpp::convert<data::variable_list>(infinite_parameters);

        data::classic_enumerator<> enumerator(p.data(),rewr);
        mcrl2::data::mutable_indexed_substitution<> local_sigma;
        for (auto j = enumerator.begin(data::variable_list(finite_parameters.begin(), finite_parameters.end()), 
                    data::sort_bool::true_(), local_sigma); j != enumerator.end(); ++j)
        {
          // apply the substitution *j
          // TODO: use a generic substitution routine (does that already exist in the data library?)
          data::mutable_indexed_substitution<> sigma_j;
          data::data_expression_list::const_iterator kk=j->begin();
          for(auto jj=finite_parameters.begin(); jj!=finite_parameters.end(); ++jj, ++kk)
          {
            sigma_j[*jj]=*kk;
          }

          std::vector<data::data_expression> finite;
          for (std::vector<data::variable>::iterator k = finite_parameters.begin(); k != finite_parameters.end(); ++k)
          {
            mCRL2log(log::debug1) << "sigma(" << data::pp(*k) << ") = " << data::pp(sigma_j(*k)) << "\n";
            finite.push_back(sigma_j(*k));
          }
          core::identifier_string name = pbesinst_finite_rename()(i->variable().name(), finite);
          propositional_variable X(name, infinite);
          mCRL2log(log::debug1) << "formula before = " << pbes_system::pp(i->formula()) << "\n";
          mCRL2log(log::debug1) << "sigma = " << sigma_j << "\n";
          detail::pbesinst_finite_builder<data::rewriter, substitution_type> visitor(rewr, sigma_j, pbesinst_finite_rename(), p.data(), index_map, variable_map);
          pbes_expression formula = visitor(i->formula());
          mCRL2log(log::debug1) << "formula after  = " << pbes_system::pp(formula) << "\n";
          pbes_equation eqn(i->symbol(), X, formula);
          equations.push_back(eqn);
          mCRL2log(log::debug, "pbesinst_finite") << print_equation_count(++m_equation_count);
          mCRL2log(log::debug, "pbesinst_finite") << "Added equation " << pbes_system::pp(eqn) << "\n";
        }
      }

      // compute new initial state
      detail::NoSubst sigma;
      detail::pbesinst_finite_builder<data::rewriter, detail::NoSubst> visitor(rewr, sigma, pbesinst_finite_rename(), p.data(), index_map, variable_map);
      propositional_variable_instantiation initial_state = visitor.visit_initial_state(p.initial_state());

      // assign the result
      p.equations() = equations;
      p.initial_state() = initial_state;
    }

    /// \brief Runs the algorithm.
    /// \param p A PBES
    void run(pbes& p)
    {
      // put all finite variables in a variable map
      pbesinst_variable_map variable_map;
      for (auto i = p.equations().begin(); i != p.equations().end(); ++i)
      {
        data::variable_list v = i->variable().parameters();
        for (data::variable_list::const_iterator j = v.begin(); j != v.end(); ++j)
        {
          if (p.data().is_certainly_finite(j->sort()))
          {
            variable_map[i->variable().name()].push_back(*j);
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
  for (auto i = parameter_map.begin(); i != parameter_map.end(); ++i)
  {
    if (!((i->second).empty()))
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_FINITE_ALGORITHM_H
