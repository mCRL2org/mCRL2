// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/smt_solver_cvc_fast.h
/// \brief SMT solver based on the CVC3 library

#ifdef HAVE_CVC

#ifndef SMT_SOLVER_CVC_FAST_H
#define SMT_SOLVER_CVC_FAST_H

#include "aterm2.h"
#include "smt_solver.h"
#include "mcrl2/utilities/expression_info.h"
#include "mcrl2/utilities/sort_info.h"
#include <string>
#include <vector>
#include "vc.h"

template <class Type>
class Pair {
  private:
    ATermAppl f_aterm;
    Type f_element;
  public:
    Pair(ATermAppl a_aterm, Type a_element);
    ATermAppl get_aterm();
    Type get_element();
};

template <class Type>
class List_Of_Pairs {
  private:
    std::vector< Pair<Type> > f_list_of_pairs;
  public:
    void add_pair_to_list(ATermAppl a_aterm, Type a_element);
    bool element_in_list(ATermAppl a_aterm);
    Type get_element(ATermAppl a_aterm);
};

class SMT_Solver_CVC_Fast: public SMT_Solver {
  private:
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
    CVC3::ValidityChecker* f_validity_checker;
    List_Of_Pairs<CVC3::Type> f_list_of_types;
    List_Of_Pairs<CVC3::Op> f_list_of_operators;

    ATermIndexedSet f_nat_variables;
    ATermIndexedSet f_pos_variables;

    CVC3::Type translate_sort(ATermAppl a_sort_expression);

    CVC3::Expr translate_clause(ATermAppl a_clause);

    CVC3::Expr translate_not(ATermAppl a_clause);
    CVC3::Expr translate_equality(ATermAppl a_clause);
    CVC3::Expr translate_inequality(ATermAppl a_clause);
    CVC3::Expr translate_greater_than(ATermAppl a_clause);
    CVC3::Expr translate_greater_than_or_equal(ATermAppl a_clause);
    CVC3::Expr translate_less_than(ATermAppl a_clause);
    CVC3::Expr translate_less_than_or_equal(ATermAppl a_clause);
    CVC3::Expr translate_plus(ATermAppl a_clause);
    CVC3::Expr translate_unary_minus(ATermAppl a_clause);
    CVC3::Expr translate_binary_minus(ATermAppl a_clause);
    CVC3::Expr translate_multiplication(ATermAppl a_clause);
    CVC3::Expr translate_max(ATermAppl a_clause);
    CVC3::Expr translate_min(ATermAppl a_clause);
    CVC3::Expr translate_abs(ATermAppl a_clause);
    CVC3::Expr translate_succ(ATermAppl a_clause);
    CVC3::Expr translate_pred(ATermAppl a_clause);
    CVC3::Expr translate_add_c(ATermAppl a_clause);
    CVC3::Expr translate_c_nat(ATermAppl a_clause);
    CVC3::Expr translate_c_int(ATermAppl a_clause);
    //CVC3::Expr translate_c_real(ATermAppl a_clause);
    CVC3::Expr translate_unknown_operator(ATermAppl a_clause);
    CVC3::Expr translate_variable(ATermAppl a_clause);
    CVC3::Expr translate_nat_variable(ATermAppl a_clause);
    CVC3::Expr translate_pos_variable(ATermAppl a_clause);
    CVC3::Expr translate_int_constant(ATermAppl a_clause);
    CVC3::Expr translate_nat_constant(ATermAppl a_clause);
    CVC3::Expr translate_pos_constant(ATermAppl a_clause);
    CVC3::Expr translate_true();
    CVC3::Expr translate_false();
    CVC3::Expr translate_constant(ATermAppl a_clause);
    void add_nat_clauses(std::vector<CVC3::Expr>& a_expressions);
    void add_pos_clauses(std::vector<CVC3::Expr>& a_expressions);

    CVC3::Expr f_formula;
    void translate(ATermList a_formula);
  public:
    SMT_Solver_CVC_Fast();
    virtual ~SMT_Solver_CVC_Fast();
    virtual bool is_satisfiable(ATermList a_formula);
};

#endif // SMT_SOLVER_CVC_FAST_H

#endif // HAVE_CVC
