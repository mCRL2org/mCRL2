// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/parse.h
///
/// \brief Parse mCRL2 specifications and expressions.

#ifndef MCRL2_CORE_PARSE_H
#define MCRL2_CORE_PARSE_H

#include <iostream>
#include <sstream>
#include <string>
#include <boost/bind.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/core/dparser.h"
#include "mcrl2/core/detail/dparser_functions.h"

extern "C"
{
  extern D_ParserTables parser_tables_mcrl2;
  extern D_ParserTables parser_tables_fsm;
  extern D_ParserTables parser_tables_dot;
}

namespace mcrl2
{
namespace core
{

struct parser_actions
{
  const parser_table& table;

  parser_actions(const parser_table& table_)
    : table(table_)
  {}

  // starts a traversal in node, and calls the function f to each subnode of the given type
  template <typename Function>
  void traverse(const parse_node& node, Function f)
  {
    if (!node)
    {
      return;
    }
    if (!f(node))
    {
      for (int i = 0; i < node.child_count(); i++)
      {
        traverse(node.child(i), f);
      }
    }
  }

  // callback function that applies a function to nodes of a given type
  template <typename Function>
  struct visitor
  {
    const parser_table& table;
    const std::string& type;
    Function f;

    visitor(const parser_table& table_, const std::string& type_, Function f_)
      : table(table_),
        type(type_),
        f(f_)
    {}

    bool operator()(const parse_node& node) const
    {
      if (table.symbol_name(node) == type)
      {
        f(node);
        return true;
      }
      return false;
    }
  };

  template <typename Function>
  visitor<Function> make_visitor(const parser_table& table, const std::string& type, Function f)
  {
    return visitor<Function>(table, type, f);
  }

  // callback function that applies a function to a node, and adds the result to a container
  template <typename Container, typename Function>
  struct collector
  {
    const parser_table& table;
    const std::string& type;
    Container& container;
    Function f;

    collector(const parser_table& table_, const std::string& type_, Container& container_, Function f_)
      : table(table_),
        type(type_),
        container(container_),
        f(f_)
    {}

    bool operator()(const parse_node& node) const
    {
      if (table.symbol_name(node) == type)
      {
        container.push_back(f(node));
        return true;
      }
      return false;
    }
  };

  template <typename Container, typename Function>
  collector<Container, Function> make_collector(const parser_table& table, const std::string& type, Container& container, Function f)
  {
    return collector<Container, Function>(table, type, container, f);
  }

  // callback function that applies a function to a node, and adds the result to a set container
  template <typename SetContainer, typename Function>
  struct set_collector
  {
    const parser_table& table;
    const std::string& type;
    SetContainer& container;
    Function f;

    set_collector(const parser_table& table_, const std::string& type_, SetContainer& container_, Function f_)
      : table(table_),
        type(type_),
        container(container_),
        f(f_)
    {}

    bool operator()(const parse_node& node) const
    {
      if (table.symbol_name(node) == type)
      {
        container.insert(f(node));
        return true;
      }
      return false;
    }
  };

  template <typename SetContainer, typename Function>
  set_collector<SetContainer, Function> make_set_collector(const parser_table& table, const std::string& type, SetContainer& container, Function f)
  {
    return set_collector<SetContainer, Function>(table, type, container, f);
  }

  std::string symbol_name(const parse_node& node) const
  {
    return table.symbol_name(node.symbol());
  }

  std::string print_node(const parse_node& node)
  {
    std::ostringstream out;
    out << "symbol      = " << symbol_name(node) << std::endl;
    out << "string      = " << node.string() << std::endl;
    out << "child_count = " << node.child_count() << std::endl;
    for (int i = 0; i < node.child_count(); i++)
    {
      out << "child " << i << " = " << symbol_name(node.child(i)) << " " << node.child(i).string() << std::endl;
    }
    return out.str();
  }

  void report_unexpected_node(const parse_node& node)
  {
    std::cout << "--- unexpected node ---\n" << print_node(node);
    throw mcrl2::runtime_error("unexpected node detected!");
  }
};

struct default_parser_actions: public parser_actions
{
  default_parser_actions(const parser_table& table_)
    : parser_actions(table_)
  {}

  template <typename T, typename Function>
  atermpp::term_list<T> parse_list(const parse_node& node, const std::string& type, Function f)
  {
    std::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return atermpp::term_list<T>(result.begin(), result.end());
  }

  template <typename T, typename Function>
  std::vector<T> parse_vector(const parse_node& node, const std::string& type, Function f)
  {
    std::vector<T> result;
    traverse(node, make_collector(table, type, result, f));
    return result;
  }

  core::identifier_string parse_Id(const parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string parse_Number(const parse_node& node)
  {
    return core::identifier_string(node.string());
  }

  core::identifier_string_list parse_IdList(const parse_node& node)
  {
    return parse_list<core::identifier_string>(node, "Id", boost::bind(&default_parser_actions::parse_Id, this, _1));
  }
};

template <typename T>
void print_aterm(const T&)
{
}

template<>
inline
void print_aterm(const atermpp::aterm_appl& x)
{
  std::clog << "aterm: " << x << std::endl;
}

/// \brief Parse an identifier.
inline
identifier_string parse_identifier(const std::string& text)
{
  core::parser p(parser_tables_mcrl2, core::detail::ambiguity_fn, core::detail::syntax_error_fn);
  unsigned int start_symbol_index = p.start_symbol_index("Id");
  bool partial_parses = false;
  core::parse_node node = p.parse(text, start_symbol_index, partial_parses);
  identifier_string result = default_parser_actions(parser_tables_mcrl2).parse_Id(node);
  p.destroy_parse_node(node);
  return result;
}

inline
bool is_user_identifier(std::string const& s)
{
  try
  {
    parse_identifier(s);
  }
  catch (...)
  {
    return false;
  }
  return true;
}

}
}

#endif // MCRL2_CORE_PARSE_H
