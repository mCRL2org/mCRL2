// Author(s): Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_TRAVERSER_H
#define MCRL2_CORE_TRAVERSER_H

#include <set>
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/nil.h"

namespace mcrl2
{

namespace core
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
  template <typename Expression>
  void enter(Expression const&)
  {}

  template <typename Expression>
  void leave(Expression const&)
  {}

  template <typename T>
  void apply(const T& x, typename atermpp::disable_if_container<T>::type* = 0)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).leave(x);
  }

  // traverse containers
  template <typename Container>
  void apply(Container const& container, typename atermpp::enable_if_container<Container>::type* = 0)
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      static_cast<Derived*>(this)->apply(*i);
    }
  }

  // TODO: This dependency on identifier_string and nil should be moved elsewhere...
  void apply(const core::identifier_string& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).leave(x);
  }

  void apply(const core::nil& x)
  {
    static_cast<Derived&>(*this).enter(x);
    static_cast<Derived&>(*this).leave(x);
  }
};

// apply a builder without additional template arguments
template <template <class> class Traverser>
struct apply_traverser: public Traverser<apply_traverser<Traverser> >
{
  typedef Traverser<apply_traverser<Traverser> > super;

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

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_TRAVERSER_H
