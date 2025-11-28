// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_TRAVERSER_H
#define MCRL2_CORE_TRAVERSER_H

#include "mcrl2/core/identifier_string.h"

namespace mcrl2::core
{

/**
 * \brief expression traverser that visits all sub expressions
 *
 * Types:
 *  \arg Derived the type of a derived class, as per CRTP
 *
 **/
template <typename Derived>
struct traverser
{

  // The function below may not be uncommented. It applies the function apply when
  // no other function apply in the traverser framework is applicable. This apply has no effect,
  // but this is not noted by the compiler, and may not be the intention. If it is the intention 
  // that the apply has no effect an explicit apply without effect of the appropriate type must 
  // be added.  
  /* template <typename T>
  void apply(const T& x, typename atermpp::disable_if_container<T>::type* = nullptr)
  {
    assert(0)
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).leave(x);
  }
  DO NOT REMOVE THIS COMMENT */

  template <typename Expression>
  void enter(Expression const&)
  {}

  template <typename Expression>
  void leave(Expression const&)
  {}

  // traverse containers
  template <typename Container>
  void apply(const Container& container, typename atermpp::enable_if_container<Container>::type* = nullptr)
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      static_cast<Derived*>(this)->apply(*i);
    }
  }

  // TODO: This dependency on identifier_string should be moved elsewhere...
  void apply(const core::identifier_string& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).leave(x);
  }
};

// apply a builder without additional template arguments
template <template <class> class Traverser>
struct apply_traverser: public Traverser<apply_traverser<Traverser> >
{
  using super = Traverser<apply_traverser<Traverser>>;

  using super::enter;
  using super::leave;
  using super::apply;
};

template <template <class> class Traverser>
apply_traverser<Traverser>
make_apply_traverser()
{
  return apply_traverser<Traverser>();
}

} // namespace mcrl2::core

#endif // MCRL2_CORE_TRAVERSER_H
