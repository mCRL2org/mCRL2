// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/function_update.h
/// \brief The standard sort function_update.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/function_update.spec.

#ifndef MCRL2_DATA_FUNCTION_UPDATE_H
#define MCRL2_DATA_FUNCTION_UPDATE_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"

namespace mcrl2 {

  namespace data {


      /// \brief Generate identifier \@func_update
      /// \return Identifier \@func_update
      inline
      core::identifier_string const& function_update_name()
      {
        static core::identifier_string function_update_name = core::identifier_string("@func_update");
        return function_update_name;
      }

      /// \brief Constructor for function symbol \@func_update
      /// \param s A sort expression
      /// \param t A sort expression
      /// \return Function symbol function_update
      inline
      function_symbol function_update(const sort_expression& s, const sort_expression& t)
      {
        function_symbol function_update(function_update_name(), make_function_sort(make_function_sort(s, t), s, t, make_function_sort(s, t)));
        return function_update;
      }

      /// \brief Recogniser for function \@func_update
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@func_update
      inline
      bool is_function_update_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e).name() == function_update_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@func_update
      /// \param s A sort expression
      /// \param t A sort expression
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@func_update to a number of arguments
      inline
      application function_update(const sort_expression& s, const sort_expression& t, const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return function_update(s, t)(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@func_update
      /// \param e A data expression
      /// \return true iff e is an application of function symbol function_update to a
      ///     number of arguments
      inline
      bool is_function_update_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_function_update_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for function_update
      /// \param s A sort expression
      /// \param t A sort expression
      /// \return All system defined mappings for function_update
      inline
      function_symbol_vector function_update_generate_functions_code(const sort_expression& s, const sort_expression& t)
      {
        function_symbol_vector result;
        result.push_back(function_update(s, t));
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
        assert(is_function_update_application(e));
        return atermpp::aterm_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_function_update_application(e));
        return atermpp::aterm_cast<const application >(e)[1];
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_function_update_application(e));
        return atermpp::aterm_cast<const application >(e)[2];
      }

      /// \brief Give all system defined equations for function_update
      /// \param s A sort expression
      /// \param t A sort expression
      /// \return All system defined equations for sort function_update
      inline
      data_equation_vector function_update_generate_equations_code(const sort_expression& s, const sort_expression& t)
      {
        variable vx("x",s);
        variable vy("y",s);
        variable vv("v",t);
        variable vw("w",t);
        variable vf("f",make_function_sort(s, t));

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vf, vv, vx), equal_to(vf(vx), vv), function_update(s, t, vf, vx, vv), vf));
        result.push_back(data_equation(atermpp::make_vector(vf, vv, vw, vx), function_update(s, t, function_update(s, t, vf, vx, vw), vx, vv), function_update(s, t, vf, vx, vv)));
        result.push_back(data_equation(atermpp::make_vector(vf, vv, vw, vx, vy), greater(vx, vy), function_update(s, t, function_update(s, t, vf, vy, vw), vx, vv), function_update(s, t, function_update(s, t, vf, vx, vv), vy, vw)));
        result.push_back(data_equation(atermpp::make_vector(vf, vv, vx, vy), not_equal_to(vx, vy), function_update(s, t, vf, vx, vv)(vy), vf(vy)));
        result.push_back(data_equation(atermpp::make_vector(vf, vv, vx), function_update(s, t, vf, vx, vv)(vx), vv));
        return result;
      }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_FUNCTION_UPDATE_H
