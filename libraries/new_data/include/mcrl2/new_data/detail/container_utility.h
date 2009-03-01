// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/container_utility.h
/// \brief Provides utilities for working with container.

#ifndef MCRL2_NEW_DATA_DETAIL_CONTAINER_UTILITY_H
#define MCRL2_NEW_DATA_DETAIL_CONTAINER_UTILITY_H

#include <set>
#include <vector>

#include "boost/utility/enable_if.hpp"

#include "mcrl2/atermpp/vector.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/aterm_appl.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {

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
      struct is_container_impl< atermpp::set< T > > {
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

      // type condition for use with boost::enable_if
      template < typename T >
      struct is_container : public is_container_impl< typename boost::remove_reference< typename boost::remove_const< T >::type >::type >
      { };

      // Used for converting terms to expressions before inserting into a sequence
      template < typename Expression, typename OutputIterator >
      class conversion_insert_iterator {

        public:

          typedef std::output_iterator_tag iterator_category;
          typedef atermpp::aterm_appl value_type;
          typedef void                difference_type;
          typedef void                pointer;
          typedef void                reference;

          struct proxy {
            OutputIterator m_sink;

            void operator=(atermpp::aterm_appl p) {
              *m_sink = static_cast< Expression >(p);
            }

            void operator=(atermpp::aterm p) {
              *m_sink = static_cast< Expression >(p);
            }

            proxy(OutputIterator const& sink) : m_sink(sink) {
            }
          };

        private:

          proxy m_proxy;

        public:

        proxy& operator*() {
          return m_proxy;
        }

        conversion_insert_iterator& operator++() {
          return *this;
        }
        conversion_insert_iterator& operator++(int) {
          return *this;
        }

        conversion_insert_iterator(OutputIterator const& sink) : m_proxy(sink) {
        }
      };

      // factory method
      template < typename Container >
      conversion_insert_iterator< typename Container::value_type, std::insert_iterator< Container > >
      make_inserter(Container& c, typename boost::enable_if< typename is_container< Container >::type >::type* = 0) {
        return conversion_insert_iterator< typename Container::value_type, std::insert_iterator< Container > >(std::inserter(c, c.end()));
      }
    } // namespace detail


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

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_UTILITY_H
