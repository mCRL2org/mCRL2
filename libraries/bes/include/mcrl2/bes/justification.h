// Author(s): Xiao Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/justification.h
/// \brief Printing justification tree.

#ifndef MCRL2_BES_JUSTIFICATION_H
#define MCRL2_BES_JUSTIFICATION_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/utilities/logger.h"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::string;
using std::map;
using std::set;

namespace mcrl2
{

namespace bes
{

namespace detail
{

inline string get_rest_of_string_after_at_symbol(const string& s)
{
  const std::size_t i=s.find('@',1);
  if (i==std::string::npos)
  {
    return "";
  }
  return s.substr(i+1);
}

inline string get_string_until_at_symbol(const string& s)
{
  const std::size_t i=s.find('@',1);
  return s.substr(0,i);
}

}  // end namespace detail

inline void print_justification_tree_rec(
             const boolean_equation_system& b, 
             const vector<bool>& solution, 
             const bool init_solution, 
             const map<boolean_variable, std::size_t>& index_of, 
             int indent, 
             const boolean_expression& expr, 
             set<boolean_variable>& visited)
{
  if (is_and(expr))
  {
    const and_& expra=atermpp::down_cast<and_>(expr);
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, expra.left(), visited);
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, expra.right(), visited);
    return;
  }
  else if (is_or(expr))
  {
    const or_& expro=atermpp::down_cast<or_>(expr);
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, expro.left(), visited);
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, expro.right(), visited);
    return;
  }
  else if (is_true(expr))
  {
    cout << string(indent, ' ') << "True\n";
    return;
  }
  else if (is_false(expr))
  {
    cout << string(indent, ' ') << "False\n";
    return;
  }
  assert(is_boolean_variable(expr));
  {
    const boolean_variable& X = atermpp::down_cast<boolean_variable>(expr);

    const map<boolean_variable, std::size_t>::const_iterator idx = index_of.find(X);
    if (idx==index_of.end() || idx->second >= b.equations().size())
    {
      // X is out of the range of the solution. There is some error in the
      // solving algorithm.
      throw mcrl2::runtime_error("Encountered undefined propositional variable " + string(X.name()) + " in the boolean equation system. Cannot generate a counter example. ");
    }

    const std::size_t variable_index=idx->second;
    if (solution[variable_index] != init_solution)
    {
      // X is not relevant for the justification.
      return;
    }

    // Print justification rooted at X recursively.
    string X_string(X.name());
    cout << string(indent, ' ') << variable_index << ": " << detail::get_string_until_at_symbol(X_string);
    X_string=detail::get_rest_of_string_after_at_symbol(X_string);
    string divider="(";
    for( ;  !X_string.empty() ; X_string=detail::get_rest_of_string_after_at_symbol(X_string))
    {
      cout << divider << detail::get_string_until_at_symbol(X_string);
      divider=",";
    }
    if (divider!="(")
    {
      cout << ")";
    }

    if (visited.count(X))
    {
      // X has been visited before. Indicate this with a "*" and stop.
      cout << "*\n";
      return;
    }
    visited.insert(X);
    cout << "\n";
    print_justification_tree_rec(b, solution, init_solution, index_of, indent+1, b.equations()[index_of.at(X)].formula(), visited);
    return;
  }
}

/// \brief Print a justification tree generated from the full solution of a
// BES.
inline void print_justification_tree(const boolean_equation_system& b, const vector<bool>& solution, bool init_solution)
{
  assert(b.equations().size()==solution.size());
  set<boolean_variable> visited;
  map<boolean_variable, std::size_t> index_of;
  for (std::size_t i = 0; i < b.equations().size(); i++)
  {
    index_of[b.equations()[i].variable()] = i;
  }
  print_justification_tree_rec(b, solution, init_solution, index_of, 0, b.initial_state(), visited);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_JUSTIFICATION_H
