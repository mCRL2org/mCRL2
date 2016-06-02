// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/experimental/type_check_tree.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_EXPERIMENTAL_TYPE_CHECK_TREE_H
#define MCRL2_DATA_EXPERIMENTAL_TYPE_CHECK_TREE_H

#include <iostream>
#include <memory>
#include <sstream>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/type_checker.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/untyped_sort_variable.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace data {

typedef std::map<untyped_sort_variable, sort_expression> sort_substitution;
typedef std::pair<sort_substitution, int> solution; // the second element is the cost of the solution

inline
const untyped_sort_variable& make_untyped_sort_variable(const sort_expression& x)
{
  return atermpp::down_cast<untyped_sort_variable>(x);
}

inline sort_expression substitute(const sort_expression& x, const sort_substitution& sigma)
{
  if (is_untyped_sort_variable(x))
  {
    const untyped_sort_variable& v = make_untyped_sort_variable(x);
    auto i = sigma.find(v);
    if (i != sigma.end())
    {
      return i->second;
    }
  }
  return x;
}

template <typename Container>
std::string print_node_vector(const std::string& name, const Container& nodes, const std::string& sep = ", ", const std::string& first = "", const std::string& last = "")
{
  std::vector<std::string> s;
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
  {
    s.push_back((*i)->print());
  }
  std::ostringstream out;
  out << name << "(" << first;
  out << utilities::string_join(s, sep);
  out << last << ")";
  return out.str();
}

template <typename Container>
std::string print_vector(const std::string& name, const Container& nodes)
{
  std::ostringstream out;
  out << name << "(";
  for (auto i = nodes.begin(); i != nodes.end(); ++i)
  {
    if (i != nodes.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  out << ")";
  return out.str();
}

struct type_check_node;

typedef std::shared_ptr<type_check_node> type_check_node_ptr;

struct type_check_context
{
  std::map<core::identifier_string, sort_expression_list> system_constants;
  std::map<core::identifier_string, function_sort_list> system_functions;
  std::map<core::identifier_string, sort_expression> user_constants;
  std::map<core::identifier_string, function_sort_list> user_functions;
  std::map<core::identifier_string, std::vector<sort_expression> > declared_variables;
  std::size_t sort_variable_index;

  type_check_context(const data::data_specification& dataspec = data::data_specification())
    : sort_variable_index(0)
  {
    type_checker checker(dataspec);
    system_constants = checker.system_constants();
    system_functions = checker.system_functions();
    user_constants = checker.user_constants();
    user_functions = checker.user_functions();
  }

  // Returns the system defined constants and the user defined constants matching with name
  std::pair<sort_expression_list, sort_expression_list> find_matching_constants(const std::string& name) const;

  // Returns the system defined functions and the user defined functions matching with (name, arity)
  std::pair<function_sort_list, function_sort_list> find_matching_functions(const std::string& name, std::size_t arity) const;

  // Returns the system defined functions and the user defined functions matching with name
  std::pair<function_sort_list, function_sort_list> find_matching_functions(const std::string& name) const;

  // Returns the variables matching with name
  std::vector<sort_expression> find_matching_variables(const std::string& name) const;

  untyped_sort_variable create_sort_variable()
  {
    return untyped_sort_variable(sort_variable_index++);
  }

  void add_context_variable(const variable& v)
  {
    declared_variables[v.name()].push_back(v.sort());
  }

  void add_context_variables(const variable_list& variables)
  {
    for (const variable& v: variables)
    {
      add_context_variable(v);
    }
  }

  void remove_context_variable(const variable& v)
  {
    auto i = declared_variables.find(v.name());
    if (i->second.size() <= 1)
    {
      declared_variables.erase(i);
    }
    else
    {
      i->second.pop_back();
    }
  }

  void remove_context_variables(const variable_list& variables)
  {
    for (const variable& v: variables) // N.B. the order of insertion and removal should not matter
    {
      remove_context_variable(v);
    }
  }
};

struct type_check_constraint;
typedef std::shared_ptr<type_check_constraint> constraint_ptr;
constraint_ptr substitute_constraint(constraint_ptr p, const sort_substitution& sigma);

struct type_check_constraint
{
  int cost;

  type_check_constraint(int cost_ = 0)
    : cost(cost_)
  {}

  virtual std::string print() const = 0;
};

constraint_ptr make_and_constraint(const std::vector<constraint_ptr>& alternatives);
constraint_ptr make_or_constraint(const std::vector<constraint_ptr>& alternatives);
constraint_ptr make_is_equal_to_constraint(const sort_expression& s1, const sort_expression& s2, int cost = 0);

struct true_constraint: public type_check_constraint
{
  true_constraint(int cost = 0)
    : type_check_constraint(cost)
  {}

  std::string print() const
  {
    return "true_constraint()";
  }
};

struct false_constraint: public type_check_constraint
{
  std::string message;

  false_constraint(const std::string& message_)
    : message(message_)
  {}

  std::string print() const
  {
    return "false(" + message + ")";
  }
};

inline
constraint_ptr make_false_constraint(const std::string& message)
{
  return constraint_ptr(new false_constraint(message));
}

inline
constraint_ptr make_true_constraint(int cost = 0)
{
  return constraint_ptr(new true_constraint(cost));
}

// The sort of the corresponding data expression should be equal to 'sort'.
struct is_element_of_constraint: public type_check_constraint
{
  untyped_sort_variable s;
  std::vector<sort_expression> sorts;

  is_element_of_constraint(const untyped_sort_variable& s_, const std::vector<sort_expression>& sorts_, int cost = 0)
    : type_check_constraint(cost), s(s_), sorts(sorts_)
  {}

  std::string print() const
  {
    return "is_element_of(" + data::pp(s) + ", " + core::detail::print_set(sorts) + ")";
  }
};

inline
constraint_ptr make_function_sort_constraint(const function_sort& f1, const sort_expression& s2)
{
  auto const& domain1 = f1.domain();
  if (!is_function_sort(s2))
  {
    throw mcrl2::runtime_error("could not make function sort constraint");
  }
  const function_sort& f2 = atermpp::down_cast<function_sort>(s2);
  auto const& domain2 = f2.domain();
  if (domain1.size() != domain1.size())
  {
    return make_false_constraint("function sorts do not match");
  }
  std::vector<constraint_ptr> alternatives;
  alternatives.push_back(make_is_equal_to_constraint(f1.codomain(), f2.codomain()));
  auto i1 = domain1.begin();
  auto i2 = domain2.begin();
  for (; i1 != domain1.end(); ++i1, ++i2)
  {
    alternatives.push_back(make_is_equal_to_constraint(*i1, *i2));
  }
  return make_and_constraint(alternatives);
}

inline
constraint_ptr make_is_element_of_constraint(const sort_expression& s, const std::vector<sort_expression>& sorts, int cost = 0)
{
  // optimizations
  if (is_untyped_sort(s))
  {
    return make_true_constraint();
  }
  if (std::find(sorts.begin(), sorts.end(), untyped_sort()) != sorts.end())
  {
    return make_true_constraint();
  }
  if (is_function_sort(s))
  {
    const function_sort& f1 = atermpp::down_cast<function_sort>(s);
    std::vector<constraint_ptr> alternatives;
    for (const sort_expression& s2: sorts)
    {
      alternatives.push_back(make_function_sort_constraint(f1, s2));
    }
    return make_and_constraint(alternatives);
  }
  if (std::find(sorts.begin(), sorts.end(), s) != sorts.end())
  {
    return make_true_constraint();
  }
  return constraint_ptr(new is_element_of_constraint(make_untyped_sort_variable(s), sorts, cost));
}

// The sort of the corresponding data expression should be equal to 'sort'.
struct is_equal_to_constraint: public type_check_constraint
{
  untyped_sort_variable s1;
  sort_expression s2;

  is_equal_to_constraint(const untyped_sort_variable& s1_, const sort_expression& s2_, int cost = 0)
    : type_check_constraint(cost), s1(s1_), s2(s2_)
  {}

  std::string print() const
  {
    return "is_equal_to(" + data::pp(s1) + ", " + data::pp(s2) + ")";
  }
};

inline
constraint_ptr make_is_equal_to_constraint(const sort_expression& s1, const sort_expression& s2, int cost)
{
  if (s1 == s2)
  {
    return make_true_constraint();
  }
  if (is_untyped_sort(s1) || is_untyped_sort(s2))
  {
    return make_true_constraint();
  }
  if (is_untyped_sort_variable(s1))
  {
    return make_is_element_of_constraint(s1, { s2 });
  }
  if (is_untyped_sort_variable(s2))
  {
    return make_is_element_of_constraint(s2, { s1 });
    // return constraint_ptr(new is_equal_to_constraint(make_untyped_sort_variable(s2), s1, cost));
  }
  throw mcrl2::runtime_error("cannot make is_equal_to constraint");
}

// The sort variable s1 should be a subsort of s2.
struct subsort_constraint: public type_check_constraint
{
  sort_expression s1;
  sort_expression s2;

  subsort_constraint(const sort_expression& s1_, const sort_expression& s2_, int cost = 0)
    : type_check_constraint(cost), s1(s1_), s2(s2_)
  {}

  std::string print() const
  {
    return "subsort(" + data::pp(s1) + ", " + data::pp(s2) + ")";
  }
};

inline
constraint_ptr make_subsort_constraint(const sort_expression& s1, const sort_expression& s2, int cost = 0)
{
  // optimizations
  if (sort_bool::is_bool(s1) || sort_bool::is_bool(s2))
  {
    return make_is_equal_to_constraint(s1, s2, cost);
  }
  if (is_untyped_sort(s1) || is_untyped_sort(s2))
  {
    return make_true_constraint(cost);
  }
  if (is_container_sort(s1) && is_container_sort(s2))
  {
    const container_sort& c1 = atermpp::down_cast<container_sort>(s1);
    const container_sort& c2 = atermpp::down_cast<container_sort>(s2);
    if (c1.container_name() == c2.container_name())
    {
      return make_subsort_constraint(c1.element_sort(), c2.element_sort());
    }
    else
    {
      return make_false_constraint("incompatible container sorts");
    }
  }
  if (is_container_sort(s1) && is_untyped_sort_variable(s2))
  {
    return make_is_equal_to_constraint(s1, s2);
  }
  if (is_untyped_sort_variable(s1) && is_container_sort(s2))
  {
    return make_is_equal_to_constraint(s1, s2);
  }
  if (is_function_sort(s1) && is_function_sort(s2))
  {
    const function_sort& f1 = atermpp::down_cast<function_sort>(s1);
    const function_sort& f2 = atermpp::down_cast<function_sort>(s2);
    auto const& domain1 = f1.domain();
    auto const& domain2 = f2.domain();

    if (domain1.size() != domain1.size())
    {
      return make_false_constraint("function sorts do not match");
    }

    std::vector<constraint_ptr> alternatives;
    alternatives.push_back(make_subsort_constraint(f1.codomain(), f2.codomain()));
    auto i1 = domain1.begin();
    auto i2 = domain2.begin();
    for (; i1 != domain1.end(); ++i1, ++i2)
    {
      alternatives.push_back(make_subsort_constraint(*i2, *i1));
    }
    return make_and_constraint(alternatives);
  }
  if (sort_pos::is_pos(s1))
  {
    return make_is_element_of_constraint(s2, { sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  }
  if (sort_nat::is_nat(s1))
  {
    return make_is_element_of_constraint(s2, { sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  }
  if (sort_int::is_int(s1))
  {
    return make_is_element_of_constraint(s2, { sort_int::int_(), sort_real::real_() });
  }
  if (sort_real::is_real(s1))
  {
    return make_is_equal_to_constraint(s2, sort_real::real_());
  }
  if (sort_pos::is_pos(s2))
  {
    return make_is_equal_to_constraint(s1, sort_pos::pos());
  }
  if (sort_nat::is_nat(s2))
  {
    return make_is_element_of_constraint(s1, { sort_pos::pos(), sort_nat::nat() });
  }
  if (sort_int::is_int(s2))
  {
    return make_is_element_of_constraint(s1, { sort_pos::pos(), sort_nat::nat(), sort_int::int_() });
  }
  if (sort_real::is_real(s2))
  {
    return make_is_element_of_constraint(s1, { sort_pos::pos(), sort_nat::nat(), sort_int::int_(), sort_real::real_() });
  }

  return constraint_ptr(new subsort_constraint(s1, s2, cost));
}

struct or_constraint: public type_check_constraint
{
  std::vector<constraint_ptr> alternatives;

  or_constraint(const std::vector<constraint_ptr>& alternatives_)
    : alternatives(alternatives_)
  {}

  std::string print() const
  {
    return print_node_vector("or", alternatives, "\n  ", "\n  ", "\n  ");
  }
};

inline
constraint_ptr make_or_constraint(const std::vector<constraint_ptr>& alternatives)
{
  std::vector<constraint_ptr> v;
  for (constraint_ptr p: alternatives)
  {
    true_constraint* x_no = dynamic_cast<true_constraint*>(p.get());
    if (x_no)
    {
      continue;
    }
    false_constraint* x_false = dynamic_cast<false_constraint*>(p.get());
    if (x_false)
    {
      continue;
    }
    or_constraint* x_or = dynamic_cast<or_constraint*>(p.get());
    if (x_or)
    {
      v.insert(v.end(), x_or->alternatives.begin(), x_or->alternatives.end());
      continue;
    }
    v.push_back(p);
  }
  if (v.size() == 0)
  {
    return constraint_ptr(new true_constraint());
  }
  if (v.size() == 1)
  {
    return v.front();
  }
  return constraint_ptr(new or_constraint(v));
}

inline
std::vector<constraint_ptr> join_is_element_of_constraints(const std::vector<constraint_ptr>& constraints)
{
  std::vector<constraint_ptr> result;
  std::map<untyped_sort_variable, std::set<sort_expression> > is_element_of_constraints;

  for (constraint_ptr p: constraints)
  {
    is_element_of_constraint* x_is_element_of = dynamic_cast<is_element_of_constraint*>(p.get());
    if (x_is_element_of)
    {
      is_element_of_constraints[x_is_element_of->s].insert(x_is_element_of->sorts.begin(), x_is_element_of->sorts.end());
    }
    else
    {
      result.push_back(p);
    }
  }
  for (auto i = is_element_of_constraints.begin(); i != is_element_of_constraints.end(); ++i)
  {
    result.push_back(make_is_element_of_constraint(i->first, std::vector<sort_expression>(i->second.begin(), i->second.end())));
  }
  return result;
}

struct and_constraint: public type_check_constraint
{
  std::vector<constraint_ptr> alternatives;

  and_constraint(const std::vector<constraint_ptr>& alternatives_)
    : alternatives(alternatives_)
  {}

  std::string print() const
  {
    return print_node_vector("and", alternatives);
  }
};

inline
constraint_ptr make_and_constraint(const std::vector<constraint_ptr>& alternatives)
{
  std::vector<constraint_ptr> v;
  for (constraint_ptr p: alternatives)
  {
    false_constraint* x_false = dynamic_cast<false_constraint*>(p.get());
    if (x_false)
    {
      return p;
    }
    true_constraint* x_no = dynamic_cast<true_constraint*>(p.get());
    if (x_no)
    {
      continue;
    }
    and_constraint* x_and = dynamic_cast<and_constraint*>(p.get());
    if (x_and)
    {
      v.insert(v.end(), x_and->alternatives.begin(), x_and->alternatives.end());
      continue;
    }
    v.push_back(p);
  }
  v = join_is_element_of_constraints(v);
  if (v.size() == 0)
  {
    return make_true_constraint();
  }
  if (v.size() == 1)
  {
    return v.front();
  }
  return constraint_ptr(new and_constraint(v));
}

struct type_check_node
{
  std::vector<type_check_node_ptr> children;
  constraint_ptr constraint;
  untyped_sort_variable sort;

  type_check_node()
    : constraint(make_true_constraint())
  {}

  type_check_node(const std::vector<type_check_node_ptr>& children_)
    : children(children_), constraint(make_true_constraint())
  {}

  // Adds a value to the 'sort' attribute
  // Sets the constraints that apply to this node to 'constraint'
  virtual void set_constraint(type_check_context& context)
  {}

  virtual void apply_substitution(const sort_substitution& sigma)
  {
    for (type_check_node_ptr child: children)
    {
      child->apply_substitution(sigma);
    }
    constraint = substitute_constraint(constraint, sigma);
  }

  void set_children_constraints(type_check_context& context)
  {
    for (type_check_node_ptr child: children)
    {
      child->set_constraint(context);
    }
  }

  // Throws an exception if the node violates a well typedness rule
  virtual void check_well_typedness(const type_check_context& context)
  {}

  virtual std::string print() const = 0;
};

struct id_node: public type_check_node
{
  std::string value;

  id_node(const std::string& value_)
    : value(value_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();

    std::vector<constraint_ptr> alternatives;

    // it is a variable
    std::vector<sort_expression> variable_sorts = context.find_matching_variables(value);
    if (variable_sorts.size() == 1)
    {
      alternatives.push_back(make_is_equal_to_constraint(sort, variable_sorts.front()));
    }

    // it is a constant
    std::pair<sort_expression_list, sort_expression_list> p = context.find_matching_constants(value);
    for (const sort_expression& s: p.first + p.second)
    {
      alternatives.push_back(make_is_equal_to_constraint(sort, s));
    }

    // it is a function
    std::pair<function_sort_list, function_sort_list> q = context.find_matching_functions(value);
    for (const function_sort& s: q.first + q.second)
    {
      alternatives.push_back(make_is_equal_to_constraint(sort, s));
    }

    if (alternatives.empty())
    {
      constraint = make_false_constraint("The id " + value + " is not declared!");
    }
    else
    {
      constraint = make_or_constraint(alternatives);
    }
  }

  std::string print() const
  {
    return "id(" + value + ")";
  }
};

struct number_node: public type_check_node
{
  std::string value;

  number_node(const std::string& value_)
    : value(value_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();

    if (detail::is_pos(value))
    {
      constraint = make_subsort_constraint(sort_pos::pos(), sort);
    }
    else if (detail::is_nat(value))
    {
      constraint = make_subsort_constraint(sort_nat::nat(), sort);
    }
    else
    {
      throw mcrl2::runtime_error("unknown numeric string " + value);
    }
  }

  std::string print() const
  {
    return "number(" + value + ")";
  }
};

struct constant_node: public type_check_node
{
  std::string name;

  constant_node(const std::string& name_)
    : name(name_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();

    std::pair<sort_expression_list, sort_expression_list> p = context.find_matching_constants(name);
    std::vector<constraint_ptr> alternatives;
    for (const sort_expression& s: p.first + p.second)
    {
      alternatives.push_back(make_subsort_constraint(s, sort));
    }
    constraint = make_or_constraint(alternatives);
    set_children_constraints(context);
  }

  std::string print() const
  {
    return "constant(" + name + ")";
  }
};

struct true_node: public constant_node
{
  true_node()
    : constant_node("true")
  { }
};

struct false_node: public constant_node
{
  false_node()
    : constant_node("false")
  { }
};

struct empty_list_node: public constant_node
{
  empty_list_node()
    : constant_node("[]")
  { }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    constraint = make_is_equal_to_constraint(sort, sort_list::list(element_sort));
  }
};

struct empty_set_node: public constant_node
{
  empty_set_node()
    : constant_node("{}")
  { }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    constraint = make_is_equal_to_constraint(sort, sort_set::set_(element_sort));
  }
};

struct empty_bag_node: public constant_node
{
  empty_bag_node()
    : constant_node("{:}")
  { }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    constraint = make_is_equal_to_constraint(sort, sort_bag::bag(element_sort));
  }
};

struct list_enumeration_node: public type_check_node
{
  list_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    set_children_constraints(context);

    std::vector<constraint_ptr> alternatives;
    alternatives.push_back(make_is_equal_to_constraint(sort, sort_list::list(element_sort)));
    for (type_check_node_ptr child: children)
    {
      alternatives.push_back(make_subsort_constraint(element_sort, child->sort));
    }
    constraint = make_and_constraint(alternatives);
  }

  std::string print() const
  {
    return print_node_vector("list_enumeration", children);
  }
};

struct bag_enumeration_node: public type_check_node
{
  bag_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    set_children_constraints(context);

    std::vector<constraint_ptr> alternatives;
    alternatives.push_back(make_is_equal_to_constraint(sort, sort_bag::bag(element_sort)));
    for (std::size_t i = 0; i < children.size(); i++)
    {
      if (i % 2 == 0)
      {
        alternatives.push_back(make_subsort_constraint(element_sort, children[i]->sort));
      }
      else
      {
        alternatives.push_back(make_subsort_constraint(sort_nat::nat(), children[i]->sort));
      }
    }
    constraint = make_and_constraint(alternatives);
  }

  std::string print() const
  {
    return print_node_vector("bag_enumeration", children);
  }
};

struct set_enumeration_node: public type_check_node
{
  set_enumeration_node(const std::vector<type_check_node_ptr>& children)
    : type_check_node(children)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    auto element_sort = context.create_sort_variable();
    set_children_constraints(context);

    std::vector<constraint_ptr> alternatives;
    alternatives.push_back(make_is_equal_to_constraint(sort, sort_set::set_(element_sort)));
    for (type_check_node_ptr child: children)
    {
      alternatives.push_back(make_subsort_constraint(element_sort, child->sort));
    }
    constraint = make_and_constraint(alternatives);
  }

  std::string print() const
  {
    return print_node_vector("set_enumeration", children);
  }
};

struct bag_or_set_enumeration_node: public type_check_node
{
  variable v;

  bag_or_set_enumeration_node(const variable& v_, type_check_node_ptr x)
    : v(v_)
  {
    children.push_back(x);
  }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    context.add_context_variable(v);
    set_children_constraints(context);

    auto element_sort = v.sort();
    constraint_ptr set_constraint = make_and_constraint({
      make_is_equal_to_constraint(sort, sort_set::set_(element_sort)),
      make_is_equal_to_constraint(children.front()->sort, sort_bool::bool_())
    });
    constraint_ptr bag_constraint = make_and_constraint({
        make_is_equal_to_constraint(sort, sort_bag::bag(element_sort)),
        make_subsort_constraint(sort_nat::nat(), children.front()->sort)
       });
    constraint = make_or_constraint({ set_constraint, bag_constraint });
    context.remove_context_variable(v);
  }

  std::string print() const
  {
    return print_node_vector("bag_or_set_enumeration", children);
  }
};

struct function_update_node: public type_check_node
{
  function_update_node(type_check_node_ptr x1, type_check_node_ptr x2, type_check_node_ptr x3)
  {
    children.push_back(x1);
    children.push_back(x2);
    children.push_back(x3);
  }

  std::string print() const
  {
    return print_node_vector("function_update", children);
  }
};

struct application_node: public type_check_node
{
  std::size_t arity;

  application_node(type_check_node_ptr head, const std::vector<type_check_node_ptr>& arguments)
    : arity(arguments.size())
  {
    children.push_back(head);
    for (auto arg: arguments)
    {
      children.push_back(arg);
    }
  }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    set_children_constraints(context);

    const sort_expression& codomain = sort;
    std::vector<constraint_ptr> alternatives;
    std::vector<sort_expression> domain;
    for (std::size_t i = 0; i < arity; i++)
    {
      domain.push_back(children[i+1]->sort);
    }
    alternatives.push_back(make_is_equal_to_constraint(children[0]->sort, function_sort(sort_expression_list(domain.begin(), domain.end()), codomain)));

    // add missing constraints for if application
    // TODO: handle this in a more structured way
    if (children[0]->print() == "id(if)")
    {
      alternatives.push_back(make_subsort_constraint(codomain, children[1]->sort));
      alternatives.push_back(make_subsort_constraint(codomain, children[2]->sort));
    }

    constraint = make_and_constraint(alternatives);
  }

  std::string print() const
  {
    return print_node_vector("application", children);
  }
};

struct unary_operator_node: public type_check_node
{
  std::string name;

  unary_operator_node(const std::string& name_, type_check_node_ptr arg)
    : type_check_node({ arg }), name(name_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    set_children_constraints(context);

    std::pair<function_sort_list, function_sort_list> p = context.find_matching_functions(name, 1);
    std::vector<constraint_ptr> alternatives;
    for (const function_sort& s: p.first + p.second)
    {
      alternatives.push_back(make_and_constraint({
          make_subsort_constraint(children[0]->sort, s.domain().front()),
          make_subsort_constraint(s.codomain(), sort)
         })
      );
    }
    constraint = make_or_constraint(alternatives);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "unary_operator(" << name << ", " << children.front()->print() << ")";
    return out.str();
  }
};

struct forall_node: public type_check_node
{
  variable_list variables;

  forall_node(const variable_list& variables_, type_check_node_ptr arg)
    : type_check_node({ arg }), variables(variables_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    context.add_context_variables(variables);
    set_children_constraints(context);

    constraint = make_is_equal_to_constraint(sort, sort_bool::bool_());
    context.remove_context_variables(variables);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "forall(" << data::pp(variables) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct exists_node: public type_check_node
{
  variable_list variables;

  exists_node(const variable_list& variables_, type_check_node_ptr arg)
    : type_check_node({ arg }), variables(variables_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    context.add_context_variables(variables);
    set_children_constraints(context);

    constraint = make_is_equal_to_constraint(sort, sort_bool::bool_());
    context.remove_context_variables(variables);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "exists(" << data::pp(variables) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct lambda_node: public unary_operator_node
{
  variable_list variables;

  lambda_node(const variable_list& variables_, type_check_node_ptr arg)
    : unary_operator_node("lambda", arg), variables(variables_)
  {}

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    context.add_context_variables(variables);
    set_children_constraints(context);

    constraint = make_true_constraint();
    context.remove_context_variables(variables);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "lambda(" << data::pp(variables) << ". " << children.front()->print() << ")";
    return out.str();
  }
};

struct binary_operator_node: public type_check_node
{
  std::string name;

  binary_operator_node(const std::string& name_, type_check_node_ptr left, type_check_node_ptr right)
    : type_check_node({ left, right }), name(name_)
  {}

  void set_constraint(type_check_context& context)
  {
    set_children_constraints(context);

    untyped_sort_variable s = context.create_sort_variable();
    sort = s;

    std::pair<function_sort_list, function_sort_list> p = context.find_matching_functions(name, 2);
    std::vector<constraint_ptr> alternatives;
    for (const function_sort& s: p.first + p.second)
    {
      auto i = s.domain().begin();
      const sort_expression& left = *i++;
      const sort_expression& right = *i;
      alternatives.push_back(make_and_constraint({
          make_subsort_constraint(children[0]->sort, left),
          make_subsort_constraint(children[1]->sort, right),
          make_subsort_constraint(s.codomain(), sort)
         })
      );
    }
    constraint = make_or_constraint(alternatives);
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "binary_operator(" << name << ", " << children[0]->print() << ", " << children[1]->print() << ")";
    return out.str();
  }
};

struct where_clause_node: public type_check_node
{
  std::vector<std::string> variable_names;

  where_clause_node(type_check_node_ptr body, const std::vector<std::pair<std::string, type_check_node_ptr> >& assignments)
  {
    children.push_back(body);
    variable_vector v;
    for (const std::pair<std::string, type_check_node_ptr>& a: assignments)
    {
      variable_names.push_back(a.first);
      children.push_back(a.second);
    }
  }

  void set_constraint(type_check_context& context)
  {
    sort = context.create_sort_variable();
    variable_list variables;
    for (const std::string& name: variable_names)
    {
      variables.push_front(variable(name, untyped_sort()));
    }
    context.add_context_variables(variables);
    set_children_constraints(context);

    std::vector<constraint_ptr> constraints = { make_subsort_constraint(untyped_sort(), sort) };
    for (type_check_node_ptr child: children)
    {
      constraints.push_back(make_subsort_constraint(untyped_sort(), child->sort));
    }
    constraint = make_and_constraint(constraints);

    context.remove_context_variables(variables);
  }

  std::string print() const
  {
    std::ostringstream out;
    return print_node_vector("where_clause", children);
    return out.str();
  }
};

struct type_check_tree_generator: public detail::data_expression_actions
{
  type_check_tree_generator(const core::parser& parser_)
    : data_expression_actions(parser_)
  {}

  std::vector<type_check_node_ptr> parse_DataExprList(const core::parse_node& node) const
  {
    return parse_vector<type_check_node_ptr>(node, "DataExpr", [&](const core::parse_node& node) { return parse_DataExpr(node); });
  }

  std::vector<type_check_node_ptr> parse_BagEnumEltList(const core::parse_node& node) const
  {
    return parse_DataExprList(node);
  }

  std::pair<std::string, type_check_node_ptr> parse_Assignment(const core::parse_node& node) const
  {
    return std::make_pair(parse_Id(node.child(0)), parse_DataExpr(node.child(2)));
  }

  std::vector<std::pair<std::string, type_check_node_ptr> > parse_AssignmentList(const core::parse_node& node) const
  {
    return parse_vector<std::pair<std::string, type_check_node_ptr> >(node, "Assignment", [&](const core::parse_node& node) { return parse_Assignment(node); });
  }

  type_check_node_ptr parse_DataExpr(const core::parse_node& node) const
  {
    assert(symbol_name(node) == "DataExpr");
    if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Id")) { return type_check_node_ptr(new id_node(parse_Id(node.child(0)))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "Number")) { return type_check_node_ptr(new number_node(parse_Number(node.child(0)))); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "true")) { return type_check_node_ptr(new true_node()); }
    else if ((node.child_count() == 1) && (symbol_name(node.child(0)) == "false")) { return type_check_node_ptr(new false_node()); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "]")) { return type_check_node_ptr(new empty_list_node()); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "}")) { return type_check_node_ptr(new empty_set_node()); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == ":") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new empty_bag_node()); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "[") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "]")) { return type_check_node_ptr(new list_enumeration_node(parse_DataExprList(node.child(1)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "BagEnumEltList") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new bag_enumeration_node(parse_BagEnumEltList(node.child(1)))); }
    else if ((node.child_count() == 5) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "VarDecl") && (symbol_name(node.child(2)) == "|") && (symbol_name(node.child(3)) == "DataExpr") && (symbol_name(node.child(4)) == "}")) { return type_check_node_ptr(new bag_or_set_enumeration_node(parse_VarDecl(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "{") && (symbol_name(node.child(1)) == "DataExprList") && (symbol_name(node.child(2)) == "}")) { return type_check_node_ptr(new set_enumeration_node(parse_DataExprList(node.child(1)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "(") && (symbol_name(node.child(1)) == "DataExpr") && (symbol_name(node.child(2)) == ")")) { return parse_DataExpr(node.child(1)); }
    else if ((node.child_count() == 6) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "[") && (symbol_name(node.child(2)) == "DataExpr") && (symbol_name(node.child(3)) == "->") && (symbol_name(node.child(4)) == "DataExpr") && (symbol_name(node.child(5)) == "]")) { return type_check_node_ptr(new function_update_node(parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)), parse_DataExpr(node.child(4)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "(") && (symbol_name(node.child(2)) == "DataExprList") && (symbol_name(node.child(3)) == ")")) { return type_check_node_ptr(new application_node(parse_DataExpr(node.child(0)), parse_DataExprList(node.child(2)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "!") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("!", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "-") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("-", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 2) && (symbol_name(node.child(0)) == "#") && (symbol_name(node.child(1)) == "DataExpr")) { return type_check_node_ptr(new unary_operator_node("#", parse_DataExpr(node.child(1)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "forall") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new forall_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "exists") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new exists_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "lambda") && (symbol_name(node.child(1)) == "VarsDeclList") && (symbol_name(node.child(2)) == ".") && (symbol_name(node.child(3)) == "DataExpr")) { return type_check_node_ptr(new lambda_node(parse_VarsDeclList(node.child(1)), parse_DataExpr(node.child(3)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "=>" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("=>" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "&&" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("&&" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "||" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("||" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "==" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("==" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "!=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("!=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">=" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node(">=" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == ">"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node(">"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "in" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("in" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "|>" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("|>" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "<|" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("<|" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "++" ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("++" , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "+"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("+"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "-"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("-"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "/"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("/"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "div") && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("div", parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "mod") && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("mod", parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "*"  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("*"  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 3) && (symbol_name(node.child(0)) == "DataExpr") && (node.child(1).string() == "."  ) && (symbol_name(node.child(2)) == "DataExpr")) { return type_check_node_ptr(new binary_operator_node("."  , parse_DataExpr(node.child(0)), parse_DataExpr(node.child(2)))); }
    else if ((node.child_count() == 4) && (symbol_name(node.child(0)) == "DataExpr") && (symbol_name(node.child(1)) == "whr") && (symbol_name(node.child(2)) == "AssignmentList") && (symbol_name(node.child(3)) == "end")) { return type_check_node_ptr(new where_clause_node({ parse_DataExpr(node.child(0)) }, parse_AssignmentList(node.child(2)))); }
    throw core::parse_node_unexpected_exception(m_parser, node);
  }
};

inline
std::pair<sort_expression_list, sort_expression_list> type_check_context::find_matching_constants(const std::string& name) const
{
  sort_expression_list system_result;
  sort_expression_list user_result;
  auto i = system_constants.find(core::identifier_string(name));
  if (i != system_constants.end())
  {
    system_result = i->second;
  }
  auto j = user_constants.find(core::identifier_string(name));
  if (j != user_constants.end())
  {
    user_result = { j->second };
  }
  return std::make_pair(system_result, user_result);
}

inline
function_sort_list filter_sorts(const function_sort_list& sorts, std::size_t arity)
{
  std::vector<function_sort> result;
  for (const function_sort& sort: sorts)
  {
    if (sort.domain().size() == arity)
    {
      result.push_back(sort);
    }
  }
  return function_sort_list(result.begin(), result.end());
}

inline
std::pair<function_sort_list, function_sort_list> type_check_context::find_matching_functions(const std::string& name, std::size_t arity) const
{
  function_sort_list system_result;
  function_sort_list user_result;
  auto i = system_functions.find(core::identifier_string(name));
  if (i != system_functions.end())
  {
    system_result = filter_sorts(i->second, arity);
  }
  auto j = user_functions.find(core::identifier_string(name));
  if (j != user_functions.end())
  {
    user_result = filter_sorts(j->second, arity);
  }
  return std::make_pair(system_result, user_result);
}

inline
std::pair<function_sort_list, function_sort_list> type_check_context::find_matching_functions(const std::string& name) const
{
  function_sort_list system_result;
  function_sort_list user_result;
  auto i = system_functions.find(core::identifier_string(name));
  if (i != system_functions.end())
  {
    system_result = i->second;
  }
  auto j = user_functions.find(core::identifier_string(name));
  if (j != user_functions.end())
  {
    user_result = j->second;
  }
  return std::make_pair(system_result, user_result);
}

inline
std::vector<sort_expression> type_check_context::find_matching_variables(const std::string& name) const
{
  std::vector<sort_expression> result;
  auto i = declared_variables.find(core::identifier_string(name));
  if (i != declared_variables.end())
  {
    result.push_back(i->second.back());
  }
  return result;
}

inline
void print_node(type_check_node_ptr node)
{
  std::cout << "\nnode = " << node->print() << std::endl;
  std::cout << "sort = " << node->sort << std::endl;
  std::cout << "constraint = " << node->constraint->print() << std::endl;
  for (type_check_node_ptr child: node->children)
  {
    print_node(child);
  }
}

// TODO: This design is ugly, but for the moment it seems the easiest solution to modify
// the constraint tree
inline
constraint_ptr substitute_constraint(constraint_ptr p, const sort_substitution& sigma)
{
  {
    or_constraint* x = dynamic_cast<or_constraint*>(p.get());
    if (x)
    {
      for (constraint_ptr& q: x->alternatives)
      {
        q = substitute_constraint(q, sigma);
      }
      return p;
    }
  }
  {
    and_constraint* x = dynamic_cast<and_constraint*>(p.get());
    if (x)
    {
      for (constraint_ptr& q: x->alternatives)
      {
        q = substitute_constraint(q, sigma);
      }
      return p;
    }
  }
  {
    is_equal_to_constraint* x = dynamic_cast<is_equal_to_constraint*>(p.get());
    if (x)
    {
      sort_expression s1 = substitute(x->s1, sigma);
      sort_expression s2 = substitute(x->s2, sigma);
      return make_is_equal_to_constraint(s1, s2);
    }
  }
  return p;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_EXPERIMENTAL_TYPE_CHECK_TREE_H
