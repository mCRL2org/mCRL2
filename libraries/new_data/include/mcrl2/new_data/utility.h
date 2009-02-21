// Author(s): Jeroen Keiren, Jeroen van der Wulp, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_NEW_DATA_UTILITY_H
#define MCRL2_NEW_DATA_UTILITY_H

#include <vector>

#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/type_traits/is_floating_point.hpp>

#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"
#include "mcrl2/new_data/real.h"
#include "mcrl2/new_data/list.h"

namespace mcrl2 {

  namespace new_data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      std::vector< char > string_to_vector_number(std::string const& s) {
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
      std::string as_decimal_string(T t) {
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
      void divide_by_two(std::vector< char >& n)
      {
        BOOST_ASSERT(0 < n.size());

        std::vector< char > result(n.size(), 0);

        std::vector< char >::const_iterator i = n.begin();
        std::vector< char >::iterator       j = result.begin();

        if (1 < *i) {
          *(j++) = *i / 2;
        }

        for (++i; i != n.end(); ++i, ++j)
        {
          //result[a] = 5*(n[b - 1] mod 2) + n[b] div 2   where result[a] = *j, n[b - 1] = *(i - 1) and n[b] = *i
          *j = 5 * (*(i - 1) % 2) + (*i / 2);
        }

        result.resize(j - result.begin());

        n.swap(result);
      }
    }
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
          assert(i->sort() == s);

          list_expression = sort_list::snoc(s, list_expression, *i);
        }

        return static_cast< application >(list_expression);
      }
    }

    namespace sort_bool_{
      /// \brief Constructs expression of type Bool from an integral type
      data_expression bool_(bool b) {
        return (b) ? sort_bool_::true_() : sort_bool_::false_();
      }
    }

    namespace sort_pos {
      /// \brief Constructs expression of type Bool from an integral type
      /// Type T is an unsigned integral type.
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type,
        typename boost::enable_if< typename boost::is_unsigned< T >::type, data_expression >::type >::type
      pos(const T t) {
        return data_expression(core::detail::gsMakeDataExprPos(
                const_cast< char* >(detail::as_decimal_string(t).c_str())));
      }

      /// \brief Constructs expression of type Pos from a string
      data_expression pos(std::string const& n) {
        using namespace mcrl2::core::detail;

        std::vector< char > number_as_vector(detail::string_to_vector_number(n));
        std::vector< bool > bits;

        bits.reserve(number_as_vector.size());

        while (number_as_vector[0] != 1 || 1 < number_as_vector.size()) { // number != 1
          bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));

          detail::divide_by_two(number_as_vector);
        }

        data_expression result(core::detail::gsMakeDataExprC1());

        for (std::vector< bool >::const_reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i) {
          result = data_expression(gsMakeDataExprCDub(((*i) ? gsMakeDataExprTrue() : gsMakeDataExprFalse()), result));
        }

        return result;
      }
    }

    namespace sort_nat {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type,
        typename boost::enable_if< typename boost::is_unsigned< T >::type, data_expression >::type >::type
      nat(T t) {
        return data_expression(core::detail::gsMakeDataExprNat(const_cast< char* >(detail::as_decimal_string(t).c_str())));
      }

      /// \brief Constructs expression of type Nat from a string
      data_expression nat(std::string const& n) {
        if (n == "0") {
          return data_expression(core::detail::gsMakeDataExprC0());
        }

        return data_expression(core::detail::gsMakeDataExprCNat(sort_pos::pos(n)));
      }
    }

    namespace sort_int_ {

      /// \brief Constructs expression of type pos from an integral type
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      int_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

        return data_expression(core::detail::gsMakeDataExprInt(const_cast< char* >((0 < t) ?
                        number.c_str() : std::string("-").append(number).c_str())));
      }

      /// \brief Constructs expression of type Int from a string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      data_expression int_(std::string const& n) {
        if (n[0] == '-') {
          return data_expression(core::detail::gsMakeDataExprCNeg(sort_pos::pos(n.substr(1))));
        }

        return data_expression(core::detail::gsMakeDataExprCInt(sort_nat::nat(n)));
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
      template < typename T >
      inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
      real_(T t) {
        std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

        return data_expression(core::detail::gsMakeDataExprReal(const_cast< char* >((0 <= t) ?
                        number.c_str() : std::string("-").append(number).c_str())));
      }

      /// \brief Constructs expression of type Real from a string
      /// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
      data_expression real_(std::string const& s) {
        return data_expression(core::detail::gsMakeDataExprCReal(sort_int_::int_(s), core::detail::gsMakeDataExprC1()));
      }
    }

    /// \brief Construct numeric expression from a string representing a number in decimal notation
    /// \pre n is of the form [0...9]+
    data_expression number(sort_expression const& s, std::string const& n) {
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

#endif //MCRL2_NEW_DATA_UTILITY_H

