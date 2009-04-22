// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/detail/data_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_DETAIL_DATA_UTILITY_H
#define MCRL2_DATA_DETAIL_DATA_UTILITY_H

#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "boost/range/iterator_range.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/algorithm/string/split.hpp"

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/container_sort.h"
#include "mcrl2/new_data/assignment.h"
#include "mcrl2/new_data/alias.h"

namespace mcrl2 {

namespace new_data {

namespace detail {

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
bool check_assignment_variables(assignment_list const& assignments, variable_list variables)
{
  std::set<variable> v;
  std::copy(variables.begin(), variables.end(), std::inserter(v, v.begin()));
  for (assignment_list::const_iterator i = assignments.begin(); i != assignments.end(); ++i)
  {
    if (v.find(i->lhs()) == v.end())
      return false;
  }
  return true;
}

/// \brief Function object for checking if an operator has a sort equal to a given sort.
struct is_operation_with_given_sort
{
  atermpp::aterm_appl sort_to_compare_with;

  is_operation_with_given_sort(sort_expression u):sort_to_compare_with(u)
  // is_operation_with_given_sort(atermpp::aterm_appl u):sort_to_compare_with(u)
  {}

  /// \brief Function call operator
  /// \param t A data operation
  /// \return The function result
  bool operator()(function_symbol t) const
  { function_symbol op=t;
    return op.sort()==sort_to_compare_with;
  }
};

/// \brief Function object for checking if a term is a constant sort.
struct is_constant_sort
{
  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  bool operator()(atermpp::aterm_appl t) const
  {
    return !sort_expression(t).is_function_sort();
  }
};

/// \brief Function object for checking if an operator is a constant sort.
struct is_constant_operation
{
  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  bool operator()(atermpp::aterm_appl t) const
  { function_symbol op=t;
    return !sort_expression(op.sort()).is_function_sort();
  }
};

/// \brief Function object for checking if an operator is a not a constant sort.
struct is_not_a_constant_operation
{
  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  bool operator()(atermpp::aterm_appl t) const
  { function_symbol op=t;
    return sort_expression(op.sort()).is_function_sort();
  }
};

/// \brief Returns true if the domain sorts and the codomain sort of the given sort s are contained in sorts.
/// \param s A sort expression
/// \param sorts A set of sort expressions
/// \return True if the sort is contained in <tt>sorts</tt>
inline bool check_sort(sort_expression s, const std::set<sort_expression>& sorts)
{
  std::set<sort_expression> s_sorts;
  atermpp::find_all_if(s, is_constant_sort(), std::inserter(s_sorts, s_sorts.begin()));
  for (std::set<sort_expression>::const_iterator i = s_sorts.begin(); i != s_sorts.end(); ++i)
  {
    if (sorts.find(*i) == sorts.end()) {
      // sort *i is not well-typed, a standard sort or an alias 
      if (!(i->is_standard()) && i->is_alias()) {
        alias sort_alias(*i);

        if (sorts.find(sort_alias.name()) == sorts.end()) {
          // sort_alias.reference() is a basic, structured or container sort
          sort_expression sort_reference(sort_alias.reference());

          if (sorts.find(sort_reference) == sorts.end()) {
            // sort_reference is structured or container sort
            if (sort_reference.is_structured_sort()) {
              assert(false);
            }
            else if (sort_reference.is_container_sort()) {
              if (sorts.find(container_sort(sort_reference).element_sort()) == sorts.end()) {
                return false;
              }
            }
            else {
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
template <typename Iterator>
bool check_sorts(Iterator first, Iterator last, const std::set<sort_expression>& sorts)
{
  for (Iterator i = first; i != last; ++i)
  {
    if (!check_sort(*i, sorts))
      return false;
  }
  return true;
}

/// \brief Returns true if the domain sorts and the range sort of the given variables are contained in sorts.
/// \param variables A container with data variables
/// \param sorts A set of sort expressions
/// \return True if the domain sorts and the range sort of the given variables are contained in sorts.
template <typename VariableContainer>
bool check_variable_sorts(const VariableContainer& variables, const std::set<sort_expression>& sorts)
{
  for (typename VariableContainer::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
      return false;
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
  for (variable_list::const_iterator i = variables.begin(); i != variables.end(); ++i)
  {
    if (names.find(i->name()) != names.end())
      return false;
  }
  return true;
}

/// \brief Returns true if the domain sorts and range sort of the given functions are contained in sorts.
/// \param range A sequence of data operations
/// \param sorts A set of sort expressions
/// \return True if the domain sorts and range sort of the given functions are contained in sorts.
template < typename ForwardTraversalIterator >
inline
bool check_data_spec_sorts(boost::iterator_range< ForwardTraversalIterator > const& range, const atermpp::set<sort_expression>& sorts)
{
  for (ForwardTraversalIterator i = range.begin(); i != range.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
      return false;
  }
  return true;
}

/// \brief Returns true if the domain sorts and range sort of the given functions are contained in sorts.
/// \param functions A sequence of data operations
/// \param sorts A set of sort expressions
/// \return True if the domain sorts and range sort of the given functions are contained in sorts.
inline
bool check_data_spec_sorts(function_symbol_list const& functions, const atermpp::set<sort_expression>& sorts)
{
  return check_data_spec_sorts(boost::make_iterator_range(functions), sorts);
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
inline
data_expression_list operator+(data_expression_list l, variable_list m)
{ return data_expression_list(ATconcat(l, m)); }

/// \brief Returns the concatenation of the lists l and m.
/// \param l A sequence of data variables
/// \param m A sequence of data expressions
/// \return The concatenation of the lists l and m
inline
data_expression_list operator+(variable_list l, data_expression_list m)
{ return data_expression_list(ATconcat(l, m)); }

/// \brief Returns the concatenation of [v] and the list l.
/// \param v A data variable
/// \param l A sequence of data expressions
/// \return The concatenation of [v] and the list l.
inline
data_expression_list operator+(variable v, data_expression_list l)
{
  return data_expression(atermpp::aterm_appl(v)) + l;
}

/// \brief Returns the concatenation of the list l and [v].
/// \param l A sequence of data expressions
/// \param v A data variable
/// \return The concatenation of the list l and [v].
inline
data_expression_list operator+(data_expression_list l, variable v)
{
  return l + data_expression(atermpp::aterm_appl(v));
}

/// \brief Returns the names of the variables in t
/// \param t A sequence of data variables
/// \return The names of the variables in t
inline
std::vector<std::string> variable_strings(variable_list const& t)
{
  std::vector<std::string> result;
  for (variable_list::const_iterator i = t.begin(); i != t.end(); ++i)
    result.push_back(i->name());
  return result;
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of data expressions
/// \param m A sequence of data variables
/// \return The concatenation of the lists l and m
inline
data_expression_vector operator+(data_expression_vector const& l, variable_vector const& m)
{
  data_expression_vector result(l.begin(), l.end());

  result.insert(result.end(), m.begin(), m.end());

  return result;
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of variables
/// \param m A sequence of variables
/// \return The concatenation of the lists l and m
inline
variable_vector operator+(variable_vector const& l, variable_vector const& m)
{
  variable_vector result(l.begin(), l.end());

  result.insert(result.end(), m.begin(), m.end());

  return result;
}

/// \brief Returns the concatenation of the lists l and m
/// \param l A sequence of variables
/// \param m A sequence of variables
/// \return The concatenation of the lists l and m
inline
data_expression_vector operator+(data_expression_vector const& l, data_expression_vector const& m)
{
  data_expression_vector result(l.begin(), l.end());

  result.insert(result.end(), m.begin(), m.end());

  return result;
}

/// \brief Returns the concatenation of the lists l and m.
/// \param l A sequence of data variables
/// \param m A sequence of data expressions
/// \return The concatenation of the lists l and m
inline
data_expression_vector operator+(variable_vector const& l, data_expression_vector const& m)
{ return m + l; }

/// \brief Returns the concatenation of [v] and the list l.
/// \param v A data variable
/// \param l A sequence of data expressions
/// \return The concatenation of [v] and the list l.
inline
variable_vector operator+(variable_vector const& l, variable v)
{
  variable_vector result(l.begin(), l.end());

  result.insert(result.end(), data_expression(v));

  return result;
}

/// \brief Returns the concatenation of [v] and the list l.
/// \param v A data variable
/// \param l A sequence of data expressions
/// \return The concatenation of [v] and the list l.
inline
data_expression_vector operator+(data_expression_vector const& l, variable v)
{
  data_expression_vector result(l.begin(), l.end());

  result.insert(result.end(), data_expression(v));

  return result;
}

/// \brief Returns the concatenation of the list l and [v].
/// \param l A sequence of data expressions
/// \param v A data variable
/// \return The concatenation of the list l and [v].
inline
data_expression_vector operator+(variable v, data_expression_vector const& l)
{
  return l + v;
}

/// \brief Returns the concatenation of the list l and [v].
/// \param l A sequence of data expressions
/// \param v A data variable
/// \return The concatenation of the list l and [v].
inline
variable_vector operator+(variable v, variable_vector const& l)
{
  return l + v;
}

} // namespace detail

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_DATA_UTILITY_H
