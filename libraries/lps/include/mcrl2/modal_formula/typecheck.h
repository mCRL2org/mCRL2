// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TYPECHECK_H
#define MCRL2_MODAL_FORMULA_TYPECHECK_H

#include "mcrl2/lps/typecheck.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace state_formulas
{


class state_formula_type_checker : lps::multi_action_type_checker
{

  public:
    state_formula_type_checker(const data::data_specification &data_spec, const lps::action_label_list& action_decls);
        /** \brief     Type check a state formula.
         *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
         *  \param[in] d A state formula that has not been type checked.
         *  \param[in] check_monotonicity Check whether the formula is monotonic, in the sense that no fixed point
         *             variable occurs in the scope of an odd number of negations.
         *  \return    a state formula where all untyped identifiers have been replace by typed ones.
         **/
     state_formula  operator()(const state_formula &d, bool check_monotonicity = true);

  protected:
    state_formula TraverseStateFrm(
                const std::map<core::identifier_string,data::sort_expression> &Vars,
                const std::map<core::identifier_string,data::sort_expression_list> &StateVars,                       
                const state_formula &StateFrm);
    regular_formulas::regular_formula TraverseRegFrm(
                const std::map<core::identifier_string,data::sort_expression> &Vars, 
                const regular_formulas::regular_formula &RegFrm);
    action_formulas::action_formula TraverseActFrm(
             const std::map<core::identifier_string,data::sort_expression> &Vars, 
             const action_formulas::action_formula &ActFrm);

};

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] formula A state formula that has not been type checked.
 *  \post      formula is type checked.
 **/
inline
void type_check(state_formula& formula, const lps::specification& lps_spec, bool check_monotonicity = true)
{
  try 
  {
    state_formula_type_checker type_checker(lps_spec.data(),lps_spec.action_labels());
    formula=type_checker(formula,check_monotonicity);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula TEMPORARILY IN ATERM FORMAT" +  
                             mcrl2::utilities::to_string(formula) /* pp(formula)*/);
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
