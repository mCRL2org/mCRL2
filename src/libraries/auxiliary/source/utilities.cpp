// Implementation of utilities
// file: utilities.cpp

#include "stdlib.h"
#include "utilities.h"
#include "libprint_c.h"
#include "libstruct.h"
#include <string>

using namespace std;

// General functions ------------------------------------------------------------------------------

  char* bool_to_char_string(bool a_bool) {
    if (a_bool) {
      return "true";
    } else {
      return "false";
    }
  }

  // ----------------------------------------------------------------------------------------------

  char* blank_spaces(int a_number) {
    char* v_spaces;
    int i;

    v_spaces = new char[a_number + 1];
    for (i = 0; i < a_number; i++) {
      v_spaces[i] = ' ';
    }
    v_spaces[a_number] = '\0';
    return v_spaces;
  }

  // ----------------------------------------------------------------------------------------------

  int number_of_digits(int a_integer) {
    int v_result = 0;

    while (a_integer != 0) {
      a_integer = a_integer / 10;
      v_result++;
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  Compare_Result compare_address(ATerm a_term1, ATerm a_term2) {
    long v_address1, v_address2;

    v_address1 = (long) a_term1;
    v_address2 = (long) a_term2;
    if (v_address1 < v_address2) {
      return compare_result_smaller;
    }
    if (v_address1 > v_address2) {
      return compare_result_bigger;
    }
    return compare_result_equal;
  }

  // ----------------------------------------------------------------------------------------------

  Compare_Result lexico(Compare_Result a_result1, Compare_Result a_result2) {
    if (a_result1 != compare_result_equal) {
      return a_result1;
    } else {
      return a_result2;
    }
  }

  // ----------------------------------------------------------------------------------------------

  ATerm read_ATerm_from_file(char* a_file_name, char* a_feedback_string) {
    FILE* v_stream;
    ATerm v_result;

    if (a_file_name == 0) {
      v_stream = stdin;
      gsVerboseMsg("Reading the %s from stdin...\n", a_feedback_string);
      v_result = ATreadFromFile(v_stream);
      if (v_result == 0) {
        gsErrorMsg("Could not read the %s from stdin.\n", a_feedback_string);
        exit(1);
      }
    } else {
      v_stream = fopen(a_file_name, "rb");
      if (v_stream != 0) {
        gsVerboseMsg("Reading the %s from '%s'...\n", a_feedback_string, a_file_name);
        v_result = ATreadFromFile(v_stream);
        if (v_result == 0) {
          gsErrorMsg("Could not read the %s from '%s'.", a_feedback_string, a_file_name);
          exit(1);
        }
        fclose(v_stream);
      } else {
        gsErrorMsg("Cannot open the file '%s'.\n", a_file_name);
        exit(1);
      }
    }

    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  void write_ATerm_to_file(char* a_file_name, ATermAppl a_term, char* a_feedback_string) {
    if (a_file_name == 0) {
      gsVerboseMsg("Writing the %s to stdout.\n", a_feedback_string);
      ATwriteToBinaryFile((ATerm) a_term, stdout);
      fprintf(stdout, "\n");
    } else {
      FILE* v_output_stream = fopen(a_file_name, "wb");
      if (v_output_stream == 0) {
        gsErrorMsg("Cannot open output file '%s'.\n", a_file_name);
        exit(1);
      } else {
        gsVerboseMsg("Writing the %s to '%s'.\n", a_feedback_string, a_file_name);
        ATwriteToBinaryFile((ATerm) a_term, v_output_stream);
        fclose(v_output_stream);
      }
    }
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl make_ctau_act_id() {
    return gsMakeActId(ATmakeAppl0(ATmakeAFun("ctau", 0, ATtrue)), ATmakeList0());
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl make_ctau_action() {
    return gsMakeAction(make_ctau_act_id(), ATmakeList0());
  }
