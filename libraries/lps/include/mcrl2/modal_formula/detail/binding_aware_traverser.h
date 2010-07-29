// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/detail/binding_aware_traverser.h
/// \brief add your file description here.

#include "mcrl2/modal_formula/detail/traverser.h"
#include "mcrl2/data/detail/binding_aware_traverser.h"

#ifndef MCRL2_MODAL_FORMULA_DETAIL_BINDING_AWARE_TRAVERSER_H
#define MCRL2_MODAL_FORMULA_DETAIL_BINDING_AWARE_TRAVERSER_H

namespace mcrl2 {

namespace state_formulas {

namespace detail {

  // Adds state formula traversal functions to the data binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser_base: public data::detail::binding_aware_traverser<Derived>
  {
    typedef data::detail::binding_aware_traverser<Derived> super;
    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/traverser.inc.h"
  };

  /// \brief Binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser: public binding_aware_traverser_base<Derived>
  {
    typedef binding_aware_traverser_base<Derived> super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    /// \brief Returns the left hand sides of the assignments.
    data::variable_list left_hand_sides(const data::assignment_list& a)
    {
      std::vector<data::variable> v;
      for (data::assignment_list::const_iterator i = a.begin(); i != a.end(); ++i)
      {
        v.push_back(i->lhs());
      }
      return atermpp::convert<data::variable_list>(v);
    }

    /// \brief Traverses a forall quantifier
    /// \param x A forall quantifier
    void operator()(const forall& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    /// \brief Traverses a forall quantifier
    /// \param x An exists quantifier
    void operator()(const exists& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    /// \brief Traverses a mu expression
    /// \param x A mu expression
    void operator()(const mu& x)
    {
    	data::variable_list v = left_hand_sides(x.assignments());
      increase_bind_count(v);
      super::operator()(x);
      decrease_bind_count(v);
    }

    /// \brief Traverses a nu expression
    /// \param x A nu expression
    void operator()(const nu& x)
    {
    	data::variable_list v = left_hand_sides(x.assignments());
      increase_bind_count(v);
      super::operator()(x);
      decrease_bind_count(v);
    }
  };

  // Adds state formula traversal functions to the data selective binding aware traverser.
  template <typename Derived, typename AdaptablePredicate>
  class selective_binding_aware_traverser_base: public data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate>
  {
    typedef data::detail::selective_binding_aware_traverser<Derived, AdaptablePredicate> super;
    using super::operator();

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/modal_formula/detail/traverser.inc.h"
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

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_DETAIL_BINDING_AWARE_TRAVERSER_H
