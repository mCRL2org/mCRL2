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

//  /// \brief Returns true if the term has a given variable as subterm.
//  /// \param[in] container an expression or container with expressions
//  /// \param[in] v an expression or container with expressions
//  /// \param d A variable
//  /// \return True if the term has a given variable as subterm.
//  template <typename Container>
//  bool search_variable(Container const& container, const data::variable& v)
//  {
//    return data::detail::make_search_helper<data::variable, lps::detail::selective_data_traverser>(data::detail::compare_variable(v)).apply(container);
//  }

  /// \brief Returns true if the term has a given variable as subterm.
  /// \param[in] container an expression or container with expressions
  /// \param d A data variable
  /// \return True if the term has a given variable as subterm.
  template <typename Container>
  bool search_free_variable(Container container, const data::variable& d)
  {
    return data::detail::make_free_variable_search_helper<lps::detail::selective_binding_aware_traverser>(data::detail::compare_variable(d)).apply(container);
  }
  
//  /// \brief Returns true if the term has a given sort expression as subterm.
//  /// \param[in] container an expression or container of expressions
//  /// \param[in] s A sort expression
//  /// \return True if the term has a given sort expression as subterm.
//  template <typename Container>
//  bool search_sort_expression(Container const& container, const data::sort_expression& s)
//  {
//    return data::detail::make_search_helper<data::sort_expression, lps::detail::selective_sort_traverser>(data::detail::compare_sort(s)).apply(container);
//  } 
//
//  /// \brief Returns true if the term has a given data expression as subterm.
//  /// \param[in] container an expression or container of expressions
//  /// \param[in] s A data expression
//  /// \return True if the term has a given data expression as subterm.
//  template <typename Container>
//  bool search_data_expression(Container const& container, const data::data_expression& s)
//  {
//    return data::detail::make_search_helper<data::data_expression, lps::detail::selective_data_traverser>(data::detail::compare_term<data::data_expression>(s)).apply(container);
//  }  

} // namespace lps

} // namespace mcrl2

#endif
