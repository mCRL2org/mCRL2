#ifndef MCRL2_DATA_SET_H
#define MCRL2_DATA_SET_H

#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/forall.h"


namespace mcrl2 {

  namespace data {

    namespace sort_set {

      // Sort expression Set(s)
      inline
      container_sort set(const sort_expression& s)
      {
        //static container_sort set("set", s);
        container_sort set("set", s);
        return set;
      }

      // Recogniser for sort expression Set(s)
      inline
      bool is_set(const sort_expression& e)
      {
        if (e.is_container_sort())
        {
          return static_cast<const container_sort&>(e).container_name() == "set";
        }
        return false;
      }

      // Function symbol @set
      inline
      function_symbol set_comprehension(const sort_expression& s)
      {
        //static function_symbol set_comprehension("@set", function_sort(function_sort(s, sort_bool_::bool_()), sort_set::set(s)));
        function_symbol set_comprehension("@set", function_sort(function_sort(s, sort_bool_::bool_()), sort_set::set(s)));
        return set_comprehension;
      }

      // Recogniser for @set
      inline
      bool is_set_comprehension_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "@set";
        }
        return false;
      }

      // Application of @set
      inline
      application set_comprehension(const sort_expression& s, const data_expression& arg0)
      {
        
        return application(set_comprehension(s),arg0);
      }

      // Recogniser for application of @set
      inline
      bool is_set_comprehension_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_set_comprehension_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol {}
      inline
      function_symbol emptyset(const sort_expression& s)
      {
        //static function_symbol emptyset("{}", sort_set::set(s));
        function_symbol emptyset("{}", sort_set::set(s));
        return emptyset;
      }

      // Recogniser for {}
      inline
      bool is_emptyset_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "{}";
        }
        return false;
      }

      // Function symbol in
      inline
      function_symbol setin(const sort_expression& s)
      {
        //static function_symbol setin("in", function_sort(s, sort_set::set(s), sort_bool_::bool_()));
        function_symbol setin("in", function_sort(s, sort_set::set(s), sort_bool_::bool_()));
        return setin;
      }

      // Recogniser for in
      inline
      bool is_setin_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "in";
        }
        return false;
      }

      // Application of in
      inline
      application setin(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(arg0.sort() == s);
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(setin(s),arg0, arg1);
      }

      // Recogniser for application of in
      inline
      bool is_setin_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setin_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <=
      inline
      function_symbol subset_or_equal(const sort_expression& s)
      {
        //static function_symbol subset_or_equal("<=", function_sort(sort_set::set(s), sort_set::set(s), sort_bool_::bool_()));
        function_symbol subset_or_equal("<=", function_sort(sort_set::set(s), sort_set::set(s), sort_bool_::bool_()));
        return subset_or_equal;
      }

      // Recogniser for <=
      inline
      bool is_subset_or_equal_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "<=";
        }
        return false;
      }

      // Application of <=
      inline
      application subset_or_equal(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_set::is_set(arg0.sort()));
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(subset_or_equal(s),arg0, arg1);
      }

      // Recogniser for application of <=
      inline
      bool is_subset_or_equal_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_subset_or_equal_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol <
      inline
      function_symbol subset(const sort_expression& s)
      {
        //static function_symbol subset("<", function_sort(sort_set::set(s), sort_set::set(s), sort_bool_::bool_()));
        function_symbol subset("<", function_sort(sort_set::set(s), sort_set::set(s), sort_bool_::bool_()));
        return subset;
      }

      // Recogniser for <
      inline
      bool is_subset_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "<";
        }
        return false;
      }

      // Application of <
      inline
      application subset(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_set::is_set(arg0.sort()));
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(subset(s),arg0, arg1);
      }

      // Recogniser for application of <
      inline
      bool is_subset_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_subset_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol +
      inline
      function_symbol setunion_(const sort_expression& s)
      {
        //static function_symbol setunion_("+", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        function_symbol setunion_("+", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        return setunion_;
      }

      // Recogniser for +
      inline
      bool is_setunion__function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "+";
        }
        return false;
      }

      // Application of +
      inline
      application setunion_(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_set::is_set(arg0.sort()));
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(setunion_(s),arg0, arg1);
      }

      // Recogniser for application of +
      inline
      bool is_setunion__application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setunion__function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol -
      inline
      function_symbol setdifference(const sort_expression& s)
      {
        //static function_symbol setdifference("-", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        function_symbol setdifference("-", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        return setdifference;
      }

      // Recogniser for -
      inline
      bool is_setdifference_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "-";
        }
        return false;
      }

      // Application of -
      inline
      application setdifference(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_set::is_set(arg0.sort()));
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(setdifference(s),arg0, arg1);
      }

      // Recogniser for application of -
      inline
      bool is_setdifference_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setdifference_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol *
      inline
      function_symbol setintersection(const sort_expression& s)
      {
        //static function_symbol setintersection("*", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        function_symbol setintersection("*", function_sort(sort_set::set(s), sort_set::set(s), sort_set::set(s)));
        return setintersection;
      }

      // Recogniser for *
      inline
      bool is_setintersection_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "*";
        }
        return false;
      }

      // Application of *
      inline
      application setintersection(const sort_expression& s, const data_expression& arg0, const data_expression& arg1)
      {
        //assert(sort_set::is_set(arg0.sort()));
        //assert(sort_set::is_set(arg1.sort()));
        
        return application(setintersection(s),arg0, arg1);
      }

      // Recogniser for application of *
      inline
      bool is_setintersection_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setintersection_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Function symbol !
      inline
      function_symbol setcomplement(const sort_expression& s)
      {
        //static function_symbol setcomplement("!", function_sort(sort_set::set(s), sort_set::set(s)));
        function_symbol setcomplement("!", function_sort(sort_set::set(s), sort_set::set(s)));
        return setcomplement;
      }

      // Recogniser for !
      inline
      bool is_setcomplement_function_symbol(const data_expression& e)
      {
        if (e.is_function_symbol())
        {
          return static_cast<const function_symbol&>(e).name() == "!";
        }
        return false;
      }

      // Application of !
      inline
      application setcomplement(const sort_expression& s, const data_expression& arg0)
      {
        //assert(sort_set::is_set(arg0.sort()));
        
        return application(setcomplement(s),arg0);
      }

      // Recogniser for application of !
      inline
      bool is_setcomplement_application(const data_expression& e)
      {
        if (e.is_application())
        {
          return is_setcomplement_function_symbol(static_cast<const application&>(e).head());
        }
        return false;
      }

      // Give all system defined constructors for Set
      inline
      function_symbol_list set_generate_constructors_code()
      {
        function_symbol_list result;

        return result;
      }

      // Give all system defined constructors for Set
      inline
      function_symbol_list set_generate_functions_code(const sort_expression& s)
      {
        function_symbol_list result;
        result.push_back(set_comprehension(s));
        result.push_back(emptyset(s));
        result.push_back(setin(s));
        result.push_back(subset_or_equal(s));
        result.push_back(subset(s));
        result.push_back(setunion_(s));
        result.push_back(setdifference(s));
        result.push_back(setintersection(s));
        result.push_back(setcomplement(s));

        return result;
      }

      // Function for projecting out right
      inline
      data_expression right(const data_expression& e)
      {
        //assert( || is_setin_application(e) || is_subset_or_equal_application(e) || is_subset_application(e) || is_setunion__application(e) || is_setdifference_application(e) || is_setintersection_application(e));
        
        if (is_setin_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_subset_or_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_subset_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_setunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_setdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[1];
        }

        if (is_setintersection_application(e))
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
        //assert( || is_setin_application(e) || is_subset_or_equal_application(e) || is_subset_application(e) || is_setunion__application(e) || is_setdifference_application(e) || is_setintersection_application(e));
        
        if (is_setin_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_subset_or_equal_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_subset_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_setunion__application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_setdifference_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_setintersection_application(e))
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
        //assert( || is_set_comprehension_application(e) || is_setcomplement_application(e));
        
        if (is_set_comprehension_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        if (is_setcomplement_application(e))
        {
          return static_cast<const application&>(e).arguments()[0];
        }

        // This should never be reached, otherwise something is severely wrong.
        assert(false); 
      }

      // Give all system defined equations for Set
      inline
      data_equation_list set_generate_equations_code(const sort_expression& s)
      {
        data_equation_list result;
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_())), variable("g", function_sort(s, sort_bool_::bool_()))), equal_to(sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_()))), sort_set::set_comprehension(s, variable("g", function_sort(s, sort_bool_::bool_())))), equal_to(variable("f", function_sort(s, sort_bool_::bool_())), variable("g", function_sort(s, sort_bool_::bool_())))));
        result.push_back(data_equation(variable_list(), sort_set::emptyset(s), sort_set::set_comprehension(s, lambda(make_vector(variable("x", s)),sort_bool_::false_()))));
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_())), variable("d", s)), sort_set::setin(s, variable("d", s), sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_())))), variable("f", function_sort(s, sort_bool_::bool_()))(variable("d", s))));
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_())), variable("g", function_sort(s, sort_bool_::bool_()))), sort_set::subset_or_equal(s, sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_()))), sort_set::set_comprehension(s, variable("g", function_sort(s, sort_bool_::bool_())))), forall(make_vector(variable("x", s)),sort_bool_::implies(variable("f", function_sort(s, sort_bool_::bool_()))(variable("x", s)), variable("g", function_sort(s, sort_bool_::bool_()))(variable("x", s))))));
        result.push_back(data_equation(make_vector(variable("t", sort_set::set(s)), variable("s", sort_set::set(s))), sort_set::subset(s, variable("s", sort_set::set(s)), variable("t", sort_set::set(s))), sort_bool_::and_(sort_set::subset_or_equal(s, variable("s", sort_set::set(s)), variable("t", sort_set::set(s))), not_equal_to(variable("s", sort_set::set(s)), variable("t", sort_set::set(s))))));
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_())), variable("g", function_sort(s, sort_bool_::bool_()))), sort_set::setunion_(s, sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_()))), sort_set::set_comprehension(s, variable("g", function_sort(s, sort_bool_::bool_())))), sort_set::set_comprehension(s, lambda(make_vector(variable("x", s)),sort_bool_::or_(variable("f", function_sort(s, sort_bool_::bool_()))(variable("x", s)), variable("g", function_sort(s, sort_bool_::bool_()))(variable("x", s)))))));
        result.push_back(data_equation(make_vector(variable("t", sort_set::set(s)), variable("s", sort_set::set(s))), sort_set::setdifference(s, variable("s", sort_set::set(s)), variable("t", sort_set::set(s))), sort_set::setintersection(s, variable("s", sort_set::set(s)), sort_set::setcomplement(s, variable("t", sort_set::set(s))))));
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_())), variable("g", function_sort(s, sort_bool_::bool_()))), sort_set::setintersection(s, sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_()))), sort_set::set_comprehension(s, variable("g", function_sort(s, sort_bool_::bool_())))), sort_set::set_comprehension(s, lambda(make_vector(variable("x", s)),sort_bool_::and_(variable("f", function_sort(s, sort_bool_::bool_()))(variable("x", s)), variable("g", function_sort(s, sort_bool_::bool_()))(variable("x", s)))))));
        result.push_back(data_equation(make_vector(variable("f", function_sort(s, sort_bool_::bool_()))), sort_set::setcomplement(s, sort_set::set_comprehension(s, variable("f", function_sort(s, sort_bool_::bool_())))), sort_set::set_comprehension(s, lambda(make_vector(variable("x", s)),sort_bool_::not_(variable("f", function_sort(s, sort_bool_::bool_()))(variable("x", s)))))));

        return result;
      }

    } // namespace set
  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_SET_H
