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

#include "mcrl2/data/variable.h"
#include "mcrl2/exception.h"

#include "mcrl2/data/find.h"
#include "mcrl2/core/detail/find_impl.h"
#include "mcrl2/data/detail/find_impl.h"
#include "mcrl2/lps/traverser.h"
#include "mcrl2/core/detail/print_utility.h"

namespace mcrl2 {

namespace lps {

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
std::cerr << "<search_free_variable>" << std::endl;
std::cerr << pp(container) << std::endl;
    std::set<data::variable> variables = lps::find_free_variables(container);
std::cerr << core::detail::print_pp_set(variables) << std::endl;
    return variables.find(d) != variables.end();
  }

} // namespace lps

} // namespace mcrl2

#endif
