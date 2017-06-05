// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/induction.h
/// \brief Proving with induction on lists

#ifndef MCRL2_DATA_DETAIL_PROVER_INDUCTION_H
#define MCRL2_DATA_DETAIL_PROVER_INDUCTION_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/prover/bdd_info.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2
{
namespace data
{
namespace detail
{
/// The class Induction generates statements corresponding to

class Induction
{
  private:
    /// \brief The smallest number x for which no variable named "dummyx" exists.
    // size_t f_fresh_dummy_number;
    set_identifier_generator fresh_identifier_generator;

    /// \brief The number of variables used during the last application of induction.
    size_t f_count;

    /// \brief An expression of sort Bool in mCRL2 format.
    data_expression f_formula;

    /// \brief
    function_symbol_list f_constructors;

    /// \brief
    core::identifier_string f_cons_name;

    /// \brief
    std::vector < variable >  f_list_variables;

    /// \brief
    std::map < variable, sort_expression > f_lists_to_sorts;

    /// \brief
    // BDD_Info f_bdd_info;

    /// \brief
    void recurse_expression_for_lists(const data_expression& a_expression)
    {
      if (is_variable(a_expression))
      {
        const sort_expression v_sort = a_expression.sort();
        if (sort_list::is_list(v_sort))  // the sort has shape List(D) for some sort D.
        {
          if (std::find(f_list_variables.begin(), f_list_variables.end(),a_expression)==f_list_variables.end()) // not found
          {
            f_list_variables.push_back(atermpp::down_cast<variable>(a_expression));
          }
        }
      }
      else if (is_application(a_expression))
      {
        const data::application& a = atermpp::down_cast<const data::application>(a_expression);
        for (application::const_iterator i = a.begin(); i != a.end(); ++i)
        {
          recurse_expression_for_lists(*i);
        }
      }
    }

    /// \brief
    void map_lists_to_sorts()
    {
      for(std::vector < variable >::const_iterator it=f_list_variables.begin(); it!=f_list_variables.end(); ++it)
      {
        const variable v_list_variable = *it;
        const sort_expression v_sort = get_sort_of_list_elements(v_list_variable);
        f_lists_to_sorts[v_list_variable]=v_sort;
      }
    }

    /// \brief
    sort_expression get_sort_of_list_elements(const variable& a_list_variable)
    {
      function_symbol_list v_constructors;
      function_symbol v_constructor;
      core::identifier_string v_constructor_name;
      sort_expression v_constructor_sort;
      sort_expression v_constructor_element_sort;
      sort_expression v_list_sort;
      sort_expression v_result;

      v_constructors = f_constructors;
      v_list_sort = a_list_variable.sort();
      while (!v_constructors.empty())
      {
        v_constructor = v_constructors.front();
        v_constructors.pop_front();
        v_constructor_name = v_constructor.name();
        if (v_constructor_name == f_cons_name)
        {
          v_constructor_sort = v_constructor.sort();
          v_constructor_element_sort = *(function_sort(v_constructor_sort).domain().begin());
          v_constructor_sort = *(++(function_sort(v_constructor_sort).domain().begin()));
          if (v_constructor_sort == v_list_sort)
          {
            v_result = v_constructor_element_sort;
          }
        }
      }

      return v_result;
    }

    /// \brief
    variable get_fresh_dummy(const sort_expression& a_sort)
    {
      return variable(fresh_identifier_generator("dummy$"),a_sort);
    }


    /// \brief
    data_expression apply_induction_one()
    {
      const variable v_induction_variable = f_list_variables.front();

      const sort_expression v_dummy_sort = get_sort_of_list_elements(v_induction_variable);
      const variable v_dummy_variable = get_fresh_dummy(v_dummy_sort);

      mutable_map_substitution<> v_substitution1;
      assert(sort_list::is_list(v_induction_variable.sort()));
      v_substitution1[v_induction_variable]=sort_list::empty(atermpp::down_cast<container_sort>(v_induction_variable.sort()).element_sort());
      std::set<variable> variables_occurring_in_rhs_sigma;
      const data_expression v_base_case = data::replace_variables_capture_avoiding(f_formula,v_substitution1,variables_occurring_in_rhs_sigma);

      mutable_map_substitution<> v_substitution2;
      v_substitution2[v_induction_variable]=sort_list::cons_(v_dummy_variable.sort(), v_dummy_variable, v_induction_variable);
      variables_occurring_in_rhs_sigma.clear();
      variables_occurring_in_rhs_sigma.insert(v_dummy_variable);
      variables_occurring_in_rhs_sigma.insert(v_induction_variable);
      const data_expression v_induction_step = sort_bool::implies(f_formula, data::replace_variables_capture_avoiding(f_formula,v_substitution2,variables_occurring_in_rhs_sigma));

      return sort_bool::and_(v_base_case, v_induction_step);
    }

    /// \brief
    data_expression create_hypotheses(
                        const data_expression& a_hypothesis,
                        variable_list a_list_of_variables,
                        variable_list a_list_of_dummies)
    {
      if (a_list_of_variables.empty())
      {
        return sort_bool::true_();
      }
      else
      {
        data_expression v_clause = a_hypothesis;
        if (a_list_of_variables.size() > 1)
        {
          while (!a_list_of_variables.size())
          {
            const variable v_variable(a_list_of_variables.front());
            a_list_of_variables.pop_front();
            const variable v_dummy(a_list_of_dummies.front());
            a_list_of_dummies.pop_front();
            
            mutable_map_substitution<> v_substitution;
            v_substitution[v_variable]=sort_list::cons_(v_dummy.sort(), v_dummy, v_variable);
            std::set<variable> variables_occurring_in_rhs_sigma;
            variables_occurring_in_rhs_sigma.insert(v_dummy);
            variables_occurring_in_rhs_sigma.insert(v_variable);
            v_clause = sort_bool::and_(v_clause, data::replace_variables_capture_avoiding(a_hypothesis,v_substitution,variables_occurring_in_rhs_sigma));
          }
        }

        return v_clause;
      }
    }

    /// \brief
    data_expression_list create_clauses(const data_expression& a_formula,
                             const data_expression& a_hypothesis,
                             const size_t a_variable_number,
                             const size_t a_number_of_variables,
                             const variable_list& a_list_of_variables,
                             const variable_list& a_list_of_dummies)
    {
      const variable v_variable = f_list_variables[a_variable_number];
      variable_list v_list_of_variables = a_list_of_variables;
      v_list_of_variables.push_front(v_variable);
      const sort_expression v_dummy_sort = get_sort_of_list_elements(v_variable);
      const variable v_dummy = get_fresh_dummy(v_dummy_sort);
      variable_list v_list_of_dummies = a_list_of_dummies;
      v_list_of_dummies.push_front(v_dummy);

      mutable_map_substitution<> v_substitution1;
      v_substitution1[v_variable]=sort_list::cons_(v_dummy.sort(), v_dummy, v_variable);
      std::set<variable> variables_occurring_in_rhs_sigma;
      variables_occurring_in_rhs_sigma.insert(v_dummy);
      variables_occurring_in_rhs_sigma.insert(v_variable);
      const data_expression v_formula_1 = data::replace_variables_capture_avoiding(a_formula,v_substitution1,variables_occurring_in_rhs_sigma);

      mutable_map_substitution<> v_substitution2;
      assert(sort_list::is_list(v_variable.sort()));
      v_substitution2[v_variable]=sort_list::empty(atermpp::down_cast<container_sort>(v_variable.sort()).element_sort());
      variables_occurring_in_rhs_sigma.clear();
      const data_expression v_formula_2 = data::replace_variables_capture_avoiding(a_formula,v_substitution2,variables_occurring_in_rhs_sigma);
      const data_expression v_hypothesis = data::replace_variables_capture_avoiding(a_hypothesis,v_substitution2,variables_occurring_in_rhs_sigma);

      if (a_variable_number < a_number_of_variables - 1)
      {
        const data_expression_list v_list_1 = create_clauses(v_formula_1, a_hypothesis, a_variable_number + 1, a_number_of_variables, v_list_of_variables, v_list_of_dummies);
        const data_expression_list v_list_2 = create_clauses(v_formula_2, v_hypothesis, a_variable_number + 1, a_number_of_variables, a_list_of_variables, a_list_of_dummies);
        return v_list_1+v_list_2;
      }
      else
      {
        data_expression v_hypotheses_1 = create_hypotheses(a_hypothesis, v_list_of_variables, v_list_of_dummies);
        data_expression v_hypotheses_2 = create_hypotheses(v_hypothesis, a_list_of_variables, a_list_of_dummies);
        return data_expression_list(sort_bool::implies(v_hypotheses_1, v_formula_1))+
                   data_expression_list(sort_bool::implies(v_hypotheses_2, v_formula_2));
      }
    }

  public:
    /// \brief
    Induction(const data_specification& a_data_spec)
    {
      f_constructors=function_symbol_list(a_data_spec.constructors().begin(),a_data_spec.constructors().end());
      f_cons_name = sort_list::cons_name();
    }

    /// \brief
    ~Induction()
    {
    }

    /// \brief
    void initialize(const data_expression& a_formula)
    {
      f_formula = a_formula;
      f_list_variables.clear();
      recurse_expression_for_lists(a_formula);
      map_lists_to_sorts();
      f_count = 0;
    }

    /// \brief
    bool can_apply_induction()
    {
      if (f_list_variables.size() == f_count)
      {
        return false;
      }
      else
      {
        f_count++;
        return true;
      }
    }

    /// \brief
    data_expression apply_induction()
    {
      data_expression v_result;

      if (f_count == 1)
      {
        mCRL2log(log::verbose) << "Induction on one variable." << std::endl;
        v_result = apply_induction_one();
      }
      else
      {
        mCRL2log(log::verbose) << "Induction on " << f_count << " variables." << std::endl;
        data_expression_list v_list_of_clauses = create_clauses(f_formula, f_formula, 0, f_count, variable_list(), variable_list());
        v_result = v_list_of_clauses.front();
        v_list_of_clauses.pop_front();
        while (!v_list_of_clauses.empty())
        {
          data_expression v_clause(v_list_of_clauses.front());
          v_list_of_clauses.pop_front();
          v_result = sort_bool::and_(v_result, v_clause);
        }
      }
      return v_result;
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
