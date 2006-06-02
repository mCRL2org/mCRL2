// Interface to classes Expression_Info
// file: expression_info.h

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "aterm2.h"

class Expression_Info {
  private:
    ATermAppl f_greater_than_int;
    ATermAppl f_greater_than_real;
    ATermAppl f_greater_than_or_equal_int;
    ATermAppl f_greater_than_or_equal_real;
    ATermAppl f_less_than_int;
    ATermAppl f_less_than_real;
    ATermAppl f_less_than_or_equal_int;
    ATermAppl f_less_than_or_equal_real;
    ATermAppl f_plus_int;
    ATermAppl f_plus_real;
    ATermAppl f_unary_minus_int;
    ATermAppl f_unary_minus_real;
    ATermAppl f_binary_minus_int;
    ATermAppl f_binary_minus_real;
    ATermAppl f_multiplication_int;
    ATermAppl f_multiplication_real;
  public:
    Expression_Info();
    bool is_not(ATermAppl a_expression);
    bool is_equality(ATermAppl a_expression);
    bool is_inequality(ATermAppl a_expression);
    bool is_greater_than_int(ATermAppl a_expression);
    bool is_greater_than_real(ATermAppl a_expression);
    bool is_greater_than_or_equal_int(ATermAppl a_expression);
    bool is_greater_than_or_equal_real(ATermAppl a_expression);
    bool is_less_than_int(ATermAppl a_expression);
    bool is_less_than_real(ATermAppl a_expression);
    bool is_less_than_or_equal_int(ATermAppl a_expression);
    bool is_less_than_or_equal_real(ATermAppl a_expression);
    bool is_plus_int(ATermAppl a_expression);
    bool is_plus_real(ATermAppl a_expression);
    bool is_unary_minus_int(ATermAppl a_expression);
    bool is_unary_minus_real(ATermAppl a_expression);
    bool is_binary_minus_int(ATermAppl a_expression);
    bool is_binary_minus_real(ATermAppl a_expression);
    bool is_multiplication_int(ATermAppl a_expression);
    bool is_multiplication_real(ATermAppl a_expression);
    bool is_operator(ATermAppl a_expression);
    bool is_real_variable(ATermAppl a_expression);
    bool is_int_variable(ATermAppl a_expression);
    bool is_variable(ATermAppl a_expression);
    bool is_int_constant(ATermAppl a_expression);
    bool is_constant(ATermAppl a_expression);
    int get_number_of_arguments(ATermAppl a_expression);
    ATermAppl get_sort_of_variable(ATermAppl a_expression);
    ATermAppl get_sort_of_operator(ATermAppl a_expression);
    ATermAppl get_argument(ATermAppl a_expression, int a_number);
    ATermAppl get_operator(ATermAppl a_expression);
};

#endif
