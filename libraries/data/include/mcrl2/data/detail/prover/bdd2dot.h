// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/bdd2dot.h
/// \brief Interface to class BDD2Dot

#ifndef BDD2DOT_H
#define BDD2DOT_H

#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/data/detail/prover/bdd_info.h"

  /// \brief The class BDD2Dot offers the ability to write binary decision diagrams to dot files.
  /// The method BDD2Dot::output_bdd receives a binary decision diagram as parameter a_bdd and writes it to a file in dot
  /// format with the name passed as parameter a_file_name.

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
    /// \param a_bdd A binary decision diagram.
    /// \pre The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of Bool as guard, and a then-branch and an else-branch that again follow these
    /// restrictions
    void aux_output_bdd(ATermAppl a_bdd)
    {
      if (ATtableGet(f_visited, (ATerm) a_bdd)) {
        return;
      }

      if (f_bdd_info.is_true(a_bdd)) {
        fprintf(f_dot_file, "  %d [shape=box, label=\"T\"];\n", f_node_number);
      } else if (f_bdd_info.is_false(a_bdd)) {
        fprintf(f_dot_file, "  %d [shape=box, label=\"F\"];\n", f_node_number);
      } else if (f_bdd_info.is_if_then_else(a_bdd)) {
        ATermAppl v_true_branch = f_bdd_info.get_true_branch(a_bdd);
        ATermAppl v_false_branch = f_bdd_info.get_false_branch(a_bdd);
        aux_output_bdd(v_true_branch);
        aux_output_bdd(v_false_branch);
        int v_true_number = ATgetInt((ATermInt) ATtableGet(f_visited, (ATerm) v_true_branch));
        int v_false_number = ATgetInt((ATermInt) ATtableGet(f_visited, (ATerm) v_false_branch));
        ATermAppl v_guard = f_bdd_info.get_guard(a_bdd);
        mcrl2::core::gsfprintf(f_dot_file, "  %d [label=\"%P\"];\n", f_node_number, v_guard);
        fprintf(f_dot_file, "  %d -> %d;\n", f_node_number, v_true_number);
        fprintf(f_dot_file, "  %d -> %d [style=dashed];\n", f_node_number, v_false_number);
      } else {
        mcrl2::core::gsfprintf(f_dot_file, "  %d [shape=box, label=\"%P\"];\n", f_node_number, a_bdd);
      }
      ATtablePut(f_visited, (ATerm) a_bdd, (ATerm) ATmakeInt(f_node_number++));
    }

  public:
    /// \brief Writes the BDD it receives as input to a file
    /// \brief in dot format with the name received as input.
    /// precondition: The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of Bool as guard, and a then-branch and an else-branch that again follow these
    /// restrictions
    /// \param a_bdd A binary decision diagram.
    /// \param a_file_name A file name.
    void output_bdd(ATermAppl a_bdd, char const* a_file_name)
    {
      f_visited = ATtableCreate(200, 75);
      f_node_number = 0;
      f_dot_file = fopen(a_file_name, "w");
      fprintf(f_dot_file, "digraph BDD {\n");
      aux_output_bdd(a_bdd);
      fprintf(f_dot_file, "}\n");
      fclose(f_dot_file);
      ATtableDestroy(f_visited);
    }
};

#endif
