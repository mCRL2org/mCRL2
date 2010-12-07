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

//      // traverse non-containers
//      template <typename T>
//      void operator()(const T&, typename atermpp::detail::disable_if_container<T>::type* = 0)
//      {
//        // skip
//      }

      // traverse containers
      template <typename Container>
      void operator()(Container const& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
        {
          static_cast<Derived&>(*this)(*i);
        }
      }

      // TODO: This dependency on identifier_string should be moved elsewhere...
      void operator()(const core::identifier_string& x)
      {
        static_cast<Derived&>(*this).enter(x);
        static_cast<Derived&>(*this).leave(x);
      }
  };

  /// \brief Traverser that defines functions for maintaining bound variables.
  template <typename Derived, typename Variable>
  class binding_aware_traverser : public traverser<Derived>
  {
    public:
      typedef traverser<Derived> super;
      typedef Variable variable_type;

      using super::operator();
      using super::enter;
      using super::leave;

    protected:
      std::multiset<variable_type> m_bound_variables;

      /// \brief Add a variable to the multiset of bound variables.
      void increase_bind_count(const variable_type& var)
      {
        m_bound_variables.insert(var);
      }

      /// \brief Add a sequence of variables to the multiset of bound variables.
      template <typename Container>
      void increase_bind_count(const Container& variables, typename atermpp::detail::enable_if_container<Container, variable_type>::type* = 0)
      {
        for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
        {
          m_bound_variables.insert(*i);
        }
      }

      /// \brief Remove a variable from the multiset of bound variables.
      void decrease_bind_count(const variable_type& var)
      {
        m_bound_variables.erase(m_bound_variables.find(var));
      }

      /// \brief Remove a sequence of variables from the multiset of bound variables.
      template <typename Container>
      void decrease_bind_count(const Container& variables, typename atermpp::detail::enable_if_container<Container, variable_type>::type* = 0)
      {
        for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
        {
          m_bound_variables.erase(m_bound_variables.find(*i));
        }
      }

    public:
      /// \brief Constructor
      binding_aware_traverser()
      { }

      /// \brief Constructor
      template <typename Container>
      binding_aware_traverser(Container const& bound_variables,
                              typename atermpp::detail::enable_if_container<Container, variable_type>::type* = 0) :
                              m_bound_variables(bound_variables.begin(), bound_variables.end())
      { }

      /// \brief Returns true if the variable v is bound.
      bool is_bound(variable_type const& v) const
      {
        return m_bound_variables.find(v) != m_bound_variables.end();
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
