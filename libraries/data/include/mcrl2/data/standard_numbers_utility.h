// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/standard_utility.h
/// \brief Provides utilities for working with data expressions of standard sorts

#ifndef MCRL2_DATA_STANDARD_NUMBERS_UTILITY_H
#define MCRL2_DATA_STANDARD_NUMBERS_UTILITY_H

#include <type_traits>

// Workaround for OS X with Apples patched gcc 4.0.1
#undef nil

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/standard_utility.h"

namespace mcrl2
{

namespace data
{

/// \cond INTERNAL_DOCS
namespace detail
{
// Convert to number represented as character array where each character
// represents a decimal digit
inline std::vector< char > string_to_vector_number(const std::string& s)
{
  assert(s.size() > 0);
  std::vector< char > result;

  result.reserve(s.size());

  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    assert('0' <= *i && *i <= '9');

    result.push_back(*i - '0');
  }

  return result;
}

// Convert from number represented as character array where each character
// represents a decimal digit
inline std::string vector_number_to_string(const std::vector< char >& v)
{
  assert(v.size() > 0);
  std::string result;

  result.reserve(v.size());

  for (std::vector< char >::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    result.push_back(*i + '0');
  }

  return result;
}

/// Type T is an unsigned type
template< typename T >
inline std::string as_decimal_string(T t)
{
  if (t != 0)
  {
    std::string result;

    while (0 < t)
    {
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
  assert(0 < number.size());

  std::vector< char >           result(number.size(), 0);
  std::vector< char >::iterator j(result.begin());

  if (2 <= number[0])
  {
    *(j++) = number[0] / 2;
  }

  for (std::vector< char >::const_iterator i = number.begin() + 1; i != number.end(); ++i, ++j)
  {
    // result[a] = 5*(number[b - 1] mod 2) + number[b] div 2   where result[a] = *j, number[b - 1] = *(i - 1)
    *j = 5 * (*(i - 1) % 2) + *i / 2;
  }

  result.resize(j - result.begin());

  number.swap(result);
}

/// \brief Multiplies a number in decimal notation represented by an array by two
/// \param[in,out] number the number
/// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
inline void decimal_number_multiply_by_two(std::vector< char >& number)
{
  assert(0 < number.size());

  std::vector< char >           result(number.size() + 2, 0);
  std::vector< char >::iterator j(result.begin());

  if (5 <= number[0])
  {
    *(j++) = number[0] / 5;
  }

  for (std::vector< char >::const_iterator i = number.begin(); i < number.end(); ++i, ++j)
  {
    // result[a] = 2*(number[b] mod 5) + number[b+1] div 5   where result[a] = *j and number[b] = *(i)
    if (i == number.end() - 1)
    {
      *j = 2 * (*i % 5);
    }
    else
    {
      *j = 2 * (*i % 5) + *(i+1) / 5;
    }
  }

  result.resize(j - result.begin());

  number.swap(result);
}

/// \brief Adds one to a number in decimal notation represented by an array
/// \param[in,out] number the number
/// A number d0 d1 ... dn is represented as s[0] s[1] ... s[n]
inline void decimal_number_increment(std::vector< char >& number)
{
  assert(0 < number.size());

  for (std::vector< char >::reverse_iterator i = number.rbegin(); i != number.rend(); ++i)
  {
    if (*i < 9)
    {
      ++(*i);

      return;
    }
    else
    {
      *i = 0;
    }
  }

  number.insert(number.begin(), 1);
}

} // namespace detail
/// \endcond

namespace sort_pos
{
/// \brief Constructs expression of type Bool from an integral type
/// Type T is an unsigned integral type.
template < typename T >
inline typename std::enable_if<std::is_integral< T >::value, data_expression>::type 
pos(const T t)
{
  assert(t>0);

  std::vector< bool > bits;
  bits.reserve(8 * sizeof(T));

  for (T u = t; 1 < u; u /= 2)
  {
    bits.push_back(u % 2 != 0);
  }

  data_expression result(sort_pos::c1());

  for (std::vector< bool >::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i)
  {
    result = sort_pos::cdub(sort_bool::bool_(*i), result);
  }

  return result;
}

/// \brief Constructs expression of type Pos from a string
/// \param n A string
inline data_expression pos(const std::string& n)
{
  std::vector< char > number_as_vector(detail::string_to_vector_number(n));

  std::vector< bool > bits;
  bits.reserve(number_as_vector.size());

  while (0 < number_as_vector.size() && !((number_as_vector.size() == 1) && number_as_vector[0] == 1))   // number != 1
  {
    bits.push_back((static_cast< int >(*number_as_vector.rbegin()) % 2 != 0));

    detail::decimal_number_divide_by_two(number_as_vector);
  }

  data_expression result(sort_pos::c1());

  for (std::vector< bool >::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i)
  {
    result = sort_pos::cdub(sort_bool::bool_(*i), result);
  }

  return result;
}

/// \brief Determines whether n is a positive constant
/// \param n A data expression
inline bool is_positive_constant(const data_expression& n)
{
  return sort_pos::is_c1_function_symbol(n) ||
         (sort_pos::is_cdub_application(n) &&
          sort_bool::is_boolean_constant(sort_pos::left(n)) &&
          sort_pos::is_positive_constant(sort_pos::right(n))
         );
}

/// \brief Return the string representation of a positive number
/// \param n_in A data expression
/// \pre is_positive_constant(n)
/// \return String representation of n
/// Transforms a positive constant n into a character array containing
/// the decimal representation of n.
inline
std::string positive_constant_as_string(const data_expression& n_in)
{
  std::vector<bool> bits;
  data_expression n=n_in;

  while (sort_pos::is_cdub_application(n))
  {
    bits.push_back(sort_bool::is_true_function_symbol(sort_pos::left(n)));
    n = sort_pos::right(n);
  }

  assert(sort_pos::is_c1_function_symbol(n));

  std::vector< char > result = detail::string_to_vector_number("1");

  for (std::vector<bool>::reverse_iterator i = bits.rbegin(); i != bits.rend(); ++i)
  {
    detail::decimal_number_multiply_by_two(result);
    if (*i)
    {
      detail::decimal_number_increment(result);
    }
  }

  return detail::vector_number_to_string(result);
}
}

namespace sort_nat
{

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
nat(T t)
{
  if (t == 0) 
  {
    return sort_nat::c0();
  }
  return sort_nat::cnat(sort_pos::pos(t));
}

/// \brief Constructs expression of type Nat from a string
/// \param n A string
inline data_expression nat(const std::string& n)
{
  return (n == "0") ? sort_nat::c0() : static_cast< data_expression const& >(sort_nat::cnat(sort_pos::pos(n)));
}

/// \brief Determines whether n is a natural constant
/// \param n A data expression
inline bool is_natural_constant(const data_expression& n)
{
  return sort_nat::is_c0_function_symbol(n) ||
         (sort_nat::is_cnat_application(n) &&
          sort_pos::is_positive_constant(sort_nat::arg(n))
         );
}

/// \brief Return the string representation of a natural number
/// \param n A data expression
/// \pre is_natural_constant(n)
/// \return String representation of n
inline std::string natural_constant_as_string(const data_expression& n)
{
  assert(is_natural_constant(n));
  if (sort_nat::is_c0_function_symbol(n))
  {
    return "0";
  }
  else
  {
    return sort_pos::positive_constant_as_string(sort_nat::arg(n));
  }
}
}

namespace sort_int
{

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value && std::is_unsigned< T >::value, data_expression >::type
int_(T t)
{
  return sort_int::cint(sort_nat::nat(t));
}

/// \brief Constructs expression of type pos from an integral type
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value && std::is_signed< T >::value, data_expression >::type
int_(T t)
{
  if (t<0)
  {
    return sort_int::cneg(sort_pos::pos(typename std::make_unsigned<T>::type(-t)));
  }
  return sort_int::cint(sort_nat::nat(typename std::make_unsigned<T>::type(t)));
}

/// \brief Constructs expression of type Int from a string
/// \param n A string
/// \pre n is of the form ([-]?[0...9][0...9]+)([0...9]+)
inline data_expression int_(const std::string& n)
{
  if (n[0] == '-')
  {
    return sort_int::cneg(sort_pos::pos(n.substr(1)));
  }
  return sort_int::cint(sort_nat::nat(n));
}

/// \brief Determines whether n is an integer constant
/// \param n A data expression
inline bool is_integer_constant(const data_expression& n)
{
  return (sort_int::is_cint_application(n) &&
          sort_nat::is_natural_constant(sort_int::arg(n))) ||
         (sort_int::is_cneg_application(n) &&
          sort_pos::is_positive_constant(sort_int::arg(n))
         );
}

/// \brief Return the string representation of an integer number
/// \param n A data expression
/// \pre is_integer_constant(n)
/// \return String representation of n
inline std::string integer_constant_as_string(const data_expression& n)
{
  assert(is_integer_constant(n));
  if (sort_int::is_cint_application(n))
  {
    return sort_nat::natural_constant_as_string(sort_int::arg(n));
  }
  else
  {
    return "-" + sort_pos::positive_constant_as_string(sort_int::arg(n));
  }
}
}

namespace sort_real
{
/// \brief Constructs expression of type pos from an integral type
/// \param t An expression of type T
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
real_(T t)
{
  return sort_real::creal(sort_int::int_(t), sort_pos::c1());
}

/// \brief Constructs expression of type pos from an integral type
/// \param numerator numerator
/// \param denominator denominator
template < typename T >
inline typename std::enable_if< std::is_integral< T >::value, data_expression >::type
real_(T numerator, T denominator)
{
  return sort_real::creal(sort_int::int_(numerator), sort_pos::pos(denominator));
}

/// \brief Constructs expression of type Real from a string
/// \param n A string
/// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
inline data_expression real_(const std::string& n)
{
  return sort_real::creal(sort_int::int_(n), sort_pos::c1());
}
}

/// \brief Construct numeric expression from a string representing a number in decimal notation
/// \param s A sort expression
/// \param n A string
/// \pre n is of the form [1]?[0...9]+
inline data_expression number(const sort_expression& s, const std::string& n)
{
  if (s == sort_pos::pos())
  {
    return sort_pos::pos(n);
  }
  else if (s == sort_nat::nat())
  {
    return sort_nat::nat(n);
  }
  else if (s == sort_int::int_())
  {
    return sort_int::int_(n);
  }

  return sort_real::real_(n);
}

/// \brief Returns true if and only if s1 == s2, or if s1 is a less specific numeric type than s2
///
/// \param[in] s1 a sort expression
/// \param[in] s2 a sort expression
inline bool is_convertible(const sort_expression& s1, const sort_expression& s2)
{
  if (s1 != s2)
  {
    if (s2 == sort_real::real_())
    {
      return s1 == sort_int::int_() || s1 == sort_nat::nat() || s1 == sort_pos::pos();
    }
    else if (s2 == sort_int::int_())
    {
      return s1 == sort_nat::nat() || s1 == sort_pos::pos();
    }
    else if (s2 == sort_nat::nat())
    {
      return s1 == sort_pos::pos();
    }

    return false;
  }

  return true;
}

} // namespace data

} // namespace mcrl2

#endif

