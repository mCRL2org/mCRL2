// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/preprocess_state_formula.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H
#define MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H

#include "mcrl2/data/detail/find.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/has_name_clashes.h"
#include "mcrl2/modal_formula/is_monotonous.h"
#include "mcrl2/modal_formula/normalize.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2
{

namespace state_formulas
{

namespace detail
{

struct count_modal_operator_nesting_traverser: public state_formula_traverser<count_modal_operator_nesting_traverser>
{
  typedef state_formula_traverser<count_modal_operator_nesting_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::size_t result;
  std::vector<std::size_t> nesting_depth;

  count_modal_operator_nesting_traverser()
    : result(0)
  {
    enter_scope();
  }

  void enter_scope()
  {
    nesting_depth.push_back(0);
  }

  void leave_scope()
  {
    nesting_depth.pop_back();
  }

  void increase_nesting_depth()
  {
    nesting_depth.back()++;
    if (nesting_depth.back() > result)
    {
      result = nesting_depth.back();
    }
  }

  void decrease_nesting_depth()
  {
    nesting_depth.back()--;
  }

  void enter(const must&)
  {
    increase_nesting_depth();
  }

  void leave(const must&)
  {
    decrease_nesting_depth();
  }

  void enter(const may&)
  {
    increase_nesting_depth();
  }

  void leave(const may&)
  {
    decrease_nesting_depth();
  }

  void enter(const mu&)
  {
    enter_scope();
  }

  void leave(const mu&)
  {
    leave_scope();
  }

  void enter(const nu&)
  {
    enter_scope();
  }

  void leave(const nu&)
  {
    leave_scope();
  }

  void enter(const forall&)
  {
    enter_scope();
  }

  void leave(const forall&)
  {
    leave_scope();
  }

  void enter(const exists&)
  {
    enter_scope();
  }

  void leave(const exists&)
  {
    leave_scope();
  }
};

inline
std::size_t count_modal_operator_nesting(const state_formula& x)
{
  count_modal_operator_nesting_traverser f;
  f.apply(x);
  return f.result;
}

struct has_unscoped_modal_formula_traverser: public state_formula_traverser<has_unscoped_modal_formula_traverser>
{
  typedef state_formula_traverser<has_unscoped_modal_formula_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;
  std::vector<state_formula> fixpoints;

  void push(const state_formula& x)
  {
    fixpoints.push_back(x);
  }

  void pop()
  {
    fixpoints.pop_back();
  }

  has_unscoped_modal_formula_traverser()
    : result(false)
  {}

  void enter(const must&)
  {
    if (fixpoints.empty())
    {
      result = true;
    }
  }

  void enter(const may&)
  {
    if (fixpoints.empty())
    {
      result = true;
    }
  }

  void enter(const mu& x)
  {
    push(x);
  }

  void leave(const mu&)
  {
    pop();
  }

  void enter(const nu& x)
  {
    push(x);
  }

  void leave(const nu&)
  {
    pop();
  }
};

inline
bool has_unscoped_modal_formulas(const state_formula& x)
{
  has_unscoped_modal_formula_traverser f;
  f.apply(x);
  return f.result;
}

/// Visitor that transforms state formulas. This can be useful if the state formula contains nested modal operators.
template <typename IdentifierGenerator>
struct state_formula_preprocess_nested_modal_operators_builder: public state_formulas::state_formula_builder<state_formula_preprocess_nested_modal_operators_builder<IdentifierGenerator> >
{
  typedef state_formulas::state_formula_builder<state_formula_preprocess_nested_modal_operators_builder<IdentifierGenerator> > super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  /// \brief An identifier generator
  IdentifierGenerator& generator;
  std::vector<state_formula> fixpoints;

  /// \brief Constructor
  /// \param generator A generator for fresh identifiers
  state_formula_preprocess_nested_modal_operators_builder(IdentifierGenerator& generator)
    : generator(generator)
  {}

  void push(const state_formula& x)
  {
    fixpoints.push_back(x);
  }

  void pop()
  {
    fixpoints.pop_back();
  }

  bool is_mu()
  {
    if (fixpoints.empty())
    {
      return true;
    }
    return state_formulas::is_mu(fixpoints.back());
  }

  void enter(const mu& x)
  {
    push(x);
  }

  void leave(const mu&)
  {
    pop();
  }

  void enter(const nu& x)
  {
    push(x);
  }

  void leave(const nu&)
  {
    pop();
  }

  state_formula apply(const must& x)
  {
    state_formula operand = apply(x.operand());
    if (!has_unscoped_modal_formulas(operand))
    {
      return must(x.formula(), operand);
    }
    core::identifier_string X = generator("X");
    if (is_mu())
    {
      return must(x.formula(), mu(X, {}, operand));
    }
    else
    {
      return must(x.formula(), nu(X, {}, operand));
    }
  }

  state_formula apply(const may& x)
  {
    state_formula operand = apply(x.operand());
    if (!has_unscoped_modal_formulas(operand))
    {
      return x;
    }
    core::identifier_string X = generator("X");
    if (is_mu())
    {
      return may(x.formula(), mu(X, {}, operand));
    }
    else
    {
      return may(x.formula(), nu(X, {}, operand));
    }
  }
};

/// \brief Utility function for creating a state_formula_preprocess_nested_modal_operators_builder.
/// \param generator A generator for fresh identifiers
/// \return a state_formula_preprocess_nested_modal_operators_builder
template <typename IdentifierGenerator>
state_formula_preprocess_nested_modal_operators_builder<IdentifierGenerator> make_state_formula_preprocess_nested_modal_operators_builder(IdentifierGenerator& generator)
{
  return state_formula_preprocess_nested_modal_operators_builder<IdentifierGenerator>(generator);
}

} // namespace detail

/// \brief Preprocesses a state formula that contains (nested) modal operators
/// \param x A modal formula
inline
state_formula preprocess_nested_modal_operators(const state_formula& x)
{
  data::set_identifier_generator generator;
  std::set<core::identifier_string> ids = state_formulas::find_identifiers(x);
  generator.add_identifiers(ids);
  return detail::make_state_formula_preprocess_nested_modal_operators_builder(generator).apply(x);
}

/// \brief Renames data variables and predicate variables in the formula \p f, and
///        wraps the formula inside a 'nu' if needed. This is needed as a preprocessing
///        step for the algorithm.
/// \param formula A modal formula.
/// \param context_ids A set of identifier strings.
/// \param context_variable_names A set of identifier strings.
/// \param preprocess_modal_operators A boolean indicating that dummy fixed point symbols can be
///                                   inserted which makes subsequent handling easier.
/// \param warn_for_modal_operator_nesting A boolean enabling warnings for modal operator nesting.
/// \return The preprocessed formula.
inline
state_formulas::state_formula preprocess_state_formula(const state_formulas::state_formula& formula,
                                                       const std::set<core::identifier_string>& /* context_ids */,
                                                       const std::set<core::identifier_string>& /* context_variable_names */,
                                                       bool preprocess_modal_operators,
                                                       bool warn_for_modal_operator_nesting = true
                                                      )
{
  state_formulas::state_formula f = formula;

  if (!state_formulas::is_monotonous(f))
  {
    throw mcrl2::runtime_error("The formula " + state_formulas::pp(f) + " is not monotonous!");
  }

  if (!preprocess_modal_operators && warn_for_modal_operator_nesting && state_formulas::detail::count_modal_operator_nesting(formula) >= 3)
  {
    mCRL2log(log::info) <<
      "Warning: detected nested modal operators. This may result in a long execution time.\n"
      "Use the option -m or insert dummy fix point operators in between manually to speed\n"
      "up the transformation." << std::endl;
  }

  mCRL2log(log::debug) << "formula before preprocessing: " << f << std::endl;

  // rename data variables in f, to prevent name clashes with data variables in the context
  f = state_formulas::rename_variables(f, state_formulas::find_identifiers(f));

  // rename predicate variables in f, to prevent name clashes
  data::xyz_identifier_generator xyz_generator;
  xyz_generator.add_identifiers(state_formulas::find_identifiers(f));
  f = rename_predicate_variables(f, xyz_generator);

  mCRL2log(log::debug) << "formula after renaming variables: " << f << std::endl;

  // add dummy fixpoints between nested modal operators
  if (preprocess_modal_operators)
  {
    state_formula f0 = f;
    f = state_formulas::preprocess_nested_modal_operators(f);
    if (f0 != f)
    {
      mCRL2log(log::debug) << "formula after inserting dummy fix points between modal operators:  " << f << std::endl;
    }
  }

  // remove occurrences of ! and =>
  if (!state_formulas::is_normalized(f))
  {
    f = state_formulas::normalize(f);
    mCRL2log(log::debug) << "formula after normalization:  " << f << std::endl;
    assert(state_formulas::is_normalized(f));
  }

  // wrap the formula inside a 'nu' if needed
  if (!state_formulas::is_mu(f) && !state_formulas::is_nu(f))
  {
    data::set_identifier_generator generator;
    generator.add_identifiers(state_formulas::find_identifiers(f));
    core::identifier_string X = generator("X");
    f = state_formulas::nu(X, data::assignment_list(), f);
    mCRL2log(log::debug) << "formula after wrapping the formula inside a 'nu':  " << f << std::endl;
  }

  mCRL2log(log::debug) << "formula after preprocessing:  " << f << std::endl;

  // check for parameter name clashes like these mu X(n: Nat). forall n: Nat
  state_formulas::check_parameter_name_clashes(f);

  return f;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H
