// Author(s): Jeroen Keiren, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/pg_parse.h
/// \brief Parsing of parity games in the format used by PGSolver.

#ifndef MCRL2_BES_PG_PARSE_H
#define MCRL2_BES_PG_PARSE_H

#include <fstream>
#include <cstdio>
#include <cctype>
#include <map>

#include "mcrl2/utilities/text_utility.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/core/parser_utility.h"

extern "C"
{
  extern D_ParserTables parser_tables_pg;
}

namespace mcrl2
{

namespace bes
{

typedef unsigned long long identifier_t;
typedef unsigned short priority_t;

typedef bool owner_t;

struct node_t
{
  identifier_t id;
  priority_t prio;
  owner_t owner;
  std::set<identifier_t> successors;

  bool operator<(node_t const& other)
  {
    return id < other.id;
  }
};

// Build a formula from the strings in v. if p = 0, than a disjunction is built,
// otherwise the result is a conjunction.
// Prefix is added to each of the identifiers in v.
inline
boolean_expression formula(std::set<identifier_t> const& v, const owner_t owner, std::string prefix = "X")
{
  std::set<boolean_expression> v_prefixed;
  for (std::set<identifier_t>::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    std::stringstream id;
    id << prefix << *i;
    v_prefixed.insert(boolean_variable(id.str()));
  }

  if (owner == 0)
  {
    return join_or(v_prefixed.begin(), v_prefixed.end());
  }
  else
  {
    return join_and(v_prefixed.begin(), v_prefixed.end());
  }
}

struct pg_actions: public core::default_parser_actions
{
  // Parse node specifications (store in map)
  std::map<identifier_t, node_t> game;
  identifier_t initial_node;

  pg_actions(const core::parser& parser_)
    : core::default_parser_actions(parser_),
      initial_node((std::numeric_limits<identifier_t>::max)())
  {}

  template <typename T, typename Function>
  std::set<T> parse_set(const core::parse_node& node, const std::string& type, Function f)
  {
    std::set<T> result;
    traverse(node, make_set_collector(m_parser, type, result, f));
    return result;
  }

  void create_boolean_equation_system(boolean_equation_system& b, bool maxpg)
  {
    // Build Boolean equation system. First we group equations by block
    std::map<priority_t, std::set<boolean_equation> > blocks;
    // Translation scheme:
    // prefix every id with X. Owner 0 means ||, owner 1 means &&

    for (std::map<identifier_t, node_t>::const_iterator i = game.begin(); i != game.end(); ++i)
    {
      std::stringstream id;
      id << "X" << i->second.id;

      fixpoint_symbol fp(fixpoint_symbol::mu());
      if (i->second.prio % 2 == 0)
      {
        fp = fixpoint_symbol::nu();
      }

      boolean_equation eqn(fp, boolean_variable(id.str()), formula(i->second.successors, i->second.owner));

      blocks[i->second.prio].insert(eqn);
    }

    std::vector<boolean_equation> eqns;
    if(maxpg)
    {
      for (std::map<priority_t, std::set<boolean_equation> >::reverse_iterator i = blocks.rbegin(); i != blocks.rend(); ++i)
      {
        eqns.insert(eqns.end(), i->second.begin(), i->second.end());
      }
    }
    else
    {
      for (std::map<priority_t, std::set<boolean_equation> >::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
      {
        eqns.insert(eqns.end(), i->second.begin(), i->second.end());
      }
    }

    b.equations() = eqns;
    std::stringstream init_id;
    init_id << initial_node;
    b.initial_state() = boolean_variable("X" + init_id.str());
  }

  void parse_ParityGame(const core::parse_node& node, boolean_equation_system& result, bool maxpg)
  {
    if(node.child_count() == 5)
    {

      initial_node = parse_Id(node.child(3));
    }
    if(node.child_count() == 3 && node.child(0).string() == "start")
    {
      initial_node = parse_Id(node.child(1));
    }

    game.clear();
    parse_NodeSpecList(node.child(node.child_count()-1));
    create_boolean_equation_system(result, maxpg);
  }

  void parse_NodeSpec(const core::parse_node& node)
  {
    node_t result;
    result.id = parse_Id(node.child(0));
    result.prio = parse_Priority(node.child(1));
    result.owner = parse_Owner(node.child(2));
    result.successors = parse_Successors(node.child(3));
    if (game.empty() && initial_node == (std::numeric_limits<identifier_t>::max)())
    {
      initial_node = result.id;
    }
    game[result.id] = result;
  }

  void parse_NodeSpecList(const core::parse_node& node)
  {
    traverse(node, make_visitor(m_parser.symbol_table(), "NodeSpec", std::bind(&pg_actions::parse_NodeSpec, this, std::placeholders::_1)));
  }

  identifier_t parse_Id(const core::parse_node& node)
  {
    return parse_Number<identifier_t>(node.child(0));
  }

  priority_t parse_Priority(const core::parse_node& node)
  {
    return parse_Number<priority_t>(node.child(0));
  }

  bool parse_Owner(const core::parse_node& node)
  {
    return node.string() == "1";
  }

  std::set<identifier_t> parse_Successors(const core::parse_node& node)
  {
    std::set<identifier_t> result;
    traverse(node, make_set_collector(m_parser.symbol_table(), "Id", result, std::bind(&pg_actions::parse_Id, this, std::placeholders::_1)));
    return result;
  }

  template <typename T>
  T parse_Number(const core::parse_node& node)
  {
    T result;
    std::stringstream tmp;
    tmp << node.string();
    tmp >> result;
    return result;
  }
};

/// \brief Reads a parity game from an input stream, and stores it as a BES.
/// \param text A string
/// \param result A boolean equation system
/// \param maxpg If true a max-parity game is generated in \a result, otherwise a min-parity
///        game is obtained.
inline
void parse_pgsolver_string(const std::string& text, boolean_equation_system& result, bool maxpg = true)
{
  core::parser p(parser_tables_pg);
  unsigned int start_symbol_index = p.start_symbol_index("ParityGame");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  pg_actions(p).parse_ParityGame(node, result, maxpg);
  p.destroy_parse_node(node);
}

/// \brief Reads a parity game from an input stream, and stores it as a BES.
/// \param from An input stream
/// \param result A boolean equation system
/// \param maxpg If true a max-parity game is generated in \a result, otherwise a min-parity
///        game is obtained.
inline
void parse_pgsolver(std::istream& from, boolean_equation_system& result, bool maxpg = true)
{
  std::string text = utilities::read_text(from);
  parse_pgsolver_string(text, result, maxpg);
}

/// \brief Parse parity game in PGSolver format from filename, and store the
///        resulting BES in b.
inline void parse_pgsolver(const std::string& filename, boolean_equation_system& b, bool maxpg = true)
{
  if(filename == "-" || filename.empty())
  {
    parse_pgsolver(std::cin, b, maxpg);
  }
  else
  {
    std::ifstream f;
    f.open(filename.c_str());
    if(!f)
    {
      throw mcrl2::runtime_error("cannot open file " + filename + " for reading");
    }
    parse_pgsolver(f, b, maxpg);
  }
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_PG_PARSE_H
