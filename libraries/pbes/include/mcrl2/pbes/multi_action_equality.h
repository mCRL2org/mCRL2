// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/multi_action_equality.h
/// \brief Functions for determining equality of multi actions.

#ifndef MCRL2_PBES_MULTI_ACTION_EQUALITY_H
#define MCRL2_PBES_MULTI_ACTION_EQUALITY_H

#include <algorithm>
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/modal_formula/state_formula_rename.h"
#include "mcrl2/modal_formula/free_variables.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/detail/algorithm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/pbes_translate_impl.h"
#include "mcrl2/data/xyz_identifier_generator.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {
    /// Precondition: The range [first, last[ contains sorted arrays.
    /// Visits all permutations of the arrays, and calls f for
    /// each instance.
    template <typename Iter, typename Function>
    void forall_permutations(Iter first, Iter last, Function f)
    {
      if (first == last)
      {
        f();
        return;
      }
      Iter next = first;
      ++next;
      forall_permutations(next, last, f);
      while (std::next_permutation(first->first, first->second))
      {
        forall_permutations(next, last, f);
      }
    }
    
    /// Returns true if the actions in a and b have the same names, and the same sorts.
    /// \pre a and b are sorted w.r.t. to the names of the actions.
    bool equal_action_signatures(const std::vector<lps::action>& a, const std::vector<lps::action>& b)
    {
      if(a.size() != b.size())
      {
        return false;
      }
      std::vector<lps::action>::const_iterator i, j;
      for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
      {
        if (i->label().name() != j->label().name() || i->label().sorts() != j->label().sorts())
          return false;
      }
      return true;
    }
    
    struct compare_action_name
    {
      bool operator()(const lps::action& a, const lps::action& b) const
      {
        return a.label().name() < b.label().name();
      }
    };
    
    /// Used for building an expression for the comparison of data parameters.
    struct equal_data_parameters_builder
    {
      const std::vector<lps::action>& a;
      const std::vector<lps::action>& b;
      atermpp::vector<pbes_expression>& result;
    
      equal_data_parameters_builder(const std::vector<lps::action>& a_,
                                    const std::vector<lps::action>& b_,
                                    atermpp::vector<pbes_expression>& result_
                                   )
        : a(a_),
          b(b_),
          result(result_)
      {}
    
      /// Adds the expression 'a == b' to result.
      void operator()()
      {
        using namespace pbes_expr_optimized;
        namespace d = data::data_expr;
    
        atermpp::vector<pbes_expression> v;
        std::vector<lps::action>::const_iterator i, j;
        for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
        {
          data::data_expression_list d1 = i->arguments();
          data::data_expression_list d2 = j->arguments();
          assert(d1.size() == d2.size());
          data::data_expression_list::iterator i1, i2;
          for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
          {
            v.push_back(d::equal_to(*i1, *i2));
          }
        }
        result.push_back(join_and(v.begin(), v.end()));
      }
    };

    /// Used for building an expression for the comparison of data parameters.
    struct not_equal_multi_actions_builder
    {
      const std::vector<lps::action>& a;
      const std::vector<lps::action>& b;
      atermpp::vector<pbes_expression>& result;
    
      not_equal_multi_actions_builder(const std::vector<lps::action>& a_,
                                        const std::vector<lps::action>& b_,
                                        atermpp::vector<pbes_expression>& result_
                                       )
        : a(a_),
          b(b_),
          result(result_)
      {}
    
      /// Adds the expression 'a == b' to result.
      void operator()()
      {
        using namespace pbes_expr_optimized;
        namespace d = data::data_expr;
    
        atermpp::vector<pbes_expression> v;
        std::vector<lps::action>::const_iterator i, j;
        for (i = a.begin(), j = b.begin(); i != a.end(); ++i, ++j)
        {
          data::data_expression_list d1 = i->arguments();
          data::data_expression_list d2 = j->arguments();
          assert(d1.size() == d2.size());
          data::data_expression_list::iterator i1, i2;
          for (i1 = d1.begin(), i2 = d2.begin(); i1 != d1.end(); ++i1, ++i2)
          {
            v.push_back(d::not_equal_to(*i1, *i2));
          }
        }
        result.push_back(join_or(v.begin(), v.end()));
      }
    };

} // namespace detail
    
    /// Returns a pbes expression that expresses under which conditions the
    /// multi actions a and b are equal.
    pbes_expression equal_multi_actions(lps::action_list a, lps::action_list b)
    {
      using namespace pbes_expr_optimized;
    
      // make copies of a and b and sort them
      std::vector<lps::action> va(a.begin(), a.end()); // protection not needed
      std::vector<lps::action> vb(b.begin(), b.end()); // protection not needed
      std::sort(va.begin(), va.end(), detail::compare_action_name());
      std::sort(vb.begin(), vb.end(), detail::compare_action_name());
    
      if (!detail::equal_action_signatures(va, vb))
      {
        return false_();
      }
    
      // compute the intervals of a with equal names
      typedef std::vector<lps::action>::iterator action_iterator;
      std::vector<std::pair<action_iterator, action_iterator> > intervals;
      action_iterator first = va.begin();
      while (first != va.end())
      {
        action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_action_name());
        intervals.push_back(std::make_pair(first, next));
        first = next;
      }
      atermpp::vector<pbes_expression> z;
      detail::equal_data_parameters_builder f(va, vb, z);
      detail::forall_permutations(intervals.begin(), intervals.end(), f);
      pbes_expression result = join_or(z.begin(), z.end());
      return result;
    }
    
    /// Returns a pbes expression that expresses under which conditions the
    /// multi actions a and b are not equal.
    pbes_expression not_equal_multi_actions(lps::action_list a, lps::action_list b)
    {
      using namespace pbes_expr_optimized;
    
      // make copies of a and b and sort them
      std::vector<lps::action> va(a.begin(), a.end());
      std::vector<lps::action> vb(b.begin(), b.end());
      std::sort(va.begin(), va.end(), detail::compare_action_name());
      std::sort(vb.begin(), vb.end(), detail::compare_action_name());
    
      if (!detail::equal_action_signatures(va, vb))
        return true_();
    
      // compute the intervals of a with equal names
      typedef std::vector<lps::action>::iterator action_iterator;
      std::vector<std::pair<action_iterator, action_iterator> > intervals;
      action_iterator first = va.begin();
      while (first != va.end())
      {
        action_iterator next = std::upper_bound(first, va.end(), *first, detail::compare_action_name());
        intervals.push_back(std::make_pair(first, next));
        first = next;
      }
      atermpp::vector<pbes_expression> z;
      detail::not_equal_multi_actions_builder f(va, vb, z);
      detail::forall_permutations(intervals.begin(), intervals.end(), f);
      pbes_expression result = join_and(z.begin(), z.end());
      return result;
    }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_MULTI_ACTION_EQUALITY_H
