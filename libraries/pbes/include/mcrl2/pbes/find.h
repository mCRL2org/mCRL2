// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find.h
/// \brief Search functions of the pbes library.

#ifndef MCRL2_PBES_FIND_H
#define MCRL2_PBES_FIND_H

#include <set>
#include <iterator>
#include <functional>
#include "mcrl2/core/detail/find_impl.h"     
#include "mcrl2/data/detail/find_impl.h"     
#include "mcrl2/data/variable.h" 
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/detail/pbes_free_variable_finder.h"
#include "mcrl2/pbes/detail/free_variable_visitor.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/add_binding.h"
#include "mcrl2/exception.h"
                                 
namespace mcrl2 {                
                                 
namespace pbes_system {          

//--- start generated pbes_system find code ---//
#ifdef MCRL2_NEW_FIND_VARIABLES
  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \return All variables that occur in the term x
  template <typename T, typename OutputIterator>
  void find_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_variables_traverser<pbes_system::traverser>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_variables(const T& x)
  {
    std::set<data::variable> result;
    pbes_system::find_variables(x, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are added.
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator>
  void find_free_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_free_variables_traverser<pbes_system::variable_traverser, pbes_system::add_data_variable_binding>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \param[in] bound a container of variables
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator, typename VariableContainer>
  void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
  {
    data::detail::make_find_free_variables_traverser<pbes_system::variable_traverser, pbes_system::add_data_variable_binding>(o, bound)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All free variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_free_variables(const T& x)
  {
    std::set<data::variable> result;
    pbes_system::find_free_variables(x, std::inserter(result, result.end()));
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
    pbes_system::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
    return result;
  }

  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
  /// \return All identifiers that occur in the term x
  template <typename T, typename OutputIterator>
  void find_identifiers(const T& x, OutputIterator o)
  {
    data::detail::make_find_identifiers_traverser<pbes_system::traverser>(o)(x);
  }
  
  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \return All identifiers that occur in the object x
  template <typename T>
  std::set<core::identifier> find_identifiers(const T& x)
  {
    std::set<core::identifier> result;
    pbes_system::find_identifiers(x, std::inserter(result, result.end()), bound);
    return result;
  }
 
#endif // MCRL2_NEW_FIND_VARIABLES
//--- end generated pbes_system find code ---//

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_variables(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<data::variable, pbes_system::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_variables(Container const& container)
  {
    std::set<data::variable> result;
    pbes_system::find_variables(container, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_propositional_variable_instantiations(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<propositional_variable_instantiation, pbes_system::traverser, OutputIterator>(o)(container);
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

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_free_variables(Container const& container, OutputIterator o,
  		           typename atermpp::detail::disable_if_container<OutputIterator>::type* = 0)
  {
    data::detail::make_free_variable_find_helper<pbes_system::binding_aware_traverser>(o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All data variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename OutputIterator, typename Sequence>
  void find_free_variables(Container const& container, OutputIterator o, Sequence const& bound)
  {
    data::detail::make_free_variable_find_helper<pbes_system::binding_aware_traverser>(bound, o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_free_variables(Container const& container)
  {
    std::set<data::variable> result;
    pbes_system::find_free_variables(container, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All data variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename Sequence>
  std::set<data::variable> find_free_variables(Container const& container, Sequence const& bound,
                                          typename atermpp::detail::enable_if_container<Sequence, data::variable>::type* = 0)
  {
    std::set<data::variable> result;
    pbes_system::find_free_variables(container, std::inserter(result, result.end()), bound);
    return result;
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \param[in] o an output iterator
  /// \return All sort expressions that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_sort_expressions(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<data::sort_expression, pbes_system::traverser>(o)(container);
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \return All sort expressions that occur in the term t
  template <typename Container>
  std::set<data::sort_expression> find_sort_expressions(Container const& container)
  {
    std::set<data::sort_expression> result;
    pbes_system::find_sort_expressions(container, std::inserter(result, result.end()));
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_H
