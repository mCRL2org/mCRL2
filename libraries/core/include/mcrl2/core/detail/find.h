// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/find.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DETAIL_FIND_H
#define MCRL2_CORE_DETAIL_FIND_H

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

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_FIND_H
