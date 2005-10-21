/*{{{  includes */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "tafio.h"
#include "aterm2.h"
#include "_aterm.h"
#include "_afun.h"
#include "memory.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*}}}  */
/*{{{  defines */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

#define MAX_ENCODED_SIZE 64

#define ISBASE64(c) (isalnum((c)) || (c) == '+' || (c) == '/')
#define TOBASE64(n) tobase64[(n)]

/*}}}  */
/*{{{  variables */

/* From RFC2045 (Base64 encoding: The Base64 Alphabet) */
static char tobase64[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static long topWriteToSharedTextFile(ATerm t, byte_writer *writer, ATermIndexedSet abbrevs);

static long next_abbrev = 0;

static char print_buffer[BUFSIZ];

/* We need a buffer for printing and parsing */
static int      parse_buffer_size = 0;
static char    *parse_buffer = NULL;

static int line;
static int col;

/*}}}  */
/*{{{  functions */

static ATerm rparse_term(int *c, byte_reader *reader, ATermIndexedSet abbrevs);

/*}}}  */

/*{{{  static void resize_parse_buffer(int n) */

/**
  * Resize the resident parse buffer
  */

static void resize_parse_buffer(int n)
{
  parse_buffer_size = n;
  parse_buffer = (char *) realloc(parse_buffer, parse_buffer_size);
  if (!parse_buffer) {
    ATerror("resize_parse_buffer(tafio.c): cannot allocate parse buffer of size %d\n", 
	    parse_buffer_size);
  }
}

/*}}}  */

/*{{{  static long abbrev_size(long abbrev) */

static int abbrev_size(long abbrev)
{
  int size = 1;

  if (abbrev == 0) {
    return 2;
  }

  while (abbrev > 0) {
    size++;
    abbrev /= 64;
  }

  return size;
}

/*}}}  */
/*{{{  static long write_abbrev(long abbrev, byte_writer *writer) */

static long write_abbrev(long abbrev, byte_writer *writer)
{
  char *ptr, buf[MAX_ENCODED_SIZE+1] ;

  write_byte('#', writer);
  
  ptr = buf+MAX_ENCODED_SIZE;
  ptr[0] = '\0';
  
  if (abbrev == 0) {
    *(--ptr) = TOBASE64(0);
  }

  while (abbrev > 0) {
    *(--ptr) = TOBASE64(abbrev%64);
    abbrev /= 64;
  }

  return write_bytes(ptr, buf-ptr+MAX_ENCODED_SIZE, writer)+1;
}

/*}}}  */

/*{{{  static int writeToSharedTextFile(ATerm t, byte_writer *writer, ATermIndexedSet abbrevs) */

/**
  * Write a term in text format to file.
  */

static int writeToSharedTextFile(ATerm t, byte_writer *writer, ATermIndexedSet abbrevs)
{
  Symbol          sym;
  ATerm           arg;
  int             i, arity, elem_size, blob_size;
  long            size;
  ATermAppl       appl;
  ATermList       list;
  ATermBlob       blob;

  size = 0;

  switch (ATgetType(t))
    {
    case AT_INT:
      /*{{{  Print an integer */

      elem_size = sprintf(print_buffer, "%d", ((ATermInt) t)->value);
      elem_size = write_bytes(print_buffer, elem_size, writer);
      if (elem_size < 0) {
	return -1;
      }
      size += elem_size;

      /*}}}  */
      break;

    case AT_REAL:
      /*{{{  Print a real */

      elem_size = sprintf(print_buffer, "%.15e", ((ATermReal) t)->value);
      elem_size = write_bytes(print_buffer, elem_size, writer);
      if (elem_size < 0) {
	return -1;
      }
      size += elem_size;

      /*}}}  */
      break;

    case AT_APPL:
      /*{{{  Print application */

      appl = (ATermAppl) t;
				
      sym = ATgetSymbol(appl);
      elem_size = AT_writeAFun(sym, writer);
      if (elem_size < 0) {
	return -1;
      }
      size += elem_size;
      arity = ATgetArity(sym);
      if (arity > 0) {
	write_byte('(', writer);
	size++;
	for (i = 0; i < arity; i++) {
	  if (i != 0) {
	    write_byte(',', writer);
	    size++;
	  }
	  arg = ATgetArgument(appl, i);
	  elem_size = topWriteToSharedTextFile(arg, writer, abbrevs);
	  if (elem_size < 0) {
	    return -1;
	  }
	  size += elem_size;
	}
	write_byte(')', writer);
	size++;
      }

      /*}}}  */
      break;

    case AT_LIST:
      /*{{{  Print list */

      list = (ATermList) t;
      if(!ATisEmpty(list)) {
	elem_size = topWriteToSharedTextFile(ATgetFirst(list), writer, abbrevs);
	if (elem_size < 0) {
	  return -1;
	}
	size += elem_size;
	list = ATgetNext(list);
      }
      while(!ATisEmpty(list)) {
	write_byte(',', writer);				
	size++;
	elem_size = topWriteToSharedTextFile(ATgetFirst(list), writer, abbrevs);
	if (elem_size < 0) {
	  return -1;
	}
	size += elem_size;
	list = ATgetNext(list);
      }

      /*}}}  */
      break;

    case AT_PLACEHOLDER:
      /*{{{  Print placeholder */

      write_byte('<', writer);
      elem_size = topWriteToSharedTextFile(ATgetPlaceholder((ATermPlaceholder) t), writer, abbrevs);
      if (elem_size < 0) {
	return -1;
      }
      write_byte('>', writer);
      size += elem_size+2;

      /*}}}  */
      break;

    case AT_BLOB:
      /*{{{  Print blob */

      blob = (ATermBlob) t;
      blob_size = ATgetBlobSize(blob);
      elem_size = sprintf(print_buffer, "\"%c%-.*d%c", STRING_MARK, LENSPEC, blob_size, STRING_MARK);
      elem_size = write_bytes(print_buffer, elem_size, writer);
      if (elem_size < 0) {
	return -1;
      }
      size += elem_size;
      elem_size = write_bytes(ATgetBlobData(blob), blob_size, writer);
      if (elem_size < 0) {
	return -1;
      }
      size += elem_size;
      write_byte('"', writer);
      size++;

      /*}}}  */
      break;

    case AT_FREE:
      if(AT_inAnyFreeList(t))
	ATerror("ATwriteToTextFile: printing free term at %p!\n", t);
      else
	ATerror("ATwriteToTextFile: free term %p not in freelist?\n", t);
      return -1;

    case AT_SYMBOL:
      ATerror("ATwriteToTextFile: not a term but an afun: %y\n", t);
      return -1;
    }

  return size;
}

/*}}}  */

/*{{{  static long topWriteToSharedTextFile(ATerm t, byte_writer *writer, ATermIndexedSet abbrevs) */

static long topWriteToSharedTextFile(ATerm t, byte_writer *writer, ATermIndexedSet abbrevs)
{
  ATerm annos;
  long abbrev, size = 0, anno_size;

  abbrev = ATindexedSetGetIndex(abbrevs, t);
  if (abbrev >= 0) {
    return write_abbrev(abbrev, writer);
  }
 
  if (ATgetType(t) == AT_LIST) {
    write_byte('[', writer);

    if (!ATisEmpty((ATermList) t)) {
      size = writeToSharedTextFile(t, writer, abbrevs);
      if (size < 0) {
	return -1;
      }
    }

    write_byte(']', writer);
    size += 2;
  } else {
    size = writeToSharedTextFile(t, writer, abbrevs);
    if (size < 0) {
      return -1;
    }
  }

  annos = (ATerm) AT_getAnnotations(t);
  if (annos) {
    write_byte('{', writer);
    anno_size = writeToSharedTextFile(annos, writer, abbrevs);
    if (anno_size < 0) {
      return -1;
    }
    write_byte('}', writer);
    size += anno_size + 2;
  }

  if (size > abbrev_size(next_abbrev)) {
    ATbool isnew;

    abbrev = ATindexedSetPut(abbrevs, t, &isnew);
    /*ATfprintf(stderr, "%5d: %t\n", abbrev, t);*/
    assert(isnew);
    assert(abbrev == next_abbrev);
    next_abbrev++;
  } 

  return size;
}

/*}}}  */

/*{{{  long ATwriteToSharedTextFile(ATerm t, FILE *file) */

long ATwriteToSharedTextFile(ATerm t, FILE *file)
{
  byte_writer writer;
  long size;
  ATermIndexedSet abbrevs;
  
  writer.type = FILE_WRITER;
  writer.u.file_data = file;

  write_byte(START_OF_SHARED_TEXT_FILE, &writer);

  next_abbrev = 0;
  abbrevs = ATindexedSetCreate(1024, 75);

  size = topWriteToSharedTextFile(t, &writer, abbrevs);

  ATindexedSetDestroy(abbrevs);

  return size;
}

/*}}}  */
/*{{{  char *ATwriteToSharedString(ATerm t, int *len) */

char *ATwriteToSharedString(ATerm t, int *len)
{
  static byte_writer writer;
  static ATbool initialized = ATfalse;

  ATermIndexedSet abbrevs;
  int length;

  next_abbrev = 0;
  abbrevs = ATindexedSetCreate(1024, 75);

  if (!initialized) {
    writer.type = STRING_WRITER;
    writer.u.string_data.buf = (char *)calloc(BUFSIZ, 1);
    writer.u.string_data.max_size = BUFSIZ;
    initialized = ATtrue;
  }
  writer.u.string_data.cur_size = 0;

  write_byte(START_OF_SHARED_TEXT_FILE, &writer);

  length = topWriteToSharedTextFile(t, &writer, abbrevs);
  if (length < 0) {
    ATindexedSetDestroy(abbrevs);
    return NULL;
  }

  length++; /* START_OF_SHARED_TEXT_FILE */

  assert(length == writer.u.string_data.cur_size);

  writer.u.string_data.buf[length] = '\0';

  if (len != NULL) {
    *len = length;
  }

  ATindexedSetDestroy(abbrevs);

  return writer.u.string_data.buf;
}

/*}}}  */

/*{{{  static void store_char(int char) */

/**
 * Store a single character in the parse buffer
 */

static void store_char(int c, int pos)
{
  if (pos >= parse_buffer_size) {
    if (parse_buffer_size == 0) {
      resize_parse_buffer(128);
    } else {
      resize_parse_buffer(parse_buffer_size * 2);	/* Double the space */
    }
  }

  parse_buffer[pos] = c;
}

/*}}}  */

/*{{{  static void rnext_char(int *c, byte_reader *reader) */

/**
  * Read the next character from reader.
  */

static void rnext_char(int *c, byte_reader *reader)
{
  *c = read_byte(reader);
  if (*c != EOF) {
    if (*c == '\n') {
      line++;
      col = 0;
    } else {
      col++;
    }
  }
}

/*}}}  */
/*{{{  static void rskip_layout(int *c, byte_reader *reader) */

/**
  * Skip layout from reader.
  */

static void rskip_layout(int *c, byte_reader *reader)
{
  while (isspace(*c)) {
    rnext_char(c, reader);
  }
}

/*}}}  */
/*{{{  static void rnext_skip_layout(int *c, byte_reader *reader) */

/**
  * Skip layout from file.
  */

static void rnext_skip_layout(int *c, byte_reader *reader)
{
  do {
    rnext_char(c, reader);
  } while (isspace(*c));
}

/*}}}  */
/*{{{  static ATermList rparse_terms(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

/**
  * Parse a list of arguments.
  */

ATermList rparse_terms(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  ATermList list;
  ATerm el = rparse_term(c, reader, abbrevs);

  if(el == NULL) {
    return NULL;
  }

  list = ATinsert(ATempty, el);

  while(*c == ',') {
    rnext_skip_layout(c, reader);
    el = rparse_term(c, reader, abbrevs);
    if(el == NULL) {
      return NULL;
    }
    list = ATinsert(list, el);
  }

  return ATreverse(list);
}

/*}}}  */
/*{{{  static ATerm rparse_blob(int *c, byte_reader *reader) */

static ATerm rparse_blob(int *c, byte_reader *reader)
{
  char lenspec[LENSPEC+2];
  int len;
  char *data;

  if (read_bytes(lenspec, LENSPEC+1, reader) != LENSPEC+1) {
    return NULL;
  }

  if (lenspec[LENSPEC] != ((char)STRING_MARK)) {
    return NULL;
  }

  lenspec[LENSPEC] = '\0';

  len = atoi(lenspec);

  data = (char *)malloc(len);
  if (!data) {
    ATerror("out of memory in rparse_blob\n");
  }
  if (read_bytes(data, len, reader) != len) {
    return NULL;
  }

  rnext_char(c, reader);
  if (*c != '"') {
    return NULL;
  }

  rnext_skip_layout(c, reader);

  return (ATerm)ATmakeBlob(len, data);
}

/*}}}  */
/*{{{  static ATermAppl rparse_quoted_appl(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

/**
  * Parse a quoted application.
  */

static ATerm rparse_quoted_appl(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  int             len = 0;
  ATermList       args = ATempty;
  Symbol          sym;
  char           *name;
  
  /* First parse the identifier */
  rnext_char(c, reader);

  if (*c == STRING_MARK) {
    return rparse_blob(c, reader);
  }

  while (*c != '"') {
    switch (*c) {
    case EOF:
      return NULL;
    case '\\':
      rnext_char(c, reader);
      if (*c == EOF) {
	return NULL;
      }
      switch (*c) {
      case 'n':
	store_char('\n', len++);
	break;
      case 'r':
	store_char('\r', len++);
	break;
      case 't':
	store_char('\t', len++);
	break;
      default:
	store_char(*c, len++);
	break;
      }
      break;
    default:
      store_char(*c, len++);
      break;
    }
    rnext_char(c, reader);
  }
  
  store_char('\0', len);
  
  name = strdup(parse_buffer);
  if (!name) {
    ATerror("fparse_quoted_appl: symbol to long.");
  }
	
  rnext_skip_layout(c, reader);

  /* Time to parse the arguments */
  if (*c == '(') {
    rnext_skip_layout(c, reader);
    if(*c != ')') {
      args = rparse_terms(c, reader, abbrevs);
    } else {
      args = ATempty;
    }
    if (args == NULL || *c != ')')
      return NULL;
    rnext_skip_layout(c, reader);
  }

  /* Wrap up this function application */
  sym = ATmakeSymbol(name, ATgetLength(args), ATtrue);
  free(name);
  return (ATerm)ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static ATermAppl rparse_unquoted_appl(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

/**
  * Parse a quoted application.
  */

static ATermAppl rparse_unquoted_appl(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  int             len = 0;
  Symbol          sym;
  ATermList       args = ATempty;
  char           *name;

  /* First parse the identifier */
  while (isalnum(*c) || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      store_char(*c, len++);
      rnext_char(c, reader);
    }
  store_char('\0', len++);
  name = strdup(parse_buffer);
  if (!name) {
    ATerror("fparse_unquoted_appl: symbol to long.");
  }

  rskip_layout(c, reader);

  /* Time to parse the arguments */
  if (*c == '(')
    {
      rnext_skip_layout(c, reader);
      if(*c != ')') {
	args = rparse_terms(c, reader, abbrevs);
      } else {
	args = ATempty;
      }
      if (args == NULL || *c != ')')
	return NULL;
      rnext_skip_layout(c, reader);
    }
		
  /* Wrap up this function application */
  sym = ATmakeSymbol(name, ATgetLength(args), ATfalse);
  free(name);
  return ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static void rparse_num(int *c, byte_reader *reader) */

/**
  * Parse a number or blob.
  */

static ATerm rparse_num(int *c, byte_reader *reader)
{
  char num[32], *ptr = num;

  if (*c == '-') {
    *ptr++ = *c;
    rnext_char(c, reader);
  }

  while (isdigit(*c)) {
    *ptr++ = *c;
    rnext_char(c, reader);
  }
  if (*c == '.' || toupper(*c) == 'E') {
    /*{{{  A real number */
    
    if (*c == '.') {
      *ptr++ = *c;
      rnext_char(c, reader);
      while (isdigit(*c)) {
	*ptr++ = *c;
	rnext_char(c, reader);
      }
    }
    if (toupper(*c) == 'E') {
      *ptr++ = *c;
      rnext_char(c, reader);
      if (*c == '-' || *c == '+') {
	*ptr++ = *c;
	rnext_char(c, reader);
      }
      while (isdigit(*c)) {
	*ptr++ = *c;
	rnext_char(c, reader);
      }
    }
    *ptr = '\0';
    return (ATerm) ATmakeReal(atof(num));
    
    /*}}}  */
  } else {
    /*{{{  An integer */

    *ptr = '\0';
    return (ATerm) ATmakeInt(atoi(num));

    /*}}}  */
  }
}

/*}}}  */
/*{{{  static ATerm rparse_abbrev(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

static ATerm rparse_abbrev(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  ATerm result;
  long abbrev;

  rnext_char(c, reader);

  abbrev = 0;
  while (ISBASE64(*c)) {
    abbrev *= 64;
    if (*c >= 'A' && *c <= 'Z') { 
     abbrev += *c - 'A';
    } else if (*c >= 'a' && *c <= 'z') {
      abbrev += *c - 'a' + 26;
    } else if (*c >= '0' && *c <= '9') {
      abbrev += *c - '0' + 52;
    } else if (*c == '+') {
      abbrev += 62;
    } else if (*c == '/') {
      abbrev += 63;
    } else {
      abort();
    }

    rnext_char(c, reader);
  }

  result = ATindexedSetGetElem(abbrevs, abbrev);
  assert(result != NULL);

  return result;
}

/*}}}  */

/*{{{  static ATerm rparse_term(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

/**
  * Parse a term from a reader.
  */

static ATerm rparse_term(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  ATerm t, result = NULL;
  long start, end;

  start = reader->bytes_read;

  switch (*c)
    {
    case '#':
      result = rparse_abbrev(c, reader, abbrevs);
      assert(result != NULL);
      return result;
      break;

    case '"':
      result = (ATerm) rparse_quoted_appl(c, reader, abbrevs);
      break;

    case '[':
      rnext_skip_layout(c, reader);
      if (*c == ']') {
	result = (ATerm) ATempty;
      } else {
	result = (ATerm) rparse_terms(c, reader, abbrevs);
	if (result == NULL || *c != ']') {
	  return NULL;
	}
      }
      rnext_skip_layout(c, reader);
      break;

    case '<':
      rnext_skip_layout(c, reader);
      t = rparse_term(c, reader, abbrevs);
      if (t != NULL && *c == '>') {
	result = (ATerm) ATmakePlaceholder(t);
	rnext_skip_layout(c, reader);
      }
      break;

    default:
      if (isalpha(*c) || *c == '(') {
	result = (ATerm) rparse_unquoted_appl(c, reader, abbrevs);
      } else if (isdigit(*c)) {
	result = rparse_num(c, reader);
      } else if (*c == '.' || *c == '-') {
	result = rparse_num(c, reader);
      } else {
	result = NULL;
      }
    }

  if(result != NULL) {
    rskip_layout(c, reader);
			
    if (*c == '{') {
      /* Term is annotated */
      rnext_skip_layout(c, reader);
      if (*c != '}') {
	ATerm annos = (ATerm) rparse_terms(c, reader, abbrevs);
	if (annos == NULL || *c != '}') {
	  return NULL;
	}
	result = AT_setAnnotations(result, annos);
      }
      rnext_skip_layout(c, reader);
    }
    /*{{{  Parse backwards compatible toolbus anomalies */
    
    if (*c == ':') {
      ATerm type;
      rnext_skip_layout(c, reader);
      type = rparse_term(c, reader, abbrevs);
      if (type != NULL) {
	result = ATsetAnnotation(result, ATparse("type"), type);
      } else {
	return NULL;
      }
    }

    if (*c == '?') {
      rnext_skip_layout(c, reader);
      result = ATsetAnnotation(result, ATparse("result"), ATparse("true"));
    }

    /*}}}  */

    end = reader->bytes_read;

    if (abbrev_size(next_abbrev) < (end-start)) {
      ATbool isnew;
      long abbrev;

      abbrev = ATindexedSetPut(abbrevs, result, &isnew);
      if (isnew) {
	/*ATfprintf(stderr, "%5d: %t\n", abbrev, result);*/
	assert(abbrev == next_abbrev);
	next_abbrev++;
      }
    }
  }

  return result;
}

/*}}}  */
/*{{{  static ATerm readFromSharedText(int *c, byte_reader *reader, ATermIndexedSet abbrevs) */

/**
 * Read a shared term from a byte reader. The first character has been read.
 */

static ATerm readFromSharedText(int *c, byte_reader *reader, ATermIndexedSet abbrevs)
{
  ATerm term;

  rskip_layout(c, reader);
		
  term = rparse_term(c, reader, abbrevs);

  if (!term) {
    ATwarning("readFromSharedText: parse error at line %d, col %d\n",
	      line, col);
  }
		
  return term;
}

/*}}}  */

/*{{{  ATerm AT_readFromSharedTextFile(int *c, FILE *f) */

ATerm AT_readFromSharedTextFile(int *c, FILE *f)
{
  byte_reader reader;
  ATermIndexedSet abbrevs;
  ATerm result;

  *c = fgetc(f);

  init_file_reader(&reader, f);
  
  abbrevs = ATindexedSetCreate(1024, 75);
  next_abbrev = 0;

  result = readFromSharedText(c, &reader, abbrevs);

  ATindexedSetDestroy(abbrevs);

  return result;
}

/*}}}  */
/*{{{  ATerm ATreadFromSharedTextFile(FILE *f) */

ATerm ATreadFromSharedTextFile(FILE *f)
{
  ATerm result;
  int c;

  line = 0;
  col = 0;

  c = fgetc(f);

  if (c != START_OF_SHARED_TEXT_FILE) {
    ATwarning("not a shared text file!\n");
    return NULL;
  }

  if (c != EOF) {
    if (c == '\n') {
      line++;
      col = 0;
    } else {
      col++;
    }
  }

  result = AT_readFromSharedTextFile(&c, f);

  if (c != EOF) {
    ungetc(c, f);
  }

  return result;
}

/*}}}  */
/*{{{  ATerm ATreadFromSharedString(char *s, int size) */

ATerm ATreadFromSharedString(char *s, int size)
{
  byte_reader reader;
  ATermIndexedSet abbrevs;
  ATerm result;
  int c;

  line = 0;
  col = 0;

  init_string_reader(&reader, s, size);

  c = read_byte(&reader);
  if (c != START_OF_SHARED_TEXT_FILE) {
    ATwarning("not in shared text format: %s\n");
    return NULL;
  }

  abbrevs = ATindexedSetCreate(1024, 75);
  next_abbrev = 0;

  rnext_char(&c, &reader);
  
  result = readFromSharedText(&c, &reader, abbrevs);

  ATindexedSetDestroy(abbrevs);

  return result;
}

/*}}}  */
