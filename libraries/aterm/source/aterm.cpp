/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
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

/*{{{  defines */

static const size_t DEFAULT_BUFFER_SIZE = 4096;

static const int ERROR_SIZE = 32;

/* Initial number of terms that can be protected */
/* In the current implementation this means that
   excessive term protection can lead to deteriorating
   performance! */
// static const size_t INITIAL_PROT_TABLE_SIZE = 100003;
// static const size_t PROTECT_EXPAND_SIZE = 100000;

/* The same for protection function */
// static const size_t PROTECT_FUNC_INITIAL_SIZE = 32;
// static const size_t PROTECT_FUNC_EXPAND_SIZE = 32;

/*}}}  */
/*{{{  globals */

char            aterm_id[] = "$Id$";

/* Flag set when ATinit is called. */
static bool initialized = false;

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
int ATvfprintf(FILE* stream, const char* format, va_list args);

/*{{{  int ATprintf(const char *format, ...) */

/**
 * Extension of printf() with ATerm-support.
 */

int
ATprintf(const char* format,...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = ATvfprintf(stdout, format, args);
  va_end(args);

  return result;
}

/*}}}  */
/*{{{  int ATfprintf(FILE *stream, const char *format, ...) */

/**
 * Extension of fprintf() with ATerm-support.
 */

int
ATfprintf(FILE* stream, const char* format,...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = ATvfprintf(stream, format, args);
  va_end(args);

  return result;
}
/*}}}  */
/*{{{  int ATvfprintf(FILE *stream, const char *format, va_list args) */

int ATvfprintf(FILE* stream, const char* format, va_list args)
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
    while (!isalpha((int) *p))  /* parse formats %-20s, etc. */
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

        /*
         * ATerm specifics start here: "%t" to print an ATerm; "%l" to
         * print a list; "%y" to print a AFun; "%n" to print a single
         * ATerm node
         */
      case 't':
        ATwriteToTextFile(va_arg(args, _ATerm*), stream);
        break;
      case 'l':
        {
          _ATermList* l = va_arg(args, _ATermList*);
          fmt[strlen(fmt) - 1] = '\0';  /* Remove 'l' */
          while (!ATisEmpty(l))
          {
            ATwriteToTextFile(ATgetFirst(l), stream);
            l = l->tail;
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
      /* case 'n':
        {
          _ATerm* t = va_arg(args, _ATerm*);
          switch (ATgetType(t))
          {
            case AT_INT:
            case AT_LIST:
              fprintf(stream, "[...(%zu)]", ATgetLength((ATermList) t));
              break;

            case AT_APPL:
              if (AT_isValidAFun(ATgetAFun((ATermAppl)t)))
              {
                AT_printAFun(ATgetAFun((ATermAppl)t), stream);
                fprintf(stream, "(...(%zu))",
                        GET_ARITY(t->header));
              }
              else
              {
                fprintf(stream, "<sym>(...(%zu))",
                        GET_ARITY(t->header));
              }
              break;
            case AT_FREE:
              fprintf(stream, "@");
              break;
            default:
              fprintf(stream, "#");
              break;
          }
          break;
        } */
      default:
        fputc(*p, stream);
        break;
    }
  }
  return result;
}

/*}}}  */

/*{{{  ATbool ATwriteToTextFile(ATerm t, FILE *f) */

/**
 * Write a term in text format to file.
 */

/* static bool
writeToTextFile(const ATerm &t, FILE* f)
{
  AFun          sym;
  ATerm           arg;
  size_t    i, arity; / * size; * /
  ATermAppl       appl;
  ATermList       list;
  char*            name;

  assert(t->reference_count>0);
  switch (ATgetType(t))
  {
    case AT_INT:
      fprintf(f, "%d", ATgetInt((ATermInt)t));
      break;
    case AT_APPL:
      / *{{{  Print application * /

      appl = (ATermAppl) t;

      sym = ATgetAFun(appl);
      AT_printAFun(sym.number(), f);
      arity = ATgetArity(sym);
      name = ATgetName(sym);
      if (arity > 0 || (!ATisQuoted(sym) && *name == '\0'))
      {
        fputc('(', f);
        for (i = 0; i < arity; i++)
        {
          if (i != 0)
          {
            fputc(',', f);
          }
          arg = ATgetArgument(appl, i);
          ATwriteToTextFile(arg, f);
        }
        fputc(')', f);
      }

      / *}}}  * /
      break;
    case AT_LIST:
      / *{{{  Print list * /

      list = (ATermList) t;
      if (!ATisEmpty(list))
      {
        ATwriteToTextFile(ATgetFirst(list), f);
        list = ATgetNext(list);
      }
      while (!ATisEmpty(list))
      {
        fputc(',', f);
        ATwriteToTextFile(ATgetFirst(list), f);
        list = ATgetNext(list);
      }

      / *}}}  * /
      break;
  }

  return true;
} */

/*}}}  */

/*{{{  ATerm ATwriteToNamedTextFile(char *name) */

/**
 * Write an ATerm to a named plaintext file
 */

bool ATwriteToNamedTextFile(const ATerm &t, const char* name)
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
}

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
  char* name;

  switch (ATgetType(t))
  {
    case AT_INT:
      os << ATgetInt((ATermInt) t);
      break;
    case AT_APPL:
      appl = (ATermAppl) t;
      sym = ATgetAFun(appl);
      arity = ATgetArity(sym);
      name = ATgetName(sym);
      os << ATwriteAFunToString(sym);
      if (arity > 0 || (!ATisQuoted(sym) && *name == '\0'))
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

    case AT_LIST:
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

  list = ATinsert(ATempty, el);

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
  ATermList       args = ATempty;
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
      args = ATempty;
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
  ATermList args = ATempty;
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
      args = ATempty;
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
        result = ATempty;
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

/*{{{  ATerm readFromTextFile(FILE *file) */

/**
 * Read a term from a text file. The first character has been read.
 */

static ATerm
readFromTextFile(int* c, FILE* file)
{
  ATerm term;
  fskip_layout(c, file);

  term = fparse_term(c, file);

  if (&*term)
  {
    ungetc(*c, file);
  }
  else
  {
    int i;
    mCRL2log(mcrl2::log::error) << "readFromTextFile: parse error at line " << line
                                << ", col " << col << ((line||col)?":\n":"");
    for (i = 0; i < ERROR_SIZE; ++i)
    {
      char c = error_buf[(i + error_idx) % ERROR_SIZE];
      if (c)
      {
        mCRL2log(mcrl2::log::error) << c;
      }
    }
    mCRL2log(mcrl2::log::error) << std::endl;
  }

  return term;
}

/*}}}  */
/*{{{  ATerm ATreadFromTextFile(FILE *file) */

/**
 * Read a term from a text file.
 */

ATerm
ATreadFromTextFile(FILE* file)
{
  int c;

  line = 0;
  col = 0;
  error_idx = 0;
  memset(error_buf, 0, ERROR_SIZE);

  fnext_char(&c, file);
  return readFromTextFile(&c, file);
}

/*}}}  */
/*{{{  ATerm ATreadFromNamedFile(char *name) */

/**
 * Read an ATerm from a named file
 */

ATerm ATreadFromNamedFile(const char* name)
{
  FILE*  f;
  ATerm t;

  if (!strcmp(name, "-"))
  {
    return ATreadFromFile(stdin);
  }

  if (!(f = fopen(name, "rb")))
  {
    return ATerm();
  }

  t = ATreadFromFile(f);
  fclose(f);

  return t;
}

/*}}}  */

inline
void snext_char(int* c, char** s)
{
  *c = (unsigned char)*(*s)++;
}
//#define snext_char(c,s) ((*c) = ((unsigned char)*(*s)++))

inline
void sskip_layout(int* c, char** s)
{
  while (isspace(*c))
  {
    snext_char(c,s);
  }
}
//#define sskip_layout(c,s) while(isspace(*c)) snext_char(c,s)

inline
void snext_skip_layout(int* c, char** s)
{
  do
  {
    snext_char(c, s);
  }
  while (isspace(*c));
}
//#define snext_skip_layout(c,s) do { snext_char(c, s); } while(isspace(*c))

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

  ATermList list = ATinsert(ATempty, el);

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
  ATermList       args = ATempty;
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
      args = ATempty;
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
  ATermList args = ATempty;
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
      args = ATempty;
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

/*}}}  */
/*{{{  static void sparse_num(int *c, char **s) */

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

/*}}}  */

/*{{{  static ATerm sparse_term(int *c, char **s) */

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
        result = ATempty;
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

/*}}}  */

/*{{{  void AT_markTerm(ATerm t) */

/**
 * Mark a term and all of its children.
 */
/* void AT_markTerm(const ATerm t1)
{
  return 0;

  if (IS_MARKED(t1->header))
  {
    return;
  }

  std::stack < ATerm > mark_stack;
  mark_stack.push(t1);

  while (!mark_stack.empty())
  {
    const ATerm t=mark_stack.top();
    mark_stack.pop();

    if (!(IS_MARKED(t->header)))
    {
      SET_MARK(t->header);

      switch (GET_TYPE(t->header))
      {
        case AT_INT:
          break;

        case AT_APPL:
          {
            const AFun sym = ATgetAFun((ATermAppl) t);

            if (AT_isValidAFun(sym.number()))
            {
              AT_markAFun(sym);
            }
            else
            {
              continue;
            }
            size_t arity = GET_ARITY(t->header);
            if (arity > MAX_INLINE_ARITY)
            {
              arity = ATgetArity(sym);
            }
            for (size_t i = 0; i < arity; i++)
            {
              mark_stack.push((ATerm)ATgetArgument((ATermAppl) t, i));
            }
            break;
          }
        case AT_LIST:
          if (!ATisEmpty((ATermList) t))
          {
            mark_stack.push(ATgetNext((ATermList) t));
            mark_stack.push(ATgetFirst((ATermList) t));
          }
          break;

      }
    }
  } 
} */


/*}}}  */
/*{{{  void AT_unmarkIfAllMarked(ATerm t) */

/* void AT_unmarkIfAllMarked(const ATerm &t)
{
  if (IS_MARKED(t->header))
  {
    CLR_MARK(t->header);
    switch (ATgetType(t))
    {
      case AT_INT:
        break;

      case AT_LIST:
      {
        ATermList list = (ATermList)t;
        while (!ATisEmpty(list))
        {
          CLR_MARK(list->header);
          AT_unmarkIfAllMarked(ATgetFirst(list));
          list = ATgetNext(list);
          if (!IS_MARKED(list->header))
          {
            break;
          }
        }
        CLR_MARK(list->header);
      }
      break;
      case AT_APPL:
      {
        ATermAppl appl = (ATermAppl)t;
        size_t cur_arity, cur_arg;
        AFun sym;

        sym = ATgetAFun(appl);
        AT_unmarkAFun(sym);
        cur_arity = ATgetArity(sym);
        for (cur_arg=0; cur_arg<cur_arity; cur_arg++)
        {
          AT_unmarkIfAllMarked(ATgetArgument(appl, cur_arg));
        }
      }
      break;
      default:
        throw std::runtime_error("collect_terms: illegal term");
        break;
    }

  }
}  */

/*}}}  */

/**
 * Calculate the number of unique symbols.
 */

/*{{{  static size_t calcUniqueAFuns(ATerm t) */

static size_t calcUniqueAFuns(const ATerm &t, std::set<ATerm> &visited)
{
  size_t nr_unique = 0;
  size_t  i, arity;
  AFun sym;
  ATermList list;

  // if (IS_MARKED(t->header))
  if (visited.count(t)>0)
  {
    return 0;
  }

  switch (ATgetType(t))
  {
    case AT_INT:
      if (!AFun::at_lookup_table[AS_INT.number()]->count++)
      {
        nr_unique = 1;
      }
      break;

    case AT_APPL:
      sym = ATgetAFun((ATermAppl) t);
      assert(AFun::at_lookup_table.size()>sym.number());
      nr_unique = AFun::at_lookup_table[sym.number()]->count>0 ? 0 : 1;
      AFun::at_lookup_table[sym.number()]->count++;
      // AT_markAFun(sym);
      arity = ATgetArity(sym);
      for (i = 0; i < arity; i++)
      {
        nr_unique += calcUniqueAFuns(ATgetArgument((ATermAppl)t, i),visited);
      }
      break;

    case AT_LIST:
      list = (ATermList)t;
      while (!ATisEmpty(list) && visited.count(list)==0 /* !IS_MARKED(list->header)*/ )
      {
        // SET_MARK(list->header);
        visited.insert(list);
        if (!AFun::at_lookup_table[AS_LIST.number()]->count++)
        {
          nr_unique++;
        }
        nr_unique += calcUniqueAFuns(ATgetFirst(list),visited);
        list = ATgetNext(list);
      }
      if (ATisEmpty(list) && visited.count(list)==0 /* !IS_MARKED(list->header)*/ )
      {
        // SET_MARK(list->header);
        visited.insert(list);
        if (!AFun::at_lookup_table[AS_EMPTY_LIST.number()]->count++)
        {
          nr_unique++;
        }
      }
      break;
  }

  visited.insert(t);
  // SET_MARK(t->header);

  return nr_unique;
}

size_t AT_calcUniqueAFuns(const ATerm &t)
{
  std::set<ATerm> visited;
  size_t result = calcUniqueAFuns(t,visited);
  // AT_unmarkIfAllMarked(t);

  return result;
}



} // namespace aterm_deprecated
