// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/bdd_prover.h
/// \brief EQ-BDD based prover for mCRL2 boolean data expressions

#ifndef MCRL2_DATA_DETAIL_BDD_PROVER_H
#define MCRL2_DATA_DETAIL_BDD_PROVER_H

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/prover/bdd_path_eliminator.h"
#include "mcrl2/data/detail/prover/bdd_simplifier.h"
#include "mcrl2/data/detail/prover/induction.h"
#include "mcrl2/data/detail/prover/manipulator.h"
#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/data/detail/prover/utilities.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/** \brief A prover that uses EQ-BDDs.
 *
 * \detail
 * A class based on the Prover class that takes an expression of sort
 * Bool in internal mCRL2 format and creates the corresponding EQ-BDD.
 * Using this EQ-BDD, the class can determine if the original
 * formula is a tautology or a contradiction. The term "formula" in
 * the following text denotes arbitrary expressions of sort Bool in
 * the mCRL2 format.
 *
 * A prover uses a rewriter to rewrite parts of the formulas it
 * manipulates. The constructor BDD_Prover::BDD_Prover initializes the
 * prover's rewriter with the data equations in internal mCRL2 format
 * contained in the LPS passed as parameter a_lps and the rewrite
 * strategy passed as parameter a_rewrite_strategy. The parameter
 * a_rewrite_strategy can be set to either
 * GS_REWR_data::jitty or GS_REWR_data::jittyC. To limit the
 * number of seconds spent on proving a single formula, a time limit
 * can be set. If the time limit is set to 0, no time limit will be
 * enforced. The parameter a_apply_induction indicates whether or
 * induction on lists is applied. The constructor
 * BDD_Prover::BDD_Prover has two additional parameters,
 * a_path_eliminator and a_solver_type. The parameter
 * a_path_eliminator can be used to enable the use of an instance of
 * the class BDD_Path_Eliminator. Instances of this class use an SMT
 * solver to eliminate inconsistent paths from BDDs. The parameter
 * a_solver_type can be used to indicate which SMT solver should be
 * used for this task. Either the SMT solver ario
 * (http://www.eecs.umich.edu/~ario/) or cvc-lite
 * (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one of
 * these solvers, the directory containing the corresponding
 * executable must be in the path. If the parameter a_path_eliminator
 * is set to false, the parameter a_solver_type is ignored and no
 * instance of the class BDD_Path_Eliminator is initialized.
 *
 * The formula to be handled is set using the method
 * Prover::set_formula inherited from the class Prover. An entity of
 * the class BDD_Prover uses binary decision diagrams to determine if
 * a given formula is a tautology or a contradiction. The resulting
 * BDD can be retreived using the method BDD_Prover::get_bdd.
 *
 * The methods BDD_Prover::is_tautology and
 * BDD_Prover::is_contradiction indicate whether or not a formula is a
 * tautology or a contradiction. These methods will return answer_yes,
 * answer_no or answer_undefined. If a formula is neither a tautology
 * nor a contradiction according to the prover, a so called witness or
 * counter example can be returned by the methods
 * BDD_Prover::get_witness and BDD_Prover::get_counter_example. A
 * witness is a valuation for which the formula holds, a counter
 * example is a valuation for which it does not hold.
*/

enum Answer
{
  answer_yes,
  answer_no,
  answer_undefined
};


// class BDD_Prover: public Prover
class BDD_Prover: protected rewriter
{
  public:
    typedef rewriter::substitution_type substitution_type;

  protected:
    /// \brief An expression of sort Bool.
    data_expression f_formula;

    /// \brief A class that can be used to manipulate expressions.
    Manipulator f_manipulator;

    /// \brief A class that provides information about expressions.
    Info f_info;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula has been processed.
    bool f_processed = false;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula is a tautology.
    Answer f_tautology;

    /// \brief A flag that indicates whether or not the formala Prover::f_formula is a contradiction.
    Answer f_contradiction;

    /// \brief An integer representing the maximal amount of seconds to be spent on processing a formula.
    int f_time_limit;

    /// \brief A timestamp representing the moment when the maximal amount of seconds has been spent on processing the current formula.
    time_t f_deadline;


  private:

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    bool f_reverse = true;

    /// \brief Flag indicating whether or not the arguments of equality functions are taken into account
    /// \brief when determining the order of expressions.
    bool f_full = false;

    /// \brief A flag indicating whether or not induction on lists is applied.
    bool f_apply_induction;

    /// \brief A data specification.
    // const data_specification& f_data_spec;

    /// \brief A hashtable that maps formulas to BDDs.
    /// \brief If the BDD of a formula is unknown, it maps this formula to 0.
    std::map < data_expression, data_expression > f_formula_to_bdd;

    /// \brief A hashtable that maps formulas to the smallest guard occuring in those formulas.
    /// \brief If the smallest guard of a formula is unknown, it maps this formula to 0.
    std::map < data_expression, data_expression > f_smallest;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that simplifies a BDD.
    BDD_Simplifier* f_bdd_simplifier;

    /// \brief Class that creates all statements needed to prove a given property using induction.
    Induction f_induction;

    /// \brief Constructs the EQ-BDD corresponding to the formula Prover::f_formula.
    void build_bdd()
    {
      f_deadline = time(nullptr) + f_time_limit;

      data_expression v_previous_1;
      data_expression v_previous_2;

      mCRL2log(log::debug) << "Formula: " << f_formula << std::endl;

      data_expression intermediate_bdd = f_formula;

      intermediate_bdd = m_rewriter->rewrite(intermediate_bdd,bdd_sigma);
      intermediate_bdd = f_manipulator.orient(intermediate_bdd);

      mCRL2log(log::debug) << "Formula rewritten and oriented: " << intermediate_bdd << std::endl;

      while (v_previous_1 != intermediate_bdd && v_previous_2 != intermediate_bdd)
      {
        v_previous_2 = v_previous_1;
        v_previous_1 = intermediate_bdd;
        intermediate_bdd = bdd_down(intermediate_bdd);
        mCRL2log(log::debug) << "End of iteration." << std::endl;
        mCRL2log(log::debug) << "Intermediate BDD: " << intermediate_bdd << std::endl;
      }

      f_bdd = intermediate_bdd;
      mCRL2log(log::debug) << "Resulting BDD: " << f_bdd << std::endl;

    }

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    data_expression bdd_down(const data_expression& a_formula)
    {
      std::string indent;

      return bdd_down(a_formula, indent);
    }

    /// \brief Creates the EQ-BDD corresponding to the formula a_formula.
    data_expression bdd_down(const data_expression& a_formula, const std::string& a_indent)
    {

      if (f_time_limit != 0 && (f_deadline - time(nullptr)) <= 0)
      {
        mCRL2log(log::debug) << "The time limit has passed." << std::endl;
        return a_formula;
      }

      if (a_formula==sort_bool::true_())
      {
        return a_formula;
      }
      if (a_formula==sort_bool::false_())
      {
        return a_formula;
      }

      if (is_abstraction(a_formula))
      {
        const abstraction& a = atermpp::down_cast<abstraction>(a_formula);
        return abstraction(a.binding_operator(), a.variables(), bdd_down(a.body(), a_indent));
      }

      const std::map < data_expression, data_expression >::const_iterator i = f_formula_to_bdd.find(a_formula);
      if (i!=f_formula_to_bdd.end()) // found
      {
        return i->second;
      }

      data_expression v_guard = smallest(a_formula);
      if (v_guard==data_expression())
      {
        return a_formula;
      }
      else
      {
        mCRL2log(log::debug) << a_indent << "Smallest guard: " << v_guard << std::endl;
      }

      const std::string extra_indent = a_indent + "  ";

      data_expression v_term1 = f_manipulator.set_true(a_formula, v_guard);
      v_term1 = m_rewriter->rewrite(v_term1,bdd_sigma);
      v_term1 = f_manipulator.orient(v_term1);
      mCRL2log(log::debug) << extra_indent << "True-branch after rewriting and orienting: " << v_term1 << std::endl;
      v_term1 = bdd_down(v_term1, extra_indent);
      mCRL2log(log::debug) << extra_indent << "BDD of the true-branch: " << v_term1 << std::endl;

      data_expression v_term2 = f_manipulator.set_false(a_formula, v_guard);
      v_term2 = m_rewriter->rewrite(v_term2,bdd_sigma);
      v_term2 = f_manipulator.orient(v_term2);
      mCRL2log(log::debug) << extra_indent << "False-branch after rewriting and orienting: " << v_term2 << std::endl;
      v_term2 = bdd_down(v_term2, extra_indent);
      mCRL2log(log::debug) << extra_indent << "BDD of the false-branch: " << v_term2 << std::endl;

      data_expression v_bdd = f_manipulator.make_reduced_if_then_else(v_guard, v_term1, v_term2);
      f_formula_to_bdd[a_formula]=v_bdd;

      return v_bdd;
    }

    /// \brief Removes all inconsistent paths from the BDD BDD_Prover::f_bdd.
    void eliminate_paths()
    {
      time_t v_new_time_limit;

      v_new_time_limit = f_deadline - time(nullptr);
      if (v_new_time_limit > 0 || f_time_limit == 0)
      {
        mCRL2log(log::debug) << "Simplifying the BDD:" << std::endl;
        f_bdd_simplifier->set_time_limit((std::max)(v_new_time_limit, time(nullptr)));
        f_bdd = f_bdd_simplifier->simplify(f_bdd);
        mCRL2log(log::debug) << "Resulting BDD: " << f_bdd << std::endl;
      }
    }

    /// \brief Updates the values of Prover::f_tautology and Prover::f_contradiction.
    void update_answers()
    {
      if (!f_processed)
      {
        build_bdd();
        eliminate_paths();
        data_expression v_original_formula = f_formula;
        data_expression v_original_bdd = f_bdd;
        if (f_apply_induction && !(f_bdd_info.is_true(f_bdd) || f_bdd_info.is_false(f_bdd)))
        {
          f_induction.initialize(v_original_formula);
          while (f_induction.can_apply_induction() && !f_bdd_info.is_true(f_bdd))
          {
            mCRL2log(log::debug) << "Applying induction." << std::endl;
            f_formula = f_induction.apply_induction();
            build_bdd();
            eliminate_paths();
          }
          if (f_bdd_info.is_true(f_bdd))
          {
            f_tautology = answer_yes;
            f_contradiction = answer_no;
          }
          else
          {
            v_original_formula = sort_bool::not_(v_original_formula);
            f_bdd = v_original_bdd;
            f_induction.initialize(v_original_formula);
            while (f_induction.can_apply_induction() && !f_bdd_info.is_true(f_bdd))
            {
              mCRL2log(log::debug) << "Applying induction on the negated formula." << std::endl;
              f_formula = f_induction.apply_induction();
              build_bdd();
              eliminate_paths();
            }
            if (f_bdd_info.is_true(f_bdd))
            {
              f_bdd = sort_bool::false_();
              f_tautology = answer_no;
              f_contradiction = answer_yes;
            }
            else
            {
              f_bdd = v_original_bdd;
              f_tautology = answer_undefined;
              f_contradiction = answer_undefined;
            }
          }
        }
        else
        {
          if (f_bdd_info.is_true(f_bdd))
          {
            f_tautology = answer_yes;
            f_contradiction = answer_no;
          }
          else if (f_bdd_info.is_false(f_bdd))
          {
            f_tautology = answer_no;
            f_contradiction = answer_yes;
          }
          else
          {
            f_tautology = answer_undefined;
            f_contradiction = answer_undefined;
          }
        }
        f_processed = true;
      }
    };

    /// \brief Returns the smallest guard in the formula a_formula.
    data_expression smallest(data_expression a_formula)
    {
      if (is_variable(a_formula))
      {
        if (a_formula.sort()==sort_bool::bool_())
        {
          return a_formula;
        }
        else
        {
          return data_expression();
        }
      }
      if (a_formula==sort_bool::true_() || a_formula==sort_bool::false_())
      {
        return data_expression();
      }
      if (is_abstraction(a_formula))
      {
        // Guards from within an abstraction may contain
        // variables that are not bound outside that abstraction.
        // Therefore, we never return a smallest guard from
        // within an abstraction.
        return data_expression();
      }

      const std::map < data_expression, data_expression >::const_iterator i = f_smallest.find(a_formula);
      if (i!=f_smallest.end()) //found
      {
        return i->second;
      }

      data_expression v_result;

      std::size_t v_length = f_info.get_number_of_arguments(a_formula);

      for (std::size_t s = 0; s < v_length; s++)
      {
        const data_expression v_small = smallest(f_info.get_argument(a_formula, s));
        if (v_small!=data_expression())
        {
          if (v_result!=data_expression())
          {
            if (f_info.compare_guard(v_small, v_result) == compare_result_smaller)
            {
              v_result = v_small;
            }
          }
          else
          {
            v_result = v_small;
          }
        }
      }
      if (v_result==data_expression() && a_formula.sort()==sort_bool::bool_())
      {
        v_result = a_formula;
      }
      if (v_result!=data_expression())
      {
        f_smallest[a_formula]=v_result;
      }

      return v_result;
    }

    /// \brief Returns branch of the BDD a_bdd, depending on the polarity a_polarity.
    data_expression get_branch(const data_expression& a_bdd, const bool a_polarity)
    {
      data_expression v_result;

      if (f_bdd_info.is_if_then_else(a_bdd))
      {
        data_expression v_guard = f_bdd_info.get_guard(a_bdd);
        data_expression v_true_branch = f_bdd_info.get_true_branch(a_bdd);
        data_expression v_false_branch = f_bdd_info.get_false_branch(a_bdd);
        data_expression v_branch = get_branch(v_true_branch, a_polarity);
        if (v_branch==data_expression())
        {
          v_branch = get_branch(v_false_branch, a_polarity);
          if (v_branch==data_expression())
          {
            v_result = data_expression();
          }
          else
          {
            data_expression v_term = sort_bool::not_(v_guard);
            v_result = lazy::and_(v_branch, v_term);
          }
        }
        else
        {
          v_result = lazy::and_(v_branch, v_guard);
        }
      }
      else
      {
        if ((f_bdd_info.is_true(a_bdd) && a_polarity) || (f_bdd_info.is_false(a_bdd) && !a_polarity))
        {
          v_result = sort_bool::true_();
        }
        else
        {
          v_result = data_expression();
        }
      }
      return v_result;
    }

  protected:

    /// \brief A binary decision diagram in the internal representation of the rewriter.
    substitution_type bdd_sigma;

    /// \brief A binary decision diagram in the internal representation of mCRL2.
    data_expression f_bdd;
  public:

    /// \brief Constructor that initializes the attributes BDD_Prover::f_data_spec, Prover::f_time_limit and
    /// \brief BDD_Prover::f_bdd_simplifier.
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    /// precondition: the argument passed as parameter a_lps is an LPS
    /* BDD_Prover(
      const data_specification& data_spec,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::rewriter::data::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false
    ); */

    BDD_Prover(
      const data_specification& data_spec,
      const used_data_equation_selector& equations_selector,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      smt_solver_type a_solver_type = solver_type_cvc,
      bool a_apply_induction = false)
    : rewriter(data_spec, equations_selector, a_rewrite_strategy)
    , f_manipulator(f_info)
    , f_bdd_simplifier(a_path_eliminator ? new BDD_Path_Eliminator(a_solver_type) : new BDD_Simplifier())
    , f_induction(data_spec)
    {
      f_time_limit = a_time_limit;

      switch (a_rewrite_strategy)
      {
        case(jitty):
#ifdef MCRL2_JITTYC_AVAILABLE
        case(jitty_compiling):
#endif
        {
          /* These provers are ok */
          break;
        }
        case(jitty_prover):
#ifdef MCRL2_JITTYC_AVAILABLE
        case(jitty_compiling_prover):
#endif
        {
          throw mcrl2::runtime_error("The proving rewriters are not supported by the prover (only jitty and jittyc are supported).");
        }
        default:
        {
          throw mcrl2::runtime_error("Unknown type of rewriter.");
          break;
        }
      }

      f_apply_induction = a_apply_induction;
      f_info.set_reverse(f_reverse);
      f_info.set_full(f_full);
      mCRL2log(log::debug) << "Flags:" << std::endl
                      << "  Reverse: " << bool_to_char_string(f_reverse) << "," << std::endl
                      << "  Full: " << bool_to_char_string(f_full) << "," << std::endl;
    }

    /// \brief Destructor that destroys the BDD simplifier BDD_Prover::f_bdd_simplifier.
    ~BDD_Prover()
    {
      delete f_bdd_simplifier;
      f_bdd_simplifier = nullptr;
    }

    /// \brief Set the substitution to be used to construct the BDD
    void set_substitution(substitution_type& sigma)
    {
      bdd_sigma = sigma;
    }

    /// \brief Set the substitution in internal format to be used to construct the BDD
    void set_substitution_internal(substitution_type& sigma)
    {
      bdd_sigma = sigma;
    }

    /// \brief Indicates whether or not the formula Prover::f_formula is a tautology.
    Answer is_tautology()
    {
      update_answers();
      return f_tautology;
    }

    /// \brief Indicates whether or not the formula Prover::f_formula is a contradiction.
    Answer is_contradiction()
    {
      update_answers();
      return f_contradiction;
    }

    /// \brief Returns the BDD BDD_Prover::f_bdd.
    data_expression get_bdd()
    {
      update_answers();
      return f_bdd;
    }

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "true", if such a leaf exists.
    data_expression get_witness()
    {
      update_answers();
      if (is_contradiction() == answer_yes)
      {
        mCRL2log(log::debug) << "The formula is a contradiction." << std::endl;
        return sort_bool::true_();
      }
      else if (is_tautology() == answer_yes)
      {
        mCRL2log(log::debug) << "The formula is a tautology." << std::endl;
        return sort_bool::false_();
      }
      else
      {
        mCRL2log(log::debug) << "The formula is satisfiable, but not a tautology." << std::endl;
        data_expression t=get_branch(f_bdd, true);
        if (t==data_expression())
        { throw mcrl2::runtime_error(
            "Cannot provide witness. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.");
        }
        return t;
      }
    }

    /// \brief Returns all the guards on a path in the BDD that leads to a leaf labelled "false", if such a leaf exists.
    data_expression get_counter_example()
    {
      update_answers();
      if (is_contradiction() == answer_yes)
      {
        mCRL2log(log::debug) << "The formula is a contradiction." << std::endl;
        return sort_bool::false_();
      }
      else if (is_tautology() == answer_yes)
      {
        mCRL2log(log::debug) << "The formula is a tautology." << std::endl;
        return sort_bool::true_();
      }
      else
      {
        mCRL2log(log::debug) << "The formula is satisfiable, but not a tautology." << std::endl;
        data_expression t=get_branch(f_bdd, false);
        if (t==data_expression())
        { throw mcrl2::runtime_error(
            "Cannot provide counter example. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.");
        }
        return t;
      }
    }

    /// \brief Returns the rewriter used by this prover (i.e. it returns Prover::f_rewriter).
    std::shared_ptr<detail::Rewriter> get_rewriter()
    {
      return m_rewriter;
    }

    /// \brief Sets Prover::f_formula to a_formula.
    /// precondition: the argument passed as parameter a_formula is an expression of sort Bool
    void set_formula(const data_expression& a_formula)
    {
      f_formula = a_formula;
      f_processed = false;
      mCRL2log(log::debug) << "The formula has been set." << std::endl;
    }
  

};
} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
