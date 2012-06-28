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

#include "boost/utility.hpp"

#include "mcrl2/utilities/workarounds.h"
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

namespace mcrl2 {

  namespace data {

    /// \brief Namespace for system defined sort nat
    namespace sort_nat {

      inline
      core::identifier_string const& nat_name()
      {
        static core::identifier_string nat_name = core::detail::initialise_static_expression(nat_name, core::identifier_string("Nat"));
        return nat_name;
      }

      /// \brief Constructor for sort expression Nat
      /// \return Sort expression Nat
      inline
      basic_sort const& nat()
      {
        static basic_sort nat = core::detail::initialise_static_expression(nat, basic_sort(nat_name()));
        return nat;
      }

      /// \brief Recogniser for sort expression Nat
      /// \param e A sort expression
      /// \return true iff e == nat()
      inline
      bool is_nat(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == nat();
        }
        return false;
      }

      inline
      core::identifier_string const& natpair_name()
      {
        static core::identifier_string natpair_name = core::detail::initialise_static_expression(natpair_name, core::identifier_string("@NatPair"));
        return natpair_name;
      }

      /// \brief Constructor for sort expression \@NatPair
      /// \return Sort expression \@NatPair
      inline
      basic_sort const& natpair()
      {
        static basic_sort natpair = core::detail::initialise_static_expression(natpair, basic_sort(natpair_name()));
        return natpair;
      }

      /// \brief Recogniser for sort expression \@NatPair
      /// \param e A sort expression
      /// \return true iff e == natpair()
      inline
      bool is_natpair(const sort_expression& e)
      {
        if (is_basic_sort(e))
        {
          return basic_sort(e) == natpair();
        }
        return false;
      }

      /// \brief Generate identifier \@c0
      /// \return Identifier \@c0
      inline
      core::identifier_string const& c0_name()
      {
        static core::identifier_string c0_name = core::detail::initialise_static_expression(c0_name, core::identifier_string("@c0"));
        return c0_name;
      }

      /// \brief Constructor for function symbol \@c0
      /// \return Function symbol c0
      inline
      function_symbol const& c0()
      {
        static function_symbol c0 = core::detail::initialise_static_expression(c0, function_symbol(c0_name(), nat()));
        return c0;
      }


      /// \brief Recogniser for function \@c0
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@c0
      inline
      bool is_c0_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == c0();
        }
        return false;
      }

      /// \brief Generate identifier \@cNat
      /// \return Identifier \@cNat
      inline
      core::identifier_string const& cnat_name()
      {
        static core::identifier_string cnat_name = core::detail::initialise_static_expression(cnat_name, core::identifier_string("@cNat"));
        return cnat_name;
      }

      /// \brief Constructor for function symbol \@cNat
      /// \return Function symbol cnat
      inline
      function_symbol const& cnat()
      {
        static function_symbol cnat = core::detail::initialise_static_expression(cnat, function_symbol(cnat_name(), make_function_sort(sort_pos::pos(), nat())));
        return cnat;
      }


      /// \brief Recogniser for function \@cNat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cNat
      inline
      bool is_cnat_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == cnat();
        }
        return false;
      }

      /// \brief Application of function symbol \@cNat
      /// \param arg0 A data expression
      /// \return Application of \@cNat to a number of arguments
      inline
      application cnat(const data_expression& arg0)
      {
        return cnat()(arg0);
      }

      /// \brief Recogniser for application of \@cNat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cnat to a
      ///     number of arguments
      inline
      bool is_cnat_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cnat_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@cPair
      /// \return Identifier \@cPair
      inline
      core::identifier_string const& cpair_name()
      {
        static core::identifier_string cpair_name = core::detail::initialise_static_expression(cpair_name, core::identifier_string("@cPair"));
        return cpair_name;
      }

      /// \brief Constructor for function symbol \@cPair
      /// \return Function symbol cpair
      inline
      function_symbol const& cpair()
      {
        static function_symbol cpair = core::detail::initialise_static_expression(cpair, function_symbol(cpair_name(), make_function_sort(nat(), nat(), natpair())));
        return cpair;
      }


      /// \brief Recogniser for function \@cPair
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@cPair
      inline
      bool is_cpair_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == cpair();
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
        return cpair()(arg0, arg1);
      }

      /// \brief Recogniser for application of \@cPair
      /// \param e A data expression
      /// \return true iff e is an application of function symbol cpair to a
      ///     number of arguments
      inline
      bool is_cpair_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_cpair_function_symbol(application(e).head());
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
      /// \brief Generate identifier Pos2Nat
      /// \return Identifier Pos2Nat
      inline
      core::identifier_string const& pos2nat_name()
      {
        static core::identifier_string pos2nat_name = core::detail::initialise_static_expression(pos2nat_name, core::identifier_string("Pos2Nat"));
        return pos2nat_name;
      }

      /// \brief Constructor for function symbol Pos2Nat
      /// \return Function symbol pos2nat
      inline
      function_symbol const& pos2nat()
      {
        static function_symbol pos2nat = core::detail::initialise_static_expression(pos2nat, function_symbol(pos2nat_name(), make_function_sort(sort_pos::pos(), nat())));
        return pos2nat;
      }


      /// \brief Recogniser for function Pos2Nat
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Pos2Nat
      inline
      bool is_pos2nat_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == pos2nat();
        }
        return false;
      }

      /// \brief Application of function symbol Pos2Nat
      /// \param arg0 A data expression
      /// \return Application of Pos2Nat to a number of arguments
      inline
      application pos2nat(const data_expression& arg0)
      {
        return pos2nat()(arg0);
      }

      /// \brief Recogniser for application of Pos2Nat
      /// \param e A data expression
      /// \return true iff e is an application of function symbol pos2nat to a
      ///     number of arguments
      inline
      bool is_pos2nat_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_pos2nat_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier Nat2Pos
      /// \return Identifier Nat2Pos
      inline
      core::identifier_string const& nat2pos_name()
      {
        static core::identifier_string nat2pos_name = core::detail::initialise_static_expression(nat2pos_name, core::identifier_string("Nat2Pos"));
        return nat2pos_name;
      }

      /// \brief Constructor for function symbol Nat2Pos
      /// \return Function symbol nat2pos
      inline
      function_symbol const& nat2pos()
      {
        static function_symbol nat2pos = core::detail::initialise_static_expression(nat2pos, function_symbol(nat2pos_name(), make_function_sort(nat(), sort_pos::pos())));
        return nat2pos;
      }


      /// \brief Recogniser for function Nat2Pos
      /// \param e A data expression
      /// \return true iff e is the function symbol matching Nat2Pos
      inline
      bool is_nat2pos_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == nat2pos();
        }
        return false;
      }

      /// \brief Application of function symbol Nat2Pos
      /// \param arg0 A data expression
      /// \return Application of Nat2Pos to a number of arguments
      inline
      application nat2pos(const data_expression& arg0)
      {
        return nat2pos()(arg0);
      }

      /// \brief Recogniser for application of Nat2Pos
      /// \param e A data expression
      /// \return true iff e is an application of function symbol nat2pos to a
      ///     number of arguments
      inline
      bool is_nat2pos_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_nat2pos_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier max
      /// \return Identifier max
      inline
      core::identifier_string const& maximum_name()
      {
        static core::identifier_string maximum_name = core::detail::initialise_static_expression(maximum_name, core::identifier_string("max"));
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
          throw mcrl2::runtime_error("cannot compute target sort for maximum with domain sorts " + s0.to_string() + ", " + s1.to_string());
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
          return f.name() == maximum_name() && function_sort(f.sort()).domain().size() == 2 && (f == maximum(sort_pos::pos(), nat()) || f == maximum(nat(), sort_pos::pos()) || f == maximum(nat(), nat()) || f == maximum(sort_pos::pos(), sort_pos::pos()));
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
        return maximum(arg0.sort(), arg1.sort())(arg0, arg1);
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
        static core::identifier_string minimum_name = core::detail::initialise_static_expression(minimum_name, core::identifier_string("min"));
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
          throw mcrl2::runtime_error("cannot compute target sort for minimum with domain sorts " + s0.to_string() + ", " + s1.to_string());
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
          return f.name() == minimum_name() && function_sort(f.sort()).domain().size() == 2 && (f == minimum(nat(), nat()) || f == minimum(sort_pos::pos(), sort_pos::pos()));
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
        return minimum(arg0.sort(), arg1.sort())(arg0, arg1);
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

      /// \brief Generate identifier succ
      /// \return Identifier succ
      inline
      core::identifier_string const& succ_name()
      {
        static core::identifier_string succ_name = core::detail::initialise_static_expression(succ_name, core::identifier_string("succ"));
        return succ_name;
      }

      ///\brief Constructor for function symbol succ
      /// \param s0 A sort expression
      ///\return Function symbol succ
      inline
      function_symbol succ(const sort_expression& s0)
      {
        sort_expression target_sort(sort_pos::pos());

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
          return f.name() == succ_name() && function_sort(f.sort()).domain().size() == 1 && (f == succ(nat()) || f == succ(sort_pos::pos()));
        }
        return false;
      }

      /// \brief Application of function symbol succ
      /// \param arg0 A data expression
      /// \return Application of succ to a number of arguments
      inline
      application succ(const data_expression& arg0)
      {
        return succ(arg0.sort())(arg0);
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
        static core::identifier_string pred_name = core::detail::initialise_static_expression(pred_name, core::identifier_string("pred"));
        return pred_name;
      }

      /// \brief Constructor for function symbol pred
      /// \return Function symbol pred
      inline
      function_symbol const& pred()
      {
        static function_symbol pred = core::detail::initialise_static_expression(pred, function_symbol(pred_name(), make_function_sort(sort_pos::pos(), nat())));
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
          return function_symbol(e) == pred();
        }
        return false;
      }

      /// \brief Application of function symbol pred
      /// \param arg0 A data expression
      /// \return Application of pred to a number of arguments
      inline
      application pred(const data_expression& arg0)
      {
        return pred()(arg0);
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
        static core::identifier_string dub_name = core::detail::initialise_static_expression(dub_name, core::identifier_string("@dub"));
        return dub_name;
      }

      /// \brief Constructor for function symbol \@dub
      /// \return Function symbol dub
      inline
      function_symbol const& dub()
      {
        static function_symbol dub = core::detail::initialise_static_expression(dub, function_symbol(dub_name(), make_function_sort(sort_bool::bool_(), nat(), nat())));
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
          return function_symbol(e) == dub();
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
        return dub()(arg0, arg1);
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
        static core::identifier_string plus_name = core::detail::initialise_static_expression(plus_name, core::identifier_string("+"));
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
          throw mcrl2::runtime_error("cannot compute target sort for plus with domain sorts " + s0.to_string() + ", " + s1.to_string());
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
          return f.name() == plus_name() && function_sort(f.sort()).domain().size() == 2 && (f == plus(sort_pos::pos(), nat()) || f == plus(nat(), sort_pos::pos()) || f == plus(nat(), nat()) || f == plus(sort_pos::pos(), sort_pos::pos()));
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
        return plus(arg0.sort(), arg1.sort())(arg0, arg1);
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

      /// \brief Generate identifier \@gtesubtb
      /// \return Identifier \@gtesubtb
      inline
      core::identifier_string const& gte_subtract_with_borrow_name()
      {
        static core::identifier_string gte_subtract_with_borrow_name = core::detail::initialise_static_expression(gte_subtract_with_borrow_name, core::identifier_string("@gtesubtb"));
        return gte_subtract_with_borrow_name;
      }

      /// \brief Constructor for function symbol \@gtesubtb
      /// \return Function symbol gte_subtract_with_borrow
      inline
      function_symbol const& gte_subtract_with_borrow()
      {
        static function_symbol gte_subtract_with_borrow = core::detail::initialise_static_expression(gte_subtract_with_borrow, function_symbol(gte_subtract_with_borrow_name(), make_function_sort(sort_bool::bool_(), sort_pos::pos(), sort_pos::pos(), nat())));
        return gte_subtract_with_borrow;
      }


      /// \brief Recogniser for function \@gtesubtb
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@gtesubtb
      inline
      bool is_gte_subtract_with_borrow_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == gte_subtract_with_borrow();
        }
        return false;
      }

      /// \brief Application of function symbol \@gtesubtb
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@gtesubtb to a number of arguments
      inline
      application gte_subtract_with_borrow(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return gte_subtract_with_borrow()(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@gtesubtb
      /// \param e A data expression
      /// \return true iff e is an application of function symbol gte_subtract_with_borrow to a
      ///     number of arguments
      inline
      bool is_gte_subtract_with_borrow_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_gte_subtract_with_borrow_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier *
      /// \return Identifier *
      inline
      core::identifier_string const& times_name()
      {
        static core::identifier_string times_name = core::detail::initialise_static_expression(times_name, core::identifier_string("*"));
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
          throw mcrl2::runtime_error("cannot compute target sort for times with domain sorts " + s0.to_string() + ", " + s1.to_string());
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
          return f.name() == times_name() && function_sort(f.sort()).domain().size() == 2 && (f == times(nat(), nat()) || f == times(sort_pos::pos(), sort_pos::pos()));
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
        return times(arg0.sort(), arg1.sort())(arg0, arg1);
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
        static core::identifier_string div_name = core::detail::initialise_static_expression(div_name, core::identifier_string("div"));
        return div_name;
      }

      /// \brief Constructor for function symbol div
      /// \return Function symbol div
      inline
      function_symbol const& div()
      {
        static function_symbol div = core::detail::initialise_static_expression(div, function_symbol(div_name(), make_function_sort(nat(), sort_pos::pos(), nat())));
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
          return function_symbol(e) == div();
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
        return div()(arg0, arg1);
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
        static core::identifier_string mod_name = core::detail::initialise_static_expression(mod_name, core::identifier_string("mod"));
        return mod_name;
      }

      /// \brief Constructor for function symbol mod
      /// \return Function symbol mod
      inline
      function_symbol const& mod()
      {
        static function_symbol mod = core::detail::initialise_static_expression(mod, function_symbol(mod_name(), make_function_sort(nat(), sort_pos::pos(), nat())));
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
          return function_symbol(e) == mod();
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
        return mod()(arg0, arg1);
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
        static core::identifier_string exp_name = core::detail::initialise_static_expression(exp_name, core::identifier_string("exp"));
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
          throw mcrl2::runtime_error("cannot compute target sort for exp with domain sorts " + s0.to_string() + ", " + s1.to_string());
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
          return f.name() == exp_name() && function_sort(f.sort()).domain().size() == 2 && (f == exp(sort_pos::pos(), nat()) || f == exp(nat(), nat()));
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
        return exp(arg0.sort(), arg1.sort())(arg0, arg1);
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

      /// \brief Generate identifier \@even
      /// \return Identifier \@even
      inline
      core::identifier_string const& even_name()
      {
        static core::identifier_string even_name = core::detail::initialise_static_expression(even_name, core::identifier_string("@even"));
        return even_name;
      }

      /// \brief Constructor for function symbol \@even
      /// \return Function symbol even
      inline
      function_symbol const& even()
      {
        static function_symbol even = core::detail::initialise_static_expression(even, function_symbol(even_name(), make_function_sort(nat(), sort_bool::bool_())));
        return even;
      }


      /// \brief Recogniser for function \@even
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@even
      inline
      bool is_even_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == even();
        }
        return false;
      }

      /// \brief Application of function symbol \@even
      /// \param arg0 A data expression
      /// \return Application of \@even to a number of arguments
      inline
      application even(const data_expression& arg0)
      {
        return even()(arg0);
      }

      /// \brief Recogniser for application of \@even
      /// \param e A data expression
      /// \return true iff e is an application of function symbol even to a
      ///     number of arguments
      inline
      bool is_even_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_even_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@monus
      /// \return Identifier \@monus
      inline
      core::identifier_string const& monus_name()
      {
        static core::identifier_string monus_name = core::detail::initialise_static_expression(monus_name, core::identifier_string("@monus"));
        return monus_name;
      }

      /// \brief Constructor for function symbol \@monus
      /// \return Function symbol monus
      inline
      function_symbol const& monus()
      {
        static function_symbol monus = core::detail::initialise_static_expression(monus, function_symbol(monus_name(), make_function_sort(nat(), nat(), nat())));
        return monus;
      }


      /// \brief Recogniser for function \@monus
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@monus
      inline
      bool is_monus_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == monus();
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
        return monus()(arg0, arg1);
      }

      /// \brief Recogniser for application of \@monus
      /// \param e A data expression
      /// \return true iff e is an application of function symbol monus to a
      ///     number of arguments
      inline
      bool is_monus_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_monus_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@swap_zero
      /// \return Identifier \@swap_zero
      inline
      core::identifier_string const& swap_zero_name()
      {
        static core::identifier_string swap_zero_name = core::detail::initialise_static_expression(swap_zero_name, core::identifier_string("@swap_zero"));
        return swap_zero_name;
      }

      /// \brief Constructor for function symbol \@swap_zero
      /// \return Function symbol swap_zero
      inline
      function_symbol const& swap_zero()
      {
        static function_symbol swap_zero = core::detail::initialise_static_expression(swap_zero, function_symbol(swap_zero_name(), make_function_sort(nat(), nat(), nat())));
        return swap_zero;
      }


      /// \brief Recogniser for function \@swap_zero
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero
      inline
      bool is_swap_zero_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == swap_zero();
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
        return swap_zero()(arg0, arg1);
      }

      /// \brief Recogniser for application of \@swap_zero
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero to a
      ///     number of arguments
      inline
      bool is_swap_zero_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_swap_zero_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@swap_zero_add
      /// \return Identifier \@swap_zero_add
      inline
      core::identifier_string const& swap_zero_add_name()
      {
        static core::identifier_string swap_zero_add_name = core::detail::initialise_static_expression(swap_zero_add_name, core::identifier_string("@swap_zero_add"));
        return swap_zero_add_name;
      }

      /// \brief Constructor for function symbol \@swap_zero_add
      /// \return Function symbol swap_zero_add
      inline
      function_symbol const& swap_zero_add()
      {
        static function_symbol swap_zero_add = core::detail::initialise_static_expression(swap_zero_add, function_symbol(swap_zero_add_name(), make_function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_add;
      }


      /// \brief Recogniser for function \@swap_zero_add
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_add
      inline
      bool is_swap_zero_add_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == swap_zero_add();
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
        return swap_zero_add()(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_add
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_add to a
      ///     number of arguments
      inline
      bool is_swap_zero_add_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_swap_zero_add_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@swap_zero_min
      /// \return Identifier \@swap_zero_min
      inline
      core::identifier_string const& swap_zero_min_name()
      {
        static core::identifier_string swap_zero_min_name = core::detail::initialise_static_expression(swap_zero_min_name, core::identifier_string("@swap_zero_min"));
        return swap_zero_min_name;
      }

      /// \brief Constructor for function symbol \@swap_zero_min
      /// \return Function symbol swap_zero_min
      inline
      function_symbol const& swap_zero_min()
      {
        static function_symbol swap_zero_min = core::detail::initialise_static_expression(swap_zero_min, function_symbol(swap_zero_min_name(), make_function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_min;
      }


      /// \brief Recogniser for function \@swap_zero_min
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_min
      inline
      bool is_swap_zero_min_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == swap_zero_min();
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
        return swap_zero_min()(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_min
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_min to a
      ///     number of arguments
      inline
      bool is_swap_zero_min_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_swap_zero_min_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@swap_zero_monus
      /// \return Identifier \@swap_zero_monus
      inline
      core::identifier_string const& swap_zero_monus_name()
      {
        static core::identifier_string swap_zero_monus_name = core::detail::initialise_static_expression(swap_zero_monus_name, core::identifier_string("@swap_zero_monus"));
        return swap_zero_monus_name;
      }

      /// \brief Constructor for function symbol \@swap_zero_monus
      /// \return Function symbol swap_zero_monus
      inline
      function_symbol const& swap_zero_monus()
      {
        static function_symbol swap_zero_monus = core::detail::initialise_static_expression(swap_zero_monus, function_symbol(swap_zero_monus_name(), make_function_sort(nat(), nat(), nat(), nat(), nat())));
        return swap_zero_monus;
      }


      /// \brief Recogniser for function \@swap_zero_monus
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@swap_zero_monus
      inline
      bool is_swap_zero_monus_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == swap_zero_monus();
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
        return swap_zero_monus()(arg0, arg1, arg2, arg3);
      }

      /// \brief Recogniser for application of \@swap_zero_monus
      /// \param e A data expression
      /// \return true iff e is an application of function symbol swap_zero_monus to a
      ///     number of arguments
      inline
      bool is_swap_zero_monus_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_swap_zero_monus_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@first
      /// \return Identifier \@first
      inline
      core::identifier_string const& first_name()
      {
        static core::identifier_string first_name = core::detail::initialise_static_expression(first_name, core::identifier_string("@first"));
        return first_name;
      }

      /// \brief Constructor for function symbol \@first
      /// \return Function symbol first
      inline
      function_symbol const& first()
      {
        static function_symbol first = core::detail::initialise_static_expression(first, function_symbol(first_name(), make_function_sort(natpair(), nat())));
        return first;
      }


      /// \brief Recogniser for function \@first
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@first
      inline
      bool is_first_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == first();
        }
        return false;
      }

      /// \brief Application of function symbol \@first
      /// \param arg0 A data expression
      /// \return Application of \@first to a number of arguments
      inline
      application first(const data_expression& arg0)
      {
        return first()(arg0);
      }

      /// \brief Recogniser for application of \@first
      /// \param e A data expression
      /// \return true iff e is an application of function symbol first to a
      ///     number of arguments
      inline
      bool is_first_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_first_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@last
      /// \return Identifier \@last
      inline
      core::identifier_string const& last_name()
      {
        static core::identifier_string last_name = core::detail::initialise_static_expression(last_name, core::identifier_string("@last"));
        return last_name;
      }

      /// \brief Constructor for function symbol \@last
      /// \return Function symbol last
      inline
      function_symbol const& last()
      {
        static function_symbol last = core::detail::initialise_static_expression(last, function_symbol(last_name(), make_function_sort(natpair(), nat())));
        return last;
      }


      /// \brief Recogniser for function \@last
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@last
      inline
      bool is_last_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == last();
        }
        return false;
      }

      /// \brief Application of function symbol \@last
      /// \param arg0 A data expression
      /// \return Application of \@last to a number of arguments
      inline
      application last(const data_expression& arg0)
      {
        return last()(arg0);
      }

      /// \brief Recogniser for application of \@last
      /// \param e A data expression
      /// \return true iff e is an application of function symbol last to a
      ///     number of arguments
      inline
      bool is_last_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_last_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@divmod
      /// \return Identifier \@divmod
      inline
      core::identifier_string const& divmod_name()
      {
        static core::identifier_string divmod_name = core::detail::initialise_static_expression(divmod_name, core::identifier_string("@divmod"));
        return divmod_name;
      }

      /// \brief Constructor for function symbol \@divmod
      /// \return Function symbol divmod
      inline
      function_symbol const& divmod()
      {
        static function_symbol divmod = core::detail::initialise_static_expression(divmod, function_symbol(divmod_name(), make_function_sort(sort_pos::pos(), sort_pos::pos(), natpair())));
        return divmod;
      }


      /// \brief Recogniser for function \@divmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@divmod
      inline
      bool is_divmod_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == divmod();
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
        return divmod()(arg0, arg1);
      }

      /// \brief Recogniser for application of \@divmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol divmod to a
      ///     number of arguments
      inline
      bool is_divmod_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_divmod_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@gdivmod
      /// \return Identifier \@gdivmod
      inline
      core::identifier_string const& generalised_divmod_name()
      {
        static core::identifier_string generalised_divmod_name = core::detail::initialise_static_expression(generalised_divmod_name, core::identifier_string("@gdivmod"));
        return generalised_divmod_name;
      }

      /// \brief Constructor for function symbol \@gdivmod
      /// \return Function symbol generalised_divmod
      inline
      function_symbol const& generalised_divmod()
      {
        static function_symbol generalised_divmod = core::detail::initialise_static_expression(generalised_divmod, function_symbol(generalised_divmod_name(), make_function_sort(natpair(), sort_bool::bool_(), sort_pos::pos(), natpair())));
        return generalised_divmod;
      }


      /// \brief Recogniser for function \@gdivmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@gdivmod
      inline
      bool is_generalised_divmod_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == generalised_divmod();
        }
        return false;
      }

      /// \brief Application of function symbol \@gdivmod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@gdivmod to a number of arguments
      inline
      application generalised_divmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return generalised_divmod()(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@gdivmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol generalised_divmod to a
      ///     number of arguments
      inline
      bool is_generalised_divmod_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_generalised_divmod_function_symbol(application(e).head());
        }
        return false;
      }

      /// \brief Generate identifier \@ggdivmod
      /// \return Identifier \@ggdivmod
      inline
      core::identifier_string const& doubly_generalised_divmod_name()
      {
        static core::identifier_string doubly_generalised_divmod_name = core::detail::initialise_static_expression(doubly_generalised_divmod_name, core::identifier_string("@ggdivmod"));
        return doubly_generalised_divmod_name;
      }

      /// \brief Constructor for function symbol \@ggdivmod
      /// \return Function symbol doubly_generalised_divmod
      inline
      function_symbol const& doubly_generalised_divmod()
      {
        static function_symbol doubly_generalised_divmod = core::detail::initialise_static_expression(doubly_generalised_divmod, function_symbol(doubly_generalised_divmod_name(), make_function_sort(nat(), nat(), sort_pos::pos(), natpair())));
        return doubly_generalised_divmod;
      }


      /// \brief Recogniser for function \@ggdivmod
      /// \param e A data expression
      /// \return true iff e is the function symbol matching \@ggdivmod
      inline
      bool is_doubly_generalised_divmod_function_symbol(const atermpp::aterm_appl& e)
      {
        if (is_function_symbol(e))
        {
          return function_symbol(e) == doubly_generalised_divmod();
        }
        return false;
      }

      /// \brief Application of function symbol \@ggdivmod
      /// \param arg0 A data expression
      /// \param arg1 A data expression
      /// \param arg2 A data expression
      /// \return Application of \@ggdivmod to a number of arguments
      inline
      application doubly_generalised_divmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        return doubly_generalised_divmod()(arg0, arg1, arg2);
      }

      /// \brief Recogniser for application of \@ggdivmod
      /// \param e A data expression
      /// \return true iff e is an application of function symbol doubly_generalised_divmod to a
      ///     number of arguments
      inline
      bool is_doubly_generalised_divmod_application(const atermpp::aterm_appl& e)
      {
        if (is_application(e))
        {
          return is_doubly_generalised_divmod_function_symbol(application(e).head());
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
        result.push_back(succ(nat()));
        result.push_back(pred());
        result.push_back(dub());
        result.push_back(plus(sort_pos::pos(), nat()));
        result.push_back(plus(nat(), sort_pos::pos()));
        result.push_back(plus(nat(), nat()));
        result.push_back(gte_subtract_with_borrow());
        result.push_back(times(nat(), nat()));
        result.push_back(div());
        result.push_back(mod());
        result.push_back(exp(sort_pos::pos(), nat()));
        result.push_back(exp(nat(), nat()));
        result.push_back(even());
        result.push_back(monus());
        result.push_back(swap_zero());
        result.push_back(swap_zero_add());
        result.push_back(swap_zero_min());
        result.push_back(swap_zero_monus());
        result.push_back(first());
        result.push_back(last());
        result.push_back(divmod());
        result.push_back(generalised_divmod());
        result.push_back(doubly_generalised_divmod());
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
        assert(is_maximum_application(e) || is_minimum_application(e) || is_dub_application(e) || is_plus_application(e) || is_times_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_monus_application(e) || is_swap_zero_application(e) || is_divmod_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 1);
      }

      ///\brief Function for projecting out argument
      ///        arg1 from an application
      /// \param e A data expression
      /// \pre arg1 is defined for e
      /// \return The argument of e that corresponds to arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_cpair_application(e) || is_gte_subtract_with_borrow_application(e) || is_swap_zero_add_application(e) || is_swap_zero_min_application(e) || is_swap_zero_monus_application(e) || is_generalised_divmod_application(e) || is_doubly_generalised_divmod_application(e));
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
        assert(is_cpair_application(e) || is_gte_subtract_with_borrow_application(e) || is_swap_zero_add_application(e) || is_swap_zero_min_application(e) || is_swap_zero_monus_application(e) || is_generalised_divmod_application(e) || is_doubly_generalised_divmod_application(e));
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
        assert(is_gte_subtract_with_borrow_application(e) || is_swap_zero_add_application(e) || is_swap_zero_min_application(e) || is_swap_zero_monus_application(e) || is_generalised_divmod_application(e) || is_doubly_generalised_divmod_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 2);
      }

      ///\brief Function for projecting out argument
      ///        arg4 from an application
      /// \param e A data expression
      /// \pre arg4 is defined for e
      /// \return The argument of e that corresponds to arg4
      inline
      data_expression arg4(const data_expression& e)
      {
        assert(is_swap_zero_add_application(e) || is_swap_zero_min_application(e) || is_swap_zero_monus_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 3);
      }

      ///\brief Function for projecting out argument
      ///        arg from an application
      /// \param e A data expression
      /// \pre arg is defined for e
      /// \return The argument of e that corresponds to arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_cnat_application(e) || is_pos2nat_application(e) || is_nat2pos_application(e) || is_succ_application(e) || is_pred_application(e) || is_even_application(e) || is_first_application(e) || is_last_application(e));
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
        assert(is_maximum_application(e) || is_minimum_application(e) || is_dub_application(e) || is_plus_application(e) || is_times_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_monus_application(e) || is_swap_zero_application(e) || is_divmod_application(e));
        return *boost::next(static_cast< application >(e).arguments().begin(), 0);
      }

      /// \brief Give all system defined equations for nat
      /// \return All system defined equations for sort nat
      inline
      data_equation_vector nat_generate_equations_code()
      {
        variable vb("b",sort_bool::bool_());
        variable vc("c",sort_bool::bool_());
        variable vp("p",sort_pos::pos());
        variable vq("q",sort_pos::pos());
        variable vn("n",nat());
        variable vm("m",nat());
        variable vu("u",nat());
        variable vv("v",nat());

        data_equation_vector result;
        result.push_back(data_equation(atermpp::make_vector(vp), equal_to(c0(), cnat(vp)), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vp), equal_to(cnat(vp), c0()), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), equal_to(cnat(vp), cnat(vq)), equal_to(vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vn), less(vn, c0()), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vp), less(c0(), cnat(vp)), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less(cnat(vp), cnat(vq)), less(vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vn), less_equal(c0(), vn), sort_bool::true_()));
        result.push_back(data_equation(atermpp::make_vector(vp), less_equal(cnat(vp), c0()), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), less_equal(cnat(vp), cnat(vq)), less_equal(vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vp), pos2nat(vp), cnat(vp)));
        result.push_back(data_equation(atermpp::make_vector(vp), nat2pos(cnat(vp)), vp));
        result.push_back(data_equation(atermpp::make_vector(vp), maximum(vp, c0()), vp));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), maximum(vp, cnat(vq)), if_(less_equal(vp, vq), vq, vp)));
        result.push_back(data_equation(atermpp::make_vector(vp), maximum(c0(), vp), vp));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), maximum(cnat(vp), vq), if_(less_equal(vp, vq), vq, vp)));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), maximum(vm, vn), if_(less_equal(vm, vn), vn, vm)));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), minimum(vm, vn), if_(less_equal(vm, vn), vm, vn)));
        result.push_back(data_equation(variable_list(), succ(c0()), sort_pos::c1()));
        result.push_back(data_equation(atermpp::make_vector(vp), succ(cnat(vp)), succ(vp)));
        result.push_back(data_equation(variable_list(), pred(sort_pos::c1()), c0()));
        result.push_back(data_equation(atermpp::make_vector(vp), pred(sort_pos::cdub(sort_bool::true_(), vp)), cnat(sort_pos::cdub(sort_bool::false_(), vp))));
        result.push_back(data_equation(atermpp::make_vector(vp), pred(sort_pos::cdub(sort_bool::false_(), vp)), dub(sort_bool::true_(), pred(vp))));
        result.push_back(data_equation(variable_list(), dub(sort_bool::false_(), c0()), c0()));
        result.push_back(data_equation(variable_list(), dub(sort_bool::true_(), c0()), cnat(sort_pos::c1())));
        result.push_back(data_equation(atermpp::make_vector(vb, vp), dub(vb, cnat(vp)), cnat(sort_pos::cdub(vb, vp))));
        result.push_back(data_equation(atermpp::make_vector(vp), plus(vp, c0()), vp));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), plus(vp, cnat(vq)), sort_pos::add_with_carry(sort_bool::false_(), vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vp), plus(c0(), vp), vp));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), plus(cnat(vp), vq), sort_pos::add_with_carry(sort_bool::false_(), vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vn), plus(c0(), vn), vn));
        result.push_back(data_equation(atermpp::make_vector(vn), plus(vn, c0()), vn));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), plus(cnat(vp), cnat(vq)), cnat(sort_pos::add_with_carry(sort_bool::false_(), vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vp), gte_subtract_with_borrow(sort_bool::false_(), vp, sort_pos::c1()), pred(vp)));
        result.push_back(data_equation(atermpp::make_vector(vp), gte_subtract_with_borrow(sort_bool::true_(), vp, sort_pos::c1()), pred(nat2pos(pred(vp)))));
        result.push_back(data_equation(atermpp::make_vector(vb, vc, vp, vq), gte_subtract_with_borrow(vb, sort_pos::cdub(vc, vp), sort_pos::cdub(vc, vq)), dub(vb, gte_subtract_with_borrow(vb, vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vb, vp, vq), gte_subtract_with_borrow(vb, sort_pos::cdub(sort_bool::false_(), vp), sort_pos::cdub(sort_bool::true_(), vq)), dub(sort_bool::not_(vb), gte_subtract_with_borrow(sort_bool::true_(), vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vb, vp, vq), gte_subtract_with_borrow(vb, sort_pos::cdub(sort_bool::true_(), vp), sort_pos::cdub(sort_bool::false_(), vq)), dub(sort_bool::not_(vb), gte_subtract_with_borrow(sort_bool::false_(), vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vn), times(c0(), vn), c0()));
        result.push_back(data_equation(atermpp::make_vector(vn), times(vn, c0()), c0()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), times(cnat(vp), cnat(vq)), cnat(times(vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vp), exp(vp, c0()), sort_pos::c1()));
        result.push_back(data_equation(atermpp::make_vector(vp), exp(vp, cnat(sort_pos::c1())), vp));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), exp(vp, cnat(sort_pos::cdub(sort_bool::false_(), vq))), exp(times(vp, vp), cnat(vq))));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), exp(vp, cnat(sort_pos::cdub(sort_bool::true_(), vq))), times(vp, exp(times(vp, vp), cnat(vq)))));
        result.push_back(data_equation(atermpp::make_vector(vn), exp(vn, c0()), cnat(sort_pos::c1())));
        result.push_back(data_equation(atermpp::make_vector(vp), exp(c0(), cnat(vp)), c0()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), exp(cnat(vp), vn), cnat(exp(vp, vn))));
        result.push_back(data_equation(variable_list(), even(c0()), sort_bool::true_()));
        result.push_back(data_equation(variable_list(), even(cnat(sort_pos::c1())), sort_bool::false_()));
        result.push_back(data_equation(atermpp::make_vector(vb, vp), even(cnat(sort_pos::cdub(vb, vp))), sort_bool::not_(vb)));
        result.push_back(data_equation(atermpp::make_vector(vp), div(c0(), vp), c0()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), div(cnat(vp), vq), first(divmod(vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vp), mod(c0(), vp), c0()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), mod(cnat(vp), vq), last(divmod(vp, vq))));
        result.push_back(data_equation(atermpp::make_vector(vn), monus(c0(), vn), c0()));
        result.push_back(data_equation(atermpp::make_vector(vn), monus(vn, c0()), vn));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), monus(cnat(vp), cnat(vq)), gte_subtract_with_borrow(sort_bool::false_(), vp, vq)));
        result.push_back(data_equation(atermpp::make_vector(vm), swap_zero(vm, c0()), vm));
        result.push_back(data_equation(atermpp::make_vector(vn), swap_zero(c0(), vn), vn));
        result.push_back(data_equation(atermpp::make_vector(vp), swap_zero(cnat(vp), cnat(vp)), c0()));
        result.push_back(data_equation(atermpp::make_vector(vp, vq), not_equal_to(vp, vq), swap_zero(cnat(vp), cnat(vq)), cnat(vq)));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), swap_zero_add(c0(), c0(), vm, vn), plus(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vm, vp), swap_zero_add(cnat(vp), c0(), vm, c0()), vm));
        result.push_back(data_equation(atermpp::make_vector(vm, vp, vq), swap_zero_add(cnat(vp), c0(), vm, cnat(vq)), swap_zero(cnat(vp), plus(swap_zero(cnat(vp), vm), cnat(vq)))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), swap_zero_add(c0(), cnat(vp), c0(), vn), vn));
        result.push_back(data_equation(atermpp::make_vector(vn, vp, vq), swap_zero_add(c0(), cnat(vp), cnat(vq), vn), swap_zero(cnat(vp), plus(cnat(vq), swap_zero(cnat(vp), vn)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vp, vq), swap_zero_add(cnat(vp), cnat(vq), vm, vn), swap_zero(plus(cnat(vp), cnat(vq)), plus(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), swap_zero_min(c0(), c0(), vm, vn), minimum(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vm, vp), swap_zero_min(cnat(vp), c0(), vm, c0()), c0()));
        result.push_back(data_equation(atermpp::make_vector(vm, vp, vq), swap_zero_min(cnat(vp), c0(), vm, cnat(vq)), minimum(swap_zero(cnat(vp), vm), cnat(vq))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), swap_zero_min(c0(), cnat(vp), c0(), vn), c0()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp, vq), swap_zero_min(c0(), cnat(vp), cnat(vq), vn), minimum(cnat(vq), swap_zero(cnat(vp), vn))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vp, vq), swap_zero_min(cnat(vp), cnat(vq), vm, vn), swap_zero(minimum(cnat(vp), cnat(vq)), minimum(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), swap_zero_monus(c0(), c0(), vm, vn), monus(vm, vn)));
        result.push_back(data_equation(atermpp::make_vector(vm, vp), swap_zero_monus(cnat(vp), c0(), vm, c0()), vm));
        result.push_back(data_equation(atermpp::make_vector(vm, vp, vq), swap_zero_monus(cnat(vp), c0(), vm, cnat(vq)), swap_zero(cnat(vp), monus(swap_zero(cnat(vp), vm), cnat(vq)))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), swap_zero_monus(c0(), cnat(vp), c0(), vn), c0()));
        result.push_back(data_equation(atermpp::make_vector(vn, vp, vq), swap_zero_monus(c0(), cnat(vp), cnat(vq), vn), monus(cnat(vq), swap_zero(cnat(vp), vn))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vp, vq), swap_zero_monus(cnat(vp), cnat(vq), vm, vn), swap_zero(monus(cnat(vp), cnat(vq)), monus(swap_zero(cnat(vp), vm), swap_zero(cnat(vq), vn)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vu, vv), equal_to(cpair(vm, vn), cpair(vu, vv)), sort_bool::and_(equal_to(vm, vu), equal_to(vn, vv))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vu, vv), less(cpair(vm, vn), cpair(vu, vv)), sort_bool::or_(less(vm, vu), sort_bool::and_(equal_to(vm, vu), less(vn, vv)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn, vu, vv), less_equal(cpair(vm, vn), cpair(vu, vv)), sort_bool::or_(less(vm, vu), sort_bool::and_(equal_to(vm, vu), less_equal(vn, vv)))));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), first(cpair(vm, vn)), vm));
        result.push_back(data_equation(atermpp::make_vector(vm, vn), last(cpair(vm, vn)), vn));
        result.push_back(data_equation(variable_list(), divmod(sort_pos::c1(), sort_pos::c1()), cpair(cnat(sort_pos::c1()), c0())));
        result.push_back(data_equation(atermpp::make_vector(vb, vp), divmod(sort_pos::c1(), sort_pos::cdub(vb, vp)), cpair(c0(), cnat(sort_pos::c1()))));
        result.push_back(data_equation(atermpp::make_vector(vb, vp, vq), divmod(sort_pos::cdub(vb, vp), vq), generalised_divmod(divmod(vp, vq), vb, vq)));
        result.push_back(data_equation(atermpp::make_vector(vb, vm, vn, vp), generalised_divmod(cpair(vm, vn), vb, vp), doubly_generalised_divmod(dub(vb, vn), vm, vp)));
        result.push_back(data_equation(atermpp::make_vector(vn, vp), doubly_generalised_divmod(c0(), vn, vp), cpair(dub(sort_bool::false_(), vn), c0())));
        result.push_back(data_equation(atermpp::make_vector(vn, vp, vq), less(vp, vq), doubly_generalised_divmod(cnat(vp), vn, vq), cpair(dub(sort_bool::false_(), vn), cnat(vp))));
        result.push_back(data_equation(atermpp::make_vector(vn, vp, vq), less_equal(vq, vp), doubly_generalised_divmod(cnat(vp), vn, vq), cpair(dub(sort_bool::true_(), vn), gte_subtract_with_borrow(sort_bool::false_(), vp, vq))));
        return result;
      }

    } // namespace sort_nat

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_NAT_H
