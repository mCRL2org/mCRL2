// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/container_utility.h
/// \brief Provides utilities for working with container.

#ifndef MCRL2_DATA_DETAIL_CONTAINER_UTILITY_H
#define MCRL2_DATA_DETAIL_CONTAINER_UTILITY_H

#include <algorithm>
#include <set>
#include <vector>

#include "boost/assert.hpp"
#include "boost/static_assert.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_convertible.hpp"
#include "boost/range/iterator_range.hpp"

#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list_iterator.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      /**
       * \brief Container adapter to use random access iterators for a term_list object
       * \note Necessary for range construction with term_list_random_iterator
       **/
      template < typename Expression >
      class random_access_list;

      /// \brief Iterator for term_list.
      template <typename Expression>
      class term_list_random_iterator: public boost::iterator_facade<
              term_list_random_iterator<Expression>, // Derived
              const Expression,                      // Value
              boost::random_access_traversal_tag,    // CategoryOrTraversal
              const Expression                       // Reference
          >
      {
       public:

          /// \brief For efficient conversion of iterator ranges
          atermpp::term_list< Expression > list() const
          {
            return m_start;
          }

          /// \brief Conversion to term_list_iterator
          operator atermpp::term_list_iterator< Expression >() {
            return atermpp::term_list_iterator< Expression >(m_list);
          }

       private:
          friend class boost::iterator_core_access;

          ATermList m_start;
          ATermList m_list;

          template < typename TermList >
          friend class random_access_list;

          /// \brief Constructor for a sequence with elements of l
          /// \param l A sequence of terms
          /// \param p A sequence of terms or the empty list
          /// The empty list specifies that the object is past-the-end iterator
          /// \note decrement will not work properly with objects constructed with default l
          term_list_random_iterator(ATermList l = ATempty, ATermList p = ATempty)
            : m_start(l), m_list(p)
          {}

          /// \brief Equality operator
          /// \param other An iterator
          /// \return True if the iterators are equal
          bool equal(term_list_random_iterator const& other) const
          {
            return this->m_list == other.m_list;
          }

          /// \brief Dereference operator
          /// \return The value that the iterator references
          const Expression dereference() const
          {
            assert(!ATisEmpty(m_list));

            return Expression(typename atermpp::term_list_iterator_traits<Expression>::value_type(ATgetFirst(m_list)));
          }

          /// \brief Increments the iterator (linear)
          void decrement()
          {
            for (ATermList l = m_start; l != ATempty; l = ATgetNext(l)) {
              if (ATgetNext(l) == m_list) {
                m_list = l;

                break;
              }
            }
          }

          /// \brief Increments the iterator
          void increment()
          {
            assert(!ATisEmpty(m_list));

            m_list = ATgetNext(m_list);
          }

          /// \brief Advance by n
          void advance(size_t n)
          {
            for (; n != 0; --n) {
              increment();
            }
          }

          /// \brief Measure distance to other iterator
          ptrdiff_t distance_to(term_list_random_iterator const& other) const
          {
            ptrdiff_t count = 0;

            for (ATermList l = m_list; !ATisEmpty(l); l = ATgetNext(l), ++count) {
              if (l == other.m_list) {
                return count;
              }
            }

            count = 0;

            for (ATermList l = other.m_list; !ATisEmpty(l); l = ATgetNext(l), --count) {
              if (l == m_list) {
                break;
              }
            }

            return count;
          }
      };

      // Condition for recognising types that represent containers
      template < typename T >
      struct is_container_impl {
        typedef boost::false_type type;
      };

      template < typename T >
      struct is_container_impl< std::set< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< std::multiset< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< atermpp::set< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< atermpp::multiset< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< atermpp::vector< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< std::vector< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< atermpp::term_list< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_container_impl< boost::iterator_range< T > > {
        typedef boost::true_type type;
      };

      template < bool C, typename Container, typename Value >
      struct lazy_check_value_type {
        typedef boost::false_type type;
      };

      template < typename Container, typename ValueType >
      struct lazy_check_value_type< true, Container, ValueType > {
        typedef typename boost::is_convertible< typename Container::value_type, ValueType >::type type;
      };

      /// type condition for use with boost::enable_if
      /// T the type to be tested
      /// \pre V is void or T::value_type convertible to V
      template < typename T, typename V = void >
      struct is_container;

      /// type condition for use with boost::enable_if
      /// T is the container type
      template < typename T >
      struct is_container< T, void > : public
        is_container_impl< typename boost::remove_reference< typename boost::remove_const< T >::type >::type >
      { };

      template < typename T, typename V >
      struct is_container {
        typedef typename lazy_check_value_type< is_container< T, void >::type::value, T, V >::type type;
      };

      /// type condition for use with boost::enable_if
      /// T the type to be tested
      /// \pre V is void or T::value_type convertible to V
      template < typename T, typename V = void >
      struct enable_if_container : public
        boost::enable_if< typename detail::is_container< T, V >::type >
      {};

      /// type condition for use with boost::enable_if
      /// T the type to be tested
      /// \pre V is void or T::value_type convertible to V
      template < typename T, typename V = void >
      struct disable_if_container : public
        boost::disable_if< typename detail::is_container< T, V >::type >
      {};

      template < typename T >
      struct is_set_impl {
        typedef boost::false_type type;
      };

      template < typename T >
      struct is_set_impl< std::set< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_set_impl< std::multiset< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_set_impl< atermpp::set< T > > {
        typedef boost::true_type type;
      };

      template < typename T >
      struct is_set_impl< atermpp::multiset< T > > {
        typedef boost::true_type type;
      };

      // type condition for use with boost::enable_if
      template < typename T >
      struct is_set : public is_set_impl< typename boost::remove_reference< typename boost::remove_const< T >::type >::type >
      { };

      template < typename Expression, typename Predicate, typename OutputIterator >
      class filter_insert_iterator {

        public:

          typedef std::output_iterator_tag iterator_category;
          typedef Expression               value_type;
          typedef void                     difference_type;
          typedef void                     pointer;
          typedef void                     reference;

          struct proxy {
            Predicate      m_filter;
            OutputIterator m_sink;

            void operator=(Expression const& p) {
              if (m_filter(p)) {
                *m_sink = static_cast< typename OutputIterator::value_type >(p);
              }
            }

            proxy(Predicate const& filter, OutputIterator const& sink) : m_filter(filter), m_sink(sink) {
            }
          };

        private:

          proxy m_proxy;

        public:

          proxy& operator*() {
            return m_proxy;
          }

          filter_insert_iterator& operator++() {
            return *this;
          }

          filter_insert_iterator& operator++(int) {
            return *this;
          }

          filter_insert_iterator(Predicate const& m_filter, OutputIterator const& sink) : m_proxy(m_filter, sink) {
          }
      };

      // factory method
      // \param[in] predicate is a filter predicate
      // \parampin] filter is a filter predicate
      template < typename Container, typename Predicate >
      inline filter_insert_iterator< typename Container::value_type, Predicate, std::insert_iterator< Container > >
      make_filter_inserter(Container& c, Predicate const& filter, typename boost::enable_if< typename is_container< Container >::type >::type* = 0) {
        return filter_insert_iterator< typename Container::value_type, Predicate, std::insert_iterator< Container > >(filter, std::inserter(c, c.end()));
      }

      // factory method
      // \param[in] predicate is a filter predicate
      // \parampin] filter is a filter predicate
      template < typename Expression, typename Predicate, typename OutputIterator >
      inline filter_insert_iterator< Expression, Predicate, OutputIterator >
      make_filter_inserter(Predicate const& filter, OutputIterator& o) {
        return filter_insert_iterator< Expression, Predicate, OutputIterator >(filter, o);
      }

      template < typename Expression >
      class random_access_list {

        atermpp::aterm_list m_list;

        public:
          typedef Expression                              value_type;
          typedef term_list_random_iterator< Expression > iterator;
          typedef term_list_random_iterator< Expression > const_iterator;

          template < typename SourceExpression >
          random_access_list(atermpp::term_list< SourceExpression > const& list) : m_list(list) {
          }

          random_access_list(ATermList list) : m_list(atermpp::aterm_list(list)) {
          }

          term_list_random_iterator< Expression > begin() const {
            return term_list_random_iterator< Expression >(m_list, m_list);
          }

          term_list_random_iterator< Expression > end() const {
            return term_list_random_iterator< Expression >(m_list);
          }
      };

      template < typename Container, typename ReplaceFunction, typename OutputIterator >
      void apply(Container const& container, ReplaceFunction function, OutputIterator o, typename detail::enable_if_container< Container >::type* = 0)
      {
        for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
        {
          *o = function(*i);
        }
      }

      template < typename Expression, typename ReplaceFunction >
      typename ReplaceFunction::result_type apply_copy(Expression const& expression, ReplaceFunction function, typename detail::disable_if_container< Expression >::type* = 0)
      {
        return function(expression);
      }

      template < typename Container, typename ReplaceFunction >
      Container apply_copy(Container const& container, ReplaceFunction function, typename detail::enable_if_container< Container >::type* = 0)
      {
        Container result;

        typename std::insert_iterator< Container > o(result, result.end());

        apply(container, o);

        return result;
      }

      template < typename MutableContainer, typename UnaryFunction >
      void apply(MutableContainer& container, UnaryFunction function)
      {
        std::for_each(container.begin(), container.end(), function);
      }
    } // namespace detail

    /**
     * \brief Container adapter to use random access iterators for a term_list object
     * \note Necessary for range construction with term_list_random_iterator
     **/
    template < typename TargetExpression, typename SourceExpression >
    detail::random_access_list< TargetExpression > add_random_access(atermpp::term_list< SourceExpression > const& list)
    {
      return detail::random_access_list< TargetExpression >(list);
    }

    /**
     * \brief Container adapter to use random access iterators for a term_list object
     * \note Necessary for range construction with term_list_random_iterator
     **/
    template < typename Expression >
    detail::random_access_list< Expression > add_random_access(ATermList list)
    {
      return detail::random_access_list< Expression >(list);
    }

    /// \brief Returns the concatenation of the lists l and m
    /// \param l A sequence of data expressions
    /// \param m A sequence of data variables
    /// \return The concatenation of the lists l and m
    /// \note Candidate for moving to atermpp library
    template < typename Expression, typename CompatibleExpression >
    inline
    atermpp::term_list< Expression > operator+(atermpp::term_list< Expression > l, atermpp::term_list< CompatibleExpression > m)
    {
      BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));

      return atermpp::term_list< Expression >(ATconcat(l, m));
    }

    /// \brief Returns the concatenation of the lists l and m
    /// \param l A sequence of data expressions
    /// \param m A sequence of data variables
    /// \return The concatenation of the lists l and m
    /// \note Candidate for moving to atermpp library
    template < typename Expression, typename CompatibleExpression >
    inline
    atermpp::term_list< Expression > operator+(atermpp::term_list< Expression > l, CompatibleExpression const& m)
    {
      BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));

      return atermpp::push_back(l, static_cast< Expression >(m));
    }

    /// \brief Returns the concatenation of the lists l and m
    /// \param l A sequence of data expressions
    /// \param m A sequence of data variables
    /// \return The concatenation of the lists l and m
    /// \note Candidate for moving to atermpp library
    template < typename Expression, typename CompatibleExpression >
    inline
    atermpp::term_list< Expression > operator+(CompatibleExpression const& m, atermpp::term_list< Expression > l)
    {
      BOOST_STATIC_ASSERT((boost::is_convertible< CompatibleExpression, Expression >::value));

      return atermpp::push_front(l, static_cast< Expression >(m));
    }


    /// \brief Constructs a vector with element type T of one argument.
    ///
    /// \param[in] t1 The first element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      return v;
    }

    /// \brief Constructs a vector with element type T of two arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      return v;
    }

    /// \brief Constructs a vector with element type T of three arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      return v;
    }

    /// \brief Constructs a vector with element type T of four arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    /// \param[in] t4 The fourth element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      v.push_back(t4);
      return v;
    }

    /// \brief Constructs a vector with element type T of four arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    /// \param[in] t4 The fourth element of the vector.
    /// \param[in] t5 The fifth element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      v.push_back(t4);
      v.push_back(t5);
      return v;
    }

    /// \brief Constructs a vector with element type T of four arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    /// \param[in] t4 The fourth element of the vector.
    /// \param[in] t5 The fifth element of the vector.
    /// \param[in] t6 The sixth element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      v.push_back(t4);
      v.push_back(t5);
      v.push_back(t6);
      return v;
    }

    /// \brief Constructs a vector with element type T of four arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    /// \param[in] t4 The fourth element of the vector.
    /// \param[in] t5 The fifth element of the vector.
    /// \param[in] t6 The sixth element of the vector.
    /// \param[in] t7 The seventh element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6, const T& t7)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      v.push_back(t4);
      v.push_back(t5);
      v.push_back(t6);
      v.push_back(t7);
      return v;
    }

    /// \brief Constructs a vector with element type T of four arguments.
    ///
    /// \param[in] t1 The first element of the vector.
    /// \param[in] t2 The second element of the vector.
    /// \param[in] t3 The third element of the vector.
    /// \param[in] t4 The fourth element of the vector.
    /// \param[in] t5 The fifth element of the vector.
    /// \param[in] t6 The sixth element of the vector.
    /// \param[in] t7 The seventh element of the vector.
    /// \param[in] t8 The eighth element of the vector.
    template <typename T>
    atermpp::vector<T> make_vector(const T& t1, const T& t2, const T& t3, const T& t4, const T& t5, const T& t6, const T& t7, const T& t8)
    {
      atermpp::vector<T> v;
      v.push_back(t1);
      v.push_back(t2);
      v.push_back(t3);
      v.push_back(t4);
      v.push_back(t5);
      v.push_back(t6);
      v.push_back(t7);
      v.push_back(t8);
      return v;
    }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H
