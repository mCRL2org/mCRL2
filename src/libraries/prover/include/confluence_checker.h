// Interface to class Confluence_Checker
// file: confluence_checker.h

#ifndef CONFLUENCE_CHECKER_H
#define CONFLUENCE_CHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "prover/bdd_prover.h"
#include "disjointness_checker.h"
#include "invariant_checker.h"
#include "auxiliary/bdd2dot.h"

    /// \brief A class that takes a linear process equation and checks all tau-summands of that LPE for confluence.
    /// \brief The tau actions of all confluent tau-summands are renamed to ctau.

class Confluence_Checker {
  private:
    /// \brief Class that can check if two summands are disjoint.
    Disjointness_Checker f_disjointness_checker;

    /// \brief Class that checks if an invariant holds for an LPE.
    Invariant_Checker f_invariant_checker;

    /// \brief BDD based prover.
    BDD_Prover f_bdd_prover;

    /// \brief Class that prints BDDs in dot format.
    BDD2Dot f_bdd2dot;

    /// \brief A linear process equation.
    ATermAppl f_lpe;

    /// \brief Flag indicating whether or not the tau actions of confluent tau summands are renamed to ctau.
    bool f_no_marking;

    /// \brief Flag indicating whether or not the process of checking the confluence of a summand stops when
    /// \brief a summand is encountered that is not confluent with the tau summand at hand.
    bool f_check_all;

    /// \brief Flag indicating whether or not counter examples are printed.
    bool f_counter_example;

    /// \brief The prefix for the names of the files written in dot format.
    char* f_dot_file_name;

    /// \brief Flag indicating whether or not invariants are generated and checked each time a
    /// \brief summand is encountered that is not confluent with the tau summand at hand.
    bool f_generate_invariants;

    /// \brief The number of summands of the current LPE.
    int f_number_of_summands;

    /// \brief A boolean matrix, indicating which summands commute with one another.
    int* f_commutes;

    /// \brief Enables the storage of obtained results.
    bool f_enable_commutes;

    /// \brief Writes a dot file of the BDD created when checking the confluence of summands a_summand_number_1 and a_summand_number_2.
    void save_dot_file(int a_summand_number_1, int a_summand_number_2);

    /// \brief Outputs a path in the BDD corresponding to the condition at hand that leads to a node labelled false.
    void print_counter_example();

    /// \brief Checks the confluence of summand a_summand.
    ATermAppl check_confluence_and_mark_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number, bool& a_is_marked);
  public:
    /// \brief Constructor that initializes Confluence_Checker::f_lpe, Confluence_Checker::f_bdd_prover,
    /// \brief Confluence_Checker::f_generate_invariants and Confluence_Checker::f_dot_file_name.
    Confluence_Checker(
      ATermAppl a_lpe,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false,
      bool a_no_marking = false,
      bool a_check_all = false,
      bool a_counter_example = false,
      bool a_generate_invariants = false,
      char* a_dot_file_name = 0,
      bool a_enable_commutes = true
    );

    /// \brief Destructor that frees the memory used by Confluence_Checker::f_dot_file_name.
    ~Confluence_Checker();

    /// \brief Check the confluence of the LPE Confluence_Checker::f_lpe.
    ATermAppl check_confluence_and_mark(ATermAppl a_invariant, int a_summand_number);
};

#endif
