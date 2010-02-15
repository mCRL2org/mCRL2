// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/find.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_FIND_H
#define MCRL2_DATA_DETAIL_FIND_H

#include "mcrl2/data/detail/traverser.h"
#include "mcrl2/data/detail/sort_traverser.h"
#include "mcrl2/data/detail/binding_aware_traverser.h"

namespace mcrl2 {

namespace data {

/// \cond INTERNAL_DOCS
namespace detail {

  template < typename Expression, typename OutputIterator >
  class collect_action {

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

  template < typename Expression, typename Action, template < class > class Traverser = detail::traverser >
  class find_helper : public Traverser< find_helper< Expression, Action, Traverser > > {

     typedef Traverser< find_helper< Expression, Action, Traverser > > super;

    protected:

      Action m_action;

    public:

      using super::enter;
      using super::leave;

#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template < typename Container >
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

  template < typename Expression, template < class > class Traverser, typename OutputIterator >
  find_helper< Expression, collect_action< Expression, OutputIterator& >, Traverser >
  make_find_helper(OutputIterator& sink)
  {
    return find_helper< Expression, collect_action< Expression, OutputIterator& >, Traverser >(collect_action< Expression, OutputIterator& >(sink));
  }

  template < typename Expression, template < class > class Traverser, typename OutputIterator >
  find_helper< Expression, collect_action< Expression, OutputIterator >, Traverser >
  make_find_helper(OutputIterator const& sink)
  {
    return find_helper< Expression, collect_action< Expression, OutputIterator >, Traverser >(collect_action< Expression, OutputIterator >(sink));
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

      template < typename Expression >
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
  template < typename Expression, typename AdaptablePredicate, template < class, class > class SelectiveTraverser = detail::selective_data_traverser >
  class search_helper : public SelectiveTraverser< search_helper< Expression, AdaptablePredicate, SelectiveTraverser >, search_traversal_condition > {

      typedef SelectiveTraverser< search_helper< Expression, AdaptablePredicate, SelectiveTraverser >, search_traversal_condition > super;

    protected:

      AdaptablePredicate m_predicate;

    public:

      using super::enter;
      using super::leave;

#if BOOST_MSVC
      // Workaround for malfunctioning MSVC 2008 overload resolution
      template < typename Container >
      void operator()(Container const& a)
      {
        super::operator()(a);
      }
#endif

      void enter(Expression const& e)
      {
        super::m_traverse_condition = super::m_traverse_condition() && !m_predicate(e);
      }

      template < typename Container >
      bool apply(Container const& container) {
        (*this)(container);

        return !super::m_traverse_condition();
      }

      search_helper()
      { }

      search_helper(AdaptablePredicate search_predicate) : m_predicate(search_predicate)
      { }
  };

  template < typename Expression, template < class, class > class SelectiveTraverser, typename AdaptablePredicate >
  search_helper< Expression, AdaptablePredicate, SelectiveTraverser >
  make_search_helper(AdaptablePredicate search_predicate)
  {
    return search_helper< Expression, AdaptablePredicate, SelectiveTraverser >(search_predicate);
  }

  template < typename Action, template < class > class BindingAwareTraverser >
  class free_variable_find_helper : public BindingAwareTraverser< free_variable_find_helper< Action, BindingAwareTraverser > > {

     typedef BindingAwareTraverser< free_variable_find_helper< Action, BindingAwareTraverser > > super;

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
      template < typename Expression >
      void operator()(Expression const& a)
      {
        super::operator()(a);
      }

      free_variable_find_helper()
      { }

      free_variable_find_helper(Action action) : m_action(action)
      { }

      template < typename Container >
      free_variable_find_helper(Container const& bound, Action action) : super(bound), m_action(action)
      { }
  };

  template < template < class > class BindingAwareTraverser, typename OutputIterator >
  free_variable_find_helper< collect_action< variable, OutputIterator& >, BindingAwareTraverser >
  make_free_variable_find_helper(OutputIterator& sink)
  {
    return free_variable_find_helper< collect_action< variable, OutputIterator& >, BindingAwareTraverser >(
							collect_action< variable, OutputIterator& >(sink));
  }

  template < template < class > class BindingAwareTraverser, typename Container, typename OutputIterator >
  free_variable_find_helper< collect_action< variable, OutputIterator& >, BindingAwareTraverser >
  make_free_variable_find_helper(Container const& bound, OutputIterator& sink)
  {
    return free_variable_find_helper< collect_action< variable, OutputIterator& >, BindingAwareTraverser >(bound,
							collect_action< variable, OutputIterator& >(sink));
  }

  /**
   * \brief Component for searching expressions
   *
   * Types:
   *  \arg Expression the type of sub expressions that is considered
   *  \arg AdaptablePredicate represents the search test on expressions (of type Expression)
   *
   * When m_predicate(e) becomes true expression traversal will terminate.
   **/
  template < typename AdaptablePredicate, template < class, class > class SelectiveBindingAwareTraverser = detail::selective_binding_aware_traverser >
  class free_variable_search_helper : public SelectiveBindingAwareTraverser<
			 free_variable_search_helper< AdaptablePredicate >, search_traversal_condition > {

      typedef SelectiveBindingAwareTraverser<
			 free_variable_search_helper< AdaptablePredicate >, search_traversal_condition > super;

    protected:

      AdaptablePredicate m_search_predicate;

    public:

      using super::operator();
      using super::enter;

      void enter(variable const& v)
      {
        if (!super::is_bound(v))
        {
          super::m_traverse_condition = !m_search_predicate(v);
        }
      }

      void operator()(assignment const& a)
      {
        (*this)(a.rhs());
      }

#if BOOST_MSVC
      // Workaround for mal-functioning MSVC 2008 overload resolution
      template < typename Expression >
      void operator()(Expression const& a)
      {
        super::operator()(a);
      }
#endif

      template < typename Expression >
      bool apply(Expression const& container) {
        super::operator()(container);

        return !super::m_traverse_condition();
      }

      free_variable_search_helper()
      { }

      free_variable_search_helper(AdaptablePredicate search_predicate) : m_search_predicate(search_predicate)
      { }

      template < typename Container >
      free_variable_search_helper(Container const& bound,
			 AdaptablePredicate search_predicate) : super(bound), m_search_predicate(search_predicate)
      { }
  };

  template < template < class, class > class BindingAwareTraverser, typename AdaptablePredicate >
  free_variable_search_helper< AdaptablePredicate, BindingAwareTraverser >
  make_free_variable_search_helper(AdaptablePredicate search_predicate)
  {
    return free_variable_search_helper< AdaptablePredicate, BindingAwareTraverser >(search_predicate);
  }

  template < template < class, class > class BindingAwareTraverser, typename Container, typename AdaptablePredicate >
  free_variable_search_helper< AdaptablePredicate, BindingAwareTraverser >
  make_free_variable_search_helper(Container const& bound, AdaptablePredicate search_predicate)
  {
    return free_variable_search_helper< AdaptablePredicate, BindingAwareTraverser >(bound, search_predicate);
  }
}
/// \endcond

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_FIND_H
