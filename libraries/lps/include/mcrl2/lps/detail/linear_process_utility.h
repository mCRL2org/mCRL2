// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file linear_process_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_LINEAR_PROCESS_UTILITY_H
#define MCRL2_LPS_DETAIL_LINEAR_PROCESS_UTILITY_H

#include <set>
#include <vector>
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/linear_process.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Returns the process parameter names of a linear process
  /// \param p A linear process
  /// \return The process parameter names of a linear process
  inline
  std::set<core::identifier_string> process_parameter_names(const linear_process& p)
  {
    std::set<core::identifier_string> result;
    result.insert(boost::make_transform_iterator(p.process_parameters().begin(), data::detail::data_variable_name()),
                  boost::make_transform_iterator(p.process_parameters().end()  , data::detail::data_variable_name()));
    return result;
  }

  /// \brief Returns the free variable names of a linear process
  /// \param p A linear process
  /// \return The free variable names of a linear process
  inline
  std::set<core::identifier_string> free_variable_names(const linear_process& p)
  {
    std::set<core::identifier_string> result;
    result.insert(boost::make_transform_iterator(p.free_variables().begin(), data::detail::data_variable_name()),
                  boost::make_transform_iterator(p.free_variables().end()  , data::detail::data_variable_name()));
    return result;
  }

  /// \brief Returns the summation variable names of a linear process
  /// \param p A linear process
  /// \return The summation variable names of a linear process
  inline
  std::set<core::identifier_string> summand_variable_names(const linear_process& p)
  {
    std::set<core::identifier_string> result;
    for (summand_list::iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      result.insert(
        boost::make_transform_iterator(i->summation_variables().begin(), data::detail::data_variable_name()),
        boost::make_transform_iterator(i->summation_variables().end(),   data::detail::data_variable_name())
      );
    }
    return result;
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LINEAR_PROCESS_UTILITY_H
