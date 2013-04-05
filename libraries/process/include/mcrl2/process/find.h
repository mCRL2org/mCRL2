// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/find.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_FIND_H
#define MCRL2_PROCESS_FIND_H

#include "mcrl2/data/find.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/lps/find.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/add_binding.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{

namespace process
{

namespace detail
{
/// \cond INTERNAL_DOCS
struct find_action_names_traverser: public process::action_label_traverser<find_action_names_traverser>
{
  typedef process::action_label_traverser<find_action_names_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  std::set<core::identifier_string> result;

  void operator()(const lps::action_label& x)
  {
    result.insert(x.name());
  }

  void operator()(const process::block& x)
  {
    super::operator()(x);
    core::identifier_string_list B = x.block_set();
    result.insert(B.begin(), B.end());
  }

  void operator()(const process::hide& x)
  {
    super::operator()(x);
    core::identifier_string_list I = x.hide_set();
    result.insert(I.begin(), I.end());
  }

  void operator()(const process::rename& x)
  {
    super::operator()(x);
    rename_expression_list R = x.rename_set();
    for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
    {
      result.insert(i->source());
      result.insert(i->target());
    }
  }

  void operator()(const process::comm& x)
  {
    super::operator()(x);
    communication_expression_list C = x.comm_set();
    for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
    {
      core::identifier_string_list names = i->action_name().names();
      result.insert(names.begin(), names.end());
      result.insert(i->name());
    }
  }

  void operator()(const process::allow& x)
  {
    super::operator()(x);
    action_name_multiset_list V = x.allow_set();
    for (action_name_multiset_list::const_iterator i = V.begin(); i != V.end(); ++i)
    {
      core::identifier_string_list names = i->names();
      result.insert(names.begin(), names.end());
    }
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};
/// \endcond

} // namespace detail

//--- start generated process find code ---//
/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_variables_traverser<process::variable_traverser>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_variables(const T& x)
{
  std::set<data::variable> result;
  process::find_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are added.
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<process::data_expression_traverser, process::add_data_variable_binding>(o)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \param[in,out] o an output iterator to which all variables occurring in x are written.
/// \param[in] bound a container of variables
/// \return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<process::data_expression_traverser, process::add_data_variable_binding>(o, bound)(x);
}

/// \brief Returns all variables that occur in an object
/// \param[in] x an object containing variables
/// \return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  process::find_free_variables(x, std::inserter(result, result.end()));
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
  process::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<process::identifier_string_traverser>(o)(x);
}

/// \brief Returns all identifiers that occur in an object
/// \param[in] x an object containing identifiers
/// \return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  process::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<process::sort_expression_traverser>(o)(x);
}

/// \brief Returns all sort expressions that occur in an object
/// \param[in] x an object containing sort expressions
/// \return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  process::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<process::data_expression_traverser>(o)(x);
}

/// \brief Returns all function symbols that occur in an object
/// \param[in] x an object containing function symbols
/// \return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  process::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated process find code ---//

/// \brief Returns all action labels that occur in an object
/// \param[in] x an object containing action labels
/// \param[in,out] o an output iterator to which all action labels occurring in x are written.
/// \return All action labels that occur in the term x
template <typename T, typename OutputIterator>
void find_action_labels(const T& x, OutputIterator o)
{
  lps::detail::make_find_action_labels_traverser<process::action_label_traverser>(o)(x);
}

/// \brief Returns all action labels that occur in an object
/// \param[in] x an object containing action labels
/// \return All action labels that occur in the object x
template <typename T>
std::set<lps::action_label> find_action_labels(const T& x)
{
  std::set<lps::action_label> result;
  process::find_action_labels(x, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns all action names that occur in an object
/// \param[in] x an object containing action names
/// \return All action names that occur in the object x
template <typename T>
std::set<core::identifier_string> find_action_names(const T& x)
{
  detail::find_action_names_traverser f;
  f(x);
  return f.result;
}

/// \brief Finds an equation that corresponds to a process identifier
/// \param[in] equations a sequence of process equations
/// \return The equation with the given process identifier. Throws an exception if no such equation was found.
inline
const process_equation& find_equation(const std::vector<process_equation>& equations, const process_identifier& id)
{
  for (std::vector<process_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->identifier() == id)
    {
      return *i;
    }
  }
  throw mcrl2::runtime_error("unknown process identifier " + process::pp(id));
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_FIND_H
