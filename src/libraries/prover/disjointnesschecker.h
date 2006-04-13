// Interface to class Disjointness_Checker
// file: disjointnesschecker.h

#ifndef DISJOINTNESSCHECKER_H
#define DISJOINTNESSCHECKER_H

#include "aterm2.h"
#include "stdlib.h"

class Disjointness_Checker {
  private:
    int f_number_of_summands;
    int f_number_of_parameters;
    ATermIndexedSet f_parameter_set;
    bool* f_used_parameters_per_summand;
    bool* f_changed_parameters_per_summand;
    void process_data_expression(int a_summand_number, ATermAppl a_expression);
    void process_multi_action(int a_summand_number, ATermAppl a_multi_action);
    void process_summand(int a_summand_number, ATermAppl a_summand);
    void process_parameters(ATermList a_parameters);
  public:
    Disjointness_Checker(ATermAppl a_process_equation);
    ~Disjointness_Checker();
    bool disjoint(int a_summand_number_1, int a_summand_number_2);
};

#endif
