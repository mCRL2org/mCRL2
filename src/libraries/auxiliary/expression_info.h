// Interface to classes Expression_Info
// file: expression_info.h

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "aterm2.h"

class Expression_Info {
  private:
    ATermAppl f_greater_than;
    ATermAppl f_greater_than_or_equal;
    ATermAppl f_less_than;
    ATermAppl f_less_than_or_equal;
    ATermAppl f_plus;
    ATermAppl f_unary_minus;
    ATermAppl f_binary_minus;
    ATermAppl f_multiplication;
    ATermAppl f_add_c;
    ATermAppl f_true;
    ATermAppl f_false;
  public:
    Expression_Info();
    bool is_not(ATermAppl a_expression);
    bool is_equality(ATermAppl a_expression);
    bool is_inequality(ATermAppl a_expression);
    bool is_greater_than(ATermAppl a_expression);
    bool is_greater_than_or_equal(ATermAppl a_expression);
    bool is_less_than(ATermAppl a_expression);
    bool is_less_than_or_equal(ATermAppl a_expression);
    bool is_plus(ATermAppl a_expression);
    bool is_unary_minus(ATermAppl a_expression);
    bool is_binary_minus(ATermAppl a_expression);
    bool is_multiplication(ATermAppl a_expression);
    bool is_add_c(ATermAppl a_expression);
    bool is_operator(ATermAppl a_expression);
    bool is_variable(ATermAppl a_expression);
    bool is_constant(ATermAppl a_expression);
    bool is_int_constant(ATermAppl a_expression);
    bool is_nat_constant(ATermAppl a_expression);
    bool is_pos_constant(ATermAppl a_expression);
    bool is_true(ATermAppl a_expression);
    bool is_false(ATermAppl a_expression);
    int get_number_of_arguments(ATermAppl a_expression);
    ATermAppl get_sort_of_variable(ATermAppl a_expression);
    ATermAppl get_sort_of_operator(ATermAppl a_expression);
    ATermAppl get_argument(ATermAppl a_expression, int a_number);
    ATermAppl get_operator(ATermAppl a_expression);
};

#endif
