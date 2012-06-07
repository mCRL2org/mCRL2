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

#ifndef MCRL2_DATA_DATA_EXPRESSION_STANDARD_UTILITY_H
#define MCRL2_DATA_DATA_EXPRESSION_STANDARD_UTILITY_H

#include "boost/utility.hpp"
#include "boost/utility/enable_if.hpp"
#include "boost/assert.hpp"
#include "boost/type_traits/is_integral.hpp"
#include "boost/type_traits/make_unsigned.hpp"
#include "boost/type_traits/is_floating_point.hpp"

#include "mcrl2/utilities/detail/join.h"

// Workaround for OS X with Apples patched gcc 4.0.1
#undef nil

#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2
{

namespace data
{

/// \cond INTERNAL_DOCS
namespace detail
{
// Convert to number represented as character array where each character
// represents a decimal digit
inline std::vector< char > string_to_vector_number(std::string const& s)
{
  assert(s.size() > 0);
  std::vector< char > result;

  result.reserve(s.size());

  for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
  {
    BOOST_ASSERT('0' <= *i && *i <= '9');

    result.push_back(*i - '0');
  }

  return result;
}

// Convert from number represented as character array where each character
// represents a decimal digit
inline std::string vector_number_to_string(std::vector< char > const& v)
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
  BOOST_ASSERT(0 < number.size());

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
  BOOST_ASSERT(0 < number.size());

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

namespace sort_bool
{
/// \brief Constructs expression of type Bool from an integral type
/// \param b A Boolean
inline data_expression bool_(bool b)
{
  return (b) ? sort_bool::true_() : sort_bool::false_();
}

/// \brief Determines whether b is a Boolean constant
/// \param b A data expression
inline bool is_boolean_constant(data_expression const& b)
{
  return sort_bool::is_true_function_symbol(b) ||
         sort_bool::is_false_function_symbol(b);
}
}

namespace sort_pos
{
/// \brief Constructs expression of type Bool from an integral type
/// Type T is an unsigned integral type.
template < typename T >
inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
pos(const T t)
{
  BOOST_ASSERT(0 < t);

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
inline data_expression pos(std::string const& n)
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
inline bool is_positive_constant(data_expression const& n)
{
  return sort_pos::is_c1_function_symbol(n) ||
         (sort_pos::is_cdub_application(n) &&
          sort_bool::is_boolean_constant(sort_pos::left(n)) &&
          sort_pos::is_positive_constant(sort_pos::right(n))
         );
}

/// \brief Return the string representation of a positive number
/// \param n A data expression
/// \pre is_positive_constant(n)
/// \ret String representation of n
/// Transforms a positive constant n into a character array containing
/// the decimal representation of n.
inline
std::string positive_constant_as_string(data_expression n)
{
  std::vector<bool> bits;

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
inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
nat(T t)
{
  BOOST_ASSERT(0 <= t);
  return (t == 0) ? sort_nat::c0() : static_cast< data_expression const& >(sort_nat::cnat(sort_pos::pos(t)));
}

/// \brief Constructs expression of type Nat from a string
/// \param n A string
inline data_expression nat(std::string const& n)
{
  return (n == "0") ? sort_nat::c0() : static_cast< data_expression const& >(sort_nat::cnat(sort_pos::pos(n)));
}

/// \brief Determines whether n is a natural constant
/// \param n A data expression
inline bool is_natural_constant(data_expression const& n)
{
  return sort_nat::is_c0_function_symbol(n) ||
         (sort_nat::is_cnat_application(n) &&
          sort_pos::is_positive_constant(sort_nat::arg(n))
         );
}

/// \brief Return the string representation of a natural number
/// \param n A data expression
/// \pre is_natural_constant(n)
/// \ret String representation of n
inline std::string natural_constant_as_string(data_expression const& n)
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
inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
int_(T t)
{
  std::string number(detail::as_decimal_string< typename boost::make_unsigned< T >::type >((0 <= t) ? t : -t));

  return (t < 0) ? sort_int::cneg(sort_pos::pos(-t)) :
         static_cast< data_expression const& >(sort_int::cint(sort_nat::nat(t)));
}

/// \brief Constructs expression of type Int from a string
/// \param n A string
/// \pre n is of the form ([-]?[0...9][0...9]+)([0...9]+)
inline data_expression int_(std::string const& n)
{
  return (n[0] == '-') ? sort_int::cneg(sort_pos::pos(n.substr(1))) :
         static_cast< data_expression const& >(sort_int::cint(sort_nat::nat(n)));
}

/// \brief Determines whether n is an integer constant
/// \param n A data expression
inline bool is_integer_constant(data_expression const& n)
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
/// \ret String representation of n
inline std::string integer_constant_as_string(data_expression const& n)
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
inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
real_(T t)
{
  return sort_real::creal(sort_int::int_(t), sort_pos::c1());
}

/// \brief Constructs expression of type pos from an integral type
/// \param numerator numerator
/// \param denominator denominator
template < typename T >
inline typename boost::enable_if< typename boost::is_integral< T >::type, data_expression >::type
real_(T numerator, T denominator)
{
  return sort_real::creal(sort_int::int_(numerator), sort_pos::pos(denominator));
}

/// \brief Constructs expression of type Real from a string
/// \param n A string
/// \pre n is of the form (-[1...9][0...9]+)([0...9]+)
inline data_expression real_(std::string const& n)
{
  return sort_real::creal(sort_int::int_(n), sort_pos::c1());
}
}

/// \brief Construct numeric expression from a string representing a number in decimal notation
/// \param s A sort expression
/// \param n A string
/// \pre n is of the form [1]?[0...9]+
inline data_expression number(sort_expression const& s, std::string const& n)
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
inline bool is_convertible(sort_expression const& s1, sort_expression const& s2)
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

namespace sort_list
{
/// \brief Constructs a list expression from a range of expressions
//
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \param[in] s the sort of list elements
/// \param[in] range an iterator range of elements of sort s
template < typename Sequence >
inline
application list(const sort_expression& s,
                 Sequence const& range,
                 typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  data_expression                list_expression(nil(s));
  std::vector< data_expression > elements(range.begin(), range.end());

  for (std::vector< data_expression >::reverse_iterator i = elements.rbegin(); i != elements.rend(); ++i)
  {
    // BOOST_ASSERT(is_convertible(i->sort(), s)); This is not always true, due to type conversion.

    list_expression = sort_list::cons_(s, *i, list_expression);
  }

  return static_cast< application >(list_expression);
}

/// \brief Generate identifier list_enumeration
/// \return Identifier list_enumeration
inline
core::identifier_string const& list_enumeration_name()
{
  static core::identifier_string list_enumeration_name = core::identifier_string("@ListEnum");
  return list_enumeration_name;
}

/// \brief Constructor for function symbol list_enumeration
/// \param s A sort expression
/// \return Function symbol list_enumeration
inline
function_symbol list_enumeration(const sort_expression& s)
{
  function_symbol list_enumeration(list_enumeration_name(), s);
  return list_enumeration;
}

/// \brief Recogniser for function list_enumeration
/// \param e A data expression
/// \return true iff e is the function symbol matching list_enumeration
inline
bool is_list_enumeration_function_symbol(const atermpp::aterm_appl& e)
{
  if (is_function_symbol(e))
  {
    return function_symbol(e).name() == list_enumeration_name();
  }
  return false;
}

/// \brief Application of function symbol list_enumeration
///
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of list_enum to the data expressions in range.
template <typename Sequence>
inline
data_expression list_enumeration(const sort_expression& s,
                                 Sequence const& range,
                                 typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  if (range.empty())
  {
    return list_enumeration(s);
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(list_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Application of function symbol list_enumeration
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of list_enum to the data expressions in range.
inline
data_expression list_enumeration(const sort_expression& s, data_expression_list const& range)
{
  if (range.empty())
  {
    return list_enumeration(s);
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(list_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Recogniser for application of list_enumeration
/// \param e A data expression
/// \return true iff e is an application of function symbol
///         list_enumeration to a number of arguments
inline
bool is_list_enumeration_application(const atermpp::aterm_appl& e)
{
  if (is_application(e))
  {
    return is_list_enumeration_function_symbol(application(e).head());
  }
  return false;
}
}

namespace sort_set
{
/// \brief Generate identifier set_enumeration
/// \return Identifier set_enumeration
inline
core::identifier_string const& set_enumeration_name()
{
  static core::identifier_string set_enumeration_name = core::identifier_string("@SetEnum");
  return set_enumeration_name;
}

/// \brief Constructor for function symbol set_enumeration
/// \param s A sort expression
/// \return Function symbol set_enumeration
inline
function_symbol set_enumeration(const sort_expression& s)
{
  function_symbol set_enumeration(set_enumeration_name(), s);
  return set_enumeration;
}

/// \brief Recogniser for function set_enumeration
/// \param e A data expression
/// \return true iff e is the function symbol matching set_enumeration
inline
bool is_set_enumeration_function_symbol(const atermpp::aterm_appl& e)
{
  if (is_function_symbol(e))
  {
    return function_symbol(e).name() == set_enumeration_name();
  }
  return false;
}

/// \brief Application of function symbol set_enumeration
///
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of set_enum to the data expressions in range.
template <typename Sequence>
inline
data_expression set_enumeration(const sort_expression& s,
                                Sequence const& range,
                                typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  if (range.empty())
  {
    return set_enumeration(s);
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(set_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Application of function symbol set_enumeration
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of set_enum to the data expressions in range.
inline
data_expression set_enumeration(const sort_expression& s,
                                data_expression_list const& range)
{
  if (range.empty())
  {
    return set_enumeration(s);
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(set_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Recogniser for application of set_enumeration
/// \param e A data expression
/// \return true iff e is an application of function symbol
///         set_enumeration to a number of arguments
inline
bool is_set_enumeration_application(const atermpp::aterm_appl& e)
{
  if (is_application(e))
  {
    return is_set_enumeration_function_symbol(application(e).head());
  }
  return false;
}
}

namespace sort_fset
{

/// \brief Constructs a finite set expression from a range of expressions
//
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \param[in] s the sort of list elements
/// \param[in] range a sequence of elements
template < typename Sequence >
inline
application fset(const sort_expression& s,
                 Sequence const& range,
                 typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  data_expression fset_expression(sort_fset::empty(s));

  // We process the elements in reverse order to have a resulting enumeration
  // in the same order as the input
  for (typename Sequence::const_reverse_iterator i = range.rbegin(); i != range.rend(); ++i)
  {
    // BOOST_ASSERT(is_convertible(i->sort(), s));

    fset_expression = sort_fset::insert(s, *i, fset_expression);
  }

  return static_cast< application >(fset_expression);
}

/// \brief Constructs a finite set expression from a list of expressions
//
/// \param[in] s the sort of list elements
/// \param[in] range a sequence of elements
inline
application fset(const sort_expression& s,
                 data_expression_list const& range)
{
  return fset(s, atermpp::convert<data_expression_vector, data_expression_list>(range));
}

}

namespace sort_bag
{
/// \brief Generate identifier bag_enumeration
/// \return Identifier bag_enumeration
inline
core::identifier_string const& bag_enumeration_name()
{
  static core::identifier_string bag_enumeration_name = core::identifier_string("@BagEnum");
  return bag_enumeration_name;
}

/// \brief Constructor for function symbol bag_enumeration
/// \param s A sort expression
/// \return Function symbol bag_enumeration
inline
function_symbol bag_enumeration(const sort_expression& s)
{
  function_symbol bag_enumeration(bag_enumeration_name(), s);
  return bag_enumeration;
}

/// \brief Recogniser for function bag_enumeration
/// \param e A data expression
/// \return true iff e is the function symbol matching bag_enumeration
inline
bool is_bag_enumeration_function_symbol(const atermpp::aterm_appl& e)
{
  if (is_function_symbol(e))
  {
    return function_symbol(e).name() == bag_enumeration_name();
  }
  return false;
}

/// \brief Application of function symbol bag_enumeration
///
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of bag_enum to the data expressions in range.
template <typename Sequence>
inline
data_expression bag_enumeration(const sort_expression& s,
                                Sequence const& range,
                                typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  if (range.empty())
  {
    return bag_enumeration(s);
  }
  else
  {
    assert(range.size() % 2 == 0);
    sort_expression t(range.begin()->sort());

    sort_expression_vector v;

    for (size_t i = 0; i < range.size() / 2; ++i)
    {
      v.push_back(t);
      v.push_back(sort_nat::nat());
    }

    return application(bag_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Application of function symbol bag_enumeration
/// \param s A sort expression
/// \param range A range of data expressions
/// \return Application of bag_enum to the data expressions in range.
inline
data_expression bag_enumeration(const sort_expression& s,
                                data_expression_list const& range)
{
  if (range.empty())
  {
    return bag_enumeration(s);
  }
  else
  {
    assert(range.size() % 2 == 0);
    sort_expression t(range.begin()->sort());
    sort_expression_vector v;

    for (size_t i = 0; i < range.size() / 2; ++i)
    {
      v.push_back(t);
      v.push_back(sort_nat::nat());
    }

    return application(bag_enumeration(function_sort(v,s)), range);
  }
}

/// \brief Recogniser for application of bag_enumeration
/// \param e A data expression
/// \return true iff e is an application of function symbol
///         bag_enumeration to a number of arguments
inline
bool is_bag_enumeration_application(const atermpp::aterm_appl& e)
{
  if (is_application(e))
  {
    return is_bag_enumeration_function_symbol(application(e).head());
  }
  return false;
}

}

namespace sort_fbag
{
/// \brief Constructs a finite bag expression from a range of expressions
/// Type Sequence must be a model of the Forward Traversal Iterator concept;
/// with value_type convertible to data_expression.
/// \pre range must contain element, count, element, count, ...
/// \param[in] s the sort of list elements
/// \param[in] range a range of elements of sort s.
template < typename Sequence >
inline
application fbag(const sort_expression& s, Sequence const& range,
                 typename atermpp::detail::enable_if_container< Sequence, data_expression >::type* = 0)
{
  data_expression fbag_expression(sort_fbag::empty(s));

  // The sequence contains element, count, ...
  // As we process the list in reverse, we have count, element, ...
  // We process the elements in reverse order to have a resulting enumeration
  // in the same order as the input
  for (typename Sequence::const_reverse_iterator i = range.rbegin(); i != range.rend(); ++i, ++i)
  {
    // BOOST_ASSERT(is_convertible(boost::next(i, 1)->sort(), s));
    fbag_expression = sort_fbag::cinsert(s, *boost::next(i, 1), *i, fbag_expression);
  }

  return static_cast< application >(fbag_expression);
}

/// \brief Constructs a finite bag expression from a list of expressions
/// \pre range must contain element, count, element, count, ...
/// \param[in] s the sort of list elements
/// \param[in] range a range of elements of sort s.
inline
application fbag(const sort_expression& s, data_expression_list const& range)
{
  return fbag(s, atermpp::convert<data_expression_vector, data_expression_list>(range));
}
}

/// \brief Returns true if the term t is equal to nil
inline bool is_nil(atermpp::aterm_appl t)
{
  return t == core::detail::gsMakeNil();
}

/// \brief Returns true iff the expression represents a standard sort.
/// \param[in] s a sort expression.
inline
bool
is_system_defined(const sort_expression& s)
{
  return sort_bool::is_bool(s) || sort_real::is_real(s)
         || sort_int::is_int(s) || sort_nat::is_nat(s) || sort_pos::is_pos(s)
         || is_container_sort(s) || is_structured_sort(s);
}

/** \brief A collection of utilities for lazy expression construction
 *
 * The basic idea is to keep expressions that result from application of
 * any of the container operations by applying the usual rules of logic.
 *
 * For example and(true, x) as in `and' applied to `true' and `x' yields x.
 **/
namespace lazy
{
/// \brief Returns an expression equivalent to not p
/// \param p A data expression
/// \return The value <tt>!p</tt>
inline data_expression not_(data_expression const& p)
{
  if (p == sort_bool::true_())
  {
    return sort_bool::false_();
  }
  else if (p == sort_bool::false_())
  {
    return sort_bool::true_();
  }

  return sort_bool::not_(p);
}

/// \brief Returns an expression equivalent to p and q
/// \param p A data expression
/// \param q A data expression
/// \return The value <tt>p && q</tt>
inline data_expression or_(data_expression const& p, data_expression const& q)
{
  if ((p == sort_bool::true_()) || (q == sort_bool::true_()))
  {
    return sort_bool::true_();
  }
  else if ((p == q) || (p == sort_bool::false_()))
  {
    return q;
  }
  else if (q == sort_bool::false_())
  {
    return p;
  }

  return sort_bool::or_(p, q);
}

/// \brief Returns an expression equivalent to p or q
/// \param p A data expression
/// \param q A data expression
/// \return The value p || q
inline data_expression and_(data_expression const& p, data_expression const& q)
{
  if ((p == sort_bool::false_()) || (q == sort_bool::false_()))
  {
    return sort_bool::false_();
  }
  else if ((p == q) || (p == sort_bool::true_()))
  {
    return q;
  }
  else if (q == sort_bool::true_())
  {
    return p;
  }

  return sort_bool::and_(p, q);
}

/// \brief Returns an expression equivalent to p implies q
/// \param p A data expression
/// \param q A data expression
/// \return The value p || q
inline data_expression implies(data_expression const& p, data_expression const& q)
{
  if ((p == sort_bool::false_()) || (q == sort_bool::true_()) || (p == q))
  {
    return sort_bool::true_();
  }
  else if (p == sort_bool::true_())
  {
    return q;
  }
  else if (q == sort_bool::false_())
  {
    return sort_bool::not_(p);
  }

  return sort_bool::implies(p, q);
}

/// \brief Returns an expression equivalent to p == q
/// \param p A data expression
/// \param q A data expression
/// \return The value p == q
inline data_expression equal_to(data_expression const& p, data_expression const& q)
{
  if (p == q)
  {
    return sort_bool::true_();
  }

  return data::equal_to(p, q);
}

/// \brief Returns an expression equivalent to p == q
/// \param p A data expression
/// \param q A data expression
/// \return The value ! p == q
inline data_expression not_equal_to(data_expression const& p, data_expression const& q)
{
  if (p == q)
  {
    return sort_bool::false_();
  }

  return data::not_equal_to(p, q);
}

/// \brief Returns or applied to the sequence of data expressions [first, last)
/// \param first Start of a sequence of data expressions
/// \param last End of a sequence of data expressions
/// \return Or applied to the sequence of data expressions [first, last)
template < typename ForwardTraversalIterator >
inline data_expression join_or(ForwardTraversalIterator first, ForwardTraversalIterator last)
{
  return utilities::detail::join(first, last, lazy::or_, static_cast< data_expression const& >(sort_bool::false_()));
}

/// \brief Returns and applied to the sequence of data expressions [first, last)
/// \param first Start of a sequence of data expressions
/// \param last End of a sequence of data expressions
/// \return And applied to the sequence of data expressions [first, last)
template < typename ForwardTraversalIterator >
inline data_expression join_and(ForwardTraversalIterator first, ForwardTraversalIterator last)
{
  return utilities::detail::join(first, last, lazy::and_, static_cast< data_expression const& >(sort_bool::true_()));
}
}


} // namespace data

} // namespace mcrl2

#endif

