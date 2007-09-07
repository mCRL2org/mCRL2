// Author(s): Jeroen Keiren, Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/sort_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_SORT_UTILITY_H
#define MCRL2_DATA_SORT_UTILITY_H

#include <cassert>
#include "mcrl2/data/data.h"
#include "mcrl2/lps/specification.h"

namespace lps {

/// Test is a term is a sort, and if it is equal to s
struct compare_sort
{
  lps::sort s;

  compare_sort(lps::sort s_)
    : s(s_)
  {}

  bool operator()(aterm_appl t) const
  {
    return s == t;
  }
};

///pre: l is a list type of some sort (e.g. sort_list)
///ret: sort s occurs in l.
template <typename list_type>
bool occurs_in(list_type l, lps::sort s)
{
  return find_if(l, compare_sort(s)) != aterm();
}

///\ret the list of all functions f of sort s in fl
inline function_list get_constructors(const function_list& fl, const lps::sort& s)
{
  function_list result;
  for(function_list::iterator i = fl.begin(); i != fl.end(); ++i)
  {
    if (i->sort().range_sort() == s)
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\ret true if f has 1 or more arguments, false otherwise
inline bool has_arguments(const function& f)
{
  return !gsIsSortId(aterm_appl(f.argument(1)));
}

//prototype
bool is_finite(const function_list& fl, const lps::sort& s, const lps::sort_list visited);

///\ret true if all sorts in sl are finite, false otherwise
///Note that when a constructor sort is in visited we hold the sort as infinite because loops are created!
inline bool is_finite(const function_list& fl, const lps::sort_list& sl, const lps::sort_list visited = lps::sort_list())
{
  bool result = true;
  
  // A list of sorts is finite if all sorts in the list are finite
  // If a sort is in "visited" that means that we have already seen the sort
  // during our calculation. We now get loops of the sort D = d1(E), sort E=e1(D),
  // this makes our sort infinite.
  for (sort_list::iterator i = sl.begin(); i != sl.end(); ++i)
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
///\ret sort s is finite
inline bool is_finite(const function_list& fl, const lps::sort& s, const lps::sort_list visited = lps::sort_list())
{
  bool result = true;
  function_list cl = get_constructors(fl, s);

  //If a sort has not got any constructors it is infinite
  if (cl.empty())
  {
    result = false;
  }

  //Otherwise a sort is finite if all its constructors are finite;
  //i.e. the constructors have no arguments, or their arguments are finite.
  //In the recursive call pass s add s to the visited sorts, so that we know
  //it may not occur in a constructor anymore.
  for (function_list::iterator i = cl.begin(); i != cl.end(); ++i)
  {
    result = result && (!(has_arguments(*i)) || is_finite(fl, i->sort().domain_sorts(), push_front(visited, s)));
  }

  return result;
}

///\pre s is a sort that occurs in data_specfication data
///\ret true iff there exists a constructor function with s as target sort
inline bool is_constructorsort(const lps::sort &s,const data_specification &data)
{ // This function is added by Jan Friso Groote on 8/7/2007.
  // cl contains all constructors with target sort s. 

  assert(occurs_in(data.sorts(),s));
  return (!data.constructors(s).empty());
}


} // namespace lps

#endif // MCRL2_DATA_SORT_UTILITY_H
