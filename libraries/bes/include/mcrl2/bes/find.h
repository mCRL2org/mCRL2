// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/find.h
/// \brief add your file description here.

#ifndef MCRL2_BES_FIND_H
#define MCRL2_BES_FIND_H

#include <iterator>
#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/bes/traverser.h"
//#include "mcrl2/bes/binding_aware_traverser.h"
#include "mcrl2/core/detail/find.h"

namespace mcrl2 {

namespace bes {

  /// \brief Returns all boolean variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all variables occurring in t
  ///             are added.
  /// \return All variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_variables(Container const& container, OutputIterator o)
  {
    core::detail::make_find_helper<boolean_variable, bes::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All variables that occur in the term t
  template <typename Container>
  std::set<boolean_variable> find_variables(Container const& container)
  {
    std::set<boolean_variable> result;
    bes::find_variables(container, std::inserter(result, result.end()));
    return result;
  }

/*
  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all variables occurring in t
  ///             are added.
  /// \return All variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_free_variables(Container const& container, OutputIterator o,
  		           typename atermpp::detail::disable_if_container<OutputIterator>::type* = 0)
  {
    core::detail::make_free_variable_find_helper<bes::detail::binding_aware_traverser>(o)(container);
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all variables occurring in t
  ///             are added.
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename OutputIterator, typename Sequence>
  void find_free_variables(Container const& container, OutputIterator o, Sequence const& bound)
  {
    core::detail::make_free_variable_find_helper<bes::detail::binding_aware_traverser>(bound, o)(container);
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All variables that occur in the term t
  template <typename Container>
  std::set<boolean_variable> find_free_variables(Container const& container)
  {
    std::set<boolean_variable> result;
    bes::find_free_variables(container, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename Sequence>
  std::set<boolean_variable> find_free_variables(Container const& container, Sequence const& bound,
                                          typename atermpp::detail::enable_if_container<Sequence, boolean_variable>::type* = 0)
  {
    std::set<boolean_variable> result;
    bes::find_free_variables(container, std::inserter(result, result.end()), bound);
    return result;
  }
*/

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_FIND_H
