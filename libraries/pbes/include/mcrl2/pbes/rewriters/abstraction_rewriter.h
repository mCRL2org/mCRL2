// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/abstraction_rewriter.h
/// \brief This file provides an abstraction rewriter for PBES expressions
///        that applies data abstraction to PBES expressions.

#ifndef MCRL2_PBES_REWRITERS_ABSTRACTION_REWRITER_H
#define MCRL2_PBES_REWRITERS_ABSTRACTION_REWRITER_H

#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/utilities/logger.h"
#include <map>
#include <set>

namespace mcrl2::pbes_system
{

// Helper function to check if a container has an element
template<typename Container, typename Value>
bool contains(const Container& c, const Value& v)
{
  return c.find(v) != c.end();
}

// Abstraction builder for pbescegps - applies data abstraction to PBES expressions
template<typename Dummy = void>
struct abstraction_rewriter : public pbes_expression_builder<abstraction_rewriter<Dummy>>
{
  using super = pbes_expression_builder<abstraction_rewriter<Dummy>>;
  using super::apply;

  std::map<core::identifier_string, std::set<std::size_t>> m_pbes_parameters_abstraction_indices;
  const std::set<data::variable>& m_abstraction_vars;
  bool m_is_overapproximation;

  abstraction_rewriter(const std::set<data::variable>& vars,
    const std::map<core::identifier_string, std::set<std::size_t>>& pbes_parameters_abstraction_indices,
    bool is_over)
    : m_pbes_parameters_abstraction_indices(pbes_parameters_abstraction_indices),
      m_abstraction_vars(vars),
      m_is_overapproximation(is_over)
  {}

  template<class T>
  void apply(T& result, const data::data_expression& x)
  {
    mCRL2log(log::trace) << "Processing data expression: " << x << std::endl;

    // Check if it's a variable using built-in function
    if (data::is_variable(x))
    {
      const data::variable& var = atermpp::down_cast<data::variable>(x);
      mCRL2log(log::trace) << "  -> Handling as data::variable: " << var.name() << std::endl;

      if (contains(m_abstraction_vars, var))
      {
        mCRL2log(log::trace) << "     Abstracting variable " << var.name() << " to "
                             << (m_is_overapproximation ? "true" : "false") << std::endl;
        data::data_expression abstracted_val = m_is_overapproximation ? data::true_() : data::false_();
        result = pbes_expression(abstracted_val);
      }
      else
      {
        // Variable is not abstracted, return as-is
        mCRL2log(log::trace) << "     Keeping variable " << var.name() << " as-is" << std::endl;
        result = pbes_expression(var);
      }
      return;
    }

    // Check if it's an application using built-in function
    if (data::is_application(x))
    {
      const data::application& app = atermpp::down_cast<data::application>(x);
      mCRL2log(log::trace) << "  -> Handling as data::application" << std::endl;

      // First check if any free variables depend on abstracted variables
      std::set<data::variable> free_vars = pbes_system::find_free_variables(app);

      bool depends_on_abstracted = false;
      for (const auto& var: free_vars)
      {
        if (contains(m_abstraction_vars, var))
        {
          depends_on_abstracted = true;
          break;
        }
      }

      if (!depends_on_abstracted)
      {
        mCRL2log(log::trace) << "     Application does not depend on abstracted variables, keeping as-is" << std::endl;
        result = pbes_expression(app);
        return;
      }

      mCRL2log(log::trace) << "     Application depends on abstracted variables" << std::endl;

      // Expression depends on abstracted variables
      // Check if the function is monotonic or anti-monotonic
      bool is_monotonic = false;
      bool is_anti_monotonic = false;

      // Check for known monotonic functions using predicate functions
      if (data::sort_bool::is_and_application(app) || data::sort_bool::is_or_application(app))
      {
        is_monotonic = true;
        mCRL2log(log::trace) << "     Function is monotonic (and/or)" << std::endl;
      }
      // Check for known anti-monotonic functions (implication in antecedent)
      else if (data::sort_bool::is_not_application(app))
      {
        // Implication is monotonic in consequent, anti-monotonic in antecedent
        // For now, treat conservatively
        is_anti_monotonic = true;
        mCRL2log(log::trace) << "     Function is anti-monotonic (not)" << std::endl;
      }
      // For unknown functions that depend on abstracted variables, use conservative approach
      else
      {
        // Conservative: return true/false
        mCRL2log(log::trace) << "     Unknown function, using conservative approach, returning "
                             << (m_is_overapproximation ? "true" : "false") << std::endl;
        data::data_expression conservative_val = m_is_overapproximation ? data::true_() : data::false_();
        result = pbes_expression(conservative_val);
        return;
      }

      // Apply appropriate abstraction to arguments
      data::data_expression_list abstracted_args;
      for (const data::data_expression& arg: app)
      {
        pbes_expression abstracted_arg_expr;

        if (is_monotonic)
        {
          // Apply same abstraction mode to all arguments
          mCRL2log(log::trace) << "     Abstracting argument with monotonic mode" << std::endl;
          apply(abstracted_arg_expr, arg);
        }
        else if (is_anti_monotonic)
        {
          // Flip abstraction mode for arguments
          mCRL2log(log::trace) << "     Flipping abstraction mode for anti-monotonic argument" << std::endl;
          m_is_overapproximation = !m_is_overapproximation;
          apply(abstracted_arg_expr, arg);
          m_is_overapproximation = !m_is_overapproximation;
        }

        // Extract the data expression from the pbes_expression
        const data::data_expression& data_expr = atermpp::down_cast<data::data_expression>(abstracted_arg_expr);
        abstracted_args.push_front(data_expr);
      }

      // Reconstruct the application with abstracted arguments
      data::data_expression reconstructed = data::application(app.head(), abstracted_args);
      result = pbes_expression(reconstructed);
      return;
    }

    // For any other data expression type (e.g., function symbols), use the default builder behavior
    mCRL2log(log::trace) << "  -> Using default builder behavior (not a variable or application)" << std::endl;
    super::apply(result, x);
  }

  template<class T>
  void apply(T& result, const and_& x)
  {
    mCRL2log(log::trace) << "Processing PBES conjunction" << std::endl;
    pbes_expression left;
    pbes_expression right;
    super::apply(left, x.left());
    super::apply(right, x.right());
    make_and_(result, left, right);
  }

  template<class T>
  void apply(T& result, const or_& x)
  {
    mCRL2log(log::trace) << "Processing PBES disjunction" << std::endl;
    pbes_expression left;
    pbes_expression right;
    super::apply(left, x.left());
    super::apply(right, x.right());
    make_or_(result, left, right);
  }

  template<class T>
  void apply(T& result, const forall& x)
  {
    mCRL2log(log::trace) << "Processing PBES forall" << std::endl;
    pbes_expression body;
    super::apply(body, x.body());
    result = make_forall_(x.variables(), body);
  }

  template<class T>
  void apply(T& result, const exists& x)
  {
    mCRL2log(log::trace) << "Processing PBES exists" << std::endl;
    pbes_expression body;
    super::apply(body, x.body());
    result = make_exists_(x.variables(), body);
  }

  template<class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    mCRL2log(log::trace) << "Processing PBES propositional variable instantiation: " << x.name() << std::endl;
    data::data_expression_list filtered_args_vec;

    std::size_t i = 0;
    for (const auto& param: x.parameters())
    {
      if (!m_pbes_parameters_abstraction_indices[x.name()].contains(i))
      {
        filtered_args_vec.push_front(atermpp::down_cast<data::data_expression>(param));
      }
      ++i;
    }
    result = propositional_variable_instantiation(x.name(), filtered_args_vec);
  }

  template<class T>
  void apply(T& result, const not_& x)
  {
    (void)result;
    (void)x;
    throw mcrl2::runtime_error("apply_abstraction: not_ is not supported in this tool");
  }

  // Override imp: not allowed after normalization
  template<class T>
  void apply(T& result, const imp& x)
  {
    (void)result;
    (void)x;
    throw mcrl2::runtime_error("apply_abstraction encountered implication; PBES should be normalized");
  }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_REWRITERS_ABSTRACTION_REWRITER_H
