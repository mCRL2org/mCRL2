// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/data_parse.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DATA_PARSE_H
#define MCRL2_PBES_DATA_PARSE_H

#include <string>
#include "mcrl2/pbes/pbes_parse.h"

namespace mcrl2 {

namespace pbes_system {

  /// Parses a single data expression.
  /// \param[in] text The text that is parsed.
  /// \param[in] var_decl An optional declaration of data variables
  /// with their types.<br>
  /// An example of this is:
  /// \code
  /// datavar
  ///   n: Nat;
  ///   b: Bool;
  /// \endcode
  /// \result the parsed expression
  inline
  data::data_expression parse_data_expression(std::string text, std::string var_decl = "datavar\n")
  {
    // we need a boolean expression, so let's parse the expression x == x
    std::string expr = "val((" + text + ") == (" + text + "))";
    data::data_expression e = parse_pbes_expression(expr, var_decl + "predvar\n");
      
    // take the left hand side of e
    atermpp::aterm_list l = e(1);
    data::data_expression result = l.front();
      
    return result;
  }

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DATA_PARSE_H
