#ifndef MCRL2_NEW_DATA_REAL__H
#define MCRL2_NEW_DATA_REAL__H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/bool.h"
#include "mcrl2/new_data/pos.h"
#include "mcrl2/new_data/nat.h"
#include "mcrl2/new_data/int.h"


namespace mcrl2 {

  namespace new_data {

    namespace sort_real_ {

      // Sort expression Real
      inline
      basic_sort real_()
      {
        static basic_sort real_("Real");
        return real_;
      }

      // Recogniser for sort expression Real
      inline
      bool is_real_(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == real_();
        }
        return false;
      }

      // Function symbol @cReal
      inline
      function_symbol creal()
      {
        //static function_symbol creal("@cReal", function_sort(sort_int_::int_(), sort_pos::pos(), sort_real_::real_()));
        function_symbol creal("@cReal", function_sort(sort_int_::int_(), sort_pos::pos(), sort_real_::real_()));
        return creal;
      }

      // Recogniser for @cReal
      inline
      bool is_creal_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cReal";
        }
        return false;
      }

      // Application of @cReal
      inline
      application creal(const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
        return application(creal(),arg0, arg1);
      }

      // Recogniser for application of @cReal
      inline
      bool is_creal_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_creal_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Pos2Real
      inline
      function_symbol pos2real()
      {
        //static function_symbol pos2real("Pos2Real", function_sort(sort_pos::pos(), sort_real_::real_()));
        function_symbol pos2real("Pos2Real", function_sort(sort_pos::pos(), sort_real_::real_()));
        return pos2real;
      }

      // Recogniser for Pos2Real
      inline
      bool is_pos2real_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Pos2Real";
        }
        return false;
      }

      // Application of Pos2Real
      inline
      application pos2real(const data_expression& arg0)
      {
        //assert(sort_pos::is_pos(arg0.sort()));
        
        return application(pos2real(),arg0);
      }

      // Recogniser for application of Pos2Real
      inline
      bool is_pos2real_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_pos2real_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Nat2Real
      inline
      function_symbol nat2real()
      {
        //static function_symbol nat2real("Nat2Real", function_sort(sort_nat::nat(), sort_real_::real_()));
        function_symbol nat2real("Nat2Real", function_sort(sort_nat::nat(), sort_real_::real_()));
        return nat2real;
      }

      // Recogniser for Nat2Real
      inline
      bool is_nat2real_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Nat2Real";
        }
        return false;
      }

      // Application of Nat2Real
      inline
      application nat2real(const data_expression& arg0)
      {
        //assert(sort_nat::is_nat(arg0.sort()));
        
        return application(nat2real(),arg0);
      }

      // Recogniser for application of Nat2Real
      inline
      bool is_nat2real_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_nat2real_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Int2Real
      inline
      function_symbol int2real()
      {
        //static function_symbol int2real("Int2Real", function_sort(sort_int_::int_(), sort_real_::real_()));
        function_symbol int2real("Int2Real", function_sort(sort_int_::int_(), sort_real_::real_()));
        return int2real;
      }

      // Recogniser for Int2Real
      inline
      bool is_int2real_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Int2Real";
        }
        return false;
      }

      // Application of Int2Real
      inline
      application int2real(const data_expression& arg0)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        
        return application(int2real(),arg0);
      }

      // Recogniser for application of Int2Real
      inline
      bool is_int2real_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_int2real_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Real2Pos
      inline
      function_symbol real2pos()
      {
        //static function_symbol real2pos("Real2Pos", function_sort(sort_real_::real_(), sort_pos::pos()));
        function_symbol real2pos("Real2Pos", function_sort(sort_real_::real_(), sort_pos::pos()));
        return real2pos;
      }

      // Recogniser for Real2Pos
      inline
      bool is_real2pos_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Real2Pos";
        }
        return false;
      }

      // Application of Real2Pos
      inline
      application real2pos(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(real2pos(),arg0);
      }

      // Recogniser for application of Real2Pos
      inline
      bool is_real2pos_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_real2pos_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Real2Nat
      inline
      function_symbol real2nat()
      {
        //static function_symbol real2nat("Real2Nat", function_sort(sort_real_::real_(), sort_nat::nat()));
        function_symbol real2nat("Real2Nat", function_sort(sort_real_::real_(), sort_nat::nat()));
        return real2nat;
      }

      // Recogniser for Real2Nat
      inline
      bool is_real2nat_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Real2Nat";
        }
        return false;
      }

      // Application of Real2Nat
      inline
      application real2nat(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(real2nat(),arg0);
      }

      // Recogniser for application of Real2Nat
      inline
      bool is_real2nat_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_real2nat_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol Real2Int
      inline
      function_symbol real2int()
      {
        //static function_symbol real2int("Real2Int", function_sort(sort_real_::real_(), sort_int_::int_()));
        function_symbol real2int("Real2Int", function_sort(sort_real_::real_(), sort_int_::int_()));
        return real2int;
      }

      // Recogniser for Real2Int
      inline
      bool is_real2int_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "Real2Int";
        }
        return false;
      }

      // Application of Real2Int
      inline
      application real2int(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(real2int(),arg0);
      }

      // Recogniser for application of Real2Int
      inline
      bool is_real2int_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_real2int_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol min
      inline
      function_symbol minimum(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_real_::real_() && s1 == sort_real_::real_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else
        {
          assert(false);
        }

        //static function_symbol minimum("min", function_sort(s0, s1, target_sort));
        function_symbol minimum("min", function_sort(s0, s1, target_sort));
        return minimum;
      }

      // Recogniser for min
      inline
      bool is_minimum_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "min";
        }
        return false;
      }

      // Application of min
      inline
      application minimum(const data_expression& arg0, const data_expression& arg1)
      {
        
        return application(minimum(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      // Recogniser for application of min
      inline
      bool is_minimum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_minimum_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol max
      inline
      function_symbol maximum(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_real_::real_() && s1 == sort_real_::real_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_pos::pos() && s1 == sort_int_::int_())||
               //(s0 == sort_int_::int_() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_int_::int_())||
               //(s0 == sort_int_::int_() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
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
        else if(s0 == sort_pos::pos() && s1 == sort_int_::int_())
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
        else
        {
          assert(false);
        }

        //static function_symbol maximum("max", function_sort(s0, s1, target_sort));
        function_symbol maximum("max", function_sort(s0, s1, target_sort));
        return maximum;
      }

      // Recogniser for max
      inline
      bool is_maximum_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "max";
        }
        return false;
      }

      // Application of max
      inline
      application maximum(const data_expression& arg0, const data_expression& arg1)
      {
        
        return application(maximum(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      // Recogniser for application of max
      inline
      bool is_maximum_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_maximum_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol abs
      inline
      function_symbol abs(const sort_expression& s0)
      {
        //assert(//(s0 == sort_real_::real_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_())
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
        //assert(//(s0 == sort_real_::real_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else
        {
          assert(false);
        }

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
        //assert(//(s0 == sort_real_::real_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
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
        //assert(//(s0 == sort_real_::real_())||
               //(s0 == sort_pos::pos())||
               //(s0 == sort_nat::nat())||
               //(s0 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
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

      // Function symbol +
      inline
      function_symbol plus(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_real_::real_() && s1 == sort_real_::real_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
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
        else if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
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
        //assert(//(s0 == sort_real_::real_() && s1 == sort_real_::real_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
        }
        else
        {
          assert(false);
        }

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
        //assert(//(s0 == sort_real_::real_() && s1 == sort_real_::real_())||
               //(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_real_::real_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_pos::pos())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_int_::int_())
        {
          target_sort = sort_int_::int_();
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

      // Function symbol exp
      inline
      function_symbol exp(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_real_::real_() && s1 == sort_int_::int_())||
               //(s0 == sort_pos::pos() && s1 == sort_nat::nat())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_nat::nat()));

        sort_expression target_sort;
        if(s0 == sort_real_::real_() && s1 == sort_int_::int_())
        {
          target_sort = sort_real_::real_();
        }
        else if(s0 == sort_pos::pos() && s1 == sort_nat::nat())
        {
          target_sort = sort_pos::pos();
        }
        else if(s0 == sort_nat::nat() && s1 == sort_nat::nat())
        {
          target_sort = sort_nat::nat();
        }
        else if(s0 == sort_int_::int_() && s1 == sort_nat::nat())
        {
          target_sort = sort_int_::int_();
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

      // Function symbol /
      inline
      function_symbol divides(const sort_expression& s0, const sort_expression& s1)
      {
        //assert(//(s0 == sort_pos::pos() && s1 == sort_pos::pos())||
               //(s0 == sort_nat::nat() && s1 == sort_nat::nat())||
               //(s0 == sort_int_::int_() && s1 == sort_int_::int_())||
               //(s0 == sort_real_::real_() && s1 == sort_real_::real_()));

        sort_expression target_sort(sort_real_::real_());
        //static function_symbol divides("/", function_sort(s0, s1, target_sort));
        function_symbol divides("/", function_sort(s0, s1, target_sort));
        return divides;
      }

      // Recogniser for /
      inline
      bool is_divides_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "/";
        }
        return false;
      }

      // Application of /
      inline
      application divides(const data_expression& arg0, const data_expression& arg1)
      {
        
        return application(divides(arg0.sort(), arg1.sort()),arg0, arg1);
      }

      // Recogniser for application of /
      inline
      bool is_divides_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_divides_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol floor
      inline
      function_symbol floor()
      {
        //static function_symbol floor("floor", function_sort(sort_real_::real_(), sort_int_::int_()));
        function_symbol floor("floor", function_sort(sort_real_::real_(), sort_int_::int_()));
        return floor;
      }

      // Recogniser for floor
      inline
      bool is_floor_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "floor";
        }
        return false;
      }

      // Application of floor
      inline
      application floor(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(floor(),arg0);
      }

      // Recogniser for application of floor
      inline
      bool is_floor_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_floor_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol ceil
      inline
      function_symbol ceil()
      {
        //static function_symbol ceil("ceil", function_sort(sort_real_::real_(), sort_int_::int_()));
        function_symbol ceil("ceil", function_sort(sort_real_::real_(), sort_int_::int_()));
        return ceil;
      }

      // Recogniser for ceil
      inline
      bool is_ceil_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "ceil";
        }
        return false;
      }

      // Application of ceil
      inline
      application ceil(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(ceil(),arg0);
      }

      // Recogniser for application of ceil
      inline
      bool is_ceil_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_ceil_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol round
      inline
      function_symbol round()
      {
        //static function_symbol round("round", function_sort(sort_real_::real_(), sort_int_::int_()));
        function_symbol round("round", function_sort(sort_real_::real_(), sort_int_::int_()));
        return round;
      }

      // Recogniser for round
      inline
      bool is_round_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "round";
        }
        return false;
      }

      // Application of round
      inline
      application round(const data_expression& arg0)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        
        return application(round(),arg0);
      }

      // Recogniser for application of round
      inline
      bool is_round_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_round_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @redfrac
      inline
      function_symbol redfrac()
      {
        //static function_symbol redfrac("@redfrac", function_sort(sort_int_::int_(), sort_int_::int_(), sort_real_::real_()));
        function_symbol redfrac("@redfrac", function_sort(sort_int_::int_(), sort_int_::int_(), sort_real_::real_()));
        return redfrac;
      }

      // Recogniser for @redfrac
      inline
      bool is_redfrac_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@redfrac";
        }
        return false;
      }

      // Application of @redfrac
      inline
      application redfrac(const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        //assert(sort_int_::is_int_(arg1.sort()));
        
        return application(redfrac(),arg0, arg1);
      }

      // Recogniser for application of @redfrac
      inline
      bool is_redfrac_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_redfrac_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @redfracwhr
      inline
      function_symbol redfracwhr()
      {
        //static function_symbol redfracwhr("@redfracwhr", function_sort(sort_pos::pos(), sort_int_::int_(), sort_nat::nat(), sort_real_::real_()));
        function_symbol redfracwhr("@redfracwhr", function_sort(sort_pos::pos(), sort_int_::int_(), sort_nat::nat(), sort_real_::real_()));
        return redfracwhr;
      }

      // Recogniser for @redfracwhr
      inline
      bool is_redfracwhr_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@redfracwhr";
        }
        return false;
      }

      // Application of @redfracwhr
      inline
      application redfracwhr(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        //assert(sort_pos::is_pos(arg0.sort()));
        //assert(sort_int_::is_int_(arg1.sort()));
        //assert(sort_nat::is_nat(arg2.sort()));
        
        return application(redfracwhr(),arg0, arg1, arg2);
      }

      // Recogniser for application of @redfracwhr
      inline
      bool is_redfracwhr_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_redfracwhr_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol @redfrachlp
      inline
      function_symbol redfrachlp()
      {
        //static function_symbol redfrachlp("@redfrachlp", function_sort(sort_real_::real_(), sort_int_::int_(), sort_real_::real_()));
        function_symbol redfrachlp("@redfrachlp", function_sort(sort_real_::real_(), sort_int_::int_(), sort_real_::real_()));
        return redfrachlp;
      }

      // Recogniser for @redfrachlp
      inline
      bool is_redfrachlp_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@redfrachlp";
        }
        return false;
      }

      // Application of @redfrachlp
      inline
      application redfrachlp(const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_int_::is_int_(arg1.sort()));
        
        return application(redfrachlp(),arg0, arg1);
      }

      // Recogniser for application of @redfrachlp
      inline
      bool is_redfrachlp_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_redfrachlp_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Give all system defined constructors for Real
      inline
      function_symbol_list real__generate_constructors_code()
      {
        function_symbol_list result;

        return result;
      }

      // Give all system defined constructors for Real
      inline
      function_symbol_list real__generate_functions_code()
      {
        function_symbol_list result;
        result.push_back(creal());
        result.push_back(pos2real());
        result.push_back(nat2real());
        result.push_back(int2real());
        result.push_back(real2pos());
        result.push_back(real2nat());
        result.push_back(real2int());
        result.push_back(minimum(sort_real_::real_(), sort_real_::real_()));
        result.push_back(maximum(sort_real_::real_(), sort_real_::real_()));
        result.push_back(abs(sort_real_::real_()));
        result.push_back(negate(sort_real_::real_()));
        result.push_back(succ(sort_real_::real_()));
        result.push_back(pred(sort_real_::real_()));
        result.push_back(plus(sort_real_::real_(), sort_real_::real_()));
        result.push_back(minus(sort_real_::real_(), sort_real_::real_()));
        result.push_back(times(sort_real_::real_(), sort_real_::real_()));
        result.push_back(exp(sort_real_::real_(), sort_int_::int_()));
        result.push_back(divides(sort_pos::pos(), sort_pos::pos()));
        result.push_back(divides(sort_nat::nat(), sort_nat::nat()));
        result.push_back(divides(sort_int_::int_(), sort_int_::int_()));
        result.push_back(divides(sort_real_::real_(), sort_real_::real_()));
        result.push_back(floor());
        result.push_back(ceil());
        result.push_back(round());
        result.push_back(redfrac());
        result.push_back(redfracwhr());
        result.push_back(redfrachlp());

        return result;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        //assert( || is_minimum_application(e) || is_maximum_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e) || is_divides_application(e) || is_redfrachlp_application(e));
        
        if (is_minimum_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_maximum_application(e))
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

        if (is_exp_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_divides_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_redfrachlp_application(e))
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
        //assert( || is_redfracwhr_application(e));
        
        if (is_redfracwhr_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg1
      inline
      data_expression arg1(const data_expression& e)
      {
        //assert( || is_redfracwhr_application(e));
        
        if (is_redfracwhr_application(e))
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
        //assert( || is_redfracwhr_application(e));
        
        if (is_redfracwhr_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out denominator
      inline
      data_expression denominator(const data_expression& e)
      {
        //assert( || is_creal_application(e));
        
        if (is_creal_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out numerator
      inline
      data_expression numerator(const data_expression& e)
      {
        //assert( || is_creal_application(e));
        
        if (is_creal_application(e))
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
        //assert( || is_pos2real_application(e) || is_nat2real_application(e) || is_int2real_application(e) || is_real2pos_application(e) || is_real2nat_application(e) || is_real2int_application(e) || is_abs_application(e) || is_negate_application(e) || is_succ_application(e) || is_pred_application(e) || is_floor_application(e) || is_ceil_application(e) || is_round_application(e));
        
        if (is_pos2real_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_nat2real_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_int2real_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_real2pos_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_real2nat_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_real2int_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_abs_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_negate_application(e))
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

        if (is_floor_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_ceil_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_round_application(e))
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
        //assert( || is_minimum_application(e) || is_maximum_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e) || is_divides_application(e) || is_redfrachlp_application(e));
        
        if (is_minimum_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_maximum_application(e))
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

        if (is_exp_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_divides_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_redfrachlp_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for Real
      inline
      data_equation_list real__generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), equal_to(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), equal_to(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_())), less(variable("r", sort_real_::real_()), variable("r", sort_real_::real_())), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), less(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_())), less_equal(variable("r", sort_real_::real_()), variable("r", sort_real_::real_())), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), less_equal(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::int2real(variable("x", sort_int_::int_())), sort_real_::creal(variable("x", sort_int_::int_()), sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_real_::nat2real(variable("n", sort_nat::nat())), sort_real_::creal(sort_int_::cint(variable("n", sort_nat::nat())), sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_real_::pos2real(variable("p", sort_pos::pos())), sort_real_::creal(sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2int(sort_real_::creal(variable("x", sort_int_::int_()), sort_pos::c1())), variable("x", sort_int_::int_())));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2nat(sort_real_::creal(variable("x", sort_int_::int_()), sort_pos::c1())), sort_int_::int2nat(variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2pos(sort_real_::creal(variable("x", sort_int_::int_()), sort_pos::c1())), sort_int_::int2pos(variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_real_::minimum(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), if_(less(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), variable("r", sort_real_::real_()), variable("s", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_real_::maximum(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), if_(less(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), variable("s", sort_real_::real_()), variable("r", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_())), sort_real_::abs(variable("r", sort_real_::real_())), if_(less(variable("r", sort_real_::real_()), sort_real_::creal(sort_int_::cint(sort_nat::c0()), sort_pos::c1())), sort_real_::negate(variable("r", sort_real_::real_())), variable("r", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::negate(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()))), sort_real_::creal(sort_real_::negate(variable("x", sort_int_::int_())), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::succ(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()))), sort_real_::creal(sort_real_::plus(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::pred(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()))), sort_real_::creal(sort_real_::minus(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::plus(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), sort_real_::redfrac(sort_real_::plus(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))))), sort_int_::cint(sort_nat::cnat(sort_real_::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::minus(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), sort_real_::redfrac(sort_real_::minus(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))))), sort_int_::cint(sort_nat::cnat(sort_real_::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::times(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), sort_real_::redfrac(sort_real_::times(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_int_::cint(sort_nat::cnat(sort_real_::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), not_equal_to(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::c0())), sort_real_::divides(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::creal(variable("y", sort_int_::int_()), variable("q", sort_pos::pos()))), sort_real_::redfrac(sort_real_::times(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), sort_real_::times(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::divides(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::redfrac(sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("m", sort_nat::nat())), not_equal_to(variable("m", sort_nat::nat()), sort_nat::c0()), sort_real_::divides(variable("m", sort_nat::nat()), variable("n", sort_nat::nat())), sort_real_::redfrac(sort_int_::cint(variable("m", sort_nat::nat())), sort_int_::cint(variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), not_equal_to(variable("y", sort_int_::int_()), sort_int_::cint(sort_nat::c0())), sort_real_::divides(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_real_::redfrac(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::exp(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_int_::cint(variable("n", sort_nat::nat()))), sort_real_::redfrac(sort_real_::exp(variable("x", sort_int_::int_()), variable("n", sort_nat::nat())), sort_int_::cint(sort_nat::cnat(sort_real_::exp(variable("p", sort_pos::pos()), variable("n", sort_nat::nat())))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), not_equal_to(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::c0())), sort_real_::exp(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_int_::cneg(variable("q", sort_pos::pos()))), sort_real_::redfrac(sort_int_::cint(sort_nat::cnat(sort_real_::exp(variable("p", sort_pos::pos()), sort_nat::cnat(variable("q", sort_pos::pos()))))), sort_real_::exp(variable("x", sort_int_::int_()), sort_nat::cnat(variable("q", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::floor(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()))), sort_int_::div(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_())), sort_real_::ceil(variable("r", sort_real_::real_())), sort_real_::negate(sort_real_::floor(sort_real_::negate(variable("r", sort_real_::real_()))))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_())), sort_real_::round(variable("r", sort_real_::real_())), sort_real_::floor(sort_real_::plus(variable("r", sort_real_::real_()), sort_real_::creal(sort_int_::cint(sort_nat::cnat(sort_pos::c1())), sort_pos::cdub(sort_bool_::false_(), sort_pos::c1()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::redfrac(variable("x", sort_int_::int_()), sort_int_::cneg(variable("p", sort_pos::pos()))), sort_real_::redfrac(sort_real_::negate(variable("x", sort_int_::int_())), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("x", sort_int_::int_())), sort_real_::redfrac(variable("x", sort_int_::int_()), sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos())))), sort_real_::redfracwhr(variable("p", sort_pos::pos()), sort_int_::div(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_int_::mod(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::redfracwhr(variable("p", sort_pos::pos()), variable("x", sort_int_::int_()), sort_nat::c0()), sort_real_::creal(variable("x", sort_int_::int_()), sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_real_::redfracwhr(variable("p", sort_pos::pos()), variable("x", sort_int_::int_()), sort_nat::cnat(variable("q", sort_pos::pos()))), sort_real_::redfrachlp(sort_real_::redfrac(sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_int_::cint(sort_nat::cnat(variable("q", sort_pos::pos())))), variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()), variable("p", sort_pos::pos())), sort_real_::redfrachlp(sort_real_::creal(variable("x", sort_int_::int_()), variable("p", sort_pos::pos())), variable("y", sort_int_::int_())), sort_real_::creal(sort_real_::plus(sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))), sort_real_::times(variable("y", sort_int_::int_()), variable("x", sort_int_::int_()))), sort_int_::int2pos(variable("x", sort_int_::int_())))));

        return result;
      }

    } // namespace real_
  } // namespace new_data
} // namespace mcrl2

#endif // MCRL2_NEW_DATA_REAL__H
