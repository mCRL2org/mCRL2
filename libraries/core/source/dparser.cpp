// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/dparser.cpp
/// \brief add your file description here.

#include "d.h"
#include "dparse.h"
#include "mcrl2/core/detail/dparser_functions.h"
#include "mcrl2/core/dparser.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include "parse.h"
#include <iomanip>
#include <iostream>
#include <locale>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

extern "C"
{
  extern D_ParserTables parser_tables_mcrl2;
}

namespace mcrl2 {

namespace core {

std::string parse_node::add_context(const std::string& message) const
{
  return detail::add_context(&node->start_loc, message);
}

int parse_node::symbol() const
{
  return node->symbol;
}

int parse_node::child_count() const
{
  return d_get_number_of_children(node);
}

// 0 <= i < child_count()
parse_node parse_node::child(int i) const
{
  return parse_node(d_get_child(node, i));
}

parse_node parse_node::find_in_tree(int symbol) const
{
  return parse_node(d_find_in_tree(node, symbol));
}

std::string parse_node::string() const
{
  return std::string(node->start_loc.s, node->end - node->start_loc.s);
}

std::string parse_node::tree() const
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

int parse_node::column() const
{
  return node->start_loc.col;
}

int parse_node::line() const
{
  return node->start_loc.line;
}

std::string parse_node::pathname() const
{
  return std::string(node->start_loc.pathname);
}

// Prints a tree of
std::string parser_table::tree(const core::parse_node& node) const
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
unsigned int parser_table::symbol_count() const
{
  return m_table.nsymbols;
}

// Returns the name of the i-th symbol
std::string parser_table::symbol_name(unsigned int i) const
{
  if (i >= m_table.nsymbols)
  {
    print();
    std::ostringstream out;
    out << "parser_table::symbol_name: index " << i << " out of bounds!";
    throw std::runtime_error(out.str());
  }
  const char* name = m_table.symbols[i].name;
  if (!name)
  {
    return "";
  }
  return std::string(name);
}

std::string parser_table::symbol_name(const parse_node& node) const
{
  return symbol_name(node.symbol());
}

// Returns the 'start symbol' of the i-th symbol
int parser_table::start_symbol(unsigned int i) const
{
  return m_table.symbols[i].start_symbol;
}

// Returns true if the i-th symbol is of type D_SYMBOL_NTERM
bool parser_table::is_term_symbol(unsigned int i) const
{
  return m_table.symbols[i].kind == D_SYMBOL_NTERM;
}

unsigned int parser_table::start_symbol_index(const std::string& name) const
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

void parser_table::print() const
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

parser::parser(D_ParserTables& tables, D_AmbiguityFn ambiguity_fn, D_SyntaxErrorFn syntax_error_fn, std::size_t max_error_message_count)
  : m_table(tables)
{
  detail::set_dparser_max_error_message_count(max_error_message_count);
  m_parser = new_D_Parser(&tables, 0);
  m_parser->initial_globals = this;
  m_parser->save_parse_tree = 1;
  m_parser->initial_scope = nullptr;
  m_parser->dont_use_greediness_for_disambiguation = 1;
  m_parser->dont_use_height_for_disambiguation = 1;
  if (ambiguity_fn)
  {
    m_parser->ambiguity_fn = ambiguity_fn;
  }
  if (syntax_error_fn)
  {
    m_parser->syntax_error_fn = syntax_error_fn;
  }
}

parser::~parser()
{
  free_D_Parser(m_parser);
}

const parser_table& parser::symbol_table() const
{
  return m_table;
}

unsigned int parser::start_symbol_index(const std::string& name) const
{
  return m_table.start_symbol_index(name);
}

/// \brief Parses a string. N.B. The user is responsible for destruction of the returned
/// value by calling destroy_parse_node!!!
parse_node parser::parse(const std::string& text, unsigned int start_symbol_index, bool partial_parses)
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

void parser::print_symbol_table() const
{
  m_table.print();
}

std::string parser::indent(unsigned int count) const
{
  return std::string(count, ' ');
}

std::string parser::truncate(const std::string& s, unsigned int max_size) const
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

void parser::print_tree(const parse_node& node, unsigned int level) const
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

void parser::destroy_parse_node(const parse_node& node)
{
  free_D_ParseNode(m_parser, node.node);
}

/// \brief Callback function for nodes in the parse tree
void parser::announce(D_ParseNode& node_ref)
{
  parse_node node(&node_ref);
  std::cout << "parsed " << m_table.symbol_name(node.symbol()) << " " << node.string() << std::endl;
}

namespace detail {

std::string add_context(const d_loc_t* loc, const std::string& message)
{
  std::stringstream s;
  s << "Line " << loc->line << ", column " << loc->col << ": "
    << message << std::endl;
  char* beg = loc->s - loc->col;
  char* end = loc->s;
  while (*end != '\0' && *end != '\n' && *end != '\r')
  {
    ++end;
  }
  std::string line(beg, end);
  s << "  " << line << std::endl;
  for (int i = 0; i < loc->col + 2; ++i)
  {
    s << ' ';
  }
  s << '^';
  return s.str();
}

inline
bool is_all_of_type(D_ParseNode* nodes[], int n, const char* type, const core::parser_table& table)
{
  for (int i = 0; i < n; i++)
  {
    core::parse_node node(nodes[i]);
    if (table.symbol_name(node) != type)
    {
      return false;
    }
  }
  return true;
}

inline
void print_ambiguous_nodes(D_ParseNode* nodes[], int n, const char* type, const core::parser_table& table)
{
  mCRL2log(log::verbose, "parser") << "--- " << type << " ambiguity" << std::endl;
  for (int i = 0; i < n; ++i)
  {
    core::parse_node vi(nodes[i]);
    // mCRL2log(log::verbose, "parser") << vi.tree() << " " << table.tree(vi) << std::endl;
    mCRL2log(log::verbose, "parser") << "ALT " << table.tree(vi) << std::endl;
  }
}

inline
void print_chosen_node(D_ParseNode* node, const core::parser_table& table)
{
  core::parse_node vi(node);
  mCRL2log(log::verbose, "parser") << "CHOOSE " << table.tree(vi) << std::endl;
}

/// \brief Function for resolving parser ambiguities.
D_ParseNode* ambiguity_fn(struct D_Parser * /*p*/, int n, struct D_ParseNode **v)
{
  core::parser_table table(parser_tables_mcrl2);

  // resolve PbesExpr ambiguities
  if (is_all_of_type(v, n, "PbesExpr", table))
  {
    D_ParseNode* result = nullptr;
    for (int i = 0; i < n; i++)
    {
      core::parse_node node(v[i]);
      if (table.symbol_name(node.child(0)) == "Id")
      {
        return v[i];
      }
      else if (table.symbol_name(node.child(0)) != "DataExpr")
      {
        result = v[i];
      }
    }
    if (result)
    {
      return result;
    }
    return v[0];
  }

  // resolve ActFrm ambiguities
  if (is_all_of_type(v, n, "ActFrm", table))
  {
//print_ambiguous_nodes(v, n, "ActFrm", table);
    D_ParseNode* result = nullptr;
    for (int i = 0; i < n; i++)
    {
      core::parse_node node(v[i]);
      if (table.symbol_name(node.child(0)) == "MultAct")
      {
//print_chosen_node(v[i], table);
        return v[i];
      }
      else if (table.symbol_name(node.child(0)) != "DataExpr")
      {
        result = v[i];
      }
    }
    if (result)
    {
//print_chosen_node(result, table);
      return result;
    }
//print_chosen_node(v[0], table);
    return v[0];
  }

  // resolve StateFrm ambiguities
  if (is_all_of_type(v, n, "StateFrm", table))
  {
//print_ambiguous_nodes(v, n, "StateFrm", table);
    D_ParseNode* result = nullptr;
    for (int i = 0; i < n; i++)
    {
      core::parse_node node(v[i]);
      if (table.symbol_name(node.child(0)) == "Id")
      {
//print_chosen_node(v[i], table);
        return v[i];
      }
      else if (table.symbol_name(node.child(0)) != "DataExpr")
      {
        result = v[i];
      }
    }
    if (result)
    {
//print_chosen_node(result, table);
      return result;
    }
//print_chosen_node(v[0], table);
    return v[0];
  }

  // resolve RegFrm ambiguities
  if (is_all_of_type(v, n, "RegFrm", table))
  {
//print_ambiguous_nodes(v, n, "RegFrm", table);
    for (int i = 0; i < n; i++)
    {
      core::parse_node node(v[i]);
      if (table.symbol_name(node.child(0)) == "RegFrm" || table.symbol_name(node.child(0)) == "(")
      {
//print_chosen_node(v[i], table);
        return v[i];
      }
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

static void log_location(struct D_Parser *ap)
{
  // We recover information about the last parsed node by casting D_Parser to Parser, which
  // is the structure that the dparser library internally uses to keep its administration in.
  std::string after;
  SNode *s = ((Parser*)ap)->snode_hash.last_all;
  ZNode *z = s != nullptr ? s->zns.v[0] : nullptr;
  while (z != nullptr && z->pn->parse_node.start_loc.s == z->pn->parse_node.end)
  {
    z = (z->sns.v && z->sns.v[0]->zns.v) ? z->sns.v[0]->zns.v[0] : nullptr;
  }
  if (z && z->pn->parse_node.start_loc.s != z->pn->parse_node.end)
  {
    after = std::string(z->pn->parse_node.start_loc.s, z->pn->parse_node.end);
  }

  std::string message = "syntax error";
  if (!after.empty())
  {
    message = message + " after '" + after + "'";
  }
  mCRL2log(log::error, "parser") << add_context(&ap->loc, message) << std::endl;
}

void syntax_error_fn(struct D_Parser *ap)
{
  core::detail::increment_dparser_error_message_count();
  if (core::detail::get_dparser_error_message_count() > core::detail::get_dparser_max_error_message_count())
  {
    return;
  }
  log_location(ap);
  if (ap->loc.s == nullptr)
  {
    mCRL2log(log::error, "parser") << "Unexpected end of input." << std::endl;
  }
  else
  {
    // Dive into the internals of dparser to recover some extra diagnostics.
    Parser* p = (Parser*)ap;
    if (p->pnode_hash.all && p->pnode_hash.all->latest)
    {
      core::parse_node n(&p->pnode_hash.all->latest->parse_node);
      D_Symbol &s = p->t->symbols[n.symbol()];
      if (s.kind == D_SYMBOL_INTERNAL)
      {
        /* DParser stores production rules in order: search for the corresponding nonterminal. */
        int parentsym = n.symbol() - 1;
        while (p->t->symbols[parentsym].kind == D_SYMBOL_INTERNAL)
          --parentsym;
        s = p->t->symbols[parentsym];
      }

      switch (s.kind)
      {
      case D_SYMBOL_STRING:
      case D_SYMBOL_TOKEN:
        {
          std::locale loc;
          mCRL2log(log::error, "parser") << "Unexpected "
                                         << (std::isalpha(n.string()[0], loc) ? "keyword " : "")
                                         << "'" << n.string() << "'" << std::endl;
        }
        break;
      case D_SYMBOL_NTERM:
        mCRL2log(log::error, "parser") << "Unexpected " << s.name << " '" << n.string() << "'" << std::endl;
        break;
      default:
        // TODO: check if we can give more sensible output in the remaining cases.
        break;
      }
    }
  }
}

} // namespace detail

void parser::custom_parse_error(const std::string& message) const
{
  core::detail::increment_dparser_error_message_count();
  if (core::detail::get_dparser_error_message_count() > core::detail::get_dparser_max_error_message_count())
  {
    return;
  }
  detail::log_location(m_parser);
  mCRL2log(log::error, "parser") << message << std::endl;
}

} // namespace core

} // namespace mcrl2

