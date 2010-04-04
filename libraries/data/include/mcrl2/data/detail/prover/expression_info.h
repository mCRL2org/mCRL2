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

#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/application.h"


/// \brief Extracts information from data expressions.
/// The class Expression_Info provides information about the structure of data expressions.
class Expression_Info {

  public:

    /// \brief Indicates whether or not the expression has a main operator.
    inline bool is_operator(ATermAppl a_expression) {
      mcrl2::data::data_expression a(a_expression);
      return mcrl2::data::is_application(a);
    }

    /// \brief Returns an argument of the main operator of an expression.
    inline ATermAppl get_argument(ATermAppl a_expression, const size_t a_number) {
      mcrl2::data::data_expression a(a_expression);
      assert(is_application(a));
      mcrl2::data::data_expression_list::const_iterator i = mcrl2::data::application(a).arguments().begin();
      for(size_t j = 0; j < a_number; ++j)
      {
        ++i;
      }

      return *i;
    }

    /// \brief Returns the main operator of an expression.
    inline ATermAppl get_operator(ATermAppl a_expression) {
      assert(mcrl2::data::is_application(a_expression));
      return mcrl2::data::application(mcrl2::data::data_expression(a_expression)).head();
    }
};

#endif
