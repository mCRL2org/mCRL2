// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/find_impl.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_DETAIL_FIND_IMPL_H
#define MCRL2_DATA_DETAIL_FIND_IMPL_H

#include "mcrl2/core/detail/find_impl.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/assignment.h"

namespace mcrl2 {

namespace data {

namespace detail {

  template <typename Action, template <class> class BindingAwareTraverser>
  class free_variable_find_helper : public BindingAwareTraverser<free_variable_find_helper<Action, BindingAwareTraverser> > {

     typedef BindingAwareTraverser<free_variable_find_helper<Action, BindingAwareTraverser> > super;

    protected:

      Action m_action;

    public:

      using super::enter;

      void enter(variable const& v)
      {
        if (!super::is_bound(v))
        {
          m_action(v);
        }
      }

      void operator()(assignment const& a)
      {
        (*this)(a.rhs());
      }

      // Workaround for mal-functioning MSVC 2008 overload resolution
      template <typename Expression>
      void operator()(Expression const& a)
      {
        super::operator()(a);
      }

      free_variable_find_helper()
      { }

      free_variable_find_helper(Action action) : m_action(action)
      { }

      template <typename Container>
      free_variable_find_helper(Container const& bound, Action action) : super(bound), m_action(action)
      { }
  };

  template <template <class> class BindingAwareTraverser, typename OutputIterator>
  free_variable_find_helper<core::detail::collect_action<variable, OutputIterator&>, BindingAwareTraverser>
  make_free_variable_find_helper(OutputIterator& sink)
  {
    return free_variable_find_helper<core::detail::collect_action<variable, OutputIterator&>, BindingAwareTraverser>(
							core::detail::collect_action<variable, OutputIterator&>(sink));
  }

  template <template <class> class BindingAwareTraverser, typename Container, typename OutputIterator>
  free_variable_find_helper<core::detail::collect_action<variable, OutputIterator&>, BindingAwareTraverser>
  make_free_variable_find_helper(Container const& bound, OutputIterator& sink)
  {
    return free_variable_find_helper<core::detail::collect_action<variable, OutputIterator&>, BindingAwareTraverser>(bound,
							core::detail::collect_action<variable, OutputIterator&>(sink));
  }

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FIND_IMPL_H
