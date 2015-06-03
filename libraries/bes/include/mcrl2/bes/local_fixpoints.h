// Author(s): XIAO Qi
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/local_fixpoints.h
/// \brief Solve a BES by finding fixpoints for groups of equations of the
///        same rank.

#ifndef MCRL2_BES_LOCAL_FIXPOINTS_H
#define MCRL2_BES_LOCAL_FIXPOINTS_H

#include <vector>
#include <unordered_set>
#include <map>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{

namespace bes
{

/// \brief Algorithm class
class local_fixpoints_algorithm
{
  protected:
    const boolean_equation_system& m_bes;
    std::vector<size_t> ranks;
    std::map<boolean_variable, size_t> indices;
    size_t max_rank;

  public:
    local_fixpoints_algorithm(const boolean_equation_system& b)
      : m_bes(b), ranks(b.equations().size())
    {
      auto eqs = b.equations();
      for (size_t i = 1; i < eqs.size(); i++)
      {
        indices[eqs[i].variable()] = i;
        ranks[i] = ranks[i-1] + (eqs[i].symbol() != eqs[i-1].symbol());
      }
      max_rank = ranks[eqs.size()-1];
    }

    template <bool rexact>
    boolean_expression evaluate(boolean_expression expr, size_t r, const std::vector<boolean_expression> approx)
    {
      if (is_true(expr) || is_false(expr))
      {
        return expr;
      }
      if (is_boolean_variable(expr))
      {
        auto i = indices[atermpp::down_cast<boolean_variable>(expr)];
        if (rexact)
        {
          if (ranks[i] == r)
          {
            return approx[i];
          }
          return expr;
        }
        else
        {
          if (ranks[i] >= r)
          {
            return approx[i];
          }
          return expr;
        }
      }
      using accessors::left;
      using accessors::right;
      if (is_and(expr))
      {
        auto eval_left = evaluate<rexact>(left(expr), r, approx);
        if (is_false(eval_left))
        {
          return false_();
        }
        auto eval_right = evaluate<rexact>(right(expr), r, approx);
        if (is_false(eval_right))
        {
          return false_();
        }
        if (is_true(eval_left))
        {
          return eval_right;
        }
        if (is_true(eval_right))
        {
          return eval_left;
        }
        return and_(eval_left, eval_right);
      }
      if (is_or(expr))
      {
        auto eval_left = evaluate<rexact>(left(expr), r, approx);
        if (is_true(eval_left))
        {
          return true_();
        }
        auto eval_right = evaluate<rexact>(right(expr), r, approx);
        if (is_true(eval_right))
        {
          return true_();
        }
        if (is_false(eval_left))
        {
          return eval_right;
        }
        if (is_false(eval_right))
        {
          return eval_left;
        }
        return and_(eval_left, eval_right);
      }
      return expr;
    }

    bool run(const boolean_variable& first_variable)
    {
      auto eqs = m_bes.equations();
      std::vector<boolean_expression> approx(eqs.size());
      for (size_t i = 0; i < eqs.size(); i++)
      {
        if (eqs[i].symbol().is_nu() || is_true(eqs[i].formula()))
        {
          approx[i] = true_();
        }
        else
        {
          approx[i] = false_();
        }
      }
      for (auto r = max_rank; r >= 0; r--)
      {
        // Find the fix point of all equations of rank r

        std::unordered_set<size_t> todo;
        for (size_t i = 0; i < eqs.size(); i++)
        {
          if (ranks[i] == r)
          {
            todo.insert(i);
          }
        }

        while (!todo.empty())
        {
          auto i = todo.begin();
          auto t = evaluate<false>(eqs[*i].formula(), ranks[*i], approx);
          if (t == eqs[*i])
          {
            todo.erase(i);
          }
          else
          {
            approx[*i] = t;
          }
        }

        for (size_t i = 0; i < eqs.size(); i++)
        {
          eqs[i] = evaluate<true>(eqs[i].formula(), ranks[i], approx);
        }
      }
      return is_true(eqs[0].formula());
    }
};

inline
bool local_fixpoints(boolean_equation_system& b)
{
  boolean_variable first = b.equations().front().variable();
  make_standard_form(b, true);
  local_fixpoints_algorithm algorithm(b);
  return algorithm.run(first);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_LOCAL_FIXPOINTS_H
