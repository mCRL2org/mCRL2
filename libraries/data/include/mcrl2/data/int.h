#ifndef MCRL2_DATA_INT__H
#define MCRL2_DATA_INT__H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool_.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"


namespace mcrl2 {

  namespace data {

    namespace sort_int_ {

      // Sort expression Int
      inline
      basic_sort int_()
      {
        static basic_sort int_("Int");
        return int_;
      }

      // Recogniser for sort expression Int
      inline
      bool is_int_(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == int_();
        }
        return false;
      }

      // Function symbol @cInt
      inline
      function_symbol cint()
      {
        static function_symbol cint("@cInt", function_sort(nat(), int_()));
        return cint;
      }

      // Recogniser for @cInt
      inline
      bool is_cint_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == cint();
        }
        return false;
      }

      // Application of @cInt
      inline
      application cint(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(cint(),arg0);
      }

      // Recogniser for application of @cInt
      inline
      bool is_cint_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cint_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @cNeg
      inline
      function_symbol cneg()
      {
        static function_symbol cneg("@cNeg", function_sort(pos(), int_()));
        return cneg;
      }

      // Recogniser for @cNeg
      inline
      bool is_cneg_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == cneg();
        }
        return false;
      }

      // Application of @cNeg
      inline
      application cneg(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(cneg(),arg0);
      }

      // Recogniser for application of @cNeg
      inline
      bool is_cneg_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cneg_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Nat2Int
      inline
      function_symbol nat2int()
      {
        static function_symbol nat2int("Nat2Int", function_sort(nat(), int_()));
        return nat2int;
      }

      // Recogniser for Nat2Int
      inline
      bool is_nat2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == nat2int();
        }
        return false;
      }

      // Application of Nat2Int
      inline
      application nat2int(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(nat2int(),arg0);
      }

      // Recogniser for application of Nat2Int
      inline
      bool is_nat2int_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_nat2int_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Int2Nat
      inline
      function_symbol int2nat()
      {
        static function_symbol int2nat("Int2Nat", function_sort(int_(), nat()));
        return int2nat;
      }

      // Recogniser for Int2Nat
      inline
      bool is_int2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == int2nat();
        }
        return false;
      }

      // Application of Int2Nat
      inline
      application int2nat(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
        return application(int2nat(),arg0);
      }

      // Recogniser for application of Int2Nat
      inline
      bool is_int2nat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_int2nat_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Pos2Int
      inline
      function_symbol pos2int()
      {
        static function_symbol pos2int("Pos2Int", function_sort(pos(), int_()));
        return pos2int;
      }

      // Recogniser for Pos2Int
      inline
      bool is_pos2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == pos2int();
        }
        return false;
      }

      // Application of Pos2Int
      inline
      application pos2int(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(pos2int(),arg0);
      }

      // Recogniser for application of Pos2Int
      inline
      bool is_pos2int_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pos2int_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Int2Pos
      inline
      function_symbol int2pos()
      {
        static function_symbol int2pos("Int2Pos", function_sort(int_(), pos()));
        return int2pos;
      }

      // Recogniser for Int2Pos
      inline
      bool is_int2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == int2pos();
        }
        return false;
      }

      // Application of Int2Pos
      inline
      application int2pos(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
        return application(int2pos(),arg0);
      }

      // Recogniser for application of Int2Pos
      inline
      bool is_int2pos_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_int2pos_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <=
      inline
      function_symbol less_equal()
      {
        static function_symbol less_equal("<=", function_sort(int_(), int_(), bool_()));
        return less_equal;
      }

      // Recogniser for <=
      inline
      bool is_less_equal_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == less_equal();
        }
        return false;
      }

      // Application of <=
      inline
      application less_equal(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol less("<", function_sort(int_(), int_(), bool_()));
        return less;
      }

      // Recogniser for <
      inline
      bool is_less_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == less();
        }
        return false;
      }

      // Application of <
      inline
      application less(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol geq(">=", function_sort(int_(), int_(), bool_()));
        return geq;
      }

      // Recogniser for >=
      inline
      bool is_geq_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == geq();
        }
        return false;
      }

      // Application of >=
      inline
      application geq(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol greater(">", function_sort(int_(), int_(), bool_()));
        return greater;
      }

      // Recogniser for >
      inline
      bool is_greater_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == greater();
        }
        return false;
      }

      // Application of >
      inline
      application greater(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol max("max", function_sort(pos(), int_(), pos()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == max();
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol max("max", function_sort(int_(), pos(), pos()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == max();
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
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
        static function_symbol max("max", function_sort(nat(), int_(), nat()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == max();
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol max("max", function_sort(int_(), nat(), nat()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == max();
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
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
        static function_symbol max("max", function_sort(int_(), int_(), int_()));
        return max;
      }

      // Recogniser for max
      inline
      bool is_max_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == max();
        }
        return false;
      }

      // Application of max
      inline
      application max(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol min("min", function_sort(int_(), int_(), int_()));
        return min;
      }

      // Recogniser for min
      inline
      bool is_min_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == min();
        }
        return false;
      }

      // Application of min
      inline
      application min(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol abs("abs", function_sort(int_(), nat()));
        return abs;
      }

      // Recogniser for abs
      inline
      bool is_abs_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == abs();
        }
        return false;
      }

      // Application of abs
      inline
      application abs(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
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

      // Function symbol -
      inline
      function_symbol negate()
      {
        static function_symbol negate("-", function_sort(pos(), int_()));
        return negate;
      }

      // Recogniser for -
      inline
      bool is_negate_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == negate();
        }
        return false;
      }

      // Application of -
      inline
      application negate(const data_expression& arg0)
      {
        assert(is_pos(arg0.sort()));
        
        return application(negate(),arg0);
      }

      // Recogniser for application of -
      inline
      bool is_negate_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_negate_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol -
      inline
      function_symbol negate()
      {
        static function_symbol negate("-", function_sort(nat(), int_()));
        return negate;
      }

      // Recogniser for -
      inline
      bool is_negate_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == negate();
        }
        return false;
      }

      // Application of -
      inline
      application negate(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
        return application(negate(),arg0);
      }

      // Recogniser for application of -
      inline
      bool is_negate_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_negate_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol -
      inline
      function_symbol negate()
      {
        static function_symbol negate("-", function_sort(int_(), int_()));
        return negate;
      }

      // Recogniser for -
      inline
      bool is_negate_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == negate();
        }
        return false;
      }

      // Application of -
      inline
      application negate(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
        return application(negate(),arg0);
      }

      // Recogniser for application of -
      inline
      bool is_negate_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_negate_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol succ
      inline
      function_symbol succ()
      {
        static function_symbol succ("succ", function_sort(int_(), int_()));
        return succ;
      }

      // Recogniser for succ
      inline
      bool is_succ_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == succ();
        }
        return false;
      }

      // Application of succ
      inline
      application succ(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
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
        static function_symbol pred("pred", function_sort(nat(), int_()));
        return pred;
      }

      // Recogniser for pred
      inline
      bool is_pred_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == pred();
        }
        return false;
      }

      // Application of pred
      inline
      application pred(const data_expression& arg0)
      {
        assert(is_nat(arg0.sort()));
        
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

      // Function symbol pred
      inline
      function_symbol pred()
      {
        static function_symbol pred("pred", function_sort(int_(), int_()));
        return pred;
      }

      // Recogniser for pred
      inline
      bool is_pred_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == pred();
        }
        return false;
      }

      // Application of pred
      inline
      application pred(const data_expression& arg0)
      {
        assert(is_int_(arg0.sort()));
        
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
        static function_symbol dub("@dub", function_sort(bool_(), int_(), int_()));
        return dub;
      }

      // Recogniser for @dub
      inline
      bool is_dub_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == dub();
        }
        return false;
      }

      // Application of @dub
      inline
      application dub(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_bool_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol plus("+", function_sort(int_(), int_(), int_()));
        return plus;
      }

      // Recogniser for +
      inline
      bool is_plus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == plus();
        }
        return false;
      }

      // Application of +
      inline
      application plus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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

      // Function symbol -
      inline
      function_symbol minus()
      {
        static function_symbol minus("-", function_sort(pos(), pos(), int_()));
        return minus;
      }

      // Recogniser for -
      inline
      bool is_minus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == minus();
        }
        return false;
      }

      // Application of -
      inline
      application minus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_pos(arg0.sort()));
        assert(is_pos(arg1.sort()));
        
        return application(minus(),arg0, arg1);
      }

      // Recogniser for application of -
      inline
      bool is_minus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol -
      inline
      function_symbol minus()
      {
        static function_symbol minus("-", function_sort(nat(), nat(), int_()));
        return minus;
      }

      // Recogniser for -
      inline
      bool is_minus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == minus();
        }
        return false;
      }

      // Application of -
      inline
      application minus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_nat(arg0.sort()));
        assert(is_nat(arg1.sort()));
        
        return application(minus(),arg0, arg1);
      }

      // Recogniser for application of -
      inline
      bool is_minus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol -
      inline
      function_symbol minus()
      {
        static function_symbol minus("-", function_sort(int_(), int_(), int_()));
        return minus;
      }

      // Recogniser for -
      inline
      bool is_minus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == minus();
        }
        return false;
      }

      // Application of -
      inline
      application minus(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
        return application(minus(),arg0, arg1);
      }

      // Recogniser for application of -
      inline
      bool is_minus_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minus_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol *
      inline
      function_symbol times()
      {
        static function_symbol times("*", function_sort(int_(), int_(), int_()));
        return times;
      }

      // Recogniser for *
      inline
      bool is_times_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == times();
        }
        return false;
      }

      // Application of *
      inline
      application times(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
        assert(is_int_(arg1.sort()));
        
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
        static function_symbol div("div", function_sort(int_(), pos(), int_()));
        return div;
      }

      // Recogniser for div
      inline
      bool is_div_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == div();
        }
        return false;
      }

      // Application of div
      inline
      application div(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
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
        static function_symbol mod("|", function_sort(int_(), pos(), int_()));
        return mod;
      }

      // Recogniser for |
      inline
      bool is_mod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == mod();
        }
        return false;
      }

      // Application of |
      inline
      application mod(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
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
        static function_symbol exp("exp", function_sort(int_(), nat(), int_()));
        return exp;
      }

      // Recogniser for exp
      inline
      bool is_exp_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e) == exp();
        }
        return false;
      }

      // Application of exp
      inline
      application exp(const data_expression& arg0, const data_expression& arg1)
      {
        assert(is_int_(arg0.sort()));
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

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        assert(is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e));
        
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

        if (is_minus_application(e))
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

      // Function for projecting out arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        assert(is_div_application(e) || is_mod_application(e) || is_exp_application(e));
        
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

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        assert(is_div_application(e) || is_mod_application(e) || is_exp_application(e));
        
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
        assert(is_cint_application(e) || is_cneg_application(e) || is_nat2int_application(e) || is_int2nat_application(e) || is_pos2int_application(e) || is_int2pos_application(e) || is_negate_application(e) || is_dub_application(e));
        
        if (is_cint_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_cneg_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_nat2int_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_int2nat_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_pos2int_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_int2pos_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_negate_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_dub_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out bit
      inline
      data_expression bit(const data_expression& e)
      {
        assert(is_dub_application(e));
        
        if (is_dub_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out left
      inline
      data_expression left(const data_expression& e)
      {
        assert(is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e));
        
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

        if (is_minus_application(e))
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

      // Give all system defined equations for int_
      inline
      data_equation_list int__generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), equal_to(cint(variable("m", nat())), cint(variable("n", nat()))), equal_to(variable("m", nat()), variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), equal_to(variable("n", nat()), cneg(variable("p", pos()))), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), equal_to(cneg(variable("p", pos())), variable("n", nat())), false_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), equal_to(cneg(variable("p", pos())), cneg(variable("q", pos()))), equal_to(variable("p", pos()), variable("q", pos()))));
        result.push_back(data_equation(variable_list(), true_(), nat2int(), cint()));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), int2nat(cint(variable("n", nat()))), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pos2int(variable("p", pos())), cint(cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), int2pos(cint(variable("n", nat()))), nat2pos(variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), less_equal(cint(variable("m", nat())), cint(variable("n", nat()))), less_equal(variable("m", nat()), variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), less_equal(cint(variable("n", nat())), cneg(variable("p", pos()))), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), less_equal(cneg(variable("p", pos())), cint(variable("n", nat()))), true_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), less_equal(cneg(variable("p", pos())), cneg(variable("q", pos()))), less_equal(variable("q", pos()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), less(cint(variable("m", nat())), cint(variable("n", nat()))), less(variable("m", nat()), variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), less(cint(variable("n", nat())), cneg(variable("p", pos()))), false_()));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), less(cneg(variable("p", pos())), cint(variable("n", nat()))), true_()));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), less(cneg(variable("p", pos())), cneg(variable("q", pos()))), less(variable("q", pos()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), geq(variable("x", int_()), variable("y", int_())), less_equal(variable("y", int_()), variable("x", int_()))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), greater(variable("x", int_()), variable("y", int_())), less(variable("y", int_()), variable("x", int_()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), max(variable("p", pos()), cint(variable("n", nat()))), max(variable("p", pos()), variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), max(variable("p", pos()), cneg(variable("q", pos()))), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), max(cint(variable("n", nat())), variable("p", pos())), max(variable("n", nat()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), max(cneg(variable("q", pos())), variable("p", pos())), variable("p", pos())));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), max(variable("m", nat()), cint(variable("n", nat()))), if_(less_equal(variable("m", nat()), variable("n", nat())), variable("n", nat()), variable("m", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), max(variable("n", nat()), cneg(variable("p", pos()))), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), max(cint(variable("m", nat())), variable("n", nat())), if_(less_equal(variable("m", nat()), variable("n", nat())), variable("n", nat()), variable("m", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), max(cneg(variable("p", pos())), variable("n", nat())), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), max(variable("x", int_()), variable("y", int_())), if_(less_equal(variable("x", int_()), variable("y", int_())), variable("y", int_()), variable("x", int_()))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), min(variable("x", int_()), variable("y", int_())), if_(less_equal(variable("x", int_()), variable("y", int_())), variable("x", int_()), variable("y", int_()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), abs(cint(variable("n", nat()))), variable("n", nat())));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), abs(cneg(variable("p", pos()))), cnat(variable("p", pos()))));
        result.push_back(data_equation(variable_list(), true_(), negate(c0()), cint(c0())));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), negate(cnat(variable("p", pos()))), cneg(variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), negate(cint(variable("n", nat()))), negate(variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), negate(cneg(variable("p", pos()))), cint(cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), succ(cint(variable("n", nat()))), cint(cnat(succ(variable("n", nat()))))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), succ(cneg(variable("p", pos()))), negate(pred(variable("p", pos())))));
        result.push_back(data_equation(variable_list(), true_(), pred(c0()), negate(c1())));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pred(cnat(variable("p", pos()))), cint(pred(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), pred(cint(variable("n", nat()))), pred(variable("n", nat()))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pred(cneg(variable("p", pos()))), cneg(succ(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("b", bool_()), variable("n", nat())), true_(), dub(variable("b", bool_()), cint(variable("n", nat()))), cint(dub(variable("b", bool_()), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), dub(false_(), cneg(variable("p", pos()))), cneg(cdub(false_(), variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), dub(true_(), cneg(variable("p", pos()))), negate(dub(true_(), pred(variable("p", pos()))))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), plus(cint(variable("m", nat())), cint(variable("n", nat()))), cint(plus(variable("m", nat()), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), plus(cint(variable("n", nat())), cneg(variable("p", pos()))), minus(variable("n", nat()), cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), plus(cneg(variable("p", pos())), cint(variable("n", nat()))), minus(variable("n", nat()), cnat(variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), plus(cneg(variable("p", pos())), cneg(variable("q", pos()))), cneg(add_with_carry(false_(), variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less_equal(variable("q", pos()), variable("p", pos())), minus(variable("p", pos()), variable("q", pos())), cint(gtesubt(false_(), variable("p", pos()), variable("q", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), less(variable("p", pos()), variable("q", pos())), minus(variable("p", pos()), variable("q", pos())), negate(gtesubt(false_(), variable("q", pos()), variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), less_equal(variable("n", nat()), variable("m", nat())), minus(variable("m", nat()), variable("n", nat())), cint(gtesubt(variable("m", nat()), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), less(variable("m", nat()), variable("n", nat())), minus(variable("m", nat()), variable("n", nat())), negate(gtesubt(variable("n", nat()), variable("m", nat())))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), minus(variable("x", int_()), variable("y", int_())), plus(variable("x", int_()), negate(variable("y", int_())))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), times(cint(variable("m", nat())), cint(variable("n", nat()))), cint(times(variable("m", nat()), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), times(cint(variable("n", nat())), cneg(variable("p", pos()))), cneg(times(cnat(variable("p", pos())), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), times(cneg(variable("p", pos())), cint(variable("n", nat()))), cneg(times(cnat(variable("p", pos())), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), times(cneg(variable("p", pos())), cneg(variable("q", pos()))), cint(cnat(times(variable("p", pos()), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), div(cint(variable("n", nat())), variable("p", pos())), cint(div(variable("n", nat()), variable("p", pos())))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), div(cneg(variable("p", pos())), variable("q", pos())), cneg(succ(div(pred(variable("p", pos())), variable("q", pos()))))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), true_(), mod(cint(variable("n", nat())), variable("p", pos())), mod(variable("n", nat()), variable("p", pos()))));
        result.push_back(data_equation(make_vector(variable("q", pos()), variable("p", pos())), true_(), mod(cneg(variable("p", pos())), variable("q", pos())), int2nat(minus(variable("q", pos()), succ(mod(pred(variable("p", pos())), variable("q", pos())))))));
        result.push_back(data_equation(make_vector(variable("m", nat()), variable("n", nat())), true_(), exp(cint(variable("m", nat())), variable("n", nat())), cint(exp(variable("m", nat()), variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), even(variable("n", nat())), exp(cneg(variable("p", pos())), variable("n", nat())), cint(cnat(exp(variable("p", pos()), variable("n", nat()))))));
        result.push_back(data_equation(make_vector(variable("p", pos()), variable("n", nat())), not_(even(variable("n", nat()))), exp(cneg(variable("p", pos())), variable("n", nat())), cneg(exp(variable("p", pos()), variable("n", nat())))));

        return result;
      }

    } // namespace int_
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_INT__H
