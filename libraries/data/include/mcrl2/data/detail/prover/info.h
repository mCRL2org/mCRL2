// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/data/detail/prover/utilities.h"
#include "mcrl2/data/rewriter.h"

namespace mcrl2
{
namespace data
{
namespace detail
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
    bool f_full;

    /// \brief Flag indicating whether or not the result of the comparison between the first two arguments
    /// \brief weighs stronger than the result of the comparison between the second pair of arguments of an
    /// \brief equation, when determining the order of expressions.
    bool f_reverse;

    Compare_Result lexico(
              const Compare_Result& a_result1,
              const Compare_Result& a_result2)
    {
      return (a_result1 != compare_result_equal) ? a_result1 : a_result2;
    }

    Compare_Result compare_address(
              const atermpp::aterm& a_term1,
              const atermpp::aterm& a_term2)
    {
      if (a_term1 < a_term2)
      {
        return compare_result_smaller;
      }
      if (a_term2 < a_term1)
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    bool alpha1(const data_expression& a_term1, const data_expression& a_term2, std::size_t a_number)
    {
      if (get_number_of_arguments(a_term1) == a_number)
      {
        return false;
      }
      else
      {
        data_expression v_term = get_argument(a_term1, a_number);
        return (v_term == a_term2) || lpo1(v_term, a_term2) || alpha1(a_term1, a_term2, ++a_number);
      }
    }

    bool beta1(const data_expression& a_term1, const data_expression& a_term2)
    {
      const atermpp::aterm v_operator_1 = get_operator(a_term1);
      const atermpp::aterm v_operator_2 = get_operator(a_term2);
      return (compare_address(v_operator_1, v_operator_2) == compare_result_bigger) && majo1(a_term1, a_term2, 0);
    }

    class equals // A simple class containing an equality predicate.
    {
      private:
        const atermpp::aterm_appl m_t;

      public:
        // constructor
        equals(const data_expression& t1):
          m_t(t1)
        {}

        bool operator()(const atermpp::aterm_appl& t) const
        {
          return (t==m_t);
        }
    };

    static bool occurs(const data_expression& t1, const data_expression& t2)
    {
      return atermpp::find_if(t1,equals(t2))!=atermpp::aterm_appl();
    }

    bool gamma1(const data_expression& a_term1, const data_expression& a_term2)
    {
      const atermpp::aterm v_operator_1 = get_operator(a_term1);
      const atermpp::aterm v_operator_2 = get_operator(a_term2);
      return (v_operator_1 == v_operator_2) && lex1(a_term1, a_term2, 0) && majo1(a_term1, a_term2, 0);
    }

    bool majo1(const data_expression& a_term1, const data_expression& a_term2, std::size_t a_number)
    {
      if (get_number_of_arguments(a_term2) == a_number)
      {
        return true;
      }
      else
      {
        data_expression v_term = get_argument(a_term2, a_number);
        return lpo1(a_term1, v_term) && majo1(a_term1, a_term2, ++a_number);
      }
    }

    bool lex1(const data_expression& a_term1, const data_expression& a_term2, std::size_t a_number)
    {
      if (get_number_of_arguments(a_term1) == a_number)
      {
        return false;
      }
      else
      {
        data_expression v_term1 = get_argument(a_term1, a_number);
        data_expression v_term2 = get_argument(a_term2, a_number);
        if (v_term1 == v_term2)
        {
          return lex1(a_term1, a_term2, ++a_number);
        }
        else
        {
          return lpo1(v_term1, v_term2);
        }
      }
    }

    /// \brief Returns an integer corresponding to the structure of the guard passed as argument \c a_guard.
    int get_guard_structure(const data_expression& a_guard)
    {
      if (is_variable(a_guard))
      {
        return 0;
      }
      if (is_equal_to_application(a_guard))
      {
        data_expression v_term1, v_term2;

        v_term1 = get_argument(a_guard, 0);
        v_term2 = get_argument(a_guard, 1);
        if (is_variable(v_term1) && is_variable(v_term2))
        {
          return 1;
        }
        return 2;
      }
      return 3;
    }

    /// \brief Compares the structure of two guards.
    Compare_Result compare_guard_structure(const data_expression& a_guard1, const data_expression& a_guard2)
    {
      if (get_guard_structure(a_guard1) < get_guard_structure(a_guard2))
      {
        return compare_result_smaller;
      }
      if (get_guard_structure(a_guard1) > get_guard_structure(a_guard2))
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    /// \brief Compares two guards by their arguments.
    Compare_Result compare_guard_equality(const data_expression& a_guard1, const data_expression& a_guard2)
    {
      if (f_full && is_equal_to_application(a_guard1) && is_equal_to_application(a_guard2))
      {
        data_expression v_g1a0, v_g1a1, v_g2a0, v_g2a1;

        v_g1a0 = get_argument(a_guard1, 0);
        v_g1a1 = get_argument(a_guard1, 1);
        v_g2a0 = get_argument(a_guard2, 0);
        v_g2a1 = get_argument(a_guard2, 1);
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

    /// \brief Compares terms by their type.
    Compare_Result compare_term_type(const data_expression& a_term1, const data_expression& a_term2)
    {
      if (is_variable(a_term1) && !is_variable(a_term2))
      {
        return compare_result_bigger;
      }
      if (!is_variable(a_term1) && is_variable(a_term2))
      {
        return compare_result_smaller;
      }
      return compare_result_equal;
    }

    /// \brief Compares terms by checking whether one is a part of the other.
    Compare_Result compare_term_occurs(const data_expression& a_term1, const data_expression& a_term2)
    {
      if (occurs(a_term1, a_term2))
      {
        return compare_result_smaller;
      }
      if (occurs(a_term2, a_term1))
      {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

  public:
    /// \brief Constructor that initializes the rewriter.
    Info()
    {
    }

    /// \brief Destructor with no particular functionality.
    ~Info()
    {}

    /// \brief Sets the flag Info::f_reverse.
    void set_reverse(bool a_reverse)
    {
      f_reverse = a_reverse;
    }

    /// \brief Sets the flag Info::f_full.
    void set_full(bool a_bool)
    {
      f_full = a_bool;
    }

    /// \brief Compares two guards.
    Compare_Result compare_guard(const data_expression& a_guard1, const data_expression& a_guard2)
    {
      return lexico(
               lexico(
                 compare_guard_structure(a_guard1, a_guard2),
                 compare_guard_equality(a_guard1, a_guard2)
               ),
               compare_address(a_guard1, a_guard2)
             );
    }

    /// \brief Compares two terms.
    Compare_Result compare_term(const data_expression& a_term1, const data_expression& a_term2)
    {
      return lexico(
               lexico(
                 compare_term_occurs(a_term1, a_term2),
                 compare_term_type(a_term1, a_term2)
               ),
               compare_address(a_term1, a_term2)
             );
    }

    /// \brief Compares two terms using lpo.
    bool lpo1(const data_expression& a_term1, const data_expression& a_term2)
    {
      if (is_variable(a_term1) && is_variable(a_term2))
      {
        return compare_address(a_term1, a_term2) == compare_result_bigger;
      }
      else if (is_variable(a_term1))
      {
        return false;
      }
      else if (is_variable(a_term2))
      {
        return occurs(a_term2, a_term1);
      }
      else
      {
        return alpha1(a_term1, a_term2, 0) || beta1(a_term1, a_term2) || gamma1(a_term1, a_term2);
      }
    }

    /// \brief Indicates whether or not a term has type bool.
    /// \brief Indicates whether or not a term has type bool.
    /* bool has_type_bool(const data_expression& a_term)
    {
      return a_term.sort()==sort_bool::bool_();
    } */

    /// \brief Returns the number of arguments of the main operator of a term.
    /// \param a_term An expression in the internal format of the rewriter with the jitty strategy.
    /// \return 0, if \c aterm is a constant or a variable.
    ///         The number of arguments of the main operator, otherwise.
    std::size_t get_number_of_arguments(const data_expression& a_term)
    {
      if (!is_variable(a_term) && !is_function_symbol(a_term) && !is_abstraction(a_term))
      {
        return a_term.size() - 1;
      }
      else
      {
        return 0;
      }
    }

    /// \brief Returns the main operator of the term \c a_term;
    data_expression get_operator(const data_expression& a_term)
    {
      if (is_function_symbol(a_term))
      {
        return a_term;
      }
      if (is_abstraction(a_term))
      {
        return get_operator(atermpp::down_cast<abstraction>(a_term).body());
      }
      const application& a = atermpp::down_cast<application>(a_term);
      return get_operator(a.head());
    }

    /// \brief Returns the argument with number \c a_number of the main operator of term \c a_term.
    data_expression get_argument(const data_expression& a_term, const std::size_t a_number)
    {
      return data_expression(a_term[a_number + 1]);
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
