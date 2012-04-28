// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/filter_iterator.h
/// \brief Provides filter iterator for containers.

#ifndef MCRL2_ATERMPP_FILTER_ITERATOR_H
#define MCRL2_ATERMPP_FILTER_ITERATOR_H

#include <algorithm>
#include <set>
#include <vector>

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

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list_iterator.h"
#include "mcrl2/atermpp/container_utility.h"

namespace atermpp
{

/// \cond INTERNAL_DOCS
namespace detail
{

template < typename Expression, typename Predicate, typename OutputIterator >
class filter_insert_iterator
{

  public:

    typedef std::output_iterator_tag iterator_category;
    typedef Expression               value_type;
    typedef void                     difference_type;
    typedef void                     pointer;
    typedef void                     reference;

    struct proxy
    {
      Predicate      m_filter;
      OutputIterator m_sink;

      void operator=(Expression const& p)
      {
        if (m_filter(p))
        {
          *m_sink = static_cast< typename OutputIterator::value_type >(p);
        }
      }

      proxy(Predicate const& filter, OutputIterator const& sink) : m_filter(filter), m_sink(sink)
      {
      }
    };

  private:

    proxy m_proxy;

  public:

    proxy& operator*()
    {
      return m_proxy;
    }

    filter_insert_iterator& operator++()
    {
      return *this;
    }

    filter_insert_iterator& operator++(int)
    {
      return *this;
    }

    filter_insert_iterator(Predicate const& m_filter, OutputIterator const& sink) : m_proxy(m_filter, sink)
    {
    }
};

// factory method
// \param[in] predicate is a filter predicate
// \parampin] filter is a filter predicate
template < typename Container, typename Predicate >
inline filter_insert_iterator< typename Container::value_type, Predicate, std::insert_iterator< Container > >
make_filter_inserter(Container& c, Predicate const& filter, typename boost::enable_if< typename is_container< Container >::type >::type* = 0)
{
  return filter_insert_iterator< typename Container::value_type, Predicate, std::insert_iterator< Container > >(filter, std::inserter(c, c.end()));
}

// factory method
// \param[in] predicate is a filter predicate
// \parampin] filter is a filter predicate
template < typename Expression, typename Predicate, typename OutputIterator >
inline filter_insert_iterator< Expression, Predicate, OutputIterator >
make_filter_inserter(Predicate const& filter, OutputIterator& o)
{
  return filter_insert_iterator< Expression, Predicate, OutputIterator >(filter, o);
}

template < typename Iterator, typename AdaptableUnaryPredicate >
class filter_iterator : public boost::iterator_adaptor<
  atermpp::detail::filter_iterator< Iterator, AdaptableUnaryPredicate >, Iterator >
{

    friend class boost::iterator_core_access;

  protected:

    AdaptableUnaryPredicate    m_predicate;
    Iterator                   m_end;

    void increment()
    {
      if (this->base_reference() != m_end)
      {
        while (++this->base_reference() != m_end && !m_predicate(*(this->base_reference())))
          { }
      }
    }

  public:

    filter_iterator(AdaptableUnaryPredicate predicate, boost::iterator_range< Iterator > const& range) :
      filter_iterator::iterator_adaptor_(range.begin()), m_predicate(predicate), m_end(range.end())
    {}

    filter_iterator(AdaptableUnaryPredicate predicate, Iterator begin, Iterator end) :
      filter_iterator::iterator_adaptor_(begin), m_predicate(predicate), m_end(end)
    {}

    filter_iterator(AdaptableUnaryPredicate predicate, Iterator end) :
      filter_iterator::iterator_adaptor_(end), m_predicate(predicate), m_end(end)
    {}
};


template < typename AdaptableUnaryPredicate, typename Iterator >
boost::iterator_range< filter_iterator< Iterator, AdaptableUnaryPredicate > >
make_filter_iterator_range(boost::iterator_range< Iterator > const& r, AdaptableUnaryPredicate predicate)
{
  typedef filter_iterator< Iterator, AdaptableUnaryPredicate > iterator_type;

  return boost::iterator_range< iterator_type >(
           iterator_type(predicate, r), iterator_type(predicate, r.end()));
}

template < typename AdaptableUnaryPredicate, typename Container >
boost::iterator_range< filter_iterator< typename Container::const_iterator, AdaptableUnaryPredicate > >
make_filter_iterator_range(Container const& c, AdaptableUnaryPredicate predicate)
{
  typedef filter_iterator< typename Container::const_iterator, AdaptableUnaryPredicate > iterator_type;

  return boost::iterator_range< iterator_type >(
           iterator_type(predicate, c), iterator_type(predicate, c.end()));
}

} // namespace detail

} // namespace atermpp

#endif // MCRL2_ATERMPP_CONTAINER_UTILITY_H
