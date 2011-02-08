// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find.h
/// \brief Search functions of the data library.

#ifndef MCRL2_LPS_FIND_H
#define MCRL2_LPS_FIND_H

#include "mcrl2/exception.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/find.h"
#include "mcrl2/lps/traverser.h"
#include "mcrl2/lps/add_binding.h"

#include "mcrl2/core/detail/find_impl.h"
#include "mcrl2/data/detail/find_impl.h"

namespace mcrl2 {

namespace lps {

//--- start generated lps find code ---//
  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \return All variables that occur in the term x
  template <typename T, typename OutputIterator>
  void find_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_variables_traverser<lps::traverser>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_variables(const T& x)
  {
    std::set<data::variable> result;
    lps::find_variables(x, std::inserter(result, result.end()));
    return result;
  }

#ifdef MCRL2_NEW_FIND_VARIABLES
  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are added.
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator>
  void find_free_variables(const T& x, OutputIterator o)
  {
    data::detail::make_find_free_variables_traverser<lps::variable_traverser, lps::add_data_variable_binding>(o)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \param[in,out] o an output iterator to which all variables occurring in x are written.
  /// \param[in] bound a container of variables
  /// \return All free variables that occur in the object x
  template <typename T, typename OutputIterator, typename VariableContainer>
  void find_free_variables_with_bound(const T& x, OutputIterator o, const VariableContainer& bound)
  {
    data::detail::make_find_free_variables_traverser<lps::variable_traverser, lps::add_data_variable_binding>(o, bound)(x);
  }

  /// \brief Returns all variables that occur in an object
  /// \param[in] x an object containing variables
  /// \return All free variables that occur in the object x
  template <typename T>
  std::set<data::variable> find_free_variables(const T& x)
  {
    std::set<data::variable> result;
    lps::find_free_variables(x, std::inserter(result, result.end()));
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
    lps::find_free_variables_with_bound(x, std::inserter(result, result.end()), bound);
    return result;
  }
#endif // MCRL2_NEW_FIND_VARIABLES

  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \param[in,out] o an output iterator to which all identifiers occurring in x are written.
  /// \return All identifiers that occur in the term x
  template <typename T, typename OutputIterator>
  void find_identifiers(const T& x, OutputIterator o)
  {
    data::detail::make_find_identifiers_traverser<lps::traverser>(o)(x);
  }
  
  /// \brief Returns all identifiers that occur in an object
  /// \param[in] x an object containing identifiers
  /// \return All identifiers that occur in the object x
  template <typename T>
  std::set<core::identifier_string> find_identifiers(const T& x)
  {
    std::set<core::identifier_string> result;
    lps::find_identifiers(x, std::inserter(result, result.end()));
    return result;
  }
//--- end generated lps find code ---//

/*
  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_variables(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<data::variable, lps::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_variables(Container const& container)
  {
    std::set<data::variable> result;
    lps::find_variables(container, std::inserter(result, result.end()));
    return result;
  }
*/

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_free_variables(Container const& container, OutputIterator o,
  		           typename atermpp::detail::disable_if_container<OutputIterator>::type* = 0)
  {
    data::detail::make_free_variable_find_helper<lps::binding_aware_traverser>(o)(container);
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
    data::detail::make_free_variable_find_helper<lps::binding_aware_traverser>(bound, o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_free_variables(Container const& container)
  {
    std::set<data::variable> result;
    lps::find_free_variables(container, std::inserter(result, result.end()));
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
    lps::find_free_variables(container, std::inserter(result, result.end()), bound);
    return result;
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \param[in] o an output iterator
  /// \return All sort expressions that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_sort_expressions(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<data::sort_expression, lps::traverser>(o)(container);
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \return All sort expressions that occur in the term t
  template <typename Container>
  std::set<data::sort_expression> find_sort_expressions(Container const& container)
  {
    std::set<data::sort_expression> result;
    lps::find_sort_expressions(container, std::inserter(result, result.end()));
    return result;
  }

  inline
  std::string pp(const action_summand_vector& v)
  {
    std::ostringstream out;
    for (action_summand_vector::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      out << core::pp(action_summand_to_aterm(*i)) << std::endl;
    }
    return out.str();
  }

  inline
  std::string pp(const deadlock_summand_vector& v)
  {
    std::ostringstream out;
    for (deadlock_summand_vector::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      out << core::pp(deadlock_summand_to_aterm(*i)) << std::endl;
    }
    return out.str();
  }

  /// \brief Returns true if the term has a given variable as subterm.
  /// \param[in] container an expression or container with expressions
  /// \param d A data variable
  /// \return True if the term has a given variable as subterm.
  template <typename Container>
  bool search_free_variable(const Container& container, const data::variable& d)
  {
    // TODO: replace this by a more efficient implementation
    std::set<data::variable> variables = lps::find_free_variables(container);
    return variables.find(d) != variables.end();
  }

} // namespace lps

} // namespace mcrl2

#endif
