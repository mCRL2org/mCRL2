// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_greybox_interface.h
/// \brief The pbes_greybox_interface class provides a wrapper for the
/// parity_game_generator classes, for use in the PBES explorer.

#ifndef MCRL2_PBES_DETAIL_PBES_GAME_GREYBOX_INTERFACE_H
#define MCRL2_PBES_DETAIL_PBES_GAME_GREYBOX_INTERFACE_H

#include <set>
#include <string>
#include <utility>
#include <vector>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// A class that provides initial state and successors functions for PBESs,
  /// allowing to explore the PBES as a transition system, where states are
  /// instantiated propositional variables.
  class pbes_greybox_interface: public parity_game_generator
  {
    protected:
      data::rewriter datar;
      pbes_system::enumerate_quantifiers_rewriter pbes_rewriter;

    public:
    /// \brief Constructor.
    /// \param p A PBES
    /// \param true_false_dependencies If true, nodes are generated for the values <tt>true</tt> and <tt>false</tt>.
    /// \param is_min_parity If true a min-parity game is produced, otherwise a max-parity game
    /// \param rewrite_strategy The rewrite engine to use. (Default: jitty)
    pbes_greybox_interface(pbes& p, bool true_false_dependencies = false, bool is_min_parity = true, data::rewriter::strategy rewrite_strategy = data::jitty)
      : parity_game_generator(p, true_false_dependencies, is_min_parity, rewrite_strategy),
      	datar(p.data()),
        pbes_rewriter(datar, p.data(), true)
    {
      initialize_generation();
    }

    virtual ~pbes_greybox_interface() {}

    /// \brief Returns the initial state, rewritten and simplified.
    /// \return the initial state.
    propositional_variable_instantiation get_initial_state()
    {
      //std::clog << "get_initial_state()" << std::endl;
      propositional_variable_instantiation phi = atermpp::down_cast<propositional_variable_instantiation>(rewrite_and_simplify_expression(m_pbes.initial_state()));
      //std::clog << "  phi = " << phi << std::endl;
      return phi;
    }

    /// \brief Rewrites and simplifies an expression.
    /// \param e a PBES expression.
    /// \return the result of the rewrite.
    pbes_expression rewrite_and_simplify_expression(const pbes_expression& e, const bool /* convert_data_to_pbes */ = true)
    {
      data::rewriter::substitution_type sigma;
      pbes_expression phi = pbes_rewriter(e, sigma);
      return phi;
    }

    /// \brief Returns the equation for variable s.
    /// \param s the identifier string of a variable.
    /// \return the equation for variable s.
    pbes_equation get_pbes_equation(const core::identifier_string& s)
    {
      const pbes_equation& e = *m_pbes_equation_index[s];
      return e;
    }

    /// \brief Returns the successors of a state, which is a instantiated propositional variable.
    /// Fetches the right hand side of the equation for the variable of the state,
    /// Substitutes the variables in the right hand side with the parameter variables in the state
    /// and rewrites the expression.
    /// \param phi An instantiated propositional variable
    /// \return The set of variable instantiations (successor states) that appear in the rewritten
    /// right hand side expression.
    std::set<pbes_expression> get_successors(const pbes_expression& phi)
    {
      //std::clog << "get_successors(psi)" << std::endl;
      initialize_generation();

      std::set<pbes_expression> result;
      mCRL2log(log::debug, "pbes_greybox_interface") << "Generating equation for expression " << phi << std::endl;

      // expand the right hand side if needed
      pbes_expression psi = expand_rhs(phi);

      // top_flatten
      if (is_propositional_variable_instantiation(psi))
      {
        result.insert(psi);
      }
      else if (is_and(psi))
      {
        std::set<pbes_expression> terms = split_and(psi);
        for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
        {
          result.insert(*i);
        }
      }
      else if (is_or(psi))
      {
        std::set<pbes_expression> terms = split_or(psi);
        for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
        {
          result.insert(*i);
        }
      }
      else if (is_true(psi))
      {
        if (m_true_false_dependencies)
        {
          result.insert(true_());
        }
      }
      else if (is_false(psi))
      {
        if (m_true_false_dependencies)
        {
          result.insert(false_());
        }
      }
      else
      {
        throw(std::runtime_error("Error in pbes_greybox_interface: unexpected expression " + pbes_system::pp(psi) + "\n" + pp(psi)));
      }
      mCRL2log(log::debug, "pbes_greybox_interface") << print_successors(result);
      return result;
    }

    /// \brief Expands a formula expr for a instantiated state variable psi, which means
    /// substituting the variables in expr by the parameter values in psi and rewriting the
    /// expression.
    /// \param psi the instantiated propositional variable.
    /// \param expr the expression to be expanded.
    /// \return the result of the expansion.
    virtual
    pbes_expression expand_group(const pbes_expression& psi, const pbes_expression& expr)
    {
      // expand the right hand side if needed
      if (is_propositional_variable_instantiation(psi))
      {
        const pbes_equation& pbes_eqn = *m_pbes_equation_index[atermpp::down_cast<propositional_variable_instantiation>(psi).name()];

        mCRL2log(log::debug2, "pbes_greybox_interface") << "Expanding right hand side of formula " << psi << std::endl << "  rhs: " << expr << " into ";

        pbes_expression result;

        data::rewriter::substitution_type sigma;
        make_substitution(pbes_eqn.variable().parameters(), atermpp::down_cast<propositional_variable_instantiation>(psi).parameters(),sigma);
        result = pbes_rewriter(expr,sigma);

        mCRL2log(log::debug2, "pbes_greybox_interface") << result << std::endl;
        return result;
      }
      return psi;
    }

    /// \brief Prints the set of successors states
    /// \param successors a set of successor expressions.
    /// \return a string representation of successors.
    virtual
    std::string print_successors(const std::set<pbes_expression>& successors)
    {
      std::ostringstream out;
      out << "-- print_successors --" << std::endl;
      for (std::set<pbes_expression>::const_iterator s = successors.begin(); s != successors.end(); ++s)
      {
        out << " * " << *s << std::endl;
      }
      return out.str();
    }

    /// \brief Returns the successors of a state, which is a instantiated propositional variable,
    /// for a certain 'transition group'.
    /// Checks if the name of the state variable equals the variable associated with the transition group
    /// var. If so, in the expression expr the variables are substituted with the parameter variables in the state
    /// and the expression is rewritten.
    /// \param phi An instantiated propositional variable
    /// \param var The variable name associated with the transition group.
    /// \param expr The expression associated with the transition group.
    /// \return The set of variable instantiations (successor states) that appear in the rewritten
    /// expression.
    std::set<pbes_expression> get_successors(const pbes_expression& phi, const std::string& var, const pbes_expression& expr)
    {
      initialize_generation();

      std::set<pbes_expression> result;
      mCRL2log(log::debug, "pbes_greybox_interface") << "Generating equation for expression "  << phi << " (var = " << var
                                                                                               << ", expr = " << expr << ")" <<std::endl;

      assert(is_propositional_variable_instantiation(phi));
      std::string varname = atermpp::down_cast<propositional_variable_instantiation>(phi).name();
      // check that varname for current group equals varname.
      if (varname==var)
      {
        // expand the right hand side if needed
        pbes_expression psi = expand_group(phi, expr);

        // top_flatten
        if (is_propositional_variable_instantiation(psi))
        {
          result.insert(psi);
        }
        else if (is_and(psi))
        {
          std::set<pbes_expression> terms = split_and(psi);
          for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
          {
            result.insert(*i);
          }
        }
        else if (is_or(psi))
        {
          std::set<pbes_expression> terms = split_or(psi);
          for (std::set<pbes_expression>::iterator i = terms.begin(); i != terms.end(); ++i)
          {
            result.insert(*i);
          }
        }
        else if (is_true(psi))
        {
          if (m_true_false_dependencies)
          {
            result.insert(true_());
          }
        }
        else if (is_false(psi))
        {
          if (m_true_false_dependencies)
          {
            result.insert(false_());
          }
        }
        else
        {
          throw(std::runtime_error("Error in pbes_greybox_interface: unexpected expression " + pbes_system::pp(psi)));
        }
      }
      mCRL2log(log::debug, "pbes_greybox_interface") << print_successors(result);
      return result;
    }


  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_GREYBOX_INTERFACE_H
