// Interface to classes Sort_Info
// file: sort_info.h

#ifndef SORT_INFO_H
#define SORT_INFO_H

#include "aterm2.h"

class Sort_Info {
  private:
    ATermAppl f_sort_id_int;
    ATermAppl f_sort_id_nat;
    ATermAppl f_sort_id_pos;
    ATermAppl f_sort_id_real;
    ATermAppl f_sort_id_bool;
  public:
    Sort_Info();
    bool is_sort_id(ATermAppl a_sort_expression);
    bool is_sort_arrow(ATermAppl a_sort_expression);
    bool is_sort_int(ATermAppl a_sort_expression);
    bool is_sort_nat(ATermAppl a_sort_expression);
    bool is_sort_pos(ATermAppl a_sort_expression);
    bool is_sort_real(ATermAppl a_sort_expression);
    bool returns_bool(ATermAppl a_sort_expression);
    ATermAppl get_domain(ATermAppl a_sort_expression);
    ATermAppl get_range(ATermAppl a_sort_expression);
};

#endif
