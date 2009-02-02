// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_utility1.h
/// \brief Utility functions for sorts.

#ifndef MCRL2_DATA_SORT_UTILITY1_H
#define MCRL2_DATA_SORT_UTILITY1_H

#include <vector>
#include <cassert>
#include <algorithm>
#include "mcrl2/data/data.h"

namespace mcrl2 {

namespace data {

/// Test is a term is a sort, and if it is equal to s
struct compare_sort
{
  sort_expression s;

  compare_sort(sort_expression s_)
    : s(s_)
  {}

  bool operator()(atermpp::aterm_appl t) const
  {
    return s == t;
  }
};

///\pre l is a list type of some sort (e.g. sort_expression_list)
///\return sort s occurs in l.
template <typename list_type>
bool occurs_in(list_type l, sort_expression s)
{
  return atermpp::find_if(l, compare_sort(s)) != atermpp::aterm();
}

///\return the list of all functions f of sort s in fl
inline data_operation_list get_constructors(const data_operation_list& fl, const sort_expression& s)
{
  data_operation_list result;
  for(data_operation_list::iterator i = fl.begin(); i != fl.end(); ++i)
  {
    if (result_sort(i->sort()) == s)
    {
      result = push_front(result, *i);
    }
  }
  atermpp::reverse(result);
  return result;
}

///\return true if f has 1 or more arguments, false otherwise
inline bool has_arguments(const data_operation& f)
{
  return !core::detail::gsIsSortId(atermpp::aterm_appl(f.argument(1)));
}

//prototype
bool is_finite(const data_operation_list& fl, const sort_expression& s, const mcrl2::data::sort_expression_list visited);

///\return true if all sorts in sl are finite, false otherwise
///Note that when a constructor sort is in visited we hold the sort as infinite because loops are created!
inline bool is_finite(const data_operation_list& fl, const mcrl2::data::sort_expression_list& sl, const mcrl2::data::sort_expression_list visited = mcrl2::data::sort_expression_list())
{
  bool result = true;

  // A list of sorts is finite if all sorts in the list are finite
  // If a sort is in "visited" that means that we have already seen the sort
  // during our calculation. We now get loops of the sort D = d1(E), sort E=e1(D),
  // this makes our sort infinite.
  for (sort_expression_list::iterator i = sl.begin(); i != sl.end(); ++i)
  {
    if (!occurs_in(visited, *i))
    {
      result = result && is_finite(fl, *i, visited);
    }
    else
    {
      result = false;
    }
  }
  return result;
}

///\pre fl is a list of constructors
///\return sort s is finite
inline bool is_finite(const data_operation_list& fl, const sort_expression& s, const mcrl2::data::sort_expression_list visited = mcrl2::data::sort_expression_list())
{
  bool result = true;
  data_operation_list cl = get_constructors(fl, s);

  //If a sort has not got any constructors it is infinite
  if (cl.empty())
  {
    result = false;
  }

  //Otherwise a sort is finite if all its constructors are finite;
  //i.e. the constructors have no arguments, or their arguments are finite.
  //In the recursive call pass s add s to the visited sorts, so that we know
  //it may not occur in a constructor anymore.
  for (data_operation_list::iterator i = cl.begin(); i != cl.end(); ++i)
  {
    result = result && (!(has_arguments(*i)) || is_finite(fl, domain_sorts(i->sort()), push_front(visited, s)));
  }

  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_SORT_UTILITY1_H
