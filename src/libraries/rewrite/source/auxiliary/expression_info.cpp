// Implementation of class Expression_Info
// file: expression_info.cpp

#include "auxiliary/expression_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Expression_Info --------------------------------------------------------------------------
  // Class Expression_Info - Functions declared public --------------------------------------------

    Expression_Info::Expression_Info() {
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_operator(ATermAppl a_expression) {
      int v_number_of_arguments;

      v_number_of_arguments = ATgetLength(gsGetDataExprArgs(a_expression));
      for (int i = 0; i < v_number_of_arguments; i++) {
        a_expression = ATAgetArgument(a_expression, 0);
      }
      if (gsIsOpId(a_expression) && (v_number_of_arguments != 0)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Expression_Info::get_sort_of_operator(ATermAppl a_expression) {
      int v_number_of_arguments;

      v_number_of_arguments = ATgetLength(gsGetDataExprArgs(a_expression));
      for (int i = 0; i < v_number_of_arguments; i++) {
        a_expression = ATAgetArgument(a_expression, 0);
      }
      a_expression = ATAgetArgument(a_expression, 1);
      return a_expression;
    }

    // --------------------------------------------------------------------------------------------
    ATermAppl Expression_Info::get_argument(ATermAppl a_expression, int a_number) {
      int v_number_of_arguments;
      int v_remove_number;

      v_number_of_arguments = ATgetLength(gsGetDataExprArgs(a_expression));
      v_remove_number = v_number_of_arguments - (a_number + 1);
      for (int i = 0; i < v_remove_number; i++) {
        a_expression = ATAgetArgument(a_expression, 0);
      }
      return ATAgetArgument(a_expression, 1);
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

