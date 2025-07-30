// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/dparser.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_DPARSER_H
#define MCRL2_CORE_DPARSER_H

#include <cstddef>
#include <sstream>
#include <string>

// prototypes
struct D_ParseNode;
struct D_ParserTables;
struct D_Parser;
struct d_loc_t;
using D_SyntaxErrorFn = void (*)(struct D_Parser*);
using D_AmbiguityFn = struct D_ParseNode* (*)(struct D_Parser*, int, struct D_ParseNode**);

namespace mcrl2::core
{

namespace detail
{

std::string add_context(const d_loc_t* loc, const std::string& message);

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
  D_Parser* parser = nullptr;

  explicit parse_node(D_ParseNode* n, D_Parser* parser_ = nullptr)
    : node(n), parser(parser_)
  {}

  int symbol() const;
  int child_count() const;
  /// \brief Returns the priority of the rule assocated with this parse node.
  int priority() const;

  // 0 <= i < child_count()
  parse_node child(int i) const;
  parse_node find_in_tree(int symbol) const;
  std::string string() const;
  std::string tree() const;
  int column() const;
  int line() const;
  std::string pathname() const;
  std::string add_context(const std::string& message) const;

  explicit operator bool() const
  {
    return node != nullptr;
  }

  ~parse_node();
};

/// \brief Wrapper for D_ParserTables
struct parser_table
{
  D_ParserTables& m_table;

  explicit parser_table(D_ParserTables& table)
    : m_table(table)
  { }

  // Prints a tree of
  std::string tree(const core::parse_node& node) const;

  // Returns the number of symbols in the table
  unsigned int symbol_count() const;

  // Returns the name of the i-th symbol
  std::string symbol_name(unsigned int i) const;

  std::string symbol_name(const parse_node& node) const;

  // Returns the 'start symbol' of the i-th symbol
  int start_symbol(unsigned int i) const;

  // Returns true if the i-th symbol is of type D_SYMBOL_NTERM
  bool is_term_symbol(unsigned int i) const;

  unsigned int start_symbol_index(const std::string& name) const;

  void print() const;
};

/// \brief Wrapper for D_Parser and its corresponding D_ParserTables
struct parser
{
  parser_table m_table;
  D_Parser* m_parser;
  std::size_t m_max_error_message_count = 0UL;

  explicit parser(D_ParserTables& tables, D_AmbiguityFn ambiguity_fn = nullptr, D_SyntaxErrorFn syntax_error_fn = nullptr, std::size_t max_error_message_count = 1);

  ~parser();

  const parser_table& symbol_table() const;

  unsigned int start_symbol_index(const std::string& name) const;

  /// \brief Parses a string. N.B. The user is responsible for destruction of the returned
  /// value by calling destroy_parse_node!!!
  parse_node parse(const std::string& text, unsigned int start_symbol_index = 0, bool partial_parses = false);

  void print_symbol_table() const;

  std::string indent(unsigned int count) const;

  std::string truncate(const std::string& s, unsigned int max_size = 20) const;

  void print_tree(const parse_node& node, unsigned int level = 0) const;

  void destroy_parse_node(const parse_node& node);

  void custom_parse_error(const std::string& message) const;

  /// \brief Callback function for nodes in the parse tree
  void announce(D_ParseNode& node_ref);

  void print_node(std::ostream& out, const parse_node& node) const
  {
    out << "symbol      = " << symbol_table().symbol_name(node) << std::endl
        << "string      = " << node.string() << std::endl
        << "child_count = " << node.child_count();
    for (int i = 0; i < node.child_count(); i++)
    {
      out << std::endl
          << "child " << i << " = " << symbol_table().symbol_name(node.child(i))
          << " " << node.child(i).string();
    }
  }

  std::string print_node(const parse_node& node) const
  {
    std::ostringstream out;
    print_node(out, node);
    return out.str();
  }
};

} // namespace mcrl2::core

#endif // MCRL2_CORE_DPARSER_H
