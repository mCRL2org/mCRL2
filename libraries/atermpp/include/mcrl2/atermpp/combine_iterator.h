// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/combine_iterator.h
/// \brief Provides a combine iterator

#ifndef MCRL2_ATERMPP_COMBINE_ITERATOR_H
#define MCRL2_ATERMPP_COMBINE_ITERATOR_H

#include <algorithm>
#include <set>
#include <vector>

// FIXME: remove superfluous headers (JK: 23/11/2010)
#include "boost/assert.hpp"
#include "boost/static_assert.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_convertible.hpp"
#include "boost/call_traits.hpp"
#include "boost/type_traits/remove_reference.hpp"
#include "boost/type_traits/add_reference.hpp"
#include "boost/range/iterator_range.hpp"
#include "boost/iterator/iterator_adaptor.hpp"
#include "boost/iterator/iterator_facade.hpp"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

template < typename AdaptableUnaryFunction, typename ForwardTraversalIterator1, typename ForwardTraversalIterator2 >
class combine_iterator : public boost::iterator_facade<
  atermpp::detail::combine_iterator< AdaptableUnaryFunction, ForwardTraversalIterator1, ForwardTraversalIterator2 >,
  typename boost::remove_reference< AdaptableUnaryFunction >::type::result_type, boost::forward_traversal_tag,
  typename boost::remove_reference< AdaptableUnaryFunction >::type::result_type >
{

    friend class boost::iterator_core_access;

  private:

    AdaptableUnaryFunction     m_transformer;
    ForwardTraversalIterator1  m_iterator_1;
    ForwardTraversalIterator2  m_iterator_2;

    typename boost::remove_reference< AdaptableUnaryFunction >::type::result_type dereference() const
    {
      return m_transformer(*m_iterator_1, *m_iterator_2);
    }

    void increment()
    {
      ++m_iterator_1;
      ++m_iterator_2;
    }

    bool equal(combine_iterator const& other) const
    {
      return m_iterator_1 == other.m_iterator_1 && m_iterator_2 == other.m_iterator_2;
    }

  public:

    combine_iterator(typename boost::call_traits< AdaptableUnaryFunction >::param_type transformer,
                     ForwardTraversalIterator1 i1, ForwardTraversalIterator2 i2) : m_transformer(transformer), m_iterator_1(i1), m_iterator_2(i2)
    { }
};

template < typename AdaptableUnaryFunction, typename ForwardTraversalIterator1, typename ForwardTraversalIterator2 >
combine_iterator< typename boost::add_reference< AdaptableUnaryFunction >::type, ForwardTraversalIterator1, ForwardTraversalIterator2 >
make_combine_iterator(AdaptableUnaryFunction f, ForwardTraversalIterator1 const& i1, ForwardTraversalIterator2 const& i2)
{
  return combine_iterator< typename boost::add_reference< AdaptableUnaryFunction >::type, ForwardTraversalIterator1, ForwardTraversalIterator2 >(f, i1, i2);
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_CONTAINER_UTILITY_H
