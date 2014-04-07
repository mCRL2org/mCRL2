// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/data_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_UTILITY_H
#define MCRL2_DATA_DETAIL_DATA_UTILITY_H

#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include "boost/bind.hpp"
#include "boost/range/iterator_range.hpp"

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/alias.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Returns true if the names of the given variables are unique.
/// \param variables A sequence of data variables
/// \return True if the names of the given variables are unique.
inline
bool unique_names(variable_list const& variables)
{
  std::set<core::identifier_string> variable_names;
  for (variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    variable_names.insert(i->name());
  }
  if (variable_names.size() != variables.size())
  {
    return false;
  }
  return true;
}

/// \brief Returns true if the left hand sides of assignments are contained in variables.
/// \param assignments A sequence of assignments to data variables
/// \param variables A sequence of data variables
/// \return True if the left hand sides of assignments are contained in variables.
inline
bool check_assignment_variables(assignment_list const& assignments, variable_list const& variables)
{
	using utilities::detail::contains;

  std::set<variable> v(variables.begin(), variables.end());
  for (auto i = assignments.begin(); i != assignments.end(); ++i)
  {
    if (!contains(v, i->lhs()))
    {
      return false;
    }
  }
  return true;
}

/// \brief Removes elements from the set s that satisfy predicate f.
template <typename T, typename UnaryPredicate>
void set_remove_if(std::set<T>& s, UnaryPredicate f)
{
  for (auto i = s.begin(); i != s.end();)
  {
    if (f(*i))
    {
      s.erase(i++);
    }
    else
    {
      ++i;
    }
  }
}

/// \brief Returns true if the domain sorts and the codomain sort of the given sort s are contained in sorts.
/// \param s A sort expression
/// \param sorts A set of sort expressions
/// \return True if the sort is contained in <tt>sorts</tt>
template <typename SortContainer>
inline bool check_sort(sort_expression s, const SortContainer& sorts)
{
  struct local
  {
    static bool is_not_function_sort(atermpp::aterm_appl t)
    {
      return is_sort_expression(t) && !is_function_sort(t);
    }
  };

  std::set<sort_expression> s_sorts = data::find_sort_expressions(s);
  set_remove_if(s_sorts, boost::bind(&local::is_not_function_sort, _1));
  for (std::set<sort_expression>::const_iterator i = s_sorts.begin(); i != s_sorts.end(); ++i)
  {
    if (std::find(sorts.begin(), sorts.end(), *i) == sorts.end())
    {
      // sort *i is not well-typed, a system defined sort or an alias
      if (!(is_system_defined(*i)) && is_alias(*i))
      {
        alias sort_alias(*i);

        if (std::find(sorts.begin(), sorts.end(), sort_alias.name()) == sorts.end())
        {
          // sort_alias.reference() is a basic, structured or container sort
          sort_expression sort_reference(sort_alias.reference());

          if (std::find(sorts.begin(), sorts.end(), sort_reference) == sorts.end())
          {
            // sort_reference is structured or container sort
            if (is_structured_sort(sort_reference))
            {
              assert(false);
            }
            else if (is_container_sort(sort_reference))
            {
              if (std::find(sorts.begin(), sorts.end(), container_sort(sort_reference).element_sort()) == sorts.end())
              {
                return false;
              }
            }
            else
            {
              assert(false);
            }
          }
        }
      }
    }
  }

  return true;
}

/// \brief Returns true if the domain sorts and the range sort of the sorts in the sequence [first, last)
/// are contained in sorts.
/// \param first Start of a sequence of sorts
/// \param last End of a sequence of sorts
/// \param sorts A set of sort expressions
/// \return True if the sequence of sorts is contained in <tt>sorts</tt>
template <typename Iterator, typename SortContainer>
bool check_sorts(Iterator first, Iterator last, const SortContainer& sorts)
{
  for (Iterator i = first; i != last; ++i)
  {
    if (!check_sort(*i, sorts))
    {
      return false;
    }
  }
  return true;
}

/// \brief Returns true if the domain sorts and the range sort of the given variables are contained in sorts.
/// \param variables A container with data variables
/// \param sorts A set of sort expressions
/// \return True if the domain sorts and the range sort of the given variables are contained in sorts.
template <typename VariableContainer, typename SortContainer>
bool check_variable_sorts(const VariableContainer& variables, const SortContainer& sorts)
{
  for (typename VariableContainer::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
    {
      return false;
    }
  }
  return true;
}

/// \brief Returns true if names of the given variables are not contained in names.
/// \param variables A sequence of data variables
/// \param names A set of strings
/// \return True if names of the given variables are not contained in names.
inline
bool check_variable_names(variable_list const& variables, const std::set<core::identifier_string>& names)
{
	using utilities::detail::contains;

  for (auto i = variables.begin(); i != variables.end(); ++i)
  {
    if (contains(names, i->name()))
    {
      return false;
    }
  }
  return true;
}

/// \brief Returns true if the domain sorts and range sort of the given functions are contained in sorts.
/// \param range A sequence of data operations
/// \param sorts A set of sort expressions
/// \return True if the domain sorts and range sort of the given functions are contained in sorts.
template < typename Container, typename SortContainer >
inline
bool check_data_spec_sorts(const Container& container, const SortContainer& sorts)
{
  for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
    {
      return false;
    }
  }
  return true;
}

} // namespace detail

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_UTILITY_H
