/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>
#include <stack>
#include <iostream>
#include <sstream>

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"


namespace atermpp
{

using namespace std;

/* defines */

static const size_t ERROR_SIZE = 32;

/* globals */

/* Parse error description */
static int      line = 0;
static int      col = 0;
static char     error_buf[ERROR_SIZE];
static int      error_idx = 0;

static aterm    fparse_term(int* c, istream &is);


static void aterm_io_init()
{
  static bool initialized = false;
  if (initialized)
  {
    return;
  }

  /* Check for reasonably sized aterm (32 bits, 4 bytes)     */
  /* This check might break on perfectly valid architectures */
  /* that have char == 2 bytes, and sizeof(header_type) == 2 */
  assert(sizeof(size_t) == sizeof(aterm*));
  assert(sizeof(size_t) >= 4);

#ifdef WIN32
  if (_setmode(_fileno(stdin), _O_BINARY) == -1)
  {
    perror("Warning: Cannot set inputstream to binary mode.");
  }
  if (_setmode(_fileno(stdout), _O_BINARY) == -1)
  {
    perror("Warning: Cannot set outputstream to binary mode.");
  }
  if (_setmode(_fileno(stderr), _O_BINARY) == -1)
  {
    perror("Warning: Cannot set errorstream to binary mode.");
  }
#endif

  initialized = true;
}


static void write_string_with_escape_symbols(const std::string &s, std::ostream& os)
{
  std::string::const_iterator id = s.begin();

  /* This function symbol needs quotes */
  while (id!=s.end())
  {
    /* We need to escape special characters */
    switch (*id)
    {
      case '\\':
      case '"':
        os << "\\" << *id;
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
        os << *id;
        break;
    }
    ++id;
  }
}

static void topWriteToStream(const aterm &t, std::ostream& os);

static void writeToStream(const aterm &t, std::ostream& os)
{
  switch (t.type())
  {
    case AT_INT:
    {
      os << aterm_int(t).value();
      break;
    }
    case AT_APPL:
    {
      const aterm_appl &appl = aterm_cast<aterm_appl>(t);
      const function_symbol sym = appl.function();
      os << "\""; 
      write_string_with_escape_symbols(sym.name(),os);
      os << "\"";
      if (sym.arity() > 0)
      {
        os << "(";
        topWriteToStream(appl(0), os);
        for (size_t i = 1; i < sym.arity(); i++)
        {
          os << ",";
          topWriteToStream(appl(i), os);
        }
        os << ")";
      }
      break;
    }
    case AT_LIST:
    {
      aterm_list list = aterm_cast<aterm_list>(t);
      if (!list.empty())
      {
        topWriteToStream(list.front(), os);
        list = list.tail();
        while (!list.empty())
        {
          os << ",";
          topWriteToStream(list.front(), os);
          list = list.tail();
        }
      }
      break;
    }
  }
}

static void topWriteToStream(const aterm &t, std::ostream& os)
{
  if (t.type() == AT_LIST)
  {
    os << "[";
    writeToStream(t, os);
    os << "]";
  }
  else
  {
    writeToStream(t, os);
  }
}

/**
 * Write a term into its text representation.
 */

std::string aterm::to_string() const
{
  aterm_io_init();
  std::ostringstream oss;
  topWriteToStream(*this, oss);
  return oss.str();
}

void write_term_to_text_stream(const aterm &t, std::ostream &os)
{
  aterm_io_init();
  topWriteToStream(t,os);
}

/**
 * Read the next character from file.
 */

static void fnext_char(int* c, istream &is)
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
    error_buf[error_idx++] = *c;
    error_idx %= ERROR_SIZE;
  }
}

/**
 * Skip layout from file.
 */

static void fskip_layout(int* c, istream &is)
{
  while (isspace(*c))
  {
    fnext_char(c, is);
  }
}

/**
 * Skip layout from file.
 */

static void fnext_skip_layout(int* c, istream &is)
{
  do
  {
    fnext_char(c, is);
  }
  while (isspace(*c));
}

/**
 * Parse a list of arguments.
 */

static aterm_list fparse_terms(int* c, istream &is)
{
  aterm_list list;
  aterm el = fparse_term(c, is);

  if (!el.defined())
  {
    return aterm_list();
  }

  list = push_front(aterm_list(), el);

  while (*c == ',')
  {
    fnext_skip_layout(c, is);
    el = fparse_term(c, is);
    if (!el.defined())
    {
      return aterm_list();
    }
    list = push_front(list, el);
  }

  return reverse(list);
}

/**
 * Parse a quoted application.
 */

static aterm fparse_quoted_appl(int* c, istream &is)
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
        return aterm();
      case '\\':
        fnext_char(c, is);
        if (*c == EOF)
        {
          return aterm();
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

  /* Time to parse the arguments */
  aterm_list args;
  if (*c == '(')
  {
    fnext_skip_layout(c, is);
    if (*c != ')')
    {
      args = fparse_terms(c, is);
    }

    if (!args.defined() || *c != ')')
    {
      return aterm();
    }
    fnext_skip_layout(c, is);
  }

  /* Wrap up this function application */
  const function_symbol sym(function_string, args.size());
  
  return aterm_appl(sym, args.begin(), args.end());
}

/**
 *  * Parse an unquoted application. If a term is printed, applications
 *    are always quoted. But when typing aterms, it is convenient not to be
 *    forced to type quoted around each string. Therefore non quoted 
 *    function symbols are still allowed.
 *   */


static aterm_appl fparse_unquoted_appl(int* c, istream &is)
{
  std::string function_string;
  if (*c != '(')
  {
    /* First parse the identifier */
    while (isalnum(*c)
           || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      function_string+= *c;
      fnext_char(c, is);
    }

    fskip_layout(c, is);
  }

  if (function_string.empty())
  {
    return aterm_appl();
  }

  /* Time to parse the arguments */
  aterm_list args;
  if (*c == '(')
  {
    fnext_skip_layout(c, is);
    if (*c != ')')
    {
      args = fparse_terms(c, is);
    }

    if (!args.defined() || *c != ')')
    {
      return aterm_appl();
    }
    fnext_skip_layout(c, is);
  }

  /* Wrap up this function application */
  const function_symbol sym(function_string, args.size());
  return aterm_appl(sym, args.begin(), args.end());
}



/**
 * Parse a number.
 */

static aterm fparse_num(int* c, istream &is)
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
    return aterm_int(static_cast<size_t>(atol(num)));
  }
}

/**
 * Parse a term from file.
 */

static aterm fparse_term(int* c, istream &is)
{
  aterm result;

  switch (*c)
  {
    case '"':
      result = (aterm) fparse_quoted_appl(c, is);
      break;
    case '[':
      fnext_skip_layout(c, is);
      if (*c == ']')
      {
        result = aterm_list();
      }
      else
      {
        result = fparse_terms(c, is);
        if (!result.defined() || *c != ']')
        {
          return aterm();
        }
      }
      fnext_skip_layout(c, is);
      break;
    default:
      if (isalpha(*c) || *c == '(')
      {
        result = fparse_unquoted_appl(c, is);
      }
      else if (isdigit(*c) || *c == '-')
      {
        result = fparse_num(c, is);
      }
      else
      {
        result = aterm();
      }
      break;
  }

  return result;
}

/**
 * Read a term from a text file. The first character has been read.
 */

static aterm read_term_from_text_stream(int *c, istream &is)
{
  fskip_layout(c, is);

  aterm term = fparse_term(c, is);

  if (term.address())
  {
    is.unget();
  }
  else
  {
    std::stringstream os;
    os << "parse error at line " << line << ", col " << col << ":";
    for (size_t i = 0; i < ERROR_SIZE; ++i)
    {
      char c = error_buf[(i + error_idx) % ERROR_SIZE];
      if (c)
      {
        os << c;
      }
    }
    throw std::runtime_error(os.str());
  }

  return term;
} 


/**
 * Read an aterm from a file that could be binary or text.
 */

aterm read_term_from_stream(istream &is)
{
  aterm_io_init();
  int c;

  fnext_char(&c, is);
  if (c == 0)
  {
    return read_term_from_binary_stream(is);  
  }
  else
  {
    /* Probably a text file */
    line = 0;
    col = 0;
    error_idx = 0;
    memset(error_buf, 0, ERROR_SIZE);

    return read_term_from_text_stream(&c, is);
  }
}

/**
 * Read from a string.
 */

aterm read_term_from_string(const std::string& s)
{
  stringstream ss(s);
  return  read_term_from_stream(ss);
}

aterm read_term_from_text_stream(istream &is)
{
  aterm_io_init();
  int             c;

  fnext_skip_layout(&c, is);

  aterm term = fparse_term(&c, is);

  if (!term.defined())
  {
    mCRL2log(mcrl2::log::error) << "ATreadFromString: parse error at or near:" << std::endl
                                << is << std::endl;
  }

  return term;
}



} // namespace atermpp
