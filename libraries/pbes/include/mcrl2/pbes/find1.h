// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find1.h
/// \brief Search functions of the pbes library.

#ifndef MCRL2_PBES_FIND1_H
#define MCRL2_PBES_FIND1_H

#include <set>
#include <iterator>
#include <functional>
#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/pbes/detail/traverser.h"
#include "mcrl2/pbes/detail/sort_traverser.h"
#include "mcrl2/pbes/detail/binding_aware_traverser.h"

namespace mcrl2 {

namespace pbes_system {

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_variables(Container const& container, OutputIterator o)
  {
    data::detail::make_find_helper<data::variable, pbes::detail::traverser, OutputIterator>(o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_variables(Container const& container)
  {
    std::set<data::variable> result;
    pbes::find_variables(container, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \return All data variables that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_free_variables1(Container const& container, OutputIterator o,
  		           typename data::detail::disable_if_container<OutputIterator>::type* = 0)
  {
    data::detail::make_free_variable_find_helper<pbes::detail::binding_aware_traverser>(o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in,out] o an output iterator to which all data variables occurring in t
  ///             are added.
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All data variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename OutputIterator, typename Sequence>
  void find_free_variables1(Container const& container, OutputIterator o, Sequence const& bound)
  {
   // data::detail::make_free_variable_find_helper<pbes::detail::binding_aware_traverser>(bound, o)(container);
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \return All data variables that occur in the term t
  template <typename Container>
  std::set<data::variable> find_free_variables1(Container const& container)
  {
    std::set<data::variable> result;
    pbes::find_free_variables1(container, std::inserter(result, result.end()));
    return result;
  }

  /// \brief Returns all data variables that occur in a range of expressions
  /// \param[in] container a container with expressions
  /// \param[in] bound a set of variables that should be considered as bound
  /// \return All data variables that occur in the term t
  /// TODO prevent copy of Sequence
  template <typename Container, typename Sequence>
  std::set<data::variable> find_free_variables1(Container const& container, Sequence const& bound,
                                          typename data::detail::enable_if_container<Sequence, data::variable>::type* = 0)
  {
    std::set<data::variable> result;
    pbes::find_free_variables1(container, std::inserter(result, result.end()), bound);
    return result;
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \param[in] o an output iterator
  /// \return All sort expressions that occur in the term t
  template <typename Container, typename OutputIterator>
  void find_sort_expressions(Container const& container, OutputIterator o)
  {
    data::detail::make_find_helper<data::sort_expression, pbes::detail::sort_traverser>(o)(container);
  }

  /// \brief Returns all sort expressions that occur in the term t
  /// \param[in] container an expression or container of expressions
  /// \return All sort expressions that occur in the term t
  template <typename Container>
  std::set<data::sort_expression> find_sort_expressions(Container const& container)
  {
    std::set<data::sort_expression> result;
    pbes::find_sort_expressions(container, std::inserter(result, result.end()));
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND1_H
