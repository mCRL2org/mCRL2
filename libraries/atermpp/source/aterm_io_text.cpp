// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

namespace atermpp
{

using namespace std;

// utility functions

static void write_string_with_escape_symbols(const std::string& s, std::ostream& os)
{
  // Check whether the string starts with a - or a number, or contains the symbols
  // \, ", (, ), [, ], comma, space, \t, \n or \r. If yes, the string will be
  // surrounded by quotes, and the symbols \, ", \t, \n, \r
  // will be preceded by an escape symbol.

  char c = s[0];
  bool contains_special_symbols = ((c =='-') || isdigit(c));

  for(std::string::const_iterator i=s.begin(); !contains_special_symbols && i!=s.end(); ++i)
  {
    if (*i=='\\' || *i=='"' || *i=='(' || *i==')' || *i=='[' || *i==']' || *i==',' || *i==' ' || *i=='\n' || *i=='\t' || *i=='\r')
    {
      contains_special_symbols=true;
    }
  }

  if (contains_special_symbols)
  {
    // This function symbol needs quotes.
    os << "\"";
    for(std::string::const_iterator i=s.begin(); i!=s.end(); ++i)
    {
      // We need to escape special characters.
      switch (*i)
      {
        case '\\':
        case '"':
          os << "\\" << *i;
          break;
        case '\n':
          os << "\\n";
          break;
        case '\t':
          os << "\\t";
          break;
        case '\r':
          os << "\\r";
          break;
        default:
          os << *i;
          break;
      }
    }
    os << "\"";
  }
  else
  {
    os << s;
  }
}

// Public functions

text_aterm_output::text_aterm_output(std::ostream& os, bool newline)
  : m_stream(os),
    m_newline(newline)
{}

void text_aterm_output::write_term(const aterm& term)
{
  write_term_line(term);

  if (m_newline)
  {
    m_stream << "\n";
  }
}

text_aterm_input::text_aterm_input(std::istream& is)
  : m_stream(is)
{
  character = next_char();
}

aterm text_aterm_input::read_term()
{
  aterm term;

  try
  {
    if (character != EOF)
    {
      term = parse_aterm(character);
    }
  }
  catch (atermpp::runtime_error& e)
  {
    throw atermpp::runtime_error(e.what() + string("\n") + print_parse_error_position());
  }

  // Reset the parsing error buffers.
  m_column = 0;
  m_history.clear();

  std::cerr << "Read term " << term << "\n";
  return term;
}

// Private functions

void text_aterm_output::write_term_line(const aterm& t)
{
  if (t.type_is_int())
  {
    // Write a single integer as is.
    m_stream << atermpp::down_cast<aterm_int>(t).value();
  }
  else if (t.type_is_list())
  {
    // A list l0...ln is formatted as [l0, ..., ln].
    m_stream << "[";
    const aterm_list& list = down_cast<aterm_list>(t);
    for (aterm_list::const_iterator it = list.begin(); it != list.end(); ++it)
    {
      if (it!=list.begin())
      {
        m_stream << ",";
      }
      write_term_line(*it);
    }

    m_stream << "]";
  }
  else
  {
    // An aterm_appl f(t0, ..., tn) is written as f(t0, ..., tn)
    assert(t.type_is_appl());

    const aterm_appl& appl = down_cast<aterm_appl>(t);
    const function_symbol& sym = appl.function();

    write_string_with_escape_symbols(sym.name(), m_stream);

    if (sym.arity() > 0)
    {
      m_stream << "(";
      write_term_line(appl[0]);
      for (std::size_t i = 1; i < sym.arity(); i++)
      {
        m_stream << ",";
        write_term_line(appl[i]);
      }
      m_stream << ")";
    }
  }
}

aterm text_aterm_input::parse_aterm(int& character)
{
  // Parse the term.
  switch (character)
  {
    case '"':
    {
      std::string function_name = parse_quoted_string(character);
      return parse_aterm_appl(function_name, character);
    }
    case '[':
    {
      return parse_aterm_list(character, '[', ']');
    }
    default:
    {
      if (isdigit(character) || character == '-')
      {
        return parse_aterm_int(character);
      }

      std::string function_symbol = parse_unquoted_string(character);
      return parse_aterm_appl(function_symbol, character);
    }
  }
}

aterm_appl text_aterm_input::parse_aterm_appl(const std::string& function_name, int& character)
{
  // Parse the arguments.
  aterm_list arguments = parse_aterm_list(character, '(', ')');

  // Wrap up this function application.
  function_symbol symbol(function_name, arguments.size());
  return aterm_appl(symbol, arguments.begin(), arguments.end());
}

aterm_int text_aterm_input::parse_aterm_int(int& character)
{
  std::array<char, 64> number;
  auto it = number.begin();

  if (character == '-')
  {
    *it = static_cast<char>(character);
    ++it;
    character = next_char(true, true);
  }

  while (isdigit(character) && it != number.end())
  {
    *it = static_cast<char>(character);
    ++it;
    character = next_char();
  }

  return aterm_int(static_cast<std::size_t>(atol(number.begin())));
}

aterm_list text_aterm_input::parse_aterm_list(int& character, char begin, char end)
{
  aterm_list list;

  // A list is [t0, ..., tn] or surrounded by ().
  if (character == begin)
  {
    character = next_char(true, true);
    if (character != end)
    {
      list.push_front(parse_aterm(character));

      while (character == ',')
      {
        character = next_char(true, true);
        list.push_front(parse_aterm(character));
      }
      while (character == ',');

      if (character != end)
      {
        throw atermpp::runtime_error(std::string("Missing ") + end + " while parsing a list term");
      }
    }

    character = next_char(true);
  }

  return reverse(list);
}


std::string text_aterm_input::print_parse_error_position()
{
  std::stringstream s;
  s << "Error occurred at line " << m_line << ", col " << m_column << " near: ";
  for(const auto& element : m_history)
  {
    s << element;
  }
  return s.str();
}

char text_aterm_input::next_char(bool skip_whitespace, bool required)
{
  char character = 0;

  do
  {
    int value = m_stream.get();

    if (value != EOF)
    {
      if (value == '\n')
      {
        m_line++;
        m_column = 0;
      }
      else
      {
        m_column++;
      }

      if (m_history.size() >= m_history_limit)
      {
        // If the history is full the first element must be removed.
        m_history.erase(m_history.begin());
      }

      m_history.emplace_back(value);
    }
    else if (required)
    {
      throw atermpp::runtime_error("Premature end of file while parsing.");
    }

    character = static_cast<char>(value);
  }
  while (isspace(character) && skip_whitespace);

  return character;
}

std::string text_aterm_input::parse_quoted_string(int& character)
{
  // We need a buffer for printing and parsing.
  std::string string;

  // First parse the identifier.
  character = next_char();

  while (character != '"')
  {
    switch (character)
    {
      case '\\':
        character = next_char(false, true);
        switch (character)
        {
          case 'n':
            string += '\n';
            break;
          case 'r':
            string += '\r';
            break;
          case 't':
            string += '\t';
            break;
          default:
            string += static_cast<char>(character);
            break;
        }
        break;
      default:
        string += static_cast<char>(character);
        break;
    }
    character = next_char(false, true);
  }

  character = next_char(true, false);
  return string;
}

std::string text_aterm_input::parse_unquoted_string(int& character)
{
  std::string string;

  if (character != '(')
  {
    // First parse the identifier
    while (character != '"' && character != '(' && character != ')' && character != ']'
      && character != ']' && character != ',' && character != ' ' && character != '\n'
      && character != '\t' && character != '\r' && character != EOF)
    {
      string += static_cast<char>(character);
      character = next_char(false);
    }
  }

  return string;
}

void write_term_to_text_stream(const aterm& term, std::ostream& os)
{
  text_aterm_output stream(os);
  stream.write_term(term);
}

aterm read_term_from_string(const std::string& s)
{
  stringstream ss(s);
  return  read_term_from_text_stream(ss);
}

aterm read_term_from_text_stream(istream& is)
{
  text_aterm_input stream(is);
  return stream.read_term();
}


std::ostream& operator<<(std::ostream& os, const aterm& term)
{
  text_aterm_output stream(os);
  stream.write_term(term);
  return os;
}

} // namespace atermpp
