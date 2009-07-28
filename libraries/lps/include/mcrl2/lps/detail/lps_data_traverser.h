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

#ifndef MCRL2_LPS_DETAIL_LPS_DATA_TRAVERSER_H
#define MCRL2_LPS_DETAIL_LPS_DATA_TRAVERSER_H

#include "mcrl2/data/detail/traverser.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Function object for applying a substitution to LPS data types.
  template < typename Derived >
  struct lps_data_traverser : public data::detail::traverser< Derived >
  {
    typedef data::detail::traverser< Derived > super;

    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      (*this)(l.name());
    }

    /// \brief Traverses an action
    /// \param a An action
    void operator()(const action& a)
    {
      (*this)(a.label());
      (*this)(a.arguments());
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void operator()(const deadlock& d)
    {
      if (d.has_time())
      {
        (*this)(d.time());
      }
    } 

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void operator()(const multi_action& a)
    {
      if (a.has_time())
      {
        (*this)(a.time());
      }

      (*this)(a.actions());
    } 

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      (*this)(s.condition());
      (*this)(s.multi_action());
      (*this)(s.assignments());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      (*this)(s.condition());
      (*this)(s.deadlock());
    }
    
    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void operator()(const process_initializer& i)
    {
      (*this)(i.assignments());
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p)
    {
      (*this)(p.process_parameters());
      (*this)(p.action_summands());
      (*this)(p.deadlock_summands());
    }
                        
    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void operator()(const specification& spec)
    {
      (*this)(spec.process());
      (*this)(spec.global_variables());
      (*this)(spec.initial_process());
      (*this)(spec.action_labels());
    }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_LPS_SORT_TRAVERSER_H
