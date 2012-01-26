// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/find.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_FIND_H
#define MCRL2_MODAL_FORMULA_FIND_H

#include "mcrl2/data/find.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/modal_formula/add_binding.h"

namespace mcrl2
{

namespace action_formulas
{

//--- start generated action_formulas find code ---//
/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_variables_traverser<action_formulas::variable_traverser>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_variables(const T& x)
{
  std::set<data::variable> result;
  action_formulas::find_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<action_formulas::variable_traverser, action_formulas::add_data_variable_binding>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<action_formulas::variable_traverser, action_formulas::add_data_variable_binding>(o, bound)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  action_formulas::find_free_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in] bound a bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename VariableContainer>
std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
{
  std::set<data::variable> result;
  action_formulas::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<action_formulas::identifier_string_traverser>(o)(x);
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  action_formulas::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<action_formulas::sort_expression_traverser>(o)(x);
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  action_formulas::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<action_formulas::data_expression_traverser>(o)(x);
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  action_formulas::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated action_formulas find code ---//

} // namespace action_formulas

namespace regular_formulas
{

//--- start generated regular_formulas find code ---//
/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_variables_traverser<regular_formulas::variable_traverser>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_variables(const T& x)
{
  std::set<data::variable> result;
  regular_formulas::find_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<regular_formulas::variable_traverser, regular_formulas::add_data_variable_binding>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<regular_formulas::variable_traverser, regular_formulas::add_data_variable_binding>(o, bound)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  regular_formulas::find_free_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in] bound a bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename VariableContainer>
std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
{
  std::set<data::variable> result;
  regular_formulas::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<regular_formulas::identifier_string_traverser>(o)(x);
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  regular_formulas::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<regular_formulas::sort_expression_traverser>(o)(x);
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  regular_formulas::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<regular_formulas::data_expression_traverser>(o)(x);
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  regular_formulas::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated regular_formulas find code ---//

} // namespace regular_formulas

namespace state_formulas
{

//--- start generated state_formulas find code ---//
/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_variables_traverser<state_formulas::variable_traverser>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_variables(const T& x)
{
  std::set<data::variable> result;
  state_formulas::find_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<state_formulas::variable_traverser, state_formulas::add_data_variable_binding>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<state_formulas::variable_traverser, state_formulas::add_data_variable_binding>(o, bound)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  state_formulas::find_free_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in] bound a bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename VariableContainer>
std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
{
  std::set<data::variable> result;
  state_formulas::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<state_formulas::identifier_string_traverser>(o)(x);
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  state_formulas::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<state_formulas::sort_expression_traverser>(o)(x);
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  state_formulas::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<state_formulas::data_expression_traverser>(o)(x);
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  state_formulas::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated state_formulas find code ---//

namespace detail {

/// \cond INTERNAL_DOCS
struct nil_traverser: public state_formulas::regular_formula_traverser<nil_traverser>
{
  typedef state_formulas::regular_formula_traverser<nil_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  bool result;

  nil_traverser()
    : result(false)
  {}

  void operator()(const regular_formulas::nil&)
  {
    result = true;
  }
};
/// \endcond

} // namespace detail

/// \brief Returns true if the regular expression nil occurs in the object x
/// \param[in] x
template <typename T>
bool find_nil(const T& x)
{
  detail::nil_traverser f;
  f(x);
  return f.result;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_FIND_H
