// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/lps_sort_traverser.h
/// \brief add your file description here.

// This is to deal with circular header file dependencies
#ifndef MCRL2_LPS_SPECIFICATION_H
#include "mcrl2/lps/specification.h"
#endif

#ifndef MCRL2_LPS_DETAIL_LPS_BINDING_AWARE_TRAVERSER_H
#define MCRL2_LPS_DETAIL_LPS_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/lps/detail/lps_data_traverser.h"
#include "mcrl2/data/detail/binding_aware_traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  /// \note explicit static casts are used to work around malfunctioning MSVC overload resolution for anonymous template members
  template < typename Derived >
  struct lps_binding_aware_traverser : public lps::detail::lps_data_traverser< Derived, data::detail::binding_aware_traverser >
  {
    typedef lps::detail::lps_data_traverser< Derived, data::detail::binding_aware_traverser > super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      increase_bind_count(s.summation_variables());
      (*this)(s.condition());
      (*this)(s.multi_action());
      super::operator()(s.assignments());
      decrease_bind_count(s.summation_variables());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      increase_bind_count(s.summation_variables());
      (*this)(s.condition());
      (*this)(s.deadlock());
      decrease_bind_count(s.summation_variables());
    }
    
    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p)
    {
      increase_bind_count(p.process_parameters());
      super::operator()(p.process_parameters());
      super::operator()(p.action_summands());
      super::operator()(p.deadlock_summands());
      decrease_bind_count(p.process_parameters());
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
