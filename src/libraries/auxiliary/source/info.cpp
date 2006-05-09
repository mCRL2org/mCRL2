// Implementation of classes ATerm_Info, AI_Jitty and AI_Inner
// file: info.cpp

#include "aterm2.h"
#include "librewrite.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "info.h"
#include "utilities.h"

// Class ATerm_Info -------------------------------------------------------------------------------
  // Class ATerm_Info - Functions declared public -------------------------------------------------

    ATerm_Info::ATerm_Info(Rewriter* a_rewriter) {
      // pre: true
      // post: f_rewriter == a_rewriter
      f_rewriter = a_rewriter;
    }

    // --------------------------------------------------------------------------------------------

    ATerm_Info::~ATerm_Info() {
      // Nothing to free here.
    }

    // --------------------------------------------------------------------------------------------

    void ATerm_Info::set_reverse(bool a_reverse) {
      f_reverse = a_reverse;
    }

    // --------------------------------------------------------------------------------------------

    void ATerm_Info::set_full(bool a_full) {
      f_full = a_full;
    }


    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_guard(ATerm a_guard1, ATerm a_guard2) {
      return lexico(
               lexico(
                 compare_guard_structure(a_guard1, a_guard2),
                 compare_guard_equation(a_guard1, a_guard2)
               ),
               compare_address(a_guard1, a_guard2)
             );
    }

    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_term(ATerm a_term1, ATerm a_term2) {
      return lexico(
               lexico(
                 compare_term_occurs(a_term1, a_term2),
                 compare_term_type(a_term1, a_term2)
               ),
               compare_address(a_term1, a_term2)
             );
    }

  // Class ATerm_Info - Functions declared protected ----------------------------------------------

    int ATerm_Info::get_guard_structure(ATerm a_guard) {
      if (is_variable(a_guard)) {
        return 0;
      }
      if (is_equation(a_guard)) {
        ATerm v_term1, v_term2;

        v_term1 = get_argument(a_guard, 0);
        v_term2 = get_argument(a_guard, 1);
        if (is_variable(v_term1) && is_variable(v_term2)) {
          return 1;
        }
        return 2;
      }
      return 3;
    }

    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_guard_structure(ATerm a_guard1, ATerm a_guard2) {
      if (get_guard_structure(a_guard1) < get_guard_structure(a_guard2)) {
        return compare_result_smaller;
      }
      if (get_guard_structure(a_guard1) > get_guard_structure(a_guard2)) {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_guard_equation(ATerm a_guard1, ATerm a_guard2) {
      if (f_full && is_equation(a_guard1) && is_equation(a_guard2)) {
        ATerm v_g1a0, v_g1a1, v_g2a0, v_g2a1;

        v_g1a0 = get_argument(a_guard1, 0);
        v_g1a1 = get_argument(a_guard1, 1);
        v_g2a0 = get_argument(a_guard2, 0);
        v_g2a1 = get_argument(a_guard2, 1);
        if (f_reverse) {
          return lexico(compare_term(v_g1a1, v_g2a1), compare_term(v_g1a0, v_g2a0));
        } else {
          return lexico(compare_term(v_g1a0, v_g2a0), compare_term(v_g1a1, v_g2a1));
        }
      }
      return compare_result_equal;
    }

    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_term_type(ATerm a_term1, ATerm a_term2) {
      if (is_variable(a_term1) && !is_variable(a_term2)) {
        return compare_result_bigger;
      }
      if (!is_variable(a_term1) && is_variable(a_term2)) {
        return compare_result_smaller;
      }
      return compare_result_equal;
    }

    // --------------------------------------------------------------------------------------------

    Compare_Result ATerm_Info::compare_term_occurs(ATerm a_term1, ATerm a_term2) {
      if (gsOccurs(a_term1, a_term2)) {
        return compare_result_smaller;
      }
      if (gsOccurs(a_term2, a_term1)) {
        return compare_result_bigger;
      }
      return compare_result_equal;
    }

// Class AI_Jitty ---------------------------------------------------------------------------------
  // Class AI_Jitty - Functions declared public ---------------------------------------------------

    AI_Jitty::AI_Jitty(Rewriter* a_rewriter): ATerm_Info(a_rewriter) {
      // pre: true
      // post: f_true, f_false, f_if_then_else_bool and f_eq are initialized
      f_true = (ATerm) ATgetArgument((ATermAppl) f_rewriter->toRewriteFormat(gsMakeOpIdTrue()), 0);
      f_false = (ATerm) ATgetArgument((ATermAppl) f_rewriter->toRewriteFormat(gsMakeOpIdFalse()), 0);
      f_if_then_else_bool = (ATerm) ATgetArgument((ATermAppl) f_rewriter->toRewriteFormat(gsMakeOpIdIf(gsMakeSortExprBool())), 0);
      f_eq = (ATerm) gsMakeOpIdNameEq();
    }

    // --------------------------------------------------------------------------------------------

    AI_Jitty::~AI_Jitty() {
      // Nothing to free here.
    }

    // --------------------------------------------------------------------------------------------

    ATerm AI_Jitty::get_argument(ATerm a_term, int a_number) {
      // pre: get_number_of_arguments(a_term) > a_number
      // ret: argument with number a_number of a_term
      return ATgetArgument(a_term, a_number + 1);
    }

    // --------------------------------------------------------------------------------------------

    int AI_Jitty::get_number_of_arguments(ATerm a_term) {
      // pre: true
      // ret: 0,                             if a_term is true, false or a variable
      //      number of arguments of a_term, otherwise
      if (!is_true(a_term) && !is_false(a_term) && !is_variable(a_term)) {
        Symbol v_symbol;

        v_symbol = ATgetSymbol(a_term);
        return ATgetArity(v_symbol) - 1;
      } else {
        return 0;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::has_type_bool(ATerm a_term) {
      // pre: true
      // ret: true,  if a_term is of type bool
      //      false, otherwise
      if (gsIsDataVarId((ATermAppl) a_term) || gsIsOpId((ATermAppl) a_term)) {
        ATerm v_term;

        v_term = ATgetArgument(a_term, 1);
        return (v_term == (ATerm) gsMakeSortIdBool());
      }

      int v_number_of_arguments;

      v_number_of_arguments = get_number_of_arguments(a_term);
      if (v_number_of_arguments == 0) {
        ATerm v_term;

        v_term = (ATerm) f_rewriter->fromRewriteFormat(a_term);
        if (gsIsDataVarId((ATermAppl) v_term) || gsIsOpId((ATermAppl) v_term)) {
          v_term = ATgetArgument(v_term, 1);
          return (v_term == (ATerm) gsMakeSortIdBool());
        } else {
          return false;
        }
      } else {
        ATerm v_term;

        v_term = ATgetArgument(a_term, 0);
        v_term = (ATerm) ATmakeAppl1(ATmakeAFun("wrap", 1, ATfalse), v_term);
        v_term = (ATerm) f_rewriter->fromRewriteFormat(v_term);
        if (gsIsOpId((ATermAppl) v_term)) {
          v_term = ATgetArgument(v_term, 1);
          while (v_number_of_arguments != 0) {
            v_term = ATgetArgument(v_term, 1);
            v_number_of_arguments--;
          }
          return (v_term == (ATerm) gsMakeSortIdBool());
        } else {
          return false;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::is_true(ATerm a_term) {
      ATerm v_term;

      v_term = ATgetArgument(a_term, 0);
      return (v_term == f_true);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::is_false(ATerm a_term) {
      ATerm v_term;

      v_term = ATgetArgument(a_term, 0);
      return (v_term == f_false);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::is_if_then_else_bool(ATerm a_term) {
      ATerm v_function;

      v_function = ATgetArgument(a_term, 0);
      return (v_function == f_if_then_else_bool && get_number_of_arguments(a_term) == 3);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::is_variable(ATerm a_term) {
      return gsIsDataVarId((ATermAppl) a_term);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Jitty::is_equation(ATerm a_term) {
      if (get_number_of_arguments(a_term) == 2) {
        ATerm v_term;

        v_term = ATgetArgument(a_term, 0);
        v_term = (ATerm) ATmakeAppl1(ATmakeAFun("wrap", 1, ATfalse), v_term);
        v_term = (ATerm) f_rewriter->fromRewriteFormat(v_term);
        v_term = ATgetArgument(v_term, 0);
        return (v_term == f_eq);
      } else {
        return false;
      }
    }

// Class AM_Inner ---------------------------------------------------------------------------------
  // Class AM_Inner - Functions declared public ---------------------------------------------------

    AI_Inner::AI_Inner(Rewriter* a_rewriter): ATerm_Info(a_rewriter) {
      f_true = f_rewriter->toRewriteFormat(gsMakeOpIdTrue());
      f_false = f_rewriter->toRewriteFormat(gsMakeOpIdFalse());
      f_if_then_else_bool = f_rewriter->toRewriteFormat(gsMakeOpIdIf(gsMakeSortExprBool()));
      f_eq = (ATerm) gsMakeOpIdNameEq();
    }

    // --------------------------------------------------------------------------------------------

    AI_Inner::~AI_Inner() {
      // There's nothing to free here, since there weren't any
      // new objects created in the constructor.
    }

    // --------------------------------------------------------------------------------------------

    ATerm AI_Inner::get_argument(ATerm a_term, int a_number) {
      return ATelementAt((ATermList) a_term, a_number + 1);
    }

    // --------------------------------------------------------------------------------------------

    int AI_Inner::get_number_of_arguments(ATerm a_term) {
      if (!is_true(a_term) && !is_false(a_term) && !is_variable(a_term) && ATgetType(a_term) == AT_LIST) {
        return ATgetLength((ATermList) a_term) - 1;
      } else {
        return 0;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::has_type_bool(ATerm a_term) {
      // pre: true
      // ret: true,  if a_term is of type bool
      //      false, otherwise
      if (gsIsDataVarId((ATermAppl) a_term) || gsIsOpId((ATermAppl) a_term)) {
        ATerm v_term;

        v_term = ATgetArgument(a_term, 1);
        return (v_term == (ATerm) gsMakeSortIdBool());
      }

      int v_number_of_arguments;

      v_number_of_arguments = get_number_of_arguments(a_term);
      if (v_number_of_arguments == 0) {
        ATerm v_term;

        v_term = (ATerm) f_rewriter->fromRewriteFormat(a_term);
        if (gsIsDataVarId((ATermAppl) v_term) || gsIsOpId((ATermAppl) v_term)) {
          v_term = ATgetArgument(v_term, 1);
          return (v_term == (ATerm) gsMakeSortIdBool());
        } else {
          return false;
        }
      } else {
        ATerm v_term;

        v_term = ATgetFirst((ATermList) a_term);
        v_term = (ATerm) f_rewriter->fromRewriteFormat(v_term);
        if (gsIsOpId((ATermAppl) v_term)) {
          v_term = ATgetArgument(v_term, 1);
          while (v_number_of_arguments != 0) {
            v_term = ATgetArgument(v_term, 1);
            v_number_of_arguments--;
          }
          return (v_term == (ATerm) gsMakeSortIdBool());
        } else {
          return false;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::is_true(ATerm a_term) {
      return (a_term == f_true);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::is_false(ATerm a_term) {
      return (a_term == f_false);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::is_if_then_else_bool(ATerm a_term) {
      if (ATgetType(a_term) == AT_LIST) {
        ATerm v_term;

        v_term = ATgetFirst((ATermList) a_term);
        return (v_term == f_if_then_else_bool && get_number_of_arguments(a_term) == 3);
      } else {
        return false;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::is_variable(ATerm a_term) {
      return gsIsDataVarId((ATermAppl) a_term);
    }

    // --------------------------------------------------------------------------------------------

    bool AI_Inner::is_equation(ATerm a_term) {
      if (get_number_of_arguments(a_term) == 2) {
        ATerm v_term;

        v_term = ATgetFirst((ATermList) a_term);
        v_term = (ATerm) f_rewriter->fromRewriteFormat(v_term);
        v_term = ATgetArgument(v_term, 0);
        return (v_term == f_eq);
      } else {
        return false;
      }
    }
