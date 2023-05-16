// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/parse.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PARSE_H
#define MCRL2_MODAL_FORMULA_PARSE_H

#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/modal_formula/has_name_clashes.h"
#include "mcrl2/modal_formula/resolve_name_clashes.h"
#include "mcrl2/modal_formula/translate_regular_formulas.h"
#include "mcrl2/modal_formula/translate_user_notation.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/process/merge_action_specifications.h"

namespace mcrl2
{

namespace action_formulas
{

namespace detail {

action_formula parse_action_formula(const std::string& text);

} // namespace detail

template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable> >
action_formula parse_action_formula(const std::string& text,
                                    const data::data_specification& dataspec,
                                    const VariableContainer& variables,
                                    const ActionLabelContainer& actions
                                   )
{
  action_formula x = detail::parse_action_formula(text);
  x = action_formulas::typecheck_action_formula(x, dataspec, variables, actions);
  x = action_formulas::translate_user_notation(x);
  return x;
}

inline
action_formula parse_action_formula(const std::string& text, const lps::stochastic_specification& lpsspec)
{
  return parse_action_formula(text, lpsspec.data(), lpsspec.global_variables(), lpsspec.action_labels());
}

} // namespace action_formulas

namespace regular_formulas
{

namespace detail
{

regular_formula parse_regular_formula(const std::string& text);

} // namespace detail

template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable> >
regular_formula parse_regular_formula(const std::string& text,
                                      const data::data_specification& dataspec,
                                      const VariableContainer& variables,
                                      const ActionLabelContainer& actions
                                     )
{
  regular_formula x = detail::parse_regular_formula(text);
  x = regular_formulas::typecheck_regular_formula(x, dataspec, variables, actions);
  x = regular_formulas::translate_user_notation(x);
  return x;
}

inline
regular_formula parse_regular_formula(const std::string& text, const lps::stochastic_specification& lpsspec)
{
  return parse_regular_formula(text, lpsspec.data(), lpsspec.global_variables(), lpsspec.action_labels());
}

} // namespace regular_formulas

namespace state_formulas
{

namespace detail {

state_formula parse_state_formula(const std::string& text);
state_formula_specification parse_state_formula_specification(const std::string& text);

} // namespace detail

struct parse_state_formula_options
{
  bool check_monotonicity = true;
  bool translate_regular_formulas = true;
  bool type_check = true;
  bool translate_user_notation = true;
  bool resolve_name_clashes = true;
};

inline
state_formula post_process_state_formula(
  const state_formula& formula,
  parse_state_formula_options options = parse_state_formula_options()
)
{
  state_formula x = formula;
  if (options.translate_regular_formulas)
  {
    mCRL2log(log::debug) << "formula before translating regular formulas: " << x << std::endl;
    x = translate_regular_formulas(x);
    mCRL2log(log::debug) << "formula after translating regular formulas: " << x << std::endl;
  }
  if (options.translate_user_notation)
  {
    x = state_formulas::translate_user_notation(x);
  }
  if (options.check_monotonicity && !is_monotonous(x))
  {
    throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(x));
  }
  if (options.resolve_name_clashes && has_state_variable_name_clashes(x))
  {
    mCRL2log(log::debug) << "formula before resolving name clashes: " << x << std::endl;
    x = state_formulas::resolve_state_variable_name_clashes(x);
    mCRL2log(log::debug) << "formula after resolving name clashes: " << x << std::endl;
  }
  return x;
}

/// \brief Parses a state formula from an input stream
/// \param text A string.
/// \param lpsspec A stochastic linear process specification used as context. The data specification of lpsspec is extended with sorts appearing in the formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula parse_state_formula(const std::string& text,
                                  lps::stochastic_specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  state_formula x = detail::parse_state_formula(text);
  if (options.type_check)
  {
    x = state_formulas::typecheck_state_formula(x, lpsspec, formula_is_quantitative);
  }
  lpsspec.data().add_context_sorts(state_formulas::find_sort_expressions(x));
  return post_process_state_formula(x, options);
}

/// \brief Parses a state formula from an input stream
/// \param text A string.
/// \param lpsspec A linear process specification used as context. The data specification of lpsspec is extended with sorts appearing in the formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula parse_state_formula(const std::string& text,
                                  lps::specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  lps::stochastic_specification stoch_lps_spec=lps::stochastic_specification(lpsspec);
  state_formula phi = parse_state_formula(text, stoch_lps_spec, formula_is_quantitative, options);
  lpsspec = remove_stochastic_operators(stoch_lps_spec);
  return phi;
}

/// \brief Parses a state formula from an input stream
/// \param in A stream.
/// \param lpsspec A stochastic linear process specification used as context. The data specification of lpsspec is extended with sorts appearing in the formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula parse_state_formula(std::istream& in,
                                  lps::stochastic_specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  std::string text = utilities::read_text(in);
  return parse_state_formula(text, lpsspec, formula_is_quantitative, options);
}

/// \brief Parses a state formula from an input stream
/// \param in A stream.
/// \param lpsspec A linear process specification used as context. The data specification of lpsspec is extended with sorts appearing in the formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula parse_state_formula(std::istream& in,
                                  lps::specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  lps::stochastic_specification stoch_lps_spec=lps::stochastic_specification(lpsspec);
  state_formula phi = parse_state_formula(in, stoch_lps_spec, formula_is_quantitative, options);
  lpsspec = remove_stochastic_operators(stoch_lps_spec);
  return phi;
}

/// \brief Parses a state formula specification from a string.
/// \param text A string.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula_specification parse_state_formula_specification(
  const std::string& text,
  const bool formula_is_quantitative,
  parse_state_formula_options options = parse_state_formula_options()
)
{
  state_formula_specification result = detail::parse_state_formula_specification(text);
  if (options.type_check)
  {
    result.formula() = state_formulas::typecheck_state_formula(result.formula(), formula_is_quantitative, result.data(), result.action_labels(), data::variable_list());
  }
  result.formula() = post_process_state_formula(result.formula(), options);
  return result;
}

/// \brief Parses a state formula specification from an input stream.
/// \param in An input stream.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula_specification parse_state_formula_specification(
  std::istream& in,
  const bool formula_is_quantitative,
  parse_state_formula_options options = parse_state_formula_options()
)
{
  std::string text = utilities::read_text(in);
  return parse_state_formula_specification(text, formula_is_quantitative, options);
}

/// \brief Parses a state formula specification from a string
/// \param text A string
/// \param lpsspec A linear process containing data and action declarations necessary to type check the state formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result
inline
state_formula_specification parse_state_formula_specification(const std::string& text,
                                  lps::stochastic_specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  state_formula_specification result = detail::parse_state_formula_specification(text);
  // Merge data specification checks whether the combined datatypes are well typed. 
  data::data_specification dataspec = data::merge_data_specifications(lpsspec.data(), result.data());
  process::action_label_list actspec = process::merge_action_specifications(lpsspec.action_labels(), result.action_labels());

  if (options.type_check)
  {
    data::data_type_checker type_checker(dataspec);
    // The type checker below checks whether the combined action list is well typed. 
    type_checker(result.data().user_defined_equations()); // This changes the data equations in result.data() to become well typed.
    // Note that while type checking the formula below the non type checked equations are used. This is not an issue
    // as the shape of equations do not influence well typedness of a modal formula.  
    result.formula() = state_formulas::typecheck_state_formula(result.formula(), formula_is_quantitative, dataspec, actspec, lpsspec.global_variables());
  }

  result.formula() = post_process_state_formula(result.formula(), options);
  return result;
} 


/// \brief Parses a state formula specification from a string
/// \param text A string
/// \param lpsspec A linear process containing data and action declarations necessary to type check the state formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result
inline
state_formula_specification parse_state_formula_specification(const std::string& text,
                                  lps::specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  lps::stochastic_specification stoch_lps_spec=lps::stochastic_specification(lpsspec);
  state_formula_specification sfs = parse_state_formula_specification(text, stoch_lps_spec, formula_is_quantitative, options);
  lpsspec = remove_stochastic_operators(stoch_lps_spec);
  return sfs;
}

/// \brief Parses a state formula specification from an input stream.
/// \param in An input stream.
/// \param lpsspec A stochastic linear process containing data and action declarations necessary to type check the state formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula_specification parse_state_formula_specification(std::istream& in,
                                  lps::stochastic_specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  return parse_state_formula_specification(utilities::read_text(in), lpsspec, formula_is_quantitative, options);
}

/// \brief Parses a state formula specification from an input stream.
/// \param in An input stream.
/// \param lpsspec A linear process containing data and action declarations necessary to type check the state formula.
/// \param formula_is_quantitative If true the formula is interpreted as a quantitative modal formula, instead of a classical boolean modal formula.
/// \param options A set of options guiding parsing.
/// \return The parse result.
inline
state_formula_specification parse_state_formula_specification
                                 (std::istream& in,
                                  lps::specification& lpsspec,
                                  const bool formula_is_quantitative,
                                  parse_state_formula_options options = parse_state_formula_options()
                                 )
{
  lps::stochastic_specification stoch_lps_spec=lps::stochastic_specification(lpsspec);
  state_formula_specification sfs = parse_state_formula_specification(in, stoch_lps_spec, formula_is_quantitative, options);
  lpsspec = remove_stochastic_operators(stoch_lps_spec);
  return sfs;
}
} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PARSE_H
