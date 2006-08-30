// Interface to classes Expression_Info
// file: expression_info.h

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "aterm2.h"

  /// The class Expression_Info provides information about the structure of data expressions.

class Expression_Info {
  private:
    /// \brief ATermAppl representing the \c not function.
    ATermAppl f_not;

    /// \brief ATermAppl representing the \c equality function.
    ATermAppl f_equality;

    /// \brief ATermAppl representing the \c inequality function.
    ATermAppl f_inequality;

    /// \brief ATermAppl representing the \c greater \c than function.
    ATermAppl f_greater_than;

    /// \brief ATermAppl representing the \c greater \c than \c or \c equal function.
    ATermAppl f_greater_than_or_equal;

    /// \brief ATermAppl representing the \c less \c than function.
    ATermAppl f_less_than;

    /// \brief ATermAppl representing the \c less \c than \c or \c equal function.
    ATermAppl f_less_than_or_equal;

    /// \brief ATermAppl representing the \c plus function.
    ATermAppl f_plus;

    /// \brief ATermAppl representing the \c unary \c minus function.
    ATermAppl f_unary_minus;

    /// \brief ATermAppl representing the \c binary \c minus function.
    ATermAppl f_binary_minus;

    /// \brief ATermAppl representing the \c multiplication function.
    ATermAppl f_multiplication;

    /// \brief ATermAppl representing the \c max function.
    ATermAppl f_max;

    /// \brief ATermAppl representing the \c min function.
    ATermAppl f_min;

    /// \brief ATermAppl representing the \c abs function.
    ATermAppl f_abs;

    /// \brief ATermAppl representing the \c succ function.
    ATermAppl f_succ;

    /// \brief ATermAppl representing the \c pred function.
    ATermAppl f_pred;

    /// \brief ATermAppl representing the \c AddC function.
    ATermAppl f_add_c;

    /// \brief ATermAppl representing the \c cNat function.
    ATermAppl f_c_nat;

    /// \brief ATermAppl representing the \c cInt function.
    ATermAppl f_c_int;

    /// \brief ATermAppl representing the \c cReal function.
    ATermAppl f_c_real;

    /// \brief ATermAppl representing the constant \c true.
    ATermAppl f_true;

    /// \brief ATermAppl representing the constant \c false.
    ATermAppl f_false;
  public:
    /// \brief Constructor initializing all fields.
    Expression_Info();

    /// \brief Indicates whether or not the main operator of an expression is the \c not function.
    bool is_not(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c equality function.
    bool is_equality(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c inequality function.
    bool is_inequality(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c greater \c than function.
    bool is_greater_than(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c greater \c than \c or \c equal function.
    bool is_greater_than_or_equal(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c less \c than function.
    bool is_less_than(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c less \c than \c or \c equal function.
    bool is_less_than_or_equal(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c plus function.
    bool is_plus(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c minus function.
    bool is_unary_minus(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c binary \c minus function.
    bool is_binary_minus(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c multiplication function.
    bool is_multiplication(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c max function.
    bool is_max(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c min function.
    bool is_min(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c inequality function.
    bool is_abs(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c succ function.
    bool is_succ(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c pred function.
    bool is_pred(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c AddC function.
    bool is_add_c(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c cNat function.
    bool is_c_nat(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c cInt function.
    bool is_c_int(ATermAppl a_expression);

    /// \brief Indicates whether or not the main operator of an expression is the \c cReal function.
    bool is_c_real(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression has a main operator.
    bool is_operator(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is a single variable.
    bool is_variable(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is a single constant.
    bool is_constant(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is a single integer constant.
    bool is_int_constant(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is a single natural constant.
    bool is_nat_constant(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is a single positive constant.
    bool is_pos_constant(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is the constant true.
    bool is_true(ATermAppl a_expression);

    /// \brief Indicates whether or not the expression is the constant false.
    bool is_false(ATermAppl a_expression);

    /// \brief Returns the name of a variable.
    char* get_name_of_variable(ATermAppl a_expression);

    /// \brief Returns the name of a constant.
    char* get_name_of_constant(ATermAppl a_expression);

    /// \brief Returns the name of an operator.
    char* get_name_of_operator(ATermAppl a_expression);

    /// \brief Returns the number of arguments of the main operator of an expression.
    int get_number_of_arguments(ATermAppl a_expression);

    /// \brief Returns the sort of a variable.
    ATermAppl get_sort_of_variable(ATermAppl a_expression);

    /// \brief Returns the sort of the main operator of an expression.
    ATermAppl get_sort_of_operator(ATermAppl a_expression);

    /// \brief Returns the sort of a constant.
    ATermAppl get_sort_of_constant(ATermAppl a_expression);

    /// \brief Returns an argument of the main operator of an expression.
    ATermAppl get_argument(ATermAppl a_expression, int a_number);

    /// \brief Returns the main operator of an expression.
    ATermAppl get_operator(ATermAppl a_expression);
};

#endif
