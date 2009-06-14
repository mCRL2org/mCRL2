// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/int.h
/// \brief The standard sort int_.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/int.spec.

#ifndef MCRL2_DATA_INT__H
#define MCRL2_DATA_INT__H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort int_
    namespace sort_int_ {

      /// \brief Constructor for sort expression Int
      /// \return Sort expression Int
      inline
      basic_sort int_()
      {
        static basic_sort int_ = data::detail::initialise_static_expression(int_, basic_sort("Int"));
        return int_;
      }

      /// \brief Recogniser for sort expression Int
      /// \param e A sort expression
      /// \return true iff e == int_()
      inline
      bool is_int_(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast< basic_sort >(e) == int_();
        }
        return false;
      }

      /// \brief Constructor for function symbol \@cInt
      /// \return Function symbol cint
      inline
      function_symbol cint()
      {
        static function_symbol cint = data::detail::initialise_static_expression(cint, function_symbol("@cInt", function_sort(sort_nat::nat(), int_())));
        return cint;
      }

      /// \brief Recogniser for function \@cInt
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cInt
      inline
      bool is_cint_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@cInt";
        }
        return false;
      }

      /// \brief Application of function symbol \@cInt
      /// \param arg0 A data expression
      /// \return Application of \@cInt to a number of arguments
      inline
      application cint(const data_expression& arg0)
      {
        return application(cint(),arg0);
      }

      /// \brief Recogniser for application of \@cInt
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cint to a
      ///     number of arguments
      inline
      bool is_cint_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cint_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@cNeg
      /// \return Function symbol cneg
      inline
      function_symbol cneg()
      {
        static function_symbol cneg = data::detail::initialise_static_expression(cneg, function_symbol("@cNeg", function_sort(sort_pos::pos(), int_())));
        return cneg;
      }

      /// \brief Recogniser for function \@cNeg
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cNeg
      inline
      bool is_cneg_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@cNeg";
        }
        return false;
      }

      /// \brief Application of function symbol \@cNeg
      /// \param arg0 A data expression
      /// \return Application of \@cNeg to a number of arguments
      inline
      application cneg(const data_expression& arg0)
      {
        return application(cneg(),arg0);
      }

      /// \brief Recogniser for application of \@cNeg
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cneg to a
      ///     number of arguments
      inline
      bool is_cneg_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cneg_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for int_
      /// \return All system defined constructors for int_
      inline
      function_symbol_vector int__generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(cint());
        result.push_back(cneg());

        return result;
      }
      /// \brief Constructor for function symbol Nat2Int
      /// \return Function symbol nat2int
      inline
      function_symbol nat2int()
      {
        static function_symbol nat2int = data::detail::initialise_static_expression(nat2int, function_symbol("Nat2Int", function_sort(sort_nat::nat(), int_())));
        return nat2int;
      }

      /// \brief Recogniser for function Nat2Int
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Nat2Int
      inline
      bool is_nat2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Nat2Int";
        }
        return false;
      }

      /// \brief Application of function symbol Nat2Int
      /// \param arg0 A data expression
      /// \return Application of Nat2Int to a number of arguments
      inline
      application nat2int(const data_expression& arg0)
      {
        return application(nat2int(),arg0);
      }

      /// \brief Recogniser for application of Nat2Int
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2int to a
      ///     number of arguments
      inline
      bool is_nat2int_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_nat2int_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Int2Nat
      /// \return Function symbol int2nat
      inline
      function_symbol int2nat()
      {
        static function_symbol int2nat = data::detail::initialise_static_expression(int2nat, function_symbol("Int2Nat", function_sort(int_(), sort_nat::nat())));
        return int2nat;
      }

      /// \brief Recogniser for function Int2Nat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Int2Nat
      inline
      bool is_int2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Int2Nat";
        }
        return false;
      }

      /// \brief Application of function symbol Int2Nat
      /// \param arg0 A data expression
      /// \return Application of Int2Nat to a number of arguments
      inline
      application int2nat(const data_expression& arg0)
      {
        return application(int2nat(),arg0);
      }

      /// \brief Recogniser for application of Int2Nat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol int2nat to a
      ///     number of arguments
      inline
      bool is_int2nat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_int2nat_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Pos2Int
      /// \return Function symbol pos2int
      inline
      function_symbol pos2int()
      {
        static function_symbol pos2int = data::detail::initialise_static_expression(pos2int, function_symbol("Pos2Int", function_sort(sort_pos::pos(), int_())));
        return pos2int;
      }

      /// \brief Recogniser for function Pos2Int
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Pos2Int
      inline
      bool is_pos2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Pos2Int";
        }
        return false;
      }

      /// \brief Application of function symbol Pos2Int
      /// \param arg0 A data expression
      /// \return Application of Pos2Int to a number of arguments
      inline
      application pos2int(const data_expression& arg0)
      {
        return application(pos2int(),arg0);
      }

      /// \brief Recogniser for application of Pos2Int
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos2int to a
      ///     number of arguments
      inline
      bool is_pos2int_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pos2int_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Int2Pos
      /// \return Function symbol int2pos
      inline
      function_symbol int2pos()
      {
        static function_symbol int2pos = data::detail::initialise_static_expression(int2pos, function_symbol("Int2Pos", function_sort(int_(), sort_pos::pos())));
        return int2pos;
      }

      /// \brief Recogniser for function Int2Pos
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Int2Pos
      inline
      bool is_int2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Int2Pos";
        }
        return false;
      }

      /// \brief Application of function symbol Int2Pos
      /// \param arg0 A data expression
      /// \return Application of Int2Pos to a number of arguments
      inline
      application int2pos(const data_expression& arg0)
      {
        return application(int2pos(),arg0);
      }

      /// \brief Recogniser for application of Int2Pos
      /// \param e A data expression
      /// \return true iff e is an application of function symbol int2pos to a
      ///     number of arguments
      inline
      bool is_int2pos_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_int2pos_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol max
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol maximum
      inline
      function_symbol maximum(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == sort_pos::pos() && s1 == int_())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == int_() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == sort_nat::nat() && s1 == int_())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == int_() && s1 == int_())
        {
          target_sort = int_();
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

        function_symbol maximum("max", function_sort(s0, s1, target_sort));
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
          return static_cast< function_symbol >(e).name() == "max";
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
      bool is_maximum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_maximum_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol min
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol minimum
      inline
      function_symbol minimum(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == int_())
        {
          target_sort = int_();
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

        function_symbol minimum("min", function_sort(s0, s1, target_sort));
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
          return static_cast< function_symbol >(e).name() == "min";
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
      bool is_minimum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minimum_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol abs
      /// \param s0 A sort expression
      ///\return Function symbol abs
      inline
      function_symbol abs(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == int_())
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

        function_symbol abs("abs", function_sort(s0, target_sort));
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
          return static_cast< function_symbol >(e).name() == "abs";
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
      bool is_abs_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_abs_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol -
      /// \param s0 A sort expression
      ///\return Function symbol negate
      inline
      function_symbol negate(const sort_expression& s0)
      {
        sort_expression target_sort(int_());

        function_symbol negate("-", function_sort(s0, target_sort));
        return negate;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_negate_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "-";
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
      bool is_negate_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_negate_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol succ
      /// \param s0 A sort expression
      ///\return Function symbol succ
      inline
      function_symbol succ(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == int_())
        {
          target_sort = int_();
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

        function_symbol succ("succ", function_sort(s0, target_sort));
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
          return static_cast< function_symbol >(e).name() == "succ";
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
      bool is_succ_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_succ_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol pred
      /// \param s0 A sort expression
      ///\return Function symbol pred
      inline
      function_symbol pred(const sort_expression& s0)
      {
        sort_expression target_sort;
        if (s0 == sort_nat::nat())
        {
          target_sort = int_();
        }
        else if (s0 == int_())
        {
          target_sort = int_();
        }
        else if (s0 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        function_symbol pred("pred", function_sort(s0, target_sort));
        return pred;
      }

      /// \brief Recogniser for function pred
      /// \param e A data expression
      /// \return true iff e is the function symbol matching pred
      inline
      bool is_pred_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "pred";
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
      bool is_pred_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pred_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol \@dub
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol dub
      inline
      function_symbol dub(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == sort_bool_::bool_() && s1 == int_())
        {
          target_sort = int_();
        }
        else if (s0 == sort_bool_::bool_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        function_symbol dub("@dub", function_sort(s0, s1, target_sort));
        return dub;
      }

      /// \brief Recogniser for function \@dub
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@dub
      inline
      bool is_dub_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@dub";
        }
        return false;
      }

      /// \brief Application of function symbol \@dub
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@dub to a number of arguments
      inline
      application dub(const data_expression& arg0, const data_expression& arg1)
      {
        return application(dub(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of \@dub
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dub to a
      ///     number of arguments
      inline
      bool is_dub_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_dub_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol +
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol plus
      inline
      function_symbol plus(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == int_())
        {
          target_sort = int_();
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

        function_symbol plus("+", function_sort(s0, s1, target_sort));
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
          return static_cast< function_symbol >(e).name() == "+";
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
      bool is_plus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_plus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol -
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol minus
      inline
      function_symbol minus(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(int_());

        function_symbol minus("-", function_sort(s0, s1, target_sort));
        return minus;
      }

      /// \brief Recogniser for function -
      /// \param e A data expression
      /// \return true iff e is the function symbol matching -
      inline
      bool is_minus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "-";
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
      bool is_minus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol *
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol times
      inline
      function_symbol times(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == int_())
        {
          target_sort = int_();
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

        function_symbol times("*", function_sort(s0, s1, target_sort));
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
          return static_cast< function_symbol >(e).name() == "*";
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
      bool is_times_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_times_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol div
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol div
      inline
      function_symbol div(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == sort_pos::pos())
        {
          target_sort = int_();
        }
        else if (s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        function_symbol div("div", function_sort(s0, s1, target_sort));
        return div;
      }

      /// \brief Recogniser for function div
      /// \param e A data expression
      /// \return true iff e is the function symbol matching div
      inline
      bool is_div_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "div";
        }
        return false;
      }

      /// \brief Application of function symbol div
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of div to a number of arguments
      inline
      application div(const data_expression& arg0, const data_expression& arg1)
      {
        return application(div(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of div
      /// \param e A data expression
      /// \return true iff e is an application of function symbol div to a
      ///     number of arguments
      inline
      bool is_div_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_div_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol mod
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol mod
      inline
      function_symbol mod(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(sort_nat::nat());

        function_symbol mod("mod", function_sort(s0, s1, target_sort));
        return mod;
      }

      /// \brief Recogniser for function mod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching mod
      inline
      bool is_mod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "mod";
        }
        return false;
      }

      /// \brief Application of function symbol mod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of mod to a number of arguments
      inline
      application mod(const data_expression& arg0, const data_expression& arg1)
      {
        return application(mod(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of mod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol mod to a
      ///     number of arguments
      inline
      bool is_mod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_mod_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      ///\brief Constructor for function symbol exp
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol exp
      inline
      function_symbol exp(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == sort_nat::nat())
        {
          target_sort = int_();
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

        function_symbol exp("exp", function_sort(s0, s1, target_sort));
        return exp;
      }

      /// \brief Recogniser for function exp
      /// \param e A data expression
      /// \return true iff e is the function symbol matching exp
      inline
      bool is_exp_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "exp";
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
      bool is_exp_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_exp_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for int_
      /// \return All system defined mappings for int_
      inline
      function_symbol_vector int__generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(nat2int());
        result.push_back(int2nat());
        result.push_back(pos2int());
        result.push_back(int2pos());
        result.push_back(maximum(sort_pos::pos(), int_()));
        result.push_back(maximum(int_(), sort_pos::pos()));
        result.push_back(maximum(sort_nat::nat(), int_()));
        result.push_back(maximum(int_(), sort_nat::nat()));
        result.push_back(maximum(int_(), int_()));
        result.push_back(minimum(int_(), int_()));
        result.push_back(abs(int_()));
        result.push_back(negate(sort_pos::pos()));
        result.push_back(negate(sort_nat::nat()));
        result.push_back(negate(int_()));
        result.push_back(succ(int_()));
        result.push_back(pred(sort_nat::nat()));
        result.push_back(pred(int_()));
        result.push_back(dub(sort_bool_::bool_(), int_()));
        result.push_back(plus(int_(), int_()));
        result.push_back(minus(sort_pos::pos(), sort_pos::pos()));
        result.push_back(minus(sort_nat::nat(), sort_nat::nat()));
        result.push_back(minus(int_(), int_()));
        result.push_back(times(int_(), int_()));
        result.push_back(div(int_(), sort_pos::pos()));
        result.push_back(mod(int_(), sort_pos::pos()));
        result.push_back(exp(int_(), sort_nat::nat()));
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
        if (is_maximum_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_minimum_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_plus_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_minus_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_times_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        if (is_div_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_mod_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_exp_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg2 from an application
      /// \param e A data expression
      /// \pre arg2 is defined for e
      /// \return The argument of e that corresponds to arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        if (is_div_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_mod_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_exp_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        number from an application
      /// \param e A data expression
      /// \pre number is defined for e
      /// \return The argument of e that corresponds to number
      inline
      data_expression number(const data_expression& e)
      {
        if (is_abs_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_succ_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_pred_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        if (is_cint_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_cneg_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_nat2int_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_int2nat_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_pos2int_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_int2pos_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_negate_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_dub_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        bit from an application
      /// \param e A data expression
      /// \pre bit is defined for e
      /// \return The argument of e that corresponds to bit
      inline
      data_expression bit(const data_expression& e)
      {
        if (is_dub_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        if (is_maximum_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_minimum_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_plus_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_minus_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_times_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for int_
      /// \return All system defined equations for sort int_
      inline
      data_equation_vector int__generate_equations_code()
      {
        variable vb("b",sort_bool_::bool_());
        variable vn("n",sort_nat::nat());
        variable vm("m",sort_nat::nat());
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vx("x",int_());
        variable vy("y",int_());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vm, vn), equal_to(cint(vm), cint(vn)), equal_to(vm, vn)));
        result.push_back(data_equation(make_vector(vn, vp), equal_to(cint(vn), cneg(vp)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vn, vp), equal_to(cneg(vp), cint(vn)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp, vq), equal_to(cneg(vp), cneg(vq)), equal_to(vp, vq)));
        result.push_back(data_equation(make_vector(vm, vn), less(cint(vm), cint(vn)), less(vm, vn)));
        result.push_back(data_equation(make_vector(vn, vp), less(cint(vn), cneg(vp)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vn, vp), less(cneg(vp), cint(vn)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vp, vq), less(cneg(vp), cneg(vq)), less(vq, vp)));
        result.push_back(data_equation(make_vector(vm, vn), less_equal(cint(vm), cint(vn)), less_equal(vm, vn)));
        result.push_back(data_equation(make_vector(vn, vp), less_equal(cint(vn), cneg(vp)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vn, vp), less_equal(cneg(vp), cint(vn)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(cneg(vp), cneg(vq)), less_equal(vq, vp)));
        result.push_back(data_equation(variable_list(), nat2int(), cint()));
        result.push_back(data_equation(make_vector(vn), int2nat(cint(vn)), vn));
        result.push_back(data_equation(make_vector(vp), pos2int(vp), cint(sort_nat::cnat(vp))));
        result.push_back(data_equation(make_vector(vn), int2pos(cint(vn)), sort_nat::nat2pos(vn)));
        result.push_back(data_equation(make_vector(vn, vp), maximum(vp, cint(vn)), maximum(vp, vn)));
        result.push_back(data_equation(make_vector(vp, vq), maximum(vp, cneg(vq)), vp));
        result.push_back(data_equation(make_vector(vn, vp), maximum(cint(vn), vp), maximum(vn, vp)));
        result.push_back(data_equation(make_vector(vp, vq), maximum(cneg(vq), vp), vp));
        result.push_back(data_equation(make_vector(vm, vn), maximum(vm, cint(vn)), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(make_vector(vn, vp), maximum(vn, cneg(vp)), vn));
        result.push_back(data_equation(make_vector(vm, vn), maximum(cint(vm), vn), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(make_vector(vn, vp), maximum(cneg(vp), vn), vn));
        result.push_back(data_equation(make_vector(vx, vy), maximum(vx, vy), if_(less_equal(vx, vy), vy, vx)));
        result.push_back(data_equation(make_vector(vx, vy), minimum(vx, vy), if_(less_equal(vx, vy), vx, vy)));
        result.push_back(data_equation(make_vector(vn), abs(cint(vn)), vn));
        result.push_back(data_equation(make_vector(vp), abs(cneg(vp)), sort_nat::cnat(vp)));
        result.push_back(data_equation(make_vector(vp), negate(vp), cneg(vp)));
        result.push_back(data_equation(variable_list(), negate(sort_nat::c0()), cint(sort_nat::c0())));
        result.push_back(data_equation(make_vector(vp), negate(sort_nat::cnat(vp)), cneg(vp)));
        result.push_back(data_equation(make_vector(vn), negate(cint(vn)), negate(vn)));
        result.push_back(data_equation(make_vector(vp), negate(cneg(vp)), cint(sort_nat::cnat(vp))));
        result.push_back(data_equation(make_vector(vn), succ(cint(vn)), cint(sort_nat::cnat(succ(vn)))));
        result.push_back(data_equation(make_vector(vp), succ(cneg(vp)), negate(pred(vp))));
        result.push_back(data_equation(variable_list(), pred(sort_nat::c0()), cneg(sort_pos::c1())));
        result.push_back(data_equation(make_vector(vp), pred(sort_nat::cnat(vp)), cint(pred(vp))));
        result.push_back(data_equation(make_vector(vn), pred(cint(vn)), pred(vn)));
        result.push_back(data_equation(make_vector(vp), pred(cneg(vp)), cneg(succ(vp))));
        result.push_back(data_equation(make_vector(vb, vn), dub(vb, cint(vn)), cint(dub(vb, vn))));
        result.push_back(data_equation(make_vector(vp), dub(sort_bool_::false_(), cneg(vp)), cneg(sort_pos::cdub(sort_bool_::false_(), vp))));
        result.push_back(data_equation(make_vector(vp), dub(sort_bool_::true_(), cneg(vp)), negate(dub(sort_bool_::true_(), pred(vp)))));
        result.push_back(data_equation(make_vector(vm, vn), plus(cint(vm), cint(vn)), cint(plus(vm, vn))));
        result.push_back(data_equation(make_vector(vn, vp), plus(cint(vn), cneg(vp)), minus(vn, sort_nat::cnat(vp))));
        result.push_back(data_equation(make_vector(vn, vp), plus(cneg(vp), cint(vn)), minus(vn, sort_nat::cnat(vp))));
        result.push_back(data_equation(make_vector(vp, vq), plus(cneg(vp), cneg(vq)), cneg(sort_pos::add_with_carry(sort_bool_::false_(), vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vq, vp), minus(vp, vq), cint(sort_nat::gtesubtb(sort_bool_::false_(), vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), less(vp, vq), minus(vp, vq), negate(sort_nat::gtesubtb(sort_bool_::false_(), vq, vp))));
        result.push_back(data_equation(make_vector(vm, vn), less_equal(vn, vm), minus(vm, vn), cint(sort_nat::gtesubt(vm, vn))));
        result.push_back(data_equation(make_vector(vm, vn), less(vm, vn), minus(vm, vn), negate(sort_nat::gtesubt(vn, vm))));
        result.push_back(data_equation(make_vector(vx, vy), minus(vx, vy), plus(vx, negate(vy))));
        result.push_back(data_equation(make_vector(vm, vn), times(cint(vm), cint(vn)), cint(times(vm, vn))));
        result.push_back(data_equation(make_vector(vn, vp), times(cint(vn), cneg(vp)), negate(times(sort_nat::cnat(vp), vn))));
        result.push_back(data_equation(make_vector(vn, vp), times(cneg(vp), cint(vn)), negate(times(sort_nat::cnat(vp), vn))));
        result.push_back(data_equation(make_vector(vp, vq), times(cneg(vp), cneg(vq)), cint(sort_nat::cnat(times(vp, vq)))));
        result.push_back(data_equation(make_vector(vn, vp), div(cint(vn), vp), cint(div(vn, vp))));
        result.push_back(data_equation(make_vector(vp, vq), div(cneg(vp), vq), cneg(succ(div(pred(vp), vq)))));
        result.push_back(data_equation(make_vector(vn, vp), mod(cint(vn), vp), mod(vn, vp)));
        result.push_back(data_equation(make_vector(vp, vq), mod(cneg(vp), vq), int2nat(minus(vq, succ(mod(pred(vp), vq))))));
        result.push_back(data_equation(make_vector(vm, vn), exp(cint(vm), vn), cint(exp(vm, vn))));
        result.push_back(data_equation(make_vector(vn, vp), sort_nat::even(vn), exp(cneg(vp), vn), cint(sort_nat::cnat(exp(vp, vn)))));
        result.push_back(data_equation(make_vector(vn, vp), sort_bool_::not_(sort_nat::even(vn)), exp(cneg(vp), vn), cneg(exp(vp, vn))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for int_
      /// \param specification a specification
      inline
      void add_int__to_specification(data_specification& specification)
      {
         if (specification.constructors(sort_nat::nat()).empty())
         {
           sort_nat::add_nat_to_specification(specification);
         }
         if (specification.constructors(sort_bool_::bool_()).empty())
         {
           sort_bool_::add_bool__to_specification(specification);
         }
         if (specification.constructors(sort_pos::pos()).empty())
         {
           sort_pos::add_pos_to_specification(specification);
         }
         specification.add_system_defined_sort(int_());
         specification.add_system_defined_constructors(int__generate_constructors_code());
         specification.add_system_defined_mappings(int__generate_functions_code());
         specification.add_system_defined_equations(int__generate_equations_code());
      }
    } // namespace sort_int_

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INT__H
