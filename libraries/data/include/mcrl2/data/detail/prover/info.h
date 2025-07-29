// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/info.h
/// \brief Interface to classes Info

#ifndef MCRL2_DATA_DETAIL_PROVER_INFO_H
#define MCRL2_DATA_DETAIL_PROVER_INFO_H

#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2::data::detail
{

enum Compare_Result
{
  compare_result_smaller,
  compare_result_equal,
  compare_result_bigger
};

/// \brief Base class for classes that provide information about the structure of
/// \brief data expressions in one of the internal formats of the rewriter.
class Info
{
  protected:

    /// \brief Flag indicating whether or not the arguments of equality functions are taken into account
    /// \brief when determining the order of expressions.
    const bool f_full;

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    const bool f_reverse;

    inline Compare_Result lexico(
              const Compare_Result& a_result1,
              const Compare_Result& a_result2) const
    {
      return (a_result1 != compare_result_equal) ? a_result1 : a_result2;
    }

    Compare_Result compare_address(
              const atermpp::aterm& term1,
              const atermpp::aterm& term2) const
    {
      if (term1 < term2)
      {
        return compare_result_smaller;
      }
      if (term2 < term1)
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

        /// \brief Returns an integer corresponding to the structure of the guard passed as argument \c guard.
    int get_guard_structure(const data_expression& guard) const
    {
      if (is_variable(guard))
      {
        return 0;
      }
      if (is_equal_to_application(guard))
      {
        const application& guard_appl=atermpp::down_cast<application>(guard);
        const data_expression& v_term1 = guard_appl[0]; 
        const data_expression& v_term2 = guard_appl[1]; 
        if (find_free_variables(v_term1).empty() && is_variable(v_term2))
        {
          return 1;
        }
        if (is_variable(v_term1) && is_variable(v_term2))
        {
          return 2;
        }
        return 3;
      }
      return 4;
    }

    /// \brief Compares the structure of two guards.
    Compare_Result compare_guard_structure(const data_expression& guard1, const data_expression& guard2) const
    {
      if (get_guard_structure(guard1) < get_guard_structure(guard2))
      {
        return compare_result_smaller;
      }
      if (get_guard_structure(guard1) > get_guard_structure(guard2))
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    /// \brief Compares two guards by their arguments.
    Compare_Result compare_guard_equality(const data_expression& guard1, const data_expression& guard2) const
    {
      if (f_full && is_equal_to_application(guard1) && is_equal_to_application(guard2))
      {
        const application& guard1_appl=atermpp::down_cast<application>(guard1);
        const application& guard2_appl=atermpp::down_cast<application>(guard2);
        const data_expression& v_g1a0 = guard1_appl[0];
        const data_expression& v_g1a1 = guard1_appl[1];
        const data_expression& v_g2a0 = guard2_appl[0];
        const data_expression& v_g2a1 = guard2_appl[1];
        if (f_reverse)
        {
          return lexico(compare_term(v_g1a1, v_g2a1), compare_term(v_g1a0, v_g2a0));
        }
        else
        {
          return lexico(compare_term(v_g1a0, v_g2a0), compare_term(v_g1a1, v_g2a1));
        }
      }
      return compare_result_equal;
    }

    Compare_Result compare_term_free_variables(const data_expression& term1, const data_expression& term2) const
    {
      bool term1_is_closed = find_free_variables(term1).empty();
      bool term2_is_closed = find_free_variables(term2).empty();
      if (term1_is_closed && !term2_is_closed)
      {
        return compare_result_smaller;
      }
      if (!term1_is_closed && term2_is_closed)
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    /// \brief Compares terms by their type.
    Compare_Result compare_term_type(const data_expression& term1, const data_expression& term2) const
    {
      if (is_variable(term1) && !is_variable(term2))
      {
        return compare_result_bigger;
      }
      if (!is_variable(term1) && is_variable(term2))
      {
        return compare_result_smaller;
      }
      return compare_result_equal;
    }

    /// \brief Compares terms by checking whether one is a part of the other.
    Compare_Result compare_term_occurs(const data_expression& term1, const data_expression& term2) const
    {
      if (occurs(term1, term2))
      {
        return compare_result_bigger;
      }
      if (occurs(term2, term1))
      {
        return compare_result_smaller;
      }
      return compare_result_equal;
    }

  public:
    /// \brief Constructor that initializes the rewriter.
    constexpr Info(bool a_full, bool a_reverse)
    : f_full(a_full)
    , f_reverse(a_reverse)
    {}

    // Perform an occur check of expression t2 in expression t1.
    static bool occurs(const data_expression& t1, const data_expression& t2)
    {
      return atermpp::find_if(t1,[&](const atermpp::aterm& t){return t == t2;}) != atermpp::aterm();
    }

    /// \brief Compares two guards.
    Compare_Result compare_guard(const data_expression& guard1, const data_expression& guard2) const
    {
      return lexico(
               lexico(
                 compare_guard_structure(guard1, guard2),
                 compare_guard_equality(guard1, guard2)
               ),
               compare_address(guard1, guard2)
             );
    }

    /// \brief Compares two terms.
    Compare_Result compare_term(const data_expression& term1, const data_expression& term2) const
    {
      return lexico(
                lexico(
                  lexico(
                    compare_term_free_variables(term1, term2),
                    compare_term_occurs(term1, term2)
                  ),
                  compare_term_type(term1, term2)
                ),
                compare_address(term1, term2)
             );
    }

    /// \brief Returns the number of arguments of the main operator of a term.
    /// \param a_term An expression in the internal format of the rewriter with the jitty strategy.
    /// \return 0, if \c aterm is a constant or a variable.
    ///         The number of arguments of the main operator, otherwise.
    std::size_t get_number_of_arguments(const data_expression& a_term) const
    {
      if (!is_variable(a_term) && !is_function_symbol(a_term) && !is_abstraction(a_term) && !is_machine_number(a_term))
      {
        return atermpp::down_cast<application>(a_term).size();
      }
      else
      {
        return 0;
      }
    }

    /// \brief Returns the main operator of the term \c term;
    data_expression get_operator(const data_expression& term) const
    {
      if (is_function_symbol(term))
      {
        return term;
      }
      if (is_abstraction(term))
      {
        return get_operator(atermpp::down_cast<abstraction>(term).body());
      }
      const application& a = atermpp::down_cast<application>(term);
      return get_operator(a.head());
    }
};

} // namespace mcrl2::data::detail

#endif
