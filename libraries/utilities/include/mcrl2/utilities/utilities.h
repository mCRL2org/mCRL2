// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/utilities.h
/// \brief Add your file description here.

// Interface to utility functions
// file: utilities.h

#ifndef UTILITIES_H
#define UTILITIES_H

#include "aterm2.h"

enum Compare_Result {
  compare_result_smaller,
  compare_result_equal,
  compare_result_bigger
};

const char* bool_to_char_string(bool a_bool);

class Indent {
  private:
    int f_indentation_level;
    void update_string();
  public:
    Indent();
    ~Indent();
    char* blank_spaces;
    void indent();
    void deindent();
};

int number_of_digits(int a_integer);

Compare_Result compare_address(ATerm a_term1, ATerm a_term2);

Compare_Result lexico(Compare_Result a_result1, Compare_Result a_result2);

ATerm read_ATerm_from_file(char const* a_file_name, char const* a_feedback_string);

void write_ATerm_to_file(char const* a_file_name, ATermAppl a_term, char const* a_feedback_string);

// Eventually, these two functions should probably be moved into libstruct

ATermAppl make_ctau_act_id();

ATermAppl make_ctau_action();

#endif
