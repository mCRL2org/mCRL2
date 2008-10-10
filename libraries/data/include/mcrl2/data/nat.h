#ifndef MCRL2_DATA_NAT_H
#define MCRL2_DATA_NAT_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool_.h"
#include "mcrl2/data/pos.h"


namespace mcrl2 {

  namespace data {

    namespace sort_nat {

      // Sort expression Nat
      inline
      basic_sort nat()
      {
        static basic_sort nat("Nat");
        return nat;
      }

      // Recogniser for sort expression Nat
      inline
      bool is_nat(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == nat();
        }
        return false;
      }

      // Sort expression @NatPair
      inline
      basic_sort natpair()
      {
        static basic_sort natpair("@NatPair");
        return natpair;
      }

      // Recogniser for sort expression @NatPair
      inline
      bool is_natpair(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == natpair();
        }
        return false;
      }

      // Function symbol @c0
      inline
      function_symbol c0()
      {
        static function_symbol c0("@c0", nat());
        return c0;
      }

      // Recogniser for @c0
      inline
      bool is_c0_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@c0";
        }
        return false;
      }

      // Function symbol @cNat
      inline
      function_symbol cnat()
      {
        static function_symbol cnat("@cNat", function_sort(pos(), nat()));
        return cnat;
      }

      // Recogniser for @cNat
      inline
      bool is_cnat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cNat";
        }
        return false;
      }

      // Application of @cNat
      inline
      application cnat(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(cnat(),arg0);
      }

      // Recogniser for application of @cNat
      inline
      bool is_cnat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cnat_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @cPair
      inline
      function_symbol cpair()
      {
        static function_symbol cpair("@cPair", function_sort(nat(), nat(), natpair()));
        return cpair;
      }

      // Recogniser for @cPair
      inline
      bool is_cpair_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cPair";
        }
        return false;
      }

      // Application of @cPair
      inline
      application cpair(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(cpair(),arg0, arg1);
      }

      // Recogniser for application of @cPair
      inline
      bool is_cpair_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cpair_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Pos2Nat
      inline
      function_symbol pos2nat()
      {
        static function_symbol pos2nat("Pos2Nat", function_sort(pos(), nat()));
        return pos2nat;
      }

      // Recogniser for Pos2Nat
      inline
      bool is_pos2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Pos2Nat";
        }
        return false;
      }

      // Application of Pos2Nat
      inline
      application pos2nat(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(pos2nat(),arg0);
      }

      // Recogniser for application of Pos2Nat
      inline
      bool is_pos2nat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pos2nat_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Nat2Pos
      inline
      function_symbol nat2pos()
      {
        static function_symbol nat2pos("Nat2Pos", function_sort(nat(), pos()));
        return nat2pos;
      }

      // Recogniser for Nat2Pos
      inline
      bool is_nat2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Nat2Pos";
        }
        return false;
      }

      // Application of Nat2Pos
      inline
      application nat2pos(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(nat2pos(),arg0);
      }

      // Recogniser for application of Nat2Pos
      inline
      bool is_nat2pos_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_nat2pos_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <=
      inline
      function_symbol less_equal()
      {
        static function_symbol less_equal("<=", function_sort(nat(), nat(), bool_()));
        return less_equal;
      }

      // Recogniser for <=
      inline
      bool is_less_equal_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "<=";
        }
        return false;
      }

      // Application of <=
      inline
      application less_equal(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(less_equal(),arg0, arg1);
      }

      // Recogniser for application of <=
      inline
      bool is_less_equal_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_less_equal_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <
      inline
      function_symbol less()
      {
        static function_symbol less("<", function_sort(nat(), nat(), bool_()));
        return less;
      }

      // Recogniser for <
      inline
      bool is_less_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "<";
        }
        return false;
      }

      // Application of <
      inline
      application less(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(less(),arg0, arg1);
      }

      // Recogniser for application of <
      inline
      bool is_less_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_less_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol >=
      inline
      function_symbol geq()
      {
        static function_symbol geq(">=", function_sort(nat(), nat(), bool_()));
        return geq;
      }

      // Recogniser for >=
      inline
      bool is_geq_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == ">=";
        }
        return false;
      }

      // Application of >=
      inline
      application geq(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(geq(),arg0, arg1);
      }

      // Recogniser for application of >=
      inline
      bool is_geq_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_geq_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol >
      inline
      function_symbol greater()
      {
        static function_symbol greater(">", function_sort(nat(), nat(), bool_()));
        return greater;
      }

      // Recogniser for >
      inline
      bool is_greater_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == ">";
        }
        return false;
      }

      // Application of >
      inline
      application greater(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(greater(),arg0, arg1);
      }

      // Recogniser for application of >
      inline
      bool is_greater_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_greater_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol max
      inline
      function_symbol max()
      {
        static function_symbol max("max", function_sort(pos(), nat(), pos()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "max";
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(max(),arg0, arg1);
      }

      // Recogniser for application of max
      inline
      bool is_max_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_max_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol max
      inline
      function_symbol max()
      {
        static function_symbol max("max", function_sort(nat(), pos(), pos()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "max";
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(max(),arg0, arg1);
      }

      // Recogniser for application of max
      inline
      bool is_max_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_max_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol max
      inline
      function_symbol max()
      {
        static function_symbol max("max", function_sort(nat(), nat(), nat()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "max";
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(max(),arg0, arg1);
      }

      // Recogniser for application of max
      inline
      bool is_max_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_max_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol min
      inline
      function_symbol min()
      {
        static function_symbol min("min", function_sort(nat(), nat(), nat()));
        return min;
      }

      // Recogniser for min
      inline
      bool is_min_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "min";
        }
        return false;
      }

      // Application of min
      inline
      application min(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(min(),arg0, arg1);
      }

      // Recogniser for application of min
      inline
      bool is_min_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_min_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol abs
      inline
      function_symbol abs()
      {
        static function_symbol abs("abs", function_sort(nat(), nat()));
        return abs;
      }

      // Recogniser for abs
      inline
      bool is_abs_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "abs";
        }
        return false;
      }

      // Application of abs
      inline
      application abs(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(abs(),arg0);
      }

      // Recogniser for application of abs
      inline
      bool is_abs_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_abs_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol succ
      inline
      function_symbol succ()
      {
        static function_symbol succ("succ", function_sort(nat(), pos()));
        return succ;
      }

      // Recogniser for succ
      inline
      bool is_succ_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "succ";
        }
        return false;
      }

      // Application of succ
      inline
      application succ(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(succ(),arg0);
      }

      // Recogniser for application of succ
      inline
      bool is_succ_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_succ_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol pred
      inline
      function_symbol pred()
      {
        static function_symbol pred("pred", function_sort(pos(), nat()));
        return pred;
      }

      // Recogniser for pred
      inline
      bool is_pred_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "pred";
        }
        return false;
      }

      // Application of pred
      inline
      application pred(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(pred(),arg0);
      }

      // Recogniser for application of pred
      inline
      bool is_pred_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pred_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @dub
      inline
      function_symbol dub()
      {
        static function_symbol dub("@dub", function_sort(bool_(), nat(), nat()));
        return dub;
      }

      // Recogniser for @dub
      inline
      bool is_dub_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@dub";
        }
        return false;
      }

      // Application of @dub
      inline
      application dub(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(dub(),arg0, arg1);
      }

      // Recogniser for application of @dub
      inline
      bool is_dub_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_dub_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol +
      inline
      function_symbol plus()
      {
        static function_symbol plus("+", function_sort(pos(), nat(), pos()));
        return plus;
      }

      // Recogniser for +
      inline
      bool is_plus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "+";
        }
        return false;
      }

      // Application of +
      inline
      application plus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(plus(),arg0, arg1);
      }

      // Recogniser for application of +
      inline
      bool is_plus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_plus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol +
      inline
      function_symbol plus()
      {
        static function_symbol plus("+", function_sort(nat(), pos(), pos()));
        return plus;
      }

      // Recogniser for +
      inline
      bool is_plus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "+";
        }
        return false;
      }

      // Application of +
      inline
      application plus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(plus(),arg0, arg1);
      }

      // Recogniser for application of +
      inline
      bool is_plus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_plus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol +
      inline
      function_symbol plus()
      {
        static function_symbol plus("+", function_sort(nat(), nat(), nat()));
        return plus;
      }

      // Recogniser for +
      inline
      bool is_plus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "+";
        }
        return false;
      }

      // Application of +
      inline
      application plus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(plus(),arg0, arg1);
      }

      // Recogniser for application of +
      inline
      bool is_plus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_plus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @gtesubt
      inline
      function_symbol gtesubt()
      {
        static function_symbol gtesubt("@gtesubt", function_sort(pos(), pos(), nat()));
        return gtesubt;
      }

      // Recogniser for @gtesubt
      inline
      bool is_gtesubt_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gtesubt";
        }
        return false;
      }

      // Application of @gtesubt
      inline
      application gtesubt(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(gtesubt(),arg0, arg1);
      }

      // Recogniser for application of @gtesubt
      inline
      bool is_gtesubt_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubt_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @gtesubt
      inline
      function_symbol gtesubt()
      {
        static function_symbol gtesubt("@gtesubt", function_sort(nat(), nat(), nat()));
        return gtesubt;
      }

      // Recogniser for @gtesubt
      inline
      bool is_gtesubt_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gtesubt";
        }
        return false;
      }

      // Application of @gtesubt
      inline
      application gtesubt(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(gtesubt(),arg0, arg1);
      }

      // Recogniser for application of @gtesubt
      inline
      bool is_gtesubt_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubt_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @gtesubtb
      inline
      function_symbol gtesubt()
      {
        static function_symbol gtesubt("@gtesubtb", function_sort(bool_(), pos(), pos(), nat()));
        return gtesubt;
      }

      // Recogniser for @gtesubtb
      inline
      bool is_gtesubt_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gtesubtb";
        }
        return false;
      }

      // Application of @gtesubtb
      inline
      application gtesubt(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_pos(arg1.sort()));
        assert(is_pos(arg2.sort()));
        
        return application(gtesubt(),arg0, arg1, arg2);
      }

      // Recogniser for application of @gtesubtb
      inline
      bool is_gtesubt_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubt_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol *
      inline
      function_symbol times()
      {
        static function_symbol times("*", function_sort(nat(), nat(), nat()));
        return times;
      }

      // Recogniser for *
      inline
      bool is_times_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "*";
        }
        return false;
      }

      // Application of *
      inline
      application times(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(times(),arg0, arg1);
      }

      // Recogniser for application of *
      inline
      bool is_times_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_times_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol div
      inline
      function_symbol div()
      {
        static function_symbol div("div", function_sort(pos(), pos(), nat()));
        return div;
      }

      // Recogniser for div
      inline
      bool is_div_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "div";
        }
        return false;
      }

      // Application of div
      inline
      application div(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(div(),arg0, arg1);
      }

      // Recogniser for application of div
      inline
      bool is_div_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_div_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol div
      inline
      function_symbol div()
      {
        static function_symbol div("div", function_sort(nat(), pos(), nat()));
        return div;
      }

      // Recogniser for div
      inline
      bool is_div_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "div";
        }
        return false;
      }

      // Application of div
      inline
      application div(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(div(),arg0, arg1);
      }

      // Recogniser for application of div
      inline
      bool is_div_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_div_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol |
      inline
      function_symbol mod()
      {
        static function_symbol mod("|", function_sort(pos(), pos(), nat()));
        return mod;
      }

      // Recogniser for |
      inline
      bool is_mod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "|";
        }
        return false;
      }

      // Application of |
      inline
      application mod(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(mod(),arg0, arg1);
      }

      // Recogniser for application of |
      inline
      bool is_mod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_mod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol |
      inline
      function_symbol mod()
      {
        static function_symbol mod("|", function_sort(nat(), pos(), nat()));
        return mod;
      }

      // Recogniser for |
      inline
      bool is_mod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "|";
        }
        return false;
      }

      // Application of |
      inline
      application mod(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(mod(),arg0, arg1);
      }

      // Recogniser for application of |
      inline
      bool is_mod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_mod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol exp
      inline
      function_symbol exp()
      {
        static function_symbol exp("exp", function_sort(pos(), nat(), pos()));
        return exp;
      }

      // Recogniser for exp
      inline
      bool is_exp_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "exp";
        }
        return false;
      }

      // Application of exp
      inline
      application exp(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(exp(),arg0, arg1);
      }

      // Recogniser for application of exp
      inline
      bool is_exp_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_exp_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol exp
      inline
      function_symbol exp()
      {
        static function_symbol exp("exp", function_sort(nat(), nat(), nat()));
        return exp;
      }

      // Recogniser for exp
      inline
      bool is_exp_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "exp";
        }
        return false;
      }

      // Application of exp
      inline
      application exp(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(exp(),arg0, arg1);
      }

      // Recogniser for application of exp
      inline
      bool is_exp_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_exp_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @even
      inline
      function_symbol even()
      {
        static function_symbol even("@even", function_sort(nat(), bool_()));
        return even;
      }

      // Recogniser for @even
      inline
      bool is_even_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@even";
        }
        return false;
      }

      // Application of @even
      inline
      application even(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(even(),arg0);
      }

      // Recogniser for application of @even
      inline
      bool is_even_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_even_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @first
      inline
      function_symbol first()
      {
        static function_symbol first("@first", function_sort(natpair(), nat()));
        return first;
      }

      // Recogniser for @first
      inline
      bool is_first_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@first";
        }
        return false;
      }

      // Application of @first
      inline
      application first(const data_expression& arg0)
      {
        assert(is_natpair(arg0.sort()));
        
        return application(first(),arg0);
      }

      // Recogniser for application of @first
      inline
      bool is_first_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_first_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @last
      inline
      function_symbol last()
      {
        static function_symbol last("@last", function_sort(natpair(), nat()));
        return last;
      }

      // Recogniser for @last
      inline
      bool is_last_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@last";
        }
        return false;
      }

      // Application of @last
      inline
      application last(const data_expression& arg0)
      {
        assert(is_natpair(arg0.sort()));
        
        return application(last(),arg0);
      }

      // Recogniser for application of @last
      inline
      bool is_last_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_last_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @divmod
      inline
      function_symbol divmod()
      {
        static function_symbol divmod("@divmod", function_sort(pos(), pos(), natpair()));
        return divmod;
      }

      // Recogniser for @divmod
      inline
      bool is_divmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@divmod";
        }
        return false;
      }

      // Application of @divmod
      inline
      application divmod(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(divmod(),arg0, arg1);
      }

      // Recogniser for application of @divmod
      inline
      bool is_divmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_divmod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @gdivmod
      inline
      function_symbol divmod()
      {
        static function_symbol divmod("@gdivmod", function_sort(natpair(), bool_(), pos(), natpair()));
        return divmod;
      }

      // Recogniser for @gdivmod
      inline
      bool is_divmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gdivmod";
        }
        return false;
      }

      // Application of @gdivmod
      inline
      application divmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(is_natpair(arg0.sort()));
        assert(is_bool_(arg1.sort()));
        assert(is_pos(arg2.sort()));
        
        return application(divmod(),arg0, arg1, arg2);
      }

      // Recogniser for application of @gdivmod
      inline
      bool is_divmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_divmod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @ggdivmod
      inline
      function_symbol divmod()
      {
        static function_symbol divmod("@ggdivmod", function_sort(nat(), nat(), pos(), natpair()));
        return divmod;
      }

      // Recogniser for @ggdivmod
      inline
      bool is_divmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@ggdivmod";
        }
        return false;
      }

      // Application of @ggdivmod
      inline
      application divmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        assert(is_pos(arg2.sort()));
        
        return application(divmod(),arg0, arg1, arg2);
      }

      // Recogniser for application of @ggdivmod
      inline
      bool is_divmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_divmod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_times_application(e));
        
        if (is_less_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_less_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_geq_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_greater_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_max_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_min_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_plus_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_times_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out proj2
      inline
      data_expression proj2(const data_expression& e)
      {
        assert(is_cpair_application(e));
        
        if (is_cpair_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out proj1
      inline
      data_expression proj1(const data_expression& e)
      {
        assert(is_cpair_application(e));
        
        if (is_cpair_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_gtesubt_application(e) || is_gtesubt_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_divmod_application(e));
        
        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_div_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_mod_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_exp_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_divmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_gtesubt_application(e) || is_gtesubt_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_divmod_application(e));
        
        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        if (is_div_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_mod_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_exp_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_divmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        assert(is_divmod_application(e));
        
        if (is_divmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out number
      inline
      data_expression number(const data_expression& e)
      {
        assert(is_abs_application(e) || is_succ_application(e) || is_pred_application(e));
        
        if (is_abs_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_succ_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_pred_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg
      inline
      data_expression arg(const data_expression& e)
      {
        assert(is_cnat_application(e) || is_pos2nat_application(e) || is_nat2pos_application(e) || is_dub_application(e) || is_even_application(e));
        
        if (is_cnat_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_pos2nat_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_nat2pos_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_dub_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_even_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out pair
      inline
      data_expression pair(const data_expression& e)
      {
        assert(is_first_application(e) || is_last_application(e));
        
        if (is_first_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_last_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out bit
      inline
      data_expression bit(const data_expression& e)
      {
        assert(is_dub_application(e) || is_gtesubt_application(e) || is_divmod_application(e));
        
        if (is_dub_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_divmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_times_application(e));
        
        if (is_less_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_less_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_geq_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_greater_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_max_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_min_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_plus_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_times_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for Nat
      inline
      data_equation_list nat_generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), equal_to(c0(), cnat(variable("p", pos()))), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), equal_to(cnat(variable("p", pos())), c0()), false_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), equal_to(cnat(variable("p", pos())), cnat(variable("q", pos()))), equal_to(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(variable_list(), true_(), pos2nat(), cnat()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), nat2pos(variable("p", pos())), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), less_equal(c0(), variable("n", nat())), true_()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), less_equal(cnat(variable("p", pos())), c0()), false_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), less_equal(cnat(variable("p", pos())), cnat(variable("q", pos()))), less_equal(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), less(variable("n", nat()), c0()), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), less(c0(), cnat(variable("p", pos()))), true_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), less(cnat(variable("p", pos())), cnat(variable("q", pos()))), less(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), geq(variable("m", nat()), variable("n", nat())), less_equal(variable("n", nat()), variable("m", nat()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), greater(variable("m", nat()), variable("n", nat())), less(variable("n", nat()), variable("m", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), max(variable("p", pos()), c0()), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), max(variable("p", pos()), cnat(variable("q", pos()))), max(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), max(c0(), variable("p", pos())), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), max(cnat(variable("p", pos())), variable("q", pos())), max(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), max(variable("m", nat()), variable("n", nat())), if_(less_equal(variable("m", nat()), variable("n", nat())), variable("n", nat()), variable("m", nat()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), min(variable("m", nat()), variable("n", nat())), if_(less_equal(variable("m", nat()), variable("n", nat())), variable("m", nat()), variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), abs(variable("n", nat())), variable("n", nat())));
        result.push_back(data_equation(variable_list(), true_(), succ(c0()), cnat(c1())));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), succ(cnat(variable("p", pos()))), succ(variable("p", pos()))));
        result.push_back(data_equation(variable_list(), true_(), pred(c1()), c0()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pred(cdub(true_(), variable("p", pos()))), cdub(false_(), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pred(cdub(false_(), variable("p", pos()))), dub(true_(), pred(variable("p", pos())))));
        result.push_back(data_equation(variable_list(), true_(), dub(false_(), c0()), c0()));
        result.push_back(data_equation(variable_list(), true_(), dub(true_(), c0()), cnat(c1())));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("b", bool_())), true_(), dub(variable("b", bool_()), cnat(variable("p", pos()))), cdub(variable("b", bool_()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), plus(variable("p", pos()), c0()), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), plus(variable("p", pos()), cnat(variable("q", pos()))), plus(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), plus(c0(), variable("p", pos())), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), plus(cnat(variable("p", pos())), variable("q", pos())), plus(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), plus(c0(), variable("n", nat())), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), plus(variable("n", nat()), c0()), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), plus(cnat(variable("p", pos())), cnat(variable("q", pos()))), add_with_carry(false_(), variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), gtesubt(variable("p", pos()), variable("q", pos())), gtesubt(false_(), variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), gtesubt(variable("n", nat()), c0()), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), gtesubt(cnat(variable("p", pos())), cnat(variable("q", pos()))), gtesubt(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), gtesubt(false_(), variable("p", pos()), c1()), pred(variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), gtesubt(true_(), variable("p", pos()), c1()), pred(nat2pos(pred(variable("p", pos()))))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("b", bool_())), true_(), gtesubt(variable("b", bool_()), cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), dub(not_(variable("b", bool_())), gtesubt(true_(), variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("b", bool_())), true_(), gtesubt(variable("b", bool_()), cdub(true_(), variable("p", pos())), cdub(false_(), variable("q", pos()))), dub(not_(variable("b", bool_())), gtesubt(false_(), variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), times(c0(), variable("n", nat())), c0()));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), times(variable("n", nat()), c0()), c0()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), times(cnat(variable("p", pos())), cnat(variable("q", pos()))), cnat(times(variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), exp(variable("p", pos()), c0()), c1()));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), exp(variable("p", pos()), cnat(c1())), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), exp(variable("p", pos()), cnat(cdub(false_(), variable("q", pos())))), exp(multir(false_(), c1(), variable("p", pos()), variable("p", pos())), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), exp(variable("p", pos()), cnat(cdub(true_(), variable("q", pos())))), multir(false_(), c1(), variable("p", pos()), exp(multir(false_(), c1(), variable("p", pos()), variable("p", pos())), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), exp(variable("n", nat()), c0()), cnat(c1())));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), exp(cnat(variable("p", pos())), variable("n", nat())), cnat(exp(variable("p", pos()), variable("n", nat())))));
        result.push_back(data_equation(variable_list(), true_(), even(c0()), true_()));
        result.push_back(data_equation(variable_list(), true_(), even(cnat(c1())), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("b", bool_())), true_(), even(cnat(cdub(variable("p", pos()), variable("p", pos())))), not_(variable("b", bool_()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), div(variable("p", pos()), c1()), cnat(variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("b", bool_())), true_(), div(c1(), cdub(variable("b", bool_()), variable("p", pos()))), c0()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("b", bool_())), true_(), div(cdub(variable("b", bool_()), variable("p", pos())), cdub(false_(), variable("q", pos()))), div(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less_equal(variable("p", pos()), variable("q", pos())), div(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), c0()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), greater(variable("p", pos()), variable("q", pos())), div(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), first(divmod(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less_equal(variable("p", pos()), variable("q", pos())), div(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), if_(equal_to(variable("p", pos()), variable("q", pos())), cnat(c1()), c0())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), greater(variable("p", pos()), variable("q", pos())), div(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), first(divmod(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), div(c0(), variable("p", pos())), c0()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), div(cnat(variable("p", pos())), variable("q", pos())), div(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), mod(variable("p", pos()), c1()), c0()));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("b", bool_())), true_(), mod(c1(), cdub(variable("b", bool_()), variable("p", pos()))), cnat(c1())));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("b", bool_())), true_(), mod(cdub(variable("b", bool_()), variable("p", pos())), cdub(false_(), variable("q", pos()))), dub(variable("b", bool_()), mod(variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less_equal(variable("p", pos()), variable("q", pos())), mod(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), cnat(cdub(false_(), variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), greater(variable("p", pos()), variable("q", pos())), mod(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), last(divmod(cdub(false_(), variable("p", pos())), cdub(true_(), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less_equal(variable("p", pos()), variable("q", pos())), mod(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), if_(equal_to(variable("p", pos()), variable("q", pos())), c0(), cnat(cdub(true_(), variable("p", pos()))))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), greater(variable("p", pos()), variable("q", pos())), mod(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))), last(divmod(cdub(true_(), variable("p", pos())), cdub(true_(), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), mod(c0(), variable("p", pos())), c0()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), mod(cnat(variable("p", pos())), variable("q", pos())), mod(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat()), variable("u", nat()), variable("v", nat())), true_(), equal_to(cpair(variable("m", nat()), variable("n", nat())), cpair(variable("v", nat()), variable("u", nat()))), and_(equal_to(variable("m", nat()), variable("v", nat())), equal_to(variable("n", nat()), variable("u", nat())))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), first(cpair(variable("m", nat()), variable("n", nat()))), variable("m", nat())));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), last(cpair(variable("m", nat()), variable("n", nat()))), variable("n", nat())));
        result.push_back(data_equation(variable_list(), true_(), divmod(c1(), c1()), cpair(cnat(c1()), c0())));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("b", bool_())), true_(), divmod(c1(), cdub(variable("b", bool_()), variable("p", pos()))), cpair(c0(), cnat(c1()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("b", bool_())), true_(), divmod(cdub(variable("b", bool_()), variable("p", pos())), variable("q", pos())), divmod(divmod(variable("p", pos()), variable("q", pos())), variable("b", bool_()), variable("q", pos()))));
        result.push_back(data_equation(make_vector(variable("b", bool_()), variable("m", nat()), variable("n", nat()), variable("p", pos())), true_(), divmod(cpair(variable("m", nat()), variable("n", nat())), variable("b", bool_()), variable("p", pos())), divmod(dub(variable("b", bool_()), variable("n", nat())), variable("m", nat()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), divmod(c0(), variable("n", nat()), variable("p", pos())), cpair(dub(false_(), variable("n", nat())), cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("n", nat())), less(variable("p", pos()), variable("q", pos())), divmod(variable("p", pos()), variable("n", nat()), variable("q", pos())), cpair(dub(false_(), variable("n", nat())), cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos()), variable("n", nat())), geq(variable("p", pos()), variable("q", pos())), divmod(cnat(variable("p", pos())), variable("n", nat()), variable("q", pos())), cpair(dub(true_(), variable("n", nat())), gtesubt(false_(), variable("p", pos()), variable("q", pos())))));

        return result;
      }

    } // namespace nat
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_NAT_H
