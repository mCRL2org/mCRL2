#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/standard.h"
#include "mcrl2/new_data/bool.h"

#include <iostream>

namespace mcrl2 {

  namespace new_data {

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

    // Function symbol <
    function_symbol less(const sort_expression& s)
    {
      function_symbol less("<", function_sort(s, s, sort_bool_::bool_()));
      return less;
    }

    // Recogniser for <
    bool is_less_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == "<";
      }
      return false;
    }

    // Application of <
    application less(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(less(arg0.sort()),arg0, arg1);
    }


    // Recogniser for application of <
    bool is_less_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_less_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Function symbol <=
    function_symbol less_equal(const sort_expression& s)
    {
      function_symbol less_equal("<=", function_sort(s, s, sort_bool_::bool_()));
      return less_equal;
    }

    // Recogniser for <=
    bool is_less_equal_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == "<=";
      }
      return false;
    }

    // Application of <=
    application less_equal(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(less_equal(arg0.sort()),arg0, arg1);
    }

    // Recogniser for application of <=
    bool is_less_equal_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_less_equal_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Function symbol >
    function_symbol greater(const sort_expression& s)
    {
      function_symbol greater(">", function_sort(s, s, sort_bool_::bool_()));
      return greater;
    }

    // Recogniser for >
    bool is_greater_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == ">";
      }
      return false;
    }

    // Application of >
    application greater(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(greater(arg0.sort()),arg0, arg1);
    }

    // Recogniser for application of >
    bool is_greater_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_greater_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Function symbol >=
    function_symbol greater_equal(const sort_expression& s)
    {
      function_symbol greater_equal(">=", function_sort(s, s, sort_bool_::bool_()));
      return greater_equal;
    }

    // Recogniser for >=
    bool is_greater_equal_function_symbol(const data_expression& e)
    {
      if (e.is_function_symbol())
      {
        return static_cast<const function_symbol&>(e).name() == ">=";
      }
      return false;
    }

    // Application of >=
    application greater_equal(const data_expression& arg0, const data_expression& arg1)
    {
      assert(arg0.sort() == arg1.sort());
      return application(greater_equal(arg0.sort()),arg0, arg1);
    }

    // Recogniser for application of >=
    bool is_greater_equal_application(const data_expression& e)
    {
      if (e.is_application())
      {
        return is_greater_equal_function_symbol(static_cast<const application&>(e).head());
      }
      return false;
    }

    // Give all system defined constructors for s
    function_symbol_vector standard_generate_functions_code(const sort_expression& s)
    {
      function_symbol_vector result;
      result.push_back(equal_to(s));
      result.push_back(not_equal_to(s));
      result.push_back(if_(s));
      result.push_back(less(s));
      result.push_back(less_equal(s));
      result.push_back(greater_equal(s));
      result.push_back(greater(s));

      return result;
    }

    // Give all system defined equations for s
    data_equation_vector standard_generate_equations_code(const sort_expression& s)
    {
      data_equation_vector result;
      variable b("b", sort_bool_::bool_());
      variable x("x", s);
      variable y("y", s);
      result.push_back(data_equation(make_vector(x), equal_to(x, x), sort_bool_::true_()));
      result.push_back(data_equation(make_vector(x, y), not_equal_to(x, y), sort_bool_::not_(equal_to(x, y))));
      result.push_back(data_equation(make_vector(x, y), if_(sort_bool_::true_(), x, y), x));
      result.push_back(data_equation(make_vector(x, y), if_(sort_bool_::false_(), x, y), y));
      result.push_back(data_equation(make_vector(b, x), if_(b, x, x), x));
      result.push_back(data_equation(make_vector(x), less(x,x), sort_bool_::false_()));
      result.push_back(data_equation(make_vector(x), less_equal(x,x), sort_bool_::true_()));
      result.push_back(data_equation(make_vector(x, y), greater_equal(x,y), less_equal(y,x)));
      result.push_back(data_equation(make_vector(x, y), greater(x,y), less(y,x)));

      return result;
    }

  } // namespace new_data
} // namespace mcrl2

