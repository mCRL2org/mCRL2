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
#include "mcrl2/utilities/logger.h" 
#include <sstream>

extern "C"
{
  extern D_ParserTables parser_tables_mcrl2;
}

namespace mcrl2 {

namespace core {

namespace detail {

/// \brief Function for resolving parser ambiguities.
struct D_ParseNode* ambiguity_fn(struct D_Parser *p, int n, struct D_ParseNode **v)
{
  // Print all ambiguities on the debug output, then throw an exception.
  for (int i = 0; i < n; ++i)
  {
    mCRL2log(log::debug, "parser") << "Ambiguity: " << core::parse_node(v[i]).tree() << std::endl;
  }
  throw mcrl2::runtime_error("Unresolved ambiguity.");
}

} // namespace detail

} // namespace core

} // namespace mcrl2
