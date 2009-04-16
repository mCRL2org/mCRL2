#ifndef MCRL2_NEW_DATA_STANDARD_H
#define MCRL2_NEW_DATA_STANDARD_H

#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/data_equation.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Constructor for function symbol ==
    /// \param s A sort expression
    /// \return function symbol equal_to
    function_symbol equal_to(const sort_expression& s);

    /// \brief Recogniser for function ==
    /// \param e A data expression
    /// \return true iff e is the function symbol matching ==
    bool is_equal_to_function_symbol(const data_expression& e);

    /// \brief Application of function symbol ==
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of == to a number of arguments
    application equal_to(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of ==
    /// \param e A data expression
    /// \return true iff e is an application of function symbol equal_to to a
    ///     number of arguments
    bool is_equal_to_application(const data_expression& e);

    /// \brief Constructor for function symbol !=
    /// \param s A sort expression
    /// \return function symbol not_equal_to
    function_symbol not_equal_to(const sort_expression& s);

    /// \brief Recogniser for function !=
    /// \param e A data expression
    /// \return true iff e is the function symbol matching !=
    bool is_not_equal_to_function_symbol(const data_expression& e);

    /// \brief Application of function symbol !=
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of != to a number of arguments
    application not_equal_to(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of !=
    /// \param e A data expression
    /// \return true iff e is an application of function symbol not_equal_to to a
    ///     number of arguments
    bool is_not_equal_to_application(const data_expression& e);

    /// \brief Constructor for function symbol if
    /// \param s A sort expression
    /// \return function symbol if_
    function_symbol if_(const sort_expression& s);

    /// \brief Recogniser for function if
    /// \param e A data expression
    /// \return true iff e is the function symbol matching if_
    bool is_if__function_symbol(const data_expression& e);

    /// \brief Application of function symbol if
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \param arg2 A data expression
    /// \return Application of if to a number of arguments
    application if_(const data_expression& arg0, const data_expression& arg1, const data_expression& arg2);

    /// \brief Recogniser for application of if
    /// \param e A data expression
    /// \return true iff e is an application of function symbol if_ to a
    ///     number of arguments
    bool is_if__application(const data_expression& e);

    /// \brief Constructor for function symbol <
    /// \param s A sort expression
    /// \return function symbol less
    function_symbol less(const sort_expression& s);

    /// \brief Recogniser for function <
    /// \param e A data expression
    /// \return true iff e is the function symbol matching <
    bool is_less_function_symbol(const data_expression& e);

    /// \brief Application of function symbol <
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of < to a number of arguments
    application less(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of <
    /// \param e A data expression
    /// \return true iff e is an application of function symbol less to a
    ///     number of arguments
    bool is_less_application(const data_expression& e);

    /// \brief Constructor for function symbol <=
    /// \param s A sort expression
    /// \return function symbol less_equal
    function_symbol less_equal(const sort_expression& s);

    /// \brief Recogniser for function <=
    /// \param e A data expression
    /// \return true iff e is the function symbol matching <=
    bool is_less_equal_function_symbol(const data_expression& e);

    /// \brief Application of function symbol <=
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of <= to a number of arguments
    application less_equal(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of <=
    /// \param e A data expression
    /// \return true iff e is an application of function symbol less_equal to a
    ///     number of arguments
    bool is_less_equal_application(const data_expression& e);

    /// \brief Constructor for function symbol >
    /// \param s A sort expression
    /// \return function symbol greater
    function_symbol greater(const sort_expression& s);

    /// \brief Recogniser for function >
    /// \param e A data expression
    /// \return true iff e is the function symbol matching >
    bool is_greater_function_symbol(const data_expression& e);

    /// \brief Application of function symbol >
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of > to a number of arguments
    application greater(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of >
    /// \param e A data expression
    /// \return true iff e is an application of function symbol greater to a
    ///     number of arguments
    bool is_greater_application(const data_expression& e);

    /// \brief Constructor for function symbol >=
    /// \param s A sort expression
    /// \return function symbol greater_equal
    function_symbol greater_equal(const sort_expression& s);

    /// \brief Recogniser for function >=
    /// \param e A data expression
    /// \return true iff e is the function symbol matching >=
    bool is_greater_equal_function_symbol(const data_expression& e);

    /// \brief Application of function symbol >=
    /// \param arg0 A data expression
    /// \param arg1 A data expression
    /// \return Application of >= to a number of arguments
    application greater_equal(const data_expression& arg0, const data_expression& arg1);

    /// \brief Recogniser for application of >=
    /// \param e A data expression
    /// \return true iff e is an application of function symbol greater_equal to a
    ///     number of arguments
    bool is_greater_equal_application(const data_expression& e);

    /// \brief Give all standard system defined functions for sort s
    /// \param s A sort expression
    /// \return All standard system defined functions for sort s
    function_symbol_vector standard_generate_functions_code(const sort_expression& s);

    /// \brief Give all standard system defined equations for sort s
    /// \param s A sort expression
    /// \return All standard system defined equations for sort s
    data_equation_vector standard_generate_equations_code(const sort_expression& s);

  } // namespace new_data
} // namespace mcrl2

#endif // MCRL2_NEW_DATA_STANDARD__H
