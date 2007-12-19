// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/expression_info.h
/// \brief Add your file description here.

// Interface to classes Expression_Info
// file: expression_info.h

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "mcrl2/core/struct.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace mcrl2::utilities;

/// \brief Extracts information from data expressions
/// The class Expression_Info provides information about the structure of data expressions.
class Expression_Info {

  public:

    /// \brief Indicates whether or not the expression has a main operator.
    inline bool is_operator(ATermAppl a_expression) {
      return (gsIsOpId(ATAgetArgument(a_expression, 0)) &&
                  (ATgetLength(gsGetDataExprArgs(a_expression)) != 0));
    }

    /// \brief Returns an argument of the main operator of an expression.
    inline ATermAppl get_argument(ATermAppl a_expression, const size_t a_number) {
      ATermList v_arguments = gsGetDataExprArgs(a_expression);

      for (size_t i = 0; i < a_number; ++i) {
        v_arguments = ATgetNext(v_arguments);
      }

      return ATAgetFirst(v_arguments);
    }

    /// \brief Returns the main operator of an expression.
    inline ATermAppl get_operator(ATermAppl a_expression) {
      ATermAppl v_result = a_expression;

      while (!gsIsOpId(v_result)) {
        v_result = ATAgetArgument(v_result, 0);
      }

      return v_result;
    }
};

#endif
