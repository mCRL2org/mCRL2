// Author(s): XIAO Qi
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

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/utilities/logger.h"

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

void print_justification_tree_rec(const boolean_equation_system& b, const vector<bool>& solution, bool init_solution, const map<boolean_variable, size_t>& index_of, int indent, boolean_expression expr, set<boolean_variable>& visited)
{
  if (is_and(expr) || is_or(expr))
  {
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, accessors::left(expr), visited);
    print_justification_tree_rec(b, solution, init_solution, index_of, indent, accessors::right(expr), visited);
  }
  else if (is_boolean_variable(expr))
  {
    boolean_variable X = atermpp::down_cast<boolean_variable>(expr);

    if (visited.count(X))
    {
      // X has been visited before. Stop here.
      cout << string(indent, ' ') << X << "*" << endl;
      return;
    }
    visited.insert(X);

    if (index_of.count(X) == 0)
    {
      // X not found in the left-hand-side of any equation. There is some
      // error in the solving algorithm.
      cout << string(indent, ' ') << X << "?" << endl;
      return;
    }
    size_t idx = index_of.at(X);
    if (idx >= solution.size())
    {
      // X is out of the range of the solution. There is some error in the
      // solving algorithm.
      cout << string(indent, ' ') << X << "?" << endl;
      return;
    }

    if (solution[idx] != init_solution)
    {
      // X is not relevant for the justification.
      return;
    }

    // Print justification rooted at X recursively.
    cout << string(indent, ' ') << X << endl;
    print_justification_tree_rec(b, solution, init_solution, index_of, indent+1, b.equations()[index_of.at(X)].formula(), visited);
  }
  else if (is_true(expr) || is_false(expr))
  {
    // do nothing
  }
  else
  {
    mCRL2log(mcrl2::log::error) << "Unexpected boolean expression " << b << endl;
    assert(0);
  }
}

/// \brief Print a justification tree generated from the full solution of a
// BES.
void print_justification_tree(const boolean_equation_system& b, const vector<bool>& solution, bool init_solution)
{
  set<boolean_variable> visited;
  map<boolean_variable, size_t> index_of;
  for (size_t i = 0; i < b.equations().size(); i++)
  {
    index_of[b.equations()[i].variable()] = i;
  }
  print_justification_tree_rec(b, solution, init_solution, index_of, 0, b.initial_state(), visited);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_JUSTIFICATION_H
