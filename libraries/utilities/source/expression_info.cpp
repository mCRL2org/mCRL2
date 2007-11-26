// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/expression_info.cpp
/// \brief Add your file description here.

// Implementation of class Expression_Info
// file: expression_info.cpp

#include "mcrl2/struct.h"
#include "mcrl2/utilities/expression_info.h"
#include "mcrl2/utilities/aterm_ext.h"

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

// Class Expression_Info --------------------------------------------------------------------------
  // Class Expression_Info - Functions declared public --------------------------------------------

    Expression_Info::Expression_Info() {
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_operator(ATermAppl a_expression) {
      int v_number_of_arguments;

      v_number_of_arguments = ATgetLength(gsGetDataExprArgs(a_expression));
      a_expression = ATAgetArgument(a_expression, 0);
      if (gsIsOpId(a_expression) && (v_number_of_arguments != 0)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Expression_Info::get_argument(ATermAppl a_expression, int a_number) {
      ATermList v_arguments;

      v_arguments = gsGetDataExprArgs(a_expression);
      for (int i = 0; i < a_number; i++) {
        v_arguments = ATgetNext(v_arguments);
      }
      return ATAgetFirst(v_arguments);
    }
    // --------------------------------------------------------------------------------------------

    ATermAppl Expression_Info::get_operator(ATermAppl a_expression) {
       ATermAppl v_result;

      v_result = a_expression;
      while (!gsIsOpId(v_result)) {
        v_result = ATAgetArgument(v_result, 0);
      }
      return v_result;
    }

