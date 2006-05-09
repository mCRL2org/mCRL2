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

  // Class BDD2Dot --------------------------------------------------------------------------------
    // Class BDD2Dot - Functions declared private -------------------------------------------------

      void BDD2Dot::aux_output_bdd(ATermAppl a_bdd) {
        ATermAppl v_true_branch, v_false_branch, v_guard;
        int v_true_number, v_false_number;

        if (ATtableGet(f_visited, (ATerm) a_bdd)) {
          return;
        }

        if (is_true(a_bdd)) {
          fprintf(f_dot_file, "  %d [shape=box, label=\"T\"];\n", f_node_number);
        } else if (is_false(a_bdd)) {
          fprintf(f_dot_file, "  %d [shape=box, label=\"F\"];\n", f_node_number);
        } else if (is_if_then_else(a_bdd)) {
          v_true_branch = get_true_branch(a_bdd);
          v_false_branch = get_false_branch(a_bdd);
          aux_output_bdd(v_true_branch);
          aux_output_bdd(v_false_branch);
          v_true_number = ATgetInt((ATermInt) ATtableGet(f_visited, (ATerm) v_true_branch));
          v_false_number = ATgetInt((ATermInt) ATtableGet(f_visited, (ATerm) v_false_branch));
          v_guard = get_guard(a_bdd);
          gsfprintf(f_dot_file, "  %d [label=\"%P\"];\n", f_node_number, v_guard);
          fprintf(f_dot_file, "  %d -> %d;\n", f_node_number, v_true_number);
          fprintf(f_dot_file, "  %d -> %d [style=dashed];\n", f_node_number, v_false_number);
        } else {
          gsfprintf(f_dot_file, "  %d [shape=box, label=\"%P\"];\n", f_node_number, a_bdd);
        }
        ATtablePut(f_visited, (ATerm) a_bdd, (ATerm) ATmakeInt(f_node_number++));
      }

      // ------------------------------------------------------------------------------------------

      ATermAppl BDD2Dot::get_guard(ATermAppl a_bdd) {
        return ATAgetArgument(ATAgetArgument(ATAgetArgument(a_bdd, 0), 0), 1);
      }

      // ------------------------------------------------------------------------------------------

      ATermAppl BDD2Dot::get_true_branch(ATermAppl a_bdd) {
        return ATAgetArgument(ATAgetArgument(a_bdd, 0), 1);
      }

      // ------------------------------------------------------------------------------------------

      ATermAppl BDD2Dot::get_false_branch(ATermAppl a_bdd) {
        return ATAgetArgument(a_bdd, 1);
      }

      // ------------------------------------------------------------------------------------------

      bool BDD2Dot::is_true(ATermAppl a_bdd) {
        return a_bdd == f_true;
      }

      // ------------------------------------------------------------------------------------------

      bool BDD2Dot::is_false(ATermAppl a_bdd) {
        return a_bdd == f_false;
      }

      // ------------------------------------------------------------------------------------------

      bool BDD2Dot::is_if_then_else(ATermAppl a_bdd) {
        ATermAppl v_term;

        if (gsIsDataAppl(a_bdd)) {
          v_term = ATAgetArgument(a_bdd, 0);
          if (gsIsDataAppl(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            if (gsIsDataAppl(v_term)) {
              v_term = ATAgetArgument(v_term, 0);
              return v_term == f_if_then_else;
            }
          }
        }
        return false;
      }

    // Class BDD2Dot - Functions declared public --------------------------------------------------

      BDD2Dot::BDD2Dot() {
        f_true = gsMakeOpIdTrue();
        f_false = gsMakeOpIdFalse();
        f_if_then_else = gsMakeOpIdIf(gsMakeSortExprBool());
      }

      // ------------------------------------------------------------------------------------------

      void BDD2Dot::output_bdd(ATermAppl a_bdd, char* a_file_name) {
        f_visited = ATtableCreate(200, 75);
        f_node_number = 0;
        f_dot_file = fopen(a_file_name, "w");
        fprintf(f_dot_file, "digraph BDD {\n");
        aux_output_bdd(a_bdd);
        fprintf(f_dot_file, "}\n");
        fclose(f_dot_file);
        ATtableDestroy(f_visited);
      }
