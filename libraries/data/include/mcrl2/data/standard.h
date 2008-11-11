#ifndef MCRL2_DATA_STANDARD_H
#define MCRL2_DATA_STANDARD_H

#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/data_equation.h"

namespace mcrl2 {

  namespace data {

    // Function symbol ==
    function_symbol equal_to();

    // Recogniser for ==
    bool is_equal_to_function_symbol(const data_expression& e);

    // Application of ==
    application equal_to(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of ==
    bool is_equal_to_application(const data_expression& e);

    // Function symbol !=
    function_symbol not_equal_to();

    // Recogniser for !=
    bool is_not_equal_to_function_symbol(const data_expression& e);

    // Application of !=
    application not_equal_to(const data_expression& arg0, const data_expression& arg1);

    // Recogniser for application of !=
    bool is_not_equal_to_application(const data_expression& e);

    // Function symbol if
    function_symbol if_();

    // Recogniser for if
    bool is_if__function_symbol(const data_expression& e);

    // Application of if
    application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2);

    // Recogniser for application of if
    bool is_if__application(const data_expression& e);

    // Give all system defined constructors for Bool
    function_symbol_list standard_generate_constructors_code();

    // Give all system defined constructors for Bool
    function_symbol_list standard_generate_functions_code();

    // Give all system defined equations for Bool
    data_equation_list standard_generate_equations_code();

  } // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_STANDARD__H
