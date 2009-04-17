// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_expression_utility.h
/// \brief Provides utilities for working with new_data expression.

#ifndef MCRL2_NEW_DATA_DATA_EXPRESSION_STANDARD_UTILITY_H
#define MCRL2_NEW_DATA_DATA_EXPRESSION_STANDARD_UTILITY_H

#include "boost/utility/enable_if.hpp"
#include "boost/assert.hpp"
#include "boost/type_traits/is_integral.hpp"
#include "boost/type_traits/make_unsigned.hpp"
#include "boost/type_traits/is_floating_point.hpp"

#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"
#include "mcrl2/new_data/set.h"
#include "mcrl2/new_data/bag.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      // Convert to number represented as character array where each character represents a decimal digit
      inline std::vector< char > string_to_vector_number(std::string const& s) {
        std::vector< char > result;

        result.reserve(s.size());

        for (std::string::const_iterator i = s.begin(); i != s.end(); ++i) {
          BOOST_ASSERT('0' <= *i && *i <= '9');

          result.push_back(*i - '0');
        }

        return result;
      }

      /// Type T is an unsigned type
      template< typename T >
      inline std::string as_decimal_string(T t) {
        if (t != 0) {
          std::string result;

          while (0 < t) {
            result.append(1, '0' + static_cast< char >(t % 10));

            t /= 10;
          }

          return std::string(result.rbegin(), result.rend());
        }

        return "0";
      }

      /// \brief Divides a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void decimal_number_divide_by_two(std::vector< char >& number)
      {
        BOOST_ASSERT(0 < number.size());

        std::vector< char >           result(number.size(), 0);
        std::vector< char >::iterator j(result.begin());

        if (2 <= number[0]) {
          *(j++) = number[0] / 2;
        }

        for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j) {
          // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j, number[b - 1] = *(i - 1)
          *j = 5 * (*(i - 1) % 2) + *i / 2;
        }

        result.resize(j - result.begin());

        number.swap(result);
      }

      /// \brief Multiplies a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void decimal_number_multiply_by_two(std::vector< char >& number)
      {
        BOOST_ASSERT(0 < number.size());

        std::vector< char >           result(number.size() + 2, 0);
        std::vector< char >::iterator j(result.begin());

        if (5 <= number[0]) {
          *(j++) = number[0] / 5;
        }

        for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j) {
          // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j and number[b - 1] = *(i - 1)
          *j = 2 * (*(i - 1) % 5) + *i / 5;
        }

        result.resize(j - result.begin());

        number.swap(result);
      }

      /// \brief Adds one to a number in decimal notation represented by an array by two
      /// \param[in,out] s the number
      /// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
      inline void decimal_number_increment(std::vector< char >& number) {
        BOOST_ASSERT(0 < number.size());

        for (std::vector< char >::reverse_iterator i = number.rbegin(); i != number.rend(); ++i) {
          if (*i < 9) {
            ++(*i);

            return;
          }
          else {
            *i = 0;
          }
        }

        number.insert(number.begin(), 1);
      }

    } // namespace detail
    /// \endcond

    namespace sort_list {
      /// \brief Constructs a list expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application list(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression list_expression(nil(s));

        for (I i = begin; i != end; ++i) {
          BOOST_ASSERT(i->sort() == s);

          list_expression = sort_list::snoc(s, list_expression, *i);
        }

        return static_cast< application >(list_expression);
      }
    }

    namespace sort_fset {
      /// \brief Constructs a finite set expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application fset(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression fset_expression(sort_fset::fset_empty(s));

        for (I i = begin; i != end; ++i) {
          BOOST_ASSERT(i->sort() == s);

          fset_expression = sort_fset::fsetinsert(s, *i, fset_expression);
        }

        return static_cast< application >(fset_expression);
      }
    }

    namespace sort_fbag {
      /// \brief Constructs a finite bag expression from a range of expressions
      /// Type I must be a model of the Forward Traversal Iterator concept;
      /// with value_type convertible to data_expression.
      /// \param[in] s the sort of list elements
      /// \param[in] begin iterator that marks the start of a range of elements of sort s
      /// \param[in] end the past-end iterator for a range of elements of sort s
      template < typename I >
      inline
      application fbag(const sort_expression& s, I const& begin, I const& end)
      {
        data_expression fbag_expression(sort_fbag::fbag_empty(s));

        for (I i = begin; i != end; ++i) {
          BOOST_ASSERT(i->sort() == s);

          fbag_expression = sort_fbag::fbaginsert(s, *i, fbag_expression, 1);
        }

        return static_cast< application >(fbag_expression);
      }
    }

    namespace sort_bool_{
      /// \brief Constructs expression of type Bool from an integral type
      /// \param b A Boolean
      inline data_expression bool_(bool b) {
        return (b) ? sort_bool_::true_() : sort_bool_::false_();
      }
    }

    namespace sort_pos {
      /// \brief Constructs expression of type Bool from an integral type
      /// Type T is an unsigned integral type.
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      pos(const T t) {
        BOOST_ASSERT(0 < t);

        std::vector< bool > bits;
        bits.reserve(8 * sizeof(T));

        for (T u = t; 1 < u; u /= 2)
        {
          bits.push_back(u % 2 != 0);
        }

        data_expression result(sort_pos::c1());

        for (std::vector< bool >::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i) {
          result = sort_pos::cdub(sort_bool_::bool_(*i), result);
        }

        return result;
      }

      /// \brief Constructs expression of type Pos from a string
      /// \param n A string
      inline data_expression pos(std::string const& n) {
        std::vector< char > number_as_vector(detail::string_to_vector_number(n));

        std::vector< bool > bits;
        bits.reserve(number_as_vector.size());

        while (0 < number_as_vector.size() && !((number_as_vector.size() == 1) && number_as_vector[0] == 1)) { // number != 1
          bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));

          detail::decimal_number_divide_by_two(number_as_vector);
        }

        data_expression result(sort_pos::c1());

        for (std::vector< bool >::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i) {
          result = sort_pos::cdub(sort_bool_::bool_(*i), result);
        }

        return result;
      }
    }

    namespace sort_nat {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      nat(T t) {
        BOOST_ASSERT(0 <= t);
        return (t == 0) ? sort_nat::c0() : static_cast< data_expression const& >(sort_nat::cnat(sort_pos::pos(t)));
      }

      /// \brief Constructs expression of type Nat from a string
      /// \param n A string
      inline data_expression nat(std::string const& n) {
        return (n == "0") ? sort_nat::c0() : static_cast< data_expression >(sort_nat::cnat(sort_pos::pos(n)));
      }
    }

    namespace sort_int_ {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      int_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

        return (t < 0) ? sort_int_::cneg(sort_pos::pos(-t)) :
            static_cast< data_expression const& >(sort_int_::cint(sort_nat::nat(t)));
      }

      /// \brief Constructs expression of type Int from a string
      /// \param n A string
      /// \pre n is of the form ([-]?[0...9][0...9]+)([0...9]+)
      inline data_expression int_(std::string const& n) {
        return (n[0] == '-') ? sort_int_::cneg(sort_pos::pos(n.substr(1))) :
            static_cast< data_expression const& >(sort_int_::cint(sort_nat::nat(n)));
      }
    }

    namespace sort_real_ {
      /// \brief Constructs expression of type pos from an integral type
      //template < typename T >
      //inline typename boost::enable_if< typename boost::is_floating_point< T >::type, data_expression >::type
      //real_(const T) {
      // TODO;
      //}

      /// \brief Constructs expression of type pos from an integral type
      /// \param t An expression of type T
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      real_(T t) {
        return sort_real_::creal(sort_int_::int_(t), sort_pos::c1());
      }

      /// \brief Constructs expression of type Real from a string
      /// \param n A string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      inline data_expression real_(std::string const& n) {
        return sort_real_::creal(sort_int_::int_(n), sort_pos::c1());
      }
    }

    /// \brief Construct numeric expression from a string representing a number in decimal notation
    /// \param s A sort expression
    /// \param n A string
    /// \pre n is of the form [1]?[0...9]+
    inline data_expression number(sort_expression const& s, std::string const& n)
    {
      if (s == sort_pos::pos()) {
        return sort_pos::pos(n);
      }
      else if (s == sort_nat::nat()) {
        return sort_nat::nat(n);
      }
      else if (s == sort_int_::int_()) {
        return sort_int_::int_(n);
      }

      return sort_real_::real_(n);
    }

  } // namespace new_data

} // namespace mcrl2

#endif

