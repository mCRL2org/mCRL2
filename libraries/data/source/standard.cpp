#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/bool.h"

#include <iostream>

namespace mcrl2 {

  namespace data {

    // Function symbol ==
    function_symbol equal_to(const sort_expression& s)
    {
      function_symbol equal_to("==", function_sort(s, s, sort_bool_::bool_()));
      return equal_to;
    }

    // Recogniser for ==
    bool is_equal_to_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == "==";
      }
      return false;
    }

    // Application of ==
    application equal_to(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(equal_to(arg0.sort()),arg0, arg1);
    }

    // Recogniser for application of ==
    bool is_equal_to_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_equal_to_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Function symbol !=
    function_symbol not_equal_to(const sort_expression& s)
    {
      function_symbol not_equal_to("!=", function_sort(s, s, sort_bool_::bool_()));
      return not_equal_to;
    }

    // Recogniser for !=
    bool is_not_equal_to_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == "!=";
      }
      return false;
    }

    // Application of !=
    application not_equal_to(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      
      return application(not_equal_to(arg0.sort()),arg0, arg1);
    }

    // Recogniser for application of !=
    bool is_not_equal_to_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_not_equal_to_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Function symbol if
    function_symbol if_(const sort_expression& s)
    {
      function_symbol if_("if", function_sort(sort_bool_::bool_(), s, s, s));
      return if_;
    }

    // Recogniser for if
    bool is_if__function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == "if";
      }
      return false;
    }

    // Application of if
    application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2)
    {
      assert(sort_bool_::is_bool_(arg0.sort()));
      assert(arg1.sort() == arg2.sort());
      
      return application(if_(arg1.sort()), arg0, arg1, arg2);
    }

    // Recogniser for application of if
    bool is_if__application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_if__function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Give all system defined constructors for s
    function_symbol_list standard_generate_functions_code(const sort_expression& s)
    {
      function_symbol_list result;
      result.push_back(equal_to(s));
      result.push_back(not_equal_to(s));
      result.push_back(if_(s));

      return result;
    }

    // Give all system defined equations for s
    data_equation_list standard_generate_equations_code(const sort_expression& s)
    {
      data_equation_list result;
      variable b("b", sort_bool_::bool_());
      variable x("x", s);
      variable y("y", s);
      result.push_back(data_equation(make_vector(x), sort_bool_::true_(), equal_to(x, x), sort_bool_::true_()));
      result.push_back(data_equation(make_vector(x, y), sort_bool_::true_(), not_equal_to(x, y), sort_bool_::not_(equal_to(x, y))));
      result.push_back(data_equation(make_vector(x, y), sort_bool_::true_(), if_(sort_bool_::true_(), x, y), x));
      result.push_back(data_equation(make_vector(x, y), sort_bool_::true_(), if_(sort_bool_::false_(), x, y), y));
      result.push_back(data_equation(make_vector(b, x), sort_bool_::true_(), if_(b, x, x), x));

      return result;
    }

  } // namespace data
} // namespace mcrl2

