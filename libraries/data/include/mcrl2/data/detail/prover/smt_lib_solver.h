// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/smt_lib_solver.h
/// \brief Abstract interface for SMT solvers based on the SMT-LIB format

#ifndef SMT_LIB_SOLVER_H
#define SMT_LIB_SOLVER_H

#include <string>

#include "aterm2.h"
#include "mcrl2/data/detail/prover/smt_solver.h"
#include "mcrl2/utilities/expression_info.h"
#include "mcrl2/utilities/sort_info.h"

  /// The class SMT_LIB_Solver is a base class for SMT solvers that read the SMT-LIB format
  /// [Silvio Ranise and Cesare Tinelli. The SMT-LIB Standard: Version 1.1. Technical Report, Department of Computer
  /// Science, The University of Iowa, 2005. (Available at http://goedel.cs.uiowa.edu/smtlib)]. It inherits from the class
  /// SMT_Solver.
  ///
  /// The method SMT_LIB_Solver::translate receives an expression of sort Bool in conjunctive normal form as parameter
  /// a_formula and translates it to a benchmark in SMT-LIB format. The result is saved as field std::string f_benchmark.

class SMT_LIB_Solver: public SMT_Solver {
  private:
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
    std::string f_sorts_notes;
    std::string f_operators_notes;
    std::string f_predicates_notes;
    std::string f_extrasorts;
    std::string f_operators_extrafuns;
    std::string f_variables_extrafuns;
    std::string f_extrapreds;
    std::string f_formula;
    ATermIndexedSet f_sorts;
    ATermIndexedSet f_operators;
    ATermIndexedSet f_variables;
    ATermIndexedSet f_nat_variables;
    ATermIndexedSet f_pos_variables;
    bool f_bool2pred;
    void declare_sorts();
    void declare_operators();
    void declare_variables();
    void declare_predicates();
    void produce_notes_for_sorts();
    void produce_notes_for_operators();
    void produce_notes_for_predicates();
    void translate_clause(ATermAppl a_clause, bool a_expecting_predicate);
    void add_bool2pred_and_translate_clause(ATermAppl a_clause);
    void translate_not(ATermAppl a_clause);
    void translate_equality(ATermAppl a_clause);
    void translate_inequality(ATermAppl a_clause);
    void translate_greater_than(ATermAppl a_clause);
    void translate_greater_than_or_equal(ATermAppl a_clause);
    void translate_less_than(ATermAppl a_clause);
    void translate_less_than_or_equal(ATermAppl a_clause);
    void translate_plus(ATermAppl a_clause);
    void translate_unary_minus(ATermAppl a_clause);
    void translate_binary_minus(ATermAppl a_clause);
    void translate_multiplication(ATermAppl a_clause);
    void translate_max(ATermAppl a_clause);
    void translate_min(ATermAppl a_clause);
    void translate_abs(ATermAppl a_clause);
    void translate_succ(ATermAppl a_clause);
    void translate_pred(ATermAppl a_clause);
    void translate_add_c(ATermAppl a_clause);
    void translate_c_nat(ATermAppl a_clause);
    void translate_c_int(ATermAppl a_clause);
    //void translate_c_real(ATermAppl a_clause);
    void translate_unknown_operator(ATermAppl a_clause);
    void translate_variable(ATermAppl a_clause);
    void translate_nat_variable(ATermAppl a_clause);
    void translate_pos_variable(ATermAppl a_clause);
    void translate_int_constant(ATermAppl a_clause);
    void translate_nat_constant(ATermAppl a_clause);
    void translate_pos_constant(ATermAppl a_clause);
    void translate_true();
    void translate_false();
    void translate_constant(ATermAppl a_clause);
    void add_nat_clauses();
    void add_pos_clauses();
  protected:
    std::string f_benchmark;

    /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
    /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
    /// clauses
    void translate(ATermList a_formula);
  public:
    SMT_LIB_Solver();
    virtual ~SMT_LIB_Solver();
};

namespace mcrl2 {
  namespace data {
    namespace prover {

      /**
       * Template class for SMT provers that come as an external binary and use the
       * SMT-lib format. Input to the tool is specified on standard input,
       * output is read from standard output and matches one of the strings:
       * "sat", "unsat", "unknown".
       *
       * Parameter T follows the curiously recurring template pattern (CRTP). Type T
       * is required to have the name and exec methods as in the example below.
       *
       *  \code
       *  class cvc_smt_solver : public binary_smt_solver< cvc_smt_solver > {
       *    inline static char* name() {
       *      return "CVC3";
       *    }
       *
       *    inline static void exec() {
       *      ::execlp("cvc", "cvc3", "-lang smt-lib", 0);
       *    }
       *  };
       *  \endcode
       **/
      template < typename T >
      class binary_smt_solver {

        protected:

          // \brief Calls one of the exec functions
          static bool execute(std::string const& benchmark);

        public:

          // \brief Checks the availability/usability of the prover
          static bool usable();
      };
    }
  }
}
#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
# include "mcrl2/core/messaging.h"
# include <unistd.h>

namespace mcrl2 {
  namespace data {
    namespace prover {

      /// The class inherits from the class SMT_LIB_Solver. It uses the SMT solver
      /// CVC / (http://www.cs.nyu.edu/acsys/cvcl/) to determine the satisfiability
      /// of propositional formulas. To use the solver CVC / the directory containing
      /// the corresponding executable must be in the path.
      ///
      /// The static method usable can be used to check checks if CVC's executable is indeed available.
      ///
      /// The method SMT_Solver_CVC::is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
      /// indicates whether or not this formula is satisfiable.
      class cvc_smt_solver : public SMT_LIB_Solver, public binary_smt_solver< cvc_smt_solver > {
        friend class binary_smt_solver< cvc_smt_solver >;

        private:

          inline static char const* name() {
            return "CVC3";
          }

          inline static void exec() {
            ::execlp("cvc3", "cvc3", "-lang", "smt-lib", (char*)0);
          }

        public:

          /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
          /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
          /// clauses
          bool is_satisfiable(ATermList a_formula) {
            translate(a_formula);

            return execute(f_benchmark);
          }
      };

      /// The class inherits from the class SMT_LIB_Solver. It uses the SMT solver Ario 1.1
      /// (http://www.eecs.umich.edu/~ario/) to determine the satisfiability of propositional formulas. To use the solver Ario
      /// 1.1, the directory containing the corresponding executable must be in the path.
      ///
      /// The static method usable can be used to check checks if CVC's executable is indeed available.
      ///
      /// The method is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
      /// indicates whether or not this formula is satisfiable.
      class ario_smt_solver : public SMT_LIB_Solver, public binary_smt_solver< ario_smt_solver > {
        friend class binary_smt_solver< ario_smt_solver >;

        private:

          inline static char const* name() {
            return "Ario";
          }

          inline static void exec() {
            ::execlp("ario", "ario", (char*)0);
          }

        public:

          /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
          /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
          /// clauses
          bool is_satisfiable(ATermList a_formula) {
            translate(a_formula);

            return execute(f_benchmark);
          }
      };
    }
  }
}
#endif

#endif
