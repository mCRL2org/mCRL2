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

#ifndef MCRL2_DATA_INT_H
#define MCRL2_DATA_INT_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort int_
    namespace sort_int {

      inline
      core::identifier_string const& int_name()
      {
        static core::identifier_string int_name = core::identifier_string("Int");
        return int_name;
      }

      /// \brief Constructor for sort expression Int
      /// \return Sort expression Int
      inline
      basic_sort const& int_()
      {
        static basic_sort int_ = basic_sort(int_name());
        return int_;
      }

      /// \brief Recogniser for sort expression Int
      /// \param e A sort expression
      /// \return true iff e == int_()
      inline
      bool is_int(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == int_();
        }
        return false;
      }


      /// \brief Generate identifier \@cInt
      /// \return Identifier \@cInt
      inline
      core::identifier_string const& cint_name()
      {
        static core::identifier_string cint_name = core::identifier_string("@cInt");
        return cint_name;
      }

      /// \brief Constructor for function symbol \@cInt
      
      /// \return Function symbol cint
      inline
      function_symbol const& cint()
      {
        static function_symbol cint(cint_name(), make_function_sort(sort_nat::nat(), int_()));
        return cint;
      }

      /// \brief Recogniser for function \@cInt
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cInt
      inline
      bool is_cint_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == cint();
        }
        return false;
      }

      /// \brief Application of function symbol \@cInt
      
      /// \param arg0 A data expression
      /// \return Application of \@cInt to a number of arguments
      inline
      application cint(const data_expression& arg0)
      {
        return sort_int::cint()(arg0);
      }

      /// \brief Recogniser for application of \@cInt
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cint to a
      ///     number of arguments
      inline
      bool is_cint_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cint_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@cNeg
      /// \return Identifier \@cNeg
      inline
      core::identifier_string const& cneg_name()
      {
        static core::identifier_string cneg_name = core::identifier_string("@cNeg");
        return cneg_name;
      }

      /// \brief Constructor for function symbol \@cNeg
      
      /// \return Function symbol cneg
      inline
      function_symbol const& cneg()
      {
        static function_symbol cneg(cneg_name(), make_function_sort(sort_pos::pos(), int_()));
        return cneg;
      }

      /// \brief Recogniser for function \@cNeg
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cNeg
      inline
      bool is_cneg_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == cneg();
        }
        return false;
      }

      /// \brief Application of function symbol \@cNeg
      
      /// \param arg0 A data expression
      /// \return Application of \@cNeg to a number of arguments
      inline
      application cneg(const data_expression& arg0)
      {
        return sort_int::cneg()(arg0);
      }

      /// \brief Recogniser for application of \@cNeg
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cneg to a
      ///     number of arguments
      inline
      bool is_cneg_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cneg_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined constructors for int_
      /// \return All system defined constructors for int_
      inline
      function_symbol_vector int_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(sort_int::cint());
        result.push_back(sort_int::cneg());

        return result;
      }

      /// \brief Generate identifier Nat2Int
      /// \return Identifier Nat2Int
      inline
      core::identifier_string const& nat2int_name()
      {
        static core::identifier_string nat2int_name = core::identifier_string("Nat2Int");
        return nat2int_name;
      }

      /// \brief Constructor for function symbol Nat2Int
      
      /// \return Function symbol nat2int
      inline
      function_symbol const& nat2int()
      {
        static function_symbol nat2int(nat2int_name(), make_function_sort(sort_nat::nat(), int_()));
        return nat2int;
      }

      /// \brief Recogniser for function Nat2Int
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Nat2Int
      inline
      bool is_nat2int_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == nat2int();
        }
        return false;
      }

      /// \brief Application of function symbol Nat2Int
      
      /// \param arg0 A data expression
      /// \return Application of Nat2Int to a number of arguments
      inline
      application nat2int(const data_expression& arg0)
      {
        return sort_int::nat2int()(arg0);
      }

      /// \brief Recogniser for application of Nat2Int
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2int to a
      ///     number of arguments
      inline
      bool is_nat2int_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_nat2int_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Int2Nat
      /// \return Identifier Int2Nat
      inline
      core::identifier_string const& int2nat_name()
      {
        static core::identifier_string int2nat_name = core::identifier_string("Int2Nat");
        return int2nat_name;
      }

      /// \brief Constructor for function symbol Int2Nat
      
      /// \return Function symbol int2nat
      inline
      function_symbol const& int2nat()
      {
        static function_symbol int2nat(int2nat_name(), make_function_sort(int_(), sort_nat::nat()));
        return int2nat;
      }

      /// \brief Recogniser for function Int2Nat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Int2Nat
      inline
      bool is_int2nat_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == int2nat();
        }
        return false;
      }

      /// \brief Application of function symbol Int2Nat
      
      /// \param arg0 A data expression
      /// \return Application of Int2Nat to a number of arguments
      inline
      application int2nat(const data_expression& arg0)
      {
        return sort_int::int2nat()(arg0);
      }

      /// \brief Recogniser for application of Int2Nat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol int2nat to a
      ///     number of arguments
      inline
      bool is_int2nat_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_int2nat_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Pos2Int
      /// \return Identifier Pos2Int
      inline
      core::identifier_string const& pos2int_name()
      {
        static core::identifier_string pos2int_name = core::identifier_string("Pos2Int");
        return pos2int_name;
      }

      /// \brief Constructor for function symbol Pos2Int
      
      /// \return Function symbol pos2int
      inline
      function_symbol const& pos2int()
      {
        static function_symbol pos2int(pos2int_name(), make_function_sort(sort_pos::pos(), int_()));
        return pos2int;
      }

      /// \brief Recogniser for function Pos2Int
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Pos2Int
      inline
      bool is_pos2int_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == pos2int();
        }
        return false;
      }

      /// \brief Application of function symbol Pos2Int
      
      /// \param arg0 A data expression
      /// \return Application of Pos2Int to a number of arguments
      inline
      application pos2int(const data_expression& arg0)
      {
        return sort_int::pos2int()(arg0);
      }

      /// \brief Recogniser for application of Pos2Int
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos2int to a
      ///     number of arguments
      inline
      bool is_pos2int_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_pos2int_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Int2Pos
      /// \return Identifier Int2Pos
      inline
      core::identifier_string const& int2pos_name()
      {
        static core::identifier_string int2pos_name = core::identifier_string("Int2Pos");
        return int2pos_name;
      }

      /// \brief Constructor for function symbol Int2Pos
      
      /// \return Function symbol int2pos
      inline
      function_symbol const& int2pos()
      {
        static function_symbol int2pos(int2pos_name(), make_function_sort(int_(), sort_pos::pos()));
        return int2pos;
      }

      /// \brief Recogniser for function Int2Pos
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Int2Pos
      inline
      bool is_int2pos_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == int2pos();
        }
        return false;
      }

      /// \brief Application of function symbol Int2Pos
      
      /// \param arg0 A data expression
      /// \return Application of Int2Pos to a number of arguments
      inline
      application int2pos(const data_expression& arg0)
      {
        return sort_int::int2pos()(arg0);
      }

      /// \brief Recogniser for application of Int2Pos
      /// \param e A data expression
      /// \return true iff e is an application of function symbol int2pos to a
      ///     number of arguments
      inline
      bool is_int2pos_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_int2pos_function_symbol(application(e).head());
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

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for maximum with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol maximum(maximum_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == maximum_name() && function_sort(f.sort()).domain().size() == 2 && (f == maximum(sort_pos::pos(), int_()) || f == maximum(int_(), sort_pos::pos()) || f == maximum(sort_nat::nat(), int_()) || f == maximum(int_(), sort_nat::nat()) || f == maximum(int_(), int_()) || f == maximum(sort_pos::pos(), sort_nat::nat()) || f == maximum(sort_nat::nat(), sort_pos::pos()) || f == maximum(sort_nat::nat(), sort_nat::nat()) || f == maximum(sort_pos::pos(), sort_pos::pos()));
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
        return sort_int::maximum(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_maximum_function_symbol(application(e).head());
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

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for minimum with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol minimum(minimum_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == minimum_name() && function_sort(f.sort()).domain().size() == 2 && (f == minimum(int_(), int_()) || f == minimum(sort_nat::nat(), sort_nat::nat()) || f == minimum(sort_pos::pos(), sort_pos::pos()));
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
        return sort_int::minimum(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_minimum_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier abs
      /// \return Identifier abs
      inline
      core::identifier_string const& abs_name()
      {
        static core::identifier_string abs_name = core::identifier_string("abs");
        return abs_name;
      }

      /// \brief Constructor for function symbol abs
      
      /// \return Function symbol abs
      inline
      function_symbol const& abs()
      {
        static function_symbol abs(abs_name(), make_function_sort(int_(), sort_nat::nat()));
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
          return function_symbol(e) == abs();
        }
        return false;
      }

      /// \brief Application of function symbol abs
      
      /// \param arg0 A data expression
      /// \return Application of abs to a number of arguments
      inline
      application abs(const data_expression& arg0)
      {
        return sort_int::abs()(arg0);
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
          return is_abs_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& negate_name()
      {
        static core::identifier_string negate_name = core::identifier_string("-");
        return negate_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
      inline
      function_symbol negate(const sort_expression& s0)
      {
        sort_expression target_sort(int_());
        function_symbol negate(negate_name(), make_function_sort(s0, target_sort));
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
          return f.name() == negate_name() && function_sort(f.sort()).domain().size() == 1 && (f == negate(sort_pos::pos()) || f == negate(sort_nat::nat()) || f == negate(int_()));
        }
        return false;
      }

      /// \brief Application of function symbol -
      
      /// \param arg0 A data expression
      /// \return Application of - to a number of arguments
      inline
      application negate(const data_expression& arg0)
      {
        return sort_int::negate(arg0.sort())(arg0);
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
          return is_negate_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier succ
      /// \return Identifier succ
      inline
      core::identifier_string const& succ_name()
      {
        static core::identifier_string succ_name = core::identifier_string("succ");
        return succ_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for succ with domain sorts " + to_string(s0));
        }

        function_symbol succ(succ_name(), make_function_sort(s0, target_sort));
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
          return f.name() == succ_name() && function_sort(f.sort()).domain().size() == 1 && (f == succ(int_()) || f == succ(sort_nat::nat()) || f == succ(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol succ
      
      /// \param arg0 A data expression
      /// \return Application of succ to a number of arguments
      inline
      application succ(const data_expression& arg0)
      {
        return sort_int::succ(arg0.sort())(arg0);
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
          return is_succ_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier pred
      /// \return Identifier pred
      inline
      core::identifier_string const& pred_name()
      {
        static core::identifier_string pred_name = core::identifier_string("pred");
        return pred_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for pred with domain sorts " + to_string(s0));
        }

        function_symbol pred(pred_name(), make_function_sort(s0, target_sort));
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
          return f.name() == pred_name() && function_sort(f.sort()).domain().size() == 1 && (f == pred(sort_nat::nat()) || f == pred(int_()) || f == pred(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol pred
      
      /// \param arg0 A data expression
      /// \return Application of pred to a number of arguments
      inline
      application pred(const data_expression& arg0)
      {
        return sort_int::pred(arg0.sort())(arg0);
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
          return is_pred_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@dub
      /// \return Identifier \@dub
      inline
      core::identifier_string const& dub_name()
      {
        static core::identifier_string dub_name = core::identifier_string("@dub");
        return dub_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
      inline
      function_symbol dub(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == sort_bool::bool_() && s1 == int_())
        {
          target_sort = int_();
        }
        else if (s0 == sort_bool::bool_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          throw mcrl2::runtime_error("cannot compute target sort for dub with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol dub(dub_name(), make_function_sort(s0, s1, target_sort));
        return dub;
      }

      /// \brief Recogniser for function \@dub
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@dub
      inline
      bool is_dub_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == dub_name() && function_sort(f.sort()).domain().size() == 2 && (f == dub(sort_bool::bool_(), int_()) || f == dub(sort_bool::bool_(), sort_nat::nat()));
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
        return sort_int::dub(arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of \@dub
      /// \param e A data expression
      /// \return true iff e is an application of function symbol dub to a
      ///     number of arguments
      inline
      bool is_dub_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_dub_function_symbol(application(e).head());
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

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for plus with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol plus(plus_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == plus_name() && function_sort(f.sort()).domain().size() == 2 && (f == plus(int_(), int_()) || f == plus(sort_pos::pos(), sort_nat::nat()) || f == plus(sort_nat::nat(), sort_pos::pos()) || f == plus(sort_nat::nat(), sort_nat::nat()) || f == plus(sort_pos::pos(), sort_pos::pos()));
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
        return sort_int::plus(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_plus_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier -
      /// \return Identifier -
      inline
      core::identifier_string const& minus_name()
      {
        static core::identifier_string minus_name = core::identifier_string("-");
        return minus_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
      inline
      function_symbol minus(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(int_());
        function_symbol minus(minus_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == minus_name() && function_sort(f.sort()).domain().size() == 2 && (f == minus(sort_pos::pos(), sort_pos::pos()) || f == minus(sort_nat::nat(), sort_nat::nat()) || f == minus(int_(), int_()));
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
        return sort_int::minus(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_minus_function_symbol(application(e).head());
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

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for times with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol times(times_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == times_name() && function_sort(f.sort()).domain().size() == 2 && (f == times(int_(), int_()) || f == times(sort_nat::nat(), sort_nat::nat()) || f == times(sort_pos::pos(), sort_pos::pos()));
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
        return sort_int::times(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_times_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier div
      /// \return Identifier div
      inline
      core::identifier_string const& div_name()
      {
        static core::identifier_string div_name = core::identifier_string("div");
        return div_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
      inline
      function_symbol div(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort;
        if (s0 == int_() && s1 == sort_pos::pos())
        {
          target_sort = int_();
        }
        else if (s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          throw mcrl2::runtime_error("cannot compute target sort for div with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol div(div_name(), make_function_sort(s0, s1, target_sort));
        return div;
      }

      /// \brief Recogniser for function div
      /// \param e A data expression
      /// \return true iff e is the function symbol matching div
      inline
      bool is_div_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == div_name() && function_sort(f.sort()).domain().size() == 2 && (f == div(int_(), sort_pos::pos()) || f == div(sort_nat::nat(), sort_pos::pos()));
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
        return sort_int::div(arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of div
      /// \param e A data expression
      /// \return true iff e is an application of function symbol div to a
      ///     number of arguments
      inline
      bool is_div_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_div_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier mod
      /// \return Identifier mod
      inline
      core::identifier_string const& mod_name()
      {
        static core::identifier_string mod_name = core::identifier_string("mod");
        return mod_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
      inline
      function_symbol mod(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(sort_nat::nat());
        function_symbol mod(mod_name(), make_function_sort(s0, s1, target_sort));
        return mod;
      }

      /// \brief Recogniser for function mod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching mod
      inline
      bool is_mod_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          function_symbol f(e);
          return f.name() == mod_name() && function_sort(f.sort()).domain().size() == 2 && (f == mod(int_(), sort_pos::pos()) || f == mod(sort_nat::nat(), sort_pos::pos()));
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
        return sort_int::mod(arg0.sort(), arg1.sort())(arg0, arg1);
      }

      /// \brief Recogniser for application of mod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol mod to a
      ///     number of arguments
      inline
      bool is_mod_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_mod_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier exp
      /// \return Identifier exp
      inline
      core::identifier_string const& exp_name()
      {
        static core::identifier_string exp_name = core::identifier_string("exp");
        return exp_name;
      }

      // This function is not intended for public use and therefore not documented in Doxygen.
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
          throw mcrl2::runtime_error("cannot compute target sort for exp with domain sorts " + to_string(s0) + ", " + to_string(s1));
        }

        function_symbol exp(exp_name(), make_function_sort(s0, s1, target_sort));
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
          return f.name() == exp_name() && function_sort(f.sort()).domain().size() == 2 && (f == exp(int_(), sort_nat::nat()) || f == exp(sort_pos::pos(), sort_nat::nat()) || f == exp(sort_nat::nat(), sort_nat::nat()));
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
        return sort_int::exp(arg0.sort(), arg1.sort())(arg0, arg1);
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
          return is_exp_function_symbol(application(e).head());
        }
        return false;
      }
      /// \brief Give all system defined mappings for int_
      /// \return All system defined mappings for int_
      inline
      function_symbol_vector int_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(sort_int::nat2int());
        result.push_back(sort_int::int2nat());
        result.push_back(sort_int::pos2int());
        result.push_back(sort_int::int2pos());
        result.push_back(sort_int::maximum(sort_pos::pos(), int_()));
        result.push_back(sort_int::maximum(int_(), sort_pos::pos()));
        result.push_back(sort_int::maximum(sort_nat::nat(), int_()));
        result.push_back(sort_int::maximum(int_(), sort_nat::nat()));
        result.push_back(sort_int::maximum(int_(), int_()));
        result.push_back(sort_int::minimum(int_(), int_()));
        result.push_back(sort_int::abs());
        result.push_back(sort_int::negate(sort_pos::pos()));
        result.push_back(sort_int::negate(sort_nat::nat()));
        result.push_back(sort_int::negate(int_()));
        result.push_back(sort_int::succ(int_()));
        result.push_back(sort_int::pred(sort_nat::nat()));
        result.push_back(sort_int::pred(int_()));
        result.push_back(sort_int::dub(sort_bool::bool_(), int_()));
        result.push_back(sort_int::plus(int_(), int_()));
        result.push_back(sort_int::minus(sort_pos::pos(), sort_pos::pos()));
        result.push_back(sort_int::minus(sort_nat::nat(), sort_nat::nat()));
        result.push_back(sort_int::minus(int_(), int_()));
        result.push_back(sort_int::times(int_(), int_()));
        result.push_back(sort_int::div(int_(), sort_pos::pos()));
        result.push_back(sort_int::mod(int_(), sort_pos::pos()));
        result.push_back(sort_int::exp(int_(), sort_nat::nat()));
        return result;
      }
      ///\brief Function for projecting out argument
      ///        left from an application
      /// \param e A data expression
      /// \pre left is defined for e
      /// \return The argument of e that corresponds to left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_maximum_application(e) || is_minimum_application(e) || is_dub_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e));
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
        assert(is_maximum_application(e) || is_minimum_application(e) || is_dub_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e));
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
        assert(is_cint_application(e) || is_cneg_application(e) || is_nat2int_application(e) || is_int2nat_application(e) || is_pos2int_application(e) || is_int2pos_application(e) || is_abs_application(e) || is_negate_application(e) || is_succ_application(e) || is_pred_application(e));
        return atermpp::down_cast<const application >(e)[0];
      }

      /// \brief Give all system defined equations for int_
      /// \return All system defined equations for sort int_
      inline
      data_equation_vector int_generate_equations_code()
      {
        variable vb("b",sort_bool::bool_());
        variable vn("n",sort_nat::nat());
        variable vm("m",sort_nat::nat());
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vx("x",int_());
        variable vy("y",int_());

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vm, vn), equal_to(cint(vm), cint(vn)), equal_to(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), equal_to(cint(vn), cneg(vp)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), equal_to(cneg(vp), cint(vn)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), equal_to(cneg(vp), cneg(vq)), equal_to(vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), less(cint(vm), cint(vn)), less(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), less(cint(vn), cneg(vp)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), less(cneg(vp), cint(vn)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less(cneg(vp), cneg(vq)), less(vq, vp)));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), less_equal(cint(vm), cint(vn)), less_equal(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), less_equal(cint(vn), cneg(vp)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), less_equal(cneg(vp), cint(vn)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less_equal(cneg(vp), cneg(vq)), less_equal(vq, vp)));
        result.push_back(data_equation(atermpp::make_vector(vn), nat2int(vn), cint(vn)));
        result.push_back(data_equation(atermpp::make_vector(vn), int2nat(cint(vn)), vn));
        result.push_back(data_equation(atermpp::make_vector(vp), pos2int(vp), cint(sort_nat::cnat(vp))));
        result.push_back(data_equation(atermpp::make_vector(vn), int2pos(cint(vn)), sort_nat::nat2pos(vn)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), maximum(vp, cint(vn)), maximum(vp, vn)));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), maximum(vp, cneg(vq)), vp));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), maximum(cint(vn), vp), maximum(vn, vp)));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), maximum(cneg(vq), vp), vp));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), maximum(vm, cint(vn)), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), maximum(vn, cneg(vp)), vn));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), maximum(cint(vm), vn), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), maximum(cneg(vp), vn), vn));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), maximum(vx, vy), if_(less_equal(vx, vy), vy, vx)));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), minimum(vx, vy), if_(less_equal(vx, vy), vx, vy)));
        result.push_back(data_equation(atermpp::make_vector(vn), abs(cint(vn)), vn));
        result.push_back(data_equation(atermpp::make_vector(vp), abs(cneg(vp)), sort_nat::cnat(vp)));
        result.push_back(data_equation(atermpp::make_vector(vp), negate(vp), cneg(vp)));
        result.push_back(data_equation(variable_list(), negate(sort_nat::c0()), cint(sort_nat::c0())));
        result.push_back(data_equation(atermpp::make_vector(vp), negate(sort_nat::cnat(vp)), cneg(vp)));
        result.push_back(data_equation(atermpp::make_vector(vn), negate(cint(vn)), negate(vn)));
        result.push_back(data_equation(atermpp::make_vector(vp), negate(cneg(vp)), cint(sort_nat::cnat(vp))));
        result.push_back(data_equation(atermpp::make_vector(vn), succ(cint(vn)), cint(sort_nat::cnat(succ(vn)))));
        result.push_back(data_equation(atermpp::make_vector(vp), succ(cneg(vp)), negate(pred(vp))));
        result.push_back(data_equation(variable_list(), pred(sort_nat::c0()), cneg(sort_pos::c1())));
        result.push_back(data_equation(atermpp::make_vector(vp), pred(sort_nat::cnat(vp)), cint(pred(vp))));
        result.push_back(data_equation(atermpp::make_vector(vn), pred(cint(vn)), pred(vn)));
        result.push_back(data_equation(atermpp::make_vector(vp), pred(cneg(vp)), cneg(succ(vp))));
        result.push_back(data_equation(atermpp::make_vector(vb, vn), dub(vb, cint(vn)), cint(dub(vb, vn))));
        result.push_back(data_equation(atermpp::make_vector(vp), dub(sort_bool::false_(), cneg(vp)), cneg(sort_pos::cdub(sort_bool::false_(), vp))));
        result.push_back(data_equation(atermpp::make_vector(vp), dub(sort_bool::true_(), cneg(vp)), negate(dub(sort_bool::true_(), pred(vp)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), plus(cint(vm), cint(vn)), cint(plus(vm, vn))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), plus(cint(vn), cneg(vp)), minus(vn, sort_nat::cnat(vp))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), plus(cneg(vp), cint(vn)), minus(vn, sort_nat::cnat(vp))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), plus(cneg(vp), cneg(vq)), cneg(sort_pos::add_with_carry(sort_bool::false_(), vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less_equal(vq, vp), minus(vp, vq), cint(sort_nat::gte_subtract_with_borrow(sort_bool::false_(), vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less(vp, vq), minus(vp, vq), negate(sort_nat::gte_subtract_with_borrow(sort_bool::false_(), vq, vp))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), less_equal(vn, vm), minus(vm, vn), cint(sort_nat::monus(vm, vn))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), less(vm, vn), minus(vm, vn), negate(sort_nat::monus(vn, vm))));
        result.push_back(data_equation(atermpp::make_vector(vx, vy), minus(vx, vy), plus(vx, negate(vy))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), times(cint(vm), cint(vn)), cint(times(vm, vn))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), times(cint(vn), cneg(vp)), negate(times(sort_nat::cnat(vp), vn))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), times(cneg(vp), cint(vn)), negate(times(sort_nat::cnat(vp), vn))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), times(cneg(vp), cneg(vq)), cint(sort_nat::cnat(times(vp, vq)))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), div(cint(vn), vp), cint(div(vn, vp))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), div(cneg(vp), vq), cneg(succ(div(pred(vp), vq)))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), mod(cint(vn), vp), mod(vn, vp)));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), mod(cneg(vp), vq), int2nat(minus(vq, succ(mod(pred(vp), vq))))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), exp(cint(vm), vn), cint(exp(vm, vn))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), sort_nat::even(vn), exp(cneg(vp), vn), cint(sort_nat::cnat(exp(vp, vn)))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), sort_bool::not_(sort_nat::even(vn)), exp(cneg(vp), vn), cneg(exp(vp, vn))));
        return result;
      }

    } // namespace sort_int_

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_INT_H
