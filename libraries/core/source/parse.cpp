// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parse.cpp

#include "mcrl2/core/detail/dparser_functions.h"
#include "mcrl2/core/dparser.h"
#include "mcrl2/exception.h"
#include "mcrl2/utilities/logger.h"
#include "d.h"
#include <iostream>
#include <string>
#include <sstream>

extern "C"
{
  extern D_ParserTables parser_tables_mcrl2;
}

namespace mcrl2 {

namespace core {

namespace detail {

/// \brief Function for resolving parser ambiguities.
struct D_ParseNode* ambiguity_fn(struct D_Parser * /*p*/, int n, struct D_ParseNode **v)
{
  core::parser_table table(parser_tables_mcrl2);
  if (n == 2)
  {
    // "(" ActFrm ")" can be parsed either as a new ActFrm, or as a RegFrm. We
    // choose to parse it as an ActFrm if this ambiguity occurs, as it is the
    // most specific. We do this by checking that one of the possible parse
    // trees is of the form RegFrm("(", x, y), and not choosing that particular
    // parse tree.
    core::parse_node vi(v[0]);
    if (table.symbol_name(vi) == "RegFrm" && vi.child_count() == 3 &&
        vi.child(0).string() == "(")
    {
      return v[1];
    }
    vi.node = v[1];
    if (table.symbol_name(vi) == "RegFrm" && vi.child_count() == 3 &&
        vi.child(0).string() == "(")
    {
      return v[0];
    }
  }

  // If we reach this point, then the ambiguity is unresolved. We print all
  // ambiguities on the debug output, then throw an exception.
  for (int i = 0; i < n; ++i)
  {
    core::parse_node vi(v[i]);
    mCRL2log(log::verbose, "parser") << "Ambiguity: " << vi.tree() << std::endl;
    mCRL2log(log::debug, "parser") << "Ambiguity: " << table.tree(vi) << std::endl;
  }
  throw mcrl2::runtime_error("Unresolved ambiguity.");
}

void syntax_error_fn(struct D_Parser *ap)
{
  core::detail::increment_dparser_error_message_count();
  if (core::detail::get_dparser_error_message_count() > core::detail::get_dparser_max_error_message_count())
  {
    return;
  }
  Parser *p = (Parser *) ap;
  std::string filename;
  if (p->user.loc.pathname)
  {
    filename = std::string(p->user.loc.pathname);
  }
  std::string after;
  ZNode *z = p->snode_hash.last_all ? p->snode_hash.last_all->zns.v[0] : 0;
  while (z && z->pn->parse_node.start_loc.s == z->pn->parse_node.end)
  {
    z = (z->sns.v && z->sns.v[0]->zns.v) ? z->sns.v[0]->zns.v[0] : 0;
  }
  if (z && z->pn->parse_node.start_loc.s != z->pn->parse_node.end)
  {
    after = std::string(z->pn->parse_node.start_loc.s, z->pn->parse_node.end);
  }
  mCRL2log(log::error, "parser") << filename << "line " << p->user.loc.line << " col " << p->user.loc.col << ": syntax error";
  if (!after.empty())
  {
    mCRL2log(log::error, "parser") << " after '" << after << "'";
  }
  mCRL2log(log::error, "parser") << std::endl;
}

} // namespace detail

} // namespace core

} // namespace mcrl2
