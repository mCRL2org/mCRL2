///////////////////////////////////////////////////////////////////////////////
/// \file lpe/sort_utility.h

#ifndef LPE_SORT_UTILITY_H
#define LPE_SORT_UTILITY_H

#include "lpe/data.h"
#include "lpe/specification.h"

namespace lpe {

/// Test is a term is a sort, and if it is equal to s
struct is_sort
{
  aterm s;

  is_sort(lpe::sort s_)
    : s(aterm_appl(s_))
  {}

  bool operator()(aterm t) const
  {
    return s == t;
  }
};

///pre: l is a list type of some sort (e.g. sort_list)
///ret: sort s occurs in l.
template <typename list_type>
bool occurs_in(list_type l, lpe::sort s)
{
  return find_if(l, is_sort(s)) != aterm();
}

///\ret the list of all functions f of sort s in fl
function_list get_constructors(const function_list& fl, const lpe::sort& s)
{
  function_list result;
  for(function_list::iterator i = fl.begin(); i != fl.end(); ++i)
  {
    if (i->result_type() == s)
    {
      result = push_front(result, *i);
    }
  }
  reverse(result);
  return result;
}

///\ret true if f has 1 or more arguments, false otherwise
bool has_arguments(const function& f)
{
  return !gsIsSortId(aterm_appl(f.argument(1)));
}

//prototype
bool is_finite(const function_list& fl, const lpe::sort& s, const lpe::sort_list visited);

///\ret true if all sorts in sl are finite, false otherwise
///Note that when a constructor sort is in visited we hold the sort as infinite because loops are created!
bool is_finite(const function_list& fl, const lpe::sort_list& sl, const lpe::sort_list visited = lpe::sort_list())
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
bool is_finite(const function_list& fl, const lpe::sort& s, const lpe::sort_list visited = lpe::sort_list())
{
  bool result = true;
  function_list cl = get_constructors(fl, s);

  //If a sort has not got any constructors it is infinite
  if (cl.size() == 0)
  {
    result = false;
  }

  //Otherwise a sort is finite if all its constructors are finite;
  //i.e. the constructors have no arguments, of their arguments are finite.
  //In the recursive call pass s add s to the visited sorts, so that we know
  //it may not occur in a constructor anymore.
  for (function_list::iterator i = cl.begin(); i != cl.end(); ++i)
  {
    result = result && (!(has_arguments(*i)) || is_finite(fl, i->input_types(), push_front(visited, s)));
  }

  return result;
}

} // namespace lpe

#endif // LPE_SORT_UTILITY_H
