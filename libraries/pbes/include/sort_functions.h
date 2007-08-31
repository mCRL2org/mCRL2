// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_functions.h
/// \brief Add your file description here.

#include "mcrl2/pbes/pbes.h"


lps::sort_list get_sorts(lps::data_variable_list v);

bool check_finite(lps::function_list fl, lps::sort s);

bool check_finite_list(lps::function_list fl, lps::sort_list s);

lps::data_expression_list enumerate_constructors(lps::function_list fl, lps::sort s);

lps::data_expression_list create_data_expression_list(lps::function f, std::vector< lps::data_expression_list > dess);
