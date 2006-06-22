// Interface to class BDD2Dot
// file: bdd2dot.h

#ifndef BDD2DOT_H
#define BDD2DOT_H

#include "aterm2.h"
#include "bdd_info.h"

class BDD2Dot {
  private:
    int f_node_number;
    FILE* f_dot_file;
    ATermTable f_visited;
    BDD_Info f_bdd_info;
    void aux_output_bdd(ATermAppl a_bdd);
  public:
    void output_bdd(ATermAppl a_bdd, char* a_file_name);
};

#endif
