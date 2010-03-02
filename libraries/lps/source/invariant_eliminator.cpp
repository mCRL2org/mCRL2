// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/invariant_eliminator.cpp
/// \brief Add your file description here.

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/lps/invariant_eliminator.h"

using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

// Class Invariant_Eliminator ---------------------------------------------------------------------
  // Class Invariant_Eliminator - Functions declared private --------------------------------------

    mcrl2::lps::summand Invariant_Eliminator::simplify_summand(
          const summand a_summand,
          const data_expression a_invariant,
          const bool a_no_elimination,
          const int a_summand_number)
    { 
      const data_expression v_condition = a_summand.condition();
      const data_expression v_formula = lazy::and_(a_invariant, v_condition);

      if (a_no_elimination)
      { return summand(a_summand.summation_variables(),
                            v_formula,
                            a_summand.is_delta(),
                            a_summand.actions(),
                            a_summand.time(),
                            a_summand.assignments());
      }
      
      f_bdd_prover.set_formula((ATermAppl)v_formula);
      if (f_bdd_prover.is_contradiction() == answer_yes) 
      {
        return summand(variable_list(),sort_bool::false_(),deadlock(sort_real::real_(0)));
      } 
      else 
      {
        if (f_simplify_all) 
        { 
          gsMessage("Summand number %d is simplified.\n", a_summand_number);
          return summand(a_summand.summation_variables(),
                            data_expression(f_bdd_prover.get_bdd()),
                            a_summand.is_delta(),
                            a_summand.actions(),
                            a_summand.time(),
                            a_summand.assignments());
        }
        return a_summand;
      }
    }

  // Class Invariant_Eliminator - Functions declared public ---------------------------------------

    Invariant_Eliminator::Invariant_Eliminator(
             const mcrl2::lps::specification & a_lps,
             const mcrl2::data::rewriter::strategy a_rewrite_strategy /* = mcrl2::data::rewriter::jitty */,
             const int a_time_limit /* = 0 */,
             const bool a_path_eliminator /* = false */,
             const mcrl2::data::detail::SMT_Solver_Type a_solver_type /* = mcrl2::data::detail::solver_type_ario */,
             const bool a_apply_induction /* = false */,
             const bool a_simplify_all /* = false */):
       f_bdd_prover(a_lps.data(), a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction),
       f_lps(a_lps),
       f_simplify_all(a_simplify_all)
    {
    }

    // --------------------------------------------------------------------------------------------

    Invariant_Eliminator::~Invariant_Eliminator() 
    {}

    // --------------------------------------------------------------------------------------------

    specification Invariant_Eliminator::simplify(
              const mcrl2::data::data_expression a_invariant,
              const bool a_no_elimination,
              const int a_summand_number)
    {
      const summand_list v_summands = f_lps.process().summands();
      summand_list v_simplified_summands;
      int v_summand_number = 1;

      for(summand_list::const_iterator i=v_summands.begin();
             i!=v_summands.end(); ++i) 
      {
        summand v_summand = *i;
        if ((a_summand_number == v_summand_number) || (a_summand_number == 0)) 
        { 
          v_summand = simplify_summand(v_summand, a_invariant, a_no_elimination, v_summand_number);
          if ((v_summand.condition() != sort_bool::false_())||a_no_elimination) 
          { 
            v_simplified_summands = push_front(v_simplified_summands, v_summand);
            if (!a_no_elimination)
            { gsVerboseMsg("Summand number %d could not be eliminated.\n", v_summand_number);
            }
          } 
          else 
          {
            gsMessage("Summand number %d is eliminated.\n", v_summand_number);
          }
        }
        v_summand_number++;
      }
      v_simplified_summands = reverse(v_simplified_summands);
      linear_process v_process = f_lps.process();
      v_process.set_summands(v_simplified_summands);
      return specification(f_lps.data(),f_lps.action_labels(),f_lps.global_variables(),v_process, f_lps.initial_process());
    }
