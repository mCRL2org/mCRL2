#ifndef MCRL2_DATA_INT__H
#define MCRL2_DATA_INT__H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
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
        //static function_symbol cint("@cInt", function_sort(sort_nat::nat(), sort_int_::int_()));
        function_symbol cint("@cInt", function_sort(sort_nat::nat(), sort_int_::int_()));
        return cint;
      }

      // Recogniser for @cInt
      inline
      bool is_cint_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cInt";
        }
        return false;
      }

      // Application of @cInt
      inline
      application cint(const data_expression& arg0)
      {
        //assert(sort_nat::is_nat(arg0.sort()));
        
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
        //static function_symbol cneg("@cNeg", function_sort(sort_pos::pos(), sort_int_::int_()));
        function_symbol cneg("@cNeg", function_sort(sort_pos::pos(), sort_int_::int_()));
        return cneg;
      }

      // Recogniser for @cNeg
      inline
      bool is_cneg_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cNeg";
        }
        return false;
      }

      // Application of @cNeg
      inline
      application cneg(const data_expression& arg0)
      {
        //assert(sort_pos::is_pos(arg0.sort()));
        
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
        //static function_symbol nat2int("Nat2Int", function_sort(sort_nat::nat(), sort_int_::int_()));
        function_symbol nat2int("Nat2Int", function_sort(sort_nat::nat(), sort_int_::int_()));
        return nat2int;
      }

      // Recogniser for Nat2Int
      inline
      bool is_nat2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Nat2Int";
        }
        return false;
      }

      // Application of Nat2Int
      inline
      application nat2int(const data_expression& arg0)
      {
        //assert(sort_nat::is_nat(arg0.sort()));
        
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
        //static function_symbol int2nat("Int2Nat", function_sort(sort_int_::int_(), sort_nat::nat()));
        function_symbol int2nat("Int2Nat", function_sort(sort_int_::int_(), sort_nat::nat()));
        return int2nat;
      }

      // Recogniser for Int2Nat
      inline
      bool is_int2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Int2Nat";
        }
        return false;
      }

      // Application of Int2Nat
      inline
      application int2nat(const data_expression& arg0)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        
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
        //static function_symbol pos2int("Pos2Int", function_sort(sort_pos::pos(), sort_int_::int_()));
        function_symbol pos2int("Pos2Int", function_sort(sort_pos::pos(), sort_int_::int_()));
        return pos2int;
      }

      // Recogniser for Pos2Int
      inline
      bool is_pos2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Pos2Int";
        }
        return false;
      }

      // Application of Pos2Int
      inline
      application pos2int(const data_expression& arg0)
      {
        //assert(sort_pos::is_pos(arg0.sort()));
        
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
        //static function_symbol int2pos("Int2Pos", function_sort(sort_int_::int_(), sort_pos::pos()));
        function_symbol int2pos("Int2Pos", function_sort(sort_int_::int_(), sort_pos::pos()));
        return int2pos;
      }

      // Recogniser for Int2Pos
      inline
      bool is_int2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Int2Pos";
        }
        return false;
      }

      // Application of Int2Pos
      inline
      application int2pos(const data_expression& arg0)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        
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
      function_symbol less_equal(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort(sort_bool_::bool_());
        //static function_symbol less_equal("<=", function_sort(s0, s1, target_sort));
        function_symbol less_equal("<=", function_sort(s0, s1, target_sort));
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
        
        return application(less_equal(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol less(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort(sort_bool_::bool_());
        //static function_symbol less("<", function_sort(s0, s1, target_sort));
        function_symbol less("<", function_sort(s0, s1, target_sort));
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
        
        return application(less(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol geq(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort(sort_bool_::bool_());
        //static function_symbol geq(">=", function_sort(s0, s1, target_sort));
        function_symbol geq(">=", function_sort(s0, s1, target_sort));
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
        
        return application(geq(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol greater(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort(sort_bool_::bool_());
        //static function_symbol greater(">", function_sort(s0, s1, target_sort));
        function_symbol greater(">", function_sort(s0, s1, target_sort));
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
        
        return application(greater(arg0.sort(), arg1.sort()),arg0, arg1);
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
        //assert(//(s0 == sort_pos::pos() && s1 == sort_int_::int_())||
               //(s0 == sort_int_::int_() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_int_::int_())||
               //(s0 == sort_int_::int_() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_pos::pos() && s1 == sort_int_::int_())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_int_::int_())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol max("max", function_sort(s0, s1, target_sort));
        function_symbol max("max", function_sort(s0, s1, target_sort));
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
      function_symbol min(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol min("min", function_sort(s0, s1, target_sort));
        function_symbol min("min", function_sort(s0, s1, target_sort));
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
        
        return application(min(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol abs(const sort_expression& s0)
      {
        //assert(//(s0 == sort_int_::int_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol abs("abs", function_sort(s0, target_sort));
        function_symbol abs("abs", function_sort(s0, target_sort));
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
        
        return application(abs(arg0.sort()),arg0);
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
      function_symbol negate(const sort_expression& s0)
      {
        //assert(//(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_()));

        sort_expression target_sort(sort_int_::int_());
        //static function_symbol negate("-", function_sort(s0, target_sort));
        function_symbol negate("-", function_sort(s0, target_sort));
        return negate;
      }

      // Recogniser for -
      inline
      bool is_negate_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "-";
        }
        return false;
      }

      // Application of -
      inline
      application negate(const data_expression& arg0)
      {
        
        return application(negate(arg0.sort()),arg0);
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
      function_symbol succ(const sort_expression& s0)
      {
        //assert(//(s0 == sort_int_::int_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else
        {
          assert(false);
        }

        //static function_symbol succ("succ", function_sort(s0, target_sort));
        function_symbol succ("succ", function_sort(s0, target_sort));
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
        
        return application(succ(arg0.sort()),arg0);
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
      function_symbol pred(const sort_expression& s0)
      {
        //assert(//(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_())||
               //(s0 == sort_pos::pos()));

        sort_expression target_sort;
        if(s0 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol pred("pred", function_sort(s0, target_sort));
        function_symbol pred("pred", function_sort(s0, target_sort));
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
        
        return application(pred(arg0.sort()),arg0);
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
      function_symbol dub(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_bool_::bool_() && s1 == sort_int_::int_())||
               //(s0 == sort_bool_::bool_() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_bool_::bool_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_bool_::bool_() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol dub("@dub", function_sort(s0, s1, target_sort));
        function_symbol dub("@dub", function_sort(s0, s1, target_sort));
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
        
        return application(dub(arg0.sort(), arg1.sort()),arg0, arg1);
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
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol plus("+", function_sort(s0, s1, target_sort));
        function_symbol plus("+", function_sort(s0, s1, target_sort));
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

      // Function symbol -
      inline
      function_symbol minus(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort(sort_int_::int_());
        //static function_symbol minus("-", function_sort(s0, s1, target_sort));
        function_symbol minus("-", function_sort(s0, s1, target_sort));
        return minus;
      }

      // Recogniser for -
      inline
      bool is_minus_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "-";
        }
        return false;
      }

      // Application of -
      inline
      application minus(const data_expression& arg0, const data_expression& arg1)
      {
        
        return application(minus(arg0.sort(), arg1.sort()),arg0, arg1);
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
      function_symbol times(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol times("*", function_sort(s0, s1, target_sort));
        function_symbol times("*", function_sort(s0, s1, target_sort));
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
        
        return application(times(arg0.sort(), arg1.sort()),arg0, arg1);
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
        //assert(//(s0 == sort_int_::int_() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_() && s1 == sort_pos::pos())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol div("div", function_sort(s0, s1, target_sort));
        function_symbol div("div", function_sort(s0, s1, target_sort));
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

      // Function symbol mod
      inline
      function_symbol mod(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_int_::int_() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos()));

        sort_expression target_sort(sort_nat::nat());
        //static function_symbol mod("mod", function_sort(s0, s1, target_sort));
        function_symbol mod("mod", function_sort(s0, s1, target_sort));
        return mod;
      }

      // Recogniser for mod
      inline
      bool is_mod_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "mod";
        }
        return false;
      }

      // Application of mod
      inline
      application mod(const data_expression& arg0, const data_expression& arg1)
      {
        
        return application(mod(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      // Recogniser for application of mod
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
        //assert(//(s0 == sort_int_::int_() && s1 == sort_nat::nat())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_int_::int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else
        {
          assert(false);
        }

        //static function_symbol exp("exp", function_sort(s0, s1, target_sort));
        function_symbol exp("exp", function_sort(s0, s1, target_sort));
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

      // Give all system defined constructors for Int
      inline
      function_symbol_list int__generate_constructors_code()
      {
        function_symbol_list result;
        result.push_back(cint());
        result.push_back(cneg());

        return result;
      }

      // Give all system defined constructors for Int
      inline
      function_symbol_list int__generate_functions_code()
      {
        function_symbol_list result;
        result.push_back(nat2int());
        result.push_back(int2nat());
        result.push_back(pos2int());
        result.push_back(int2pos());
        result.push_back(less_equal(sort_int_::int_(), sort_int_::int_()));
        result.push_back(less(sort_int_::int_(), sort_int_::int_()));
        result.push_back(geq(sort_int_::int_(), sort_int_::int_()));
        result.push_back(greater(sort_int_::int_(), sort_int_::int_()));
        result.push_back(max(sort_pos::pos(), sort_int_::int_()));
        result.push_back(max(sort_int_::int_(), sort_pos::pos()));
        result.push_back(max(sort_nat::nat(), sort_int_::int_()));
        result.push_back(max(sort_int_::int_(), sort_nat::nat()));
        result.push_back(max(sort_int_::int_(), sort_int_::int_()));
        result.push_back(min(sort_int_::int_(), sort_int_::int_()));
        result.push_back(abs(sort_int_::int_()));
        result.push_back(negate(sort_pos::pos()));
        result.push_back(negate(sort_nat::nat()));
        result.push_back(negate(sort_int_::int_()));
        result.push_back(succ(sort_int_::int_()));
        result.push_back(pred(sort_nat::nat()));
        result.push_back(pred(sort_int_::int_()));
        result.push_back(dub(sort_bool_::bool_(), sort_int_::int_()));
        result.push_back(plus(sort_int_::int_(), sort_int_::int_()));
        result.push_back(minus(sort_pos::pos(), sort_pos::pos()));
        result.push_back(minus(sort_nat::nat(), sort_nat::nat()));
        result.push_back(minus(sort_int_::int_(), sort_int_::int_()));
        result.push_back(times(sort_int_::int_(), sort_int_::int_()));
        result.push_back(div(sort_int_::int_(), sort_pos::pos()));
        result.push_back(mod(sort_int_::int_(), sort_pos::pos()));
        result.push_back(exp(sort_int_::int_(), sort_nat::nat()));

        return result;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        //assert( || is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e));
        
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
        //assert( || is_div_application(e) || is_mod_application(e) || is_exp_application(e));
        
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
        //assert( || is_div_application(e) || is_mod_application(e) || is_exp_application(e));
        
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
        //assert( || is_abs_application(e) || is_succ_application(e) || is_pred_application(e));
        
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
        //assert( || is_cint_application(e) || is_cneg_application(e) || is_nat2int_application(e) || is_int2nat_application(e) || is_pos2int_application(e) || is_int2pos_application(e) || is_negate_application(e) || is_dub_application(e));
        
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
        //assert( || is_dub_application(e));
        
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
        //assert( || is_less_equal_application(e) || is_less_application(e) || is_geq_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e));
        
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

      // Give all system defined equations for Int
      inline
      data_equation_list int__generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), equal_to(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat()))), equal_to(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), equal_to(sort_int_::cint(variable("n", sort_nat::nat())), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), equal_to(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), equal_to(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(variable_list(), sort_int_::nat2int(), sort_int_::cint()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::int2nat(sort_int_::cint(variable("n", sort_nat::nat()))), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::pos2int(variable("p", sort_pos::pos())), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::int2pos(sort_int_::cint(variable("n", sort_nat::nat()))), sort_nat::nat2pos(variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::less_equal(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::less_equal(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::less_equal(sort_int_::cint(variable("n", sort_nat::nat())), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::less_equal(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::less_equal(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), sort_int_::less_equal(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::less(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::less(variable("m", sort_nat::nat()), variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::less(sort_int_::cint(variable("n", sort_nat::nat())), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::less(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::less(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), sort_int_::less(variable("q", sort_pos::pos()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::geq(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::less_equal(variable("y", sort_int_::int_()), variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::greater(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::less(variable("y", sort_int_::int_()), variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::max(variable("p", sort_pos::pos()), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::max(variable("p", sort_pos::pos()), variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::max(variable("p", sort_pos::pos()), sort_int_::cneg(variable("q", sort_pos::pos()))), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::max(sort_int_::cint(variable("n", sort_nat::nat())), variable("p", sort_pos::pos())), sort_int_::max(variable("n", sort_nat::nat()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::max(sort_int_::cneg(variable("q", sort_pos::pos())), variable("p", sort_pos::pos())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::max(variable("m", sort_nat::nat()), sort_int_::cint(variable("n", sort_nat::nat()))), if_(sort_int_::less_equal(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), variable("n", sort_nat::nat()), variable("m", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::max(variable("n", sort_nat::nat()), sort_int_::cneg(variable("p", sort_pos::pos()))), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::max(sort_int_::cint(variable("m", sort_nat::nat())), variable("n", sort_nat::nat())), if_(sort_int_::less_equal(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), variable("n", sort_nat::nat()), variable("m", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::max(sort_int_::cneg(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::max(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), if_(sort_int_::less_equal(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), variable("y", sort_int_::int_()), variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::min(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), if_(sort_int_::less_equal(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), variable("x", sort_int_::int_()), variable("y", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::abs(sort_int_::cint(variable("n", sort_nat::nat()))), variable("n", sort_nat::nat())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::abs(sort_int_::cneg(variable("p", sort_pos::pos()))), sort_nat::cnat(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::negate(variable("p", sort_pos::pos())), sort_int_::cneg(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(variable_list(), sort_int_::negate(sort_nat::c0()), sort_int_::cint(sort_nat::c0())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::negate(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_int_::cneg(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::negate(sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::negate(variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::negate(sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::succ(sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::cint(sort_nat::cnat(sort_int_::succ(variable("n", sort_nat::nat()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::succ(sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::negate(sort_int_::pred(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(variable_list(), sort_int_::pred(sort_nat::c0()), sort_int_::cneg(sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::pred(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_int_::cint(sort_int_::pred(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_int_::pred(sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::pred(variable("n", sort_nat::nat()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::pred(sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::cneg(sort_int_::succ(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("b", sort_bool_::bool_())), sort_int_::dub(variable("b", sort_bool_::bool_()), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::cint(sort_int_::dub(variable("b", sort_bool_::bool_()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::dub(sort_bool_::false_(), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::cneg(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_int_::dub(sort_bool_::true_(), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::negate(sort_int_::dub(sort_bool_::true_(), sort_int_::pred(variable("p", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::plus(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::cint(sort_int_::plus(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::plus(sort_int_::cint(variable("n", sort_nat::nat())), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::minus(variable("n", sort_nat::nat()), sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::plus(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::minus(variable("n", sort_nat::nat()), sort_nat::cnat(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::plus(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), sort_int_::cneg(sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::less_equal(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_int_::minus(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::cint(sort_nat::gtesubtb(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::minus(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::negate(sort_nat::gtesubtb(sort_bool_::false_(), variable("q", sort_pos::pos()), variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::less_equal(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::minus(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_int_::cint(sort_nat::gtesubt(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::less(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_int_::minus(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_int_::negate(sort_nat::gtesubt(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::minus(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::plus(variable("x", sort_int_::int_()), sort_int_::negate(variable("y", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::times(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::cint(sort_int_::times(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::times(sort_int_::cint(variable("n", sort_nat::nat())), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_int_::negate(sort_int_::times(sort_nat::cnat(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::times(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_int_::negate(sort_int_::times(sort_nat::cnat(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::times(sort_int_::cneg(variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), sort_int_::cint(sort_nat::cnat(sort_int_::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::div(sort_int_::cint(variable("n", sort_nat::nat())), variable("p", sort_pos::pos())), sort_int_::cint(sort_int_::div(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::div(sort_int_::cneg(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_int_::cneg(sort_int_::succ(sort_int_::div(sort_int_::pred(variable("p", sort_pos::pos())), variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_int_::mod(sort_int_::cint(variable("n", sort_nat::nat())), variable("p", sort_pos::pos())), sort_int_::mod(variable("n", sort_nat::nat()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_int_::mod(sort_int_::cneg(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())), sort_int_::int2nat(sort_int_::minus(variable("q", sort_pos::pos()), sort_int_::succ(sort_int_::mod(sort_int_::pred(variable("p", sort_pos::pos())), variable("q", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), sort_int_::exp(sort_int_::cint(variable("m", sort_nat::nat())), variable("n", sort_nat::nat())), sort_int_::cint(sort_int_::exp(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_nat::even(variable("n", sort_nat::nat())), sort_int_::exp(sort_int_::cneg(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())), sort_int_::cint(sort_nat::cnat(sort_int_::exp(variable("p", sort_pos::pos()), variable("n", sort_nat::nat()))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("p", sort_pos::pos())), sort_bool_::not_(sort_nat::even(variable("n", sort_nat::nat()))), sort_int_::exp(sort_int_::cneg(variable("p", sort_pos::pos())), variable("n", sort_nat::nat())), sort_int_::cneg(sort_int_::exp(variable("p", sort_pos::pos()), variable("n", sort_nat::nat())))));

        return result;
      }

    } // namespace int_
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_INT__H
