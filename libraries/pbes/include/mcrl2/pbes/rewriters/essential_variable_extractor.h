// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/essential_variable_extractor.h
/// \brief This file provides an essential variable extractor traverser for PBES expressions
///        that collects all data::variable instances found in a PBES expression.

#ifndef MCRL2_PBES_REWRITERS_ESSENTIAL_VARIABLE_EXTRACTOR_H
#define MCRL2_PBES_REWRITERS_ESSENTIAL_VARIABLE_EXTRACTOR_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/pbes/rewriters/abstraction_rewriter.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/utilities/logger.h"
#include <set>

namespace mcrl2::pbes_system
{

// Variable extractor traverser for PBES expressions - extracts all data::variable instances
struct essential_variable_extractor : public pbes_expression_traverser<essential_variable_extractor>
{
  using super = pbes_expression_traverser<essential_variable_extractor>;
  using super::apply;
  using super::enter;
  using super::leave;

  std::set<data::variable> result;
  const std::set<data::variable>& W;
  const std::map<core::identifier_string, std::set<std::size_t>>& I;

  essential_variable_extractor(const std::set<data::variable>& W,
    const std::map<core::identifier_string, std::set<std::size_t>>& I)
    : result(),
      W(W),
      I(I)
  {}

  void apply(const data::untyped_data_parameter& x)
  {
    mCRL2log(log::trace) << "Processing data param: " << x << std::endl;
  }

  void apply(const data::data_expression& x)
  {
    mCRL2log(log::trace) << "Processing: " << x << std::endl;
    // Case 1: x is a variable in W
    if (data::is_variable(x))
    {
      const data::variable& var = atermpp::down_cast<data::variable>(x);
      mCRL2log(log::trace) << "Is a variable." << std::endl;
      if (W.find(var) != W.end())
      {
        mCRL2log(log::trace) << "Added it." << std::endl;
        result.insert(var);
      }

      mCRL2log(log::trace) << "----------------------" << std::endl << std::endl;
      return;
    }

    // Case 2 & 3: x is a function application f(t_1, ..., t_n)
    else if (data::is_application(x))
    {
      const data::application& app = atermpp::down_cast<data::application>(x);
      mCRL2log(log::trace) << "Is an application." << std::endl;

      abstraction_rewriter<> rewriter_under(W, I, false);
      pbes_expression under;
      rewriter_under.apply(under, x);

      abstraction_rewriter<> rewriter_over(W, I, true);
      pbes_expression over;
      rewriter_over.apply(over, x);

      // Down-abstraction equals up-abstraction
      // TODO: Will this ever happen theoretically?
      if (under == over)
      {
        mCRL2log(log::trace) << "Under is over." << std::endl;
        mCRL2log(log::trace) << "----------------------" << std::endl << std::endl;
        return;
      }

      // Extract variables from f(t_1, ..., t_n) that are in W
      std::set<data::variable> free_vars = data::find_free_variables(app);
      for (const data::variable& var: free_vars)
      {
        if (W.find(var) != W.end())
        {
          mCRL2log(log::trace) << "Found: " << var << std::endl;
          result.insert(var);
        }
      }
      mCRL2log(log::trace) << "----------------------" << std::endl << std::endl;
      return;
    }

    mCRL2log(log::trace) << "----------------------" << std::endl << std::endl;
    // For any other data expression type, no essential variables
  }

  void process_single_operand(const pbes_expression& operand)
  {
    abstraction_rewriter<> rewriter_under(W, I, false);
    pbes_expression under;
    rewriter_under.apply(under, operand);

    abstraction_rewriter<> rewriter_over(W, I, true);
    pbes_expression over;
    rewriter_over.apply(over, operand);

    if (under != over)
    {
      super::apply(operand);
    }
  }

  void process_and_or_operands(const pbes_expression& left, const pbes_expression& right)
  {
    process_single_operand(left);
    process_single_operand(right);
  }

  void apply(const and_& x) { process_and_or_operands(x.left(), x.right()); }

  void apply(const or_& x) { process_and_or_operands(x.left(), x.right()); }

  void enter(const not_& x)
  {
    (void)x;
    throw mcrl2::runtime_error("essential_variable_extractor encountered not_, not supported");
  }

  void enter(const imp& x)
  {
    (void)x;
    throw mcrl2::runtime_error("essential_variable_extractor encountered implication; PBES should be normalized");
  }
};

inline std::set<data::variable> find_essential_variables(const pbes_expression& x,
  const std::set<data::variable>& abstraction_vars,
  const std::map<core::identifier_string, std::set<std::size_t>>& pbes_parameters_abstraction_indices)
{
  essential_variable_extractor f(abstraction_vars, pbes_parameters_abstraction_indices);
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_REWRITERS_ESSENTIAL_VARIABLE_EXTRACTOR_H
