// Interface to class BDD2Dot
// file: bdd2dot.h

#ifndef BDD2DOT_H
#define BDD2DOT_H

#include "aterm2.h"
#include "bdd_info.h"

  /// The class BDD2Dot writes binary decision diagrams to files in dot format.

class BDD2Dot {
  private:
    /// \brief The smallest unused node number.
    int f_node_number;

    /// \brief The file the output is written to.
    FILE* f_dot_file;

    /// \brief A table containing all the visited nodes. It maps these nodes to the corresponding node numbers.
    ATermTable f_visited;

    /// \brief A class that gives information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Writes the BDD it receives to BDD2Dot::f_dot_file.
    void aux_output_bdd(ATermAppl a_bdd);
  public:
    /// \brief Writes the BDD it receives as input to a file
    /// \brief in dot format with the name received as input.
    void output_bdd(ATermAppl a_bdd, char* a_file_name);
};

#endif
