// Implementation of class Sort_Info
// file: sort_info.cpp

#include "sort_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Sort_Info --------------------------------------------------------------------------------
  // Class Sort_Info - Functions declared public --------------------------------------------------

    Sort_Info::Sort_Info() {
      f_sort_id_int = gsMakeSortIdInt();
      f_sort_id_real = gsMakeSortIdReal();
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_id(ATermAppl a_sort_expression) {
      return gsIsSortId(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_arrow(ATermAppl a_sort_expression) {
      return gsIsSortArrow(a_sort_expression);
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_int(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_int;
    }

    // --------------------------------------------------------------------------------------------

    bool Sort_Info::is_sort_real(ATermAppl a_sort_expression) {
      return a_sort_expression == f_sort_id_real;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Sort_Info::get_domain(ATermAppl a_sort_expression) {
      return ATAgetArgument(a_sort_expression, 0);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Sort_Info::get_range(ATermAppl a_sort_expression) {
      return ATAgetArgument(a_sort_expression, 1);
    }
