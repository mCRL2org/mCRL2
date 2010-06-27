// Author(s): Wieger Wesselink; Alexander van Dam
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbes2bes_finite_algorithm.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PBES2BES_FINITE_ALGORITHM_H
#define MCRL2_PBES_PBES2BES_FINITE_ALGORITHM_H

#include <algorithm>
#include <vector>
#include <set>
#include "mcrl2/atermpp/map.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/core/algorithm.h"
#include "mcrl2/data/classic_enumerator.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/detail/rewrite_container.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/substitute.h"
#include "mcrl2/pbes/detail/data_rewrite_builder.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Data structure for storing the indices of the variables that should be expanded by the finite pbes2bes algorithm.
  typedef atermpp::map<core::identifier_string, std::vector<unsigned int> > pbes2bes_index_map;

  /// \brief Data structure for storing the variables that should be expanded by the finite pbes2bes algorithm.
  typedef atermpp::map<core::identifier_string, std::vector<data::variable> > pbes2bes_variable_map;

  /// \brief Function object for renaming a propositional variable instantiation
  struct pbes2bes_finite_rename
  {
    /// \brief Renames the propositional variable x.
    template <typename ExpressionContainer>
    core::identifier_string operator()(const core::identifier_string& name, const ExpressionContainer& parameters) const
    {     
      std::ostringstream out;
      out << std::string(name);
      for (typename ExpressionContainer::const_iterator i = parameters.begin(); i != parameters.end(); ++i)
      {
        out << "@" << core::pp(*i);
      }
      return core::identifier_string(out.str());
    }
  };

namespace detail {

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
  /// \param finite A sequence of data expressions
  /// \param infinite A sequence of data expressions
  template <typename PropositionalVariable>
  void split_parameters(const PropositionalVariable& X,
                        const pbes2bes_index_map& index_map,
                        std::vector<typename PropositionalVariable::parameter_type>& finite,
                        std::vector<typename PropositionalVariable::parameter_type>& infinite
                       )
  {
    typedef typename PropositionalVariable::parameter_type parameter_type;
    pbes2bes_index_map::const_iterator pi = index_map.find(X.name());
    assert(pi != index_map.end());
    const std::vector<unsigned int>& v = pi->second;
    typename atermpp::term_list<parameter_type>::const_iterator i = X.parameters().begin();
    unsigned int index = 0;
    std::vector<unsigned int>::const_iterator j = v.begin();
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
  template <typename DataRewriter, typename RenameFunction, typename Substitution>
  struct pbes2bes_finite_builder: public pbes_system::detail::data_rewrite_builder<pbes_expression, DataRewriter, Substitution>
  {
    typedef typename pbes_system::detail::data_rewrite_builder<pbes_expression, DataRewriter, Substitution> super;
    typedef core::term_traits<pbes_expression> tr;
    
    const RenameFunction& m_rename;
    const data::data_specification& m_data_spec;
    const pbes2bes_index_map& m_index_map;
    const pbes2bes_variable_map& m_variable_map;

    pbes2bes_finite_builder(const DataRewriter& r,
                            const RenameFunction& rho,
                            const data::data_specification& data_spec,
                            const pbes2bes_index_map& index_map,
                            const pbes2bes_variable_map& variable_map
                           )
      : super(r),
        m_rename(rho),
        m_data_spec(data_spec),
        m_index_map(index_map),
        m_variable_map(variable_map)
    {}  

    /// \brief Computes the condition 'for all i: variables[i] == expressions[i]'.
    template <typename VariableContainer, typename ExpressionContainer>
    data::data_expression make_condition(const VariableContainer& variables, const ExpressionContainer& expressions) const
    {
      using namespace data::sort_bool;
      assert(variables.size() == expressions.size());
      if (variables.empty())
      {
        return true_();
      }
      typename VariableContainer::const_iterator vi = variables.begin();
      typename ExpressionContainer::const_iterator ei = expressions.begin();
      data::data_expression result = equal_to(*vi, *ei);
      ++vi;
      ++ei;
      for (; vi != variables.end(); ++vi, ++ei)
      {
        result = and_(result, equal_to(*vi, *ei));
      }
      return result;
    } 

    /// \brief Visit propositional_variable node
    /// \param x A term
    /// \return The result of visiting the node
    pbes_expression visit_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& v, Substitution& sigma)
    {
//std::clog << "visit " << core::pp(x) << std::endl;
      // TODO: this code contains too much conversion between vectors and ATerm lists

      std::vector<data::data_expression> finite_parameters;
      std::vector<data::data_expression> infinite_parameters;
      split_parameters(v, m_index_map, finite_parameters, infinite_parameters);
      data::data_expression_list d = atermpp::convert<data::data_expression_list>(finite_parameters);
      data::data_expression_list e = atermpp::convert<data::data_expression_list>(infinite_parameters);
      core::identifier_string Xi = v.name();     
      // v = Xi(d,e)

      pbes2bes_variable_map::const_iterator vi = m_variable_map.find(Xi);
      std::vector<data::variable> di;
      if (vi != m_variable_map.end())
      {
        di = vi->second;
      }
      data::data_expression condition = make_condition(di, d);
//std::clog << "condition = " << core::pp(condition) << std::endl;

      atermpp::set<pbes_expression> result;
      for (data::classic_enumerator<> i(m_data_spec, di, super::m_data_rewriter); i.enumerator_has_a_solution(); ++i)
      {
//std::clog << "sigma = " << data::to_string(sigma) << std::endl;
//std::clog << "*i    = " << data::to_string(*i) << std::endl;
        data::data_expression_list d_copy = d;
        data::detail::rewrite_container(d_copy, super::m_data_rewriter, sigma);
        data::data_expression_list e_copy = e;
        data::detail::rewrite_container(e_copy, super::m_data_rewriter, sigma);

        data::data_expression_list di_copy = atermpp::convert<data::data_expression_list>(di);
        pbes_system::substitute_gcc_workaround(di_copy, *i);

        data::data_expression c = make_condition(di_copy, d_copy);
//std::clog << "c = " << core::pp(c) << std::endl;

        core::identifier_string Y = m_rename(Xi, di_copy);
        result.insert(tr::and_(c, propositional_variable_instantiation(Y, e_copy)));
      }
      
      pbes_expression result1 = pbes_expr::join_or(result.begin(), result.end());
//std::clog << "result1 = " << core::pp(result1) << std::endl;
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

      data::detail::rewrite_container(finite_parameters, super::m_data_rewriter);
      data::detail::rewrite_container(infinite_parameters, super::m_data_rewriter);
      core::identifier_string X = m_rename(init.name(), finite_parameters);
      return propositional_variable_instantiation(X, infinite_parameters);
    }
  };

} // namespace detail

  /// \brief Algorithm class for the finite pbes2bes algorithm.
  class pbes2bes_finite_algorithm: public core::algorithm
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
                             const pbes2bes_variable_map& variable_map,
                             pbes2bes_index_map& index_map)
      {
        for (typename EquationContainer::const_iterator i = equations.begin(); i != equations.end(); ++i)
        {
          core::identifier_string name = i->variable().name();
          data::variable_list parameters = i->variable().parameters();

          std::vector<unsigned int> v;
          pbes2bes_variable_map::const_iterator j = variable_map.find(name);
          if (j != variable_map.end())
          {
            unsigned int index = 0;
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

      /// \brief Prints a log message for every 1000-th equation
      void LOG_EQUATION_COUNT(unsigned int level, unsigned int size) const
      {
        if (check_log_level(level))
        {
          if (size > 0 && size % 1000 == 0)
          {
            std::cout << "Generated " << size << " BES equations" << std::endl;
          }
        }
      }

    public:

      /// \brief Constructor.
      /// \param print_equations If true, the generated equations are printed
      /// \param print_rewriter_output If true, invocations of the rewriter are printed
      pbes2bes_finite_algorithm(data::rewriter::strategy rewriter_strategy = data::rewriter::jitty,
                                unsigned int log_level = 0
                               )
        : core::algorithm(log_level),
          m_rewriter_strategy(rewriter_strategy)
      {}

      /// \brief Runs the algorithm.
      /// \param p A PBES
      /// \param variable_map A map containing the finite parameters that should be expanded by the algorithm.
      void run(pbes<>& p,
               const pbes2bes_variable_map& variable_map
              )
      {
        p.instantiate_global_variables();
        m_equation_count = 0;

        // compute index map corresponding to the variable map
        pbes2bes_index_map index_map;
        compute_index_map(p.equations(), variable_map, index_map);

        data::rewriter rewr(p.data(), m_rewriter_strategy);
          
        typedef data::classic_enumerator<>::substitution_type substitution_type;
        detail::pbes2bes_finite_builder<data::rewriter, pbes2bes_finite_rename, substitution_type> visitor(rewr, pbes2bes_finite_rename(), p.data(), index_map, variable_map);

        // compute new equations
        atermpp::vector<pbes_equation> equations;
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
        {
          std::vector<data::variable> finite_parameters;
          std::vector<data::variable> infinite_parameters;
          detail::split_parameters(i->variable(), index_map, finite_parameters, infinite_parameters);
          data::variable_list infinite = atermpp::convert<data::variable_list>(infinite_parameters);

          for (data::classic_enumerator<> j(p.data(), finite_parameters, rewr); j.enumerator_has_a_solution(); ++j)
          {
            // apply the substitution *j
            // TODO: use a generic substitution routine (does that already exist in the data library?)
            std::vector<data::data_expression> finite;
            for (std::vector<data::variable>::iterator k = finite_parameters.begin(); k != finite_parameters.end(); ++k)
            {
              //LOG(2, "sigma(" + core::pp(*k) + ") = " + core::pp((*j)(*k)) + "\n");
              finite.push_back((*j)(*k));
            }
            core::identifier_string name = pbes2bes_finite_rename()(i->variable().name(), finite);
            propositional_variable X(name, infinite);
            //LOG(2, "formula before = " + core::pp(i->formula()) + "\n");
            //LOG(2, "sigma = " + data::to_string(*j) + "\n");
            pbes_expression formula = visitor(i->formula(), *j);
            //LOG(2, "formula after  = " + core::pp(formula) + "\n");
            pbes_equation eqn(i->symbol(), X, formula);
            equations.push_back(eqn);
            LOG_EQUATION_COUNT(1, ++m_equation_count);           
            LOG(2, "Added equation " + pbes_system::pp(eqn) + "\n");
          }
        }

        // compute new initial state
        propositional_variable_instantiation initial_state = visitor.visit_initial_state(p.initial_state());

        // assign the result
        p.equations() = equations;
        p.initial_state() = initial_state;
      }

      /// \brief Runs the algorithm.
      /// \param p A PBES
      void run(pbes<>& p)
      {
        // put all finite variables in a variable map
        pbes2bes_variable_map variable_map;
        for (atermpp::vector<pbes_equation>::const_iterator i = p.equations().begin(); i != p.equations().end(); ++i)
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

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBES2BES_FINITE_ALGORITHM_H
