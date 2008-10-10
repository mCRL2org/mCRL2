#ifndef MCRL2_DATA_REAL__H
#define MCRL2_DATA_REAL__H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/bool_.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/int_.h"


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
        static function_symbol creal("@cReal", function_sort(int_(), real_()));
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
        assert(is_int_(arg0.sort()));
        
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
        static function_symbol pos2real("Pos2Real", function_sort(pos(), real_()));
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
        assert(is_pos(arg0.sort()));
        
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
        static function_symbol nat2real("Nat2Real", function_sort(nat(), real_()));
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
        assert(is_nat(arg0.sort()));
        
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
        static function_symbol int2real("Int2Real", function_sort(int_(), real_()));
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
        assert(is_int_(arg0.sort()));
        
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
        static function_symbol real2pos("Real2Pos", function_sort(real_(), pos()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol real2nat("Real2Nat", function_sort(real_(), nat()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol real2int("Real2Int", function_sort(real_(), int_()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol less_equal("<=", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol less("<", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol greater_equal(">=", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol greater(">", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol max("max", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol min("min", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol abs("abs", function_sort(real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol negate("-", function_sort(real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol succ("succ", function_sort(real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol pred("pred", function_sort(real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        
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
        static function_symbol plus("+", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol minus("-", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol times("*", function_sort(real_(), real_(), real_()));
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
        assert(is_real_(arg0.sort()));
        assert(is_real_(arg1.sort()));
        
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
        static function_symbol exp("exp", function_sort(real_(), nat(), real_()));
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
        assert(is_real_(arg0.sort()));
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
        assert(is_less_equal_application(e) || is_less_application(e) || is_greater_equal_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e));
        
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
        assert(is_less_equal_application(e) || is_less_application(e) || is_greater_equal_application(e) || is_greater_application(e) || is_max_application(e) || is_min_application(e) || is_plus_application(e) || is_minus_application(e) || is_times_application(e) || is_exp_application(e));
        
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
        assert(is_creal_application(e) || is_pos2real_application(e) || is_nat2real_application(e) || is_int2real_application(e) || is_real2pos_application(e) || is_real2nat_application(e) || is_real2int_application(e) || is_abs_application(e) || is_negate_application(e) || is_succ_application(e) || is_pred_application(e));
        
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
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), equal_to(creal(variable("x", int_())), creal(variable("y", int_()))), equal_to(variable("x", int_()), variable("y", int_()))));
        result.push_back(data_equation(variable_list(), true_(), int2real(), creal()));
        result.push_back(data_equation(make_vector(variable("n", nat())), true_(), nat2real(variable("n", nat())), creal(cint(variable("n", nat())))));
        result.push_back(data_equation(make_vector(variable("p", pos())), true_(), pos2real(variable("p", pos())), creal(cint(cnat(variable("p", pos()))))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), real2int(creal(variable("x", int_()))), variable("x", int_())));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), real2nat(creal(variable("x", int_()))), int2nat(variable("x", int_()))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), real2pos(creal(variable("x", int_()))), int2pos(variable("x", int_()))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), less_equal(creal(variable("x", int_())), creal(variable("y", int_()))), less_equal(variable("x", int_()), variable("y", int_()))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), less(creal(variable("x", int_())), creal(variable("y", int_()))), less(variable("x", int_()), variable("y", int_()))));
        result.push_back(data_equation(make_vector(variable("s", real_()), variable("r", real_())), true_(), greater_equal(variable("r", real_()), variable("s", real_())), less_equal(variable("s", real_()), variable("r", real_()))));
        result.push_back(data_equation(make_vector(variable("s", real_()), variable("r", real_())), true_(), greater(variable("r", real_()), variable("s", real_())), less(variable("s", real_()), variable("r", real_()))));
        result.push_back(data_equation(make_vector(variable("s", real_()), variable("r", real_())), true_(), max(variable("r", real_()), variable("s", real_())), if_(less_equal(variable("r", real_()), variable("s", real_())), variable("s", real_()), variable("r", real_()))));
        result.push_back(data_equation(make_vector(variable("s", real_()), variable("r", real_())), true_(), min(variable("r", real_()), variable("s", real_())), if_(less_equal(variable("r", real_()), variable("s", real_())), variable("r", real_()), variable("s", real_()))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), abs(creal(variable("x", int_()))), creal(cint(abs(variable("x", int_()))))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), negate(creal(variable("x", int_()))), creal(negate(variable("x", int_())))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), succ(creal(variable("x", int_()))), creal(succ(variable("x", int_())))));
        result.push_back(data_equation(make_vector(variable("x", int_())), true_(), pred(creal(variable("x", int_()))), creal(pred(variable("x", int_())))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), plus(creal(variable("x", int_())), creal(variable("y", int_()))), creal(plus(variable("x", int_()), variable("y", int_())))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), minus(creal(variable("x", int_())), creal(variable("y", int_()))), creal(minus(variable("x", int_()), variable("y", int_())))));
        result.push_back(data_equation(make_vector(variable("y", int_()), variable("x", int_())), true_(), times(creal(variable("x", int_())), creal(variable("y", int_()))), creal(times(variable("x", int_()), variable("y", int_())))));
        result.push_back(data_equation(make_vector(variable("x", int_()), variable("n", nat())), true_(), exp(creal(variable("x", int_())), variable("n", nat())), creal(exp(variable("x", int_()), variable("n", nat())))));

        return result;
      }

    } // namespace real_
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_REAL__H
