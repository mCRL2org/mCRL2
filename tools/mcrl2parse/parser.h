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

struct parser_table
{
  D_ParserTables& m_table;

  parser_table(D_ParserTables& table)
    : m_table(table)
  { }

  // Returns the number of symbols in the table
  unsigned int symbol_count() const
  {
    return m_table.nsymbols;
  }

  // Returns the name of the i-th symbol
  std::string symbol_name(unsigned int i) const
  {
    return std::string(m_table.symbols[i].name);
  }

  // Returns the 'start symbol' of the i-th symbol
  int start_symbol(unsigned int i) const
  {
    return m_table.symbols[i].start_symbol;
  }

  // Returns true if the i-th symbol is of type D_SYMBOL_NTERM
  bool is_term_symbol(unsigned int i) const
  {
    return m_table.symbols[i].kind == D_SYMBOL_NTERM;
  }

  unsigned int start_symbol_index(const std::string& name) const
  {
    for (unsigned int i = 0; i < symbol_count(); i++)
    {
      if (is_term_symbol(i) && symbol_name(i) == name)
      {
        return start_symbol(i);
      }
    }
    throw user_error("unknown start symbol '" + name + "'");
    return 0;
  }

  void print() const
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
};

/// \brief Wrapper for D_Parser and it's corresponding D_ParserTables
struct parser
{
  parser_table m_table;
  D_Parser* m_parser;

  parser(D_ParserTables& tables)
    : m_table(tables)
  {
    m_parser = new_D_Parser(&tables, 0);
    m_parser->initial_globals = this;
    m_parser->save_parse_tree = 1;
    m_parser->initial_scope = NULL;
  }

  ~parser()
  {
    free_D_Parser(m_parser);
  }

  unsigned int start_symbol_index(const std::string& name) const
  {
    return m_table.start_symbol_index(name);
  }

  parse_node parse(const std::string& text, unsigned int start_symbol_index = 0, bool partial_parses = false)
  {
    m_parser->start_state = start_symbol_index;
    m_parser->partial_parses = partial_parses ? 1 : 0;
    D_ParseNode* result = dparse(m_parser, const_cast<char*>(text.c_str()), text.size());
    if (!result || m_parser->syntax_errors)
    {
      throw parse_error();
    }
    return parse_node(result);
  }

  void print_symbol_table() const
  {
    m_table.print();
  }

  /// \brief Callback function for nodes in the parse tree
  void announce(D_ParseNode& node_ref)
  {
    parse_node node(&node_ref);
    std::cout << "parsed " << m_table.symbol_name(node.symbol()) << " " << node.string() << std::endl;
  }

  /// \brief Callback function for nodes in the parse tree
  void preannounce(D_ParseNode& node_ref)
  {
    parse_node node(&node_ref);
    std::cout << "preparsed " << m_table.symbol_name(node.symbol()) << " " << node.string() << std::endl;
  }
};

} // namespace dparser

#endif // DPARSER_H
