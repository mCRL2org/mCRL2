// Author(s): Aad Mathijssen, Jeroen Keiren
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_ir.h

#ifndef MCRL2_LIBSTRUCT_IR_H
#define MCRL2_LIBSTRUCT_IR_H

#include <aterm2.h>

/*
const char* struct_prefix = "Struct@";
const char* list_prefix   = "List@";
const char* set_prefix    = "Set@";
const char* bag_prefix    = "Bag@";
const char* lambda_prefix = "lambda@";
*/

bool is_struct_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a structured sort

bool is_list_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a list sort

bool is_set_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a set sort

bool is_bag_sort_id(ATermAppl sort_expr);
//Pre: sort_expr is sort expression
//Ret: sort_expr is the implementation of a bag sort

bool is_lambda_op_id(ATermAppl data_expr);
//Pre: data_expr is a data expression
//Ret: data_expr is an operation identifier for the implementation of a lambda
//     abstraction

ATermList get_free_vars(ATermAppl data_expr);
//Pre: data_expr is a data expression that adheres to the internal syntax after
//     type checking
//Ret: The free variables in data_expr

#endif //MCRL2_LIBSTRUCT_IR_H

