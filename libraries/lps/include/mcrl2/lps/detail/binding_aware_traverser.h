// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/binding_aware_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_DETAIL_BINDING_AWARE_TRAVERSER_H
#define MCRL2_LPS_DETAIL_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/lps/detail/traverser.h"
#include "mcrl2/data/detail/binding_aware_traverser.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  // Adds lps traversal functions to the data binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser_base: public data::detail::binding_aware_traverser<Derived>
  {
    typedef data::detail::binding_aware_traverser<Derived> super;
    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/lps/detail/traverser.inc.h"
  };

  /// \brief Binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser: public binding_aware_traverser_base<Derived>
  {
    typedef binding_aware_traverser_base<Derived> super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const action_summand& s)
    {
      increase_bind_count(s.summation_variables());
      super::operator()(s);
      decrease_bind_count(s.summation_variables());
    }

    /// \brief Traverses a summand
    /// \param s A summand
    void operator()(const deadlock_summand& s)
    {
      increase_bind_count(s.summation_variables());
      super::operator()(s);
      decrease_bind_count(s.summation_variables());
    }

    /// \brief Traverses a linear_process
    /// \param s A linear_process
    void operator()(const linear_process& p)
    {
      increase_bind_count(p.process_parameters());
      super::operator()(p);
      decrease_bind_count(p.process_parameters());
    }
  };

  // Adds lps traversal functions to the data selective binding aware traverser.
  template <typename Derived, typename AdaptablePredicate>
  class selective_binding_aware_traverser_base: public data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate> super;
    using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/lps/detail/traverser.inc.h"
  };

  template <typename Derived, typename AdaptablePredicate>
  class selective_binding_aware_traverser: public selective_binding_aware_traverser_base<Derived, AdaptablePredicate>
  {
    typedef selective_binding_aware_traverser_base<Derived, AdaptablePredicate> super;
    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    public:
      selective_binding_aware_traverser()
      { }

      selective_binding_aware_traverser(AdaptablePredicate predicate) : super(predicate)
      { }
  };

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_BINDING_AWARE_TRAVERSER_H
