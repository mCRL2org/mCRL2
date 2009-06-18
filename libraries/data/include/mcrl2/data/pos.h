// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/pos.h
/// \brief The standard sort pos.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/pos.spec.

#ifndef MCRL2_DATA_POS_H
#define MCRL2_DATA_POS_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort pos
    namespace sort_pos {

      inline
      core::identifier_string const& pos_name()
      {
        static core::identifier_string pos_name = data::detail::initialise_static_expression(pos_name, core::identifier_string("Pos"));
        return pos_name;
      }

      /// \brief Constructor for sort expression Pos
      /// \return Sort expression Pos
      inline
      basic_sort const& pos()
      {
        static basic_sort pos = data::detail::initialise_static_expression(pos, basic_sort(pos_name()));
        return pos;
      }

      /// \brief Recogniser for sort expression Pos
      /// \param e A sort expression
      /// \return true iff e == pos()
      inline
      bool is_pos(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast< basic_sort >(e) == pos();
        }
        return false;
      }

      /// \brief Generate identifier \@c1
      /// \return Identifier \@c1
      inline
      core::identifier_string const& c1_name()
      {
        static core::identifier_string c1_name = data::detail::initialise_static_expression(c1_name, core::identifier_string("@c1"));
        return c1_name;
      }

      /// \brief Constructor for function symbol \@c1
      /// \return Function symbol c1
      inline
      function_symbol const& c1()
      {
        static function_symbol c1 = data::detail::initialise_static_expression(c1, function_symbol(c1_name(), pos()));
        return c1;
      }


      /// \brief Recogniser for function \@c1
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@c1
      inline
      bool is_c1_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == c1_name();
        }
        return false;
      }

      /// \brief Generate identifier \@cDub
      /// \return Identifier \@cDub
      inline
      core::identifier_string const& cdub_name()
      {
        static core::identifier_string cdub_name = data::detail::initialise_static_expression(cdub_name, core::identifier_string("@cDub"));
        return cdub_name;
      }

      /// \brief Constructor for function symbol \@cDub
      /// \return Function symbol cdub
      inline
      function_symbol const& cdub()
      {
        static function_symbol cdub = data::detail::initialise_static_expression(cdub, function_symbol(cdub_name(), function_sort(sort_bool_::bool_(), pos(), pos())));
        return cdub;
      }


      /// \brief Recogniser for function \@cDub
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cDub
      inline
      bool is_cdub_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == cdub_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@cDub
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@cDub to a number of arguments
      inline
      application cdub(const data_expression& arg0, const data_expression& arg1)
      {
        return application(cdub(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@cDub
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cdub to a
      ///     number of arguments
      inline
      bool is_cdub_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cdub_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for pos
      /// \return All system defined constructors for pos
      inline
      function_symbol_vector pos_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(c1());
        result.push_back(cdub());

        return result;
      }
      /// \brief Generate identifier max
      /// \return Identifier max
      inline
      core::identifier_string const& maximum_name()
      {
        static core::identifier_string maximum_name = data::detail::initialise_static_expression(maximum_name, core::identifier_string("max"));
        return maximum_name;
      }

      /// \brief Constructor for function symbol max
      /// \return Function symbol maximum
      inline
      function_symbol const& maximum()
      {
        static function_symbol maximum = data::detail::initialise_static_expression(maximum, function_symbol(maximum_name(), function_sort(pos(), pos(), pos())));
        return maximum;
      }


      /// \brief Recogniser for function max
      /// \param e A data expression
      /// \return true iff e is the function symbol matching max
      inline
      bool is_maximum_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == maximum_name();
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
        return application(maximum(),arg0, arg1);
      }

      /// \brief Recogniser for application of max
      /// \param e A data expression
      /// \return true iff e is an application of function symbol maximum to a
      ///     number of arguments
      inline
      bool is_maximum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_maximum_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier min
      /// \return Identifier min
      inline
      core::identifier_string const& minimum_name()
      {
        static core::identifier_string minimum_name = data::detail::initialise_static_expression(minimum_name, core::identifier_string("min"));
        return minimum_name;
      }

      /// \brief Constructor for function symbol min
      /// \return Function symbol minimum
      inline
      function_symbol const& minimum()
      {
        static function_symbol minimum = data::detail::initialise_static_expression(minimum, function_symbol(minimum_name(), function_sort(pos(), pos(), pos())));
        return minimum;
      }


      /// \brief Recogniser for function min
      /// \param e A data expression
      /// \return true iff e is the function symbol matching min
      inline
      bool is_minimum_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == minimum_name();
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
        return application(minimum(),arg0, arg1);
      }

      /// \brief Recogniser for application of min
      /// \param e A data expression
      /// \return true iff e is an application of function symbol minimum to a
      ///     number of arguments
      inline
      bool is_minimum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minimum_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier abs
      /// \return Identifier abs
      inline
      core::identifier_string const& abs_name()
      {
        static core::identifier_string abs_name = data::detail::initialise_static_expression(abs_name, core::identifier_string("abs"));
        return abs_name;
      }

      /// \brief Constructor for function symbol abs
      /// \return Function symbol abs
      inline
      function_symbol const& abs()
      {
        static function_symbol abs = data::detail::initialise_static_expression(abs, function_symbol(abs_name(), function_sort(pos(), pos())));
        return abs;
      }


      /// \brief Recogniser for function abs
      /// \param e A data expression
      /// \return true iff e is the function symbol matching abs
      inline
      bool is_abs_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == abs_name();
        }
        return false;
      }

      /// \brief Application of function symbol abs
      /// \param arg0 A data expression
      /// \return Application of abs to a number of arguments
      inline
      application abs(const data_expression& arg0)
      {
        return application(abs(),arg0);
      }

      /// \brief Recogniser for application of abs
      /// \param e A data expression
      /// \return true iff e is an application of function symbol abs to a
      ///     number of arguments
      inline
      bool is_abs_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_abs_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier succ
      /// \return Identifier succ
      inline
      core::identifier_string const& succ_name()
      {
        static core::identifier_string succ_name = data::detail::initialise_static_expression(succ_name, core::identifier_string("succ"));
        return succ_name;
      }

      /// \brief Constructor for function symbol succ
      /// \return Function symbol succ
      inline
      function_symbol const& succ()
      {
        static function_symbol succ = data::detail::initialise_static_expression(succ, function_symbol(succ_name(), function_sort(pos(), pos())));
        return succ;
      }


      /// \brief Recogniser for function succ
      /// \param e A data expression
      /// \return true iff e is the function symbol matching succ
      inline
      bool is_succ_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == succ_name();
        }
        return false;
      }

      /// \brief Application of function symbol succ
      /// \param arg0 A data expression
      /// \return Application of succ to a number of arguments
      inline
      application succ(const data_expression& arg0)
      {
        return application(succ(),arg0);
      }

      /// \brief Recogniser for application of succ
      /// \param e A data expression
      /// \return true iff e is an application of function symbol succ to a
      ///     number of arguments
      inline
      bool is_succ_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_succ_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier +
      /// \return Identifier +
      inline
      core::identifier_string const& plus_name()
      {
        static core::identifier_string plus_name = data::detail::initialise_static_expression(plus_name, core::identifier_string("+"));
        return plus_name;
      }

      /// \brief Constructor for function symbol +
      /// \return Function symbol plus
      inline
      function_symbol const& plus()
      {
        static function_symbol plus = data::detail::initialise_static_expression(plus, function_symbol(plus_name(), function_sort(pos(), pos(), pos())));
        return plus;
      }


      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_plus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == plus_name();
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
        return application(plus(),arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol plus to a
      ///     number of arguments
      inline
      bool is_plus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_plus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@addc
      /// \return Identifier \@addc
      inline
      core::identifier_string const& add_with_carry_name()
      {
        static core::identifier_string add_with_carry_name = data::detail::initialise_static_expression(add_with_carry_name, core::identifier_string("@addc"));
        return add_with_carry_name;
      }

      /// \brief Constructor for function symbol \@addc
      /// \return Function symbol add_with_carry
      inline
      function_symbol const& add_with_carry()
      {
        static function_symbol add_with_carry = data::detail::initialise_static_expression(add_with_carry, function_symbol(add_with_carry_name(), function_sort(sort_bool_::bool_(), pos(), pos(), pos())));
        return add_with_carry;
      }


      /// \brief Recogniser for function \@addc
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@addc
      inline
      bool is_add_with_carry_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == add_with_carry_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@addc
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@addc to a number of arguments
      inline
      application add_with_carry(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(add_with_carry(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@addc
      /// \param e A data expression
      /// \return true iff e is an application of function symbol add_with_carry to a
      ///     number of arguments
      inline
      bool is_add_with_carry_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_add_with_carry_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& times_name()
      {
        static core::identifier_string times_name = data::detail::initialise_static_expression(times_name, core::identifier_string("*"));
        return times_name;
      }

      /// \brief Constructor for function symbol *
      /// \return Function symbol times
      inline
      function_symbol const& times()
      {
        static function_symbol times = data::detail::initialise_static_expression(times, function_symbol(times_name(), function_sort(pos(), pos(), pos())));
        return times;
      }


      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_times_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == times_name();
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
        return application(times(),arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol times to a
      ///     number of arguments
      inline
      bool is_times_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_times_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@multir
      /// \return Identifier \@multir
      inline
      core::identifier_string const& multir_name()
      {
        static core::identifier_string multir_name = data::detail::initialise_static_expression(multir_name, core::identifier_string("@multir"));
        return multir_name;
      }

      /// \brief Constructor for function symbol \@multir
      /// \return Function symbol multir
      inline
      function_symbol const& multir()
      {
        static function_symbol multir = data::detail::initialise_static_expression(multir, function_symbol(multir_name(), function_sort(sort_bool_::bool_(), pos(), pos(), pos(), pos())));
        return multir;
      }


      /// \brief Recogniser for function \@multir
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@multir
      inline
      bool is_multir_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == multir_name();
        }
        return false;
      }

      /// \brief Application of function symbol \@multir
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@multir to a number of arguments
      inline
      application multir(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(multir(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@multir
      /// \param e A data expression
      /// \return true iff e is an application of function symbol multir to a
      ///     number of arguments
      inline
      bool is_multir_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_multir_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for pos
      /// \return All system defined mappings for pos
      inline
      function_symbol_vector pos_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(maximum());
        result.push_back(minimum());
        result.push_back(abs());
        result.push_back(succ());
        result.push_back(plus());
        result.push_back(add_with_carry());
        result.push_back(times());
        result.push_back(multir());
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
        assert(is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_add_with_carry_application(e) || is_times_application(e));
        if (is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_times_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_add_with_carry_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        throw mcrl2::runtime_error("Unexpected expression " + pp(e) + " occurred");
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_multir_application(e));
        return static_cast< application >(e).arguments()[1];
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_multir_application(e));
        return static_cast< application >(e).arguments()[2];
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_multir_application(e));
        return static_cast< application >(e).arguments()[3];
      }

      ///\brief Function for projecting out argument
      ///        number from an application
      /// \param e A data expression
      /// \pre number is defined for e
      /// \return The argument of e that corresponds to number
      inline
      data_expression number(const data_expression& e)
      {
        assert(is_cdub_application(e) || is_abs_application(e) || is_succ_application(e));
        if (is_abs_application(e) || is_succ_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_cdub_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        throw mcrl2::runtime_error("Unexpected expression " + pp(e) + " occurred");
      }

      ///\brief Function for projecting out argument
      ///        bit from an application
      /// \param e A data expression
      /// \pre bit is defined for e
      /// \return The argument of e that corresponds to bit
      inline
      data_expression bit(const data_expression& e)
      {
        assert(is_cdub_application(e) || is_add_with_carry_application(e) || is_multir_application(e));
        return static_cast< application >(e).arguments()[0];
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_add_with_carry_application(e) || is_times_application(e));
        if (is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_times_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_add_with_carry_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        throw mcrl2::runtime_error("Unexpected expression " + pp(e) + " occurred");
      }

      /// \brief Give all system defined equations for pos
      /// \return All system defined equations for sort pos
      inline
      data_equation_vector pos_generate_equations_code()
      {
        variable vb("b",sort_bool_::bool_());
        variable vc("c",sort_bool_::bool_());
        variable vp("p",pos());
        variable vq("q",pos());
        variable vr("r",pos());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vb, vp), equal_to(c1(), cdub(vb, vp)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vb, vp), equal_to(cdub(vb, vp), c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp, vq), equal_to(cdub(sort_bool_::false_(), vp), cdub(sort_bool_::true_(), vq)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp, vq), equal_to(cdub(sort_bool_::true_(), vp), cdub(sort_bool_::false_(), vq)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vb, vp, vq), equal_to(cdub(vb, vp), cdub(vb, vq)), equal_to(vp, vq)));
        result.push_back(data_equation(make_vector(vb, vc, vp, vq), equal_to(cdub(vb, vp), cdub(vc, vq)), sort_bool_::and_(equal_to(vb, vc), equal_to(vp, vq))));
        result.push_back(data_equation(make_vector(vp), less(vp, c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vb, vp), less(c1(), cdub(vb, vp)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vb, vp, vq), less(cdub(vb, vp), cdub(vb, vq)), less(vp, vq)));
        result.push_back(data_equation(make_vector(vp, vq), less(cdub(sort_bool_::false_(), vp), cdub(sort_bool_::true_(), vq)), less_equal(vp, vq)));
        result.push_back(data_equation(make_vector(vb, vp, vq), less(cdub(vb, vp), cdub(sort_bool_::false_(), vq)), less(vp, vq)));
        result.push_back(data_equation(make_vector(vb, vc, vp, vq), less(cdub(vb, vp), cdub(vc, vq)), if_(sort_bool_::implies(vc, vb), less(vp, vq), less_equal(vp, vq))));
        result.push_back(data_equation(make_vector(vp), less_equal(c1(), vp), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vb, vp), less_equal(cdub(vb, vp), c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vb, vp, vq), less_equal(cdub(vb, vp), cdub(vb, vq)), less_equal(vp, vq)));
        result.push_back(data_equation(make_vector(vb, vp, vq), less_equal(cdub(sort_bool_::false_(), vp), cdub(vb, vq)), less_equal(vp, vq)));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(cdub(sort_bool_::true_(), vp), cdub(sort_bool_::false_(), vq)), less(vp, vq)));
        result.push_back(data_equation(make_vector(vb, vc, vp, vq), less_equal(cdub(vb, vp), cdub(vc, vq)), if_(sort_bool_::implies(vb, vc), less_equal(vp, vq), less(vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), maximum(vp, vq), if_(less_equal(vp, vq), vq, vp)));
        result.push_back(data_equation(make_vector(vp, vq), minimum(vp, vq), if_(less_equal(vp, vq), vp, vq)));
        result.push_back(data_equation(make_vector(vp), abs(vp), vp));
        result.push_back(data_equation(variable_list(), succ(c1()), cdub(sort_bool_::false_(), c1())));
        result.push_back(data_equation(make_vector(vp), succ(cdub(sort_bool_::false_(), vp)), cdub(sort_bool_::true_(), vp)));
        result.push_back(data_equation(make_vector(vp), succ(cdub(sort_bool_::true_(), vp)), cdub(sort_bool_::false_(), succ(vp))));
        result.push_back(data_equation(make_vector(vp, vq), plus(vp, vq), add_with_carry(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vp), add_with_carry(sort_bool_::false_(), c1(), vp), succ(vp)));
        result.push_back(data_equation(make_vector(vp), add_with_carry(sort_bool_::true_(), c1(), vp), succ(succ(vp))));
        result.push_back(data_equation(make_vector(vp), add_with_carry(sort_bool_::false_(), vp, c1()), succ(vp)));
        result.push_back(data_equation(make_vector(vp), add_with_carry(sort_bool_::true_(), vp, c1()), succ(succ(vp))));
        result.push_back(data_equation(make_vector(vb, vc, vp, vq), add_with_carry(vb, cdub(vc, vp), cdub(vc, vq)), cdub(vb, add_with_carry(vc, vp, vq))));
        result.push_back(data_equation(make_vector(vb, vp, vq), add_with_carry(vb, cdub(sort_bool_::false_(), vp), cdub(sort_bool_::true_(), vq)), cdub(sort_bool_::not_(vb), add_with_carry(vb, vp, vq))));
        result.push_back(data_equation(make_vector(vb, vp, vq), add_with_carry(vb, cdub(sort_bool_::true_(), vp), cdub(sort_bool_::false_(), vq)), cdub(sort_bool_::not_(vb), add_with_carry(vb, vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vp, vq), times(vp, vq), multir(sort_bool_::false_(), c1(), vp, vq)));
        result.push_back(data_equation(make_vector(vp, vq), less(vq, vp), times(vp, vq), multir(sort_bool_::false_(), c1(), vq, vp)));
        result.push_back(data_equation(make_vector(vp, vq), multir(sort_bool_::false_(), vp, c1(), vq), vq));
        result.push_back(data_equation(make_vector(vp, vq), multir(sort_bool_::true_(), vp, c1(), vq), add_with_carry(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vb, vp, vq, vr), multir(vb, vp, cdub(sort_bool_::false_(), vq), vr), multir(vb, vp, vq, cdub(sort_bool_::false_(), vr))));
        result.push_back(data_equation(make_vector(vp, vq, vr), multir(sort_bool_::false_(), vp, cdub(sort_bool_::true_(), vq), vr), multir(sort_bool_::true_(), vr, vq, cdub(sort_bool_::false_(), vr))));
        result.push_back(data_equation(make_vector(vp, vq, vr), multir(sort_bool_::true_(), vp, cdub(sort_bool_::true_(), vq), vr), multir(sort_bool_::true_(), add_with_carry(sort_bool_::false_(), vp, vr), vq, cdub(sort_bool_::false_(), vr))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for pos
      /// \param specification a specification
      inline
      void add_pos_to_specification(data_specification& specification)
      {
         if (specification.constructors(sort_bool_::bool_()).empty())
         {
           sort_bool_::add_bool_to_specification(specification);
         }
         specification.add_system_defined_sort(pos());
         specification.add_system_defined_constructors(pos_generate_constructors_code());
         specification.add_system_defined_mappings(pos_generate_functions_code());
         specification.add_system_defined_equations(pos_generate_equations_code());
      }
    } // namespace sort_pos

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_POS_H
