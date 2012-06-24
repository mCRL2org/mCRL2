/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <stdexcept>
#include <stack>

#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "mcrl2/utilities/logger.h"
#include "mcrl2/aterm/aterm.h"

/*}}}  */

namespace aterm_deprecated
{

/*  defines */

static const size_t DEFAULT_BUFFER_SIZE = 4096;

static const int ERROR_SIZE = 32;

/*{{{  globals */

char            aterm_id[] = "$Id$";

/* Flag set when ATinit is called. */

/* We need a buffer for printing and parsing */
static std::string string_buffer;

/* Parse error description */
static int      line = 0;
static int      col = 0;
static char     error_buf[ERROR_SIZE];
static int      error_idx = 0;

/*}}}  */
/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char* _strdup(const char* s);
#endif

static ATerm    fparse_term(int* c, FILE* f);
static ATerm    sparse_term(int* c, char** s);
// int ATvfprintf(FILE* stream, const char* format, va_list args);

/*}}}  */
/*{{{  int ATfprintf(FILE *stream, const char *format, ...) */

/**
 * Extension of fprintf() with ATerm-support.
 */

/* int
ATfprintf(FILE* stream, const char* format,...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = ATvfprintf(stream, format, args);
  va_end(args);

  return result;
} */
/*}}}  */
/*{{{  int ATvfprintf(FILE *stream, const char *format, va_list args) */

/* int ATvfprintf(FILE* stream, const char* format, va_list args)
{
  const char*     p;
  char*           s;
  char            fmt[16];
  int             result = 0;

  for (p = format; *p; p++)
  {
    if (*p != '%')
    {
      fputc(*p, stream);
      continue;
    }

    s = fmt;
    while (!isalpha((int) *p))  / * parse formats %-20s, etc. * /
    {
      *s++ = *p++;
    }
    *s++ = *p;
    *s = '\0';

    switch (*p)
    {
      case 'c':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
        fprintf(stream, fmt, va_arg(args, int));
        break;

      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
        fprintf(stream, fmt, va_arg(args, double));
        break;

      case 'p':
        fprintf(stream, fmt, va_arg(args, void*));
        break;

      case 's':
        fprintf(stream, fmt, va_arg(args, char*));
        break;

        / *
         * ATerm specifics start here: "%t" to print an ATerm; "%l" to
         * print a list; "%y" to print a AFun; "%n" to print a single
         * ATerm node
         * /
      case 't':
        ATwriteToTextFile(va_arg(args, detail::_aterm*), stream);
        break;
      case 'l':
        {
          detail::_aterm_list<aterm>* l = va_arg(args, detail::_aterm_list<aterm>*);
          fmt[strlen(fmt) - 1] = '\0';  / * Remove 'l' * /
          while (!ATisEmpty(l))
          {
            ATwriteToTextFile(ATgetFirst(l), stream);
            l = &*(l->tail);
            if (!ATisEmpty(l))
            {
              fputs(fmt + 1, stream);
            }
          }
          break;
        }
      case 'a':
      case 'y':
        AT_printAFun(va_arg(args, size_t), stream);
        break;
      default:
        fputc(*p, stream);
        break;
    }
  }
  return result;
} */


/* bool ATwriteToNamedTextFile(const ATerm &t, const char* name)
{
  FILE*  f;
  bool result;

  if (!strcmp(name, "-"))
  {
    return ATwriteToTextFile(t, stdout);
  }

  if (!(f = fopen(name, "wb")))
  {
    return false;
  }

  result = ATwriteToTextFile(t, f);
  fclose(f);

  return result;
} */

/*}}}  */

/*{{{  static char *writeToStream(ATerm t, std::ostream& os) */

static void topWriteToStream(const ATerm &t, std::ostream& os);

static void
writeToStream(const ATerm &t, std::ostream& os)
{
  ATermList list;
  ATermAppl appl;
  AFun sym;
  size_t i, arity;

  switch (ATgetType(t))
  {
    case AT_INT:
    {
      os << ATgetInt((ATermInt) t);
      break;
    }
    case AT_APPL:
    {
      appl = (ATermAppl) t;
      sym = ATgetAFun(appl);
      arity = ATgetArity(sym);
      const std::string name = ATgetName(sym);
      os << ATwriteAFunToString(sym);
      if (arity > 0 || (!ATisQuoted(sym) && !name.empty()))
      {
        os << "(";
        if (arity > 0)
        {
          topWriteToStream(ATgetArgument(appl, 0), os);
          for (i = 1; i < arity; i++)
          {
            os << ",";
            topWriteToStream(ATgetArgument(appl, i), os);
          }
        }
        os << ")";
      }
      break;
    }
    case AT_LIST:
    {
      list = (ATermList) t;
      if (!ATisEmpty(list))
      {
        topWriteToStream(ATgetFirst(list), os);
        list = ATgetNext(list);
        while (!ATisEmpty(list))
        {
          os << ",";
          topWriteToStream(ATgetFirst(list), os);
          list = ATgetNext(list);
        }
      }
      break;
    }
  }
}

static void
topWriteToStream(const ATerm &t, std::ostream& os)
{
  if (ATgetType(t) == AT_LIST)
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

/*}}}  */

/**
 * Write a term into its text representation.
 */

/*{{{  static void fnext_char(int *c, FILE *f) */

/**
 * Read the next character from file.
 */

static void
fnext_char(int* c, FILE* f)
{
  *c = fgetc(f);
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

/*}}}  */
/*{{{  static void fskip_layout(int *c, FILE *f) */

/**
 * Skip layout from file.
 */

static void
fskip_layout(int* c, FILE* f)
{
  while (isspace(*c))
  {
    fnext_char(c, f);
  }
}

/*}}}  */
/*{{{  static void fnext_skip_layout(int *c, FILE *f) */

/**
 * Skip layout from file.
 */

static void
fnext_skip_layout(int* c, FILE* f)
{
  do
  {
    fnext_char(c, f);
  }
  while (isspace(*c));
}

/*}}}  */

/*{{{  static ATermList fparse_terms(int *c, FILE *f) */

/**
 * Parse a list of arguments.
 */

static ATermList
fparse_terms(int* c, FILE* f)
{
  ATermList list;
  ATerm el = fparse_term(c, f);

  if (el == ATerm())
  {
    return ATermList();
  }

  list = ATinsert(aterm_list(), el);

  while (*c == ',')
  {
    fnext_skip_layout(c, f);
    el = fparse_term(c, f);
    if (el == ATerm())
    {
      return ATermList();
    }
    list = ATinsert(list, el);
  }

  return ATreverse(list);
}

/*}}}  */
/*{{{  static ATermAppl fparse_quoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static ATerm
fparse_quoted_appl(int* c, FILE* f)
{
  assert(string_buffer.empty());
  ATermList       args = aterm_list();
  AFun          sym;
  char*           name;

  /* First parse the identifier */
  fnext_char(c, f);

  while (*c != '"')
  {
    switch (*c)
    {
      case EOF:
        return ATerm();
      case '\\':
        fnext_char(c, f);
        if (*c == EOF)
        {
          return ATerm();
        }
        switch (*c)
        {
          case 'n':
            string_buffer+='\n';
            break;
          case 'r':
            string_buffer+='\r';
            break;
          case 't':
            string_buffer+= '\t';
            break;
          default:
            string_buffer+= *c;
            break;
        }
        break;
      default:
        string_buffer+= *c;
        break;
    }
    fnext_char(c, f);
  }

  name = _strdup(string_buffer.c_str());
  string_buffer.clear();
  if (!name)
  {
    throw std::runtime_error("fparse_quoted_appl: symbol too long.");
  }

  fnext_skip_layout(c, f);

  /* Time to parse the arguments */
  if (*c == '(')
  {
    fnext_skip_layout(c, f);
    if (*c != ')')
    {
      args = fparse_terms(c, f);
    }
    else
    {
      args = aterm_list();
    }
    if (args == ATerm() || *c != ')')
    {
      return ATerm();
    }
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = AFun(name, ATgetLength(args), true);
  free(name);
  return ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static ATermAppl fparse_unquoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static ATermAppl
fparse_unquoted_appl(int* c, FILE* f)
{
  assert(string_buffer.empty());
  AFun sym;
  ATermList args = aterm_list();
  char* name = NULL;

  if (*c != '(')
  {
    /* First parse the identifier */
    while (isalnum(*c)
           || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      string_buffer+= *c;
      fnext_char(c, f);
    }
    name = _strdup(string_buffer.c_str());
    string_buffer.clear();
    if (!name)
    {
      throw std::runtime_error("fparse_unquoted_appl: symbol too long.");
    }

    fskip_layout(c, f);
  }

  /* Time to parse the arguments */
  if (*c == '(')
  {
    fnext_skip_layout(c, f);
    if (*c != ')')
    {
      args = fparse_terms(c, f);
    }
    else
    {
      args = aterm_list();
    }
    if (args == ATerm() || *c != ')')
    {
      return NULL;
    }
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = AFun(name ? name : "", ATgetLength(args), false);
  if (name != NULL)
  {
    free(name);
  }

  return ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static void fparse_num(int *c, FILE *f) */

/**
 * Parse a number or blob.
 */

static ATerm
fparse_num(int* c, FILE* f)
{
  char            num[32], *ptr = num, *numend = num + 30;

  if (*c == '-')
  {
    *ptr++ = *c;
    fnext_char(c, f);
  }

  while (isdigit(*c) && ptr < numend)
  {
    *ptr++ = *c;
    fnext_char(c, f);
  }

  {
    /*{{{  An integer */

    *ptr = '\0';
    return ATmakeInt(atoi(num));

    /*}}}  */
  }
}

/*}}}  */
/*{{{  static ATerm fparse_term(int *c, FILE *f) */

/**
 * Parse a term from file.
 */

static ATerm
fparse_term(int* c, FILE* f)
{
  /* ATerm t, result = NULL; */
  ATerm result;

  switch (*c)
  {
    case '"':
      result = (ATerm) fparse_quoted_appl(c, f);
      break;
    case '[':
      fnext_skip_layout(c, f);
      if (*c == ']')
      {
        result = aterm_list();
      }
      else
      {
        result = fparse_terms(c, f);
        if (result == ATerm() || *c != ']')
        {
          return ATerm();
        }
      }
      fnext_skip_layout(c, f);
      break;
    default:
      if (isalpha(*c) || *c == '(')
      {
        result = fparse_unquoted_appl(c, f);
      }
      else if (isdigit(*c))
      {
        result = fparse_num(c, f);
      }
      else if (*c == '.' || *c == '-')
      {
        result = fparse_num(c, f);
      }
      else
      {
        result = ATerm();
      }
      break;
  }

  return result;
}

/*}}}  */

static inline
void snext_char(int* c, char** s)
{
  *c = (unsigned char)*(*s)++;
}

static inline
void sskip_layout(int* c, char** s)
{
  while (isspace(*c))
  {
    snext_char(c,s);
  }
}

static inline
void snext_skip_layout(int* c, char** s)
{
  do
  {
    snext_char(c, s);
  }
  while (isspace(*c));
}

/*{{{  static ATermList sparse_terms(int *c, char **s) */

/**
 * Parse a list of arguments.
 */

static ATermList
sparse_terms(int* c, char** s)
{
  ATerm el = sparse_term(c, s);
  if (el == ATerm())
  {
    return ATermList();
  }

  ATermList list = ATinsert(aterm_list(), el);

  while (*c == ',')
  {
    snext_skip_layout(c, s);
    el = sparse_term(c, s);
    if (el == ATerm())
    {
      return ATermList();
    }
    list = ATinsert(list, el);
  }

  return ATreverse(list);
}

/*{{{  static ATermAppl sparse_quoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static ATerm
sparse_quoted_appl(int* c, char** s)
{
  assert(string_buffer.empty());
  ATermList       args = aterm_list();
  AFun          sym;
  char*           name;

  /* First parse the identifier */
  snext_char(c, s);

  while (*c != '"')
  {
    switch (*c)
    {
      case EOF:
        /*  case '\n':
          case '\r':
          case '\t':
          */
        return ATerm();
      case '\\':
        snext_char(c, s);
        if (*c == EOF)
        {
          return ATerm();
        }
        switch (*c)
        {
          case 'n':
            string_buffer+= '\n';
            break;
          case 'r':
            string_buffer+= '\r';
            break;
          case 't':
            string_buffer+= '\t';
            break;
          default:
            string_buffer+= *c;
            break;
        }
        break;
      default:
        string_buffer+= *c;
        break;
    }
    snext_char(c, s);
  }

  name = _strdup(string_buffer.c_str());
  string_buffer.clear();

  if (!name)
  {
    throw std::runtime_error("fparse_quoted_appl: symbol too long.");
  }

  snext_skip_layout(c, s);

  /* Time to parse the arguments */
  if (*c == '(')
  {
    snext_skip_layout(c, s);
    if (*c != ')')
    {
      args = sparse_terms(c, s);
    }
    else
    {
      args = aterm_list();
    }
    if (args == ATermList() || *c != ')')
    {
      return ATerm();
    }
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = AFun(name, ATgetLength(args), true);
  free(name);
  return ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static ATermAppl sparse_unquoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static ATermAppl
sparse_unquoted_appl(int* c, char** s)
{
  assert(string_buffer.empty());
  AFun sym;
  ATermList args = aterm_list();
  char* name = NULL;

  if (*c != '(')
  {
    /* First parse the identifier */
    while (isalnum(*c)
           || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      string_buffer+= *c;
      snext_char(c, s);
    }
    name = _strdup(string_buffer.c_str());
    string_buffer.clear();
    if (!name)
    {
      throw std::runtime_error("sparse_unquoted_appl: symbol too long.");
    }

    sskip_layout(c, s);
  }

  /* Time to parse the arguments */
  if (*c == '(')
  {
    snext_skip_layout(c, s);
    if (*c != ')')
    {
      args = sparse_terms(c, s);
    }
    else
    {
      args = aterm_list();
    }
    if (args == ATerm() || *c != ')')
    {
      return NULL;
    }
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = AFun(name ? name : "", ATgetLength(args), false);
  if (name != NULL)
  {
    free(name);
  }

  return ATmakeApplList(sym, args);
}

/**
 * Parse a number
 */

static ATerm
sparse_num(int* c, char** s)
{
  char            num[32], *ptr = num;

  if (*c == '-')
  {
    *ptr++ = *c;
    snext_char(c, s);
  }

  while (isdigit(*c))
  {
    *ptr++ = *c;
    snext_char(c, s);
  }
  {
    /*{{{  An integer */

    *ptr = '\0';
    return ATmakeInt(atoi(num));

    /*}}}  */
  }
}


/**
 * Parse a term from file.
 */

static ATerm
sparse_term(int* c, char** s)
{
  /* ATerm t, result = NULL; */
  ATerm result;

  switch (*c)
  {
    case '"':
      result = sparse_quoted_appl(c, s);
      break;
    case '[':
      snext_skip_layout(c, s);
      if (*c == ']')
      {
        result = aterm_list();
      }
      else
      {
        result = sparse_terms(c, s);
        if (result == ATerm() || *c != ']')
        {
          return ATerm();
        }
      }
      snext_skip_layout(c, s);
      break;
    default:
      if (isalpha(*c) || *c == '(')
      {
        result = sparse_unquoted_appl(c, s);
      }
      else if (isdigit(*c))
      {
        result = sparse_num(c, s);
      }
      else if (*c == '.' || *c == '-')
      {
        result = sparse_num(c, s);
      }
      else
      {
        result = ATerm();
      }
      break;
  }

  if (result != ATerm())
  {
    sskip_layout(c, s);
  }
  return result;
}

} // namespace aterm_deprecated
