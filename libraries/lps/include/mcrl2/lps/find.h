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
#include "mcrl2/data/detail/find.h"
#include "mcrl2/lps/detail/traverser.h"
#include "mcrl2/lps/detail/sort_traverser.h"
#include "mcrl2/lps/detail/binding_aware_traverser.h"

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
    data::detail::make_find_helper<data::variable, lps::detail::traverser, OutputIterator>(o)(container);
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
    data::detail::make_free_variable_find_helper<lps::detail::binding_aware_traverser>(o)(container);
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
   // data::detail::make_free_variable_find_helper<lps::detail::binding_aware_traverser>(bound, o)(container);
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
    data::detail::make_find_helper<data::sort_expression, lps::detail::sort_traverser>(o)(container);
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

} // namespace lps

} // namespace mcrl2

#endif
