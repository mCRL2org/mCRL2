// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/standard.h
/// \brief Standard functions that are available for all sorts.

#ifndef MCRL2_DATA_STANDARD_H
#define MCRL2_DATA_STANDARD_H

#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"

namespace mcrl2 {

  namespace data {

    // predeclare
    namespace sort_bool_ {
      basic_sort const& bool_();
      function_symbol const& false_();
      function_symbol const& true_();
      application not_(const data_expression&);
      bool is_bool_(const sort_expression&);
    }

    namespace detail {
      /// Function for initialisation of static variables, takes care of protection
      /// \param[in,out] target a reference to the static variable
      /// \param[in] original the expression that is used to initialise the variable
      /// \ return a reference to original
      template < typename Expression >
      Expression const& initialise_static_expression(Expression& target, Expression const& original)
      {
        target = original;
        target.protect();
     
        return original;
      }
    }

    /// \brief Constructor for function symbol ==
    /// \param[in] s A sort expression
    /// \return function symbol equal_to
    inline function_symbol equal_to(const sort_expression& s)
    {
      return function_symbol("==", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function ==
    /// \param e[in] A data expression
    /// \return true iff e is the function symbol matching ==
    inline bool is_equal_to_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == "==";
    }

    /// \brief Application of function symbol ==
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of == to a number of arguments
    inline application equal_to(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(equal_to(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of ==
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol equal_to to a
    ///     number of arguments
    inline bool is_equal_to_application(const data_expression& e)
    {
      return (e.is_application()) && is_equal_to_function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol !=
    /// \param[in] s A sort expression
    /// \return function symbol not_equal_to
    inline function_symbol not_equal_to(const sort_expression& s)
    {
      return function_symbol("!=", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function !=
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching !=
    inline bool is_not_equal_to_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == "!=";
    }

    /// \brief Application of function symbol !=
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of != to a number of arguments
    inline application not_equal_to(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(not_equal_to(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of !=
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol not_equal_to to a
    ///     number of arguments
    inline bool is_not_equal_to_application(const data_expression& e)
    {
      return (e.is_application()) && is_not_equal_to_function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol if
    /// \param[in] s A sort expression
    /// \return function symbol if_
    inline function_symbol if_(const sort_expression& s)
    {
      return function_symbol("if", function_sort(sort_bool_::bool_(), s, s, s));
    }

    /// \brief Recogniser for function if
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching if_
    inline bool is_if__function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == "if";
    }

    /// \brief Application of function symbol if
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \param[in] arg2 A data expression
    /// \return Application of if to a number of arguments
    inline application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
    {
      assert(sort_bool_::is_bool_(arg0.sort()));
      assert(arg1.sort() == arg2.sort());

      return application(if_(arg1.sort()), arg0, arg1, arg2);
    }

    /// \brief Recogniser for application of if
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol if_ to a
    ///     number of arguments
    inline bool is_if__application(const data_expression& e)
    {
      return e.is_application() && is_if__function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol <
    /// \param[in] s A sort expression
    /// \return function symbol less
    inline function_symbol less(const sort_expression& s)
    {
      return function_symbol("<", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function <
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching <
    inline bool is_less_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == "<";
    }

    /// \brief Application of function symbol <
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of < to a number of arguments
    inline application less(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(less(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of <
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol less to a
    ///     number of arguments
    inline bool is_less_application(const data_expression& e)
    {
      return e.is_application() && is_less_function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol <=
    /// \param[in] s A sort expression
    /// \return function symbol less_equal
    inline function_symbol less_equal(const sort_expression& s)
    {
      return function_symbol("<=", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function <=
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching <=
    inline bool is_less_equal_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == "<=";
    }

    /// \brief Application of function symbol <=
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of <= to a number of arguments
    inline application less_equal(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(less_equal(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of <=
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol less_equal to a
    ///     number of arguments
    inline bool is_less_equal_application(const data_expression& e)
    {
      return e.is_application() && is_less_equal_function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol >
    /// \param[in] s A sort expression
    /// \return function symbol greater
    inline function_symbol greater(const sort_expression& s)
    {
      return function_symbol(">", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function >
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching >
    inline bool is_greater_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == ">";
    }

    /// \brief Application of function symbol >
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of > to a number of arguments
    inline application greater(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(greater(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of >
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol greater to a
    ///     number of arguments
    inline bool is_greater_application(const data_expression& e)
    {
      return e.is_application() && is_greater_function_symbol(application(e).head());
    }

    /// \brief Constructor for function symbol >=
    /// \param[in] s A sort expression
    /// \return function symbol greater_equal
    inline function_symbol greater_equal(const sort_expression& s)
    {
      return function_symbol(">=", function_sort(s, s, sort_bool_::bool_()));
    }

    /// \brief Recogniser for function >=
    /// \param[in] e A data expression
    /// \return true iff e is the function symbol matching >=
    inline bool is_greater_equal_function_symbol(const data_expression& e)
    {
      return e.is_function_symbol() && function_symbol(e).name() == ">=";
    }

    /// \brief Application of function symbol >=
    /// \param[in] arg0 A data expression
    /// \param[in] arg1 A data expression
    /// \return Application of >= to a number of arguments
    inline application greater_equal(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(greater_equal(arg0.sort()),arg0, arg1);
    }

    /// \brief Recogniser for application of >=
    /// \param[in] e A data expression
    /// \return true iff e is an application of function symbol greater_equal to a
    ///     number of arguments
    inline bool is_greater_equal_application(const data_expression& e)
    {
      return e.is_application() && is_greater_equal_function_symbol(application(e).head());
    }

    /// \brief Give all standard system defined functions for sort s
    /// \param[in] s A sort expression
    /// \return All standard system defined functions for sort s
    inline function_symbol_vector standard_generate_functions_code(const sort_expression& s)
    {
      function_symbol_vector result;
      result.push_back(equal_to(s));
      result.push_back(not_equal_to(s));
      result.push_back(if_(s));
      result.push_back(less(s));
      result.push_back(less_equal(s));
      result.push_back(greater_equal(s));
      result.push_back(greater(s));

      return result;
    }

    /// \brief Give all standard system defined equations for sort s
    /// \param[in] s A sort expression
    /// \return All standard system defined equations for sort s
    inline data_equation_vector standard_generate_equations_code(const sort_expression& s)
    {
      data_equation_vector result;
      variable b("b", sort_bool_::bool_());
      variable x("x", s);
      variable y("y", s);
      result.push_back(data_equation(make_vector(x), equal_to(x, x), sort_bool_::true_()));
      result.push_back(data_equation(make_vector(x, y), not_equal_to(x, y), sort_bool_::not_(equal_to(x, y))));
      result.push_back(data_equation(make_vector(x, y), if_(sort_bool_::true_(), x, y), x));
      result.push_back(data_equation(make_vector(x, y), if_(sort_bool_::false_(), x, y), y));
      result.push_back(data_equation(make_vector(b, x), if_(b, x, x), x));
      result.push_back(data_equation(make_vector(x), less(x,x), sort_bool_::false_()));
      result.push_back(data_equation(make_vector(x), less_equal(x,x), sort_bool_::true_()));
      result.push_back(data_equation(make_vector(x, y), greater_equal(x,y), less_equal(y,x)));
      result.push_back(data_equation(make_vector(x, y), greater(x,y), less(y,x)));

      return result;
    }

  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_STANDARD__H
