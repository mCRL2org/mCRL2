// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse.cpp

#include "mcrl2/core/detail/dparser_ambiguity.h"
#include "mcrl2/exception.h"

extern "C"
{
  extern D_ParserTables parser_tables_mcrl2;
}

namespace mcrl2 {

namespace core {

namespace detail {

/// \brief Function for resolving ambiguities in the '_ -> _ <> _' operator for
/// process expressions.
struct D_ParseNode* ambiguity_fn(struct D_Parser *p, int n, struct D_ParseNode **v)
{
  core::parser_table table(parser_tables_mcrl2);
  // Resolve ambiguity (* is the correct alternative):
  //    ProcExpr(a, IfThen('->', ProcExpr(b, '->', c), '<>'), d))
  //  * ProcExpr(a, '->', ProcExpr(b, IfThen('->', c, '<>'), d))
  //
  // We do this by seeing if we have 2 alternatives. If this is the case, and
  // both alternatives are procexpr, then we discard an alternative if it is
  // of the form ProcExpr(a, IfThen, b); this is valid, because apparently there
  // is an alternative where the IfThen occurs deeper in the tree (the '<>'
  // symbol guarantees this).
  if (n == 2)
  {
    core::parse_node v0(v[0]);
    core::parse_node v1(v[1]);
    if (table.symbol_name(v0) == "ProcExpr" &&
        table.symbol_name(v1) == "ProcExpr" &&
        v0.child_count() == 3 &&
        v1.child_count() == 3)
    {
      if (table.symbol_name(v0.child(1)) == "IfThen")
        return v1.node;
      else
        return v0.node;
    }
  }
  // If the ambiguity has not been resolved now, then we simply don't know which
  // to choose, so we throw an error.
  throw mcrl2::runtime_error("Unresolved ambiguity.");
}

} // namespace detail

} // namespace core

} // namespace mcrl2
