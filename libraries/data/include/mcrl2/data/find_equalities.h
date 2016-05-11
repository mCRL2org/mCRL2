// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find_equalities.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_FIND_EQUALITIES_H
#define MCRL2_DATA_FIND_EQUALITIES_H

#include <iostream>
#include <map>
#include <sstream>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/traverser.h"
#include "mcrl2/data/detail/sorted_sequence_algorithm.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace data {

namespace detail {

struct find_equalities_expression
{
  std::map<variable, std::set<data_expression> > equalities;
  std::map<variable, std::set<data_expression> > inequalities;

  find_equalities_expression()
  {}

  find_equalities_expression(const variable& lhs, const data_expression& rhs, bool is_equality)
  {
    if (is_equality)
    {
      equalities[lhs].insert(rhs);
      if (is_variable(rhs))
      {
        equalities[lhs].insert(atermpp::down_cast<variable>(rhs));
      }
    }
    else
    {
      inequalities[lhs].insert(rhs);
      if (is_variable(rhs))
      {
        inequalities[lhs].insert(atermpp::down_cast<variable>(rhs));
      }
    }
  }

  void swap()
  {
    std::swap(equalities, inequalities);
  }

  // replace m1 by the intersection of m1 and m2
  void map_intersection(std::map<variable, std::set<data_expression> >& m1, const std::map<variable, std::set<data_expression> >& m2)
  {
    auto i1 = m1.begin();
    auto i2 = m2.begin();
    while (i1 != m1.end())
    {
      if (i2 == m2.end())
      {
        m1.erase(i1, m1.end());
        break;
      }
      if (i1->first > i2->first)
      {
        m1.erase(i1++);
      }
      else if (i2->first > i1->first)
      {
        ++i2;
      }
      else
      {
        i1->second = data::detail::set_intersection(i1->second, i2->second);
        if (i1->second.empty())
        {
          m1.erase(i1++);
        }
        else
        {
          ++i1;
        }
        ++i2;
      }
    }
  }

  // replace m1 by the union of m1 and m2
  void map_union(std::map<variable, std::set<data_expression> >& m1, const std::map<variable, std::set<data_expression> >& m2)
  {
    for (auto i = m2.begin(); i != m2.end(); ++i)
    {
      m1[i->first].insert(i->second.begin(), i->second.end());
    }
  }

  void join_and(const find_equalities_expression& other)
  {
    map_union(equalities, other.equalities);
    map_intersection(inequalities, other.inequalities);
  }

  void join_or(const find_equalities_expression& other)
  {
    map_intersection(equalities, other.equalities);
    map_union(inequalities, other.inequalities);
  }

  void delete_(const data::variable_list& variables)
  {
    for (const variable& v: variables)
    {
      equalities.erase(v);
      inequalities.erase(v);
    }
  }

  // for each entry b = b', b' = b is added too
  void close(std::map<variable, std::set<data_expression> >& m)
  {
    for (auto i = m.begin(); i != m.end(); ++i)
    {
      const variable& v = i->first;
      std::vector<variable> W;
      for (const data_expression& e: i->second)
      {
        if (is_variable(e))
        {
          W.push_back(atermpp::down_cast<variable>(e));
        }
      }
      for (const variable& w: W)
      {
        m[w].insert(v);
      }
    }
  }

  void close()
  {
    close(equalities);
    close(inequalities);
  }
};

inline
std::ostream& operator<<(std::ostream& out, const find_equalities_expression& x)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (auto i = x.equalities.begin(); i != x.equalities.end(); ++i)
  {
    for (auto j = i->second.begin(); j != i->second.end(); ++j)
    {
      result.push_back(equal_to(i->first, *j));
    }
  }
  for (auto i = x.inequalities.begin(); i != x.inequalities.end(); ++i)
  {
    for (auto j = i->second.begin(); j != i->second.end(); ++j)
    {
      result.push_back(not_equal_to(i->first, *j));
    }
  }
  out << print_set(result);
  return out;
}

template <template <class> class Traverser, class Derived>
struct find_equalities_traverser: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::vector<find_equalities_expression> expression_stack;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const find_equalities_expression& x)
  {
    expression_stack.push_back(x);
  }

  find_equalities_expression& top()
  {
    assert(!expression_stack.empty());
    return expression_stack.back();
  }

  const find_equalities_expression& top() const
  {
    assert(!expression_stack.empty());
    return expression_stack.back();
  }

  find_equalities_expression& below_top()
  {
    assert(expression_stack.size() >= 2);
    return expression_stack[expression_stack.size() - 2];
  }

  const find_equalities_expression& below_top() const
  {
    assert(expression_stack.size() >= 2);
    return expression_stack[expression_stack.size() - 2];
  }

  find_equalities_expression pop()
  {
    assert(!expression_stack.empty());
    find_equalities_expression result = top();
    expression_stack.pop_back();
    return result;
  }

  void apply(const data::application& x)
  {
    if (data::is_equal_to_application(x))
    {
      auto const& left = binary_left(x);
      auto const& right = binary_right(x);
      if (is_variable(left) && !search_free_variable(right, atermpp::down_cast<variable>(left)))
      {
        push(find_equalities_expression(atermpp::down_cast<variable>(left), right, true));
      }
      else if (is_variable(right) && !search_free_variable(left, atermpp::down_cast<variable>(right)))
      {
        push(find_equalities_expression(atermpp::down_cast<variable>(right), left, true));
      }
      else
      {
        push(find_equalities_expression());
      }
    }
    else if (data::is_not_equal_to_application(x))
    {
      auto const& left = binary_left(x);
      auto const& right = binary_right(x);
      if (is_variable(left) && !search_free_variable(atermpp::down_cast<variable>(right), atermpp::down_cast<variable>(left)))
      {
        push(find_equalities_expression(atermpp::down_cast<variable>(left), right, false));
      }
      else if (is_variable(right) && !search_free_variable(left, atermpp::down_cast<variable>(right)))
      {
        push(find_equalities_expression(atermpp::down_cast<variable>(right), left, false));
      }
      else
      {
        push(find_equalities_expression());
      }
    }
    else if (sort_bool::is_not_application(x))
    {
      derived().apply(sort_bool::arg(x));
      top().swap();
    }
    else if (sort_bool::is_and_application(x))
    {
      derived().apply(binary_left(x));
      derived().apply(binary_right(x));
      auto& left = below_top();
      auto const& right = top();
      left.join_and(right);
      pop();
    }
    else if (sort_bool::is_or_application(x))
    {
      derived().apply(binary_left(x));
      derived().apply(binary_right(x));
      auto& left = below_top();
      auto const& right = top();
      left.join_or(right);
      pop();
    }
    else if (sort_bool::is_implies_application(x))
    {
      derived().apply(binary_left(x));
      derived().apply(binary_right(x));
      auto& left = below_top();
      auto const& right = top();
      left.swap();
      left.join_or(right);
      pop();
    }
    else
    {
      mCRL2log(log::debug1) << "ignoring " << x << std::endl;
      push(find_equalities_expression());
    }
  }

  void leave(const data::variable& x)
  {
    if (sort_bool::is_bool(x.sort()))
    {
      push(find_equalities_expression(x, sort_bool::true_(), true));
    }
    else
    {
      push(find_equalities_expression());
    }
  }

  void leave(const data::abstraction& x)
  {
    top().delete_(x.variables());
  }

  void leave(const data::function_symbol&)
  {
    push(find_equalities_expression());
  }

  void leave(const data::where_clause&)
  {
    throw mcrl2::runtime_error("not implemented yet!");
  }
};

struct find_equalities_traverser_inst: public find_equalities_traverser<data::data_expression_traverser, find_equalities_traverser_inst>
{
  typedef find_equalities_traverser<data::data_expression_traverser, find_equalities_traverser_inst> super;

  using super::enter;
  using super::leave;
  using super::apply;
};

} // namespace detail

inline
std::map<variable, std::set<data_expression> > find_equalities(const data_expression& x)
{
  detail::find_equalities_traverser_inst f;
  f.apply(x);
  assert(f.expression_stack.size() == 1);
  f.top().close();
  return f.top().equalities;
}

inline
std::map<variable, std::set<data_expression> > find_inequalities(const data_expression& x)
{
  detail::find_equalities_traverser_inst f;
  f.apply(x);
  assert(f.expression_stack.size() == 1);
  f.top().close();
  return f.top().inequalities;
}

inline
std::string print_equalities(const std::map<variable, std::set<data_expression> >& equalities)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (auto i = equalities.begin(); i != equalities.end(); ++i)
  {
    for (auto j = i->second.begin(); j != i->second.end(); ++j)
    {
      result.push_back(equal_to(i->first, *j));
    }
  }
  return print_set(result);
}

inline
std::string print_inequalities(const std::map<variable, std::set<data_expression> >& inequalities)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (auto i = inequalities.begin(); i != inequalities.end(); ++i)
  {
    for (auto j = i->second.begin(); j != i->second.end(); ++j)
    {
      result.push_back(not_equal_to(i->first, *j));
    }
  }
  return print_set(result);
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FIND_EQUALITIES_H
