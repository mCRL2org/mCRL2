// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/expression_info.h
/// \brief Interface to classes Expression_Info

#ifndef EXPRESSION_INFO_H
#define EXPRESSION_INFO_H

#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/aterm_ext.h"


/// \brief Extracts information from data expressions.
/// The class Expression_Info provides information about the structure of data expressions.
class Expression_Info {

  public:

    /// \brief Indicates whether or not the expression has a main operator.
    inline bool is_operator(ATermAppl a_expression) {
      return (mcrl2::core::detail::gsIsOpId(mcrl2::core::ATAgetArgument(a_expression, 0)) &&
                  (ATgetLength(mcrl2::core::detail::gsGetDataExprArgs(a_expression)) != 0));
    }

    /// \brief Returns an argument of the main operator of an expression.
    inline ATermAppl get_argument(ATermAppl a_expression, const size_t a_number) {
      ATermList v_arguments = mcrl2::core::detail::gsGetDataExprArgs(a_expression);

      for (size_t i = 0; i < a_number; ++i) {
        v_arguments = ATgetNext(v_arguments);
      }

      return mcrl2::core::ATAgetFirst(v_arguments);
    }

    /// \brief Returns the main operator of an expression.
    inline ATermAppl get_operator(ATermAppl a_expression) {
      ATermAppl v_result = a_expression;

      while (!mcrl2::core::detail::gsIsOpId(v_result)) {
        v_result = mcrl2::core::ATAgetArgument(v_result, 0);
      }

      return v_result;
    }
};

#endif
