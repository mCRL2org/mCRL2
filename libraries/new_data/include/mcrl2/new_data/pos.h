#ifndef MCRL2_NEW_DATA_POS_H
#define MCRL2_NEW_DATA_POS_H

#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"
#include "mcrl2/new_data/detail/utility.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/bool.h"


namespace mcrl2 {

  namespace new_data {

    namespace sort_pos {

      // Sort expression Pos
      inline
      basic_sort pos()
      {
        static basic_sort pos("Pos");
        return pos;
      }

      // Recogniser for sort expression Pos
      inline
      bool is_pos(const sort_expression& e)
      {
        if (e.is_basic_sort())
        {
          return static_cast<const basic_sort&>(e) == pos();
        }
        return false;
      }

      // Function symbol @c1
      inline
      function_symbol c1()
      {
        //static function_symbol c1("@c1", sort_pos::pos());
        function_symbol c1("@c1", sort_pos::pos());
        return c1;
      }

      // Recogniser for @c1
      inline
      bool is_c1_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@c1";
        }
        return false;
      }

      // Function symbol @cDub
      inline
      function_symbol cdub()
      {
        //static function_symbol cdub("@cDub", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos()));
        function_symbol cdub("@cDub", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos()));
        return cdub;
      }

      // Recogniser for @cDub
      inline
      bool is_cdub_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@cDub";
        }
        return false;
      }

      // Application of @cDub
      inline
      application cdub(const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_bool_::is_bool_(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
        return application(cdub(),arg0, arg1);
      }

      // Recogniser for application of @cDub
      inline
      bool is_cdub_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_cdub_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol max
      inline
      function_symbol maximum()
      {
        //static function_symbol maximum("max", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol maximum("max", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
        return application(maximum(),arg0, arg1);
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

      // Function symbol min
      inline
      function_symbol minimum()
      {
        //static function_symbol minimum("min", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol minimum("min", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
        return application(minimum(),arg0, arg1);
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

      // Function symbol abs
      inline
      function_symbol abs()
      {
        //static function_symbol abs("abs", function_sort(sort_pos::pos(), sort_pos::pos()));
        function_symbol abs("abs", function_sort(sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        
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
        //static function_symbol succ("succ", function_sort(sort_pos::pos(), sort_pos::pos()));
        function_symbol succ("succ", function_sort(sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        
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

      // Function symbol +
      inline
      function_symbol plus()
      {
        //static function_symbol plus("+", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol plus("+", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
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

      // Function symbol @addc
      inline
      function_symbol add_with_carry()
      {
        //static function_symbol add_with_carry("@addc", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol add_with_carry("@addc", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        return add_with_carry;
      }

      // Recogniser for @addc
      inline
      bool is_add_with_carry_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@addc";
        }
        return false;
      }

      // Application of @addc
      inline
      application add_with_carry(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
      {
        //assert(sort_bool_::is_bool_(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        //assert(sort_pos::is_pos(arg2.sort()));
        
        return application(add_with_carry(),arg0, arg1, arg2);
      }

      // Recogniser for application of @addc
      inline
      bool is_add_with_carry_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_add_with_carry_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol *
      inline
      function_symbol times()
      {
        //static function_symbol times("*", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol times("*", function_sort(sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
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
        //assert(sort_pos::is_pos(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        
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

      // Function symbol @multir
      inline
      function_symbol multir()
      {
        //static function_symbol multir("@multir", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        function_symbol multir("@multir", function_sort(sort_bool_::bool_(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos(), sort_pos::pos()));
        return multir;
      }

      // Recogniser for @multir
      inline
      bool is_multir_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@multir";
        }
        return false;
      }

      // Application of @multir
      inline
      application multir(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2, const data_expression& arg3)
      {
        //assert(sort_bool_::is_bool_(arg0.sort()));
        //assert(sort_pos::is_pos(arg1.sort()));
        //assert(sort_pos::is_pos(arg2.sort()));
        //assert(sort_pos::is_pos(arg3.sort()));
        
        return application(multir(),arg0, arg1, arg2, arg3);
      }

      // Recogniser for application of @multir
      inline
      bool is_multir_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_multir_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Give all system defined constructors for Pos
      inline
      function_symbol_list pos_generate_constructors_code()
      {
        function_symbol_list result;
        result.push_back(c1());
        result.push_back(cdub());

        return result;
      }

      // Give all system defined constructors for Pos
      inline
      function_symbol_list pos_generate_functions_code()
      {
        function_symbol_list result;
        result.push_back(maximum());
        result.push_back(minimum());
        result.push_back(abs());
        result.push_back(succ());
        result.push_back(plus());
        result.push_back(add_with_carry());
        result.push_back(times());
        result.push_back(multir());

        return result;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        //assert( || is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_add_with_carry_application(e) || is_times_application(e));
        
        if (is_maximum_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_minimum_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_plus_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_add_with_carry_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
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
        //assert( || is_multir_application(e));
        
        if (is_multir_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg2
      inline
      data_expression arg2(const data_expression& e)
      {
        //assert( || is_multir_application(e));
        
        if (is_multir_application(e))
        {
          return static_cast<const application&>(e).arguments()[2];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out arg3
      inline
      data_expression arg3(const data_expression& e)
      {
        //assert( || is_multir_application(e));
        
        if (is_multir_application(e))
        {
          return static_cast<const application&>(e).arguments()[3];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Function for projecting out number
      inline
      data_expression number(const data_expression& e)
      {
        //assert( || is_cdub_application(e) || is_abs_application(e) || is_succ_application(e));
        
        if (is_cdub_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_abs_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_succ_application(e))
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
        //assert( || is_cdub_application(e) || is_add_with_carry_application(e) || is_multir_application(e));
        
        if (is_cdub_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_add_with_carry_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_multir_application(e))
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
        //assert( || is_maximum_application(e) || is_minimum_application(e) || is_plus_application(e) || is_add_with_carry_application(e) || is_times_application(e));
        
        if (is_maximum_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_minimum_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_plus_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_add_with_carry_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_times_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for Pos
      inline
      data_equation_list pos_generate_equations_code()
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), equal_to(sort_pos::c1(), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), equal_to(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), equal_to(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), equal_to(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), equal_to(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("q", sort_pos::pos()))), equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), equal_to(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("c", sort_bool_::bool_()), variable("q", sort_pos::pos()))), sort_bool_::and_(equal_to(variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), equal_to(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), less(variable("p", sort_pos::pos()), sort_pos::c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), less(sort_pos::c1(), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()))), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("q", sort_pos::pos()))), less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), less(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("c", sort_bool_::bool_()), variable("q", sort_pos::pos()))), if_(sort_bool_::implies(variable("c", sort_bool_::bool_()), variable("b", sort_bool_::bool_())), less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), less_equal(sort_pos::c1(), variable("p", sort_pos::pos())), sort_bool_::true_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), less_equal(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::c1()), sort_bool_::false_()));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("q", sort_pos::pos()))), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("q", sort_pos::pos()))), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), less_equal(sort_pos::cdub(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("c", sort_bool_::bool_()), variable("q", sort_pos::pos()))), if_(sort_bool_::implies(variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::maximum(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), if_(less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), variable("q", sort_pos::pos()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::minimum(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), if_(less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::abs(variable("p", sort_pos::pos())), variable("p", sort_pos::pos())));
        result.push_back(data_equation(variable_list(), sort_pos::succ(sort_pos::c1()), sort_pos::cdub(sort_bool_::false_(), sort_pos::c1())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::succ(sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos()))), sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::succ(sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos()))), sort_pos::cdub(sort_bool_::false_(), sort_pos::succ(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::plus(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::false_(), sort_pos::c1(), variable("p", sort_pos::pos())), sort_pos::succ(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::true_(), sort_pos::c1(), variable("p", sort_pos::pos())), sort_pos::succ(sort_pos::succ(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), sort_pos::c1()), sort_pos::succ(variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::true_(), variable("p", sort_pos::pos()), sort_pos::c1()), sort_pos::succ(sort_pos::succ(variable("p", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), variable("b", sort_bool_::bool_()), variable("c", sort_bool_::bool_())), sort_pos::add_with_carry(variable("b", sort_bool_::bool_()), sort_pos::cdub(variable("c", sort_bool_::bool_()), variable("p", sort_pos::pos())), sort_pos::cdub(variable("c", sort_bool_::bool_()), variable("q", sort_pos::pos()))), sort_pos::cdub(variable("b", sort_bool_::bool_()), sort_pos::add_with_carry(variable("c", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::add_with_carry(variable("b", sort_bool_::bool_()), sort_pos::cdub(sort_bool_::false_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos()))), sort_pos::cdub(sort_bool_::not_(variable("b", sort_bool_::bool_())), sort_pos::add_with_carry(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::add_with_carry(variable("b", sort_bool_::bool_()), sort_pos::cdub(sort_bool_::true_(), variable("p", sort_pos::pos())), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos()))), sort_pos::cdub(sort_bool_::not_(variable("b", sort_bool_::bool_())), sort_pos::add_with_carry(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less_equal(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), less(variable("q", sort_pos::pos()), variable("p", sort_pos::pos())), sort_pos::times(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::false_(), sort_pos::c1(), variable("q", sort_pos::pos()), variable("p", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::false_(), variable("p", sort_pos::pos()), sort_pos::c1(), variable("q", sort_pos::pos())), variable("q", sort_pos::pos())));
        result.push_back(data_equation(make_vector(variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::true_(), variable("p", sort_pos::pos()), sort_pos::c1(), variable("q", sort_pos::pos())), sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()))));
        result.push_back(data_equation(make_vector(variable("r", sort_pos::pos()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), variable("b", sort_bool_::bool_())), sort_pos::multir(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), sort_pos::cdub(sort_bool_::false_(), variable("q", sort_pos::pos())), variable("r", sort_pos::pos())), sort_pos::multir(variable("b", sort_bool_::bool_()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos()), sort_pos::cdub(sort_bool_::false_(), variable("r", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("r", sort_pos::pos()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::false_(), variable("p", sort_pos::pos()), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos())), variable("r", sort_pos::pos())), sort_pos::multir(sort_bool_::true_(), variable("r", sort_pos::pos()), variable("q", sort_pos::pos()), sort_pos::cdub(sort_bool_::false_(), variable("r", sort_pos::pos())))));
        result.push_back(data_equation(make_vector(variable("r", sort_pos::pos()), variable("p", sort_pos::pos()), variable("q", sort_pos::pos())), sort_pos::multir(sort_bool_::true_(), variable("p", sort_pos::pos()), sort_pos::cdub(sort_bool_::true_(), variable("q", sort_pos::pos())), variable("r", sort_pos::pos())), sort_pos::multir(sort_bool_::true_(), sort_pos::add_with_carry(sort_bool_::false_(), variable("p", sort_pos::pos()), variable("r", sort_pos::pos())), variable("q", sort_pos::pos()), sort_pos::cdub(sort_bool_::false_(), variable("r", sort_pos::pos())))));

        return result;
      }

    } // namespace pos
  } // namespace new_data
} // namespace mcrl2

#endif // MCRL2_NEW_DATA_POS_H
