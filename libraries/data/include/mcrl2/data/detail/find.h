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
