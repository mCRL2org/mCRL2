// Implementation of class Sort_Info
// file: sort_info.cpp

#include "auxiliary/sort_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "string.h"

// Class Sort_Info --------------------------------------------------------------------------------
  // Class Sort_Info - Functions declared public --------------------------------------------------

    Sort_Info::Sort_Info() {
      f_sort_id_int = gsMakeSortIdInt();
      f_sort_id_nat = gsMakeSortIdNat();
      f_sort_id_pos = gsMakeSortIdPos();
      f_sort_id_real = gsMakeSortIdReal();
      f_sort_id_bool = gsMakeSortIdBool();
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_id(ATermAppl a_sort_expression) {
      return gsIsSortId(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_arrow_prod(ATermAppl a_sort_expression) {
      return gsIsSortArrowProd(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_int(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_int;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_nat(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_nat;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_pos(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_pos;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_real(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_real;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_bool(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_bool;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_list(ATermAppl a_sort_expression) {
      char* v_string;
      int v_result;

      v_string = gsATermAppl2String(ATAgetArgument(a_sort_expression, 0));
      v_result = strncmp(v_string, "List@", 5);
      return v_result == 0;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::returns_bool(ATermAppl a_sort_expression) {
      return gsGetSortExprResult(a_sort_expression) == f_sort_id_bool;
    }

    // --------------------------------------------------------------------------------------------

    char* Sort_Info::get_sort_id(ATermAppl a_sort_expression) {
      return gsATermAppl2String(ATAgetArgument(a_sort_expression, 0));
    }

    // --------------------------------------------------------------------------------------------

    ATermList Sort_Info::get_domain(ATermAppl a_sort_expression) {
      return gsGetSortExprDomain(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Sort_Info::get_range(ATermAppl a_sort_expression) {
      return gsGetSortExprResult(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    int Sort_Info::get_number_of_arguments(ATermAppl a_sort_expression) {
      int v_result = 0;
      if (is_sort_arrow_prod(a_sort_expression)) {
        ATermList args = ATLgetArgument(a_sort_expression, 0);
        v_result = ATgetLength(args);
      }
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Sort_Info::get_type_of_argument(ATermAppl a_sort_expression, int a_number) {
      ATermList sort_expressions = get_domain(a_sort_expression);
      return ATAelementAt(sort_expressions, a_number);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Sort_Info::get_result_type(ATermAppl a_sort_expression) {
      return ATAgetArgument(a_sort_expression, 1);
    }
