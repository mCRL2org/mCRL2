// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bdd.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_BDD_H
#define MCRL2_PBES_BDD_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <utility>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/consistency.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace pbes_system {

namespace bdd {

//-------------------------------------- utilities ----------------------------------------//

template <typename InputIt>
std::string string_join(InputIt first, InputIt last, const std::string & sep = ", ")
{
  std::ostringstream out;
  if (first != last)
  {
    out << *first++;
  }
  while (first != last)
  {
    out << sep;
    out << *first++;
  }
  return out.str();
}

inline
std::string string_join(const std::vector<std::string>& v, const std::string & sep = ", ")
{
  return string_join(v.begin(), v.end(), sep);
}

inline
std::vector<std::string> add_parens(const std::vector<std::string>& v)
{
  std::vector<std::string> result;
  for (const std::string& x: v)
  {
    result.push_back('(' + x + ')');
  }
  return result;
}

inline
std::vector<std::string> add_single_quotes(const std::vector<std::string>& v)
{
  std::vector<std::string> result;
  for (const std::string& x: v)
  {
    result.push_back('\'' + x + '\'');
  }
  return result;
}

//-------------------------------------- bdd_node -----------------------------------------//

class term
{
  public:
    virtual std::string print(bool after) const = 0;
};

typedef std::shared_ptr<term> bdd_node;

template <typename BddNodeContainer>
std::vector<std::string> to_string(const BddNodeContainer& nodes, bool after = false)
{
  std::vector<std::string> result;
  for (const bdd_node& x: nodes)
  {
    result.push_back(x->print(after));
  }
  return result;
}

class true_: public term
{
  std::string print(bool /* after */) const override
  {
    return "True";
  }
};

inline
bdd_node make_true()
{
  return std::make_shared<true_>();
}

class false_: public term
{
  std::string print(bool /* after */ = false) const override
  {
    return "False";
  }
};

inline
bdd_node make_false()
{
  return std::make_shared<false_>();
}

class variable: public term
{
  protected:
    std::string m_name;

  public:
    variable(const std::string& name)
    {
      m_name = name;
    }

    std::string print(bool after) const override
    {
      return after ? m_name + '_' : m_name;
    }
};

inline
bdd_node make_variable(const std::string& name)
{
  return std::make_shared<variable>(name);
}

class not_: public term
{
  protected:
    bdd_node m_operand;

  public:
    not_(bdd_node operand)
      : m_operand(operand)
    { }

    const term& operand() const
    {
      return *m_operand;
    }

    std::string print(bool after) const override
    {
      return "~" + m_operand->print(after);
    }
};

inline
bdd_node make_not(bdd_node x)
{
  return std::make_shared<not_>(x);
}

class and_: public term
{
  protected:
    bdd_node m_left;
    bdd_node m_right;

  public:
    and_(bdd_node left, bdd_node right)
      : m_left(left), m_right(right)
    { }

    const term& left() const
    {
      return *m_left;
    }

    const term& right() const
    {
      return *m_right;
    }

    std::string print(bool after) const override
    {
      return "(" + m_left->print(after) + " & " + m_right->print(after) + ")";
    }
};

inline
bdd_node make_and(bdd_node left, bdd_node right)
{
  return std::make_shared<and_>(left, right);
}

class or_: public term
{
  protected:
    bdd_node m_left;
    bdd_node m_right;

  public:
    or_(bdd_node left, bdd_node right)
      : m_left(left), m_right(right)
    { }

    const term& left() const
    {
      return *m_left;
    }

    const term& right() const
    {
      return *m_right;
    }

    std::string print(bool after) const override
    {
      return "(" + m_left->print(after) + " | " + m_right->print(after) + ")";
    }
};

inline
bdd_node make_or(bdd_node left, bdd_node right)
{
  return std::make_shared<or_>(left, right);
}

class eq: public term
{
  protected:
    bdd_node m_left;
    bdd_node m_right;

  public:
    eq(bdd_node left, bdd_node right)
      : m_left(left), m_right(right)
    { }

    const term& left() const
    {
      return *m_left;
    }

    const term& right() const
    {
      return *m_right;
    }

    std::string print(bool after) const override
    {
      return "(" + m_left->print(after) + " <-> " + m_right->print(after) + ")";
    }
};

inline
bdd_node make_eq(bdd_node left, bdd_node right)
{
  return std::make_shared<eq>(left, right);
}

class imp: public term
{
  protected:
    bdd_node m_left;
    bdd_node m_right;

  public:
    imp(bdd_node left, bdd_node right)
      : m_left(left), m_right(right)
    { }

    const term& left() const
    {
      return *m_left;
    }

    const term& right() const
    {
      return *m_right;
    }

    std::string print(bool after) const override
    {
      return "(" + m_left->print(after) + " -> " + m_right->print(after) + ")";
    }
};

inline
bdd_node make_imp(bdd_node left, bdd_node right)
{
  return std::make_shared<imp>(left, right);
}

inline
bdd_node all(const std::vector<bdd_node>& v)
{
  if (v.empty())
  {
    return make_true();
  }
  bdd_node result = v[0];
  for (auto i = ++v.begin(); i != v.end(); ++i)
  {
    result = make_and(result, *i);
  }
  return result;
}

inline
bdd_node any(const std::vector<bdd_node>& v)
{
  if (v.empty())
  {
    return make_false();
  }
  bdd_node result = v[0];
  for (auto i = ++v.begin(); i != v.end(); ++i)
  {
    result = make_or(result, *i);
  }
  return result;
}

inline
std::vector<bdd_node> operator+(const std::vector<bdd_node>& x, const std::vector<bdd_node>& y)
{
  std::vector<bdd_node> result = x;
  result.insert(result.end(), y.begin(), y.end());
  return result;
}

//------------------------------------ data_expression ------------------------------------//

inline
bdd_node to_bdd(const data::data_expression& x)
{
  if (data::is_variable(x) && data::is_bool(atermpp::down_cast<data::variable>(x).sort()))
  {
    const auto& y = atermpp::down_cast<data::variable>(x);
    return make_variable(y.name());
  }
  else if (data::is_true(x))
  {
    return make_true();
  }
  else if (data::is_false(x))
  {
    return make_false();
  }
  else if (data::is_not(x))
  {
    const auto& operand = data::sort_bool::arg(x);
    return make_not(to_bdd(operand));
  }
  else if (data::is_equal_to(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_eq(to_bdd(left), to_bdd(right));
  }
  else if (data::is_not_equal_to(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_imp(to_bdd(left), make_not(to_bdd(right)));
  }
  else if (data::is_or(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_or(to_bdd(left), to_bdd(right));
  }
  else if (data::is_and(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_and(to_bdd(left), to_bdd(right));
  }
  else if (data::is_imp(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_imp(to_bdd(left), to_bdd(right));
  }
  throw mcrl2::runtime_error("Unsupported data expression " + data::pp(x) + " encountered in to_bdd.");
}

inline
std::vector<bdd_node> to_bdd(const data::data_expression_list& v)
{
  std::vector<bdd_node> result;
  for (const data::data_expression& x: v)
  {
    result.push_back(to_bdd(x));
  }
  return result;
}

//---------------------------------- equation identifiers ---------------------------------//

// returns the smallest value m such that n <= 2**m
// N.B. very inefficient
inline
std::size_t log2_rounded_up(std::size_t n)
{
  std::size_t m = 0;
  std::size_t value = 1;
  for (;;)
  {
    if (n <= value)
    {
      break;
    }
    m++;
    value *= 2;
  }
  return m;
}

inline
std::vector<bdd_node> id_variables(std::size_t n, bool unary_encoding)
{
  std::size_t m = unary_encoding ? n : log2_rounded_up(n);
  std::vector<bdd_node> result;
  for (std::size_t i = 0; i < m; i++)
  {
    result.push_back(make_variable("rep" + std::to_string(i)));
  }
  return result;
}

inline
std::vector<bdd_node> equation_identifiers(const std::vector<pbes_system::pbes_equation>& equations, const std::vector<bdd_node>& id_variables, bool unary_encoding)
{
  std::vector<bdd_node> result;
  std::size_t n = equations.size();
  std::vector<std::vector<bdd_node>> sequences(n, std::vector<bdd_node>());

  if (unary_encoding)
  {
    for (std::size_t i = 0; i < n; i++)
    {
      for (std::size_t j = 0; j < n; j++)
      {
        sequences[j].push_back(i == j ? make_not(id_variables[i]) : id_variables[i]);
      }
    }
  }
  else
  {
    std::size_t m = id_variables.size();

    std::size_t repeat = 1;
    for (std::size_t i = 0; i < m; i++)
    {
      for (std::size_t j = 0; j < n; j++)
      {
        bool negate = (j / repeat) % 2 == 0;
        sequences[j].push_back(negate ? make_not(id_variables[i]) : id_variables[i]);
      }
      repeat *= 2;
    }
  }

  for (std::size_t j = 0; j < n; j++)
  {
    result.push_back(all(sequences[j]));
  }
  return result;
}

//---------------------------------- bdd building blocks ----------------------------------//

inline
std::vector<bdd_node> param_variables(const pbes_system::propositional_variable& X)
{
  std::vector<bdd_node> result;
  for (const data::variable& x: X.parameters())
  {
    result.push_back(to_bdd(x));
  }
  return result;
}

inline
bdd_node at_least_one_equation(const std::map<pbes_system::propositional_variable, bdd_node>& equation_map)
{
  std::vector<bdd_node> v;
  for (const auto& p: equation_map)
  {
    v.push_back(p.second);
  }
  return any(v);
}

inline
std::string unchanged_variables(const std::vector<bdd_node>& variables)
{
  std::vector<std::string> result;
  for (const bdd_node& x: variables)
  {
    result.push_back("(" + x->print(false) + " <-> " + x->print(true) + ")");
  }
  return string_join(result.begin(), result.end(), " & ");
}

//---------------------------------- bdd equations ----------------------------------------//

struct bdd_equation
{
  typedef std::pair<data::data_expression, pbes_system::propositional_variable_instantiation> element;

  bool is_disjunctive = false;
  std::vector<element> elements;
  bdd_node id; // the unique encoding of this equation
  std::size_t rank = 0;

  // left and right consists of a propositional variable instantiation and a data expression
  void add_element(const pbes_system::pbes_expression& left, const pbes_system::pbes_expression& right)
  {
    if (data::is_data_expression(left) && pbes_system::is_propositional_variable_instantiation(right))
    {
      elements.emplace_back(atermpp::down_cast<data::data_expression>(left), atermpp::down_cast<pbes_system::propositional_variable_instantiation>(right));
    }
    else if (pbes_system::is_propositional_variable_instantiation(left) && data::is_data_expression(right))
    {
      elements.emplace_back(atermpp::down_cast<data::data_expression>(right), atermpp::down_cast<pbes_system::propositional_variable_instantiation>(left));
    }
    else
    {
      throw mcrl2::runtime_error("Unexpected expressions " + pbes_system::pp(left) + " and " + pbes_system::pp(right) + " encountered in split_pbes_equation (expected a data expression and a predicate variable).");
    }
  }

  // x is a propositional variable instantiation
  void add_element(const pbes_system::pbes_expression& x)
  {
    data::data_expression T = data::true_();
    elements.emplace_back(T, atermpp::down_cast<pbes_system::propositional_variable_instantiation>(x));
  }

  std::string parameter_updates(const element& e, const std::vector<bdd_node>& parameters) const
  {
    std::vector<std::string> param0 = to_string(parameters, true);
    std::vector<std::string> param1 = add_parens(to_string(to_bdd(e.second.parameters())));

    std::vector<std::string> v;
    for (std::size_t i = 0; i < param0.size(); i++)
    {
      v.push_back(param0[i] + " <-> " + param1[i]);
    }
    return string_join(add_parens(v), " & ");
  }

  std::vector<std::string> edge_relation(const pbes_equation_index& eqn_index, const std::vector<bdd_node>& ids, const std::vector<bdd_node>& parameters) const
  {
    std::vector<std::string> result;
    for (const element& e: elements)
    {
      std::string id0 = id->print(false);
      std::size_t i1 = eqn_index.index(e.second.name());
      std::string id1 = ids[i1]->print(true);
      std::string f = to_bdd(e.first)->print(false);
      std::string updates = parameter_updates(e, parameters);
      std::vector<std::string> v = { id0, id1, f, updates };
      result.push_back(string_join(add_parens(v), " & "));
    }
    return result;
  }
};

inline
std::ostream& operator<<(std::ostream& out, const bdd_equation& eqn)
{
  out << "disjunctive = " << std::boolalpha << eqn.is_disjunctive;
  for (const auto& element: eqn.elements)
  {
    out << " (" << element.first << ", " << element.second << ")";
  }
  return out;
}

inline
bool is_degenerate_and(const pbes_system::pbes_expression& x)
{
  const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::and_>(x).left();
  const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::and_>(x).right();
  return data::is_data_expression(left) || data::is_data_expression(right);
}

inline
bool is_degenerate_or(const pbes_system::pbes_expression& x)
{
  const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::or_>(x).left();
  const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::or_>(x).right();
  return data::is_data_expression(left) || data::is_data_expression(right);
}

inline
bdd_equation split_pbes_equation(const pbes_equation& eqn, std::size_t rank, const bdd_node& id)
{
  bdd_equation result;
  result.rank = rank;
  result.id = id;
  const pbes_system::pbes_expression& x = eqn.formula();
  if (pbes_system::is_propositional_variable_instantiation(x))
  {
    result.is_disjunctive = true;
    result.add_element(x);
  }
  else if (pbes_system::is_and(x))
  {
    if (is_degenerate_and(x))
    {
      result.is_disjunctive = true;
      const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::and_>(x).left();
      const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::and_>(x).right();
      result.add_element(left, right);
    }
    else
    {
      result.is_disjunctive = false;
      for (const pbes_system::pbes_expression& x_i: pbes_system::split_and(x))
      {
        if (pbes_system::is_propositional_variable_instantiation(x_i))
        {
          result.add_element(x_i);
        }
        else if (pbes_system::is_or(x_i))
        {
          const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::or_>(x_i).left();
          const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::or_>(x_i).right();
          result.add_element(left, right);
        }
        else
        {
          throw mcrl2::runtime_error("Unexpected expression " + pbes_system::pp(x) + " encountered in split_pbes_equation.");
        }
      }
    }
  }
  else if (pbes_system::is_or(x))
  {
    if (is_degenerate_or(x))
    {
      result.is_disjunctive = false;
      const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::or_>(x).left();
      const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::or_>(x).right();
      result.add_element(left, right);
    }
    else
    {
      result.is_disjunctive = true;
      for (const pbes_system::pbes_expression& x_i: pbes_system::split_or(x))
      {
        if (pbes_system::is_propositional_variable_instantiation(x_i))
        {
          result.add_element(x_i);
        }
        else if (pbes_system::is_and(x_i))
        {
          const pbes_system::pbes_expression& left  = atermpp::down_cast<pbes_system::and_>(x_i).left();
          const pbes_system::pbes_expression& right = atermpp::down_cast<pbes_system::and_>(x_i).right();
          result.add_element(left, right);
        }
        else
        {
          throw mcrl2::runtime_error("Unexpected expression " + pbes_system::pp(x) + " encountered in split_pbes_equation.");
        }
      }
    }
  }
  else
  {
    throw mcrl2::runtime_error("Unexpected expression " + pbes_system::pp(x) + " encountered in split_pbes_equation.");
  }
  return result;
}

inline
std::vector<bdd_equation> split_pbes(const pbes& p, const pbes_equation_index& eqn_index, const std::vector<bdd_node>& ids)
{
  std::vector<bdd_equation> result;
  const std::vector<pbes_system::pbes_equation>& equations = p.equations();
  for (std::size_t i = 0; i < equations.size(); i++)
  {
    const pbes_system::pbes_equation& eqn = equations[i];
    std::size_t rank = eqn_index.rank(eqn.variable().name());
    const bdd_node& id = ids[i];
    result.push_back(split_pbes_equation(eqn, rank, id));
  }
  return result;
}

inline
std::vector<bdd_node> even_ids(const std::vector<bdd_equation>& equations)
{
  std::vector<bdd_node> result;
  for (const bdd_equation& eqn: equations)
  {
    if (eqn.is_disjunctive)
    {
      result.push_back(eqn.id);
    }
  }
  return result;
}

inline
std::vector<bdd_node> odd_ids(const std::vector<bdd_equation>& equations)
{
  std::vector<bdd_node> result;
  for (const bdd_equation& eqn: equations)
  {
    if (!eqn.is_disjunctive)
    {
      result.push_back(eqn.id);
    }
  }
  return result;
}

inline
std::map<std::size_t, std::vector<bdd_node>> priority_map(const std::vector<bdd_equation>& equations)
{
  std::map<std::size_t, std::vector<bdd_node>> result;
  for (const bdd_equation& eqn: equations)
  {
    result[eqn.rank].push_back(eqn.id);
  }
  return result;
}

inline
std::string pbes2bdd(const pbes_system::pbes& p, bool unary_encoding = false)
{
  std::ostringstream out;

  pbes_equation_index eqn_index(p);

  // bdd variables
  std::vector<bdd_node> ivar = id_variables(p.equations().size(), unary_encoding);
  std::vector<bdd_node> pvar = param_variables(p.equations().front().variable());

  // equation ids
  std::vector<bdd_node> ids = equation_identifiers(p.equations(), ivar, unary_encoding);

  std::vector<bdd_equation> equations = split_pbes(p, eqn_index, ids);

  out << "--- bdd variables ---\n" << string_join(add_single_quotes(to_string(ivar + pvar)), ", ") << "\n\n";
  out << "--- all nodes ---\n" << any(ids)->print(false) << "\n\n";
  out << "--- even nodes ---\n" << any(even_ids(equations))->print(false) << "\n\n";
  out << "--- odd nodes ---\n" << any(odd_ids(equations))->print(false) << "\n\n";

  out << "--- priorities ---\n";
  std::map<std::size_t, std::vector<bdd_node>> priority_ids = priority_map(equations);
  std::size_t min_rank = equations.front().rank;
  std::size_t max_rank = equations.back().rank;
  // We prefer a max priority game, so the ranks need to be reversed
  // Start at 0 if max_rank is even
  for (std::size_t rank = min_rank; rank <= max_rank; rank++)
  {
    std::size_t r = max_rank - rank + (max_rank % 2);
    out << "prio[" << r << "] = " << any(priority_ids[rank])->print(false) << "\n";
  }
  out << "\n";

  out << "--- edges ---\n";
  std::vector<std::string> edges;
  for (const bdd_equation& eqn: equations)
  {
    std::vector<std::string> E = eqn.edge_relation(eqn_index, ids, pvar);
    edges.insert(edges.end(), E.begin(), E.end());
  }
  out << "'''  " << string_join(add_parens(edges), "\n| ") << "'''\n";
  return out.str();
}

} // namespace bdd

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BDD_H
