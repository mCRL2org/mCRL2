// Author(s): Wieger Wesselink, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/find_equalities.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_FIND_EQUALITIES_H
#define MCRL2_DATA_FIND_EQUALITIES_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/find.h"

namespace mcrl2::data {

namespace detail {

struct equality_set_with_top
{
  std::map<variable, std::set<data_expression> > assignments;
  bool is_top = false;

  std::set<data_expression>& operator[](const variable& var)
  {
    return assignments[var];
  }

  /// @brief Computes intersection, modifying this set
  equality_set_with_top& intersect(const equality_set_with_top& other)
  {
    if (other.is_top)
    {
      return *this;
    }
    if (is_top)
    {
      *this = other;
      return *this;
    }

    auto i1 = assignments.begin();
    auto i2 = other.assignments.begin();
    while (i1 != assignments.end())
    {
      if (i2 == other.assignments.end())
      {
        assignments.erase(i1, assignments.end());
        break;
      }
      if (i1->first > i2->first)
      {
        assignments.erase(i1++);
      }
      else if (i2->first > i1->first)
      {
        ++i2;
      }
      else
      {
        i1->second = utilities::detail::set_intersection(i1->second, i2->second);
        if (i1->second.empty())
        {
          assignments.erase(i1++);
        }
        else
        {
          ++i1;
        }
        ++i2;
      }
    }

    return *this;
  }

  /// @brief Computes union, modifying this set
  equality_set_with_top& union_(const equality_set_with_top& other)
  {
    is_top |= other.is_top;
    for (const auto& [lhs, rhss]: other.assignments)
    {
      assignments[lhs].insert(rhss.begin(), rhss.end());
    }
    return *this;
  }

  bool non_empty_intersection(const data::variable_list& variables, const std::set<data::variable>& V) const
  {
    using utilities::detail::contains;
    return std::any_of(variables.begin(), variables.end(), [&](const variable& v) { return contains(V,v); });
  }

  bool must_delete(const data::variable_list& variables, const data::variable& lhs, std::set<data::data_expression>& rhs) const
  {
    using utilities::detail::contains;
    using utilities::detail::remove_if;
    if (contains(variables, lhs))
    {
      return true;
    }
    remove_if(rhs, [&](const data::data_expression& x) { return non_empty_intersection(variables, data::find_free_variables(x)); });
    return rhs.empty();
  }

  void delete_(const data::variable_list& variables)
  {
    using utilities::detail::remove_if;
    remove_if(assignments, [&](std::pair<const variable, std::set<data_expression> >& p) { return must_delete(variables, p.first, p.second); });
  }

  // for each entry b = b', b' = b is added too
  void close()
  {
    for (auto & assignment : assignments)
    {
      const variable& v = assignment.first;
      std::vector<variable> W;
      for (const data_expression& e: assignment.second)
      {
        if (is_variable(e))
        {
          W.push_back(atermpp::down_cast<variable>(e));
        }
      }
      for (const variable& w: W)
      {
        assignments[w].insert(v);
      }
    }
  }
};

struct find_equalities_expression
{
  equality_set_with_top equalities;
  equality_set_with_top inequalities;

  /// @brief Creates (empty,empty)
  find_equalities_expression() = default;

  /// @brief Creates ({lhs == rhs}, empty) if is_equality is true, and (empty, {lhs == rhs}) otherwise
  /// @param is_equality Indicates whether to construct an equality or inequality
  find_equalities_expression(const variable& lhs, const data_expression& rhs, bool is_equality)
  {
    if (is_equality)
    {
      equalities[lhs].insert(rhs);
      if (is_variable(rhs))
      {
        equalities[atermpp::down_cast<variable>(rhs)].insert(lhs);
      }
    }
    else
    {
      inequalities[lhs].insert(rhs);
      if (is_variable(rhs))
      {
        inequalities[atermpp::down_cast<variable>(rhs)].insert(lhs);
      }
    }
  }

  /// @brief Creates ({lhs == true}, {lhs != false})
  find_equalities_expression(const variable& lhs)
  {
    assert(lhs.sort() == sort_bool::bool_());
    equalities[lhs].insert(sort_bool::true_());
    inequalities[lhs].insert(sort_bool::false_());
  }

  /// @brief Creates (empty,top) if ineq_top is true and (top,empty) otherwise
  find_equalities_expression(bool ineq_top)
  {
    equalities.is_top = !ineq_top;
    inequalities.is_top = ineq_top;
  }

  void swap()
  {
    std::swap(equalities, inequalities);
  }

  void join_and(const find_equalities_expression& other)
  {
    equalities.union_(other.equalities);
    inequalities.intersect(other.inequalities);
  }

  void join_or(const find_equalities_expression& other)
  {
    equalities.intersect(other.equalities);
    inequalities.union_(other.inequalities);
  }

  void delete_(const data::variable_list& variables)
  {
    equalities.delete_(variables);
    inequalities.delete_(variables);
  }

  void close()
  {
    equalities.close();
    inequalities.close();
  }
};

inline
std::ostream& operator<<(std::ostream& out, const find_equalities_expression& x)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (const auto& [lhs, rhss]: x.equalities.assignments)
  {
    for (const data_expression& rhs: rhss)
    {
      result.push_back(equal_to(lhs, rhs));
    }
  }
  for (const auto& [lhs, rhss]: x.inequalities.assignments)
  {
    for (const data_expression& rhs: rhss)
    {
      result.push_back(not_equal_to(lhs, rhs));
    }
  }
  out << print_set(result);
  return out;
}

template <template <class> class Traverser, class Derived>
struct find_equalities_traverser: public Traverser<Derived>
{
  using super = Traverser<Derived>;
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

  find_equalities_expression& two_below_top()
  {
    assert(expression_stack.size() >= 3);
    return expression_stack[expression_stack.size() - 3];
  }

  const find_equalities_expression& two_below_top() const
  {
    assert(expression_stack.size() >= 3);
    return expression_stack[expression_stack.size() - 3];
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
      const data_expression& left = binary_left(x);
      const data_expression& right = binary_right(x);
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
      const data_expression& left = binary_left(x);
      const data_expression& right = binary_right(x);
      if (is_variable(left) && !search_free_variable(right, atermpp::down_cast<variable>(left)))
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
      find_equalities_expression& left = below_top();
      const find_equalities_expression& right = top();
      left.join_and(right);
      pop();
    }
    else if (sort_bool::is_or_application(x))
    {
      derived().apply(binary_left(x));
      derived().apply(binary_right(x));
      find_equalities_expression& left = below_top();
      const find_equalities_expression& right = top();
      left.join_or(right);
      pop();
    }
    else if (sort_bool::is_implies_application(x))
    {
      derived().apply(binary_left(x));
      derived().apply(binary_right(x));
      find_equalities_expression& left = below_top();
      const find_equalities_expression& right = top();
      left.swap();
      left.join_or(right);
      pop();
    }
    else if (is_if_application(x))
    {
      derived().apply(x[1]);
      derived().apply(x[2]);
      derived().apply(x[0]);
      find_equalities_expression& then = two_below_top();
      find_equalities_expression& else_ = below_top();
      const find_equalities_expression& cond = top();
      
      then.equalities.union_(cond.equalities);
      else_.equalities.union_(cond.inequalities);
      then.equalities.intersect(else_.equalities);
      
      then.inequalities.union_(cond.equalities);
      else_.inequalities.union_(cond.inequalities);
      then.inequalities.intersect(else_.inequalities);
      
      pop();
      pop();
    }
    else
    {
      mCRL2log(log::trace) << "ignoring " << x << std::endl;
      push(find_equalities_expression());
    }
  }

  void leave(const data::variable& x)
  {
    if (sort_bool::is_bool(x.sort()))
    {
      push(find_equalities_expression(x));
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

  void leave(const data::function_symbol& f)
  {
    if (sort_bool::is_true_function_symbol(f))
    {
      push(find_equalities_expression(true));
    }
    else if (sort_bool::is_false_function_symbol(f))
    {
      push(find_equalities_expression(false));
    }
    else
    {
      push(find_equalities_expression());
    }
  }

  void apply(const data::where_clause& x)
  {
    // Do not search for equalities in the whr part of a where clause, and
    // remove those equalities that contain variables bound in the where clause.
    derived().apply(x.body());
    const variable_list bound_variables(x.declarations().begin(), 
                                        x.declarations().end(), 
                                        [](const assignment_expression& a){ return atermpp::down_cast<assignment>(a).lhs();});
    top().delete_(bound_variables);
  }
};

struct find_equalities_traverser_inst: public find_equalities_traverser<data::data_expression_traverser, find_equalities_traverser_inst>
{
  using super = find_equalities_traverser<data::data_expression_traverser, find_equalities_traverser_inst>;

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
  return f.top().equalities.assignments;
}

inline
std::map<variable, std::set<data_expression> > find_inequalities(const data_expression& x)
{
  detail::find_equalities_traverser_inst f;
  f.apply(x);
  assert(f.expression_stack.size() == 1);
  f.top().close();
  return f.top().inequalities.assignments;
}

inline
std::string print_equalities(const std::map<variable, std::set<data_expression> >& equalities)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (const auto& [lhs, rhss]: equalities)
  {
    for (const data_expression& rhs: rhss)
    {
      result.push_back(equal_to(lhs, rhs));
    }
  }
  return print_set(result);
}

inline
std::string print_inequalities(const std::map<variable, std::set<data_expression> >& inequalities)
{
  using core::detail::print_set;
  std::vector<data_expression> result;
  for (const auto& [lhs, rhss]: inequalities)
  {
    for (const data_expression& rhs: rhss)
    {
      result.push_back(not_equal_to(lhs, rhs));
    }
  }
  return print_set(result);
}

} // namespace mcrl2::data



#endif // MCRL2_DATA_FIND_EQUALITIES_H
