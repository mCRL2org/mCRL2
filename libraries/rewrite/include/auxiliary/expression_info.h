// Interface to classes Expression_Info
// file: expression_info.h

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "aterm2.h"

  /// The class Expression_Info provides information about the structure of data expressions.

class Expression_Info {
  public:
    /// \brief Constructor initializing all fields.
    Expression_Info();

    /// \brief Indicates whether or not the expression has a main operator.
    bool is_operator(ATermAppl a_expression);

    /// \brief Returns an argument of the main operator of an expression.
    ATermAppl get_argument(ATermAppl a_expression, int a_number);

    /// \brief Returns the main operator of an expression.
    ATermAppl get_operator(ATermAppl a_expression);
};

#endif
