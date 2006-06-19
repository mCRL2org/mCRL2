// Implementation of class Expression_Info
// file: expression_info.cpp

#include "expression_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Expression_Info --------------------------------------------------------------------------
  // Class Expression_Info - Functions declared public --------------------------------------------

    Expression_Info::Expression_Info() {
      f_greater_than = gsMakeOpIdNameGT();
      f_greater_than_or_equal = gsMakeOpIdNameGTE();
      f_less_than = gsMakeOpIdNameLT();
      f_less_than_or_equal = gsMakeOpIdNameLTE();
      f_plus = gsMakeOpIdNameAdd();
      f_unary_minus = gsMakeOpIdNameNeg();
      f_binary_minus = gsMakeOpIdNameSubt();
      f_multiplication = gsMakeOpIdNameMult();
      f_add_c = gsMakeOpIdNameAddC();
      f_c_nat = gsMakeOpIdNameCNat();
      f_true = gsMakeOpIdTrue();
      f_false = gsMakeOpIdFalse();
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

    bool Expression_Info::is_greater_than(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_greater_than;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_greater_than_or_equal(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_greater_than_or_equal;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_less_than;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_less_than_or_equal(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_less_than_or_equal;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_plus(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_plus;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_unary_minus(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsOpId(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          return v_term == f_unary_minus;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_binary_minus(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_binary_minus;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_multiplication(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsOpId(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_multiplication;
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_add_c(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsDataAppl(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            if (gsIsOpId(v_term)) {
              v_term = ATAgetArgument(v_term, 0);
              return v_term == f_add_c;
            }
          }
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_c_nat(ATermAppl a_expression) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_expression)) {
        v_term = ATAgetArgument(a_expression, 0);
        if (gsIsOpId(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          return v_term == f_c_nat;
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

    bool Expression_Info::is_variable(ATermAppl a_expression) {
      if (gsIsDataVarId(a_expression)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_constant(ATermAppl a_expression) {
      if (gsIsOpId(a_expression)) {
        return true;
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_int_constant(ATermAppl a_expression) {
      return gsIsIntConstant(a_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_nat_constant(ATermAppl a_expression) {
      return gsIsNatConstant(a_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_pos_constant(ATermAppl a_expression) {
      return gsIsPosConstant(a_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_true(ATermAppl a_expression) {
      return a_expression == f_true;
    }

    // --------------------------------------------------------------------------------------------

    bool Expression_Info::is_false(ATermAppl a_expression) {
      return a_expression == f_false;
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

