// Author(s): Alexander van Dam
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file sort_functions.h
/// \brief Add your file description here.

#include "mcrl2/data/data.h"

using namespace mcrl2::data;

sort_expression_list get_sorts(data_variable_list v);

bool check_finite(data_operation_list fl, sort_expression s);

bool check_finite_list(data_operation_list fl, sort_expression_list s);

data_expression_list enumerate_constructors(data_operation_list fl, sort_expression s);

data_expression_list create_data_expression_list(data_operation f, std::vector< data_expression_list > dess);
