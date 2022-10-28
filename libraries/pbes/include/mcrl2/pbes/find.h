// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find.h
/// \brief Search functions of the pbes library.

#ifndef MCRL2_PBES_FIND_H
#define MCRL2_PBES_FIND_H

#include "mcrl2/pbes/add_binding.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

template <template <class> class Traverser, class OutputIterator>
struct find_propositional_variables_traverser: public Traverser<find_propositional_variables_traverser<Traverser, OutputIterator> >
{
  typedef Traverser<find_propositional_variables_traverser<Traverser, OutputIterator> > super;
  using super::enter;
  using super::leave;
  using super::apply;

  OutputIterator out;

  find_propositional_variables_traverser(OutputIterator out_)
    : out(out_)
  {}

  // instead of deriving from a traverser in the data library
  void apply(const data::data_expression&)
  {}

  void apply(const propositional_variable_instantiation& v)
  {
    *out = v;
  }
};

template <template <class> class Traverser, class OutputIterator>
find_propositional_variables_traverser<Traverser, OutputIterator>
make_find_propositional_variables_traverser(OutputIterator out)
{
  return find_propositional_variables_traverser<Traverser, OutputIterator>(out);
}

} // namespace detail

//--- start generated pbes_system find code ---//
/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\param[in,out] o an output iterator to which all variables occurring in x are written.
/// \\return All variables that occur in the term x
template <typename T, typename OutputIterator>
void find_all_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_all_variables_traverser<pbes_system::variable_traverser>(o).apply(x);
}

/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\return All variables that occur in the object x
template <typename T>
std::set<data::variable> find_all_variables(const T& x)
{
  std::set<data::variable> result;
  pbes_system::find_all_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\param[in,out] o an output iterator to which all variables occurring in x are added.
/// \\return All free variables that occur in the object x
template <typename T, typename OutputIterator>
void find_free_variables(const T& x, OutputIterator o)
{
  data::detail::make_find_free_variables_traverser<pbes_system::data_expression_traverser, pbes_system::add_data_variable_traverser_binding>(o).apply(x);
}

/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\param[in,out] o an output iterator to which all variables occurring in x are written.
/// \\param[in] bound a container of variables
/// \\return All free variables that occur in the object x
template <typename T, typename OutputIterator, typename VariableContainer>
void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
{
  data::detail::make_find_free_variables_traverser<pbes_system::data_expression_traverser, pbes_system::add_data_variable_traverser_binding>(o, bound).apply(x);
}

/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\return All free variables that occur in the object x
template <typename T>
std::set<data::variable> find_free_variables(const T& x)
{
  std::set<data::variable> result;
  pbes_system::find_free_variables(x, std::inserter(result, result.end()));
  return result;
}

/// \\brief Returns all variables that occur in an object
/// \\param[in] x an object containing variables
/// \\param[in] bound a bound a container of variables
/// \\return All free variables that occur in the object x
template <typename T, typename VariableContainer>
std::set<data::variable> find_free_variables_with_bound(const T& x, VariableContainer const& bound)
{
  std::set<data::variable> result;
  pbes_system::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
  return result;
}

/// \\brief Returns all identifiers that occur in an object
/// \\param[in] x an object containing identifiers
/// \\param[in,out] o an output iterator to which all identifiers occurring in x are written.
/// \\return All identifiers that occur in the term x
template <typename T, typename OutputIterator>
void find_identifiers(const T& x, OutputIterator o)
{
  data::detail::make_find_identifiers_traverser<pbes_system::identifier_string_traverser>(o).apply(x);
}

/// \\brief Returns all identifiers that occur in an object
/// \\param[in] x an object containing identifiers
/// \\return All identifiers that occur in the object x
template <typename T>
std::set<core::identifier_string> find_identifiers(const T& x)
{
  std::set<core::identifier_string> result;
  pbes_system::find_identifiers(x, std::inserter(result, result.end()));
  return result;
}

/// \\brief Returns all sort expressions that occur in an object
/// \\param[in] x an object containing sort expressions
/// \\param[in,out] o an output iterator to which all sort expressions occurring in x are written.
/// \\return All sort expressions that occur in the term x
template <typename T, typename OutputIterator>
void find_sort_expressions(const T& x, OutputIterator o)
{
  data::detail::make_find_sort_expressions_traverser<pbes_system::sort_expression_traverser>(o).apply(x);
}

/// \\brief Returns all sort expressions that occur in an object
/// \\param[in] x an object containing sort expressions
/// \\return All sort expressions that occur in the object x
template <typename T>
std::set<data::sort_expression> find_sort_expressions(const T& x)
{
  std::set<data::sort_expression> result;
  pbes_system::find_sort_expressions(x, std::inserter(result, result.end()));
  return result;
}

/// \\brief Returns all function symbols that occur in an object
/// \\param[in] x an object containing function symbols
/// \\param[in,out] o an output iterator to which all function symbols occurring in x are written.
/// \\return All function symbols that occur in the term x
template <typename T, typename OutputIterator>
void find_function_symbols(const T& x, OutputIterator o)
{
  data::detail::make_find_function_symbols_traverser<pbes_system::data_expression_traverser>(o).apply(x);
}

/// \\brief Returns all function symbols that occur in an object
/// \\param[in] x an object containing function symbols
/// \\return All function symbols that occur in the object x
template <typename T>
std::set<data::function_symbol> find_function_symbols(const T& x)
{
  std::set<data::function_symbol> result;
  pbes_system::find_function_symbols(x, std::inserter(result, result.end()));
  return result;
}
//--- end generated pbes_system find code ---//

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \param[in,out] o an output iterator to which all data variables occurring in t
///             are added.
/// \return All data variables that occur in the term t
template <typename Container, typename OutputIterator>
void find_propositional_variable_instantiations(Container const& container, OutputIterator o)
{
  pbes_system::detail::make_find_propositional_variables_traverser<pbes_system::pbes_expression_traverser>(o).apply(container);
}

/// \brief Returns all data variables that occur in a range of expressions
/// \param[in] container a container with expressions
/// \return All data variables that occur in the term t
template <typename Container>
std::set<propositional_variable_instantiation> find_propositional_variable_instantiations(Container const& container)
{
  std::set<propositional_variable_instantiation> result;
  pbes_system::find_propositional_variable_instantiations(container, std::inserter(result, result.end()));
  return result;
}

/// \brief Returns true if the term has a given variable as subterm.
/// \param[in] x an expression
/// \param[in] v a variable
/// \return True if v occurs in x.
template <typename T>
bool search_variable(const T& x, const data::variable& v)
{
  data::detail::search_variable_traverser<pbes_system::variable_traverser> f(v);
  f.apply(x);
  return f.found;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_H
