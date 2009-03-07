#ifndef MCRL2_NEW_DATA_STANDARD_H
#define MCRL2_NEW_DATA_STANDARD_H

#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"

namespace mcrl2 {

  namespace new_data {

    // Function symbol ==
    function_symbol equal_to(const sort_expression& s);

    // Recogniser for ==
    bool is_equal_to_function_symbol(const data_expression& e);

    // Application of ==
    application equal_to(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of ==
    bool is_equal_to_application(const data_expression& e);

    // Function symbol !=
    function_symbol not_equal_to(const sort_expression& s);

    // Recogniser for !=
    bool is_not_equal_to_function_symbol(const data_expression& e);

    // Application of !=
    application not_equal_to(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of !=
    bool is_not_equal_to_application(const data_expression& e);

    // Function symbol if
    function_symbol if_(const sort_expression& s);

    // Recogniser for if
    bool is_if__function_symbol(const data_expression& e);

    // Application of if
    application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2);

    // Recogniser for application of if
    bool is_if__application(const data_expression& e);

    // Function symbol <
    function_symbol less(const sort_expression& s);

    // Recogniser for <
    bool is_less_function_symbol(const data_expression& e);

    // Application of <
    application less(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of <
    bool is_less_application(const data_expression& e);

    // Function symbol <=
    function_symbol less_equal(const sort_expression& s);

    // Recogniser for <=
    bool is_less_equal_function_symbol(const data_expression& e);

    // Application of <=
    application less_equal(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of <=
    bool is_less_equal_application(const data_expression& e);

    // Function symbol >
    function_symbol greater(const sort_expression& s);

    // Recogniser for >
    bool is_greater_function_symbol(const data_expression& e);

    // Application of >
    application greater(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of >
    bool is_greater_application(const data_expression& e);

    // Function symbol >=
    function_symbol greater_equal(const sort_expression& s);

    // Recogniser for >=
    bool is_greater_equal_function_symbol(const data_expression& e);

    // Application of >=
    application greater_equal(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of >=
    bool is_greater_equal_application(const data_expression& e);

    // Give all standard system defined functions for sort s
    function_symbol_vector standard_generate_functions_code(const sort_expression& s);

    // Give all standard system defined equations for sort s
    data_equation_vector standard_generate_equations_code(const sort_expression& s);

  } // namespace new_data
} // namespace mcrl2

#endif // MCRL2_NEW_DATA_STANDARD__H
