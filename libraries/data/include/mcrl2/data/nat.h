// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/nat.h
/// \brief The standard sort nat.
///
/// This file was generated from the data sort specification
/// mcrl2/data/build/nat.spec.

#ifndef MCRL2_DATA_NAT_H
#define MCRL2_DATA_NAT_H

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

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort nat
    namespace sort_nat {

      /// \brief Constructor for sort expression Nat
      /// \return Sort expression Nat
      inline
      basic_sort const& nat()
      {
        static basic_sort nat = data::detail::initialise_static_expression(nat, basic_sort("Nat"));
        return nat;
      }

      /// \brief Recogniser for sort expression Nat
      /// \param e A sort expression
      /// \return true iff e == nat()
      inline
      bool is_nat(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast< basic_sort >(e) == nat();
        }
        return false;
      }

      /// \brief Constructor for sort expression \@NatPair
      /// \return Sort expression \@NatPair
      inline
      basic_sort const& natpair()
      {
        static basic_sort natpair = data::detail::initialise_static_expression(natpair, basic_sort("@NatPair"));
        return natpair;
      }

      /// \brief Recogniser for sort expression \@NatPair
      /// \param e A sort expression
      /// \return true iff e == natpair()
      inline
      bool is_natpair(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast< basic_sort >(e) == natpair();
        }
        return false;
      }

      /// \brief Constructor for function symbol \@c0
      /// \return Function symbol c0
      inline
      function_symbol const& c0()
      {
        static function_symbol c0 = data::detail::initialise_static_expression(c0, function_symbol("@c0", nat()));
        return c0;
      }

      /// \brief Recogniser for function \@c0
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@c0
      inline
      bool is_c0_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@c0";
        }
        return false;
      }

      /// \brief Constructor for function symbol \@cNat
      /// \return Function symbol cnat
      inline
      function_symbol const& cnat()
      {
        static function_symbol cnat = data::detail::initialise_static_expression(cnat, function_symbol("@cNat", function_sort(sort_pos::pos(), nat())));
        return cnat;
      }

      /// \brief Recogniser for function \@cNat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cNat
      inline
      bool is_cnat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@cNat";
        }
        return false;
      }

      /// \brief Application of function symbol \@cNat
      /// \param arg0 A data expression
      /// \return Application of \@cNat to a number of arguments
      inline
      application cnat(const data_expression& arg0)
      {
        return application(cnat(),arg0);
      }

      /// \brief Recogniser for application of \@cNat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cnat to a
      ///     number of arguments
      inline
      bool is_cnat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cnat_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@cPair
      /// \return Function symbol cpair
      inline
      function_symbol const& cpair()
      {
        static function_symbol cpair = data::detail::initialise_static_expression(cpair, function_symbol("@cPair", function_sort(nat(), nat(), natpair())));
        return cpair;
      }

      /// \brief Recogniser for function \@cPair
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cPair
      inline
      bool is_cpair_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@cPair";
        }
        return false;
      }

      /// \brief Application of function symbol \@cPair
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@cPair to a number of arguments
      inline
      application cpair(const data_expression& arg0, const data_expression& arg1)
      {
        return application(cpair(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@cPair
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cpair to a
      ///     number of arguments
      inline
      bool is_cpair_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cpair_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined constructors for nat
      /// \return All system defined constructors for nat
      inline
      function_symbol_vector nat_generate_constructors_code()
      {
        function_symbol_vector result;
        result.push_back(c0());
        result.push_back(cnat());
        result.push_back(cpair());

        return result;
      }
      /// \brief Constructor for function symbol Pos2Nat
      /// \return Function symbol pos2nat
      inline
      function_symbol const& pos2nat()
      {
        static function_symbol pos2nat = data::detail::initialise_static_expression(pos2nat, function_symbol("Pos2Nat", function_sort(sort_pos::pos(), nat())));
        return pos2nat;
      }

      /// \brief Recogniser for function Pos2Nat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Pos2Nat
      inline
      bool is_pos2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Pos2Nat";
        }
        return false;
      }

      /// \brief Application of function symbol Pos2Nat
      /// \param arg0 A data expression
      /// \return Application of Pos2Nat to a number of arguments
      inline
      application pos2nat(const data_expression& arg0)
      {
        return application(pos2nat(),arg0);
      }

      /// \brief Recogniser for application of Pos2Nat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos2nat to a
      ///     number of arguments
      inline
      bool is_pos2nat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pos2nat_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol Nat2Pos
      /// \return Function symbol nat2pos
      inline
      function_symbol const& nat2pos()
      {
        static function_symbol nat2pos = data::detail::initialise_static_expression(nat2pos, function_symbol("Nat2Pos", function_sort(nat(), sort_pos::pos())));
        return nat2pos;
      }

      /// \brief Recogniser for function Nat2Pos
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Nat2Pos
      inline
      bool is_nat2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "Nat2Pos";
        }
        return false;
      }

      /// \brief Application of function symbol Nat2Pos
      /// \param arg0 A data expression
      /// \return Application of Nat2Pos to a number of arguments
      inline
      application nat2pos(const data_expression& arg0)
      {
        return application(nat2pos(),arg0);
      }

      /// \brief Recogniser for application of Nat2Pos
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2pos to a
      ///     number of arguments
      inline
      bool is_nat2pos_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_nat2pos_function_symbol(static_cast< application >(e).head());
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
        if (s0 == sort_pos::pos() && s1 == nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == nat() && s1 == nat())
        {
          target_sort = nat();
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
        if (s0 == nat() && s1 == nat())
        {
          target_sort = nat();
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
        if (s0 == nat())
        {
          target_sort = nat();
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

      ///\brief Constructor for function symbol succ
      /// \param s0 A sort expression
      ///\return Function symbol succ
      inline
      function_symbol succ(const sort_expression& s0)
      {
        sort_expression target_sort(sort_pos::pos());

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

      /// \brief Constructor for function symbol pred
      /// \return Function symbol pred
      inline
      function_symbol const& pred()
      {
        static function_symbol pred = data::detail::initialise_static_expression(pred, function_symbol("pred", function_sort(sort_pos::pos(), nat())));
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
        return application(pred(),arg0);
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

      /// \brief Constructor for function symbol \@dub
      /// \return Function symbol dub
      inline
      function_symbol const& dub()
      {
        static function_symbol dub = data::detail::initialise_static_expression(dub, function_symbol("@dub", function_sort(sort_bool_::bool_(), nat(), nat())));
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
        return application(dub(),arg0, arg1);
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
        if (s0 == sort_pos::pos() && s1 == nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == nat() && s1 == nat())
        {
          target_sort = nat();
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

      ///\brief Constructor for function symbol \@gtesubt
      /// \param s0 A sort expression
      /// \param s1 A sort expression
      ///\return Function symbol gtesubt
      inline
      function_symbol gtesubt(const sort_expression& s0, const sort_expression& s1)
      {
        sort_expression target_sort(nat());

        function_symbol gtesubt("@gtesubt", function_sort(s0, s1, target_sort));
        return gtesubt;
      }

      /// \brief Recogniser for function \@gtesubt
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@gtesubt
      inline
      bool is_gtesubt_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@gtesubt";
        }
        return false;
      }

      /// \brief Application of function symbol \@gtesubt
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@gtesubt to a number of arguments
      inline
      application gtesubt(const data_expression& arg0, const data_expression& arg1)
      {
        return application(gtesubt(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      /// \brief Recogniser for application of \@gtesubt
      /// \param e A data expression
      /// \return true iff e is an application of function symbol gtesubt to a
      ///     number of arguments
      inline
      bool is_gtesubt_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubt_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@gtesubtb
      /// \return Function symbol gtesubtb
      inline
      function_symbol const& gtesubtb()
      {
        static function_symbol gtesubtb = data::detail::initialise_static_expression(gtesubtb, function_symbol("@gtesubtb", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), nat())));
        return gtesubtb;
      }

      /// \brief Recogniser for function \@gtesubtb
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@gtesubtb
      inline
      bool is_gtesubtb_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@gtesubtb";
        }
        return false;
      }

      /// \brief Application of function symbol \@gtesubtb
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@gtesubtb to a number of arguments
      inline
      application gtesubtb(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(gtesubtb(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@gtesubtb
      /// \param e A data expression
      /// \return true iff e is an application of function symbol gtesubtb to a
      ///     number of arguments
      inline
      bool is_gtesubtb_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubtb_function_symbol(static_cast< application >(e).head());
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
        if (s0 == nat() && s1 == nat())
        {
          target_sort = nat();
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
        sort_expression target_sort(nat());

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
        sort_expression target_sort(nat());

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
        if (s0 == sort_pos::pos() && s1 == nat())
        {
          target_sort = sort_pos::pos();
        }
        else if (s0 == nat() && s1 == nat())
        {
          target_sort = nat();
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

      /// \brief Constructor for function symbol \@even
      /// \return Function symbol even
      inline
      function_symbol const& even()
      {
        static function_symbol even = data::detail::initialise_static_expression(even, function_symbol("@even", function_sort(nat(), sort_bool_::bool_())));
        return even;
      }

      /// \brief Recogniser for function \@even
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@even
      inline
      bool is_even_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@even";
        }
        return false;
      }

      /// \brief Application of function symbol \@even
      /// \param arg0 A data expression
      /// \return Application of \@even to a number of arguments
      inline
      application even(const data_expression& arg0)
      {
        return application(even(),arg0);
      }

      /// \brief Recogniser for application of \@even
      /// \param e A data expression
      /// \return true iff e is an application of function symbol even to a
      ///     number of arguments
      inline
      bool is_even_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_even_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@monus
      /// \return Function symbol monus
      inline
      function_symbol const& monus()
      {
        static function_symbol monus = data::detail::initialise_static_expression(monus, function_symbol("@monus", function_sort(nat(), nat(), nat())));
        return monus;
      }

      /// \brief Recogniser for function \@monus
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@monus
      inline
      bool is_monus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@monus";
        }
        return false;
      }

      /// \brief Application of function symbol \@monus
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@monus to a number of arguments
      inline
      application monus(const data_expression& arg0, const data_expression& arg1)
      {
        return application(monus(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@monus
      /// \param e A data expression
      /// \return true iff e is an application of function symbol monus to a
      ///     number of arguments
      inline
      bool is_monus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_monus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@swap_zero
      /// \return Function symbol swap_zero
      inline
      function_symbol const& swap_zero()
      {
        static function_symbol swap_zero = data::detail::initialise_static_expression(swap_zero, function_symbol("@swap_zero", function_sort(nat(), nat(), nat())));
        return swap_zero;
      }

      /// \brief Recogniser for function \@swap_zero
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero
      inline
      bool is_swap_zero_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@swap_zero";
        }
        return false;
      }

      /// \brief Application of function symbol \@swap_zero
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@swap_zero to a number of arguments
      inline
      application swap_zero(const data_expression& arg0, const data_expression& arg1)
      {
        return application(swap_zero(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@swap_zero
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero to a
      ///     number of arguments
      inline
      bool is_swap_zero_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_swap_zero_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@swap_zero_add
      /// \return Function symbol swap_zero_add
      inline
      function_symbol const& swap_zero_add()
      {
        static function_symbol swap_zero_add = data::detail::initialise_static_expression(swap_zero_add, function_symbol("@swap_zero_add", function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_add;
      }

      /// \brief Recogniser for function \@swap_zero_add
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_add
      inline
      bool is_swap_zero_add_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@swap_zero_add";
        }
        return false;
      }

      /// \brief Application of function symbol \@swap_zero_add
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@swap_zero_add to a number of arguments
      inline
      application swap_zero_add(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(swap_zero_add(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_add
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_add to a
      ///     number of arguments
      inline
      bool is_swap_zero_add_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_swap_zero_add_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@swap_zero_min
      /// \return Function symbol swap_zero_min
      inline
      function_symbol const& swap_zero_min()
      {
        static function_symbol swap_zero_min = data::detail::initialise_static_expression(swap_zero_min, function_symbol("@swap_zero_min", function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_min;
      }

      /// \brief Recogniser for function \@swap_zero_min
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_min
      inline
      bool is_swap_zero_min_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@swap_zero_min";
        }
        return false;
      }

      /// \brief Application of function symbol \@swap_zero_min
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@swap_zero_min to a number of arguments
      inline
      application swap_zero_min(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(swap_zero_min(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_min
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_min to a
      ///     number of arguments
      inline
      bool is_swap_zero_min_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_swap_zero_min_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@swap_zero_monus
      /// \return Function symbol swap_zero_monus
      inline
      function_symbol const& swap_zero_monus()
      {
        static function_symbol swap_zero_monus = data::detail::initialise_static_expression(swap_zero_monus, function_symbol("@swap_zero_monus", function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_monus;
      }

      /// \brief Recogniser for function \@swap_zero_monus
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_monus
      inline
      bool is_swap_zero_monus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@swap_zero_monus";
        }
        return false;
      }

      /// \brief Application of function symbol \@swap_zero_monus
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \param arg3 A data expression
      /// \return Application of \@swap_zero_monus to a number of arguments
      inline
      application swap_zero_monus(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        return application(swap_zero_monus(),arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_monus
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_monus to a
      ///     number of arguments
      inline
      bool is_swap_zero_monus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_swap_zero_monus_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@swap_zero_lte
      /// \return Function symbol swap_zero_lte
      inline
      function_symbol const& swap_zero_lte()
      {
        static function_symbol swap_zero_lte = data::detail::initialise_static_expression(swap_zero_lte, function_symbol("@swap_zero_lte", function_sort(nat(), nat(), nat(), sort_bool_::bool_())));
        return swap_zero_lte;
      }

      /// \brief Recogniser for function \@swap_zero_lte
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_lte
      inline
      bool is_swap_zero_lte_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@swap_zero_lte";
        }
        return false;
      }

      /// \brief Application of function symbol \@swap_zero_lte
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@swap_zero_lte to a number of arguments
      inline
      application swap_zero_lte(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(swap_zero_lte(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@swap_zero_lte
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_lte to a
      ///     number of arguments
      inline
      bool is_swap_zero_lte_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_swap_zero_lte_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@first
      /// \return Function symbol first
      inline
      function_symbol const& first()
      {
        static function_symbol first = data::detail::initialise_static_expression(first, function_symbol("@first", function_sort(natpair(), nat())));
        return first;
      }

      /// \brief Recogniser for function \@first
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@first
      inline
      bool is_first_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@first";
        }
        return false;
      }

      /// \brief Application of function symbol \@first
      /// \param arg0 A data expression
      /// \return Application of \@first to a number of arguments
      inline
      application first(const data_expression& arg0)
      {
        return application(first(),arg0);
      }

      /// \brief Recogniser for application of \@first
      /// \param e A data expression
      /// \return true iff e is an application of function symbol first to a
      ///     number of arguments
      inline
      bool is_first_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_first_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@last
      /// \return Function symbol last
      inline
      function_symbol const& last()
      {
        static function_symbol last = data::detail::initialise_static_expression(last, function_symbol("@last", function_sort(natpair(), nat())));
        return last;
      }

      /// \brief Recogniser for function \@last
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@last
      inline
      bool is_last_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@last";
        }
        return false;
      }

      /// \brief Application of function symbol \@last
      /// \param arg0 A data expression
      /// \return Application of \@last to a number of arguments
      inline
      application last(const data_expression& arg0)
      {
        return application(last(),arg0);
      }

      /// \brief Recogniser for application of \@last
      /// \param e A data expression
      /// \return true iff e is an application of function symbol last to a
      ///     number of arguments
      inline
      bool is_last_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_last_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@divmod
      /// \return Function symbol divmod
      inline
      function_symbol const& divmod()
      {
        static function_symbol divmod = data::detail::initialise_static_expression(divmod, function_symbol("@divmod", function_sort(sort_pos::pos(), sort_pos::pos(), natpair())));
        return divmod;
      }

      /// \brief Recogniser for function \@divmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@divmod
      inline
      bool is_divmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@divmod";
        }
        return false;
      }

      /// \brief Application of function symbol \@divmod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \return Application of \@divmod to a number of arguments
      inline
      application divmod(const data_expression& arg0, const data_expression& arg1)
      {
        return application(divmod(),arg0, arg1);
      }

      /// \brief Recogniser for application of \@divmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol divmod to a
      ///     number of arguments
      inline
      bool is_divmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_divmod_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@gdivmod
      /// \return Function symbol gdivmod
      inline
      function_symbol const& gdivmod()
      {
        static function_symbol gdivmod = data::detail::initialise_static_expression(gdivmod, function_symbol("@gdivmod", function_sort(natpair(), sort_bool_::bool_(), sort_pos::pos(), natpair())));
        return gdivmod;
      }

      /// \brief Recogniser for function \@gdivmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@gdivmod
      inline
      bool is_gdivmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@gdivmod";
        }
        return false;
      }

      /// \brief Application of function symbol \@gdivmod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@gdivmod to a number of arguments
      inline
      application gdivmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(gdivmod(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@gdivmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol gdivmod to a
      ///     number of arguments
      inline
      bool is_gdivmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gdivmod_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Constructor for function symbol \@ggdivmod
      /// \return Function symbol ggdivmod
      inline
      function_symbol const& ggdivmod()
      {
        static function_symbol ggdivmod = data::detail::initialise_static_expression(ggdivmod, function_symbol("@ggdivmod", function_sort(nat(), nat(), sort_pos::pos(), natpair())));
        return ggdivmod;
      }

      /// \brief Recogniser for function \@ggdivmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@ggdivmod
      inline
      bool is_ggdivmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast< function_symbol >(e).name() == "@ggdivmod";
        }
        return false;
      }

      /// \brief Application of function symbol \@ggdivmod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@ggdivmod to a number of arguments
      inline
      application ggdivmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return application(ggdivmod(),arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@ggdivmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol ggdivmod to a
      ///     number of arguments
      inline
      bool is_ggdivmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_ggdivmod_function_symbol(static_cast< application >(e).head());
        }
        return false;
      }

      /// \brief Give all system defined mappings for nat
      /// \return All system defined mappings for nat
      inline
      function_symbol_vector nat_generate_functions_code()
      {
        function_symbol_vector result;
        result.push_back(pos2nat());
        result.push_back(nat2pos());
        result.push_back(maximum(sort_pos::pos(), nat()));
        result.push_back(maximum(nat(), sort_pos::pos()));
        result.push_back(maximum(nat(), nat()));
        result.push_back(minimum(nat(), nat()));
        result.push_back(abs(nat()));
        result.push_back(succ(nat()));
        result.push_back(pred());
        result.push_back(dub());
        result.push_back(plus(sort_pos::pos(), nat()));
        result.push_back(plus(nat(), sort_pos::pos()));
        result.push_back(plus(nat(), nat()));
        result.push_back(gtesubt(sort_pos::pos(), sort_pos::pos()));
        result.push_back(gtesubt(nat(), nat()));
        result.push_back(gtesubtb());
        result.push_back(times(nat(), nat()));
        result.push_back(div(sort_pos::pos(), sort_pos::pos()));
        result.push_back(div(nat(), sort_pos::pos()));
        result.push_back(mod(sort_pos::pos(), sort_pos::pos()));
        result.push_back(mod(nat(), sort_pos::pos()));
        result.push_back(exp(sort_pos::pos(), nat()));
        result.push_back(exp(nat(), nat()));
        result.push_back(even());
        result.push_back(monus());
        result.push_back(swap_zero());
        result.push_back(swap_zero_add());
        result.push_back(swap_zero_min());
        result.push_back(swap_zero_monus());
        result.push_back(swap_zero_lte());
        result.push_back(first());
        result.push_back(last());
        result.push_back(divmod());
        result.push_back(gdivmod());
        result.push_back(ggdivmod());
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
        if (is_times_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_monus_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_swap_zero_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        proj2 from an application
      /// \param e A data expression
      /// \pre proj2 is defined for e
      /// \return The argument of e that corresponds to proj2
      inline
      data_expression proj2(const data_expression& e)
      {
        if (is_cpair_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        proj1 from an application
      /// \param e A data expression
      /// \pre proj1 is defined for e
      /// \return The argument of e that corresponds to proj1
      inline
      data_expression proj1(const data_expression& e)
      {
        if (is_cpair_application(e))
        {
          return static_cast< application >(e).arguments()[0];
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
        if (is_gtesubt_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_gtesubtb_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
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
        if (is_swap_zero_add_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_swap_zero_min_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_swap_zero_monus_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_swap_zero_lte_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_divmod_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_gdivmod_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_ggdivmod_application(e))
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
        if (is_gtesubt_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_gtesubtb_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
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
        if (is_swap_zero_add_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_swap_zero_min_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_swap_zero_monus_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_swap_zero_lte_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_divmod_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_ggdivmod_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg3 from an application
      /// \param e A data expression
      /// \pre arg3 is defined for e
      /// \return The argument of e that corresponds to arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        if (is_swap_zero_add_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_swap_zero_min_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_swap_zero_monus_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_swap_zero_lte_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_gdivmod_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        if (is_ggdivmod_application(e))
        {
          return static_cast< application >(e).arguments()[2];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        if (is_swap_zero_add_application(e))
        {
          return static_cast< application >(e).arguments()[3];
        }
        if (is_swap_zero_min_application(e))
        {
          return static_cast< application >(e).arguments()[3];
        }
        if (is_swap_zero_monus_application(e))
        {
          return static_cast< application >(e).arguments()[3];
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
        if (is_cnat_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_pos2nat_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_nat2pos_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_dub_application(e))
        {
          return static_cast< application >(e).arguments()[1];
        }
        if (is_even_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      ///\brief Function for projecting out argument
      ///        pair from an application
      /// \param e A data expression
      /// \pre pair is defined for e
      /// \return The argument of e that corresponds to pair
      inline
      data_expression pair(const data_expression& e)
      {
        if (is_first_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_last_application(e))
        {
          return static_cast< application >(e).arguments()[0];
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
        if (is_gtesubtb_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_gdivmod_application(e))
        {
          return static_cast< application >(e).arguments()[1];
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
        if (is_times_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_monus_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        if (is_swap_zero_application(e))
        {
          return static_cast< application >(e).arguments()[0];
        }
        // This should never be reached, otherwise something is very wrong.
        assert(false);
      }

      /// \brief Give all system defined equations for nat
      /// \return All system defined equations for sort nat
      inline
      data_equation_vector nat_generate_equations_code()
      {
        variable vb("b",sort_bool_::bool_());
        variable vc("c",sort_bool_::bool_());
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vn("n",nat());
        variable vm("m",nat());
        variable vu("u",nat());
        variable vv("v",nat());

        data_equation_vector result;
        result.push_back(data_equation(make_vector(vp), equal_to(c0(), cnat(vp)), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp), equal_to(cnat(vp), c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp, vq), equal_to(cnat(vp), cnat(vq)), equal_to(vp, vq)));
        result.push_back(data_equation(make_vector(vn), less(vn, c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp), less(c0(), cnat(vp)), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vp, vq), less(cnat(vp), cnat(vq)), less(vp, vq)));
        result.push_back(data_equation(make_vector(vn), less_equal(c0(), vn), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(vp), less_equal(cnat(vp), c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(cnat(vp), cnat(vq)), less_equal(vp, vq)));
        result.push_back(data_equation(variable_list(), pos2nat(), cnat()));
        result.push_back(data_equation(make_vector(vp), nat2pos(cnat(vp)), vp));
        result.push_back(data_equation(make_vector(vp), maximum(vp, c0()), vp));
        result.push_back(data_equation(make_vector(vp, vq), maximum(vp, cnat(vq)), if_(less_equal(vp, vq), vq, vp)));
        result.push_back(data_equation(make_vector(vp), maximum(c0(), vp), vp));
        result.push_back(data_equation(make_vector(vp, vq), maximum(cnat(vp), vq), if_(less_equal(vp, vq), vq, vp)));
        result.push_back(data_equation(make_vector(vm, vn), maximum(vm, vn), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(make_vector(vm, vn), minimum(vm, vn), if_(less_equal(vm, vn), vm, vn)));
        result.push_back(data_equation(make_vector(vn), abs(vn), vn));
        result.push_back(data_equation(variable_list(), succ(c0()), sort_pos::c1()));
        result.push_back(data_equation(make_vector(vp), succ(cnat(vp)), succ(vp)));
        result.push_back(data_equation(make_vector(vn), succ(succ(vn)), sort_pos::cdub(equal_to(mod(vn, sort_pos::cdub(sort_bool_::false_(), sort_pos::c1())), cnat(sort_pos::c1())), succ(div(vn, sort_pos::cdub(sort_bool_::false_(), sort_pos::c1()))))));
        result.push_back(data_equation(variable_list(), pred(sort_pos::c1()), c0()));
        result.push_back(data_equation(make_vector(vp), pred(sort_pos::cdub(sort_bool_::true_(), vp)), cnat(sort_pos::cdub(sort_bool_::false_(), vp))));
        result.push_back(data_equation(make_vector(vp), pred(sort_pos::cdub(sort_bool_::false_(), vp)), dub(sort_bool_::true_(), pred(vp))));
        result.push_back(data_equation(variable_list(), dub(sort_bool_::false_(), c0()), c0()));
        result.push_back(data_equation(variable_list(), dub(sort_bool_::true_(), c0()), cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(vb, vp), dub(vb, cnat(vp)), cnat(sort_pos::cdub(vb, vp))));
        result.push_back(data_equation(make_vector(vp), plus(vp, c0()), vp));
        result.push_back(data_equation(make_vector(vp, vq), plus(vp, cnat(vq)), sort_pos::add_with_carry(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vp), plus(c0(), vp), vp));
        result.push_back(data_equation(make_vector(vp, vq), plus(cnat(vp), vq), sort_pos::add_with_carry(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vn), plus(c0(), vn), vn));
        result.push_back(data_equation(make_vector(vn), plus(vn, c0()), vn));
        result.push_back(data_equation(make_vector(vp, vq), plus(cnat(vp), cnat(vq)), cnat(sort_pos::add_with_carry(sort_bool_::false_(), vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), gtesubt(vp, vq), gtesubtb(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vn), gtesubt(vn, c0()), vn));
        result.push_back(data_equation(make_vector(vp, vq), gtesubt(cnat(vp), cnat(vq)), gtesubtb(sort_bool_::false_(), vp, vq)));
        result.push_back(data_equation(make_vector(vp), gtesubtb(sort_bool_::false_(), vp, sort_pos::c1()), pred(vp)));
        result.push_back(data_equation(make_vector(vp), gtesubtb(sort_bool_::true_(), vp, sort_pos::c1()), pred(nat2pos(pred(vp)))));
        result.push_back(data_equation(make_vector(vb, vc, vp, vq), gtesubtb(vb, sort_pos::cdub(vc, vp), sort_pos::cdub(vc, vq)), dub(vb, gtesubtb(vb, vp, vq))));
        result.push_back(data_equation(make_vector(vb, vp, vq), gtesubtb(vb, sort_pos::cdub(sort_bool_::false_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), dub(sort_bool_::not_(vb), gtesubtb(sort_bool_::true_(), vp, vq))));
        result.push_back(data_equation(make_vector(vb, vp, vq), gtesubtb(vb, sort_pos::cdub(sort_bool_::true_(), vp), sort_pos::cdub(sort_bool_::false_(), vq)), dub(sort_bool_::not_(vb), gtesubtb(sort_bool_::false_(), vp, vq))));
        result.push_back(data_equation(make_vector(vn), times(c0(), vn), c0()));
        result.push_back(data_equation(make_vector(vn), times(vn, c0()), c0()));
        result.push_back(data_equation(make_vector(vp, vq), times(cnat(vp), cnat(vq)), cnat(times(vp, vq))));
        result.push_back(data_equation(make_vector(vp), exp(vp, c0()), sort_pos::c1()));
        result.push_back(data_equation(make_vector(vp), exp(vp, cnat(sort_pos::c1())), vp));
        result.push_back(data_equation(make_vector(vp, vq), exp(vp, cnat(sort_pos::cdub(sort_bool_::false_(), vq))), exp(sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), vp, vp), cnat(vq))));
        result.push_back(data_equation(make_vector(vp, vq), exp(vp, cnat(sort_pos::cdub(sort_bool_::true_(), vq))), sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), vp, exp(sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), vp, vp), cnat(vq)))));
        result.push_back(data_equation(make_vector(vn), exp(vn, c0()), cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(vp), exp(c0(), cnat(vp)), c0()));
        result.push_back(data_equation(make_vector(vn, vp), exp(cnat(vp), vn), cnat(exp(vp, vn))));
        result.push_back(data_equation(variable_list(), even(c0()), sort_bool_::true_()));
        result.push_back(data_equation(variable_list(), even(cnat(sort_pos::c1())), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(vb, vp), even(cnat(sort_pos::cdub(vb, vp))), sort_bool_::not_(vb)));
        result.push_back(data_equation(make_vector(vp), div(vp, sort_pos::c1()), cnat(vp)));
        result.push_back(data_equation(make_vector(vb, vp), div(sort_pos::c1(), sort_pos::cdub(vb, vp)), c0()));
        result.push_back(data_equation(make_vector(vb, vp, vq), div(sort_pos::cdub(vb, vp), sort_pos::cdub(sort_bool_::false_(), vq)), div(vp, vq)));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vp, vq), div(sort_pos::cdub(sort_bool_::false_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), c0()));
        result.push_back(data_equation(make_vector(vp, vq), less(vq, vp), div(sort_pos::cdub(sort_bool_::false_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), first(gdivmod(divmod(vp, sort_pos::cdub(sort_bool_::true_(), vq)), sort_bool_::false_(), sort_pos::cdub(sort_bool_::true_(), vq)))));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vp, vq), div(sort_pos::cdub(sort_bool_::true_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), if_(equal_to(vp, vq), cnat(sort_pos::c1()), c0())));
        result.push_back(data_equation(make_vector(vp, vq), less(vq, vp), div(sort_pos::cdub(sort_bool_::true_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), first(gdivmod(divmod(vp, sort_pos::cdub(sort_bool_::true_(), vq)), sort_bool_::true_(), sort_pos::cdub(sort_bool_::true_(), vq)))));
        result.push_back(data_equation(make_vector(vp), div(c0(), vp), c0()));
        result.push_back(data_equation(make_vector(vp, vq), div(cnat(vp), vq), div(vp, vq)));
        result.push_back(data_equation(make_vector(vp), mod(vp, sort_pos::c1()), c0()));
        result.push_back(data_equation(make_vector(vb, vp), mod(sort_pos::c1(), sort_pos::cdub(vb, vp)), cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(vb, vp, vq), mod(sort_pos::cdub(vb, vp), sort_pos::cdub(sort_bool_::false_(), vq)), dub(vb, mod(vp, vq))));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vp, vq), mod(sort_pos::cdub(sort_bool_::false_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), cnat(sort_pos::cdub(sort_bool_::false_(), vp))));
        result.push_back(data_equation(make_vector(vp, vq), less(vq, vp), mod(sort_pos::cdub(sort_bool_::false_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), last(gdivmod(divmod(vp, sort_pos::cdub(sort_bool_::true_(), vq)), sort_bool_::false_(), sort_pos::cdub(sort_bool_::true_(), vq)))));
        result.push_back(data_equation(make_vector(vp, vq), less_equal(vp, vq), mod(sort_pos::cdub(sort_bool_::true_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), if_(equal_to(vp, vq), c0(), cnat(sort_pos::cdub(sort_bool_::true_(), vp)))));
        result.push_back(data_equation(make_vector(vp, vq), less(vq, vp), mod(sort_pos::cdub(sort_bool_::true_(), vp), sort_pos::cdub(sort_bool_::true_(), vq)), last(gdivmod(divmod(vp, sort_pos::cdub(sort_bool_::true_(), vq)), sort_bool_::true_(), sort_pos::cdub(sort_bool_::true_(), vq)))));
        result.push_back(data_equation(make_vector(vp), mod(c0(), vp), c0()));
        result.push_back(data_equation(make_vector(vp, vq), mod(cnat(vp), vq), mod(vp, vq)));
        result.push_back(data_equation(make_vector(vm, vn), less_equal(vm, vn), monus(vm, vn), c0()));
        result.push_back(data_equation(make_vector(vm, vn), less(vn, vm), monus(vm, vn), gtesubt(vm, vn)));
        result.push_back(data_equation(make_vector(vm), swap_zero(vm, c0()), vm));
        result.push_back(data_equation(make_vector(vn), swap_zero(c0(), vn), vn));
        result.push_back(data_equation(make_vector(vp), swap_zero(cnat(vp), cnat(vp)), c0()));
        result.push_back(data_equation(make_vector(vp, vq), not_equal_to(vp, vq), swap_zero(cnat(vp), cnat(vq)), cnat(vq)));
        result.push_back(data_equation(make_vector(vm, vn), swap_zero_add(c0(), c0(), vm, vn), plus(vm, vn)));
        result.push_back(data_equation(make_vector(vm, vp), swap_zero_add(cnat(vp), c0(), vm, c0()), vm));
        result.push_back(data_equation(make_vector(vm, vp, vq), swap_zero_add(cnat(vp), c0(), vm, cnat(vq)), swap_zero(cnat(vp), plus(swap_zero(cnat(vp), vm), cnat(vq)))));
        result.push_back(data_equation(make_vector(vn, vp), swap_zero_add(c0(), cnat(vp), c0(), vn), vn));
        result.push_back(data_equation(make_vector(vn, vp, vq), swap_zero_add(c0(), cnat(vp), cnat(vq), vn), swap_zero(cnat(vp), plus(cnat(vq), swap_zero(cnat(vp), vn)))));
        result.push_back(data_equation(make_vector(vm, vn, vp, vq), swap_zero_add(cnat(vp), cnat(vq), vm, vn), swap_zero(plus(cnat(vp), cnat(vq)), plus(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(make_vector(vm, vn), swap_zero_min(c0(), c0(), vm, vn), minimum(vm, vn)));
        result.push_back(data_equation(make_vector(vm, vp), swap_zero_min(cnat(vp), c0(), vm, c0()), c0()));
        result.push_back(data_equation(make_vector(vm, vp, vq), swap_zero_min(cnat(vp), c0(), vm, cnat(vq)), minimum(swap_zero(cnat(vp), vm), cnat(vq))));
        result.push_back(data_equation(make_vector(vn, vp), swap_zero_min(c0(), cnat(vp), c0(), vn), c0()));
        result.push_back(data_equation(make_vector(vn, vp, vq), swap_zero_min(c0(), cnat(vp), cnat(vq), vn), minimum(cnat(vq), swap_zero(cnat(vp), vn))));
        result.push_back(data_equation(make_vector(vm, vn, vp, vq), swap_zero_min(cnat(vp), cnat(vq), vm, vn), swap_zero(minimum(cnat(vp), cnat(vq)), minimum(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(make_vector(vm, vn), swap_zero_monus(c0(), c0(), vm, vn), monus(vm, vn)));
        result.push_back(data_equation(make_vector(vm, vp), swap_zero_monus(cnat(vp), c0(), vm, c0()), vm));
        result.push_back(data_equation(make_vector(vm, vp, vq), swap_zero_monus(cnat(vp), c0(), vm, cnat(vq)), swap_zero(cnat(vp), monus(swap_zero(cnat(vp), vm), cnat(vq)))));
        result.push_back(data_equation(make_vector(vn, vp), swap_zero_monus(c0(), cnat(vp), c0(), vn), c0()));
        result.push_back(data_equation(make_vector(vn, vp, vq), swap_zero_monus(c0(), cnat(vp), cnat(vq), vn), monus(cnat(vq), swap_zero(cnat(vp), vn))));
        result.push_back(data_equation(make_vector(vm, vn, vp, vq), swap_zero_monus(cnat(vp), cnat(vq), vm, vn), swap_zero(monus(cnat(vp), cnat(vq)), monus(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(make_vector(vm, vn), swap_zero_lte(c0(), vm, vn), less_equal(vm, vn)));
        result.push_back(data_equation(make_vector(vm, vn, vp), swap_zero_lte(cnat(vp), vm, vn), less_equal(swap_zero(cnat(vp), vm), swap_zero(cnat(vp), vn))));
        result.push_back(data_equation(make_vector(vm, vn, vu, vv), equal_to(cpair(vm, vn), cpair(vu, vv)), sort_bool_::and_(equal_to(vm, vu), equal_to(vn, vv))));
        result.push_back(data_equation(make_vector(vm, vn, vu, vv), less(cpair(vm, vn), cpair(vu, vv)), sort_bool_::or_(less(vm, vu), sort_bool_::and_(equal_to(vm, vu), less(vn, vv)))));
        result.push_back(data_equation(make_vector(vm, vn, vu, vv), less_equal(cpair(vm, vn), cpair(vu, vv)), sort_bool_::or_(less(vm, vu), sort_bool_::and_(equal_to(vm, vu), less_equal(vn, vv)))));
        result.push_back(data_equation(make_vector(vm, vn), first(cpair(vm, vn)), vm));
        result.push_back(data_equation(make_vector(vm, vn), last(cpair(vm, vn)), vn));
        result.push_back(data_equation(variable_list(), divmod(sort_pos::c1(), sort_pos::c1()), cpair(cnat(sort_pos::c1()), c0())));
        result.push_back(data_equation(make_vector(vb, vp), divmod(sort_pos::c1(), sort_pos::cdub(vb, vp)), cpair(c0(), cnat(sort_pos::c1()))));
        result.push_back(data_equation(make_vector(vb, vp, vq), divmod(sort_pos::cdub(vb, vp), vq), gdivmod(divmod(vp, vq), vb, vq)));
        result.push_back(data_equation(make_vector(vb, vm, vn, vp), gdivmod(cpair(vm, vn), vb, vp), ggdivmod(dub(vb, vn), vm, vp)));
        result.push_back(data_equation(make_vector(vn, vp), ggdivmod(c0(), vn, vp), cpair(dub(sort_bool_::false_(), vn), c0())));
        result.push_back(data_equation(make_vector(vn, vp, vq), less(vp, vq), ggdivmod(cnat(vp), vn, vq), cpair(dub(sort_bool_::false_(), vn), cnat(vp))));
        result.push_back(data_equation(make_vector(vn, vp, vq), less_equal(vq, vp), ggdivmod(cnat(vp), vn, vq), cpair(dub(sort_bool_::true_(), vn), gtesubtb(sort_bool_::false_(), vp, vq))));
        return result;
      }

      /// \brief Add sort, constructors, mappings and equations for nat
      /// \param specification a specification
      inline
      void add_nat_to_specification(data_specification& specification)
      {
         if (specification.constructors(sort_pos::pos()).empty())
         {
           sort_pos::add_pos_to_specification(specification);
         }
         if (specification.constructors(sort_bool_::bool_()).empty())
         {
           sort_bool_::add_bool__to_specification(specification);
         }
         specification.add_system_defined_sort(natpair());
         specification.add_system_defined_sort(nat());
         specification.add_system_defined_constructors(nat_generate_constructors_code());
         specification.add_system_defined_mappings(nat_generate_functions_code());
         specification.add_system_defined_equations(nat_generate_equations_code());
      }
    } // namespace sort_nat

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NAT_H
