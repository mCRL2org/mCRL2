// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lts/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LTS_PARSE_H
#define MCRL2_LTS_PARSE_H

#include "mcrl2/core/parse.h"
#include "mcrl2/core/parser_utility.h"

namespace mcrl2 {

namespace lts {

inline
void parse_fsm_specification_new(const std::string& text)
{
  core::parser p(parser_tables_fsm);
  unsigned int start_symbol_index = p.start_symbol_index("FSM");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  p.destroy_parse_node(node);
}

} // namespace lts

} // namespace mcrl2

#endif // MCRL2_LTS_PARSE_H
