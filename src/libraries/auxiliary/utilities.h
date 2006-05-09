// Interface of utility functions
// file: utilities.h

#ifndef UTILITIES_H
#define UTILITIES_H

#include "aterm2.h"

enum Answer {
  answer_yes,
  answer_no,
  answer_undefined
};

enum Compare_Result {
  compare_result_smaller,
  compare_result_equal,
  compare_result_bigger
};

char* bool_to_char_string(bool a_bool);

char* blank_spaces(int a_number);

int number_of_digits(int a_integer);

Compare_Result compare_address(ATerm a_term1, ATerm a_term2);

Compare_Result lexico(Compare_Result a_result1, Compare_Result a_result2);

ATerm read_ATerm_from_file(char* a_file_name, char* a_feedback_string);

void write_ATerm_to_file(char* a_file_name, ATermAppl a_term, char* a_feedback_string);

class BDD2Dot {
  private:
    ATermAppl f_true;
    ATermAppl f_false;
    ATermAppl f_if_then_else;
    int f_node_number;
    FILE* f_dot_file;
    ATermTable f_visited;
    bool is_true(ATermAppl a_term);
    bool is_false(ATermAppl a_term);
    bool is_if_then_else(ATermAppl a_term);
    ATermAppl get_guard(ATermAppl a_bdd);
    ATermAppl get_true_branch(ATermAppl a_bdd);
    ATermAppl get_false_branch(ATermAppl a_bdd);
    void aux_output_bdd(ATermAppl a_bdd);
  public:
    BDD2Dot();
    void output_bdd(ATermAppl a_bdd, char* a_file_name);
};

// Eventually, these two functions should probably be moved into libstruct

ATermAppl make_ctau_act_id();

ATermAppl make_ctau_action();

#endif
