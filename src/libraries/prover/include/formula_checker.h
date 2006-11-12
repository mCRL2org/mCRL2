// Interface to class Formula_Checker
// file: formula_checker.h

#ifndef FORMULA_CHECKER_H
#define FORMULA_CHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "prover/bdd_prover.h"
#include "auxiliary/bdd2dot.h"

  /// \brief The class formula checker takes a data specification in mCRL2 format and a list of expressions
  /// \brief of sort Bool in the mCRL2 format and determines whether or not these expersions are tautologies or
  /// \brief contradictions.

class Formula_Checker {
  private:
    /// \brief BDD based prover.
    BDD_Prover f_bdd_prover;

    /// \brief Class that outputs BDDs in dot format.
    BDD2Dot f_bdd2dot;

    /// \brief Flag indicating whether or not counter-examples are displayed.
    bool f_counter_example;

    /// \brief Flag indicating whether or not witnesses are displayed.
    bool f_witness;

    /// \brief Prefix for the names of the files containing BDDs in dot format.
    char* f_dot_file_name;

    /// \brief Displays a witness.
    void print_witness();

    /// \brief Displays a counter-example.
    void print_counter_example();

    /// \brief Writes the BDD corresponding to the formula with number a_formula_number to a dot file.
    void save_dot_file(int a_formula_number);
  public:
    /// \brief Constructor that initializes Formula_Checker::f_counter_example, Formula_Checker::f_witness,
    /// \brief Formula_Checker::f_bdd_prover and Formula_Checker::f_dot_file_name.
    Formula_Checker(
      ATermAppl a_lpe,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false,
      bool a_counter_example = false,
      bool a_witness = false,
      char* a_dot_file_name = 0
    );

    /// \brief Destructor without any specific functionality.
    ~Formula_Checker();

    /// \brief Checks the formulas in the list a_formulas.
    void check_formulas(ATermList a_formulas);
};

#endif
