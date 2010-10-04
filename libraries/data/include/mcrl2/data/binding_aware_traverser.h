// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/binding_aware_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_BINDING_AWARE_TRAVERSER_H
#define MCRL2_DATA_BINDING_AWARE_TRAVERSER_H

#include "mcrl2/core/binding_aware_traverser.h"

namespace mcrl2 {

namespace data {

  // Adds data traversal functions to the core binding aware traverser.
  template <typename Derived>
  struct binding_aware_traverser_helper: public core::binding_aware_traverser<Derived, variable>
  {
    typedef core::binding_aware_traverser<Derived, variable> super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

// Include file with traverse member functions. This is to avoid duplication.
#include "mcrl2/data/detail/traverser.inc.h"
  };

  template <typename Derived>
  struct binding_aware_traverser : public binding_aware_traverser_helper<Derived>
  {
    typedef binding_aware_traverser_helper<Derived> super;

    using super::operator();
    using super::increase_bind_count;
    using super::decrease_bind_count;

    // TODO: The implementation below is kind of ugly, since it uses copy and paste
    // of the traversal. Two alternatives come to mind:
    // 1) Use the enter and leave functions instead of operator().
    // 2) Add an extra traverse member function like this:
    //
    // void operator()(where_clause const& x)
    // {
    //   static_cast<Derived&>(*this).enter(x);
    //   static_cast<Derived&>(*this).traverse(x);
    //   static_cast<Derived&>(*this).leave(x);
    // }
    //
    // void traverse(where_clause const& x)
    // {
    //   static_cast<Derived&>(*this)(x.body());
    //   static_cast<Derived&>(*this)(x.declarations());
    // }
    //
    // Then the binding aware traverser can be implemented by overriding
    // operator() function, without the need to copy the traversal code.
    //
    void operator()(where_clause const& x)
    {
      increase_bind_count(make_assignment_left_hand_side_range(x.declarations()));
      super::operator()(x);
      decrease_bind_count(make_assignment_left_hand_side_range(x.declarations()));
    }

    void operator()(lambda const& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    void operator()(exists const& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }

    void operator()(forall const& x)
    {
      increase_bind_count(x.variables());
      super::operator()(x);
      decrease_bind_count(x.variables());
    }
  };

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BINDING_AWARE_TRAVERSER_H
