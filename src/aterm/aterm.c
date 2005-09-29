/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "_aterm.h"
#include "memory.h"
#include "afun.h"
#include "list.h"
#include "make.h"
#include "gc.h"
#include "util.h"
#include "bafio.h"
#include "version.h"
#include "atypes.h"
#include "tafio.h"
#include "md5.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*}}}  */
/*{{{  defines */

#define SILENT_FLAG  "-at-silent"
#define VERBOSE_FLAG "-at-verbose"

#define LOW_MEMORY_FLAG "-at-low-memory"

#define DEFAULT_BUFFER_SIZE 4096
#define RESIZE_BUFFER(n) if(n > buffer_size) resize_buffer(n)
#define ERROR_SIZE 32
#define INITIAL_MARK_STACK_SIZE   16384
#define MARK_STACK_MARGE          MAX_ARITY

/* Initial number of terms that can be protected */
/* In the current implementation this means that
   excessive term protection can lead to deteriorating
   performance! */
#define INITIAL_PROT_TABLE_SIZE   100003
#define PROTECT_EXPAND_SIZE 100000

/* The same for the protected arrays */
#define PROTECT_ARRAY_INITIAL_SIZE 128
#define PROTECT_ARRAY_EXPAND_SIZE  256

/*}}}  */
/*{{{  globals */

char            aterm_id[] = "$Id: aterm.c,v 1.128 2004/06/01 08:29:01 jurgenv Exp $";

/* Flag to tell whether to keep quiet or not. */
ATbool silent	  = ATtrue;
ATbool low_memory = ATfalse;

/* warning_handler is called when a recoverable error is detected */
static void     (*warning_handler) (const char *format, va_list args) = NULL;
/* error_handler is called when a fatal error is detected */
static void     (*error_handler) (const char *format, va_list args) = NULL;
/* abort_handler is called when a fatal error is detected that
   warrants a core being dumped. */
static void     (*abort_handler) (const char *format, va_list args) = NULL;

/* We need a buffer for printing and parsing */
static int      buffer_size = 0;
static char    *buffer = NULL;

/* Parse error description */
static int      line = 0;
static int      col = 0;
static char     error_buf[ERROR_SIZE];
static int      error_idx = 0;

ProtEntry      *free_prot_entries = NULL;
ProtEntry     **at_prot_table = NULL;
int             at_prot_table_size = 0;
ProtEntry      *at_prot_memory = NULL;

static ATerm   *mark_stack = NULL;
static int      mark_stack_size = 0;
int             mark_stats[3] = {0, MYMAXINT, 0};

/*}}}  */
/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

static ATerm    fparse_term(int *c, FILE * f);
static ATerm    sparse_term(int *c, char **s);
static ATerm AT_diff(ATerm t1, ATerm t2, ATermList *diffs);

/*}}}  */

/*{{{  void AT_cleanup() */

/**
 * Perform necessary cleanup.
 */

void
AT_cleanup(void)
{
  AT_cleanupGC();
  AT_cleanupMemory();
}

/*}}}  */

/*{{{  void ATinitialize(int argc, char *argv[]) */

/**
 * Initialize ATerm library without supplying bottom of stack.
 * the stackbottom is determined in this function.
 * This variant might work better with some compilers (gcc)
 * that optimize local variables in 'main' into global variables.
 */ 

void ATinitialize(int argc, char *argv[])
{
  ATerm bottom;

  ATinit(argc, argv, &bottom);
}

/*}}}  */
/*{{{  void ATinit(int argc, char *argv[], ATerm *bottomOfStack) */

/**
 * Initialize the ATerm library.
 */

void
ATinit(int argc, char *argv[], ATerm * bottomOfStack)
{
  int lcv;
  static ATbool initialized = ATfalse;
  ATbool help = ATfalse;

  if (initialized)
    return;

  /*{{{  Handle arguments */

  for (lcv=1; lcv < argc; lcv++) {
    if (streq(argv[lcv], SILENT_FLAG)) {
      silent = ATtrue;
    } else if(streq(argv[lcv], VERBOSE_FLAG)) {
      silent = ATfalse;
    } else if(0 || streq(argv[lcv], LOW_MEMORY_FLAG)) {
      low_memory              = ATtrue;
    } else if(streq(argv[lcv], "-at-help")) {
      help = ATtrue;
    }
  }

  /*}}}  */
  /*{{{  Optionally print some information */

  AT_init_gc_parameters(low_memory);

  
  if (!silent) {
    ATfprintf(stderr, "  ATerm Library, version %s, built: %s\n",
	      at_version, at_date);
  }

  if(help) {
    fprintf(stderr, "    %-20s: print this help info\n", "-at-help");
    fprintf(stderr, "    %-20s: generate runtime gc information.\n",
	    "-at-verbose");
    fprintf(stderr, "    %-20s: suppress runtime gc information.\n",
	    "-at-silent");
    fprintf(stderr, "    %-20s: try to minimize the memory usage.\n",
	    "-at-low-memory");
  }

  /*}}}  */
  /*{{{  Perform some sanity checks */

  /* Protect novice users that simply pass NULL as bottomOfStack */
  if (bottomOfStack == NULL)
    ATerror("ATinit: illegal bottomOfStack (arg 3) passed.\n");

  /* Check for reasonably sized ATerm (32 bits, 4 bytes)     */
  /* This check might break on perfectly valid architectures */
  /* that have char == 2 bytes, and sizeof(header_type) == 2 */
  assert(sizeof(header_type) == sizeof(ATerm *));
  assert(sizeof(header_type) >= 4);

  /*}}}  */
  /*{{{  Initialize buffer */

  buffer_size = DEFAULT_BUFFER_SIZE;
  buffer = (char *) malloc(DEFAULT_BUFFER_SIZE);
  if (!buffer)
    ATerror("ATinit: cannot allocate string buffer of size %d\n",
	    DEFAULT_BUFFER_SIZE);

  /*}}}  */
  /*{{{  Initialize protected terms */

  at_prot_table_size = INITIAL_PROT_TABLE_SIZE;
  at_prot_table = (ProtEntry **)calloc(at_prot_table_size, sizeof(ProtEntry *));
  if(!at_prot_table)
    ATerror("ATinit: cannot allocate space for prot-table of size %d\n",
	    at_prot_table_size);

  /*}}}  */
  /*{{{  Initialize mark stack */

  /* Allocate initial mark stack */
  mark_stack = (ATerm *) malloc(sizeof(ATerm) * INITIAL_MARK_STACK_SIZE);
  if (!mark_stack)
    ATerror("cannot allocate marks stack of %d entries.\n",
	    INITIAL_MARK_STACK_SIZE);
  mark_stack_size = INITIAL_MARK_STACK_SIZE;

  /*}}}  */
  /*{{{  Initialize other components */

  /* Initialize other components */
  AT_initMemory(argc, argv);
  AT_initSymbol(argc, argv);
  AT_initList(argc, argv);
  AT_initMake(argc, argv);
  AT_initGC(argc, argv, bottomOfStack);
  AT_initBafIO(argc, argv);

  /*}}}  */

  initialized = ATtrue;

  atexit(AT_cleanup);

  if(help) {
    fprintf(stderr, "\n");
    exit(0);
  }
}

/*}}}  */
/*{{{  void ATsetWarningHandler(handler) */

/**
 * Change the warning handler.
 */

void
ATsetWarningHandler(void (*handler) (const char *format, va_list args))
{
  warning_handler = handler;
}

/*}}}  */
/*{{{  void ATsetErrorHandler(handler) */

/**
 * Change the error handler.
 */

void
ATsetErrorHandler(void (*handler) (const char *format, va_list args))
{
  error_handler = handler;
}

/*}}}  */
/*{{{  void ATsetAbortHandler(handler) */

/**
 * Change the abort handler.
 */

void
ATsetAbortHandler(void (*handler) (const char *format, va_list args))
{
  abort_handler = handler;
}

/*}}}  */
/*{{{  void ATwarning(const char *format, ...) */

void
ATwarning(const char *format,...)
{
  va_list args;

  va_start(args, format);
  if (warning_handler) {
    warning_handler(format, args);
  }
  else {
    ATvfprintf(stderr, format, args);
  }

  va_end(args);
}

/*}}}  */
/*{{{  void ATerror(const char *format, ...) */

/**
 * A fatal error was detected.
 */

void
ATerror(const char *format,...)
{
  va_list         args;

  va_start(args, format);
  if (error_handler)
    error_handler(format, args);
  else
  {
    ATvfprintf(stderr, format, args);
    exit(1);
  }

  va_end(args);
}

/*}}}  */
/*{{{  void ATabort(const char *format, ...) */

/**
 * A fatal error was detected.
 */

void
ATabort(const char *format,...)
{
  va_list         args;

  va_start(args, format);
  if (abort_handler)
    abort_handler(format, args);
  else
  {
    ATvfprintf(stderr, format, args);
    abort();
  }

  va_end(args);
}

/*}}}  */

/*{{{  void ATprotect(ATerm *term) */

/**
 * Protect a given term.
 */

void
ATprotect(ATerm * term)
{
  ATprotectArray(term, 1);
}

/*}}}  */
/*{{{  void ATunprotect(ATerm *term) */

/**
 * Unprotect a given term.
 */

void
ATunprotect(ATerm * term)
{
  ATunprotectArray(term);
}

/*}}}  */
/*{{{  void ATprotectArray(ATerm *start, int size) */

/**
 * Protect an array
 */

void ATprotectArray(ATerm *start, int size)
{
  ProtEntry *entry;
  ShortHashNumber hnr;

#ifndef NDEBUG
  int i;
  for(i=0; i<size; i++) {
    assert(start[i] == NULL || 
	   AT_isValidTerm(start[i])); /* Check the precondition */
  }
#endif

  if(!free_prot_entries) {
    int i;
    ProtEntry *entries = (ProtEntry *)calloc(PROTECT_EXPAND_SIZE, 
					     sizeof(ProtEntry));
    if(!entries)
      ATerror("out of memory in ATprotect.\n");
    for(i=0; i<PROTECT_EXPAND_SIZE; i++) {
      entries[i].next = free_prot_entries;
      free_prot_entries = &entries[i];
    }
  }
  entry = free_prot_entries;
  free_prot_entries = free_prot_entries->next;
  hnr = ADDR_TO_SHORT_HNR(start);

  hnr %= at_prot_table_size;
  entry->next = at_prot_table[hnr];
  at_prot_table[hnr] = entry;
  entry->start = start;
  entry->size  = size;
}

/*}}}  */
/*{{{  void ATunprotectArrray(ATerm *start) */

/**
 * Unprotect an array of terms.
 */

void ATunprotectArray(ATerm *start)
{
  ShortHashNumber hnr;
  ProtEntry *entry, *prev;

  hnr = ADDR_TO_SHORT_HNR(start);
  hnr %= at_prot_table_size;
  entry = at_prot_table[hnr];

  prev = NULL;
  while(entry->start != start) {
    prev  = entry;
    entry = entry->next;
    assert(entry);
  }

  if(prev)
    prev->next = entry->next;
  else
    at_prot_table[hnr] = entry->next;

  entry->next = free_prot_entries;
  free_prot_entries = entry;
}

/*}}}  */
/*{{{  void AT_printAllProtectedTerms(FILE *file) */

void AT_printAllProtectedTerms(FILE *file)
{
  int i, j;

  fprintf(file, "protected terms:\n");
  for(i=0; i<at_prot_table_size; i++) {
    ProtEntry *cur = at_prot_table[i];
    while(cur) {
      for(j=0; j<cur->size; j++) {
	if(cur->start[j]) {
	  ATfprintf(file, "%t\n", i, cur->start[j]);
	  fflush(file);
	}
      }
    }
  }
}

/*}}}  */
/*{{{  void ATprotectMemory(void *start, int size) */

void ATprotectMemory(void *start, int size)
{
  ProtEntry *entry = (ProtEntry *)malloc(sizeof(ProtEntry));
  if (entry == NULL) {
    ATerror("out of memory in ATprotectMemory.\n");
  }
  entry->start = (ATerm *)start;
  entry->size  = size;
  entry->next  = at_prot_memory;
  at_prot_memory = entry;
}

/*}}}  */
/*{{{  void ATunprotectMemory(void *start) */

void ATunprotectMemory(void *start)
{
  ProtEntry *entry, *prev;

  prev = NULL;
  for (entry=at_prot_memory; entry; entry=entry->next) {
    if (entry->start == start) {
      if (prev) {
	prev->next = entry->next;
      } else {
	at_prot_memory = entry->next;
      }
      free(entry);
      break;
    }
    prev = entry;
  }
}

/*}}}  */

/*{{{  int ATprintf(const char *format, ...) */

/**
 * Extension of printf() with ATerm-support.
 */

int
ATprintf(const char *format,...)
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
ATfprintf(FILE * stream, const char *format,...)
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

int
ATvfprintf(FILE * stream, const char *format, va_list args)
{
  const char     *p;
  char           *s;
  char            fmt[16];
  int             result = 0;
  ATerm           t;
  ATermList       l;

  for (p = format; *p; p++)
  {
    if (*p != '%')
    {
      fputc(*p, stream);
      continue;
    }

    s = fmt;
    while (!isalpha((int) *p))	/* parse formats %-20s, etc. */
      *s++ = *p++;
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
	fprintf(stream, fmt, va_arg(args, void *));
	break;

      case 's':
	fprintf(stream, fmt, va_arg(args, char *));
	break;

	/*
	 * ATerm specifics start here: "%t" to print an ATerm; "%l" to
	 * print a list; "%y" to print a Symbol; "%n" to print a single
	 * ATerm node
	 */
      case 't':
	ATwriteToTextFile(va_arg(args, ATerm), stream);
	break;
      case 'l':
	l = va_arg(args, ATermList);
	fmt[strlen(fmt) - 1] = '\0';	/* Remove 'l' */
	while (!ATisEmpty(l))
	{
	  ATwriteToTextFile(ATgetFirst(l), stream);
	  /*
	   * ATfprintf(stream, "\nlist node: %n\n", l);
	   * ATfprintf(stream, "\nlist element: %n\n", ATgetFirst(l));
	   */
	  l = ATgetNext(l);
	  if (!ATisEmpty(l))
	    fputs(fmt + 1, stream);
	}
	break;
      case 'a':
      case 'y':
	AT_printSymbol(va_arg(args, Symbol), stream);
	break;
      case 'n':
	t = va_arg(args, ATerm);
	switch (ATgetType(t))
	{
	  case AT_INT:
	  case AT_REAL:
	  case AT_BLOB:
	    ATwriteToTextFile(t, stream);
	    break;

	  case AT_PLACEHOLDER:
	    fprintf(stream, "<...>");
	    break;

	  case AT_LIST:
	    fprintf(stream, "[...(%d)]", ATgetLength((ATermList) t));
	    break;

	  case AT_APPL:
	    if (AT_isValidSymbol(ATgetAFun(t))) {
	      AT_printSymbol(ATgetAFun(t), stream);
	      fprintf(stream, "(...(%d))",
		      GET_ARITY(t->header));
	    } else {
	      fprintf(stream, "<sym>(...(%d))",
		      GET_ARITY(t->header));
	    }
	    if (HAS_ANNO(t->header)) {
	      fprintf(stream, "{}");
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

      case 'h':
	{
	  unsigned char *digest = ATchecksum(va_arg(args, ATerm));
	  int i;
	  for (i=0; i<16; i++) {
	    fprintf(stream, "%02x", digest[i]);
	  }
	}
	break;


      default:
	fputc(*p, stream);
	break;
    }
  }
  return result;
}

/*}}}  */

/*{{{  static void resize_buffer(int n) */

/**
 * Resize the resident string buffer
 */

static void
resize_buffer(int n)
{
  buffer_size = n;
  buffer = (char *) realloc(buffer, buffer_size);
  if (!buffer)
    ATerror("resize_buffer(aterm.c): cannot allocate string buffer of size %d\n", buffer_size);
}

/*}}}  */

/*{{{  ATbool ATwriteToTextFile(ATerm t, FILE *f) */

/**
 * Write a term in text format to file.
 */

ATbool
writeToTextFile(ATerm t, FILE * f)
{
  Symbol          sym;
  ATerm           arg;
  int             i, arity, size;
  ATermAppl       appl;
  ATermList       list;
  ATermBlob       blob;
  char            *name;

  switch (ATgetType(t))
  {
    case AT_INT:
      fprintf(f, "%d", ((ATermInt) t)->value);
      break;
    case AT_REAL:
      fprintf(f, "%.15e", ((ATermReal) t)->value);
      break;
    case AT_APPL:
      /*{{{  Print application */

      appl = (ATermAppl) t;

      sym = ATgetSymbol(appl);
      AT_printSymbol(sym, f);
      arity = ATgetArity(sym);
      name = ATgetName(sym);
      if (arity > 0 || (!ATisQuoted(sym) && *name == '\0')) {
	fputc('(', f);
	for (i = 0; i < arity; i++) {
	  if (i != 0) {
	    fputc(',', f);
	  }
	  arg = ATgetArgument(appl, i);
	  ATwriteToTextFile(arg, f);
	}
	fputc(')', f);
      }

      /*}}}  */
      break;
    case AT_LIST:
      /*{{{  Print list */

      list = (ATermList) t;
      if(!ATisEmpty(list)) {
	ATwriteToTextFile(ATgetFirst(list), f);
	list = ATgetNext(list);
      }
      while(!ATisEmpty(list)) {
	fputc(',', f);				
	ATwriteToTextFile(ATgetFirst(list), f);
	list = ATgetNext(list);
      }

      /*}}}  */
      break;
    case AT_PLACEHOLDER:
      /*{{{  Print placeholder */

      fputc('<', f);
      ATwriteToTextFile(ATgetPlaceholder((ATermPlaceholder) t), f);
      fputc('>', f);

      /*}}}  */
      break;
    case AT_BLOB:
      /*{{{  Print blob */

      blob = (ATermBlob) t;
      size = ATgetBlobSize(blob);
      fprintf(f, "\"%c%-.*d%c", STRING_MARK, LENSPEC, size, STRING_MARK);
      fwrite(ATgetBlobData(blob), ATgetBlobSize(blob), 1, f);
      fputc('"', f);

      /*}}}  */
      break;

    case AT_FREE:
      if(AT_inAnyFreeList(t))
	ATerror("ATwriteToTextFile: printing free term at %p!\n", t);
      else
	ATerror("ATwriteToTextFile: free term %p not in freelist?\n", t);
      return ATfalse;

    case AT_SYMBOL:
      ATerror("ATwriteToTextFile: not a term but an afun: %y\n", t);
      return ATfalse;
  }

  return ATtrue;
}

ATbool
ATwriteToTextFile(ATerm t, FILE * f)
{
  ATbool result = ATtrue;
  ATerm annos;

  if (ATgetType(t) == AT_LIST) {
    fputc('[', f);

    if (!ATisEmpty((ATermList) t))
      result = writeToTextFile(t, f);

    fputc(']', f);
  } else {
    result = writeToTextFile(t, f);
  }

  annos = (ATerm) AT_getAnnotations(t);
  if (annos) {
    fputc('{', f);
    result &= writeToTextFile(annos, f);
    fputc('}', f);
  }

  return result;
}

/*}}}  */

/*{{{  ATerm ATwriteToNamedTextFile(char *name) */

/**
 * Write an ATerm to a named plaintext file
 */

ATbool ATwriteToNamedTextFile(ATerm t, const char *name)
{  
  FILE  *f;
  ATbool result;

  if(!strcmp(name, "-")) {
    return ATwriteToTextFile(t, stdout);
  }

  if(!(f = fopen(name, "wb"))) {
    return ATfalse;
  }

  result = ATwriteToTextFile(t, f);
  fclose(f);

  return result;
}

/*}}}  */

/*{{{  char *ATwriteToString(ATerm t) */

/**
 * Write a term to a string buffer.
 */

/*{{{  static int symbolTextSize(Symbol sym) */

/**
 * Calculate the size of a symbol in text format.
 */

static int
symbolTextSize(Symbol sym)
{
  char           *id = ATgetName(sym);

  if (ATisQuoted(sym))
  {
    int             len = 2;
    while (*id)
    {
      /* We need to escape special characters */
      switch (*id)
      {
	case '\\':
	case '"':
	case '\n':
	case '\t':
	case '\r':
	  len += 2;
	  break;
	default:
	  len++;
      }
      id++;
    }
    return len;
  }
  else
    return strlen(id);
}

/*}}}  */
/*{{{  static char *writeSymbolToString(Symbol sym, char *buf) */

/**
 * Write a symbol in a string buffer.
 */

static char    *
writeSymbolToString(Symbol sym, char *buf)
{
  char           *id = ATgetName(sym);

  if (ATisQuoted(sym))
  {
    *buf++ = '"';
    while (*id)
    {
      /* We need to escape special characters */
      switch (*id)
      {
	case '\\':
	case '"':
	  *buf++ = '\\';
	  *buf++ = *id;
	  break;
	case '\n':
	  *buf++ = '\\';
	  *buf++ = 'n';
	  break;
	case '\t':
	  *buf++ = '\\';
	  *buf++ = 't';
	  break;
	case '\r':
	  *buf++ = '\\';
	  *buf++ = 'r';
	  break;
	default:
	  *buf++ = *id;
      }
      id++;
    }
    *buf++ = '"';
    return buf;
  }
  else
  {
    strcpy(buf, id);
    return buf + strlen(buf);
  }
}

/*}}}  */
/*{{{  static char *writeToString(ATerm t, char *buf) */

static char    *topWriteToString(ATerm t, char *buf);

static char *
writeToString(ATerm t, char *buf)
{
  ATerm trm;
  ATermList list;
  ATermAppl appl;
  ATermBlob blob;
  AFun sym;
  int i, size, arity;
  char *name;

  switch (ATgetType(t))
  {
    case AT_INT:
      /*{{{  write integer */

      sprintf(buf, "%d", ATgetInt((ATermInt) t));
      buf += strlen(buf);

      /*}}}  */
      break;

    case AT_REAL:
      /*{{{  write real */

      sprintf(buf, "%.15e", ATgetReal((ATermReal) t));
      buf += strlen(buf);

      /*}}}  */
      break;

    case AT_APPL:
      /*{{{  write appl */

      appl = (ATermAppl) t;
      sym = ATgetSymbol(appl);
      arity = ATgetArity(sym);
      name = ATgetName(sym);
      buf = writeSymbolToString(sym, buf);
      if (arity > 0 || (!ATisQuoted(sym) && *name == '\0')) {
	*buf++ = '(';
	if (arity > 0) {
	  buf = topWriteToString(ATgetArgument(appl, 0), buf);
	  for (i = 1; i < arity; i++) {
	    *buf++ = ',';
	    buf = topWriteToString(ATgetArgument(appl, i), buf);
	  }
	}
	*buf++ = ')';
      }

      /*}}}  */
      break;

    case AT_LIST:
      /*{{{  write list */

      list = (ATermList) t;
      if (!ATisEmpty(list))
      {
	buf = topWriteToString(ATgetFirst(list), buf);
	list = ATgetNext(list);
	while (!ATisEmpty(list))
	{
	  *buf++ = ',';
	  buf = topWriteToString(ATgetFirst(list), buf);
	  list = ATgetNext(list);
	}
      }

      /*}}}  */
      break;

    case AT_PLACEHOLDER:
      /*{{{  write placeholder */

      trm = ATgetPlaceholder((ATermPlaceholder) t);
      buf = topWriteToString(trm, buf);

      /*}}}  */
      break;

    case AT_BLOB:
      /*{{{  write blob */

      blob = (ATermBlob) t;
      size = ATgetBlobSize(blob);
      sprintf(buf, "\"%c%-.*d%c", STRING_MARK, LENSPEC, size, STRING_MARK);
      buf += 1 + 2 + LENSPEC;

      memcpy(buf, ATgetBlobData(blob), size);
      buf += size;

      *buf++ = '"';

      /*}}}  */
      break;
  }
  return buf;
}

static char    *
topWriteToString(ATerm t, char *buf)
{
  ATerm annos = AT_getAnnotations(t);

  if (ATgetType(t) == AT_LIST) {
    *buf++ = '[';
    buf = writeToString(t, buf);
    *buf++ = ']';
  } else if (ATgetType(t) == AT_PLACEHOLDER) {
    *buf++ = '<';
    buf = writeToString(t, buf);
    *buf++ = '>';
  } else {
    buf = writeToString(t, buf);
  }

  if (annos) {
    *buf++ = '{';
    buf = writeToString(annos, buf);
    *buf++ = '}';
  }

  return buf;
}

/*}}}  */
/*{{{  static int textSize(ATerm t) */

/**
 * Calculate the size of a term in text format
 */

static int      topTextSize(ATerm t);

static int
textSize(ATerm t)
{
  char numbuf[32];
  ATerm trm;
  ATermList list;
  ATermAppl appl;
  Symbol sym;
  int i, size, arity;
  char *name;

  switch (ATgetType(t))
  {
    case AT_INT:
      sprintf(numbuf, "%d", ATgetInt((ATermInt) t));
      size = strlen(numbuf);
      break;

    case AT_REAL:
      sprintf(numbuf, "%.15e", ATgetReal((ATermReal) t));
      size = strlen(numbuf);
      break;

    case AT_APPL:
      appl = (ATermAppl) t;
      sym = ATgetSymbol(appl);
      arity = ATgetArity(sym);
      name = ATgetName(sym);
      size = symbolTextSize(sym);
      for (i = 0; i < arity; i++) {
	size += topTextSize(ATgetArgument(appl, i));
      }
      if (arity > 0 || (!ATisQuoted(sym) && *name == '\0')) {
	/* Add space for the ',' characters */
	if (arity > 1) {
	  size += arity - 1;
	}
	/* and for the '(' and ')' characters */
	size += 2;
      }
      break;

    case AT_LIST:
      list = (ATermList) t;
      if (ATisEmpty(list))
	size = 0;
      else
      {
	size = ATgetLength(list) - 1;	/* Space for the ','
					 * characters */
	while (!ATisEmpty(list))
	{
	  size += topTextSize(ATgetFirst(list));
	  list = ATgetNext(list);
	}
      }
      break;

    case AT_PLACEHOLDER:
      trm = ATgetPlaceholder((ATermPlaceholder) t);
      size = topTextSize(trm);
      break;

    case AT_BLOB:
      size =  LENSPEC + 4 + ATgetBlobSize((ATermBlob) t);
      break;

    default:
      ATerror("textSize: Illegal type %d\n", ATgetType(t));
      return -1;
  }
  return size;
}

static int
topTextSize(ATerm t)
{
  ATerm annos = AT_getAnnotations(t);
  int size = textSize(t);

  if (ATgetType(t) == AT_LIST || ATgetType(t) == AT_PLACEHOLDER) {
    size += 2; /* For markers on both sides of the term */
  }

  if (annos) {
    size += 2; /* '{' and '}' */
    size += textSize(annos);
  }

  return size;
}

int
AT_calcTextSize(ATerm t)
{
  return topTextSize(t);
}

/*}}}  */

/**
 * Write a term into its text representation.
 */

char *
ATwriteToString(ATerm t)
{
  int size = topTextSize(t)+1;
  char *end;

  RESIZE_BUFFER(size);

  end = topWriteToString(t, buffer);
  *end++ = '\0';

  assert(end - buffer == size);

  return buffer;
}


/**
 * Write the text representation of a term into a buffer.
 */

void
AT_writeToStringBuffer(ATerm t, char *buffer)
{
  topWriteToString(t, buffer);
}

/*}}}  */
/*{{{  int ATcalcTextSize(ATerm t) */

int ATcalcTextSize(ATerm t)
{
  return AT_calcTextSize(t);
}

/*}}}  */

/*{{{  static void store_char(int char) */

/**
 * Store a single character in the buffer
 */

static void
store_char(int c, int pos)
{
  if (pos >= buffer_size)
    resize_buffer(buffer_size * 2);	/* Double the space */

  buffer[pos] = c;
}

/*}}}  */
/*{{{  static void fnext_char(int *c, FILE *f) */

/**
 * Read the next character from file.
 */

static void
fnext_char(int *c, FILE * f)
{
  *c = fgetc(f);
  if(*c != EOF) {
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
fskip_layout(int *c, FILE * f)
{
  while (isspace(*c))
    fnext_char(c, f);
}

/*}}}  */
/*{{{  static void fnext_skip_layout(int *c, FILE *f) */

/**
 * Skip layout from file.
 */

static void
fnext_skip_layout(int *c, FILE * f)
{
  do
  {
    fnext_char(c, f);
  } while (isspace(*c));
}

/*}}}  */

/*{{{  static ATermList fparse_terms(int *c, FILE *f) */

/**
 * Parse a list of arguments.
 */

ATermList
fparse_terms(int *c, FILE * f)
{
  ATermList list;
  ATerm el = fparse_term(c, f);

  if(el == NULL) {
    return NULL;
  }

  list = ATinsert(ATempty, el);

  while(*c == ',') {
    fnext_skip_layout(c, f);
    el = fparse_term(c, f);
    if(el == NULL) {
      return NULL;
    }
    list = ATinsert(list, el);
  }

  return ATreverse(list);
}

/*}}}  */
/*{{{  static ATerm fparse_blob(int *c, FILE *f) */

static ATerm fparse_blob(int *c, FILE *f)
{
  char lenspec[LENSPEC+2];
  int len;
  char *data;

  if (fread(lenspec, 1, LENSPEC+1, f) != LENSPEC+1) {
    return NULL;
  }

  if (lenspec[LENSPEC] != ((char)STRING_MARK)) {
    return NULL;
  }

  lenspec[LENSPEC] = '\0';

  len = atoi(lenspec);

  data = (char *)malloc(len);
  if (!data) {
    ATerror("out of memory in fparse_blob\n");
  }
  if (fread(data, 1, len, f) != len) {
    return NULL;
  }

  fnext_char(c, f);
  if (*c != '"') {
    return NULL;
  }

  fnext_skip_layout(c, f);

  return (ATerm)ATmakeBlob(len, data);
}

/*}}}  */
/*{{{  static ATermAppl fparse_quoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static ATerm
fparse_quoted_appl(int *c, FILE * f)
{
  int             len = 0;
  ATermList       args = ATempty;
  Symbol          sym;
  char           *name;

  /* First parse the identifier */
  fnext_char(c, f);

  if (*c == STRING_MARK) {
    return fparse_blob(c, f);
  }

  while (*c != '"') {
    switch (*c) {
      case EOF:
	return NULL;
      case '\\':
	fnext_char(c, f);
	if (*c == EOF)
	  return NULL;
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
    fnext_char(c, f);
  }

  store_char('\0', len);

  name = strdup(buffer);
  if (!name)
    ATerror("fparse_quoted_appl: symbol too long.");

  fnext_skip_layout(c, f);

  /* Time to parse the arguments */
  if (*c == '(') {
    fnext_skip_layout(c, f);
    if(*c != ')') {
      args = fparse_terms(c, f);
    } else {
      args = ATempty;
    }
    if (args == NULL || *c != ')')
      return NULL;
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = ATmakeSymbol(name, ATgetLength(args), ATtrue);
  free(name);
  return (ATerm)ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static ATermAppl fparse_unquoted_appl(int *c, FILE *f) */

/**
 * Parse a quoted application.
 */

static ATermAppl
fparse_unquoted_appl(int *c, FILE * f)
{
  int len = 0;
  Symbol sym;
  ATermList args = ATempty;
  char *name = NULL;

  if (*c != '(') {
    /* First parse the identifier */
    while (isalnum(*c)
	   || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      store_char(*c, len++);
      fnext_char(c, f);
    }
    store_char('\0', len++);
    name = strdup(buffer);
    if (!name) {
      ATerror("fparse_unquoted_appl: symbol too long.");
    }

    fskip_layout(c, f);
  }

  /* Time to parse the arguments */
  if (*c == '(') {
    fnext_skip_layout(c, f);
    if(*c != ')') {
      args = fparse_terms(c, f);
    } else {
      args = ATempty;
    }
    if (args == NULL || *c != ')') {
      return NULL;
    }
    fnext_skip_layout(c, f);
  }

  /* Wrap up this function application */
  sym = ATmakeSymbol(name ? name : "", ATgetLength(args), ATfalse);
  if (name != NULL) {
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
fparse_num(int *c, FILE * f)
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
  if (*c == '.' || toupper(*c) == 'E')
  {
    /*{{{  A real number */

    if (*c == '.')
    {
      *ptr++ = *c;
      fnext_char(c, f);
      while (isdigit(*c) && ptr < numend)
      {
	*ptr++ = *c;
	fnext_char(c, f);
      }
    }
    if (toupper(*c) == 'E' && ptr < numend)
    {
      *ptr++ = *c;
      fnext_char(c, f);
      if (*c == '-' || *c == '+')
      {
	*ptr++ = *c;
	fnext_char(c, f);
      }
      while (ptr < numend && isdigit(*c))
      {
	*ptr++ = *c;
	fnext_char(c, f);
      }
    }
    *ptr = '\0';
    return (ATerm) ATmakeReal(atof(num));

    /*}}}  */
  }
  else
  {
    /*{{{  An integer */

    *ptr = '\0';
    return (ATerm) ATmakeInt(atoi(num));

    /*}}}  */
  }
}

/*}}}  */
/*{{{  static ATerm fparse_term(int *c, FILE *f) */

/**
 * Parse a term from file.
 */

static ATerm
fparse_term(int *c, FILE * f)
{
  ATerm t, result = NULL;

  switch (*c)
  {
    case '"':
      result = (ATerm) fparse_quoted_appl(c, f);
      break;
    case '[':
      fnext_skip_layout(c, f);
      if (*c == ']')
	result = (ATerm) ATempty;
      else
      {
	result = (ATerm) fparse_terms(c, f);
	if (result == NULL || *c != ']')
	  return NULL;
      }
      fnext_skip_layout(c, f);
      break;
    case '<':
      fnext_skip_layout(c, f);
      t = fparse_term(c, f);
      if (t != NULL && *c == '>')
      {
	result = (ATerm) ATmakePlaceholder(t);
	fnext_skip_layout(c, f);
      }
      break;
    default:
      if (isalpha(*c) || *c == '(') {
	result = (ATerm) fparse_unquoted_appl(c, f);
      }
      else if (isdigit(*c)) {
	result = fparse_num(c, f);
      }
      else if (*c == '.' || *c == '-') {
	result = fparse_num(c, f);
      }
      else {
	result = NULL;
      }
  }

  if(result != NULL) {
    fskip_layout(c, f);

    if (*c == '{') {
      /* Term is annotated */
      fnext_skip_layout(c, f);
      if (*c != '}') {
	ATerm annos = (ATerm) fparse_terms(c, f);
	if (annos == NULL || *c != '}')
	  return NULL;
	result = AT_setAnnotations(result, annos);
      }
      fnext_skip_layout(c, f);
    }
    /*{{{  Parse backwards compatible toolbus anomalies */

    if (*c == ':') {
      ATerm type;
      fnext_skip_layout(c, f);
      type = fparse_term(c, f);
      if (type != NULL) {
	result = ATsetAnnotation(result, ATparse("type"), type);
      } else {
	return NULL;
      }
    }

    if (*c == '?') {
      fnext_skip_layout(c, f);
      result = ATsetAnnotation(result, ATparse("result"), ATparse("true"));
    }

    /*}}}  */
  }

  return result;
}

/*}}}  */

/*{{{  ATerm readFromTextFile(FILE *file) */

/**
 * Read a term from a text file. The first character has been read.
 */

ATerm
readFromTextFile(int *c, FILE *file)
{
  ATerm term;
  fskip_layout(c, file);

  term = fparse_term(c, file);

  if (term)
  {
    ungetc(*c, file);
  }
  else
  {
    int i;
    fprintf(stderr, "readFromTextFile: parse error at line %d, col %d%s",
	    line, col, (line||col)?":\n":"");
    for (i = 0; i < ERROR_SIZE; ++i)
    {
      char c = error_buf[(i + error_idx) % ERROR_SIZE];
      if (c)
	fprintf(stderr, "%c", c);
    }
    fprintf(stderr, "\n");
    fflush(stderr);
  }

  return term;
}

/*}}}  */
/*{{{  ATerm ATreadFromTextFile(FILE *file) */

/**
 * Read a term from a text file.
 */

ATerm
ATreadFromTextFile(FILE * file)
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
/*{{{  ATerm ATreadFromFile(FILE *file) */

/**
 * Read an ATerm from a file that could be binary or text.
 */

ATerm ATreadFromFile(FILE *file)
{
  int c;

  fnext_char(&c, file);
  if(c == 0) {
    /* Might be a BAF file */
    return ATreadFromBinaryFile(file);
  } else if (c == START_OF_SHARED_TEXT_FILE) {
    /* Might be a shared text file */
    return AT_readFromSharedTextFile(&c, file);
  } else {
    /* Probably a text file */
    line = 0;
    col = 0;
    error_idx = 0;
    memset(error_buf, 0, ERROR_SIZE);

    return readFromTextFile(&c, file);
  }
}

/*}}}  */
/*{{{  ATerm ATreadFromNamedFile(char *name) */

/**
 * Read an ATerm from a named file
 */

ATerm ATreadFromNamedFile(const char *name)
{  
  FILE  *f;
  ATerm t;

  if(!strcmp(name, "-"))
    return ATreadFromFile(stdin);

  if(!(f = fopen(name, "rb")))
    return NULL;

  t = ATreadFromFile(f);
  fclose(f);

  return t;
}

/*}}}  */


#define snext_char(c,s) ((*c) = ((unsigned char)*(*s)++))
#define sskip_layout(c,s) while(isspace(*c)) snext_char(c,s)
#define snext_skip_layout(c,s) do { snext_char(c, s); } while(isspace(*c))

/*{{{  static ATermList sparse_terms(int *c, char **s) */

/**
 * Parse a list of arguments.
 */

ATermList
sparse_terms(int *c, char **s)
{
  ATermList list;
  ATerm el = sparse_term(c, s);

  if(el == NULL) {
    return NULL;
  }

  list = ATinsert(ATempty, el);

  while(*c == ',') {
    snext_skip_layout(c, s);
    el = sparse_term(c, s);
    if(el == NULL) {
      return NULL;
    }
    list = ATinsert(list, el);
  }

  return ATreverse(list);
}

/*}}}  */
/*{{{  static ATerm sparse_blob(int *c, char **s) */

static ATerm sparse_blob(int *c, char **s)
{
  char *lenspec;
  int len;
  char *data;
  ATermBlob blob;

  lenspec = *s;
  len = atoi(lenspec);
  if (lenspec[LENSPEC] != (char)STRING_MARK) {
    return NULL;
  }

  *s += (LENSPEC+1);

  data = malloc(len);
  if (!data) {
    ATerror("out of memory in sparse_blob (%d)\n", len);
  }
  memcpy(data, *s, len);

  blob = ATmakeBlob(len, data);

  *s += len;

  snext_char(c, s);
  if (*c != '"') {
    return NULL;
  }

  snext_char(c, s);

  return (ATerm)blob;
}

/*}}}  */
/*{{{  static ATermAppl sparse_quoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static ATerm
sparse_quoted_appl(int *c, char **s)
{
  int             len = 0;
  ATermList       args = ATempty;
  Symbol          sym;
  char           *name;

  /* First parse the identifier */
  snext_char(c, s);

  if (*c == STRING_MARK) {
    return sparse_blob(c, s);
  }

  while (*c != '"')
  {
    switch (*c)
    {
      case EOF:
	/*	case '\n':
		case '\r':
		case '\t':
		*/
	return NULL;
      case '\\':
	snext_char(c, s);
	if (*c == EOF)
	  return NULL;
	switch (*c)
	{
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
    snext_char(c, s);
  }

  store_char('\0', len);

  name = strdup(buffer);
  if (!name)
    ATerror("fparse_quoted_appl: symbol too long.");

  snext_skip_layout(c, s);

  /* Time to parse the arguments */
  if (*c == '(')
  {
    snext_skip_layout(c, s);
    if(*c != ')') {
      args = sparse_terms(c, s);
    } else {
      args = ATempty;
    }
    if (args == NULL || *c != ')')
      return NULL;
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = ATmakeSymbol(name, ATgetLength(args), ATtrue);
  free(name);
  return (ATerm)ATmakeApplList(sym, args);
}

/*}}}  */
/*{{{  static ATermAppl sparse_unquoted_appl(int *c, char **s) */

/**
 * Parse a quoted application.
 */

static ATermAppl
sparse_unquoted_appl(int *c, char **s)
{
  int len = 0;
  Symbol sym;
  ATermList args = ATempty;
  char *name = NULL;

  if (*c != '(') {
    /* First parse the identifier */
    while (isalnum(*c)
	   || *c == '-' || *c == '_' || *c == '+' || *c == '*' || *c == '$')
    {
      store_char(*c, len++);
      snext_char(c, s);
    }
    store_char('\0', len);
    name = strdup(buffer);
    if (!name) {
      ATerror("sparse_unquoted_appl: symbol too long.");
    }

    sskip_layout(c, s);
  }

  /* Time to parse the arguments */
  if (*c == '(') {
    snext_skip_layout(c, s);
    if(*c != ')') {
      args = sparse_terms(c, s);
    } else {
      args = ATempty;
    }
    if (args == NULL || *c != ')')
      return NULL;
    snext_skip_layout(c, s);
  }

  /* Wrap up this function application */
  sym = ATmakeSymbol(name ? name : "", ATgetLength(args), ATfalse);
  if (name != NULL) {
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
sparse_num(int *c, char **s)
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
  if (*c == '.' || toupper(*c) == 'E')
  {
    /*{{{  A real number */

    if (*c == '.')
    {
      *ptr++ = *c;
      snext_char(c, s);
      while (isdigit(*c))
      {
	*ptr++ = *c;
	snext_char(c, s);
      }
    }
    if (toupper(*c) == 'E')
    {
      *ptr++ = *c;
      snext_char(c, s);
      if (*c == '-' || *c == '+')
      {
	*ptr++ = *c;
	snext_char(c, s);
      }
      while (isdigit(*c))
      {
	*ptr++ = *c;
	snext_char(c, s);
      }
    }
    *ptr = '\0';
    return (ATerm) ATmakeReal(atof(num));

    /*}}}  */
  }
  else
  {
    /*{{{  An integer */

    *ptr = '\0';
    return (ATerm) ATmakeInt(atoi(num));

    /*}}}  */
  }
}

/*}}}  */

/*{{{  static ATerm sparse_term(int *c, char **s) */

/**
 * Parse a term from file.
 */

static ATerm
sparse_term(int *c, char **s)
{
  ATerm t, result = NULL;

  switch (*c)
  {
    case '"':
      result = (ATerm) sparse_quoted_appl(c, s);
      break;
    case '[':
      snext_skip_layout(c, s);
      if (*c == ']')
	result = (ATerm) ATempty;
      else
      {
	result = (ATerm) sparse_terms(c, s);
	if (result == NULL || *c != ']')
	  return NULL;
      }
      snext_skip_layout(c, s);
      break;
    case '<':
      snext_skip_layout(c, s);
      t = sparse_term(c, s);
      if (t != NULL && *c == '>')
      {
	result = (ATerm) ATmakePlaceholder(t);
	snext_skip_layout(c, s);
      }
      break;
    default:
      if (isalpha(*c) || *c == '(') {
	result = (ATerm) sparse_unquoted_appl(c, s);
      }
      else if (isdigit(*c)) {
	result = sparse_num(c, s);
      }
      else if (*c == '.' || *c == '-') {
	result = sparse_num(c, s);
      }
      else {
	result = NULL;
      }
  }

  if(result != NULL) {
    sskip_layout(c, s);

    if (*c == '{') {
      /*{{{  Parse annotation  */

      /* Term is annotated */
      snext_skip_layout(c, s);
      if (*c != '}') {
	ATerm annos = (ATerm) sparse_terms(c, s);
	if (annos == NULL || *c != '}')
	  return NULL;
	result = AT_setAnnotations(result, annos);
      }
      snext_skip_layout(c, s);

      /*}}}  */
    }

    /*{{{  Parse backwards compatible toolbus anomalies */

    if (*c == ':') {
      ATerm type;
      snext_skip_layout(c, s);
      type = sparse_term(c, s);
      if (type != NULL) {
	result = ATsetAnnotation(result, ATparse("type"), type);
      } else {
	return NULL;
      }
    }

    if (*c == '?') {
      snext_skip_layout(c, s);
      result = ATsetAnnotation(result, ATparse("result"), ATparse("true"));
    }

    /*}}}  */
  }

  return result;
}

/*}}}  */
/*{{{  ATerm ATreadFromString(const char *string) */

/**
 * Read from a string.
 */

ATerm
ATreadFromString(const char *string)
{
  int             c;
  const char     *orig = string;
  ATerm           term;

  snext_skip_layout(&c, (char **) &string);

  term = sparse_term(&c, (char **) &string);

  if (term == NULL)
  {
    int i;
    fprintf(stderr, "ATreadFromString: parse error at or near:\n");
    fprintf(stderr, "%s\n", orig);
    for (i = 1; i < string - orig; ++i)
      fprintf(stderr, " ");
    fprintf(stderr, "^\n");
  }
  else
    string--;

  return term;
}

/*}}}  */

/*{{{  void AT_markTerm(ATerm t) */

/**
 * Mark a term and all of its children.
 */
void AT_markTerm(ATerm t)
{
  int             i, arity;
  Symbol          sym;
  ATerm          *current = mark_stack + 1;
  ATerm          *limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;
  
  mark_stack[0] = NULL;
  *current++ = t;

  while (ATtrue) {
    if (current >= limit) {
      int current_index;
      current_index = current - mark_stack;

      /* We need to resize the mark stack */
      mark_stack_size = mark_stack_size * 2;
      mark_stack = (ATerm *) realloc(mark_stack, sizeof(ATerm) * mark_stack_size);
      if (!mark_stack)
	ATerror("cannot realloc mark stack to %d entries.\n", mark_stack_size);
      limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;
      if(!silent) {
	fprintf(stderr, "resized mark stack to %d entries\n", mark_stack_size);
      }
      fflush(stderr);

      current = mark_stack + current_index;
    }

    t = *--current;

    if (!t) {
      if(current != mark_stack) {
	ATabort("AT_markTerm: premature end of mark_stack.\n");
      }
      break;
    }

    if (IS_MARKED(t->header))
      continue;

    SET_MARK(t->header);
    
    INCREMENT_AGE(t->header);
    
    if(HAS_ANNO(t->header))
      *current++ = AT_getAnnotations(t);

    switch (GET_TYPE(t->header)) {
      case AT_INT:
      case AT_REAL:
      case AT_BLOB:
	break;

      case AT_APPL:
	sym = ATgetSymbol((ATermAppl) t);

          /*fprintf(stderr,"AT_markTerm: AT_markSymbol(adr = %d, id = %d)\n",at_lookup_table[(sym)],sym);*/
        if(AT_isValidSymbol(sym)) {
          AT_markSymbol(sym);
        } else {
          continue;
        }
            /*
        {
          SymEntry tmpTerm;
          tmpTerm = at_lookup_table[(sym)];
          printf("sym = %d\n",sym);
          printf("tmpTerm = %x\n",(void*)tmpTerm);
          printf("tmpTerm->header = %x\n",tmpTerm->header);
          tmpTerm->header |= MASK_MARK;
        }
            */
	arity = GET_ARITY(t->header);
	if (arity > MAX_INLINE_ARITY) {
	  arity = ATgetArity(sym);
	}
	for (i = 0; i < arity; i++) {
	  ATerm arg = ATgetArgument((ATermAppl) t, i);
	  *current++ = arg;
	}
	break;

      case AT_LIST:
	if (!ATisEmpty((ATermList) t)) {
	  *current++ = (ATerm) ATgetNext((ATermList) t);
	  *current++ = ATgetFirst((ATermList) t);
	}
	break;

      case AT_PLACEHOLDER:
	*current++ = ATgetPlaceholder((ATermPlaceholder) t);
	break;
    }
  }
}

/* Jurgen asks: why is this function not in gc.c ? */
void AT_markTerm_young(ATerm t) 
{
  int             i, arity;
  Symbol          sym;
  ATerm          *current = mark_stack + 1;
  ATerm          *limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;

  if(IS_MARKED(t->header) || IS_OLD(t->header)) {
      /*fprintf(stderr,"AT_markTerm_young (%p) STOP MARK: age = %d\n",t,GET_AGE(t->header));*/
    return;
  }
  
  mark_stack[0] = NULL;
  *current++ = t;

  while (ATtrue) {
    if (current >= limit) {
      int current_index;

      current_index = current - mark_stack;
      /* We need to resize the mark stack */
      mark_stack_size = mark_stack_size * 2;
      mark_stack = (ATerm *) realloc(mark_stack, sizeof(ATerm) * mark_stack_size);
      if (!mark_stack)
	ATerror("cannot realloc mark stack to %d entries.\n", mark_stack_size);
      limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;
      if(!silent) {
	fprintf(stderr, "resized mark stack to %d entries\n", mark_stack_size);
      }
      fflush(stderr);

      current = mark_stack + current_index;
    }

    t = *--current;

    if (!t) {
      if(current != mark_stack) {
	ATabort("AT_markTerm: premature end of mark_stack.\n");
      }
      break;
    }
      /* TODO: optimize*/
    if(IS_MARKED(t->header) || IS_OLD(t->header)) {
        /*fprintf(stderr,"AT_markTerm_young (%p) STOP MARK: age = %d\n",t,GET_AGE(t->header));*/
      continue;
    }

    SET_MARK(t->header);
      /*fprintf(stderr,"MINOR YOUNG MARK(%x)\n",(unsigned int)t);*/
      /*fprintf(stderr,"YOUNG INCREMENT_AGE(%x,%d)\n",(unsigned int)t,GET_AGE(t->header));*/
    INCREMENT_AGE(t->header);

    if(HAS_ANNO(t->header))
      *current++ = AT_getAnnotations(t);

    switch (GET_TYPE(t->header)) {
      case AT_INT:
      case AT_REAL:
      case AT_BLOB:
	break;

      case AT_APPL:
	sym = ATgetSymbol((ATermAppl) t);
          /*fprintf(stderr,"AT_markTerm_young: AT_markSymbol_young(%d)\n",sym);*/
        if(AT_isValidSymbol(sym)) {
          AT_markSymbol_young(sym);
        } else {
          continue;
        }

	arity = GET_ARITY(t->header);
	if (arity > MAX_INLINE_ARITY) {
	  arity = ATgetArity(sym);
	}
	for (i = 0; i < arity; i++) {
	  ATerm arg = ATgetArgument((ATermAppl) t, i);
	  *current++ = arg;
	}
        
	break;

      case AT_LIST:
	if (!ATisEmpty((ATermList) t)) {
	  *current++ = (ATerm) ATgetNext((ATermList) t);
	  *current++ = ATgetFirst((ATermList) t);
	}
	break;

      case AT_PLACEHOLDER:
	*current++ = ATgetPlaceholder((ATermPlaceholder) t);
	break;
    }
  }
}

/*}}}  */
/*{{{  void AT_unmarkTerm(ATerm t) */

/**
 * Unmark a term and all of its children.
 * pre: the whole term must be marked.
 */

void
AT_unmarkTerm(ATerm t)
{
  int             i, arity;
  Symbol          sym;
  ATerm          *current = mark_stack + 1;
  ATerm          *limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;
  ATerm          *depth = mark_stack;

  mark_stack[0] = NULL;
  *current++ = t;

  while (ATtrue) {
    if (current > limit) {
      int current_index, depth_index;

      current_index = current - mark_stack;
      depth_index   = depth - mark_stack;

      /* We need to resize the mark stack */
      mark_stack_size = mark_stack_size * 2;
      mark_stack = (ATerm *) realloc(mark_stack, sizeof(ATerm) * mark_stack_size);
      if (!mark_stack)
	ATerror("cannot realloc mark stack to %d entries.\n", mark_stack_size);
      limit = mark_stack + mark_stack_size - MARK_STACK_MARGE;
      if(!silent) {
	fprintf(stderr, "resized mark stack to %d entries\n", mark_stack_size);
      }

      current = mark_stack + current_index;
      depth   = mark_stack + depth_index;
    }

    if (current > depth)
      depth = current;

    t = *--current;

    if (!t)
      break;

    CLR_MARK(t->header);

    if(HAS_ANNO(t->header))
      *current++ = AT_getAnnotations(t);

    switch (GET_TYPE(t->header)) {
      case AT_INT:
      case AT_REAL:
      case AT_BLOB:
	break;

      case AT_APPL:
	sym = ATgetSymbol((ATermAppl) t);
	AT_unmarkSymbol(sym);
	arity = GET_ARITY(t->header);
	if (arity > MAX_INLINE_ARITY)
	  arity = ATgetArity(sym);
	for (i = 0; i < arity; i++)
	  *current++ = ATgetArgument((ATermAppl) t, i);
	break;

      case AT_LIST:
	if (!ATisEmpty((ATermList) t)) {
	  *current++ = (ATerm) ATgetNext((ATermList) t);
	  *current++ = ATgetFirst((ATermList) t);
	}
	break;

      case AT_PLACEHOLDER:
	*current++ = ATgetPlaceholder((ATermPlaceholder) t);
	break;
    }
  }
}

/*}}}  */
/*{{{  void AT_unmarkIfAllMarked(ATerm t) */

void AT_unmarkIfAllMarked(ATerm t)
{
  if(IS_MARKED(t->header)) {
    /*ATfprintf(stderr, "* unmarking %t\n", t);*/
    CLR_MARK(t->header);
    switch(ATgetType(t)) {
      case AT_INT:
      case AT_REAL:
      case AT_BLOB:
	break;

      case AT_PLACEHOLDER:
	AT_unmarkIfAllMarked(ATgetPlaceholder((ATermPlaceholder)t));
	break;

      case AT_LIST:
	{
	  ATermList list = (ATermList)t;
	  while(!ATisEmpty(list)) {
	    CLR_MARK(list->header);
	    AT_unmarkIfAllMarked(ATgetFirst(list));
	    list = ATgetNext(list);
	    if (!IS_MARKED(list->header)) {
	      break;
	    }
	  }
	  CLR_MARK(list->header);
	}
	break;
      case AT_APPL:
	{
	  ATermAppl appl = (ATermAppl)t;
	  int cur_arity, cur_arg;
	  AFun sym;

	  sym = ATgetAFun(appl);
	  AT_unmarkSymbol(sym);
	  cur_arity = ATgetArity(sym);
	  for(cur_arg=0; cur_arg<cur_arity; cur_arg++) {
	    AT_unmarkIfAllMarked(ATgetArgument(appl, cur_arg));
	  }
	}
	break;
      default:
	ATerror("collect_terms: illegal term\n");
	break;
    }		

    if(HAS_ANNO(t->header)) {
      /*ATfprintf(stderr, "* unmarking annos of %t\n", t);*/
      AT_unmarkIfAllMarked(AT_getAnnotations(t));
    }
  }
  else {
    /*ATfprintf(stderr, "* already unmarked %t\n", t);*/
  }
}

/*}}}  */
/*{{{  void AT_unmarkAll() */

void AT_unmarkAll()
{
  int size;

  for (size=1; size<MAX_TERM_SIZE; size++) {
    int last = BLOCK_SIZE - (BLOCK_SIZE % size) - size;
    Block *block = at_blocks[size];
    while (block) {
      int idx;
      ATerm data = (ATerm)block->data;
      for (idx=0; idx <= last; idx += size) {
	CLR_MARK(((ATerm)(((header_type *)data)+idx))->header);
      }
      block = block->next_by_size;
    }
  }

  AT_unmarkAllAFuns();
}

/*}}}  */

/*{{{  static int calcCoreSize(ATerm t) */

/**
 * Calculate the term size in bytes.
 */

static int
calcCoreSize(ATerm t)
{
  int             i, arity, size = 0;
  Symbol          sym;

  if (IS_MARKED(t->header))
    return size;

  SET_MARK(t->header);

  switch (ATgetType(t)) {
    case AT_INT:
      size = 12;
      break;

    case AT_REAL:
    case AT_BLOB:
      size = 16;
      break;

    case AT_APPL:
      sym = ATgetSymbol((ATermAppl) t);
      arity = ATgetArity(sym);
      size = 8 + arity * 4;
      if (!AT_isMarkedSymbol(sym)) {
	size += strlen(ATgetName(sym)) + 1;
	size += sizeof(struct _SymEntry);
	AT_markSymbol(sym);
      }
      for (i = 0; i < arity; i++)
	size += calcCoreSize(ATgetArgument((ATermAppl) t, i));
      break;

    case AT_LIST:
      size = 16;
      while (!ATisEmpty((ATermList) t)) {
	size += 16;
	size += calcCoreSize(ATgetFirst((ATermList) t));
	t = (ATerm)ATgetNext((ATermList)t);
      }
      break;

    case AT_PLACEHOLDER:
      size = 12;
      size += calcCoreSize(ATgetPlaceholder((ATermPlaceholder) t));
      break;
  }

  if(HAS_ANNO(t->header))
    size += calcCoreSize(AT_getAnnotations(t));

  return size;
}

/*}}}  */
/*{{{  int AT_calcCoreSize(ATerm t) */

/**
 * Calculate the term size in bytes.
 */

int
AT_calcCoreSize(ATerm t)
{
  int result = calcCoreSize(t);
  AT_unmarkTerm(t);
  return result;
}


/*}}}  */
/*{{{  int AT_calcSubterms(ATerm t) */

/**
 * Calculate the number of subterms of a term.
 */

int AT_calcSubterms(ATerm t)
{
  int    i, arity, nr_subterms = 0;
  Symbol sym;
  ATermList list;

  switch (ATgetType(t)) {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
    case AT_PLACEHOLDER:
      nr_subterms = 1;
      break;

    case AT_APPL:
      nr_subterms = 1;
      sym = ATgetSymbol((ATermAppl) t);
      arity = ATgetArity(sym);
      for (i = 0; i < arity; i++)
	nr_subterms += AT_calcSubterms(ATgetArgument((ATermAppl)t, i));
      break;

    case AT_LIST:
      list = (ATermList)t;
      while(!ATisEmpty(list)) {
	nr_subterms += AT_calcSubterms(ATgetFirst(list)) + 1;
	list = ATgetNext(list);
      }
      break;
  }

  if(HAS_ANNO(t->header))
    nr_subterms += AT_calcSubterms(AT_getAnnotations(t));

  return nr_subterms;
}

/*}}}  */

/*{{{  static int calcUniqueSubterms(ATerm t) */

/**
 * Calculate the number of unique subterms.
 */

static int
calcUniqueSubterms(ATerm t)
{
  int    i, arity, nr_unique = 0;
  Symbol sym;
  ATermList list;

  if (IS_MARKED(t->header))
    return 0;

  switch (ATgetType(t)) {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
    case AT_PLACEHOLDER:
      nr_unique = 1;
      break;

    case AT_APPL:
      nr_unique = 1;
      sym = ATgetSymbol((ATermAppl) t);
      arity = ATgetArity(sym);
      for (i = 0; i < arity; i++)
	nr_unique += calcUniqueSubterms(ATgetArgument((ATermAppl)t, i));
      break;

    case AT_LIST:
      list = (ATermList)t;
      while(!ATisEmpty(list) && !IS_MARKED(list->header)) {
	nr_unique += calcUniqueSubterms(ATgetFirst(list)) + 1;
	SET_MARK(list->header);
	list = ATgetNext(list);
      }
      if (ATisEmpty(list) && !IS_MARKED(list->header)) {
	SET_MARK(list->header);
	nr_unique++;
      }
      break;
  }

  if(HAS_ANNO(t->header))
    nr_unique += calcUniqueSubterms(AT_getAnnotations(t));

  SET_MARK(t->header);

  return nr_unique;
}

/*}}}  */
/*{{{  int AT_calcUniqueSubterms(ATerm t) */

/**
 * Calculate the number of unique subterms.
 */

int
AT_calcUniqueSubterms(ATerm t)
{
  int result = calcUniqueSubterms(t);
  AT_unmarkIfAllMarked(t);
  return result;
}

/*}}}  */
/*{{{  int ATcalcUniqueSubterms(ATerm t) */

int ATcalcUniqueSubterms(ATerm t)
{
  return AT_calcUniqueSubterms(t);
}

/*}}}  */

/*{{{  static int calcUniqueSymbols(ATerm t) */

/**
 * Calculate the number of unique symbols.
 */

static int calcUniqueSymbols(ATerm t)
{
  int    i, arity, nr_unique = 0;
  Symbol sym;
  ATermList list;

  if (IS_MARKED(t->header))
    return 0;

  switch (ATgetType(t)) {
    case AT_INT:
      if (!at_lookup_table[AS_INT]->count++)
	nr_unique = 1;
      break;
    case AT_REAL:
      if (!at_lookup_table[AS_REAL]->count++)
	nr_unique = 1;
      break;
    case AT_BLOB:
      if (!at_lookup_table[AS_BLOB]->count++)
	nr_unique = 1;
      break;
    case AT_PLACEHOLDER:
      if (!at_lookup_table[AS_PLACEHOLDER]->count++)
	nr_unique = 1;
      nr_unique += calcUniqueSymbols(ATgetPlaceholder((ATermPlaceholder)t));
      break;

    case AT_APPL:
      sym = ATgetSymbol((ATermAppl) t);
      nr_unique = AT_isMarkedSymbol(sym) ? 0 : 1;
      assert(at_lookup_table[sym]);
      at_lookup_table[sym]->count++;
      AT_markSymbol(sym);
      arity = ATgetArity(sym);
      for (i = 0; i < arity; i++) {
	nr_unique += calcUniqueSymbols(ATgetArgument((ATermAppl)t, i));
      }
      break;

    case AT_LIST:
      list = (ATermList)t;
      while(!ATisEmpty(list) && !IS_MARKED(list->header)) {
	SET_MARK(list->header);
	if (!at_lookup_table[AS_LIST]->count++)
	  nr_unique++;
	nr_unique += calcUniqueSymbols(ATgetFirst(list));
	list = ATgetNext(list);
      }
      if(ATisEmpty(list) && !IS_MARKED(list->header)) {
	SET_MARK(list->header);
	if (!at_lookup_table[AS_EMPTY_LIST]->count++)
	  nr_unique++;
      }
      break;
  }

  if(HAS_ANNO(t->header)) {
    if (!at_lookup_table[AS_ANNOTATION]->count++)
      nr_unique++;
    nr_unique += calcUniqueSymbols(AT_getAnnotations(t));
  }

  SET_MARK(t->header);

  return nr_unique;
}

/*}}}  */
/*{{{  int AT_calcUniqueSymbols(ATerm t) */

/**
 * Calculate the number of unique symbols
 */

int
AT_calcUniqueSymbols(ATerm t)
{
  int result = calcUniqueSymbols(t);
  AT_unmarkIfAllMarked(t);
  /*AT_assertUnmarked(t);*/

  return result;
}


/*}}}  */
/*{{{  int ATcalcUniqueSymbols(ATerm t) */

int ATcalcUniqueSymbols(ATerm t)
{
  return AT_calcUniqueSymbols(t);
}

/*}}}  */

/*{{{  void AT_assertUnmarked(ATerm t) */

void AT_assertUnmarked(ATerm t)
{
  ATermAppl appl;
  Symbol sym;
  int i;

  assert(!IS_MARKED(t->header));
  switch(ATgetType(t)) {
    case AT_APPL:
      appl = (ATermAppl)t;
      sym = ATgetSymbol(appl);
      assert(!AT_isMarkedSymbol(sym));
      for(i=0; i<ATgetArity(sym); i++)
	AT_assertUnmarked(ATgetArgument(appl, i));
      break;

    case AT_LIST:
      if((ATermList)t != ATempty) {
	AT_assertUnmarked(ATgetFirst((ATermList)t));
	AT_assertUnmarked((ATerm)ATgetNext((ATermList)t));
      }
      break;

    case AT_PLACEHOLDER:
      AT_assertUnmarked(ATgetPlaceholder((ATermPlaceholder)t));
      break;
  }

  if(HAS_ANNO(t->header))
    AT_assertUnmarked(AT_getAnnotations(t));
}

/*}}}  */
/*{{{  void AT_assertMarked(ATerm t) */

void AT_assertMarked(ATerm t)
{
  ATermAppl appl;
  Symbol sym;
  int i;

  assert(IS_MARKED(t->header));
  switch(ATgetType(t)) {
    case AT_APPL:
      appl = (ATermAppl)t;
      sym = ATgetSymbol(appl);
      assert(AT_isMarkedSymbol(sym));
      for(i=0; i<ATgetArity(sym); i++)
	AT_assertMarked(ATgetArgument(appl, i));
      break;

    case AT_LIST:
      if((ATermList)t != ATempty) {
	AT_assertMarked(ATgetFirst((ATermList)t));
	AT_assertMarked((ATerm)ATgetNext((ATermList)t));
      }
      break;

    case AT_PLACEHOLDER:
      AT_assertMarked(ATgetPlaceholder((ATermPlaceholder)t));
      break;
  }

  if(HAS_ANNO(t->header))
    AT_assertMarked(AT_getAnnotations(t));

}

/*}}}  */

/*{{{  int AT_calcTermDepth(ATerm t) */

/**
 * Calculate the maximum depth of a term.
 */

int AT_calcTermDepth(ATerm t)
{
  int arity, i, maxdepth = 0, depth = 0;
  ATermAppl appl;
  ATermList list;

  if(HAS_ANNO(t->header))
    maxdepth = AT_calcTermDepth(AT_getAnnotations(t));

  switch(ATgetType(t)) {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
      return MAX(1, maxdepth);

    case AT_APPL:
      appl = (ATermAppl)t;
      arity = ATgetArity(ATgetSymbol(appl));
      for(i=0; i<arity; i++) {
	depth = AT_calcTermDepth(ATgetArgument(appl, i));
	if(depth > maxdepth)
	  maxdepth = depth;
      }
      return maxdepth+1;

    case AT_LIST:
      list = (ATermList)t;
      while(!ATisEmpty(list)) {
	depth = AT_calcTermDepth(ATgetFirst(list));
	if(depth > maxdepth)
	  maxdepth = depth;
	list = ATgetNext(list);
      }
      return maxdepth+1;

    case AT_PLACEHOLDER:
      return 1+MAX(AT_calcTermDepth(ATgetPlaceholder((ATermPlaceholder)t)),
		   maxdepth);

    default:
      ATerror("Trying to calculate the depth of a free term.\n");
      return 0;
  }
}

/*}}}  */

/*{{{  char *ATchecksum(ATerm t) */

/* Calculate checksum using the
   "RSA Data Security, Inc. MD5 Message-Digest Algorithm" (see RFC1321)
   */

unsigned char *ATchecksum(ATerm t)
{
  MD5_CTX context;
  static unsigned char digest[16];
  char *buf;
  int len;

  MD5Init(&context);
  buf = ATwriteToSharedString(t, &len);
  MD5Update(&context, buf, len);
  MD5Final(digest, &context);

  return digest;
}

/*}}}  */

/*{{{  static ATermList AT_diffList(ATermList l1, ATermList l2, ATermList *diffs) */

static ATermList AT_diffList(ATermList l1, ATermList l2, ATermList *diffs)
{
  ATermList result = ATempty;
  ATerm el1, el2;

  while (!ATisEmpty(l1)) {
    if (ATisEmpty(l2)) {
      if (*diffs) {
	*diffs = ATinsert(*diffs, ATmake("diff(<term>,[])", l1));
      }
      return ATreverse(ATinsert(result, ATparse("<diff-lists>")));
    }
    el1 = ATgetFirst(l1);
    el2 = ATgetFirst(l2);
    result = ATinsert(result, AT_diff(el1, el2, diffs));

    l1 = ATgetNext(l1);
    l2 = ATgetNext(l2);
  }

  if (!ATisEmpty(l2)) {
    if (*diffs) {
      *diffs = ATinsert(*diffs, ATmake("diff([],<term>)", l2));
    }
    return ATreverse(ATinsert(result, ATparse("<diff-lists>")));
  }

  return ATreverse(result);
}

/*}}}  */
/*{{{  static ATerm AT_diff(ATerm t1, ATerm t2, ATermList *diffs) */

static ATerm AT_diff(ATerm t1, ATerm t2, ATermList *diffs) 
{
  ATerm diff = NULL;

  if (ATisEqual(t1, t2)) {
    return t1;
  }

  if (ATgetType(t1) != ATgetType(t2)) {
    diff = ATparse("<diff-types>");
  } else {
    switch (ATgetType(t1)) {
      case AT_INT:
      case AT_REAL:
      case AT_BLOB:
	diff = ATparse("<diff-values>");
	break;

      case AT_PLACEHOLDER:
	{
	  ATerm ph1, ph2;

	  ph1 = ATgetPlaceholder((ATermPlaceholder)t1);
	  ph2 = ATgetPlaceholder((ATermPlaceholder)t2);
	  return (ATerm)ATmakePlaceholder(AT_diff(ph1, ph2, diffs));
	}
	break;

      case AT_APPL:
	{
	  ATermAppl appl1, appl2;
	  AFun afun1, afun2;

	  appl1 = (ATermAppl)t1;
	  appl2 = (ATermAppl)t2;
	  afun1 = ATgetAFun(appl1);
	  afun2 = ATgetAFun(appl2);
	  if (afun1 == afun2) {
	    ATermList args1 = ATgetArguments(appl1);
	    ATermList args2 = ATgetArguments(appl2);
	    ATermList args = AT_diffList(args1, args2, diffs);
	    return (ATerm)ATmakeApplList(afun1, args);
	  } else {
	    diff = ATparse("<diff-appls>");
	  }
	}
	break;

      case AT_LIST:
	return (ATerm)AT_diffList((ATermList)t1, (ATermList)t2, diffs);
    }
  }

  if (diffs) {
    *diffs = ATinsert(*diffs, ATmake("diff(<term>,<term>)", t1, t2));
  }

  return diff;
}

/*}}}  */

/*{{{  ATerm ATdiff(ATerm t1, ATerm t2) */

ATbool ATdiff(ATerm t1, ATerm t2, ATerm *template, ATerm *diffs)
{
  ATerm templ;

  if (diffs) {
    *diffs = (ATerm)ATempty;
  }

  templ = AT_diff(t1, t2, (ATermList *)diffs);

  if (template) {
    *template = templ;
  }

  if (diffs) {
    *diffs = (ATerm)ATreverse((ATermList)*diffs);
  }

  return !ATisEqual(t1, t2);
}

/*}}}  */

/*{{{  ATbool AT_isDeepEqual(ATerm t1, ATerm t2) */

/**
 * Check for deep ATerm equality (even identical terms are traversed completely)
 */

ATbool AT_isDeepEqual(ATerm t1, ATerm t2)
{
  int type;
  ATbool result = ATtrue;

  if (t1 == NULL && t2 == NULL) {
    return ATtrue;
  }

  if (t1 == NULL || t2 == NULL) {
    return ATfalse;
  }

  type = ATgetType(t1);
  if (type != ATgetType(t2)) {
    return ATfalse;
  }

  switch(type) {
    case AT_APPL:
      {
	ATermAppl appl1 = (ATermAppl)t1, appl2 = (ATermAppl)t2;
	AFun sym = ATgetAFun(appl1);
	int i, arity = ATgetArity(sym);

	if(sym != ATgetAFun(appl2))
	  return ATfalse;

	for(i=0; i<arity; i++) {
	  if(!AT_isDeepEqual(ATgetArgument(appl1, i), ATgetArgument(appl2, i)))
	    return ATfalse;
	}
      }
      break;

    case AT_LIST:
      {
	ATermList list1 = (ATermList)t1, list2 = (ATermList)t2;
	if(ATgetLength(list1) != ATgetLength(list2))
	  return ATfalse;

	while(!ATisEmpty(list1)) {
	  if(!AT_isDeepEqual(ATgetFirst(list1), ATgetFirst(list2)))
	    return ATfalse;

	  list1 = ATgetNext(list1);
	  list2 = ATgetNext(list2);
	}
      }
      break;

    case AT_INT:
      result = ((ATgetInt((ATermInt)t1) == ATgetInt((ATermInt)t2)) ? ATtrue : ATfalse);
      break;

    case AT_REAL:
      result = ((ATgetReal((ATermReal)t1) == ATgetReal((ATermReal)t2)) ? ATtrue : ATfalse);
      break;

    case AT_BLOB:
      result = ((ATgetBlobData((ATermBlob)t1) == ATgetBlobData((ATermBlob)t2)) &&
		(ATgetBlobSize((ATermBlob)t1) == ATgetBlobSize((ATermBlob)t2))) ? ATtrue : ATfalse;
      break;

    case AT_PLACEHOLDER:
      result = AT_isDeepEqual(ATgetPlaceholder((ATermPlaceholder)t1), 
			      ATgetPlaceholder((ATermPlaceholder)t1));
      break;

    default:
      ATerror("illegal term type: %d\n", type);
  }

  if(result) {
    if(HAS_ANNO(t1->header)) {
      if(HAS_ANNO(t2->header)) {
	result = AT_isDeepEqual(AT_getAnnotations(t1), AT_getAnnotations(t2));
      } else {
	result = ATfalse;
      }
    } else if(HAS_ANNO(t2->header)) {
      result = ATfalse;
    }
  }

  return result;
}

/*}}}  */
/*{{{  ATbool AT_isEqual(ATerm t1, ATerm t2) */

/**
 * Check for deep ATerm equality (only useful when sharing is disabled)
 */

ATbool AT_isEqual(ATerm t1, ATerm t2)
{
  int type;
  ATbool result = ATtrue;

  if(t1 == t2)
    return ATtrue;

  if (t1 == NULL || t2 == NULL) {
    return ATfalse;
  }

  type = ATgetType(t1);
  if(type != ATgetType(t2))
    return ATfalse;

  switch(type) {
    case AT_APPL:
      {
	ATermAppl appl1 = (ATermAppl)t1, appl2 = (ATermAppl)t2;
	AFun sym = ATgetAFun(appl1);
	int i, arity = ATgetArity(sym);

	if(sym != ATgetAFun(appl2))
	  return ATfalse;

	for(i=0; i<arity; i++)
	  if(!AT_isEqual(ATgetArgument(appl1, i), ATgetArgument(appl2, i)))
	    return ATfalse;
      }
      break;

    case AT_LIST:
      {
	ATermList list1 = (ATermList)t1, list2 = (ATermList)t2;
	if(ATgetLength(list1) != ATgetLength(list2))
	  return ATfalse;

	while(!ATisEmpty(list1)) {
	  if(!AT_isEqual(ATgetFirst(list1), ATgetFirst(list2)))
	    return ATfalse;

	  list1 = ATgetNext(list1);
	  list2 = ATgetNext(list2);
	}
      }
      break;

    case AT_INT:
      result = ((ATgetInt((ATermInt)t1) == ATgetInt((ATermInt)t2)) ? ATtrue : ATfalse);
      break;

    case AT_REAL:
      result = ((ATgetReal((ATermReal)t1) == ATgetReal((ATermReal)t2)) ? ATtrue : ATfalse);
      break;

    case AT_BLOB:
      result = ((ATgetBlobData((ATermBlob)t1) == ATgetBlobData((ATermBlob)t2)) &&
		(ATgetBlobSize((ATermBlob)t1) == ATgetBlobSize((ATermBlob)t2))) ? ATtrue : ATfalse;
      break;

    case AT_PLACEHOLDER:
      result = AT_isEqual(ATgetPlaceholder((ATermPlaceholder)t1), 
			 ATgetPlaceholder((ATermPlaceholder)t1));
      break;

    default:
      ATerror("illegal term type: %d\n", type);
  }

  if(result) {
    if(HAS_ANNO(t1->header)) {
      if(HAS_ANNO(t2->header)) {
	result = AT_isEqual(AT_getAnnotations(t1), AT_getAnnotations(t2));
      } else {
	result = ATfalse;
      }
    } else if(HAS_ANNO(t2->header)) {
      result = ATfalse;
    }
  }

  return result;
}

/*}}}  */
/*{{{  ATbool ATisEqualModuloAnnotations(ATerm t1, ATerm t2) */

ATbool ATisEqualModuloAnnotations(ATerm t1, ATerm t2)
{
  int type;
  ATbool result = ATtrue;

  if(t1 == t2)
    return ATtrue;

  if (t1 == NULL || t2 == NULL) {
    return ATfalse;
  }

  type = ATgetType(t1);
  if(type != ATgetType(t2)) {
    return ATfalse;
  }

  switch(type) {
    case AT_APPL:
      {
	ATermAppl appl1 = (ATermAppl)t1, appl2 = (ATermAppl)t2;
	AFun sym = ATgetAFun(appl1);
	int i, arity = ATgetArity(sym);

	if (sym != ATgetAFun(appl2)) {
	  return ATfalse;
	}

	for(i=0; i<arity; i++)
	  if(!ATisEqualModuloAnnotations(ATgetArgument(appl1, i), 
					 ATgetArgument(appl2, i))) {
	    return ATfalse;
	  }
      }
      break;

    case AT_LIST:
      {
	ATermList list1 = (ATermList)t1, list2 = (ATermList)t2;
	if(ATgetLength(list1) != ATgetLength(list2)) {
	  return ATfalse;
	}

	while(!ATisEmpty(list1)) {
	  if (!ATisEqualModuloAnnotations(ATgetFirst(list1), 
					  ATgetFirst(list2))) {
	    return ATfalse;
	  }

	  list1 = ATgetNext(list1);
	  list2 = ATgetNext(list2);
	}
      }
      break;

    case AT_INT:
      result = ((ATgetInt((ATermInt)t1) == ATgetInt((ATermInt)t2)) ? ATtrue : ATfalse);
      break;

    case AT_REAL:
      result = ((ATgetReal((ATermReal)t1) == ATgetReal((ATermReal)t2)) ? ATtrue : ATfalse);
      break;

    case AT_BLOB:
      result = ((ATgetBlobData((ATermBlob)t1) == ATgetBlobData((ATermBlob)t2)) &&
		(ATgetBlobSize((ATermBlob)t1) == ATgetBlobSize((ATermBlob)t2))) ? ATtrue : ATfalse;
      break;

    case AT_PLACEHOLDER:
      result = ATisEqualModuloAnnotations(
			  ATgetPlaceholder((ATermPlaceholder)t1), 
			  ATgetPlaceholder((ATermPlaceholder)t1));
      break;

    default:
      ATerror("illegal term type: %d\n", type);
  }

  return result;
}

/*}}}  */

/*{{{  ATerm ATremoveAllAnnotations(ATerm t) */

ATerm ATremoveAllAnnotations(ATerm t)
{
  switch(ATgetType(t)) {
    case AT_INT:
    case AT_REAL:
    case AT_BLOB:
      return AT_removeAnnotations(t);

    case AT_LIST:
      {
	if(ATisEmpty((ATermList)t)) {
	  return AT_removeAnnotations(t);
	} else {
	  ATermList l = (ATermList)t;
	  ATerm     new_head, head = ATgetFirst(l);
	  ATermList new_tail, tail = ATgetNext(l);
	  new_head = ATremoveAllAnnotations(head);
	  new_tail = (ATermList)ATremoveAllAnnotations((ATerm)tail);
	  if (new_head == head && new_tail == tail) {
	    return AT_removeAnnotations(t);
	  }
	  return (ATerm)ATinsert(new_tail, new_head);
	}
      }

    case AT_APPL:
      {
	ATermAppl appl = (ATermAppl)t;
	AFun fun = ATgetAFun(appl);
	int arity = ATgetArity(fun);
	if (arity <= MAX_INLINE_ARITY) {
	  ATerm arg, args[MAX_INLINE_ARITY];
	  int i;
	  ATbool changed = ATfalse;
	  for (i=0; i<arity; i++) {
	    arg = ATgetArgument(appl, i);
	    args[i] = ATremoveAllAnnotations(arg);
	    if (args[i] != arg) {
	      changed = ATtrue;
	    }
	  }
	  if (changed) {
	    return (ATerm)ATmakeApplArray(fun, args);
	  } else {
	    return AT_removeAnnotations(t);
	  }
	} else {
	  ATermList args = ATgetArguments(appl);
	  ATermList new_args = (ATermList)ATremoveAllAnnotations((ATerm)args);
	  if (args == new_args) {
	    return AT_removeAnnotations(t);
	  }
	  return (ATerm)ATmakeApplList(fun, new_args);
	}
      }

    case AT_PLACEHOLDER:
      {
	ATermPlaceholder ph = (ATermPlaceholder)t;
	return (ATerm)ATmakePlaceholder(ATremoveAllAnnotations(ATgetPlaceholder(ph)));
      }

    default:
      ATerror("illegal term type: %d\n", ATgetType(t));
      return NULL;
  }
}
/*}}}  */

/*{{{  static int AT_compareArguments(ATermAppl t1, ATermAppl t2)  */

static int AT_compareArguments(ATermAppl t1, ATermAppl t2) 
{
  int arity1;
  int arity2;
  int i;
  ATerm arg1;
  ATerm arg2;
  int result = 0;

  arity1 = ATgetArity(ATgetAFun(t1));
  arity2 = ATgetArity(ATgetAFun(t2));
  
  
  for (i = 0; result == 0 && i < arity1 && i < arity2; i++) {
    arg1 = ATgetArgument(t1, i);
    arg2 = ATgetArgument(t2, i);
    result = ATcompare(arg1,arg2);
  }

  if (arity1 < arity2) {
    return -1;
  }
  else if (arity1 > arity2) {
    return 1;
  }

  return result;

}

/*}}}  */
/*{{{  static int AT_compareAppls(ATermAppl t0, ATermAppl t2) */

static int AT_compareAppls(ATermAppl t1, ATermAppl t2)
{
  AFun fun1;
  AFun fun2;
  char *name1;
  char *name2;
  int result;

  fun1 = ATgetAFun(t1);
  fun2 = ATgetAFun(t2);

  name1 = ATgetName(fun1);
  name2 = ATgetName(fun2);

  result = strcmp(name1,name2);
  if (result != 0) {
    return result;
  }

  return AT_compareArguments(t1,t2);

}

/*}}}  */
/*{{{  static int AT_compareInts(ATermInt t1, ATermInt t2)  */

static int AT_compareInts(ATermInt t1, ATermInt t2) 
{
  int i1;
  int i2;
  i1 = ATgetInt(t1);
  i2 = ATgetInt(t2);
  if (i1 < i2) {
    return -1;
  }
  else if (i1 > i2) {
    return 1;
  }
  return 0;
}

/*}}}  */
/*{{{  static int AT_compareReals(ATermReal t1, ATermReal t2)  */

static int AT_compareReals(ATermReal t1, ATermReal t2) 
{
  double r1;
  double r2;
  r1 = ATgetReal(t1);
  r2 = ATgetReal(t2);
  if (r1 < r2) {
    return -1;
  }
  else if (r1 > r2) {
    return 1;
  }
  return 0;
}

/*}}}  */
/*{{{  static int AT_compareLists(ATermList t1, ATermList t2)  */

static int AT_compareLists(ATermList t1, ATermList t2) 
{
  int length1;
  int length2;
  ATerm elt1;
  ATerm elt2;
  int result = 0;

  while (result == 0 && !ATisEmpty(t1) && !ATisEmpty(t2)) {
    elt1 = ATgetFirst(t1);
    elt2 = ATgetFirst(t2);
   
    result = ATcompare(elt1,elt2);

    t1 = ATgetNext(t1);
    t2 = ATgetNext(t2);
  }
  
  if (result != 0) {
    return result;
  }

  length1 = ATgetLength(t1);
  length2 = ATgetLength(t2);

  if (length1 < length2) {
    return -1;
  }
  else if (length1 > length2) {
    return 1;
  }
  return 0;
}

/*}}}  */
/*{{{  static int AT_comparePlaceholders(ATermPlaceholder t1, ATermPlaceholder t2)  */

static int AT_comparePlaceholders(ATermPlaceholder t1, ATermPlaceholder t2) 
{
  ATerm type1;
  ATerm type2;
  type1 = ATgetPlaceholder(t1);
  type2 = ATgetPlaceholder(t2);
  return ATcompare(type1,type2);
}

/*}}}  */
/*{{{  static int AT_compareBlobs(ATermBlob t1, ATermBlob t2)  */

static int AT_compareBlobs(ATermBlob t1, ATermBlob t2) 
{
  char *data1;
  char *data2;
  int size1;
  int size2;
  int result = 0;
  data1 = ATgetBlobData(t1);
  data2 = ATgetBlobData(t2);
  size1 = ATgetBlobSize(t1);
  size2 = ATgetBlobSize(t2);

  if (size1 < size2) {
    result = memcmp(data1, data2, size1);
    if (result == 0) {
      return -1;
    }
  }
  else if (size1 > size2) {
    result = memcmp(data1, data2, size2);
    if (result == 0) {
      return 1;
    }
  }
  else {
    return memcmp(data1, data2, size1);
  }

  return result;
}

/*}}}  */
/*{{{  int ATcompare(ATerm t1, ATerm t2) */

int ATcompare(ATerm t1, ATerm t2)
{
  int type1;
  int type2;
  int result = 0;

  if (ATisEqual(t1, t2)) {
    return 0;
  }

  type1 = ATgetType(t1);
  type2 = ATgetType(t2);
	
  if (type1 < type2) {
    return -1;
  }
  else if (type2 > type1) {
    return 1;
  }

  switch (type1) {
    case AT_APPL:
      result = AT_compareAppls((ATermAppl) t1, (ATermAppl) t2);
      break;
    case AT_INT:
      result = AT_compareInts((ATermInt) t1, (ATermInt) t2);
      break;
    case AT_REAL:
      result = AT_compareReals((ATermReal) t1, (ATermReal) t2);
      break;
    case AT_LIST:
      result = AT_compareLists((ATermList) t1, (ATermList) t2);
      break;
    case AT_PLACEHOLDER:
      result = AT_comparePlaceholders((ATermPlaceholder) t1, 
				    (ATermPlaceholder) t2);
      break;
    case AT_BLOB:
      result = AT_compareBlobs((ATermBlob) t1, (ATermBlob) t2);
      break;
    default:
      ATabort("Unknown ATerm type %d\n", type1);
      break;
  }

  if (result == 0) {
    ATerm annos1 = ATgetAnnotations(t1);
    ATerm annos2 = ATgetAnnotations(t2);
    if (annos1 && annos2) {
      return ATcompare(annos1,annos2);
    }
    if (annos1) {
      return 1;
    }
    if (annos2) {
      return -1;
    }
  }

  return result;
}

/*}}}  */
