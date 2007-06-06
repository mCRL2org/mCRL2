// Implementation of classes ATerm_Manipulator, AM_Jitty and AM_Inner
// file: manipulator.cpp

#include "aterm2.h"
#include "librewrite.h"
#include "libstruct.h"
#include "auxiliary/manipulator.h"
#include "auxiliary/info.h"

// Class ATerm_Manipulator ------------------------------------------------------------------------
  // Class ATerm_Manipulator - Functions declared public ------------------------------------------

    ATerm_Manipulator::ATerm_Manipulator(Rewriter* a_rewriter, ATerm_Info* a_info) {
      f_rewriter = a_rewriter;
      f_info = a_info;
      f_orient = ATtableCreate(2000, 50);
    }

    // --------------------------------------------------------------------------------------------

    ATerm_Manipulator::~ATerm_Manipulator() {
      ATtableDestroy(f_orient);
    }

// Class AM_Jitty ---------------------------------------------------------------------------------
  // Class AM_Jitty - Functions declared protected ------------------------------------------------

    ATerm AM_Jitty::set_true_auxiliary(ATerm a_formula, ATerm a_guard) {
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_true;
      }

      bool v_is_equality;

      v_is_equality = f_info->is_equality(a_guard);
      if (v_is_equality && ATgetArgument(a_guard, 2) == a_formula) {
        return ATgetArgument(a_guard, 1);
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_true, a_formula);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_formula);
      v_function = ATgetArgument(a_formula, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = set_true_auxiliary(ATgetArgument(a_formula, i), a_guard);
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      ATtablePut(f_set_true, a_formula, v_result);
      delete[] v_parts;
      v_parts = 0;

      return v_result;
    }

  // ----------------------------------------------------------------------------------------------

    ATerm AM_Jitty::set_false_auxiliary(ATerm a_formula, ATerm a_guard) {
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_false;
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_false, a_formula);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_formula);
      v_function = ATgetArgument(a_formula, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = set_false_auxiliary(ATgetArgument(a_formula, i), a_guard);
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      ATtablePut(f_set_false, a_formula, v_result);
      delete[] v_parts;
      v_parts = 0;

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Jitty::make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) {
      return (ATerm) ATmakeAppl4(
        ATmakeAFun("@REWR@", 4, ATfalse), f_if_then_else, a_expr, a_high, a_low);
    }

  // Class AM_Jitty - Functions declared public ---------------------------------------------------

    AM_Jitty::AM_Jitty(Rewriter* a_rewriter, ATerm_Info* a_info): ATerm_Manipulator(a_rewriter, a_info) {
      f_true =  a_rewriter->toRewriteFormat(gsMakeOpIdTrue());
      f_false = a_rewriter->toRewriteFormat(gsMakeOpIdFalse());
      f_if_then_else = ATgetArgument((ATermAppl) a_rewriter->toRewriteFormat(gsMakeOpIdIf(gsMakeSortExprBool())), 0);
    }

    // --------------------------------------------------------------------------------------------

    AM_Jitty::~AM_Jitty() {
      // There's nothing to free here, since there weren't
      // any new objects created in the constructor.
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Jitty::make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) {
      if (a_high == a_low) {
        return a_high;
      } else {
        return make_if_then_else(a_expr, a_high, a_low);
      }
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Jitty::orient(ATerm a_term) {
      ATerm v_result;

      v_result = ATtableGet(f_orient, a_term);
      if (v_result) {
        return v_result;
      }

      Symbol v_symbol;
      ATerm v_function;
      int v_arity;

      v_symbol = ATgetSymbol(a_term);
      v_function = ATgetArgument(a_term, 0);
      v_arity = ATgetArity(v_symbol);

      ATerm* v_parts;
      int i;

      v_parts = new ATerm[v_arity + 1];
      v_parts[0] = v_function;
      for (i = 1; i < v_arity; i++) {
        v_parts[i] = orient(ATgetArgument(a_term, i));
      }
      v_result = (ATerm) ATmakeApplArray(v_symbol, v_parts);
      delete[] v_parts;
      v_parts = 0;

      if (f_info->is_equality(v_result)) {
        ATerm v_term1;
        ATerm v_term2;

        v_term1 = ATgetArgument(v_result, 1);
        v_term2 = ATgetArgument(v_result, 2);
        if (f_info->compare_term(v_term1, v_term2) == compare_result_bigger) {
          v_result = (ATerm) ATmakeAppl3(v_symbol, v_function, v_term2, v_term1);
        }
      }
      ATtablePut(f_orient, a_term, v_result);

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Jitty::set_true(ATerm a_formula, ATerm a_guard) {
      ATerm v_result;
      f_set_true = ATtableCreate(2047, 50);
      v_result = set_true_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_true);
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Jitty::set_false(ATerm a_formula, ATerm a_guard) {
      ATerm v_result;
      f_set_false = ATtableCreate(2047, 50);
      v_result = set_false_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_false);
      return v_result;
    }

// Class AM_Inner ---------------------------------------------------------------------------------
  // Class AM_Inner - Functions declared protected ------------------------------------------------

    ATerm AM_Inner::set_true_auxiliary(ATerm a_formula, ATerm a_guard) {
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_true;
      }

      bool v_is_equality;

      v_is_equality = f_info->is_equality(a_guard);
      if (v_is_equality && ATelementAt((ATermList) a_guard, 2) == a_formula) {
        return ATelementAt((ATermList) a_guard, 1);
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_true, a_formula);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_formula) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_formula, 0);
        v_length = ATgetLength((ATermList) a_formula);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = set_true_auxiliary(ATelementAt((ATermList) a_formula, i), a_guard);
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        ATtablePut(f_set_true, a_formula, v_result);
        delete[] v_parts;
        v_parts = 0;
      } else {
        v_result = a_formula;
      }
      return v_result;
    }

  // ----------------------------------------------------------------------------------------------

    ATerm AM_Inner::set_false_auxiliary(ATerm a_formula, ATerm a_guard) {
      if (a_formula == f_true || a_formula == f_false) {
        return a_formula;
      }
      if (a_formula == a_guard) {
        return f_false;
      }
      if (f_info->is_variable(a_formula)) {
        return a_formula;
      }

      ATerm v_result;

      v_result = ATtableGet(f_set_false, a_formula);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_formula) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_formula, 0);
        v_length = ATgetLength((ATermList) a_formula);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = set_false_auxiliary(ATelementAt((ATermList) a_formula, i), a_guard);
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        ATtablePut(f_set_false, a_formula, v_result);
        delete[] v_parts;
        v_parts = 0;
      } else {
        v_result = a_formula;
      }
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Inner::make_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) {
      return (ATerm) ATmakeList4(f_if_then_else, a_expr, a_high, a_low);
    }

  // Class AM_Inner - Functions declared public ---------------------------------------------------

    AM_Inner::AM_Inner(Rewriter* a_rewriter, ATerm_Info* a_info): ATerm_Manipulator(a_rewriter, a_info) {
      f_true =  a_rewriter->toRewriteFormat(gsMakeOpIdTrue());
      f_false = a_rewriter->toRewriteFormat(gsMakeOpIdFalse());
      f_if_then_else = a_rewriter->toRewriteFormat(gsMakeOpIdIf(gsMakeSortExprBool()));
    }

    // --------------------------------------------------------------------------------------------

    AM_Inner::~AM_Inner() {
      // There's nothing to free here, since there weren't 
      // any new objects created in the constructor.
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Inner::make_reduced_if_then_else(ATerm a_expr, ATerm a_high, ATerm a_low) {
      if (a_high == a_low) {
        return a_high;
      } else {
        return make_if_then_else(a_expr, a_high, a_low);
      }
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Inner::orient(ATerm a_term) {
      ATerm v_result;

      v_result = ATtableGet(f_orient, a_term);
      if (v_result) {
        return v_result;
      }

      if (ATgetType(a_term) == AT_LIST) {
        ATerm v_function;
        int v_length;

        v_function = ATelementAt((ATermList) a_term, 0);
        v_length = ATgetLength((ATermList) a_term);

        ATerm* v_parts;
        int i;

        v_parts = new ATerm[v_length];
        v_parts[0] = v_function;
        for (i = 1; i < v_length; i++) {
          v_parts[i] = orient(ATelementAt((ATermList) a_term, i));
        }
        v_result = (ATerm) ATmakeList0();
        for (i = v_length - 1; i >= 0; i--) {
          v_result = (ATerm) ATinsert((ATermList) v_result, v_parts[i]);
        }
        delete[] v_parts;
        v_parts = 0;

        if (f_info->is_equality(v_result)) {
          ATerm v_term1;
          ATerm v_term2;

          v_term1 = ATelementAt((ATermList) v_result, 1);
          v_term2 = ATelementAt((ATermList) v_result, 2);
          if (f_info->compare_term(v_term1, v_term2) == compare_result_bigger) {
            v_result = (ATerm) ATmakeList3(v_function, v_term2, v_term1);
          }
        }
      } else {
        v_result = a_term;
      }
      ATtablePut(f_orient, a_term, v_result);

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Inner::set_true(ATerm a_formula, ATerm a_guard) {
      ATerm v_result;
      f_set_true = ATtableCreate(2047, 50);
      v_result = set_true_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_true);
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATerm AM_Inner::set_false(ATerm a_formula, ATerm a_guard) {
      ATerm v_result;
      f_set_false = ATtableCreate(2047, 50);
      v_result = set_false_auxiliary(a_formula, a_guard);
      ATtableDestroy(f_set_false);
      return v_result;
    }

