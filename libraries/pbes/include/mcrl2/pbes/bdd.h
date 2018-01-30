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
#include "mcrl2/data/application.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2 {

namespace pbes_system {

namespace bdd {

class term
{
  public:
    virtual std::string print(bool after = false) const = 0;
};

typedef std::shared_ptr<term> bdd_node;

class true_: public term
{
  std::string print(bool after = false) const override
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
  std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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

    std::string print(bool after = false) const override
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
bdd_node to_bdd(const data::data_expression& x)
{
  if (data::is_variable(x) && data::sort_bool::is_bool(atermpp::down_cast<data::variable>(x).sort()))
  {
    const data::variable& y = atermpp::down_cast<data::variable>(x);
    return make_variable(y.name());
  }
  else if (data::sort_bool::is_true_function_symbol(x))
  {
    return make_true();
  }
  else if (data::sort_bool::is_false_function_symbol(x))
  {
    return make_false();
  }
  else if (data::is_equal_to_application(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_eq(to_bdd(left), to_bdd(right));
  }
  else if (data::is_not_equal_to_application(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_imp(to_bdd(left), make_not(to_bdd(right)));
  }
  else if (data::sort_bool::is_or_application(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_or(to_bdd(left), to_bdd(right));
  }
  else if (data::sort_bool::is_and_application(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_and(to_bdd(left), to_bdd(right));
  }
  else if (data::sort_bool::is_implies_application(x))
  {
    const auto& left  = data::binary_left1(x);
    const auto& right = data::binary_right1(x);
    return make_imp(to_bdd(left), to_bdd(right));
  }
  throw mcrl2::runtime_error("Unsupported data expression " + data::pp(x) + " encountered in to_bdd.");
}

inline
bdd_node join_and(const std::vector<bdd_node> v)
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

// returns the smallest value m such that n <= 2**m
// N.B. very inefficient
inline
constexpr std::size_t log2_rounded_up(std::size_t n)
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
std::vector<bdd_node> make_bdd_representation_variables(std::size_t n)
{
  std::size_t m = log2_rounded_up(n);
  std::vector<bdd_node> result;
  for (std::size_t i = 0; i < m; i++)
  {
    result.push_back(make_variable("rep" + std::to_string(i)));
  }
  return result;
}

inline
std::map<pbes_system::propositional_variable, bdd_node> make_bdd_representation(const std::vector<pbes_system::pbes_equation>& equations, const std::vector<bdd_node> representation_variables)
{
  std::map<pbes_system::propositional_variable, bdd_node> result;

  std::size_t m = representation_variables.size();
  std::size_t n = equations.size();

  std::vector<std::vector<bdd_node> > sequences(n, std::vector<bdd_node>());
  std::size_t repeat = 1;
  for (std::size_t i = 0; i < m; i++)
  {
    for (std::size_t j = 0; j < n; j++)
    {
      bool negate = (j / repeat) % 2 == 0;
      sequences[j].push_back(negate ? make_not(representation_variables[i]) : representation_variables[i]);
    }
  }

  for (std::size_t j = 0; j < n; j++)
  {
    result[equations[j].variable()] = join_and(sequences[j]);
  }
  return result;
}

}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_BDD_H
