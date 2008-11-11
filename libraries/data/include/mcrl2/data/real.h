#ifndef MCRL2_DATA_REAL__H
#define MCRL2_DATA_REAL__H

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
#include "mcrl2/data/int.h"


namespace mcrl2 {

  namespace data {

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
        //static function_symbol creal("@cReal", function_sort(sort_int_::int_(), sort_real_::real_()));
        function_symbol creal("@cReal", function_sort(sort_int_::int_(), sort_real_::real_()));
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
      application creal(const data_expression& arg0)
      {
        //assert(sort_int_::is_int_(arg0.sort()));
        
        return application(creal(),arg0);
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

      // Function symbol <=
      inline
      function_symbol less_equal()
      {
        //static function_symbol less_equal("<=", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol less_equal("<=", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol less("<", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol less("<", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
      function_symbol greater_equal()
      {
        //static function_symbol greater_equal(">=", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol greater_equal(">=", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        return greater_equal;
      }

      // Recogniser for >=
      inline
      bool is_greater_equal_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == ">=";
        }
        return false;
      }

      // Application of >=
      inline
      application greater_equal(const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
        return application(greater_equal(),arg0, arg1);
      }

      // Recogniser for application of >=
      inline
      bool is_greater_equal_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_greater_equal_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol >
      inline
      function_symbol greater()
      {
        //static function_symbol greater(">", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol greater(">", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol max("max", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol max("max", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol min("min", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol min("min", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol abs("abs", function_sort(sort_real_::real_(), sort_real_::real_()));
        function_symbol abs("abs", function_sort(sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        
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
        //static function_symbol negate("-", function_sort(sort_real_::real_(), sort_real_::real_()));
        function_symbol negate("-", function_sort(sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        
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
        //static function_symbol succ("succ", function_sort(sort_real_::real_(), sort_real_::real_()));
        function_symbol succ("succ", function_sort(sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        
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
        //static function_symbol pred("pred", function_sort(sort_real_::real_(), sort_real_::real_()));
        function_symbol pred("pred", function_sort(sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        
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

      // Function symbol +
      inline
      function_symbol plus()
      {
        //static function_symbol plus("+", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol plus("+", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol minus("-", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol minus("-", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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
        //static function_symbol times("*", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
        function_symbol times("*", function_sort(sort_real_::real_(), sort_real_::real_(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_real_::is_real_(arg1.sort()));
        
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

      // Function symbol exp
      inline
      function_symbol exp()
      {
        //static function_symbol exp("exp", function_sort(sort_real_::real_(), sort_nat::nat(), sort_real_::real_()));
        function_symbol exp("exp", function_sort(sort_real_::real_(), sort_nat::nat(), sort_real_::real_()));
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
        //assert(sort_real_::is_real_(arg0.sort()));
        //assert(sort_nat::is_nat(arg1.sort()));
        
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
        result.push_back(less_equal());
        result.push_back(less());
        result.push_back(greater_equal());
        result.push_back(greater());
        result.push_back(max());
        result.push_back(min());
        result.push_back(abs());
        result.push_back(negate());
        result.push_back(succ());
        result.push_back(pred());
        result.push_back(plus());
        result.push_back(minus());
        result.push_back(times());
        result.push_back(exp());

        return result;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        //assert( || is_less_equal_application(e) || is_less_application(e) || is_greater_equal_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e));
        
        if (is_less_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_less_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_greater_equal_application(e))
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

        if (is_exp_application(e))
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
        //assert( || is_less_equal_application(e) || is_less_application(e) || is_greater_equal_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e));
        
        if (is_less_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_less_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_greater_equal_application(e))
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

        if (is_exp_application(e))
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
        //assert( || is_creal_application(e) || is_pos2real_application(e) || is_nat2real_application(e) || is_int2real_application(e) || is_real2pos_application(e) || is_real2nat_application(e) || is_real2int_application(e) || is_abs_application(e) || is_negate_application(e) || is_succ_application(e) || is_pred_application(e));
        
        if (is_creal_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

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

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for Real
      inline
      data_equation_list real__generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), equal_to(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), equal_to(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()))));
        result.push_back(data_equation(variable_list(), sort_real_::int2real(), sort_real_::creal()));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat())), sort_real_::nat2real(variable("n", sort_nat::nat())), sort_real_::creal(sort_int_::cint(variable("n", sort_nat::nat())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_real_::pos2real(variable("p", sort_pos::pos())), sort_real_::creal(sort_int_::cint(sort_nat::cnat(variable("p", sort_pos::pos()))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2int(sort_real_::creal(variable("x", sort_int_::int_()))), variable("x", sort_int_::int_())));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2nat(sort_real_::creal(variable("x", sort_int_::int_()))), sort_int_::int2nat(variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::real2pos(sort_real_::creal(variable("x", sort_int_::int_()))), sort_int_::int2pos(variable("x", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_pos::less_equal(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), sort_pos::less_equal(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_pos::less(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), sort_pos::less(variable("x", sort_int_::int_()), variable("y", sort_int_::int_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_real_::greater_equal(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_pos::less_equal(variable("s", sort_real_::real_()), variable("r", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_pos::greater(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_pos::less(variable("s", sort_real_::real_()), variable("r", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_pos::max(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), if_(sort_pos::less_equal(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), variable("s", sort_real_::real_()), variable("r", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), sort_pos::min(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), if_(sort_pos::less_equal(variable("r", sort_real_::real_()), variable("s", sort_real_::real_())), variable("r", sort_real_::real_()), variable("s", sort_real_::real_()))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_pos::abs(sort_real_::creal(variable("x", sort_int_::int_()))), sort_real_::creal(sort_int_::cint(sort_pos::abs(variable("x", sort_int_::int_()))))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_real_::negate(sort_real_::creal(variable("x", sort_int_::int_()))), sort_real_::creal(sort_real_::negate(variable("x", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_pos::succ(sort_real_::creal(variable("x", sort_int_::int_()))), sort_real_::creal(sort_pos::succ(variable("x", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_())), sort_nat::pred(sort_real_::creal(variable("x", sort_int_::int_()))), sort_real_::creal(sort_nat::pred(variable("x", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_pos::plus(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), sort_real_::creal(sort_pos::plus(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_real_::minus(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), sort_real_::creal(sort_real_::minus(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())), sort_pos::times(sort_real_::creal(variable("x", sort_int_::int_())), sort_real_::creal(variable("y", sort_int_::int_()))), sort_real_::creal(sort_pos::times(variable("x", sort_int_::int_()), variable("y", sort_int_::int_())))));
        result.push_back(data_equation(make_vector(variable("n", sort_nat::nat()), variable("x", sort_int_::int_())), sort_nat::exp(sort_real_::creal(variable("x", sort_int_::int_())), variable("n", sort_nat::nat())), sort_real_::creal(sort_nat::exp(variable("x", sort_int_::int_()), variable("n", sort_nat::nat())))));

        return result;
      }

    } // namespace real_
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_REAL__H
