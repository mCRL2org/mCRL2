// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/real.h
/// \brief The standard sort real_.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/real.spec.

#ifndef MCRL2_DATA_REAL_H
#define MCRL2_DATA_REAL_H

#include "boost/utility.hpp"

#include "mcrl2/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort real_
    namespace sort_real {

      inline
      core::identifier_string const& real_name()
      {
        static core::identifier_string real_name = data::detail::initialise_static_expression(real_name, core::identifier_string("Real"));
        return real_name;
      }

      /// \brief Constructor for sort expression Real
      /// \return Sort expression Real
      inline
      basic_sort const& real_()
      {
        static basic_sort real_ = data::detail::initialise_static_expression(real_, basic_sort(real_name()));
        return real_;
      }

      /// \brief Recogniser for sort expression Real
      /// \param e A sort expression
      /// \return true iff e == real_()
      inline
      bool is_real(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast< basic_sort >(e) == real_();
        }
        return false;
      }

      /// \brief Give all system defined constructors for real_
      /// \return All system defined constructors for real_
      inline
      function_symbol_vector real_generate_constructors_code()
      {
        function_symbol_vector result;
        return result;
      }
      /// \brief Generate identifier \@cReal
      /// \return Identifier \@cReal
      inline
      core::identifier_string const& creal_name()
      {
        static core::identifier_string creal_name = data::detail::initialise_static_expression(creal_name, core::identifier_string("@cReal"));
        return creal_name;
      }

      /// \brief Constructor for function symbol \@cReal
      /// \return Function symbol creal
      inline
      function_symbol const& creal()
      {
        static function_symbol creal = data::detail::initialise_static_expression(creal, function_symbol(creal_name(), function_sort(sort_int::int_(), sort_pos::pos(), real_())));
        return creal;
      }


      /// \brief Recogniser for function \@cReal
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cReal
      inline
      bool is_creal_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == creal();
        }
        return false;
      }

      /// \brief Application of function symbol \@cReal
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@cReal to a number of arguments
      inline
      application creal(const data_expression& arg0, const data_expression& arg1)
      {
        return application(creal(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@cReal
      /// \param e A data expression
      /// \return true iff e is an application of function symbol creal to a
      ///     number of arguments
      inline
      bool is_creal_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_creal_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Pos2Real
      /// \return Identifier Pos2Real
      inline
      core::identifier_string const& pos2real_name()
      {
        static core::identifier_string pos2real_name = data::detail::initialise_static_expression(pos2real_name, core::identifier_string("Pos2Real"));
        return pos2real_name;
      }

      /// \brief Constructor for function symbol Pos2Real
      /// \return Function symbol pos2real
      inline
      function_symbol const& pos2real()
      {
        static function_symbol pos2real = data::detail::initialise_static_expression(pos2real, function_symbol(pos2real_name(), function_sort(sort_pos::pos(), real_())));
        return pos2real;
      }


      /// \brief Recogniser for function Pos2Real
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Pos2Real
      inline
      bool is_pos2real_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == pos2real();
        }
        return false;
      }

      /// \brief Application of function symbol Pos2Real
      /// \param arg0 A data expression
      /// \return Application of Pos2Real to a number of arguments
      inline
      application pos2real(const data_expression& arg0)
      {
        return application(pos2real(),arg0);
      }

      /// \brief Recogniser for application of Pos2Real
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos2real to a
      ///     number of arguments
      inline
      bool is_pos2real_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_pos2real_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Nat2Real
      /// \return Identifier Nat2Real
      inline
      core::identifier_string const& nat2real_name()
      {
        static core::identifier_string nat2real_name = data::detail::initialise_static_expression(nat2real_name, core::identifier_string("Nat2Real"));
        return nat2real_name;
      }

      /// \brief Constructor for function symbol Nat2Real
      /// \return Function symbol nat2real
      inline
      function_symbol const& nat2real()
      {
        static function_symbol nat2real = data::detail::initialise_static_expression(nat2real, function_symbol(nat2real_name(), function_sort(sort_nat::nat(), real_())));
        return nat2real;
      }


      /// \brief Recogniser for function Nat2Real
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Nat2Real
      inline
      bool is_nat2real_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == nat2real();
        }
        return false;
      }

      /// \brief Application of function symbol Nat2Real
      /// \param arg0 A data expression
      /// \return Application of Nat2Real to a number of arguments
      inline
      application nat2real(const data_expression& arg0)
      {
        return application(nat2real(),arg0);
      }

      /// \brief Recogniser for application of Nat2Real
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2real to a
      ///     number of arguments
      inline
      bool is_nat2real_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_nat2real_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Int2Real
      /// \return Identifier Int2Real
      inline
      core::identifier_string const& int2real_name()
      {
        static core::identifier_string int2real_name = data::detail::initialise_static_expression(int2real_name, core::identifier_string("Int2Real"));
        return int2real_name;
      }

      /// \brief Constructor for function symbol Int2Real
      /// \return Function symbol int2real
      inline
      function_symbol const& int2real()
      {
        static function_symbol int2real = data::detail::initialise_static_expression(int2real, function_symbol(int2real_name(), function_sort(sort_int::int_(), real_())));
        return int2real;
      }


      /// \brief Recogniser for function Int2Real
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Int2Real
      inline
      bool is_int2real_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == int2real();
        }
        return false;
      }

      /// \brief Application of function symbol Int2Real
      /// \param arg0 A data expression
      /// \return Application of Int2Real to a number of arguments
      inline
      application int2real(const data_expression& arg0)
      {
        return application(int2real(),arg0);
      }

      /// \brief Recogniser for application of Int2Real
      /// \param e A data expression
      /// \return true iff e is an application of function symbol int2real to a
      ///     number of arguments
      inline
      bool is_int2real_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_int2real_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Real2Pos
      /// \return Identifier Real2Pos
      inline
      core::identifier_string const& real2pos_name()
      {
        static core::identifier_string real2pos_name = data::detail::initialise_static_expression(real2pos_name, core::identifier_string("Real2Pos"));
        return real2pos_name;
      }

      /// \brief Constructor for function symbol Real2Pos
      /// \return Function symbol real2pos
      inline
      function_symbol const& real2pos()
      {
        static function_symbol real2pos = data::detail::initialise_static_expression(real2pos, function_symbol(real2pos_name(), function_sort(real_(), sort_pos::pos())));
        return real2pos;
      }


      /// \brief Recogniser for function Real2Pos
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Real2Pos
      inline
      bool is_real2pos_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == real2pos();
        }
        return false;
      }

      /// \brief Application of function symbol Real2Pos
      /// \param arg0 A data expression
      /// \return Application of Real2Pos to a number of arguments
      inline
      application real2pos(const data_expression& arg0)
      {
        return application(real2pos(),arg0);
      }

      /// \brief Recogniser for application of Real2Pos
      /// \param e A data expression
      /// \return true iff e is an application of function symbol real2pos to a
      ///     number of arguments
      inline
      bool is_real2pos_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_real2pos_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Real2Nat
      /// \return Identifier Real2Nat
      inline
      core::identifier_string const& real2nat_name()
      {
        static core::identifier_string real2nat_name = data::detail::initialise_static_expression(real2nat_name, core::identifier_string("Real2Nat"));
        return real2nat_name;
      }

      /// \brief Constructor for function symbol Real2Nat
      /// \return Function symbol real2nat
      inline
      function_symbol const& real2nat()
      {
        static function_symbol real2nat = data::detail::initialise_static_expression(real2nat, function_symbol(real2nat_name(), function_sort(real_(), sort_nat::nat())));
        return real2nat;
      }


      /// \brief Recogniser for function Real2Nat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Real2Nat
      inline
      bool is_real2nat_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == real2nat();
        }
        return false;
      }

      /// \brief Application of function symbol Real2Nat
      /// \param arg0 A data expression
      /// \return Application of Real2Nat to a number of arguments
      inline
      application real2nat(const data_expression& arg0)
      {
        return application(real2nat(),arg0);
      }

      /// \brief Recogniser for application of Real2Nat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol real2nat to a
      ///     number of arguments
      inline
      bool is_real2nat_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_real2nat_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Real2Int
      /// \return Identifier Real2Int
      inline
      core::identifier_string const& real2int_name()
      {
        static core::identifier_string real2int_name = data::detail::initialise_static_expression(real2int_name, core::identifier_string("Real2Int"));
        return real2int_name;
      }

      /// \brief Constructor for function symbol Real2Int
      /// \return Function symbol real2int
      inline
      function_symbol const& real2int()
      {
        static function_symbol real2int = data::detail::initialise_static_expression(real2int, function_symbol(real2int_name(), function_sort(real_(), sort_int::int_())));
        return real2int;
      }


      /// \brief Recogniser for function Real2Int
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Real2Int
      inline
      bool is_real2int_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == real2int();
        }
        return false;
      }

      /// \brief Application of function symbol Real2Int
      /// \param arg0 A data expression
      /// \return Application of Real2Int to a number of arguments
      inline
      application real2int(const data_expression& arg0)
      {
        return application(real2int(),arg0);
      }

      /// \brief Recogniser for application of Real2Int
      /// \param e A data expression
      /// \return true iff e is an application of function symbol real2int to a
      ///     number of arguments
      inline
      bool is_real2int_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_real2int_function_symbol(static_cast< application >(e).head());
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

      ///\brief Constructor for function symbol min
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol minimum
      inline
      function_symbol minimum(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_() && s1 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol minimum(minimum_name(), function_sort(s0, s1, target_sort));
        return minimum;
      }

      /// \brief Recogniser for function min
      /// \param e A data expression
      /// \return true iff e is the function symbol matching min
      inline
      bool is_minimum_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == minimum_name() && function_sort(f.sort()).domain().size() == 2 && (f == minimum(real_(), real_()) || f == minimum(sort_int::int_(), sort_int::int_()) || f == minimum(sort_nat::nat(), sort_nat::nat()) || f == minimum(sort_pos::pos(), sort_pos::pos()));
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
        return application(minimum(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of min
      /// \param e A data expression
      /// \return true iff e is an application of function symbol minimum to a
      ///     number of arguments
      inline
      bool is_minimum_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_minimum_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier max
      /// \return Identifier max
      inline
      core::identifier_string const& maximum_name()
      {
        static core::identifier_string maximum_name = data::detail::initialise_static_expression(maximum_name, core::identifier_string("max"));
        return maximum_name;
      }

      ///\brief Constructor for function symbol max
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol maximum
      inline
      function_symbol maximum(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_int::int_())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_int::int_() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_int::int_())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_int::int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_int::int_() && s1 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol maximum(maximum_name(), function_sort(s0, s1, target_sort));
        return maximum;
      }

      /// \brief Recogniser for function max
      /// \param e A data expression
      /// \return true iff e is the function symbol matching max
      inline
      bool is_maximum_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == maximum_name() && function_sort(f.sort()).domain().size() == 2 && (f == maximum(real_(), real_()) || f == maximum(sort_pos::pos(), sort_int::int_()) || f == maximum(sort_int::int_(), sort_pos::pos()) || f == maximum(sort_nat::nat(), sort_int::int_()) || f == maximum(sort_int::int_(), sort_nat::nat()) || f == maximum(sort_int::int_(), sort_int::int_()) || f == maximum(sort_pos::pos(), sort_nat::nat()) || f == maximum(sort_nat::nat(), sort_pos::pos()) || f == maximum(sort_nat::nat(), sort_nat::nat()) || f == maximum(sort_pos::pos(), sort_pos::pos()));
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
        return application(maximum(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of max
      /// \param e A data expression
      /// \return true iff e is an application of function symbol maximum to a
      ///     number of arguments
      inline
      bool is_maximum_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_maximum_function_symbol(static_cast< application >(e).head());
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

      ///\brief Constructor for function symbol abs
      /// \param s0 A sort expression
      ///\return Function symbol abs
      inline
      function_symbol abs(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol abs(abs_name(), function_sort(s0, target_sort));
        return abs;
      }

      /// \brief Recogniser for function abs
      /// \param e A data expression
      /// \return true iff e is the function symbol matching abs
      inline
      bool is_abs_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == abs_name() && function_sort(f.sort()).domain().size() == 1 && (f == abs(real_()) || f == abs(sort_int::int_()) || f == abs(sort_nat::nat()) || f == abs(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol abs
      /// \param arg0 A data expression
      /// \return Application of abs to a number of arguments
      inline
      application abs(const data_expression& arg0)
      {
        return application(abs(arg0.sort()),arg0);
      }

      /// \brief Recogniser for application of abs
      /// \param e A data expression
      /// \return true iff e is an application of function symbol abs to a
      ///     number of arguments
      inline
      bool is_abs_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_abs_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& negate_name()
      {
        static core::identifier_string negate_name = data::detail::initialise_static_expression(negate_name, core::identifier_string("-"));
        return negate_name;
      }

      ///\brief Constructor for function symbol -
      /// \param s0 A sort expression
      ///\return Function symbol negate
      inline
      function_symbol negate(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_pos::pos())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_nat::nat())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else
        {
          assert(false);
        }

        function_symbol negate(negate_name(), function_sort(s0, target_sort));
        return negate;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_negate_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == negate_name() && function_sort(f.sort()).domain().size() == 1 && (f == negate(real_()) || f == negate(sort_pos::pos()) || f == negate(sort_nat::nat()) || f == negate(sort_int::int_()));
        }
        return false;
      }

      /// \brief Application of function symbol -
      /// \param arg0 A data expression
      /// \return Application of - to a number of arguments
      inline
      application negate(const data_expression& arg0)
      {
        return application(negate(arg0.sort()),arg0);
      }

      /// \brief Recogniser for application of -
      /// \param e A data expression
      /// \return true iff e is an application of function symbol negate to a
      ///     number of arguments
      inline
      bool is_negate_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_negate_function_symbol(static_cast< application >(e).head());
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

      ///\brief Constructor for function symbol succ
      /// \param s0 A sort expression
      ///\return Function symbol succ
      inline
      function_symbol succ(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol succ(succ_name(), function_sort(s0, target_sort));
        return succ;
      }

      /// \brief Recogniser for function succ
      /// \param e A data expression
      /// \return true iff e is the function symbol matching succ
      inline
      bool is_succ_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == succ_name() && function_sort(f.sort()).domain().size() == 1 && (f == succ(real_()) || f == succ(sort_int::int_()) || f == succ(sort_nat::nat()) || f == succ(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol succ
      /// \param arg0 A data expression
      /// \return Application of succ to a number of arguments
      inline
      application succ(const data_expression& arg0)
      {
        return application(succ(arg0.sort()),arg0);
      }

      /// \brief Recogniser for application of succ
      /// \param e A data expression
      /// \return true iff e is an application of function symbol succ to a
      ///     number of arguments
      inline
      bool is_succ_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_succ_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier pred
      /// \return Identifier pred
      inline
      core::identifier_string const& pred_name()
      {
        static core::identifier_string pred_name = data::detail::initialise_static_expression(pred_name, core::identifier_string("pred"));
        return pred_name;
      }

      ///\brief Constructor for function symbol pred
      /// \param s0 A sort expression
      ///\return Function symbol pred
      inline
      function_symbol pred(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_nat::nat())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        function_symbol pred(pred_name(), function_sort(s0, target_sort));
        return pred;
      }

      /// \brief Recogniser for function pred
      /// \param e A data expression
      /// \return true iff e is the function symbol matching pred
      inline
      bool is_pred_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == pred_name() && function_sort(f.sort()).domain().size() == 1 && (f == pred(real_()) || f == pred(sort_nat::nat()) || f == pred(sort_int::int_()) || f == pred(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol pred
      /// \param arg0 A data expression
      /// \return Application of pred to a number of arguments
      inline
      application pred(const data_expression& arg0)
      {
        return application(pred(arg0.sort()),arg0);
      }

      /// \brief Recogniser for application of pred
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pred to a
      ///     number of arguments
      inline
      bool is_pred_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_pred_function_symbol(static_cast< application >(e).head());
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

      ///\brief Constructor for function symbol +
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol plus
      inline
      function_symbol plus(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_() && s1 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol plus(plus_name(), function_sort(s0, s1, target_sort));
        return plus;
      }

      /// \brief Recogniser for function +
      /// \param e A data expression
      /// \return true iff e is the function symbol matching +
      inline
      bool is_plus_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == plus_name() && function_sort(f.sort()).domain().size() == 2 && (f == plus(real_(), real_()) || f == plus(sort_int::int_(), sort_int::int_()) || f == plus(sort_pos::pos(), sort_nat::nat()) || f == plus(sort_nat::nat(), sort_pos::pos()) || f == plus(sort_nat::nat(), sort_nat::nat()) || f == plus(sort_pos::pos(), sort_pos::pos()));
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
        return application(plus(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of +
      /// \param e A data expression
      /// \return true iff e is an application of function symbol plus to a
      ///     number of arguments
      inline
      bool is_plus_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_plus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& minus_name()
      {
        static core::identifier_string minus_name = data::detail::initialise_static_expression(minus_name, core::identifier_string("-"));
        return minus_name;
      }

      ///\brief Constructor for function symbol -
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol minus
      inline
      function_symbol minus(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_int::int_() && s1 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else
        {
          assert(false);
        }

        function_symbol minus(minus_name(), function_sort(s0, s1, target_sort));
        return minus;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_minus_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == minus_name() && function_sort(f.sort()).domain().size() == 2 && (f == minus(real_(), real_()) || f == minus(sort_pos::pos(), sort_pos::pos()) || f == minus(sort_nat::nat(), sort_nat::nat()) || f == minus(sort_int::int_(), sort_int::int_()));
        }
        return false;
      }

      /// \brief Application of function symbol -
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of - to a number of arguments
      inline
      application minus(const data_expression& arg0, const data_expression& arg1)
      {
        return application(minus(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of -
      /// \param e A data expression
      /// \return true iff e is an application of function symbol minus to a
      ///     number of arguments
      inline
      bool is_minus_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_minus_function_symbol(static_cast< application >(e).head());
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

      ///\brief Constructor for function symbol *
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol times
      inline
      function_symbol times(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == real_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_() && s1 == sort_int::int_())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        function_symbol times(times_name(), function_sort(s0, s1, target_sort));
        return times;
      }

      /// \brief Recogniser for function *
      /// \param e A data expression
      /// \return true iff e is the function symbol matching *
      inline
      bool is_times_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == times_name() && function_sort(f.sort()).domain().size() == 2 && (f == times(real_(), real_()) || f == times(sort_int::int_(), sort_int::int_()) || f == times(sort_nat::nat(), sort_nat::nat()) || f == times(sort_pos::pos(), sort_pos::pos()));
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
        return application(times(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of *
      /// \param e A data expression
      /// \return true iff e is an application of function symbol times to a
      ///     number of arguments
      inline
      bool is_times_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_times_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier exp
      /// \return Identifier exp
      inline
      core::identifier_string const& exp_name()
      {
        static core::identifier_string exp_name = data::detail::initialise_static_expression(exp_name, core::identifier_string("exp"));
        return exp_name;
      }

      ///\brief Constructor for function symbol exp
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol exp
      inline
      function_symbol exp(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == real_() && s1 == sort_int::int_())
        {
          target_sort = real_();
        }
        else if (s0 == sort_int::int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_int::int_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        function_symbol exp(exp_name(), function_sort(s0, s1, target_sort));
        return exp;
      }

      /// \brief Recogniser for function exp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching exp
      inline
      bool is_exp_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == exp_name() && function_sort(f.sort()).domain().size() == 2 && (f == exp(real_(), sort_int::int_()) || f == exp(sort_int::int_(), sort_nat::nat()) || f == exp(sort_pos::pos(), sort_nat::nat()) || f == exp(sort_nat::nat(), sort_nat::nat()));
        }
        return false;
      }

      /// \brief Application of function symbol exp
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of exp to a number of arguments
      inline
      application exp(const data_expression& arg0, const data_expression& arg1)
      {
        return application(exp(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of exp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol exp to a
      ///     number of arguments
      inline
      bool is_exp_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_exp_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier /
      /// \return Identifier /
      inline
      core::identifier_string const& divides_name()
      {
        static core::identifier_string divides_name = data::detail::initialise_static_expression(divides_name, core::identifier_string("/"));
        return divides_name;
      }

      ///\brief Constructor for function symbol /
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol divides
      inline
      function_symbol divides(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(real_());

        function_symbol divides(divides_name(), function_sort(s0, s1, target_sort));
        return divides;
      }

      /// \brief Recogniser for function /
      /// \param e A data expression
      /// \return true iff e is the function symbol matching /
      inline
      bool is_divides_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == divides_name() && function_sort(f.sort()).domain().size() == 2 && (f == divides(sort_pos::pos(), sort_pos::pos()) || f == divides(sort_nat::nat(), sort_nat::nat()) || f == divides(sort_int::int_(), sort_int::int_()) || f == divides(real_(), real_()));
        }
        return false;
      }

      /// \brief Application of function symbol /
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of / to a number of arguments
      inline
      application divides(const data_expression& arg0, const data_expression& arg1)
      {
        return application(divides(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of /
      /// \param e A data expression
      /// \return true iff e is an application of function symbol divides to a
      ///     number of arguments
      inline
      bool is_divides_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_divides_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier floor
      /// \return Identifier floor
      inline
      core::identifier_string const& floor_name()
      {
        static core::identifier_string floor_name = data::detail::initialise_static_expression(floor_name, core::identifier_string("floor"));
        return floor_name;
      }

      /// \brief Constructor for function symbol floor
      /// \return Function symbol floor
      inline
      function_symbol const& floor()
      {
        static function_symbol floor = data::detail::initialise_static_expression(floor, function_symbol(floor_name(), function_sort(real_(), sort_int::int_())));
        return floor;
      }


      /// \brief Recogniser for function floor
      /// \param e A data expression
      /// \return true iff e is the function symbol matching floor
      inline
      bool is_floor_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == floor();
        }
        return false;
      }

      /// \brief Application of function symbol floor
      /// \param arg0 A data expression
      /// \return Application of floor to a number of arguments
      inline
      application floor(const data_expression& arg0)
      {
        return application(floor(),arg0);
      }

      /// \brief Recogniser for application of floor
      /// \param e A data expression
      /// \return true iff e is an application of function symbol floor to a
      ///     number of arguments
      inline
      bool is_floor_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_floor_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier ceil
      /// \return Identifier ceil
      inline
      core::identifier_string const& ceil_name()
      {
        static core::identifier_string ceil_name = data::detail::initialise_static_expression(ceil_name, core::identifier_string("ceil"));
        return ceil_name;
      }

      /// \brief Constructor for function symbol ceil
      /// \return Function symbol ceil
      inline
      function_symbol const& ceil()
      {
        static function_symbol ceil = data::detail::initialise_static_expression(ceil, function_symbol(ceil_name(), function_sort(real_(), sort_int::int_())));
        return ceil;
      }


      /// \brief Recogniser for function ceil
      /// \param e A data expression
      /// \return true iff e is the function symbol matching ceil
      inline
      bool is_ceil_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == ceil();
        }
        return false;
      }

      /// \brief Application of function symbol ceil
      /// \param arg0 A data expression
      /// \return Application of ceil to a number of arguments
      inline
      application ceil(const data_expression& arg0)
      {
        return application(ceil(),arg0);
      }

      /// \brief Recogniser for application of ceil
      /// \param e A data expression
      /// \return true iff e is an application of function symbol ceil to a
      ///     number of arguments
      inline
      bool is_ceil_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_ceil_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier round
      /// \return Identifier round
      inline
      core::identifier_string const& round_name()
      {
        static core::identifier_string round_name = data::detail::initialise_static_expression(round_name, core::identifier_string("round"));
        return round_name;
      }

      /// \brief Constructor for function symbol round
      /// \return Function symbol round
      inline
      function_symbol const& round()
      {
        static function_symbol round = data::detail::initialise_static_expression(round, function_symbol(round_name(), function_sort(real_(), sort_int::int_())));
        return round;
      }


      /// \brief Recogniser for function round
      /// \param e A data expression
      /// \return true iff e is the function symbol matching round
      inline
      bool is_round_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == round();
        }
        return false;
      }

      /// \brief Application of function symbol round
      /// \param arg0 A data expression
      /// \return Application of round to a number of arguments
      inline
      application round(const data_expression& arg0)
      {
        return application(round(),arg0);
      }

      /// \brief Recogniser for application of round
      /// \param e A data expression
      /// \return true iff e is an application of function symbol round to a
      ///     number of arguments
      inline
      bool is_round_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_round_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@redfrac
      /// \return Identifier \@redfrac
      inline
      core::identifier_string const& redfrac_name()
      {
        static core::identifier_string redfrac_name = data::detail::initialise_static_expression(redfrac_name, core::identifier_string("@redfrac"));
        return redfrac_name;
      }

      /// \brief Constructor for function symbol \@redfrac
      /// \return Function symbol redfrac
      inline
      function_symbol const& redfrac()
      {
        static function_symbol redfrac = data::detail::initialise_static_expression(redfrac, function_symbol(redfrac_name(), function_sort(sort_int::int_(), sort_int::int_(), real_())));
        return redfrac;
      }


      /// \brief Recogniser for function \@redfrac
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@redfrac
      inline
      bool is_redfrac_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == redfrac();
        }
        return false;
      }

      /// \brief Application of function symbol \@redfrac
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@redfrac to a number of arguments
      inline
      application redfrac(const data_expression& arg0, const data_expression& arg1)
      {
        return application(redfrac(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@redfrac
      /// \param e A data expression
      /// \return true iff e is an application of function symbol redfrac to a
      ///     number of arguments
      inline
      bool is_redfrac_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_redfrac_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@redfracwhr
      /// \return Identifier \@redfracwhr
      inline
      core::identifier_string const& redfracwhr_name()
      {
        static core::identifier_string redfracwhr_name = data::detail::initialise_static_expression(redfracwhr_name, core::identifier_string("@redfracwhr"));
        return redfracwhr_name;
      }

      /// \brief Constructor for function symbol \@redfracwhr
      /// \return Function symbol redfracwhr
      inline
      function_symbol const& redfracwhr()
      {
        static function_symbol redfracwhr = data::detail::initialise_static_expression(redfracwhr, function_symbol(redfracwhr_name(), function_sort(sort_pos::pos(), sort_int::int_(), sort_nat::nat(), real_())));
        return redfracwhr;
      }


      /// \brief Recogniser for function \@redfracwhr
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@redfracwhr
      inline
      bool is_redfracwhr_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == redfracwhr();
        }
        return false;
      }

      /// \brief Application of function symbol \@redfracwhr
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@redfracwhr to a number of arguments
      inline
      application redfracwhr(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(redfracwhr(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@redfracwhr
      /// \param e A data expression
      /// \return true iff e is an application of function symbol redfracwhr to a
      ///     number of arguments
      inline
      bool is_redfracwhr_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_redfracwhr_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@redfrachlp
      /// \return Identifier \@redfrachlp
      inline
      core::identifier_string const& redfrachlp_name()
      {
        static core::identifier_string redfrachlp_name = data::detail::initialise_static_expression(redfrachlp_name, core::identifier_string("@redfrachlp"));
        return redfrachlp_name;
      }

      /// \brief Constructor for function symbol \@redfrachlp
      /// \return Function symbol redfrachlp
      inline
      function_symbol const& redfrachlp()
      {
        static function_symbol redfrachlp = data::detail::initialise_static_expression(redfrachlp, function_symbol(redfrachlp_name(), function_sort(real_(), sort_int::int_(), real_())));
        return redfrachlp;
      }


      /// \brief Recogniser for function \@redfrachlp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@redfrachlp
      inline
      bool is_redfrachlp_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == redfrachlp();
        }
        return false;
      }

      /// \brief Application of function symbol \@redfrachlp
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@redfrachlp to a number of arguments
      inline
      application redfrachlp(const data_expression& arg0, const data_expression& arg1)
      {
        return application(redfrachlp(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@redfrachlp
      /// \param e A data expression
      /// \return true iff e is an application of function symbol redfrachlp to a
      ///     number of arguments
      inline
      bool is_redfrachlp_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_redfrachlp_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for real_
      /// \return All system defined mappings for real_
      inline
      function_symbol_vector real_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(creal());
        result.push_back(pos2real());
        result.push_back(nat2real());
        result.push_back(int2real());
        result.push_back(real2pos());
        result.push_back(real2nat());
        result.push_back(real2int());
        result.push_back(minimum(real_(), real_()));
        result.push_back(maximum(real_(), real_()));
        result.push_back(abs(real_()));
        result.push_back(negate(real_()));
        result.push_back(succ(real_()));
        result.push_back(pred(real_()));
        result.push_back(plus(real_(), real_()));
        result.push_back(minus(real_(), real_()));
        result.push_back(times(real_(), real_()));
        result.push_back(exp(real_(), sort_int::int_()));
        result.push_back(divides(sort_pos::pos(), sort_pos::pos()));
        result.push_back(divides(sort_nat::nat(), sort_nat::nat()));
        result.push_back(divides(sort_int::int_(), sort_int::int_()));
        result.push_back(divides(real_(), real_()));
        result.push_back(floor());
        result.push_back(ceil());
        result.push_back(round());
        result.push_back(redfrac());
        result.push_back(redfracwhr());
        result.push_back(redfrachlp());
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
        assert(is_minimum_application(e) || is_maximum_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e) || is_divides_application(e) || is_redfrachlp_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_redfracwhr_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 2);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_redfracwhr_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_redfracwhr_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        denominator from an application
      /// \param e A data expression
      /// \pre denominator is defined for e
      /// \return The argument of e that corresponds to denominator
      inline
      data_expression denominator(const data_expression& e)
      {
        assert(is_creal_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        numerator from an application
      /// \param e A data expression
      /// \pre numerator is defined for e
      /// \return The argument of e that corresponds to numerator
      inline
      data_expression numerator(const data_expression& e)
      {
        assert(is_creal_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_pos2real_application(e) || is_nat2real_application(e) || is_int2real_application(e) || is_real2pos_application(e) || is_real2nat_application(e) || is_real2int_application(e) || is_abs_application(e) || is_negate_application(e) || is_succ_application(e) || is_pred_application(e) || is_floor_application(e) || is_ceil_application(e) || is_round_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_minimum_application(e) || is_maximum_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e) || is_divides_application(e) || is_redfrachlp_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      /// \brief Give all system defined equations for real_
      /// \return All system defined equations for sort real_
      inline
      data_equation_vector real_generate_equations_code()
      {
        variable vm("m",sort_nat::nat());
        variable vn("n",sort_nat::nat());
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vx("x",sort_int::int_());
        variable vy("y",sort_int::int_());
        variable vr("r",real_());
        variable vs("s",real_());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), equal_to(creal(vx, vp), creal(vy, vq)), equal_to(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp))))));
        result.push_back(data_equation(make_vector(vr), less(vr, vr), sort_bool::false_()));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), less(creal(vx, vp), creal(vy, vq)), less(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp))))));
        result.push_back(data_equation(make_vector(vr), less_equal(vr, vr), sort_bool::true_()));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), less_equal(creal(vx, vp), creal(vy, vq)), less_equal(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp))))));
        result.push_back(data_equation(make_vector(vx), int2real(vx), creal(vx, sort_pos::c1())));
        result.push_back(data_equation(make_vector(vn), nat2real(vn), creal(sort_int::cint(vn), sort_pos::c1())));
        result.push_back(data_equation(make_vector(vp), pos2real(vp), creal(sort_int::cint(sort_nat::cnat(vp)), sort_pos::c1())));
        result.push_back(data_equation(make_vector(vx), real2int(creal(vx, sort_pos::c1())), vx));
        result.push_back(data_equation(make_vector(vx), real2nat(creal(vx, sort_pos::c1())), sort_int::int2nat(vx)));
        result.push_back(data_equation(make_vector(vx), real2pos(creal(vx, sort_pos::c1())), sort_int::int2pos(vx)));
        result.push_back(data_equation(make_vector(vr, vs), minimum(vr, vs), if_(less(vr, vs), vr, vs)));
        result.push_back(data_equation(make_vector(vr, vs), maximum(vr, vs), if_(less(vr, vs), vs, vr)));
        result.push_back(data_equation(make_vector(vr), abs(vr), if_(less(vr, creal(sort_int::cint(sort_nat::c0()), sort_pos::c1())), negate(vr), vr)));
        result.push_back(data_equation(make_vector(vp, vx), negate(creal(vx, vp)), creal(negate(vx), vp)));
        result.push_back(data_equation(make_vector(vp, vx), succ(creal(vx, vp)), creal(plus(vx, sort_int::cint(sort_nat::cnat(vp))), vp)));
        result.push_back(data_equation(make_vector(vp, vx), pred(creal(vx, vp)), creal(minus(vx, sort_int::cint(sort_nat::cnat(vp))), vp)));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), plus(creal(vx, vp), creal(vy, vq)), redfrac(plus(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp)))), sort_int::cint(sort_nat::cnat(times(vp, vq))))));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), minus(creal(vx, vp), creal(vy, vq)), redfrac(minus(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp)))), sort_int::cint(sort_nat::cnat(times(vp, vq))))));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), times(creal(vx, vp), creal(vy, vq)), redfrac(times(vx, vy), sort_int::cint(sort_nat::cnat(times(vp, vq))))));
        result.push_back(data_equation(make_vector(vp, vq, vx, vy), not_equal_to(vy, sort_int::cint(sort_nat::c0())), divides(creal(vx, vp), creal(vy, vq)), redfrac(times(vx, sort_int::cint(sort_nat::cnat(vq))), times(vy, sort_int::cint(sort_nat::cnat(vp))))));
        result.push_back(data_equation(make_vector(vp, vq), divides(vp, vq), redfrac(sort_int::cint(sort_nat::cnat(vp)), sort_int::cint(sort_nat::cnat(vq)))));
        result.push_back(data_equation(make_vector(vm, vn), not_equal_to(vm, sort_nat::c0()), divides(vm, vn), redfrac(sort_int::cint(vm), sort_int::cint(vn))));
        result.push_back(data_equation(make_vector(vx, vy), not_equal_to(vy, sort_int::cint(sort_nat::c0())), divides(vx, vy), redfrac(vx, vy)));
        result.push_back(data_equation(make_vector(vn, vp, vx), exp(creal(vx, vp), sort_int::cint(vn)), redfrac(exp(vx, vn), sort_int::cint(sort_nat::cnat(exp(vp, vn))))));
        result.push_back(data_equation(make_vector(vp, vq, vx), not_equal_to(vx, sort_int::cint(sort_nat::c0())), exp(creal(vx, vp), sort_int::cneg(vq)), redfrac(sort_int::cint(sort_nat::cnat(exp(vp, sort_nat::cnat(vq)))), exp(vx, sort_nat::cnat(vq)))));
        result.push_back(data_equation(make_vector(vp, vx), floor(creal(vx, vp)), sort_int::div(vx, vp)));
        result.push_back(data_equation(make_vector(vr), ceil(vr), negate(floor(negate(vr)))));
        result.push_back(data_equation(make_vector(vr), round(vr), floor(plus(vr, creal(sort_int::cint(sort_nat::cnat(sort_pos::c1())), sort_pos::cdub(sort_bool::false_(), sort_pos::c1()))))));
        result.push_back(data_equation(make_vector(vp, vx), redfrac(vx, sort_int::cneg(vp)), redfrac(negate(vx), sort_int::cint(sort_nat::cnat(vp)))));
        result.push_back(data_equation(make_vector(vp, vx), redfrac(vx, sort_int::cint(sort_nat::cnat(vp))), redfracwhr(vp, sort_int::div(vx, vp), sort_int::mod(vx, vp))));
        result.push_back(data_equation(make_vector(vp, vx), redfracwhr(vp, vx, sort_nat::c0()), creal(vx, sort_pos::c1())));
        result.push_back(data_equation(make_vector(vp, vq, vx), redfracwhr(vp, vx, sort_nat::cnat(vq)), redfrachlp(redfrac(sort_int::cint(sort_nat::cnat(vp)), sort_int::cint(sort_nat::cnat(vq))), vx)));
        result.push_back(data_equation(make_vector(vp, vx, vy), redfrachlp(creal(vx, vp), vy), creal(plus(sort_int::cint(sort_nat::cnat(vp)), times(vy, vx)), sort_int::int2pos(vx))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for real_
      /// \param specification a specification
      /* template <typename SpecificationType>
      inline
      void add_real_to_specification(SpecificationType const& specification)
      {
         if (specification.constructors(sort_int::int_()).empty())
         {
           sort_int::add_int_to_specification(specification);
         }
         if (specification.constructors(sort_bool::bool_()).empty())
         {
           sort_bool::add_bool_to_specification(specification);
         }
         if (specification.constructors(sort_pos::pos()).empty())
         {
           sort_pos::add_pos_to_specification(specification);
         }
         if (specification.constructors(sort_nat::nat()).empty())
         {
           sort_nat::add_nat_to_specification(specification);
         }
         specification.add_system_defined_sort(real_());
         specification.add_system_defined_constructors(real_generate_constructors_code());
         specification.add_system_defined_mappings(real_generate_functions_code());
         specification.add_system_defined_equations(real_generate_equations_code());
      } */
    } // namespace sort_real_

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REAL_H
