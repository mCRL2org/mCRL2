#ifndef MCRL2_DATA_NAT_H
#define MCRL2_DATA_NAT_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool.h"
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
        static function_symbol c0("@c0", sort_nat::nat());
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
        static function_symbol cnat("@cNat", function_sort(sort_pos::pos(), sort_nat::nat()));
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
        assert(sort_pos::is_pos(arg0.sort()));
        
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
        static function_symbol cpair("@cPair", function_sort(sort_nat::nat(), sort_nat::nat(), sort_nat::natpair()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
        static function_symbol pos2nat("Pos2Nat", function_sort(sort_pos::pos(), sort_nat::nat()));
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
        assert(sort_pos::is_pos(arg0.sort()));
        
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
        static function_symbol nat2pos("Nat2Pos", function_sort(sort_nat::nat(), sort_pos::pos()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        
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
        static function_symbol less_equal("<=", function_sort(sort_nat::nat(), sort_nat::nat(), sort_bool_::bool_()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
        static function_symbol less("<", function_sort(sort_nat::nat(), sort_nat::nat(), sort_bool_::bool_()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
        static function_symbol geq(">=", function_sort(sort_nat::nat(), sort_nat::nat(), sort_bool_::bool_()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
        static function_symbol greater(">", function_sort(sort_nat::nat(), sort_nat::nat(), sort_bool_::bool_()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
      function_symbol max(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               (s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               (s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_pos::pos() && s1 == sort_nat::nat());
        {
          target_sort = sort_pos::pos();
        }
        if(s0 == sort_nat::nat() && s1 == sort_pos::pos());
        {
          target_sort = sort_pos::pos();
        }
        if(s0 == sort_nat::nat() && s1 == sort_nat::nat());
        {
          target_sort = sort_nat::nat();
        }

        static function_symbol max("max", function_sort(s0, s1, target_sort));
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
        
        return application(max(arg0.sort(), arg1.sort()),arg0, arg1);
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
        static function_symbol min("min", function_sort(sort_nat::nat(), sort_nat::nat(), sort_nat::nat()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
        static function_symbol abs("abs", function_sort(sort_nat::nat(), sort_nat::nat()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        
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
        static function_symbol succ("succ", function_sort(sort_nat::nat(), sort_pos::pos()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        
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
        static function_symbol pred("pred", function_sort(sort_pos::pos(), sort_nat::nat()));
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
        assert(sort_pos::is_pos(arg0.sort()));
        
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
        static function_symbol dub("@dub", function_sort(sort_bool_::bool_(), sort_nat::nat(), sort_nat::nat()));
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
        assert(sort_bool_::is_bool_(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
      function_symbol plus(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               (s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               (s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_pos::pos() && s1 == sort_nat::nat());
        {
          target_sort = sort_pos::pos();
        }
        if(s0 == sort_nat::nat() && s1 == sort_pos::pos());
        {
          target_sort = sort_pos::pos();
        }
        if(s0 == sort_nat::nat() && s1 == sort_nat::nat());
        {
          target_sort = sort_nat::nat();
        }

        static function_symbol plus("+", function_sort(s0, s1, target_sort));
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
        
        return application(plus(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol gtesubt(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               (s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort(sort_nat::nat());
        static function_symbol gtesubt("@gtesubt", function_sort(s0, s1, target_sort));
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
        
        return application(gtesubt(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol gtesubtb()
      {
        static function_symbol gtesubtb("@gtesubtb", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), sort_nat::nat()));
        return gtesubtb;
      }

      // Recogniser for @gtesubtb
      inline
      bool is_gtesubtb_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gtesubtb";
        }
        return false;
      }

      // Application of @gtesubtb
      inline
      application gtesubtb(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(sort_bool_::is_bool_(arg0.sort()));
        assert(sort_pos::is_pos(arg1.sort()));
        assert(sort_pos::is_pos(arg2.sort()));
        
        return application(gtesubtb(),arg0, arg1, arg2);
      }

      // Recogniser for application of @gtesubtb
      inline
      bool is_gtesubtb_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gtesubtb_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol *
      inline
      function_symbol times()
      {
        static function_symbol times("*", function_sort(sort_nat::nat(), sort_nat::nat(), sort_nat::nat()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        
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
      function_symbol div(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               (s0 == sort_nat::nat() && s1 == sort_pos::pos()));

        sort_expression target_sort(sort_nat::nat());
        static function_symbol div("div", function_sort(s0, s1, target_sort));
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
        
        return application(div(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol mod(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               (s0 == sort_nat::nat() && s1 == sort_pos::pos()));

        sort_expression target_sort(sort_nat::nat());
        static function_symbol mod("|", function_sort(s0, s1, target_sort));
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
        
        return application(mod(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol exp(const sort_expression& s0, const sort_expression& s1)
      {
        assert((s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               (s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_pos::pos() && s1 == sort_nat::nat());
        {
          target_sort = sort_pos::pos();
        }
        if(s0 == sort_nat::nat() && s1 == sort_nat::nat());
        {
          target_sort = sort_nat::nat();
        }

        static function_symbol exp("exp", function_sort(s0, s1, target_sort));
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
        
        return application(exp(arg0.sort(), arg1.sort()),arg0, arg1);
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
        static function_symbol even("@even", function_sort(sort_nat::nat(), sort_bool_::bool_()));
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
        assert(sort_nat::is_nat(arg0.sort()));
        
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
        static function_symbol first("@first", function_sort(sort_nat::natpair(), sort_nat::nat()));
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
        assert(sort_nat::is_natpair(arg0.sort()));
        
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
        static function_symbol last("@last", function_sort(sort_nat::natpair(), sort_nat::nat()));
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
        assert(sort_nat::is_natpair(arg0.sort()));
        
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
        static function_symbol divmod("@divmod", function_sort(sort_pos::pos(), sort_pos::pos(), sort_nat::natpair()));
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
        assert(sort_pos::is_pos(arg0.sort()));
        assert(sort_pos::is_pos(arg1.sort()));
        
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
      function_symbol gdivmod()
      {
        static function_symbol gdivmod("@gdivmod", function_sort(sort_nat::natpair(), sort_bool_::bool_(), sort_pos::pos(), sort_nat::natpair()));
        return gdivmod;
      }

      // Recogniser for @gdivmod
      inline
      bool is_gdivmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@gdivmod";
        }
        return false;
      }

      // Application of @gdivmod
      inline
      application gdivmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(sort_nat::is_natpair(arg0.sort()));
        assert(sort_bool_::is_bool_(arg1.sort()));
        assert(sort_pos::is_pos(arg2.sort()));
        
        return application(gdivmod(),arg0, arg1, arg2);
      }

      // Recogniser for application of @gdivmod
      inline
      bool is_gdivmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_gdivmod_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @ggdivmod
      inline
      function_symbol ggdivmod()
      {
        static function_symbol ggdivmod("@ggdivmod", function_sort(sort_nat::nat(), sort_nat::nat(), sort_pos::pos(), sort_nat::natpair()));
        return ggdivmod;
      }

      // Recogniser for @ggdivmod
      inline
      bool is_ggdivmod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@ggdivmod";
        }
        return false;
      }

      // Application of @ggdivmod
      inline
      application ggdivmod(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        assert(sort_nat::is_nat(arg0.sort()));
        assert(sort_nat::is_nat(arg1.sort()));
        assert(sort_pos::is_pos(arg2.sort()));
        
        return application(ggdivmod(),arg0, arg1, arg2);
      }

      // Recogniser for application of @ggdivmod
      inline
      bool is_ggdivmod_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_ggdivmod_function_symbol(static_cast<const application&>(e).head());
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
        assert(is_gtesubt_application(e) || is_gtesubtb_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_divmod_application(e) || is_gdivmod_application(e) || is_ggdivmod_application(e));
        
        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_gtesubtb_application(e))
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

        if (is_gdivmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_ggdivmod_application(e))
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
        assert(is_gtesubt_application(e) || is_gtesubtb_application(e) || is_div_application(e) || is_mod_application(e) || is_exp_application(e) || is_divmod_application(e) || is_ggdivmod_application(e));
        
        if (is_gtesubt_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_gtesubtb_application(e))
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

        if (is_ggdivmod_application(e))
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
        assert(is_gdivmod_application(e) || is_ggdivmod_application(e));
        
        if (is_gdivmod_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        if (is_ggdivmod_application(e))
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
        assert(is_dub_application(e) || is_gtesubtb_application(e) || is_gdivmod_application(e));
        
        if (is_dub_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_gtesubtb_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_gdivmod_application(e))
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
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_bool_::equal_to(sort_nat::c0(), sort_nat::cnat(variable("p", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_bool_::equal_to(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_bool_::equal_to(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_bool_::equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::pos2nat(), sort_nat::cnat()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::nat2pos(variable("p", sort_pos::pos())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::less_equal(sort_nat::c0(), variable("n", sort_nat::nat())), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::less_equal(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::less_equal(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_pos::less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::less(variable("n", sort_nat::nat()), sort_nat::c0()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::less(sort_nat::c0(), sort_nat::cnat(variable("p", sort_pos::pos()))), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::less(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_pos::less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::geq(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_pos::less_equal(variable("n", sort_nat::nat()), variable("m", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::greater(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_pos::less(variable("n", sort_nat::nat()), variable("m", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::max(variable("p", sort_pos::pos()), sort_nat::c0()), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::max(variable("p", sort_pos::pos()), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_pos::max(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::max(sort_nat::c0(), variable("p", sort_pos::pos())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::max(sort_nat::cnat(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_pos::max(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::max(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::if_(sort_pos::less_equal(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), variable("n", sort_nat::nat()), variable("m", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::min(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::if_(sort_pos::less_equal(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::abs(variable("n", sort_nat::nat())), variable("n", sort_nat::nat())));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_pos::succ(sort_nat::c0()), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::succ(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_pos::succ(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::pred(sort_pos::c1()), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::pred(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos()))), sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::pred(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos()))), sort_nat::dub(sort_bool_::true_(), sort_nat::pred(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::dub(sort_bool_::false_(), sort_nat::c0()), sort_nat::c0()));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::dub(sort_bool_::true_(), sort_nat::c0()), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::dub(variable("b", sort_bool_::bool_()), sort_nat::cnat(variable("p", sort_pos::pos()))), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::plus(variable("p", sort_pos::pos()), sort_nat::c0()), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::plus(variable("p", sort_pos::pos()), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_pos::plus(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::plus(sort_nat::c0(), variable("p", sort_pos::pos())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::plus(sort_nat::cnat(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_pos::plus(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::plus(sort_nat::c0(), variable("n", sort_nat::nat())), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::plus(variable("n", sort_nat::nat()), sort_nat::c0()), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::plus(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::gtesubt(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::gtesubtb(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::gtesubt(variable("n", sort_nat::nat()), sort_nat::c0()), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::gtesubt(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_nat::gtesubt(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::gtesubtb(sort_bool_::false_(), variable("p", sort_pos::pos()), sort_pos::c1()), sort_nat::pred(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::gtesubtb(sort_bool_::true_(), variable("p", sort_pos::pos()), sort_pos::c1()), sort_nat::pred(sort_nat::nat2pos(sort_nat::pred(variable("p", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::gtesubtb(variable("b", sort_bool_::bool_()), sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::dub(sort_bool_::not_(variable("b", sort_bool_::bool_())), sort_nat::gtesubtb(sort_bool_::true_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::gtesubtb(variable("b", sort_bool_::bool_()), sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), sort_nat::dub(sort_bool_::not_(variable("b", sort_bool_::bool_())), sort_nat::gtesubtb(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::times(sort_nat::c0(), variable("n", sort_nat::nat())), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_pos::times(variable("n", sort_nat::nat()), sort_nat::c0()), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_pos::times(sort_nat::cnat(variable("p", sort_pos::pos())), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_nat::cnat(sort_pos::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::exp(variable("p", sort_pos::pos()), sort_nat::c0()), sort_pos::c1()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::exp(variable("p", sort_pos::pos()), sort_nat::cnat(sort_pos::c1())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::exp(variable("p", sort_pos::pos()), sort_nat::cnat(sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos())))), sort_nat::exp(sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), variable("p", sort_pos::pos()), variable("p", sort_pos::pos())), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::exp(variable("p", sort_pos::pos()), sort_nat::cnat(sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos())))), sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), variable("p", sort_pos::pos()), sort_nat::exp(sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), variable("p", sort_pos::pos()), variable("p", sort_pos::pos())), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::exp(variable("n", sort_nat::nat()), sort_nat::c0()), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::exp(sort_nat::cnat(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())), sort_nat::cnat(sort_nat::exp(variable("p", sort_pos::pos()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::even(sort_nat::c0()), sort_bool_::true_()));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::even(sort_nat::cnat(sort_pos::c1())), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::even(sort_nat::cnat(sort_pos::cdub(variable("p", sort_pos::pos()), variable("p", sort_pos::pos())))), sort_bool_::not_(variable("b", sort_bool_::bool_()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::div(variable("p", sort_pos::pos()), sort_pos::c1()), sort_nat::cnat(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::div(sort_pos::c1(), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::div(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), sort_nat::div(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::div(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::greater(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::div(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::first(sort_nat::divmod(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::div(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_bool_::if_(sort_bool_::equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::cnat(sort_pos::c1()), sort_nat::c0())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::greater(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::div(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::first(sort_nat::divmod(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::div(sort_nat::c0(), variable("p", sort_pos::pos())), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::div(sort_nat::cnat(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_nat::div(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::mod(variable("p", sort_pos::pos()), sort_pos::c1()), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::mod(sort_pos::c1(), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))), sort_nat::cnat(sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::mod(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), sort_nat::dub(variable("b", sort_bool_::bool_()), sort_nat::mod(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::mod(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::cnat(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::greater(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::mod(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::last(sort_nat::divmod(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::mod(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_bool_::if_(sort_bool_::equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::c0(), sort_nat::cnat(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::greater(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::mod(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_nat::last(sort_nat::divmod(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::mod(sort_nat::c0(), variable("p", sort_pos::pos())), sort_nat::c0()));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::mod(sort_nat::cnat(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_nat::mod(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()), variable("u", sort_nat::nat()), variable("v", sort_nat::nat())), sort_bool_::true_(), sort_bool_::equal_to(sort_nat::cpair(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_nat::cpair(variable("v", sort_nat::nat()), variable("u", sort_nat::nat()))), sort_bool_::and_(sort_bool_::equal_to(variable("m", sort_nat::nat()), variable("v", sort_nat::nat())), sort_bool_::equal_to(variable("n", sort_nat::nat()), variable("u", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::first(sort_nat::cpair(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))), variable("m", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::last(sort_nat::cpair(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))), variable("n", sort_nat::nat())));
        result.push_back(data_equation(variable_list(), sort_bool_::true_(), sort_nat::divmod(sort_pos::c1(), sort_pos::c1()), sort_nat::cpair(sort_nat::cnat(sort_pos::c1()), sort_nat::c0())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::divmod(sort_pos::c1(), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))), sort_nat::cpair(sort_nat::c0(), sort_nat::cnat(sort_pos::c1()))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_bool_::true_(), sort_nat::divmod(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_nat::gdivmod(sort_nat::divmod(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), variable("b", sort_bool_::bool_()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("m", sort_nat::nat()), variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_bool_::true_(), sort_nat::gdivmod(sort_nat::cpair(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_nat::ggdivmod(sort_nat::dub(variable("b", sort_bool_::bool_()), variable("n", sort_nat::nat())), variable("m", sort_nat::nat()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("n", sort_nat::nat())), sort_bool_::true_(), sort_nat::ggdivmod(sort_nat::c0(), variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_nat::cpair(sort_nat::dub(sort_bool_::false_(), variable("n", sort_nat::nat())), sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("n", sort_nat::nat())), sort_pos::less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::ggdivmod(variable("p", sort_pos::pos()), variable("n", sort_nat::nat()), variable("q", sort_pos::pos())), sort_nat::cpair(sort_nat::dub(sort_bool_::false_(), variable("n", sort_nat::nat())), sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()), variable("n", sort_nat::nat())), sort_pos::geq(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_nat::ggdivmod(sort_nat::cnat(variable("p", sort_pos::pos())), variable("n", sort_nat::nat()), variable("q", sort_pos::pos())), sort_nat::cpair(sort_nat::dub(sort_bool_::true_(), variable("n", sort_nat::nat())), sort_nat::gtesubtb(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));

        return result;
      }

    } // namespace nat
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_NAT_H
