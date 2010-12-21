// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/find_impl.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_FIND_IMPL_H
#define MCRL2_CORE_DETAIL_FIND_IMPL_H

#include <iostream>
#include "mcrl2/core/traverser.h"

namespace mcrl2 {

namespace core {

namespace detail {

  template <typename Expression, typename OutputIterator>
  class collect_action
  {
    protected:
      OutputIterator m_sink;

    public:
      void operator()(Expression const& e)
      {
        m_sink = e;
      }

      collect_action(OutputIterator sink) : m_sink(sink)
      { }
  };

  template <typename Expression, typename Action, template <class> class Traverser = traverser>
  class find_helper : public Traverser<find_helper<Expression, Action, Traverser> >
  {
     typedef Traverser<find_helper<Expression, Action, Traverser> > super;

    protected:
      Action m_action;

    public:
      using super::enter;
      using super::leave;

#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template <typename Container>
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif

      void enter(Expression const& e)
      {
        m_action(e);
      }

      find_helper()
      { }

      find_helper(Action action) : m_action(action)
      { }
  };

  template <typename Expression, template <class> class Traverser, typename OutputIterator>
  find_helper<Expression, collect_action<Expression, OutputIterator&>, Traverser>
  make_find_helper(OutputIterator& sink)
  {
    return find_helper<Expression, collect_action<Expression, OutputIterator&>, Traverser>(collect_action<Expression, OutputIterator&>(sink));
  }

  template <typename Expression, template <class> class Traverser, typename OutputIterator>
  find_helper<Expression, collect_action<Expression, OutputIterator>, Traverser>
  make_find_helper(OutputIterator const& sink)
  {
    return find_helper<Expression, collect_action<Expression, OutputIterator>, Traverser>(collect_action<Expression, OutputIterator>(sink));
  }

  class search_traversal_condition {
    private:
      bool m_result;

    public:

      search_traversal_condition() : m_result(true)
      { }

      bool operator()() {
        return m_result;
      }

      template <typename Expression>
      bool operator()(Expression const&)
      {
        return m_result;
      }

      void operator=(bool result)
      {
        m_result = result;
      }
  };

  /**
   * \brief Component for searching expressions
   *
   * Types:
   *  \arg Expression the type of sub expressions that is considered
   *  \arg AdaptablePredicate represents the search test on expressions (of type Expression)
   *
   * When m_predicate(e) becomes false traversal of sub-expressions will be
   * cut-short. The search_traversal_condition represents a condition
   * that is true initially and becomes false when the search predicate has
   * become false. It is used to cut-short expression traversal to return a
   * result.
   **/
  template <typename Expression, typename AdaptablePredicate, template <class, class> class SelectiveTraverser>
  class search_helper : public SelectiveTraverser<search_helper<Expression, AdaptablePredicate, SelectiveTraverser>, search_traversal_condition> {

      typedef SelectiveTraverser<search_helper<Expression, AdaptablePredicate, SelectiveTraverser>, search_traversal_condition> super;

    protected:

      AdaptablePredicate m_predicate;

    public:

      using super::enter;
      using super::leave;

      void enter(Expression const& e)
      {
        super::m_traverse_condition = super::m_traverse_condition() && !m_predicate(e);
      }

      template <typename Container>
      bool apply(Container const& container)
      {
        (*this)(container);

        return !super::m_traverse_condition();
      }

      search_helper()
      { }

      search_helper(AdaptablePredicate search_predicate) : m_predicate(search_predicate)
      { }
  };

  template <typename Expression, template <class, class> class SelectiveTraverser, typename AdaptablePredicate>
  search_helper<Expression, AdaptablePredicate, SelectiveTraverser>
  make_search_helper(AdaptablePredicate search_predicate)
  {
    return search_helper<Expression, AdaptablePredicate, SelectiveTraverser>(search_predicate);
  }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_FIND_IMPL_H
