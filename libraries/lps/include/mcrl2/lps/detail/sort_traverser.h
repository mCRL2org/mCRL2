// Author(s): Wieger Wesselink, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/sort_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_SORT_TRAVERSER_H
#define MCRL2_LPS_DETAIL_SORT_TRAVERSER_H

#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/lps/detail/traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  /// \brief
  // TODO: This class adds lps traversal functions to the data sort traverser.
  // This code is duplicated at several places, but it is not known how to
  // prevent that.
  template <typename Derived>
  struct sort_traverser_base: public data::detail::sort_traverser<Derived>
  {
    typedef data::detail::sort_traverser<Derived> super;
    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      static_cast<Derived&>(*this).enter(l);
      static_cast<Derived&>(*this)(l.name());
      static_cast<Derived&>(*this).leave(l);
    }

    /// \brief Traverses an action
    /// \param a An action
    void operator()(const action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      static_cast<Derived&>(*this)(a.label());
      static_cast<Derived&>(*this)(a.arguments());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void operator()(const deadlock& d)
    {
      static_cast<Derived&>(*this).enter(d);
      if (d.has_time()) {
        static_cast<Derived&>(*this)(d.time());
      }
      static_cast<Derived&>(*this).leave(d);
    }

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void operator()(const multi_action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      if (a.has_time()) {
        static_cast<Derived&>(*this)(a.time());
      }
      static_cast<Derived&>(*this)(a.actions());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      static_cast<Derived&>(*this)(s.summation_variables());
      static_cast<Derived&>(*this)(s.condition());
      static_cast<Derived&>(*this)(s.multi_action());
      static_cast<Derived&>(*this)(s.assignments());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      static_cast<Derived&>(*this)(s.summation_variables());
      static_cast<Derived&>(*this)(s.condition());
      static_cast<Derived&>(*this)(s.deadlock());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void operator()(const process_initializer& i)
    {
      static_cast<Derived&>(*this).enter(i);
      static_cast<Derived&>(*this)(i.assignments());
      static_cast<Derived&>(*this).leave(i);
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p) {
      static_cast<Derived&>(*this).enter(p);
      static_cast<Derived&>(*this)(p.process_parameters());
      static_cast<Derived&>(*this)(p.action_summands());
      static_cast<Derived&>(*this)(p.deadlock_summands());
      static_cast<Derived&>(*this).leave(p);
    }

    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void operator()(const specification& spec) {
      static_cast<Derived&>(*this).enter(spec);
      static_cast<Derived&>(*this)(spec.process());
      static_cast<Derived&>(*this)(spec.global_variables());
      static_cast<Derived&>(*this)(spec.initial_process());
      static_cast<Derived&>(*this)(spec.action_labels());
      static_cast<Derived&>(*this).leave(spec);
    }
  };

  /// \brief
  template <typename Derived>
  struct sort_traverser: public sort_traverser_base<Derived>
  {
    typedef sort_traverser_base<Derived> super;
    using super::operator();

    /// \brief Traverse the sorts of an action label
    void operator()(const action_label& l)
    {
      static_cast<Derived&>(*this)(l.sorts());
    }
  };

  /// \brief
  // TODO: This class adds lps traversal functions to the data selective sort traverser.
  // This code is duplicated at several places, but it is not known how to
  // prevent that.
  template <typename Derived, typename AdaptablePredicate>
  class selective_sort_traverser_base: public data::detail::selective_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_traverser<Derived, AdaptablePredicate> super;
    using super::operator();

    /// \brief Traverses an action label
    void operator()(const action_label& l)
    {
      static_cast<Derived&>(*this).enter(l);
      static_cast<Derived&>(*this)(l.name());
      static_cast<Derived&>(*this).leave(l);
    }

    /// \brief Traverses an action
    /// \param a An action
    void operator()(const action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      static_cast<Derived&>(*this)(a.label());
      static_cast<Derived&>(*this)(a.arguments());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a deadlock
    /// \param d A deadlock
    void operator()(const deadlock& d)
    {
      static_cast<Derived&>(*this).enter(d);
      if (d.has_time()) {
        static_cast<Derived&>(*this)(d.time());
      }
      static_cast<Derived&>(*this).leave(d);
    }

    /// \brief Traverses a multi-action
    /// \param a A multi-action
    void operator()(const multi_action& a)
    {
      static_cast<Derived&>(*this).enter(a);
      if (a.has_time()) {
        static_cast<Derived&>(*this)(a.time());
      }
      static_cast<Derived&>(*this)(a.actions());
      static_cast<Derived&>(*this).leave(a);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      static_cast<Derived&>(*this)(s.summation_variables());
      static_cast<Derived&>(*this)(s.condition());
      static_cast<Derived&>(*this)(s.multi_action());
      static_cast<Derived&>(*this)(s.assignments());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      static_cast<Derived&>(*this).enter(s);
      static_cast<Derived&>(*this)(s.summation_variables());
      static_cast<Derived&>(*this)(s.condition());
      static_cast<Derived&>(*this)(s.deadlock());
      static_cast<Derived&>(*this).leave(s);
    }

    /// \brief Traverses a process_initializer
    /// \param s A process_initializer
    void operator()(const process_initializer& i)
    {
      static_cast<Derived&>(*this).enter(i);
      static_cast<Derived&>(*this)(i.assignments());
      static_cast<Derived&>(*this).leave(i);
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p) {
      static_cast<Derived&>(*this).enter(p);
      static_cast<Derived&>(*this)(p.process_parameters());
      static_cast<Derived&>(*this)(p.action_summands());
      static_cast<Derived&>(*this)(p.deadlock_summands());
      static_cast<Derived&>(*this).leave(p);
    }

    /// \brief Traverses a linear process specification
    /// \param spec A linear process specification
    void operator()(const specification& spec) {
      static_cast<Derived&>(*this).enter(spec);
      static_cast<Derived&>(*this)(spec.process());
      static_cast<Derived&>(*this)(spec.global_variables());
      static_cast<Derived&>(*this)(spec.initial_process());
      static_cast<Derived&>(*this)(spec.action_labels());
      static_cast<Derived&>(*this).leave(spec);
    }
  };

  // TODO: find out what this traverser is for
  template <typename Derived, typename AdaptablePredicate>
  struct selective_sort_traverser: public selective_sort_traverser_base<Derived, AdaptablePredicate>
  {
    typedef selective_sort_traverser_base<Derived, AdaptablePredicate> super;

    public:
      selective_sort_traverser()
      { }

      selective_sort_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_SORT_TRAVERSER_H
