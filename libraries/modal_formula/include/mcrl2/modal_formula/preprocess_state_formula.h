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
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2
{

namespace state_formulas
{

namespace detail
{

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

/// \brief Renames data variables and predicate variables in the formula \p f, and
/// wraps the formula inside a 'nu' if needed. This is needed as a preprocessing
/// step for the algorithm.
/// \param formula A modal formula
/// \param spec A linear process specification
/// \return The preprocessed formula
inline
state_formulas::state_formula preprocess_state_formula(const state_formulas::state_formula& formula, const lps::specification& spec)
{
  namespace s = state_formulas;

  state_formulas::state_formula f = formula;
  std::set<core::identifier_string> formula_variable_names = data::detail::variable_names(state_formulas::find_all_variables(formula));
  std::set<core::identifier_string> spec_variable_names = data::detail::variable_names(lps::find_all_variables(spec));
  std::set<core::identifier_string> spec_names = lps::find_identifiers(spec);

  // rename data variables in f, to prevent name clashes with data variables in spec
  f = state_formulas::rename_variables(f, spec_variable_names);

  // rename predicate variables in f, to prevent name clashes
  data::xyz_identifier_generator xyz_generator;
  xyz_generator.add_identifiers(spec_names);
  xyz_generator.add_identifiers(formula_variable_names);
  f = rename_predicate_variables(f, xyz_generator);

  // wrap the formula inside a 'nu' if needed
  if (!s::is_mu(f) && !s::is_nu(f))
  {
    data::set_identifier_generator generator;
    generator.add_identifiers(state_formulas::find_identifiers(f));
    generator.add_identifiers(lps::find_identifiers(spec));
    core::identifier_string X = generator("X");
    f = s::nu(X, data::assignment_list(), f);
  }

  return f;
}

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

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PREPROCESS_STATE_FORMULA_H
