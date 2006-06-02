// Implementation of class Expression_Info
// file: expression_info.cpp

#include "expression_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Expression_Info --------------------------------------------------------------------------
  // Class Expression_Info - Functions declared public --------------------------------------------

    Expression_Info::Expression_Info() {
      f_greater_than_int = gsMakeOpIdGT(gsMakeSortIdInt());
      f_greater_than_real = gsMakeOpIdGT(gsMakeSortIdReal());
      f_greater_than_or_equal_int = gsMakeOpIdGTE(gsMakeSortIdInt());
      f_greater_than_or_equal_real = gsMakeOpIdGTE(gsMakeSortIdReal());
      f_less_than_int = gsMakeOpIdLT(gsMakeSortIdInt());
      f_less_than_real = gsMakeOpIdLT(gsMakeSortIdReal());
      f_less_than_or_equal_int = gsMakeOpIdLTE(gsMakeSortIdInt());
      f_less_than_or_equal_real = gsMakeOpIdLTE(gsMakeSortIdReal());
      f_plus_int = gsMakeOpIdAdd(gsMakeSortIdInt(), gsMakeSortIdInt());
      f_plus_real = gsMakeOpIdAdd(gsMakeSortIdReal(), gsMakeSortIdReal());
      f_unary_minus_int = gsMakeOpIdNeg(gsMakeSortIdInt());
      f_unary_minus_real = gsMakeOpIdNeg(gsMakeSortIdReal());
      f_binary_minus_int = gsMakeOpIdSubt(gsMakeSortIdInt());
      f_binary_minus_real = gsMakeOpIdSubt(gsMakeSortIdReal());
      f_multiplication_int = gsMakeOpIdMult(gsMakeSortIdInt());
      f_multiplication_real = gsMakeOpIdMult(gsMakeSortIdReal());
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_not(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        return v_term == gsMakeOpIdNot();
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_equality(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == gsMakeOpIdNameEq();
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_inequality(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == gsMakeOpIdNameNeq();
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_greater_than_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_greater_than_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_greater_than_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_greater_than_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_greater_than_or_equal_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_greater_than_or_equal_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_greater_than_or_equal_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_greater_than_or_equal_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_less_than_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_less_than_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than_or_equal_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_less_than_or_equal_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than_or_equal_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_less_than_or_equal_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_plus_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_plus_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_plus_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_plus_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_unary_minus_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsOpId(v_term)) {
          return v_term == f_unary_minus_int;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_unary_minus_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsOpId(v_term)) {
          return v_term == f_unary_minus_real;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_binary_minus_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_binary_minus_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_binary_minus_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_binary_minus_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_multiplication_int(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_multiplication_int;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_multiplication_real(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            return v_term == f_multiplication_real;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_operator(ATermAppl a_expression) {
      int v_number_of_arguments;

      v_number_of_arguments = get_number_of_arguments(a_expression);
      for (int i = 0; i < v_number_of_arguments; i++) {
        a_expression = ATAgetArgument(a_expression, 0);
      }
      if (gsIsOpId(a_expression) && (v_number_of_arguments != 0)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_real_variable(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataVarId(a_expression)) {
        v_term = ATAgetArgument(a_expression, 1);
        return v_term == gsMakeSortIdReal();
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_int_variable(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataVarId(a_expression)) {
        v_term = ATAgetArgument(a_expression, 1);
        return v_term == gsMakeSortIdInt();
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_variable(ATermAppl a_expression) {
      if (gsIsDataVarId(a_expression)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_int_constant(ATermAppl a_expression) {
      return gsIsIntConstant(a_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_constant(ATermAppl a_expression) {
      return gsIsOpId(a_expression);
    }

    // --------------------------------------------------------------------------------------------

    int Expression_Info::get_number_of_arguments(ATermAppl a_expression) {
      int v_result = 0;

      while (gsIsDataAppl(a_expression)) {
        a_expression = ATAgetArgument(a_expression, 0);
        v_result++;
      }
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Expression_Info::get_sort_of_variable(ATermAppl a_expression) {
      a_expression = ATAgetArgument(a_expression, 1);
      return a_expression;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Expression_Info::get_sort_of_operator(ATermAppl a_expression) {
      int v_number_of_arguments;

      v_number_of_arguments = get_number_of_arguments(a_expression);
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

      v_number_of_arguments = get_number_of_arguments(a_expression);
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

