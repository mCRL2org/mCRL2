// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/dparser.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DPARSER_H
#define MCRL2_CORE_DPARSER_H

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <set>
#include <string>
#include <sstream>
#include <vector>
#include "dparse.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

namespace core {

namespace detail
{

template <class T> // note, T is only a dummy
struct dparser_error_message_count
{
  static std::size_t value;
  static std::size_t max_value;
};

template <class T>
std::size_t dparser_error_message_count<T>::value = 0;

template <class T>
std::size_t dparser_error_message_count<T>::max_value = 1;

inline
void reset_dparser_error_message_count()
{
  dparser_error_message_count<std::size_t>::value = 0;
}

inline
void increment_dparser_error_message_count()
{
  dparser_error_message_count<std::size_t>::value++;
}

inline
std::size_t get_dparser_error_message_count()
{
  return dparser_error_message_count<std::size_t>::value;
}

inline
std::size_t get_dparser_max_error_message_count()
{
  return dparser_error_message_count<std::size_t>::max_value;
}

inline
void set_dparser_max_error_message_count(std::size_t n)
{
  dparser_error_message_count<std::size_t>::max_value = n;
}

} // namespace detail

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

  // 0 <= i < child_count()
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

  std::string tree() const
  {
    if (child_count() < 2)
      return this->string();
    std::stringstream result;
    result << "(" << child(0).tree();
    for (int i = 1; i < child_count(); ++i)
      result << " " << child(i).tree();
    result << ")";
    return result.str();
  }

  int column() const
  {
    return node->start_loc.col;
  }

  int line() const
  {
    return node->start_loc.line;
  }

  std::string pathname() const
  {
    return std::string(node->start_loc.pathname);
  }

  operator bool() const
  {
    return node != 0;
  }
};

/// \brief Wrapper for D_ParserTables
struct parser_table
{
  D_ParserTables& m_table;

  parser_table(D_ParserTables& table)
    : m_table(table)
  { }

  // Prints a tree of
  std::string tree(const core::parse_node& node) const
  {
    std::stringstream result;
    result << symbol_name(node) << "(";
    if (node.child_count() == 0)
      result << '"' << node.string() << '"';
    else
      result << tree(node.child(0));
    for (int i = 1; i < node.child_count(); ++i)
      result << " " << tree(node.child(i));
    result << ")";
    return result.str();
  }

  // Returns the number of symbols in the table
  unsigned int symbol_count() const
  {
    return m_table.nsymbols;
  }

  // Returns the name of the i-th symbol
  std::string symbol_name(unsigned int i) const
  {
    const char* name = m_table.symbols[i].name;
    if (!name)
    {
      return "";
    }
    return std::string(name);
  }

  std::string symbol_name(const parse_node& node) const
  {
    return symbol_name(node.symbol());
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
    throw mcrl2::runtime_error("unknown start symbol '" + name + "'");
    return 0;
  }

  void print() const
  {
    std::clog << "--------------------" << std::endl;
    std::clog << "-   symbol table   -" << std::endl;
    std::clog << "--------------------" << std::endl;
    for (unsigned int i = 0; i < symbol_count(); i++)
    {
      std::clog << std::setw(3) << i << " " << symbol_name(i) << std::endl;
    }
    std::clog << "--------------------" << std::endl;
  }
};

/// \brief Wrapper for D_Parser and it's corresponding D_ParserTables
struct parser
{
  parser_table m_table;
  D_Parser* m_parser;
  std::size_t m_max_error_message_count;

  parser(D_ParserTables& tables, D_AmbiguityFn ambiguity_fn = 0, D_SyntaxErrorFn syntax_error_fn = 0, std::size_t max_error_message_count = 1)
    : m_table(tables)
  {
    detail::set_dparser_max_error_message_count(max_error_message_count);
    m_parser = new_D_Parser(&tables, 0);
    m_parser->initial_globals = this;
    m_parser->save_parse_tree = 1;
    m_parser->initial_scope = NULL;
    m_parser->dont_use_greediness_for_disambiguation = 1;
    if (ambiguity_fn)
    {
      m_parser->ambiguity_fn = ambiguity_fn;
    }
    if (syntax_error_fn)
    {
      m_parser->syntax_error_fn = syntax_error_fn;
    }
  }

  ~parser()
  {
    free_D_Parser(m_parser);
  }

  const parser_table& symbol_table() const
  {
    return m_table;
  }

  unsigned int start_symbol_index(const std::string& name) const
  {
    return m_table.start_symbol_index(name);
  }

  /// \brief Parses a string. N.B. The user is responsible for destruction of the returned
  /// value by calling destroy_parse_node!!!
  parse_node parse(const std::string& text, unsigned int start_symbol_index = 0, bool partial_parses = false)
  {
    detail::reset_dparser_error_message_count();
    m_parser->start_state = start_symbol_index;
    m_parser->partial_parses = partial_parses ? 1 : 0;
    D_ParseNode* result = dparse(m_parser, const_cast<char*>(text.c_str()), static_cast<int>(text.size()));
    if (!result || m_parser->syntax_errors)
    {
      throw mcrl2::runtime_error("syntax error");
    }
    return parse_node(result);
  }

  void print_symbol_table() const
  {
    m_table.print();
  }

  std::string indent(unsigned int count) const
  {
    return std::string(count, ' ');
  }

  std::string truncate(const std::string& s, unsigned int max_size = 20) const
  {
    std::string result = s.substr(0, max_size);

    // truncate at newline
    std::string::size_type pos = result.find('\n');
    if (pos != std::string::npos)
    {
      result = result.substr(0, pos);
    }

    return result;
  }

  void print_tree(const parse_node& node, unsigned int level = 0) const
  {
    if (node)
    {
      std::string symbol = m_table.symbol_name(node.symbol());
      std::string prefix = indent(2 * level);
      std::cout << prefix << "--- " << symbol << " \"" << truncate(node.string()) << "\"" << std::endl;
      for (int i = 0; i <= node.child_count(); i++)
      {
        print_tree(node.child(i), level + 1);
      }
    }
  }

  void destroy_parse_node(const parse_node& node)
  {
    free_D_ParseNode(m_parser, node.node);
  }

  /// \brief Callback function for nodes in the parse tree
  void announce(D_ParseNode& node_ref)
  {
    parse_node node(&node_ref);
    std::cout << "parsed " << m_table.symbol_name(node.symbol()) << " " << node.string() << std::endl;
  }
};

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DPARSER_H
