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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_io.h"

/*}}}  */

namespace atermpp
{

/*{{{  defines */

static const size_t DEFAULT_BUFFER_SIZE = 4096;

static const size_t ERROR_SIZE = 32;

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

static aterm    fparse_term(int* c, FILE* f);
static aterm    sparse_term(int* c, char** s);


/*{{{  void AT_cleanup() */

/**
 * Perform necessary cleanup.
 */

bool ATwriteToTextFile(const aterm &t, FILE* f);

void aterm_init()
{
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

  // atexit(AT_cleanupMemory);
}

/*}}}  */

/*{{{  ATbool ATwriteToTextFile(aterm t, FILE *f) */

/**
 * Write a term in text format to file.
 */

static bool
writeToTextFile(const aterm &t, FILE* f)
{
  function_symbol sym;
  aterm           arg;
  size_t          i, arity; 
  aterm_appl      appl;
  aterm_list      list;
  std::string     name;

  assert(t.address()->reference_count()>0);
  switch (t.type())
  {
    case AT_INT:
      fprintf(f, "%ld", aterm_int(t).value());
      break;
    case AT_APPL:
      /*{{{  Print application */

      appl = aterm_cast<aterm_appl>(t);

      sym = appl.function();
      fprintf(f, "\"%s\"",sym.name().c_str());
      arity = sym.arity();
      name = sym.name();
      if (arity > 0 || (!true && name.empty()))
      {
        fputc('(', f);
        for (i = 0; i < arity; i++)
        {
          if (i != 0)
          {
            fputc(',', f);
          }
          arg = appl(i);
          ATwriteToTextFile(arg, f);
        }
        fputc(')', f);
      }

      /*}}}  */
      break;
    case AT_LIST:
      /*{{{  Print list */

      list = (aterm_list) t;
      if (!list.empty())
      {
        ATwriteToTextFile(list.front(), f);
        list = list.tail();
      }
      while (!list.empty())
      {
        fputc(',', f);
        ATwriteToTextFile(list.front(), f);
        list = list.tail();
      }

      /*}}}  */
      break;
  }

  return true;
}

bool
ATwriteToTextFile(const aterm &t, FILE* f)
{
  bool result = true;

  if (t.type() == AT_LIST)
  {
    fputc('[', f);

    if (!aterm_list(t).empty())
    {
      result = writeToTextFile(t, f);
    }

    fputc(']', f);
  }
  else
  {
    result = writeToTextFile(t, f);
  }

  return result;
}


/**
 * Write an aterm to a named plaintext file
 */

bool write_to_named_text_file(aterm t, const std::string& filename)
{
  FILE*  f;
  bool result;

  if (!strcmp(filename.c_str(), "-"))
  {
    return ATwriteToTextFile(t, stdout);
  }

  if (!(f = fopen(filename.c_str(), "wb")))
  {
    return false;
  }

  result = ATwriteToTextFile(t, f);
  fclose(f);

  return result;
}

/*}}}  */

/*{{{  static char *writeToStream(aterm t, std::ostream& os) */

static void topWriteToStream(const aterm &t, std::ostream& os);

static void
writeToStream(const aterm &t, std::ostream& os)
{
  aterm_list list;
  aterm_appl appl;
  function_symbol sym;
  size_t i, arity;
  std::string name;

  switch (t.type())
  {
    case AT_INT:
      os << aterm_int(t).value();
      break;
    case AT_APPL:
      appl = aterm_cast<aterm_appl>(t);
      sym = appl.function();
      arity = sym.arity();
      name = sym.name();
      os << ATwriteAFunToString(sym);
      if (arity > 0 || (!true && name.empty()))
      {
        os << "(";
        if (arity > 0)
        {
          topWriteToStream(appl(0), os);
          for (i = 1; i < arity; i++)
          {
            os << ",";
            topWriteToStream(appl(i), os);
          }
        }
        os << ")";
      }
      break;

    case AT_LIST:
      list = (aterm_list) t;
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

static void
topWriteToStream(const aterm &t, std::ostream& os)
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

/*}}}  */

/**
 * Write a term into its text representation.
 */

// std::string ATwriteToString(const aterm &t)
std::string aterm::to_string() const
{
  std::ostringstream oss;
  topWriteToStream(*this, oss);
  return oss.str();
}

/*}}}  */
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

/*{{{  static aterm_list fparse_terms(int *c, FILE *f) */

/**
 * Parse a list of arguments.
 */

static aterm_list
fparse_terms(int* c, FILE* f)
{
  aterm_list list;
  aterm el = fparse_term(c, f);

  if (el == aterm())
  {
    return aterm_list();
  }

  list = push_front(aterm_list(), el);

  while (*c == ',')
  {
    fnext_skip_layout(c, f);
    el = fparse_term(c, f);
    if (el == aterm())
    {
      return aterm_list();
    }
    list = push_front(list, el);
  }

  return reverse(list);
}

/*}}}  */
/*{{{  static aterm_appl fparse_quoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static aterm
fparse_quoted_appl(int* c, FILE* f)
{
  assert(string_buffer.empty());
  aterm_list       args ;
  function_symbol          sym;
  char*           name;

  /* First parse the identifier */
  fnext_char(c, f);

  while (*c != '"')
  {
    switch (*c)
    {
      case EOF:
        return aterm();
      case '\\':
        fnext_char(c, f);
        if (*c == EOF)
        {
          return aterm();
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
    if (args == aterm() || *c != ')')
    {
      return aterm();
    }
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = function_symbol(name, args.size());
  free(name);
  return aterm_appl(sym, args.begin(), args.end());
}

/*}}}  */
/*{{{  static aterm_appl fparse_unquoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static aterm_appl
fparse_unquoted_appl(int* c, FILE* f)
{
  assert(string_buffer.empty());
  function_symbol sym;
  aterm_list args = aterm_list();
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
    if (args == aterm() || *c != ')')
    {
      return aterm_appl();
    }
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = function_symbol(name ? name : "", args.size());
  if (name != NULL)
  {
    free(name);
  }

  return aterm_appl(sym, args.begin(), args.end());
}

/*}}}  */
/*{{{  static void fparse_num(int *c, FILE *f) */

/**
 * Parse a number or blob.
 */

static aterm
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
    return aterm_int(static_cast<size_t>(atol(num)));

    /*}}}  */
  }
}

/*}}}  */
/*{{{  static aterm fparse_term(int *c, FILE *f) */

/**
 * Parse a term from file.
 */

static aterm
fparse_term(int* c, FILE* f)
{
  /* aterm t, result = NULL; */
  aterm result;

  switch (*c)
  {
    case '"':
      result = (aterm) fparse_quoted_appl(c, f);
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
        if (result == aterm() || *c != ']')
        {
          return aterm();
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
        result = aterm();
      }
      break;
  }

  return result;
}

/*}}}  */

/*{{{  aterm readFromTextFile(FILE *file) */

/**
 * Read a term from a text file. The first character has been read.
 */

static aterm
readFromTextFile(int* c, FILE* file)
{
  aterm term;
  fskip_layout(c, file);

  term = fparse_term(c, file);

  if (term.address())
  {
    ungetc(*c, file);
  }
  else
  {
    mCRL2log(mcrl2::log::error) << "readFromTextFile: parse error at line " << line
                                << ", col " << col << ((line||col)?":\n":"");
    for (size_t i = 0; i < ERROR_SIZE; ++i)
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
/*{{{  aterm ATreadFromFile(FILE *file) */

/**
 * Read an aterm from a file that could be binary or text.
 */

aterm read_from_file(FILE* file)
{
  int c;

  fnext_char(&c, file);
  if (c == 0)
  {
    /* Might be a BAF file */
    return ATreadFromBinaryFile(file);
    /* } else if (c == START_OF_SHARED_TEXT_FILE) {
      / * Might be a shared text file * /
      return AT_readFromSharedTextFile(&c, file);   */
  }
  else if (c == SAF_IDENTIFICATION_TOKEN)
  {
    int token = ungetc(SAF_IDENTIFICATION_TOKEN, file);
    if (token != SAF_IDENTIFICATION_TOKEN)
    {
      throw std::runtime_error("Unable to unget the SAF identification token.");
    }

    return ATreadFromSAFFile(file);
  }
  else
  {
    /* Probably a text file */
    line = 0;
    col = 0;
    error_idx = 0;
    memset(error_buf, 0, ERROR_SIZE);

    return readFromTextFile(&c, file);
  }
}

/**
 * Read an aterm from a named file
 */

aterm read_from_named_file(const std::string& name)
{
  FILE*  f;
  aterm t;

  if (!strcmp(name.c_str(), "-"))
  {
    return read_from_file(stdin);
  }

  if (!(f = fopen(name.c_str(), "rb")))
  {
    return aterm();
  }

  t = read_from_file(f);
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


/**
 * Parse a list of arguments.
 */

static aterm_list
sparse_terms(int* c, char** s)
{
  aterm el = sparse_term(c, s);
  if (el == aterm())
  {
    return aterm_list();
  }

  aterm_list list = push_front(aterm_list(), el);

  while (*c == ',')
  {
    snext_skip_layout(c, s);
    el = sparse_term(c, s);
    if (el == aterm())
    {
      return aterm_list();
    }
    list = push_front(list, el);
  }

  return reverse(list);
}

/*{{{  static aterm_appl sparse_quoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static aterm
sparse_quoted_appl(int* c, char** s)
{
  assert(string_buffer.empty());
  aterm_list       args = aterm_list();
  function_symbol          sym;
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
        return aterm();
      case '\\':
        snext_char(c, s);
        if (*c == EOF)
        {
          return aterm();
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
    if (args == aterm_list() || *c != ')')
    {
      return aterm();
    }
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = function_symbol(name, args.size());
  free(name);
  return aterm_appl(sym, args.begin(), args.end());
}

/*}}}  */
/*{{{  static aterm_appl sparse_unquoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static aterm_appl
sparse_unquoted_appl(int* c, char** s)
{
  assert(string_buffer.empty());
  function_symbol sym;
  aterm_list args = aterm_list();
  char* name = NULL;

  if (*c != '(')
  {
    /* First parse the identifier */
    while (isalnum(*c)
           || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$' || *c == '@')
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
    if (args == aterm() || *c != ')')
    {
      return aterm_appl();
    }
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = function_symbol(name ? name : "", args.size());
  if (name != NULL)
  {
    free(name);
  }

  return aterm_appl(sym, args.begin(), args.end());
}

/*}}}  */
/*{{{  static void sparse_num(int *c, char **s) */

/**
 * Parse a number
 */

static aterm
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
    return aterm_int(static_cast<size_t>(atol(num)));

    /*}}}  */
  }
}

/*}}}  */

/*{{{  static aterm sparse_term(int *c, char **s) */

/**
 * Parse a term from file.
 */

static aterm
sparse_term(int* c, char** s)
{
  /* aterm t, result = NULL; */
  aterm result;

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
        if (result == aterm() || *c != ']')
        {
          return aterm();
        }
      }
      snext_skip_layout(c, s);
      break;
    default:
      if (isalpha(*c) || *c == '(' || *c == '@' )
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
        result = aterm();
      }
      break;
  }

  if (result != aterm())
  {
    sskip_layout(c, s);
  }
  return result;
}

/**
 * Read from a string.
 */

/* aterm
ATreadFromString(const char* string)  */
aterm read_from_string(const std::string& s)
{
  const char* string=s.c_str();
  int             c;
  const char*     orig = string;

  snext_skip_layout(&c, (char**) &string);

  aterm term = sparse_term(&c, (char**) &string);

  if (term == aterm())
  {
    int i;
    mCRL2log(mcrl2::log::error) << "ATreadFromString: parse error at or near:" << std::endl
                                << orig << std::endl;
    for (i = 1; i < string - orig; ++i)
    {
      mCRL2log(mcrl2::log::error) << " ";
    }
    mCRL2log(mcrl2::log::error) << "^" << std::endl;
  }
  else
  {
    string--;
  }

  return term;
}

/*}}}  */

} // namespace atermpp
