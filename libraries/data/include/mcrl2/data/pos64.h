// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/pos64.h
/// \brief The standard sort pos64.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/pos64.spec.

#ifndef MCRL2_DATA_POS64_H
#define MCRL2_DATA_POS64_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/machine_word.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort pos64
    namespace sort_pos64 {

      inline
      core::identifier_string const& pos64_name()
      {
        static core::identifier_string pos64_name = core::identifier_string("Pos64");
        return pos64_name;
      }

      /// \brief Constructor for sort expression Pos64
      /// \return Sort expression Pos64
      inline
      basic_sort const& pos64()
      {
        static basic_sort pos64 = basic_sort(pos64_name());
        return pos64;
      }

      /// \brief Recogniser for sort expression Pos64
      /// \param e A sort expression
      /// \return true iff e == pos64()
      inline
      bool is_pos64(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == pos64();
        }
        return false;
      }


      /// \brief Generate identifier \@one64
      /// \return Identifier \@one64
      inline
      core::identifier_string const& one64_name()
      {
        static core::identifier_string one64_name = core::identifier_string("@one64");
        return one64_name;
      }

      /// \brief Constructor for function symbol \@one64
      
      /// \return Function symbol one64
      inline
      function_symbol const& one64()
      {
        static function_symbol one64(one64_name(), pos64());
        return one64;
      }

      /// \brief Recogniser for function \@one64
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@one64
      inline
      bool is_one64_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == one64();
        }
        return false;
      }

      /// \brief Generate identifier succ64
      /// \return Identifier succ64
      inline
      core::identifier_string const& succ64_name()
      {
        static core::identifier_string succ64_name = core::identifier_string("succ64");
        return succ64_name;
      }

      /// \brief Constructor for function symbol succ64
      
      /// \return Function symbol succ64
      inline
      function_symbol const& succ64()
      {
        static function_symbol succ64(succ64_name(), make_function_sort(pos64(), pos64()));
        return succ64;
      }

      /// \brief Recogniser for function succ64
      /// \param e A data expression
      /// \return true iff e is the function symbol matching succ64
      inline
      bool is_succ64_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == succ64();
        }
        return false;
      }

      /// \brief Application of function symbol succ64
      
      /// \param arg0 A data expression
      /// \return Application of succ64 to a number of arguments
      inline
      application succ64(const data_expression& arg0)
      {
        return sort_pos64::succ64()(arg0);
      }

      /// \brief Recogniser for application of succ64
      /// \param e A data expression
      /// \return true iff e is an application of function symbol succ64 to a
      ///     number of arguments
      inline
      bool is_succ64_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_succ64_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }
      /// \brief Give all system defined constructors for pos64
      /// \return All system defined constructors for pos64
      inline
      function_symbol_vector pos64_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(sort_pos64::one64());
        result.push_back(sort_pos64::succ64());

        return result;
      }

      /// \brief Generate identifier \@most_significant_digit
      /// \return Identifier \@most_significant_digit
      inline
      core::identifier_string const& most_significant_digit_name()
      {
        static core::identifier_string most_significant_digit_name = core::identifier_string("@most_significant_digit");
        return most_significant_digit_name;
      }

      /// \brief Constructor for function symbol \@most_significant_digit
      
      /// \return Function symbol most_significant_digit
      inline
      function_symbol const& most_significant_digit()
      {
        static function_symbol most_significant_digit(most_significant_digit_name(), make_function_sort(sort_machine_word::machine_word(), pos64()));
        return most_significant_digit;
      }

      /// \brief Recogniser for function \@most_significant_digit
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@most_significant_digit
      inline
      bool is_most_significant_digit_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == most_significant_digit();
        }
        return false;
      }

      /// \brief Application of function symbol \@most_significant_digit
      
      /// \param arg0 A data expression
      /// \return Application of \@most_significant_digit to a number of arguments
      inline
      application most_significant_digit(const data_expression& arg0)
      {
        return sort_pos64::most_significant_digit()(arg0);
      }

      /// \brief Recogniser for application of \@most_significant_digit
      /// \param e A data expression
      /// \return true iff e is an application of function symbol most_significant_digit to a
      ///     number of arguments
      inline
      bool is_most_significant_digit_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_most_significant_digit_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@concat_digit
      /// \return Identifier \@concat_digit
      inline
      core::identifier_string const& concat_digit_name()
      {
        static core::identifier_string concat_digit_name = core::identifier_string("@concat_digit");
        return concat_digit_name;
      }

      /// \brief Constructor for function symbol \@concat_digit
      
      /// \return Function symbol concat_digit
      inline
      function_symbol const& concat_digit()
      {
        static function_symbol concat_digit(concat_digit_name(), make_function_sort(pos64(), sort_machine_word::machine_word(), pos64()));
        return concat_digit;
      }

      /// \brief Recogniser for function \@concat_digit
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@concat_digit
      inline
      bool is_concat_digit_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == concat_digit();
        }
        return false;
      }

      /// \brief Application of function symbol \@concat_digit
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@concat_digit to a number of arguments
      inline
      application concat_digit(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_pos64::concat_digit()(arg0, arg1);
      }

      /// \brief Recogniser for application of \@concat_digit
      /// \param e A data expression
      /// \return true iff e is an application of function symbol concat_digit to a
      ///     number of arguments
      inline
      bool is_concat_digit_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_concat_digit_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier max
      /// \return Identifier max
      inline
      core::identifier_string const& maximum_name()
      {
        static core::identifier_string maximum_name = core::identifier_string("max");
        return maximum_name;
      }

      /// \brief Constructor for function symbol max
      
      /// \return Function symbol maximum
      inline
      function_symbol const& maximum()
      {
        static function_symbol maximum(maximum_name(), make_function_sort(pos64(), pos64(), pos64()));
        return maximum;
      }

      /// \brief Recogniser for function max
      /// \param e A data expression
      /// \return true iff e is the function symbol matching max
      inline
      bool is_maximum_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == maximum();
        }
        return false;
      }

      /// \brief Application of function symbol max
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of max to a number of arguments
      inline
      application maximum(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_pos64::maximum()(arg0, arg1);
      }

      /// \brief Recogniser for application of max
      /// \param e A data expression
      /// \return true iff e is an application of function symbol maximum to a
      ///     number of arguments
      inline
      bool is_maximum_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_maximum_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier min
      /// \return Identifier min
      inline
      core::identifier_string const& minimum_name()
      {
        static core::identifier_string minimum_name = core::identifier_string("min");
        return minimum_name;
      }

      /// \brief Constructor for function symbol min
      
      /// \return Function symbol minimum
      inline
      function_symbol const& minimum()
      {
        static function_symbol minimum(minimum_name(), make_function_sort(pos64(), pos64(), pos64()));
        return minimum;
      }

      /// \brief Recogniser for function min
      /// \param e A data expression
      /// \return true iff e is the function symbol matching min
      inline
      bool is_minimum_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == minimum();
        }
        return false;
      }

      /// \brief Application of function symbol min
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of min to a number of arguments
      inline
      application minimum(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_pos64::minimum()(arg0, arg1);
      }

      /// \brief Recogniser for application of min
      /// \param e A data expression
      /// \return true iff e is an application of function symbol minimum to a
      ///     number of arguments
      inline
      bool is_minimum_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_minimum_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@pospred
      /// \return Identifier \@pospred
      inline
      core::identifier_string const& pos_predecessor_name()
      {
        static core::identifier_string pos_predecessor_name = core::identifier_string("@pospred");
        return pos_predecessor_name;
      }

      /// \brief Constructor for function symbol \@pospred
      
      /// \return Function symbol pos_predecessor
      inline
      function_symbol const& pos_predecessor()
      {
        static function_symbol pos_predecessor(pos_predecessor_name(), make_function_sort(pos64(), pos64()));
        return pos_predecessor;
      }

      /// \brief Recogniser for function \@pospred
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@pospred
      inline
      bool is_pos_predecessor_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == pos_predecessor();
        }
        return false;
      }

      /// \brief Application of function symbol \@pospred
      
      /// \param arg0 A data expression
      /// \return Application of \@pospred to a number of arguments
      inline
      application pos_predecessor(const data_expression& arg0)
      {
        return sort_pos64::pos_predecessor()(arg0);
      }

      /// \brief Recogniser for application of \@pospred
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos_predecessor to a
      ///     number of arguments
      inline
      bool is_pos_predecessor_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_pos_predecessor_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier +
      /// \return Identifier +
      inline
      core::identifier_string const& plus_name()
      {
        static core::identifier_string plus_name = core::identifier_string("+");
        return plus_name;
      }

      /// \brief Constructor for function symbol +
      
      /// \return Function symbol plus
      inline
      function_symbol const& plus()
      {
        static function_symbol plus(plus_name(), make_function_sort(pos64(), pos64(), pos64()));
        return plus;
      }

      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_plus_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == plus();
        }
        return false;
      }

      /// \brief Application of function symbol +
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of + to a number of arguments
      inline
      application plus(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_pos64::plus()(arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol plus to a
      ///     number of arguments
      inline
      bool is_plus_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_plus_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& times_name()
      {
        static core::identifier_string times_name = core::identifier_string("*");
        return times_name;
      }

      /// \brief Constructor for function symbol *
      
      /// \return Function symbol times
      inline
      function_symbol const& times()
      {
        static function_symbol times(times_name(), make_function_sort(pos64(), pos64(), pos64()));
        return times;
      }

      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_times_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == times();
        }
        return false;
      }

      /// \brief Application of function symbol *
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of * to a number of arguments
      inline
      application times(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_pos64::times()(arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol times to a
      ///     number of arguments
      inline
      bool is_times_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_times_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }

      /// \brief Generate identifier powerlog2
      /// \return Identifier powerlog2
      inline
      core::identifier_string const& powerlog2_pos_name()
      {
        static core::identifier_string powerlog2_pos_name = core::identifier_string("powerlog2");
        return powerlog2_pos_name;
      }

      /// \brief Constructor for function symbol powerlog2
      
      /// \return Function symbol powerlog2_pos
      inline
      function_symbol const& powerlog2_pos()
      {
        static function_symbol powerlog2_pos(powerlog2_pos_name(), make_function_sort(pos64(), pos64()));
        return powerlog2_pos;
      }

      /// \brief Recogniser for function powerlog2
      /// \param e A data expression
      /// \return true iff e is the function symbol matching powerlog2
      inline
      bool is_powerlog2_pos_function_symbol(const atermpp::aterm& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == powerlog2_pos();
        }
        return false;
      }

      /// \brief Application of function symbol powerlog2
      
      /// \param arg0 A data expression
      /// \return Application of powerlog2 to a number of arguments
      inline
      application powerlog2_pos(const data_expression& arg0)
      {
        return sort_pos64::powerlog2_pos()(arg0);
      }

      /// \brief Recogniser for application of powerlog2
      /// \param e A data expression
      /// \return true iff e is an application of function symbol powerlog2_pos to a
      ///     number of arguments
      inline
      bool is_powerlog2_pos_application(const atermpp::aterm& e)
      {
        if (is_application(e))
        {
          return is_powerlog2_pos_function_symbol(atermpp::down_cast<application>(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for pos64
      /// \return All system defined mappings for pos64
      inline
      function_symbol_vector pos64_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(sort_pos64::most_significant_digit());
        result.push_back(sort_pos64::concat_digit());
        result.push_back(sort_pos64::maximum());
        result.push_back(sort_pos64::minimum());
        result.push_back(sort_pos64::pos_predecessor());
        result.push_back(sort_pos64::plus());
        result.push_back(sort_pos64::times());
        result.push_back(sort_pos64::powerlog2_pos());
        return result;
      }
      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_concat_digit_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_concat_digit_application(e));
        return atermpp::down_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_times_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        right from an application
      /// \param e A data expression
      /// \pre right is defined for e
      /// \return The argument of e that corresponds to right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_times_application(e));
        return atermpp::down_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_succ64_application(e) || is_most_significant_digit_application(e) || is_pos_predecessor_application(e) || is_powerlog2_pos_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      /// \brief Give all system defined equations for pos64
      /// \return All system defined equations for sort pos64
      inline
      data_equation_vector pos64_generate_equations_code()
      {
        variable vp("p",pos64());
        variable vp1("p1",pos64());
        variable vp2("p2",pos64());
        variable vw("w",sort_machine_word::machine_word());
        variable vw1("w1",sort_machine_word::machine_word());
        variable vw2("w2",sort_machine_word::machine_word());

        data_equation_vector result;
        result.push_back(data_equation(variable_list(), one64(), most_significant_digit(sort_machine_word::one_word())));
        result.push_back(data_equation(variable_list({vmax_word, vone_word, vw1}), succ64(most_significant_digit(vw1)), if_(equal_to(vw1, vmax_word), concat_digit(most_significant_digit(vone_word), sort_machine_word::zero64()), most_significant_digit(sort_machine_word::succ64(vw1)))));
        result.push_back(data_equation(variable_list({vmax_word, vp, vw1}), succ64(concat_digit(vp, vw1)), if_(equal_to(vw1, vmax_word), concat_digit(succ64(vp), sort_machine_word::zero64()), concat_digit(vp, sort_machine_word::succ64(vw1)))));
        result.push_back(data_equation(variable_list({vw1, vw2}), equal_to(most_significant_digit(vw1), most_significant_digit(vw2)), equal_to(vw1, vw2)));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), equal_to(concat_digit(vp, vw1), most_significant_digit(vw2)), sort_bool::false_()));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), equal_to(most_significant_digit(vw1), concat_digit(vp, vw2)), sort_bool::false_()));
        result.push_back(data_equation(variable_list({vp1, vp2, vw1, vw2}), equal_to(concat_digit(vp1, vw1), concat_digit(vp2, vw2)), sort_bool::and_(equal_to(vp1, vp2), equal_to(vw1, vw2))));
        result.push_back(data_equation(variable_list({vw1, vw2}), less(most_significant_digit(vw1), most_significant_digit(vw2)), less(vw1, vw2)));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), less(concat_digit(vp, vw1), most_significant_digit(vw2)), sort_bool::false_()));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), less(most_significant_digit(vw1), concat_digit(vp, vw2)), sort_bool::true_()));
        result.push_back(data_equation(variable_list({vp1, vp2, vw1, vw2}), less(concat_digit(vp1, vw1), concat_digit(vp2, vw2)), if_(less(vw1, vw2), less_equal(vp1, vp2), less(vp1, vp2))));
        result.push_back(data_equation(variable_list({vw1, vw2}), less_equal(most_significant_digit(vw1), most_significant_digit(vw2)), less_equal(vw1, vw2)));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), less_equal(concat_digit(vp, vw1), most_significant_digit(vw2)), sort_bool::false_()));
        result.push_back(data_equation(variable_list({vp, vw1, vw2}), less_equal(most_significant_digit(vw1), concat_digit(vp, vw2)), sort_bool::true_()));
        result.push_back(data_equation(variable_list({vp1, vp2, vw1, vw2}), less_equal(concat_digit(vp1, vw1), concat_digit(vp2, vw2)), if_(less_equal(vw1, vw2), less_equal(vp1, vp2), less(vp1, vp2))));
        result.push_back(data_equation(variable_list({vp1, vp2}), maximum(vp1, vp2), if_(less_equal(vp1, vp2), vp2, vp1)));
        result.push_back(data_equation(variable_list({vp1, vp2}), minimum(vp1, vp2), if_(less_equal(vp1, vp2), vp1, vp2)));
        result.push_back(data_equation(variable_list({vone_word, vpred_word, vw1}), pos_predecessor(most_significant_digit(vw1)), if_(equal_to(vw1, vone_word), most_significant_digit(vone_word), most_significant_digit(vpred_word(vw1)))));
        result.push_back(data_equation(variable_list({vmax_word, vone_word, vp, vpred_word, vw1}), pos_predecessor(concat_digit(vp, vw1)), if_(equal_to(vw1, sort_machine_word::zero64()), if_(equal_to(vp, most_significant_digit(vone_word)), most_significant_digit(vmax_word), concat_digit(pos_predecessor(vp), vmax_word)), concat_digit(vp, vpred_word(vw1)))));
        result.push_back(data_equation(variable_list({vadd_overflow_word, vadd_word, vone_word, vw1, vw2}), plus(most_significant_digit(vw1), most_significant_digit(vw2)), if_(equal_to(vadd_overflow_word(vw1, vw2), sort_machine_word::zero64()), most_significant_digit(vadd_word(vw1, vw2)), concat_digit(most_significant_digit(vone_word), vadd_word(vw1, vw2)))));
        result.push_back(data_equation(variable_list({vadd_overflow_word, vadd_word, vp1, vw1, vw2}), plus(concat_digit(vp1, vw1), most_significant_digit(vw2)), concat_digit(plus(most_significant_digit(vadd_overflow_word(vw1, vw2)), vp1), vadd_word(vw1, vw2))));
        result.push_back(data_equation(variable_list({vadd_overflow_word, vadd_word, vp2, vw1, vw2}), plus(most_significant_digit(vw1), concat_digit(vp2, vw2)), concat_digit(plus(most_significant_digit(vadd_overflow_word(vw1, vw2)), vp2), vadd_word(vw1, vw2))));
        result.push_back(data_equation(variable_list({vadd_overflow_word, vadd_word, vp1, vp2, vw1, vw2}), plus(concat_digit(vp1, vw1), concat_digit(vp2, vw2)), concat_digit(plus(most_significant_digit(vadd_overflow_word(vw1, vw2)), plus(vp1, vp2)), vadd_word(vw1, vw2))));
        result.push_back(data_equation(variable_list({vp1, vtimes_overflow_word, vtimes_word, vw1, vw2}), times(concat_digit(vp1, vw1), most_significant_digit(vw2)), concat_digit(plus(times(vp1, most_significant_digit(vw2)), most_significant_digit(vtimes_overflow_word(vw1, vw2))), vtimes_word(vw1, vw2))));
        result.push_back(data_equation(variable_list({vp2, vtimes_overflow_word, vtimes_word, vw1, vw2}), times(most_significant_digit(vw1), concat_digit(vp2, vw2)), concat_digit(plus(times(vp2, most_significant_digit(vw1)), most_significant_digit(vtimes_overflow_word(vw1, vw2))), vtimes_word(vw1, vw2))));
        result.push_back(data_equation(variable_list({vp1, vp2, vw1, vw2}), times(concat_digit(vp1, vw1), concat_digit(vp2, vw2)), plus(concat_digit(times(concat_digit(vp1, vw1), vp2), sort_machine_word::zero64()), times(concat_digit(vp1, vw1), most_significant_digit(vw2)))));
        return result;
      }

    } // namespace sort_pos64

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_POS64_H
