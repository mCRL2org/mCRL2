/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>
#include <set>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/memory.h"
#include "mcrl2/aterm/util.h"

/*}}}  */

namespace aterm
{

/*{{{  defines */

static const size_t INITIAL_AFUN_TABLE_CLASS = 14;
static const size_t SYMBOL_HASH_SIZE = 65353; /* nextprime(65335) */

static const size_t SHIFT_INDEX = 1;
/* Keep the sign of sym below; Therefore ptrdiff_t is used, instead of size_t. This goes wrong when
 * the number of symbols approaches the size of the machineword/4, but this is unlikely.  */
inline
ptrdiff_t SYM_GET_NEXT_FREE(const SymEntry sym)
{
  return (ptrdiff_t)(sym) >> SHIFT_INDEX;
}

inline
size_t SYM_SET_NEXT_FREE(const AFun next)
{
  return 1 | ((next) << SHIFT_INDEX);
}

static const size_t MAGIC_PRIME = 7;

/*}}}  */
/*{{{  globals */

char afun_id[] = "$Id$";

static size_t afun_table_class = INITIAL_AFUN_TABLE_CLASS;
static MachineWord afun_table_size  = AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS);
static size_t afun_table_mask  = AT_TABLE_MASK(INITIAL_AFUN_TABLE_CLASS);

static SymEntry* hash_table     = NULL;

static AFun first_free = 0;

static std::multiset < AFun > protected_symbols;

// SymEntry* at_lookup_table = NULL;
std::vector < SymEntry > at_lookup_table;

/*}}}  */

/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char* _strdup(const char* s);
#endif

/*}}}  */

/*{{{  static void resize_table() */

static void resize_table()
{
  MachineWord i;
  size_t new_class = afun_table_class+1;
#ifdef AT_32BIT
  if (new_class>=23) // In 32 bit mode only 22 bits are reserved for function symbols.
  {
    throw std::runtime_error("afun.c:resize_table - cannot allocate space for more than 2^22 (= 4.194.304) different afuns on a 32 bit machine.");
  }
#endif
#ifdef AT_64BIT
  if (new_class>=31)
  {
    throw std::runtime_error("afun.c:resize_table - cannot allocate space for more than 2^30 (= 1.073.741.824) different afuns on a 64 bit machine.");
  }
#endif

  MachineWord new_size  = AT_TABLE_SIZE(new_class);
  size_t new_mask  = AT_TABLE_MASK(new_class);

  hash_table = (SymEntry*)AT_realloc(hash_table, new_size*sizeof(SymEntry));
  if (!hash_table)
  {
    throw std::runtime_error("afun.c:resize_table - could not allocate space for hashtable of " + to_string(new_size) + " afuns");
  }
  memset(hash_table, 0, new_size*sizeof(SymEntry));

  for (i=0; i<at_lookup_table.size(); i++)
  {
    SymEntry entry = at_lookup_table[i];
    if (!SYM_IS_FREE(entry))
    {
      ShortHashNumber hnr = AT_hashAFun(entry->name, GET_LENGTH(entry->header));
      hnr &= new_mask;
      entry->next = hash_table[hnr];
      hash_table[hnr] = entry;
    }
  }

  afun_table_class = new_class;
  afun_table_size  = new_size;
  afun_table_mask  = new_mask;
}

/*}}}  */

/*{{{  void AT_initAFun(int argc, char *argv[]) */
void AT_initAFun(int, char**)
{
  AFun sym;

  hash_table = (SymEntry*) AT_calloc(afun_table_size, sizeof(SymEntry));
  if (hash_table == NULL)
  {
    throw std::runtime_error("AT_initAFun: cannot allocate " + to_string(afun_table_size) + " hash-entries.");
  }

  first_free = 0;

  sym = ATmakeAFun("<int>", 0, false);
  assert(sym == AS_INT);
  ATprotectAFun(sym);

  /* Can't remove real and blob below, as the symbols
     for PLACE_HOLDERS have predetermined values.... They are not
     used anymore. */
  sym = ATmakeAFun("<real>", 0, false);
  ATprotectAFun(sym);

  sym = ATmakeAFun("<blob>", 0, false);
  ATprotectAFun(sym);

  sym = ATmakeAFun("<_>", 1, false);

  ATprotectAFun(sym);

  sym = ATmakeAFun("[_,_]", 2, false);
  assert(sym == AS_LIST);
  ATprotectAFun(sym);

  sym = ATmakeAFun("[]", 0, false);
  assert(sym == AS_EMPTY_LIST);
  ATprotectAFun(sym);

  sym = ATmakeAFun("{_}", 2, false);
  ATprotectAFun(sym);
}
/*}}}  */

/*{{{  int AT_printAFun(AFun sym, FILE *f) */

/**
  * Print an afun.
  */

size_t AT_printAFun(const AFun fun, FILE* f)
{
  assert(fun<at_lookup_table.size());
  SymEntry entry = at_lookup_table[fun];
  char* id = entry->name;
  size_t size = 0;

  if (IS_QUOTED(entry->header))
  {
    /* This function symbol needs quotes */
    fputc('"', f);
    size++;
    while (*id)
    {
      /* We need to escape special characters */
      switch (*id)
      {
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
          break;
      }
      id++;
    }
    fputc('"', f);
    size++;
  }
  else
  {
    fputs(id, f);
    size += strlen(id);
  }
  return size;
}

/*}}}  */

std::string ATwriteAFunToString(const AFun fun)
{
  std::ostringstream oss;
  assert(fun<at_lookup_table.size());
  SymEntry entry = at_lookup_table[fun];
  char* id = entry->name;

  if (IS_QUOTED(entry->header))
  {
    /* This function symbol needs quotes */
    oss << "\"";
    while (*id)
    {
      /* We need to escape special characters */
      switch (*id)
      {
        case '\\':
        case '"':
          oss << "\\" << *id;
          break;
        case '\n':
          oss << "\\n";
          break;
        case '\t':
          oss << "\\t";
          break;
        case '\r':
          oss << "\\r";
          break;
        default:
          oss << *id;
          break;
      }
      ++id;
    }
    oss << "\"";
  }
  else
  {
    oss << std::string(id);
  }

  return oss.str();
}

/*{{{  ShortHashNumber AT_hashAFun(const char *name, int arity) */

/**
 * Calculate the hash value of a symbol.
 */

ShortHashNumber AT_hashAFun(const char* name, const size_t arity)
{
  ShortHashNumber hnr;
  const char* walk = name;

  for (hnr = arity*3; *walk; walk++)
  {
    hnr = 251 * hnr + *walk;
  }

  return hnr*MAGIC_PRIME;
}


/*}}}  */

/*{{{  AFun ATmakeAFun(const char *name, int arity, ATbool quoted) */

AFun ATmakeAFun(const char* name, const size_t arity, const bool quoted)
{
  header_type header = SYMBOL_HEADER(arity, quoted);
  ShortHashNumber hnr = AT_hashAFun(name, arity) & afun_table_mask;
  SymEntry cur;

  if (arity >= MAX_ARITY)
  {
    throw std::runtime_error("cannot handle symbols with arity " + to_string(arity) + "(max=" + to_string(MAX_ARITY-1) + ")");
  }

  /* Find symbol in table */
  cur = hash_table[hnr];
  while (cur && (!EQUAL_HEADER(cur->header,header) || !streq(cur->name, name)))
  {
    cur = cur->next;
  }

  if (cur == NULL)
  {
    AFun free_entry = first_free;
    assert(at_lookup_table.size()<afun_table_size); // There is a free places in the hash table.

    cur = (SymEntry) AT_allocate(TERM_SIZE_SYMBOL);

    assert(free_entry<=at_lookup_table.size());
    if (free_entry<at_lookup_table.size())
    { 
      first_free = SYM_GET_NEXT_FREE(at_lookup_table[first_free]);
      at_lookup_table[free_entry] = cur;
    }
    else 
    { 
      at_lookup_table.push_back(cur);
      first_free=at_lookup_table.size();
    }

    cur->header = header;
    cur->id = free_entry;
    cur->count = 0;
    cur->index = -1;

    cur->name = _strdup(name);
    if (cur->name == NULL)
    {
      throw std::runtime_error("ATmakeAFun: no room for name of length " + to_string(strlen(name)));
    }

    cur->next = hash_table[hnr];
    hash_table[hnr] = cur;
  }

  if (at_lookup_table.size()>=afun_table_size*0.7) // Resize when more than 70% of the spots in the hash table are filled.
  {
    resize_table();
  }
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

  /* Calculate hashnumber */
  hnr = AT_hashAFun(sym->name, GET_LENGTH(sym->header));
  hnr &= afun_table_mask;

  /* Update hashtable */
  if (hash_table[hnr] == sym)
  {
    hash_table[hnr] = sym->next;
  }
  else
  {
    SymEntry cur, prev;
    prev = hash_table[hnr];
    for (cur = prev->next; cur != sym; prev = cur, cur = cur->next)
    {
      assert(cur != NULL);
    }
    prev->next = cur->next;
  }

  /* Free symbol name */
  AT_free(sym->name);
  sym->name = NULL;

  assert(sym->id<at_lookup_table.size());
  at_lookup_table[sym->id] = (SymEntry)SYM_SET_NEXT_FREE(first_free);
  first_free = sym->id;
}

/*}}}  */

/*{{{  void ATprotectAFun(AFun sym) */

/**
  * Protect a symbol.
  */

void ATprotectAFun(const AFun sym)
{
  protected_symbols.insert(sym);
}

/*}}}  */
/*{{{  void ATunprotectAFun(AFun sym) */

/**
  * Unprotect a symbol.
  */

void ATunprotectAFun(const AFun sym)
{
  // Remove only one occurrence of sym: erase cannot be used.
  const std::multiset < AFun >::const_iterator i=protected_symbols.find(sym);
  if (i!=protected_symbols.end())
  {
    protected_symbols.erase(i);
  }
  else assert(0); // A non protected symbol is being unprotected.
}

/*}}}  */
/*{{{  void AT_markProtectedAFuns() */

/**
 * Mark all symbols that were protected previously using ATprotectAFun.
 */

void AT_markProtectedAFuns(const bool only_mark_young)
{
  for(std::multiset < AFun >::const_iterator i=protected_symbols.begin(); i!=protected_symbols.end(); ++i)
  {
    assert(at_lookup_table.size()> *i);
    if (!(only_mark_young && IS_OLD(((ATerm)at_lookup_table[*i])->header)))
    {
      SET_MARK(((ATerm)at_lookup_table[*i])->header);
    }
  }
}

/*}}}  */

} // namespace aterm
