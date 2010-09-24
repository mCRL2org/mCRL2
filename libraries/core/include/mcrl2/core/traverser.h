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

#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2 {

namespace core {

  /**
   * \brief expression traverser that visits all sub expressions
   *
   * Types:
   *  \arg Derived the type of a derived class, as per CRTP
   *
   **/
  template <typename Derived>
  class traverser
  {
    public:

      template <typename Expression>
      void enter(Expression const&)
      {}

      template <typename Expression>
      void leave(Expression const&)
      {}

      template <typename Container>
      void operator()(Container const& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
        {
          static_cast<Derived&>(*this)(*i);
        }
      }
  };

  /**
   * \brief expression traverser that can be used for cases in which parts
   * of the expression should not be traversed
   *
   * Types:
   *  \arg Derived the type of a derived class, as per CRTP
   *  \arg AdaptablePredicate is a unary predicate on expressions
   *
   * Before a subterm is explored the predicate is applied to see whether
   * traversal should continue.
   *
   * \see traverser
   **/
  template <typename Derived, typename AdaptablePredicate, template <class> class Traverser = traverser>
  class selective_traverser : public Traverser<Derived>
  {
      typedef Traverser<Derived> super;

    protected:

      AdaptablePredicate m_traverse_condition;

      template <typename Expression>
      void forward_call(Expression const& e)
      {
        static_cast<super&>(*this)(e);
      }

      AdaptablePredicate& traverse_condition()
      {
        return m_traverse_condition;
      }

    public:

      template <typename Expression>
      void operator()(Expression const& e, typename atermpp::detail::disable_if_container<Expression>::type* = 0)
      {
        forward_call(e);
      }

      template <typename Container>
      void operator()(Container const& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
        {
          forward_call(*i);
        }
      }

      // Default constructor (only works if SelectionPredicate is Default Constructible)
      selective_traverser()
      { }

      selective_traverser(AdaptablePredicate predicate) : m_traverse_condition(predicate)
      { }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_TRAVERSER_H
