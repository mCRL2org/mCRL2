// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/bound.h
/// \brief The standard sort bound.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/bound.spec.

#ifndef MCRL2_DATA_BOUND_H
#define MCRL2_DATA_BOUND_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/real.h"
#include "inequality.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort bound
    namespace sort_bound {

      inline
      core::identifier_string const& bound_name()
      {
        static core::identifier_string bound_name = core::identifier_string("Bound");
        return bound_name;
      }

      /// \brief Constructor for sort expression Bound
      /// \return Sort expression Bound
      inline
      basic_sort const& bound()
      {
        static basic_sort bound = basic_sort(bound_name());
        return bound;
      }

      /// \brief Recogniser for sort expression Bound
      /// \param e A sort expression
      /// \return true iff e == bound()
      inline
      bool is_bound(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == bound();
        }
        return false;
      }


      /// \brief Generate identifier inf
      /// \return Identifier inf
      inline
      core::identifier_string const& inf_name()
      {
        static core::identifier_string inf_name = core::identifier_string("inf");
        return inf_name;
      }

      /// \brief Constructor for function symbol inf
      
      /// \return Function symbol inf
      inline
      function_symbol const& inf()
      {
        static function_symbol inf(inf_name(), bound());
        return inf;
      }

      /// \brief Recogniser for function inf
      /// \param e A data expression
      /// \return true iff e is the function symbol matching inf
      inline
      bool is_inf_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == inf();
        }
        return false;
      }

      /// \brief Generate identifier bound
      /// \return Identifier bound
      inline
      core::identifier_string const& cbound_name()
      {
        static core::identifier_string cbound_name = core::identifier_string("bound");
        return cbound_name;
      }

      /// \brief Constructor for function symbol bound
      
      /// \return Function symbol cbound
      inline
      function_symbol const& cbound()
      {
        static function_symbol cbound(cbound_name(), make_function_sort(sort_real::real_(), sort_inequality::inequality(), bound()));
        return cbound;
      }

      /// \brief Recogniser for function bound
      /// \param e A data expression
      /// \return true iff e is the function symbol matching bound
      inline
      bool is_cbound_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == cbound();
        }
        return false;
      }

      /// \brief Application of function symbol bound
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of bound to a number of arguments
      inline
      application cbound(const data_expression& arg0, const data_expression& arg1)
      {
        assert(arg0.sort() == sort_real::real_());
        return sort_bound::cbound()(arg0, arg1);
      }

      /// \brief Recogniser for application of bound
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cbound to a
      ///     number of arguments
      inline
      bool is_cbound_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cbound_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined constructors for bound
      /// \return All system defined constructors for bound
      inline
      function_symbol_vector bound_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(sort_bound::inf());
        result.push_back(sort_bound::cbound());

        return result;
      }

      /// \brief Generate identifier lt
      /// \return Identifier lt
      inline
      core::identifier_string const& lt_name()
      {
        static core::identifier_string lt_name = core::identifier_string("lt");
        return lt_name;
      }

      /// \brief Constructor for function symbol lt
      
      /// \return Function symbol lt
      inline
      function_symbol const& lt()
      {
        static function_symbol lt(lt_name(), make_function_sort(bound(), bound(), sort_bool::bool_()));
        return lt;
      }

      /// \brief Recogniser for function lt
      /// \param e A data expression
      /// \return true iff e is the function symbol matching lt
      inline
      bool is_lt_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == lt();
        }
        return false;
      }

      /// \brief Application of function symbol lt
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of lt to a number of arguments
      inline
      application lt(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bound::lt()(arg0, arg1);
      }

      /// \brief Recogniser for application of lt
      /// \param e A data expression
      /// \return true iff e is an application of function symbol lt to a
      ///     number of arguments
      inline
      bool is_lt_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_lt_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier add
      /// \return Identifier add
      inline
      core::identifier_string const& add_name()
      {
        static core::identifier_string add_name = core::identifier_string("add");
        return add_name;
      }

      /// \brief Constructor for function symbol add
      
      /// \return Function symbol add
      inline
      function_symbol const& add()
      {
        static function_symbol add(add_name(), make_function_sort(bound(), bound(), bound()));
        return add;
      }

      /// \brief Recogniser for function add
      /// \param e A data expression
      /// \return true iff e is the function symbol matching add
      inline
      bool is_add_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == add();
        }
        return false;
      }

      /// \brief Application of function symbol add
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of add to a number of arguments
      inline
      application add(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bound::add()(arg0, arg1);
      }

      /// \brief Recogniser for application of add
      /// \param e A data expression
      /// \return true iff e is an application of function symbol add to a
      ///     number of arguments
      inline
      bool is_add_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_add_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier min_b
      /// \return Identifier min_b
      inline
      core::identifier_string const& min_b_name()
      {
        static core::identifier_string min_b_name = core::identifier_string("min_b");
        return min_b_name;
      }

      /// \brief Constructor for function symbol min_b
      
      /// \return Function symbol min_b
      inline
      function_symbol const& min_b()
      {
        static function_symbol min_b(min_b_name(), make_function_sort(bound(), bound(), bound()));
        return min_b;
      }

      /// \brief Recogniser for function min_b
      /// \param e A data expression
      /// \return true iff e is the function symbol matching min_b
      inline
      bool is_min_b_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == min_b();
        }
        return false;
      }

      /// \brief Application of function symbol min_b
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of min_b to a number of arguments
      inline
      application min_b(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bound::min_b()(arg0, arg1);
      }

      /// \brief Recogniser for application of min_b
      /// \param e A data expression
      /// \return true iff e is an application of function symbol min_b to a
      ///     number of arguments
      inline
      bool is_min_b_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_min_b_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier max_b
      /// \return Identifier max_b
      inline
      core::identifier_string const& max_b_name()
      {
        static core::identifier_string max_b_name = core::identifier_string("max_b");
        return max_b_name;
      }

      /// \brief Constructor for function symbol max_b
      
      /// \return Function symbol max_b
      inline
      function_symbol const& max_b()
      {
        static function_symbol max_b(max_b_name(), make_function_sort(bound(), bound(), bound()));
        return max_b;
      }

      /// \brief Recogniser for function max_b
      /// \param e A data expression
      /// \return true iff e is the function symbol matching max_b
      inline
      bool is_max_b_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == max_b();
        }
        return false;
      }

      /// \brief Application of function symbol max_b
      
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of max_b to a number of arguments
      inline
      application max_b(const data_expression& arg0, const data_expression& arg1)
      {
        return sort_bound::max_b()(arg0, arg1);
      }

      /// \brief Recogniser for application of max_b
      /// \param e A data expression
      /// \return true iff e is an application of function symbol max_b to a
      ///     number of arguments
      inline
      bool is_max_b_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_max_b_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier not_b
      /// \return Identifier not_b
      inline
      core::identifier_string const& not_b_name()
      {
        static core::identifier_string not_b_name = core::identifier_string("not_b");
        return not_b_name;
      }

      /// \brief Constructor for function symbol not_b
      
      /// \return Function symbol not_b
      inline
      function_symbol const& not_b()
      {
        static function_symbol not_b(not_b_name(), make_function_sort(bound(), bound()));
        return not_b;
      }

      /// \brief Recogniser for function not_b
      /// \param e A data expression
      /// \return true iff e is the function symbol matching not_b
      inline
      bool is_not_b_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == not_b();
        }
        return false;
      }

      /// \brief Application of function symbol not_b
      
      /// \param arg0 A data expression
      /// \return Application of not_b to a number of arguments
      inline
      application not_b(const data_expression& arg0)
      {
        return sort_bound::not_b()(arg0);
      }

      /// \brief Recogniser for application of not_b
      /// \param e A data expression
      /// \return true iff e is an application of function symbol not_b to a
      ///     number of arguments
      inline
      bool is_not_b_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_not_b_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for bound
      /// \return All system defined mappings for bound
      inline
      function_symbol_vector bound_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(sort_bound::lt());
        result.push_back(sort_bound::add());
        result.push_back(sort_bound::min_b());
        result.push_back(sort_bound::max_b());
        result.push_back(sort_bound::not_b());
        return result;
      }
      ///\brief Function for projecting out argument
      ///        right from an application
      /// \param e A data expression
      /// \pre right is defined for e
      /// \return The argument of e that corresponds to right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_cbound_application(e) || is_lt_application(e) || is_add_application(e) || is_min_b_application(e) || is_max_b_application(e));
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
        assert(is_not_b_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_cbound_application(e) || is_lt_application(e) || is_add_application(e) || is_min_b_application(e) || is_max_b_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      /// \brief Give all system defined equations for bound
      /// \return All system defined equations for sort bound
      inline
      data_equation_vector bound_generate_equations_code()
      {
        variable vu("u",sort_inequality::inequality());
        variable vv("v",sort_inequality::inequality());
        variable vr1("r1",sort_real::real_());
        variable vr2("r2",sort_real::real_());
        variable vb1("b1",bound());
        variable vb2("b2",bound());

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vr1, vr2, vu, vv), equal_to(cbound(vr1, vu), cbound(vr2, vv)), sort_bool::and_(equal_to(vr1, vr2), equal_to(vu, vv))));
        result.push_back(data_equation(variable_list(), equal_to(inf(), inf()), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vr1, vu), equal_to(inf(), cbound(vr1, vu)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vr1, vu), equal_to(cbound(vr1, vu), inf()), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb1), lt(inf(), vb1), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vr1, vu), lt(cbound(vr1, vu), inf()), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vr1, vr2, vu, vv), lt(cbound(vr1, vu), cbound(vr2, vv)), sort_bool::or_(sort_bool::and_(sort_bool::and_(equal_to(vr1, vr2), equal_to(vu, sort_inequality::lt())), equal_to(vv, sort_inequality::le())), less(vr1, vr2))));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2), lt(vb1, vb2), min_b(vb1, vb2), vb1));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(lt(vb1, vb2)), min_b(vb1, vb2), vb2));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2), lt(vb1, vb2), max_b(vb1, vb2), vb2));
        result.push_back(data_equation(atermpp::make_vector(vb1, vb2), sort_bool::not_(lt(vb1, vb2)), max_b(vb1, vb2), vb1));
        result.push_back(data_equation(atermpp::make_vector(vb1), add(vb1, inf()), inf()));
        result.push_back(data_equation(atermpp::make_vector(vb1), add(inf(), vb1), inf()));
        result.push_back(data_equation(atermpp::make_vector(vr1, vr2), add(cbound(vr1, sort_inequality::le()), cbound(vr2, sort_inequality::le())), cbound(sort_real::plus(vr1, vr2), sort_inequality::le())));
        result.push_back(data_equation(atermpp::make_vector(vr1, vr2, vu), add(cbound(vr1, sort_inequality::lt()), cbound(vr2, vu)), cbound(sort_real::plus(vr1, vr2), sort_inequality::lt())));
        result.push_back(data_equation(atermpp::make_vector(vr1, vr2, vu), add(cbound(vr1, vu), cbound(vr2, sort_inequality::lt())), cbound(sort_real::plus(vr1, vr2), sort_inequality::lt())));
        result.push_back(data_equation(atermpp::make_vector(vr1), not_b(cbound(vr1, sort_inequality::lt())), cbound(sort_real::negate(vr1), sort_inequality::le())));
        result.push_back(data_equation(atermpp::make_vector(vr1), not_b(cbound(vr1, sort_inequality::le())), cbound(sort_real::negate(vr1), sort_inequality::lt())));
        return result;
      }

    } // namespace sort_bound

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_BOUND_H
