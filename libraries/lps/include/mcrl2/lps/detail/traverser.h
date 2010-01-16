// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/traverser.h
/// \brief add your file description here.

// lps/test/free_variables_test fails if this is not here (strange dependencies)
#include "mcrl2/lps/specification.h"

#ifndef MCRL2_LPS_DETAIL_TRAVERSER_H
#define MCRL2_LPS_DETAIL_TRAVERSER_H

#include "mcrl2/data/detail/traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief Traversal function object.
  template <typename Derived>
  class traverser: public data::detail::traverser<Derived>
  {
    typedef data::detail::traverser<Derived> super;

    public:

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
        super::operator()(a.arguments());
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

        super::operator()(a.actions());
      } 

      /// \brief Traverses a summand
      /// \param s A summand
      void operator()(const action_summand& s)
      {
        super::operator()(s.summation_variables());
        (*this)(s.condition());
        (*this)(s.multi_action());
        super::operator()(s.assignments());
      }

      /// \brief Traverses a summand
      /// \param s A summand
      void operator()(const deadlock_summand& s)
      {
        super::operator()(s.summation_variables());
        (*this)(s.condition());
        (*this)(s.deadlock());
      }
    
      /// \brief Traverses a process_initializer
      /// \param s A process_initializer
      void operator()(const process_initializer& i)
      {
        super::operator()(i.assignments());
      }

      /// \brief Traverses a linear_process
      /// \param s A linear_process
      void operator()(const linear_process& p)
      {
        super::operator()(p.process_parameters());
        super::operator()(p.action_summands());
        super::operator()(p.deadlock_summands());
      }
                        
      /// \brief Traverses a linear process specification
      /// \param spec A linear process specification
      void operator()(const specification& spec)
      {
        (*this)(spec.process());
        super::operator()(spec.global_variables());
        (*this)(spec.initial_process());
        super::operator()(spec.action_labels());
      }
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_data_traverser: public data::detail::selective_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_traverser<Derived, AdaptablePredicate> super;

    public:
      selective_data_traverser()
      { }

      selective_data_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_TRAVERSER_H
