// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/standard_container_utility.h
/// \brief Provides utilities for working with data expressions of standard container sorts

#ifndef MCRL2_DATA_STANDARD_CONTAINER_UTILITY_H
#define MCRL2_DATA_STANDARD_CONTAINER_UTILITY_H

#include <type_traits>
#include <iterator>

#include "mcrl2/utilities/detail/join.h"

// Workaround for OS X with Apples patched gcc 4.0.1
#undef nil

#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2
{

namespace data
{

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
                 typename atermpp::enable_if_container< Sequence, data_expression >::type* = nullptr)
{
  data_expression                list_expression(empty(s));
  std::vector< data_expression > elements(range.begin(), range.end());

  for (std::vector< data_expression >::reverse_iterator i = elements.rbegin(); i != elements.rend(); ++i)
  {
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
                                 typename atermpp::enable_if_container< Sequence, data_expression >::type* = nullptr)
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
                                typename atermpp::enable_if_container< Sequence, data_expression >::type* = 0)
{
  if (range.empty())
  {
    return set_enumeration(sort_fset::fset(s));
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(set_enumeration(function_sort(v,sort_fset::fset(s))), range);
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
    return set_enumeration(sort_fset::fset(s));
  }
  else
  {
    sort_expression_vector v(range.size(), range.begin()->sort());

    return application(set_enumeration(function_sort(v,sort_fset::fset(s))), range);
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
                 typename atermpp::enable_if_container< Sequence, data_expression >::type* = nullptr)
{
  data_expression fset_expression(sort_fset::empty(s));

  // We process the elements in reverse order to have a resulting enumeration
  // in the same order as the input
  for (typename Sequence::const_reverse_iterator i = range.rbegin(); i != range.rend(); ++i)
  {
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
                 const data_expression_list& range)
{
  return fset(s, data_expression_vector(range.begin(),range.end()));
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
                                typename atermpp::enable_if_container< Sequence, data_expression >::type* = 0)
{
  if (range.empty())
  {
    return bag_enumeration(sort_fbag::fbag(s));
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

    return application(bag_enumeration(function_sort(v,sort_fbag::fbag(s))), range);
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
    return bag_enumeration(sort_fbag::fbag(s));
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

    return application(bag_enumeration(function_sort(v,sort_fbag::fbag(s))), range);
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
                 typename atermpp::enable_if_container< Sequence, data_expression >::type* = nullptr)
{
  data_expression fbag_expression(sort_fbag::empty(s));

  // The sequence contains element, count, ...
  // As we process the list in reverse, we have count, element, ...
  // We process the elements in reverse order to have a resulting enumeration
  // in the same order as the input
  for (typename Sequence::const_reverse_iterator i = range.rbegin(); i != range.rend(); ++i, ++i)
  {
    fbag_expression = sort_fbag::cinsert(s, *std::next(i, 1), *i, fbag_expression);
  }

  return static_cast< application >(fbag_expression);
}

/// \brief Constructs a finite bag expression from a list of expressions
/// \pre range must contain element, count, element, count, ...
/// \param[in] s the sort of list elements
/// \param[in] range a range of elements of sort s.
inline
application fbag(const sort_expression& s, const data_expression_list& range)
{
  return fbag(s, data_expression_vector(range.begin(),range.end()));
}
}

} // namespace data

} // namespace mcrl2

#endif

