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
  void enter(const Expression&)
  {}

  template <typename Expression>
  void leave(const Expression&)
  {}

  // traverse containers
  template <typename Container>
    requires atermpp::is_container<Container>::value
  void apply(const Container& container)
  {
    for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
    {
      static_cast<Derived*>(this)->apply(*i);
    }
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

/// \brief Allows a traverser to terminate early.
/// \details This traverser wrapper adds the option to stop a term traversal early
/// by setting an additional class variable "terminated" to true. In that case
/// the remainder of a term is not traversed anymore. 
/// THIS IS EXPERIMENTAL AND NOT USED YET.
template <class Derived>
struct add_early_termination: public Derived
{
  bool terminated=false;

  using derived = Derived;

  using derived::enter;
  using derived::leave;
  using derived::apply;

  
  template <class T>
  void enter(const T& x)
  {
    if (!terminated) 
    {
      derived::enter(x);
    }
  }

  template <class T>
  void leave(const T& x)
  {
    if (!terminated) 
    {
      derived::leave(x);
    }
  }

  template <class T>
  void apply(T x)
  {
std::cerr << "VISIT " << terminated << "  " << x << "\n";
    if (!terminated) 
    {
      derived::apply(x);
    }
  }
};


} // namespace mcrl2::core

#endif // MCRL2_CORE_TRAVERSER_H
