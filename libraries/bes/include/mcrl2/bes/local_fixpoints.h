// Author(s): Xiao Qi, Jan Friso Groote
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
#include "mcrl2/utilities/logger.h"
#include "mcrl2/bes/normal_forms.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/find.h"
#include "mcrl2/bes/to_bdd.h"
#include "mcrl2/bes/justification.h"

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
      const std::vector<boolean_equation>& eqs = b.equations();
      for (size_t i = 1; i < eqs.size(); i++)
      {
        indices[eqs[i].variable()] = i;
        ranks[i] = ranks[i-1] + (eqs[i].symbol() != eqs[i-1].symbol());
      }
      max_rank = ranks[eqs.size()-1];
    }

  protected:  
    // evaluate expr by substituting all variables with their approximations.
    // If on_rank is true, only variables on rank r are substituted
    template <bool on_rank_only>
    boolean_expression evaluate(boolean_expression expr, size_t r, const std::vector<boolean_expression>& approx)
    {
      if (is_true(expr) || is_false(expr))
      {
        return expr;
      }
      if (is_boolean_variable(expr))
      {
        size_t i = indices[atermpp::down_cast<boolean_variable>(expr)];
        assert(ranks[i]<=r);
        if (!on_rank_only || ranks[i] == r)
        {
          return approx[i];
        }
        return expr;
      }
      if (is_and(expr))
      {
        const and_& expra=atermpp::down_cast<and_>(expr);
        const boolean_expression eval_left = evaluate<on_rank_only>(expra.left(), r, approx);
        if (is_false(eval_left))
        {
          return false_();
        }
        const boolean_expression eval_right = evaluate<on_rank_only>(expra.right(), r, approx);
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
        const or_& expro=atermpp::down_cast<or_>(expr);
        const boolean_expression eval_left = evaluate<on_rank_only>(expro.left(), r, approx);
        if (is_true(eval_left))
        {
          return true_();
        }
        const boolean_expression eval_right = evaluate<on_rank_only>(expro.right(), r, approx);
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
        return or_(eval_left, eval_right);
      }
      return expr;
    }

    void add_to_occurrence_set(size_t i, const boolean_expression& expr, vector < set <size_t> >& occurrence_set)
    {
      if (is_boolean_variable(expr))
      {
        size_t j = indices[atermpp::down_cast<boolean_variable>(expr)];
        occurrence_set[j].insert(i);
        return;
      }
      if (is_and(expr))
      {
        const and_& expra=atermpp::down_cast<and_>(expr);
        add_to_occurrence_set(i, expra.left(), occurrence_set);
        add_to_occurrence_set(i, expra.right(), occurrence_set);
        return;
      }
      if (is_or(expr))
      {
        const or_& expro=atermpp::down_cast<or_>(expr);
        add_to_occurrence_set(i, expro.left(), occurrence_set);
        add_to_occurrence_set(i, expro.right(), occurrence_set);
        return;
      }
      assert(is_true(expr)||is_false(expr));
    }

    vector < set < size_t> > create_occurrence_set(const std::vector<boolean_equation>& eqs, const size_t current_rank)
    {
      vector < set <size_t> > result(eqs.size());
      for(size_t i=0; i<eqs.size(); ++i)
      {
        if (ranks[i]==current_rank)
        {
          add_to_occurrence_set(i,eqs[i].formula(),result);
        }
      }
      return result;
    }

  public:
    bool run(std::vector<bool>* full_solution)
    {
      mCRL2log(mcrl2::log::verbose) << "Solving a BES with " << m_bes.equations().size() <<
              " equations using the local fixed point algorithm." << std::endl;

      std::vector<boolean_equation> eqs = m_bes.equations();
      std::vector<boolean_expression> approx(eqs.size());
      for (size_t i = 0; i < eqs.size(); i++)
      {
        approx[i] = eqs[i].variable();
      }

      for (size_t current_rank=max_rank+1; current_rank>0 ; )
      {
        current_rank--;
        mCRL2log(mcrl2::log::verbose) << "Solving equations of rank " << current_rank << "." << std::endl;
        for (size_t i = 0; i < eqs.size(); i++)
        {
          if (ranks[i]==current_rank)
          { 
            if (eqs[i].symbol().is_nu())
            {
               approx[i] = true_();
            }
            else
            {
               approx[i] = false_();
            } 
          }
        }
        // Find the fixpoint of all equations of the current_rank
        std::stack<size_t> todo;

        for (size_t v = 0; v < eqs.size(); v++)
        {
          if (ranks[v] == current_rank)
          {
            const boolean_expression t = evaluate<true>(eqs[v].formula(), current_rank, approx);
            if (!bdd_equal(t, approx[v]))
            {
              approx[v] = t;
              todo.push(v);
            }
          }
        }

        // For each variable with index i the occurrence_sets[i] contain the set indices of equations where i occurs in 
        // the right hand side.
        vector < set < size_t> > occurrence_sets=create_occurrence_set(eqs,current_rank);
     
        while (!todo.empty())
        {
          const size_t i=todo.top();
          todo.pop();
          for (size_t v: occurrence_sets[i])
          { 
            assert(ranks[v] == current_rank);
            const boolean_expression t = evaluate<true>(eqs[v].formula(), current_rank, approx);
            if (!bdd_equal(t, approx[v]))
            {
              approx[v] = t;
              todo.push(v);
            }
          }
        }

        /* substitute the stable solution for the current rank in all other
           equations. */

        for (size_t i = 0; i < eqs.size(); i++)
        {
          if (ranks[i] == current_rank)
          {
            eqs[i].formula() = approx[i];
          }
          else
          {
            eqs[i].formula() = evaluate<false>(eqs[i].formula(), current_rank, approx);
          }
        }
      }

      if (full_solution)
      {
        full_solution->resize(eqs.size());
        for (size_t i = 0; i < eqs.size(); i++)
        {
          assert(is_true(eqs[i].formula()) || is_false(eqs[i].formula()));
          (*full_solution)[i] = is_true(eqs[i].formula());
        }
      }

      for (size_t i = 0; i < eqs.size(); i++)
      {
        approx[i]=eqs[i].formula();
      }
      boolean_expression init_value = evaluate<false>(m_bes.initial_state(), -1, approx);
      assert(is_true(init_value) || is_false(init_value));
      return is_true(init_value); 
    }
};

inline
bool local_fixpoints(boolean_equation_system& b, std::vector<bool> *full_solution = 0)
{
  local_fixpoints_algorithm algorithm(b);
  return algorithm.run(full_solution);
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_LOCAL_FIXPOINTS_H
