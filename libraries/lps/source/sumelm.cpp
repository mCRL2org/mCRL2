// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsbinary.cpp
/// \brief

//Aterms
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/aterm_list.h>
#include <mcrl2/atermpp/table.h>
#include <mcrl2/atermpp/algorithm.h>

//LPS Framework
#include "mcrl2/lps/linear_process.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"

#include "mcrl2/lps/sumelm.h"

// For Aterm library extension functions
using namespace mcrl2::core;
using namespace mcrl2::data;
using namespace mcrl2::lps;

namespace mcrl2 {

namespace lps {

////////////////////////////////////////////////////////////////
// Auxiliary functions used to support sumelm operation
// Some of these might be interesting to get into the LPS library

  /// \internal
  struct sumelm_replace_helper
  {
    const std::map<data_expression, data_expression>& m_replacements;

    sumelm_replace_helper(const std::map<data_expression, data_expression>& replacements)
      : m_replacements(replacements)
    {}

    std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
    {
      if (is_sort_expression(t))
      {
        return std::pair<atermpp::aterm_appl, bool>(t, false); // do not continue the recursion
      }
      else if (is_data_expression(t))
      {
        std::map<data_expression, data_expression>::const_iterator i = m_replacements.find(t);
        if (i == m_replacements.end())
        {
          return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
        }
        else
        {
          return std::pair<atermpp::aterm_appl, bool>(i->second, false); // do not continue the recursion
        }
      }
      else
      {
        return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
      }
    }
  };

  /// Replaces all data expressions in the term t using the specified map of replacements.
  ///
  template <typename Term>
  static inline
  Term sumelm_replace(Term t, const std::map<data_expression, data_expression>& replacements)
  {
    return atermpp::partial_replace(t, sumelm_replace_helper(replacements));
  }

  /// Replaces all data expressions in the right hand sides of the list, using
  /// the specified map of replacements.
  ///
  static inline
  assignment_vector sumelm_assignment_list_replace(const assignment_list& t,
                         const std::map<data_expression, data_expression>& replacements)
  {
    assignment_vector result;

    for (assignment_list::const_iterator i = t.begin(); i != t.end(); ++i)
    {
      result.push_back(assignment(i->lhs(), sumelm_replace(i->rhs(), replacements)));
    }

    return result;
  }

  /// Adds replacement lhs := rhs to the specified map of replacements.
  /// All replacements that have lhs as a right hand side will be changed to
  /// have rhs as a right hand side.
  static inline
  void sumelm_add_replacement(std::map<data_expression, data_expression>& replacements,
                         const data_expression lhs, const data_expression rhs)
  {
    // First apply already present substitutions to rhs
    data_expression new_rhs = sumelm_replace(rhs, replacements);
    // Use 1pt-map lhs:=new_rhs to substitute in the righthandsides
    std::map<data_expression, data_expression> tmp_subst;
    tmp_subst[lhs] = new_rhs;
    for (std::map<data_expression, data_expression>::iterator i = replacements.begin(); i != replacements.end(); ++i)
    {
      i->second = sumelm_replace(i->second, tmp_subst);
    }
    replacements[lhs] = new_rhs;
  }

  ///\pre is_and(t) || is_equal_to(t)
  ///\return lefthandside of t
  static inline
  data_expression lhs(const data_expression t)
  {
    assert(sort_bool_::is_and__application(t) || is_equal_to_application(t));
    return *(application(t).arguments().begin());
  }

  ///\pre is_and(t) || is_equal_to(t)
  ///\return righthandside of t
  static inline
  data_expression rhs(const data_expression t)
  {
    assert(sort_bool_::is_and__application(t) || is_equal_to_application(t));
    return *(++application(t).arguments().begin());
  }

  ///\pre is_equal_to(t); t is of form a == b
  ///\return b == a
  static inline
  data_expression swap_equality(const data_expression t)
  {
    assert(is_equal_to_application(t));
    return data::equal_to(rhs(t), lhs(t));
  }

  ///Apply substitution to the righthand sides of the assignments in dl
  static inline
  assignment_vector substitute_rhs(const assignment_list& dl, const assignment& substitution)
  {
    assignment_vector result;

    for(assignment_list::const_iterator i = dl.begin(); i != dl.end(); ++i)
    {
      result.push_back(assignment(i->lhs(), substitute(substitution, i->rhs())));
    }

    return result;
  }

  ////////////////////////////////////////////////////////////
  // Functions for sumelm operations
  //

  ///Apply a simple form of sum elimination in the case a summation
  ///variable does not occur within the summand at all
  static inline
  lps::summand remove_unused_variables(const lps::summand& summand_)
  {
    struct local {
      static bool is_variable(atermpp::aterm p) {
        return data_expression(p).is_variable();
      }
    };

    //gsVerboseMsg("Summand: %s\n", pp(summand_).c_str());
    int num_removed = 0;
    lps::summand new_summand;
    // New summation variable list, all variables in this list occur in other terms in the summand.
    variable_vector new_summation_variables;

    // Construct a set with all variables occurring in the summand.
    // This reduces the running time from O(|summand| * |summation variables|)
    // to O(|summand| + |summation variables|)
    atermpp::set<data_expression> occurring_vars;
    partial_find_all_if(summand_.condition(), boost::bind(&local::is_variable, _1), is_sort_expression, std::inserter(occurring_vars, occurring_vars.end()));
    partial_find_all_if(summand_.actions(), boost::bind(&local::is_variable, _1), is_sort_expression, std::inserter(occurring_vars, occurring_vars.end()));
    partial_find_all_if(summand_.time(), boost::bind(&local::is_variable, _1), is_sort_expression, std::inserter(occurring_vars, occurring_vars.end()));

    assignment_list assignments(summand_.assignments());

    for (assignment_list::const_iterator i(assignments.begin()); i != assignments.end(); ++i)
    {
      partial_find_all_if(*i, boost::bind(&local::is_variable, _1), is_sort_expression, std::inserter(occurring_vars, occurring_vars.end()));
    }

    variable_vector summation_variables(data::make_variable_vector(summand_.summation_variables()));

    for (variable_vector::const_iterator i = summation_variables.begin();
                                       i != summation_variables.end(); ++i)
    {
      //Check whether variable occurs in other terms of summand
      //If variable occurs leave the variable, so add variable to new list
      if (occurring_vars.find(*i) != occurring_vars.end())
      {
        new_summation_variables.push_back(*i);
      } else {
        ++num_removed;
      }
      //else remove the variable, i.e. do not add it to the new list (skip)
    }

    new_summand = set_summation_variables(summand_, data::make_variable_list(new_summation_variables));
    gsVerboseMsg("Removed %d summation variables\n", num_removed);

    return new_summand;
  }

  //Recursively apply sum elimination on a summand.
  //We build up a list of substitutions that need to be made in substitutions
  //the caller of this function needs to apply substitutions to the summand
  //once we exit recursion
  //working_condition is a parameter that we use to split up the problem,
  //at the first call of this function working_condition == summand_->condition()
  //should hold.
  //The new condition is built up on the return path of the recursion, so
  //the last exit of the recursion is the new condition of the summand.
  //!!!INTERNAL USE ONLY!!!
  static inline
  data_expression recursive_substitute_equalities(const summand& summand_,
                                                  data_expression working_condition,
                                                  std::map<data_expression, data_expression>& substitutions)
  {
    // In all cases not explicitly handled we return the original working_condition
    data_expression result = working_condition;

    if (sort_bool_::is_and__application(working_condition))
    {
      //Recursively apply sum elimination on lhs and rhs
      //Note that recursive application provides for progress because lhs and rhs split the working condition.
      data_expression a,b;
      a = recursive_substitute_equalities(summand_, lhs(working_condition), substitutions);
      b = recursive_substitute_equalities(summand_, rhs(working_condition), substitutions);
      result = lazy::and_(a,b);
    }

    else if (is_equal_to_application(working_condition))
    {
      //Check if rhs is a variable, if so, swap lhs and rhs, so that the following code
      //is always the same.
      if (!lhs(working_condition).is_variable() && rhs(working_condition).is_variable() &&
          data::search_variable(summand_.summation_variables(), rhs(working_condition)))
      {
        working_condition = swap_equality(working_condition);
      }

      //If lhs is a variable, check if it occurs in the summation variables, if so
      //apply substitution lhs := rhs in actions, time and assignments.
      //substitution in condition is accounted for on return path of recursion,
      //substitution in summation_variables is done in calling function.
      if (lhs(working_condition).is_variable())
      {
        if (data::search_variable(summand_.summation_variables(), variable(lhs(working_condition))) &&
            !search_data_expression(rhs(working_condition), lhs(working_condition)))
        {
          if (substitutions.count(lhs(working_condition)) == 0)
          {
            // apply all previously added substitutions to the rhs.
            sumelm_add_replacement(substitutions, lhs(working_condition), rhs(working_condition));
            result = sort_bool_::true_();
          } else if (rhs(working_condition).is_variable() &&
                     data::search_variable(summand_.summation_variables(), variable(rhs(working_condition)))) {
            // check whether the converse is possible
            if (substitutions.count(rhs(working_condition)) == 0) {
              sumelm_add_replacement(substitutions, rhs(working_condition), substitutions[lhs(working_condition)]);
              result = sort_bool_::true_();
            }
          } else if (substitutions.count(substitutions[lhs(working_condition)]) == 0 &&
                       substitutions[lhs(working_condition)].is_variable() &&
                       data::search_variable(summand_.summation_variables(), variable(substitutions[lhs(working_condition)]))) {
            sumelm_add_replacement(substitutions, substitutions[lhs(working_condition)], rhs(working_condition));
            sumelm_add_replacement(substitutions, lhs(working_condition), rhs(working_condition));
            result = sort_bool_::true_();
          }
        }
      }
    }
    return result;
  }

  ///Apply sum elimination on the summand consisting of
  ///summation_variables, condition, actions, time and assignments.
  ///This checks the following:
  ///X(..) = sum d. d=e -> a(..) . X(..)
  ///and returns X(..) = e -> a(..) . X(..)
  static inline
  lps::summand substitute_equalities(const lps::summand& summand_)
  {
    lps::summand new_summand = summand_;

    //Apply elimination and store result
    std::map<data_expression, data_expression> substitutions;
    data_expression new_condition = recursive_substitute_equalities(new_summand, new_summand.condition(), substitutions);

    //Apply the substitutions that were returned from the recursive call
    new_summand = summand(new_summand.summation_variables(),
                              sumelm_replace(new_condition, substitutions),
                              new_summand.is_delta(),
                              sumelm_replace(new_summand.actions(), substitutions),
                              sumelm_replace(new_summand.time(), substitutions),
                              data::make_assignment_list(sumelm_assignment_list_replace(new_summand.assignments(), substitutions)));
    //Take the summand with substitution, and remove the summation variables that are now not needed
    new_summand = remove_unused_variables(new_summand);
    return new_summand;
  }


  ///Take an lps specification, apply equality sum elimination to it,
  ///and return an lps specification
  static inline
  lps::specification substitute_equalities_(const lps::specification& specification)
  {
    gsVerboseMsg("Substituting equality conditions in summands\n");
    lps::linear_process lps = specification.process();

    lps::specification new_specification;
    lps::summand_list new_summand_list;

    int index = 0;
    // Apply sum elimination on each of the summands in the summand_ list.
    for (lps::summand_list::const_iterator i = lps.summands().begin(); i != lps.summands().end(); ++i)
    {
      gsVerboseMsg("Summand %d: ", ++index);

      new_summand_list = push_front(new_summand_list, substitute_equalities(*i));
    }
    new_summand_list = reverse(new_summand_list);

    new_specification = specification;
    new_specification.process() = set_summands(lps, new_summand_list);
    return new_specification;
  }

  ///Returns an LPS specification in which the timed arguments have been rewritten
  lps::specification sumelm(const lps::specification& specification)
  {
    gsVerboseMsg("Applying sum elimination on an LPS of %d summands\n", specification.process().summands().size());

    lps::specification new_specification = specification;
    new_specification = substitute_equalities_(new_specification); // new_specification used for future concerns, possibly disabling substitute_equalities_
    return new_specification;
  }

} // namespace lps

} // namespace mcrl2
