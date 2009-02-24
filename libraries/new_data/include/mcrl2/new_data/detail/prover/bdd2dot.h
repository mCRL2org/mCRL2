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
#include "bdd_info.h"

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
    /// precondition: The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of Bool as guard, and a then-branch and an else-branch that again follow these
    /// restrictions
    void aux_output_bdd(ATermAppl a_bdd);
  public:
    /// \brief Writes the BDD it receives as input to a file
    /// \brief in dot format with the name received as input.
    /// precondition: The argument passed as parameter a_bdd is a data expression in internal mCRL2 format with the
    /// following restrictions: It either represents the constant true or the constant false, or it is an if-then-else
    /// expression with an expression of Bool as guard, and a then-branch and an else-branch that again follow these
    /// restrictions
    void output_bdd(ATermAppl a_bdd, char const* a_file_name);
};

#endif
