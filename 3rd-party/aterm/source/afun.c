/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include "aterm2.h"
#include "memory.h"
#include "util.h"

/*}}}  */
/*{{{  defines */

#define INITIAL_AFUN_TABLE_CLASS 14

#define SYMBOL_HASH_SIZE	65353	/* nextprime(65335) */
#define SYMBOL_HASH_OPT		"-at-symboltable"
#define AFUN_TABLE_OPT		"-at-afuntable"

#define SHIFT_INDEX 1
/* Keep the sign of sym below; Therefore ptrdiff_t is used, instead of size_t. This goes wrong when
 * the number of symbols approaches the size of the machineword/4, but this is unlikely.  */
#define SYM_GET_NEXT_FREE(sym)    ((ptrdiff_t)(sym) >> SHIFT_INDEX)
#define SYM_SET_NEXT_FREE(next)   (1 | ((next) << SHIFT_INDEX))

#define INITIAL_PROTECTED_SYMBOLS   1024
#define SYM_PROTECT_EXPAND_SIZE     1024

#define MAGIC_PRIME 7

/*}}}  */
/*{{{  globals */

char afun_id[] = "$Id: afun.c 23071 2007-07-02 10:06:17Z eriks $";

static size_t table_class = INITIAL_AFUN_TABLE_CLASS;
static MachineWord table_size  = AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS);
static size_t table_mask  = AT_TABLE_MASK(INITIAL_AFUN_TABLE_CLASS);

static SymEntry *hash_table     = NULL;

static AFun first_free = NON_EXISTING;

static AFun *protected_symbols = NULL;
static size_t nr_protected_symbols  = 0;
static size_t max_protected_symbols  = 0;

/* Efficiency hack: was static */
SymEntry *at_lookup_table = NULL;
ATerm    *at_lookup_table_alias = NULL;

/*}}}  */
/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *_strdup(const char *s);
#endif

/*}}}  */

/*{{{  static void resize_table() */

static void resize_table()
{
  MachineWord i;
  size_t new_class = table_class+1;
  MachineWord new_size  = AT_TABLE_SIZE(new_class);
  size_t new_mask  = AT_TABLE_MASK(new_class);

  at_lookup_table = (SymEntry *)AT_realloc(at_lookup_table, new_size*sizeof(SymEntry));
  at_lookup_table_alias = (ATerm *)at_lookup_table;
  if (!at_lookup_table) {
    ATerror("afun.c:resize_table - could not allocate space for lookup table of %ld afuns\n", new_size);
  }
  for (i = table_size; i < new_size; i++) 
  {
    at_lookup_table[i] = (SymEntry) SYM_SET_NEXT_FREE(first_free);
    first_free = i;
  }

  hash_table = (SymEntry *)AT_realloc(hash_table, new_size*sizeof(SymEntry));
  if (!hash_table) {
    ATerror("afun.c:resize_table - could not allocate space for hashtable of %ld afuns\n", new_size);
  }
  memset(hash_table, 0, new_size*sizeof(SymEntry));

  for (i=0; i<table_size; i++) {
    SymEntry entry = at_lookup_table[i];
    if (!SYM_IS_FREE(entry)) {
      ShortHashNumber hnr = AT_hashAFun(entry->name, GET_LENGTH(entry->header));
      hnr &= new_mask;
      entry->next = hash_table[hnr];
      hash_table[hnr] = entry;
    }
  }

  table_class = new_class;
  table_size  = new_size;
  table_mask  = new_mask;
}

/*}}}  */

/*{{{  size_t AT_symbolTableSize() */

MachineWord AT_symbolTableSize()
{
  return table_size;
}

/*}}}  */

/*{{{  void AT_initAFun(int argc, char *argv[]) */
void AT_initAFun(int argc, char *argv[])
{
  size_t i;
  AFun sym;

  for (i = 1; i < (size_t)argc; i++) {
    if (streq(argv[i], SYMBOL_HASH_OPT)) {
      ATerror("Option %s is deprecated, use %s instead!\n"
	      "Note that %s uses 2^<arg> as the actual table size.\n",
	      SYMBOL_HASH_OPT, AFUN_TABLE_OPT, AFUN_TABLE_OPT);
    } else if (streq(argv[i], AFUN_TABLE_OPT)) {
      table_class = atoi(argv[++i]);
      table_size  = AT_TABLE_SIZE(table_class);
      table_mask  = AT_TABLE_MASK(table_class);
    } else if(streq(argv[i], "-at-help")) {
      fprintf(stderr, "    %-20s: initial afun table class " 
	      "(default=%lu)\n",	AFUN_TABLE_OPT " <class>", table_class);
    }
  }

  hash_table = (SymEntry *) AT_calloc(table_size, sizeof(SymEntry));
  if (hash_table == NULL) {
    ATerror("AT_initAFun: cannot allocate %ld hash-entries.\n",
	    table_size);
  }

  at_lookup_table = (SymEntry *) AT_calloc(table_size, sizeof(SymEntry));
  at_lookup_table_alias = (ATerm *)at_lookup_table;
  if (at_lookup_table == NULL) {
    ATerror("AT_initAFun: cannot allocate %ld lookup-entries.\n",
	    table_size);
  }
  
  first_free = 0;
  for (sym = 0; sym < table_size; sym++) {
    at_lookup_table[sym] = (SymEntry) SYM_SET_NEXT_FREE(sym+1);
  }
  at_lookup_table[table_size-1] = (SymEntry) SYM_SET_NEXT_FREE((MachineWord)(-1));		/* Sentinel */

  protected_symbols = (AFun *)AT_calloc(INITIAL_PROTECTED_SYMBOLS, 
				       sizeof(AFun));
  if(!protected_symbols) {
    ATerror("AT_initAFun: cannot allocate initial protection buffer.\n");
  }
	
  sym = ATmakeAFun("<int>", 0, ATfalse);
  assert(sym == AS_INT);
  ATprotectAFun(sym);

  /* Can't remove real and blob below, as the symbols
     for PLACE_HOLDERS have predetermined values.... They are not
     used anymore. */
  sym = ATmakeAFun("<real>", 0, ATfalse);
  ATprotectAFun(sym);

  sym = ATmakeAFun("<blob>", 0, ATfalse);
  ATprotectAFun(sym); 

  sym = ATmakeAFun("<_>", 1, ATfalse);

  ATprotectAFun(sym);

  sym = ATmakeAFun("[_,_]", 2, ATfalse);
  assert(sym == AS_LIST);
  ATprotectAFun(sym);

  sym = ATmakeAFun("[]", 0, ATfalse);
  assert(sym == AS_EMPTY_LIST);
  ATprotectAFun(sym);

  sym = ATmakeAFun("{_}", 2, ATfalse);
  ATprotectAFun(sym);
}
/*}}}  */

/*{{{  int AT_printAFun(AFun sym, FILE *f) */

/**
  * Print an afun.
  */

size_t AT_printAFun(AFun fun, FILE *f)
{
  SymEntry entry = at_lookup_table[fun];
  char *id = entry->name;
  size_t size = 0;

  if (IS_QUOTED(entry->header)) {
    /* This function symbol needs quotes */
    fputc('"', f);
    size++;
    while(*id) {
      /* We need to escape special characters */
      switch(*id) {
      case '\\':
      case '"':
	fputc('\\', f);
	fputc(*id, f);
	size += 2;
	break;
      case '\n':
	fputc('\\', f);
	fputc('n', f);
	size += 2;
	break;
      case '\t':
	fputc('\\', f);
	fputc('t', f);
	size += 2;
	break;
      case '\r':
	fputc('\\', f);
	fputc('r', f);
	size += 2;
	break;
      default:
	fputc(*id, f);
	size++;
      }
      id++;
    }
    fputc('"', f);
    size++;
  } else {
    fputs(id, f);
    size += strlen(id);
  }
  return size;
}

/*}}}  */
/*{{{  int AT_writeAFun(AFun fun, byte_writer *writer) */

/**
  * Print an afun.
  */

/* size_t AT_writeAFun(AFun fun, byte_writer *writer)
{
  SymEntry entry = at_lookup_table[fun];
  char *id = entry->name;
  size_t size = 0;

  if (IS_QUOTED(entry->header)) {
    / * This function symbol needs quotes * /
    write_byte('"', writer);
    size++;
    while(*id) {
      / * We need to escape special characters * /
      switch(*id) {
      case '\\':
      case '"':
	write_byte('\\', writer);
	write_byte(*id, writer);
	size += 2;
	break;
      case '\n':
	write_byte('\\', writer);
	write_byte('n', writer);
	size += 2;
	break;
      case '\t':
	write_byte('\\', writer);
	write_byte('t', writer);
	size += 2;
	break;
      case '\r':
	write_byte('\\', writer);
	write_byte('r', writer);
	size += 2;
	break;
      default:
	write_byte(*id, writer);
	size++;
      }
      id++;
    }
    write_byte('"', writer);
    size++;
  } else {
    size += write_bytes(id, strlen(id), writer);
  }
  return size;
} */

/*}}}  */

/*{{{  ShortHashNumber AT_hashAFun(const char *name, int arity) */

/**
 * Calculate the hash value of a symbol.
 */

ShortHashNumber AT_hashAFun(const char *name, size_t arity)
{
  ShortHashNumber hnr;
  const char *walk = name;

  for(hnr = arity*3; *walk; walk++)
    hnr = 251 * hnr + *walk;
  
  return hnr*MAGIC_PRIME;
}


/*}}}  */

/*{{{  AFun ATmakeAFun(const char *name, int arity, ATbool quoted) */

AFun ATmakeAFun(const char *name, size_t arity, ATbool quoted)
{
  header_type header = SYMBOL_HEADER(arity, quoted);
  ShortHashNumber hnr = AT_hashAFun(name, arity) & table_mask;
  SymEntry cur;

  if(arity >= MAX_ARITY) {
    ATabort("cannot handle symbols with arity %d (max=%d)\n",
	    arity, MAX_ARITY-1);
  }

  /* Find symbol in table */
  cur = hash_table[hnr];
  while (cur && (!EQUAL_HEADER(cur->header,header) || !streq(cur->name, name))) {
    cur = cur->next;
  }
  
  if (cur == NULL) {
    AFun free_entry;

    free_entry = first_free;
    if (free_entry == (AFun)(-1)) 
    {
      resize_table();

      /* Hashtable size changed, so recalculate hashnumber */
      hnr = AT_hashAFun(name, arity) & table_mask;
     
      free_entry = first_free;
      if (free_entry == (AFun)(-1)) 
      {
	ATerror("AT_initAFun: out of symbol slots!\n");
      }
    }
/* fprintf(stderr,"First_free %d %lu\n",SIZEOF_LONG,first_free); */
    first_free = SYM_GET_NEXT_FREE(at_lookup_table[first_free]);

    cur = (SymEntry) AT_allocate(TERM_SIZE_SYMBOL);
    at_lookup_table[free_entry] = cur;

    cur->header = header;
    cur->id = free_entry;
    cur->count = 0;
    cur->index = -1;

    cur->name = _strdup(name);
    if (cur->name == NULL) {
      ATerror("ATmakeAFun: no room for name of length %d\n", strlen(name));
    }

    cur->next = hash_table[hnr];
    hash_table[hnr] = cur;
  }

  /*ATwarning("AT_makeAFun(%p)\tid = %d\n", cur, cur->id);*/
  
  return cur->id;
}

/*}}}  */
/*{{{  void AT_freeAFun(SymEntry sym) */

/**
 * Free a symbol
 */

void AT_freeAFun(SymEntry sym)
{
  ShortHashNumber hnr;

  terminfo[TERM_SIZE_SYMBOL].nb_reclaimed_cells_during_last_gc++;
  
  assert(sym->name);

  /*ATwarning("AT_freeAFun: name: [%s], addr: %p, id: %d\n", sym->name, sym, sym->id);*/
  
  /* Calculate hashnumber */
  hnr = AT_hashAFun(sym->name, GET_LENGTH(sym->header));
  hnr &= table_mask;
  
  /* Update hashtable */
  if (hash_table[hnr] == sym) {
    hash_table[hnr] = sym->next;
  } else {
    SymEntry cur, prev;
    prev = hash_table[hnr]; 
    for (cur = prev->next; cur != sym; prev = cur, cur = cur->next) {
      assert(cur != NULL);
    }
    prev->next = cur->next;
  }
  
  /* Free symbol name */
  AT_free(sym->name);
  sym->name = NULL;
  
  at_lookup_table[sym->id] = (SymEntry)SYM_SET_NEXT_FREE(first_free);
  first_free = sym->id;
}

/*}}}  */
/*{{{  ATbool AT_findAFun(char *name, int arity, ATbool quoted) */

/**
 * Check for the existence of a symbol
 */

/* ATbool AT_findAFun(char *name, size_t arity, ATbool quoted)
{
  header_type header = SYMBOL_HEADER(arity, quoted);
  ShortHashNumber hnr = AT_hashAFun(name, arity) & table_mask;
  SymEntry cur;
  
  if(arity >= MAX_ARITY)
    ATabort("cannot handle symbols with arity %d (max=%d)\n",
	    arity, MAX_ARITY);

  / * Find symbol in table * /
  cur = hash_table[hnr];
  while (cur && (!EQUAL_HEADER(cur->header,header) || !streq(cur->name, name)))
    cur = cur->next;
  
  return (cur == NULL) ? ATfalse : ATtrue;
} */

/*}}}  */

/*{{{  void ATprotectAFun(AFun sym) */

/**
	* Protect a symbol.
	*/

void ATprotectAFun(AFun sym)
{

  if(nr_protected_symbols >= max_protected_symbols) {
    max_protected_symbols += SYM_PROTECT_EXPAND_SIZE;
    protected_symbols = (AFun *)AT_realloc(protected_symbols,
					  max_protected_symbols * sizeof(AFun));
    if(!protected_symbols)
      ATerror("ATprotectAFun: no space to hold %ld protected symbols.\n",
	      max_protected_symbols);
  }

  protected_symbols[nr_protected_symbols++] = sym;
}

/*}}}  */
/*{{{  void ATunprotectAFun(AFun sym) */

/**
	* Unprotect a symbol.
	*/

void ATunprotectAFun(AFun sym)
{
  /* It is essential for performance that in this file
   * the protected_symbols array is traversed from back
   * to front. This function is only invoked by 
   * ATdestroyBinaryReader, which stacks symbols at the 
   * end of protected symbols, and removes them in 
   * reverse order. */

  size_t lcv;

  for(lcv = nr_protected_symbols; lcv >0 ; ) 
  { 
    --lcv;
    if(protected_symbols[lcv] == sym) {
      protected_symbols[lcv] = protected_symbols[--nr_protected_symbols];
      protected_symbols[nr_protected_symbols] = -1;
      break;
    }
  }
}

/*}}}  */
/*{{{  void AT_markProtectedAFuns() */

/**
 * Mark all symbols that were protected previously using ATprotectAFun.
 */

void AT_markProtectedAFuns()
{
  size_t lcv;
  for(lcv = 0; lcv < nr_protected_symbols; lcv++) {
    SET_MARK(((ATerm)at_lookup_table[protected_symbols[lcv]])->header);
  }
}

/* TODO: Optimisation (Old+Mark in one step)*/
void AT_markProtectedAFuns_young() {
  size_t lcv;

  for(lcv = 0; lcv < nr_protected_symbols; lcv++) {
    if(!IS_OLD(((ATerm)at_lookup_table[protected_symbols[lcv]])->header)) {
      SET_MARK(((ATerm)at_lookup_table[protected_symbols[lcv]])->header);
    }
  }
}

/*}}}  */
/*{{{  void AT_unmarkAllAFuns() */

/* void AT_unmarkAllAFuns()
{
  AFun s;

  for (s=0; s<table_size; s++) {
    if (AT_isValidAFun((AFun)s)) {
      AT_unmarkAFun(s);
    }
  }
} */

/*}}}  */

