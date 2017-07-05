// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file atermpp/source/aterm_io_text.cpp
/// \brief This file contains code to read and write aterms
///        in human readable format. 



#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cassert>
#include <stdexcept>
#include <deque>
#include <iostream>
#include <fstream>
#include <sstream>

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"


namespace atermpp
{

using namespace std;


/* Parse error description */
static std::size_t line = 0;
static std::size_t col = 0;
static deque<char> error_buf;
static const std::size_t MAX_ERROR_SIZE = 64;

/* Prototypes */
static aterm fparse_term(int* c, istream& is);

static void aterm_io_init()
{
  static bool initialized = false;
  if (!initialized)
  {
    /* Check for reasonably sized aterm (32 bits, 4 bytes)     */
    /* This check might break on perfectly valid architectures */
    /* that have char == 2 bytes, and sizeof(header_type) == 2 */
    assert(sizeof(std::size_t) == sizeof(aterm*));
    assert(sizeof(std::size_t) >= 4);
    initialized = true;
  }
}

static void write_string_with_escape_symbols(const std::string& s, std::ostream& os)
{
  // Check whether the string starts with a - or a number, or contains the symbols
  // \, ", (, ), [, ], comma, space, \t, \n or \r. If yes, the string will be
  // surrounded by quotes, and the symbols \, ", \t, \n, \r
  // will be preceded by an escape symbol.

  // assert(s.size()>0);
  char c=s[0];
  bool contains_special_symbols= c=='-' || isdigit(c);

  for(std::string::const_iterator i=s.begin(); !contains_special_symbols && i!=s.end(); ++i)
  {
    if (*i=='\\' || *i=='"' || *i=='(' || *i==')' || *i=='[' || *i==']' || *i==',' || *i==' ' || *i=='\n' || *i=='\t' || *i=='\r')
    {
      contains_special_symbols=true;
    }
  }

  if (contains_special_symbols)
  {
    /* This function symbol needs quotes */
    os << "\"";
    for(std::string::const_iterator i=s.begin(); i!=s.end(); ++i)
    {
      /* We need to escape special characters */
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

static void writeToStream(const aterm& t, std::ostream& os)
{
  if (t.type_is_int())
  {
    os << aterm_int(t).value();
  }
  else if (t.type_is_list())
  {
    os << "[";
    const aterm_list& list = down_cast<aterm_list>(t);
    for(aterm_list::const_iterator i=list.begin(); i!=list.end(); ++i)
    {
      if (i!=list.begin())
      {
        os << ",";
      }
      writeToStream(*i, os);
    }
    os << "]";
  }
  else // t.type_is_appl()
  {
    const aterm_appl& appl = down_cast<aterm_appl>(t);
    const function_symbol& sym = appl.function();
    write_string_with_escape_symbols(sym.name(),os);
    if (sym.arity() > 0)
    {
      os << "(";
      writeToStream(appl[0], os);
      for (std::size_t i = 1; i < sym.arity(); i++)
      {
        os << ",";
        writeToStream(appl[i], os);
      }
      os << ")";
    }
  }
}

/*
 * Write a term into its text representation.
 */

std::ostream& operator<<(std::ostream& out, const aterm& t)
{
  aterm_io_init();
  writeToStream(t, out);
  return out;
}

void write_term_to_text_stream(const aterm& t, std::ostream& os)
{
  aterm_io_init();
  writeToStream(t,os);
}

/*
 * Read the next character from file.
 */

static void fnext_char(int* c, istream& is)
{
  *c = is.get();
  if (*c != EOF)
  {
    if (*c == '\n')
    {
      line++;
      col = 0;
    }
    else
    {
      col++;
    }
    error_buf.push_back(*c);
    if (error_buf.size()>=MAX_ERROR_SIZE)
    {
      error_buf.pop_front();
    }
  }
}

static std::string print_parse_error_position()
{
  std::stringstream s;
  s << "Error occurred at line " << line << ", col " << col << " near: ";
  for(deque<char>::const_iterator i=error_buf.begin(); i!=error_buf.end(); ++i)
  {
    s << *i;
  }
  return s.str();
}

/*
 * Skip layout from file.
 */

static void fskip_layout(int* c, istream& is)
{
  while (isspace(*c))
  {
    fnext_char(c, is);
  }
}

/*
 * Skip layout from file.
 */

static void fnext_skip_layout(int* c, istream& is)
{
  do
  {
    fnext_char(c, is);
  }
  while (isspace(*c));
}

/*
 * Parse a list of arguments.
 */

static aterm_list fparse_terms(int* c, istream& is)
{
  if (*c==']' || *c==')')  // The termlist must be empty.
  {
    return aterm_list();
  }

  aterm el = fparse_term(c, is);
  aterm_list list;
  list.push_front(el);

  while (*c == ',')
  {
    fnext_skip_layout(c, is);
    el = fparse_term(c, is);
    list.push_front(el);
  }
  return reverse(list);
}

/*
 * Parse a quoted application.
 */

static string fparse_quoted_string(int* c, istream& is)
{
  /* We need a buffer for printing and parsing */
  std::string function_string;

  /* First parse the identifier */
  fnext_char(c, is);

  while (*c != '"')
  {
    switch (*c)
    {
      case EOF:
        throw atermpp::runtime_error("Premature end of file while parsing quoted function symbol.");
      case '\\':
        fnext_char(c, is);
        if (*c == EOF)
        {
          throw atermpp::runtime_error("Premature end of file while parsing quoted function symbol.");
        }
        switch (*c)
        {
          case 'n':
            function_string+='\n';
            break;
          case 'r':
            function_string+='\r';
            break;
          case 't':
            function_string+= '\t';
            break;
          default:
            function_string+= *c;
            break;
        }
        break;
      default:
        function_string+= *c;
        break;
    }
    fnext_char(c, is);
  }
  fnext_skip_layout(c, is);
  return function_string;
}

/* Parse an unquoted string. */

static string fparse_unquoted_string(int* c, istream& is)
{
  std::string function_string;
  if (*c != '(')
  {
    /* First parse the identifier */
    while (*c!='"' && *c!='(' && *c!=')' && *c!=']' && *c!=']' && *c!=',' && *c!=' ' && *c!='\n' && *c!='\t' && *c!='\r' && *c!=EOF)
    {
      function_string+= *c;
      fnext_char(c, is);
    }

    fskip_layout(c, is);
  }
  return function_string;
}

static aterm_appl parse_arguments(const string& f, int *c, istream& is)
{
  /* Time to parse the arguments */
  aterm_list args;
  if (*c == '(')
  {
    fnext_skip_layout(c, is);
    if (*c != ')')
    {
      args = fparse_terms(c, is);
    }

    if (*c != ')')
    {
      throw atermpp::runtime_error("Missing ')' while parsing term");
    }
    fnext_skip_layout(c, is);
  }

  /* Wrap up this function application */
  const function_symbol sym(f, args.size());
  return aterm_appl(sym, args.begin(), args.end());
}



/*
 * Parse a number.
 */

static aterm fparse_num(int* c, istream& is)
{
  char num[32], *ptr = num, *numend = num + 30;

  if (*c == '-')
  {
    *ptr++ = *c;
    fnext_char(c, is);
  }

  while (isdigit(*c) && ptr < numend)
  {
    *ptr++ = *c;
    fnext_char(c, is);
  }

  {
    *ptr = '\0';
    return aterm_int(static_cast<std::size_t>(atol(num)));
  }
}

/*
 * Parse a term from a stream.
 */

static aterm fparse_term(int* c, istream& is)
{
  switch (*c)
  {
    case '"':
    {
      string f=fparse_quoted_string(c, is);
      return parse_arguments(f,c,is);
    }
    case '[':
    {
      fnext_skip_layout(c, is);
      if (*c == ']')
      {
        fnext_skip_layout(c, is);
        return aterm_list(); // The empty list has just been parsed.
      }
      else
      {
        const aterm result = fparse_terms(c, is);
        if (*c != ']')
        {
          throw atermpp::runtime_error("Expecting ']' while parsing term");
        }
        fnext_skip_layout(c, is);
        return result;
      }
    }
    default:
    {
      if (isdigit(*c) || *c == '-')
      {
        return fparse_num(c, is);
      }
      string f=fparse_unquoted_string(c, is);
      return parse_arguments(f,c,is);
    }
  }
}

/*
 * Read from a string.
 */

aterm read_term_from_string(const std::string& s)
{
  stringstream ss(s);
  return  read_term_from_text_stream(ss);
}

aterm read_term_from_text_stream(istream& is)
{
  // Initialise global io (esp. for windows)
  aterm_io_init();

  // Initialise error handling.
  line = 0;
  col = 0;
  error_buf.clear();

  int c;
  fnext_skip_layout(&c, is);

  try
  {
    return fparse_term(&c, is);
  }
  catch (atermpp::runtime_error& e)
  {
    throw atermpp::runtime_error(e.what() + string("\n") + print_parse_error_position());
  }
}

bool is_binary_aterm_stream(std::istream& is)
{
  aterm_io_init();

  int c;
  fnext_char(&c, is);
  return (c == 0);
}

/*
 * Read an aterm from a file that could be binary or text.
 */
bool is_binary_aterm_file(const std::string& filename)
{
  if(filename.empty())
  {
    return is_binary_aterm_stream(std::cin);
  }
  else
  {
    std::ifstream is;
    is.open(filename.c_str());
    return is_binary_aterm_stream(is);
  }
}

} // namespace atermpp
