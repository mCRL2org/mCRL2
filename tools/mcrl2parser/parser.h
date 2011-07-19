// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file parser.h
/// \brief add your file description here.

// N.B. DPARSER_TABLES must be defined before including this file

#ifndef DPARSER_H
#define DPARSER_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <set>
#include <vector>

namespace dparser {
  class parser;
  class term;
}

//#define D_ParseNode_Globals dparser::parser
//#define D_ParseNode_User dparser::term*

#include "dparse.h"

extern D_ParserTables DPARSER_TABLES;

namespace dparser {

class parse_error: public std::exception
{ };

class user_error: public std::exception
{
  public:
    user_error(std::string msg)
      : m_what(msg)
    { }

    ~user_error() throw()
    { }

    virtual const char* what() const throw()
    {
      return m_what.c_str();
    }

  private:
    std::string m_what;
};

/// \brief Wrapper for D_ParseNode
struct parse_node
{
  D_ParseNode* node;

  parse_node(D_ParseNode* n)
    : node(n)
  {}

  int symbol() const
  {
    return node->symbol;
  }

  int child_count() const
  {
    return d_get_number_of_children(node);
  }

  parse_node child(int i) const
  {
    return parse_node(d_get_child(node, i));
  }

  parse_node find_in_tree(int symbol) const
  {
    return parse_node(d_find_in_tree(node, symbol));
  }

  std::string string() const
  {
    return std::string(node->start_loc.s, node->end - node->start_loc.s);
  }
};

/// \brief Wrapper for D_Parser and it's corresponding D_ParserTables
struct parser
{
  D_ParserTables& m_tables;
  D_Parser* m_parser;

  parser(D_ParserTables& tables)
    : m_tables(tables)
  {
    m_parser = new_D_Parser(&tables, 0);
    m_parser->initial_globals = this;
  }

  ~parser()
  {
    free_D_Parser(m_parser);
  }

  parse_node parse(const std::string& text)
  {
    D_ParseNode* result = dparse(m_parser, const_cast<char*>(text.c_str()), text.size());
    if (m_parser->syntax_errors)
    {
      throw parse_error();
    }
    return parse_node(result);
  }

  unsigned int symbol_count() const
  {
    return m_tables.nsymbols;
  }

  std::string symbol_name(unsigned int i) const
  {
    return std::string(m_tables.symbols[i].name);
  }

  unsigned int symbol_index(const std::string& name) const
  {
    for (unsigned int i = 0; i < symbol_count(); i++)
    {
      if (symbol_name(i) == name)
      {
        return i;
      }
    }
    throw user_error("unknown symbol '" + name + "'.");
    return 0;
  }

  void print_symbol_table() const
  {
    std::clog << "--------------------" << std::endl;
    std::clog << "-   symbol table   -" << std::endl;
    std::clog << "--------------------" << std::endl;
    for (unsigned int i = 0; i < symbol_count(); i++)
    {
      std::clog << "symbol " << symbol_name(i) << std::endl;
    }
    std::clog << "--------------------" << std::endl;
  }

  /// \brief Callback function for nodes in the parse tree
  void announce(D_ParseNode& node_ref)
  {
    parse_node node(&node_ref);
    std::cout << "parsed " << symbol_name(node.symbol()) << " " << node.string() << std::endl;
  }

  /// \brief Callback function for nodes in the parse tree
  void preannounce(D_ParseNode& node_ref)
  {
    parse_node node(&node_ref);
    std::cout << "preparsed " << symbol_name(node.symbol()) << " " << node.string() << std::endl;
  }
};

} // namespace dparser

#endif // DPARSER_H
