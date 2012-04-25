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

// static const size_t SHIFT_INDEX = 1;
/* Keep the sign of sym below; Therefore ptrdiff_t is used, instead of size_t. This goes wrong when
 * the number of symbols approaches the size of the machineword/4, but this is unlikely.  */
/* inline
ptrdiff_t SYM_GET_NEXT_FREE(const SymEntry sym)
{
  return (ptrdiff_t)(sym) >> SHIFT_INDEX;
}

inline
size_t SYM_SET_NEXT_FREE(const AFun next)
{
  return 1 | ((next) << SHIFT_INDEX);
}
*/

static const size_t MAGIC_PRIME = 7;

/*}}}  */
/*{{{  globals */

char afun_id[] = "$Id$";

static size_t afun_table_class = INITIAL_AFUN_TABLE_CLASS;
static MachineWord afun_table_size  = AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS);
static size_t afun_table_mask  = AT_TABLE_MASK(INITIAL_AFUN_TABLE_CLASS);

static std::vector < size_t > afun_hashtable(afun_table_size,size_t(-1));

size_t AFun::first_free = size_t(-1);

std::vector < _SymEntry* > AFun::at_lookup_table;

AFun AS_INT;
AFun AS_LIST;
AFun AS_EMPTY_LIST;


// static std::multiset < AFun > protected_symbols;


/*}}}  */

/*{{{  function declarations */

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char* _strdup(const char* s);
#endif

/*}}}  */

/*{{{  static void resize_table() */

static void resize_table()
{
  afun_table_class = afun_table_class+1;
fprintf(stderr,"resize_afun_hashtable to class %ld\n",afun_table_class);
#ifdef AT_32BIT
  if (afun_table_class>=23) // In 32 bit mode only 22 bits are reserved for function symbols.
  {
    throw std::runtime_error("afun.c:resize_table - cannot allocate space for more than 2^22 (= 4.194.304) different afuns on a 32 bit machine.");
  }
#endif
#ifdef AT_64BIT
  if (afun_table_class>=31)
  {
    throw std::runtime_error("afun.c:resize_table - cannot allocate space for more than 2^30 (= 1.073.741.824) different afuns on a 64 bit machine.");
  }
#endif

  afun_table_size  = AT_TABLE_SIZE(afun_table_class);
  afun_table_mask  = AT_TABLE_MASK(afun_table_class);

  afun_hashtable.clear();
  afun_hashtable.resize(afun_table_size,size_t(-1));
  // afun_hashtable = (_SymEntry**)AT_realloc(afun_hashtable, new_size*sizeof(_SymEntry*));
  /* if (!afun_hashtable)
  {
    throw std::runtime_error("afun.c:resize_table - could not allocate space for hashtable of " + to_string(new_size) + " afuns");
  }
  memset(afun_hashtable, 0, new_size*sizeof(_SymEntry*)); */

  for (size_t i=0; i<AFun::at_lookup_table.size(); i++)
  {
    _SymEntry* entry = AFun::at_lookup_table[i];
    if (entry->reference_count>0)
    {
      ShortHashNumber hnr = AT_hashAFun(entry->name, entry->arity() );
      hnr &= afun_table_mask;
      entry->next = afun_hashtable[hnr];
      afun_hashtable[hnr] = i;
    }
  }
}

/*}}}  */

/*{{{  int AT_printAFun(AFun sym, FILE *f) */

/**
  * Print an afun.
  */

size_t AT_printAFun(const size_t fun, FILE* f)
{
  assert(fun<AFun::at_lookup_table.size());
  _SymEntry* entry = AFun::at_lookup_table[fun];
  char* id = entry->name;
  size_t size = 0;

  if (entry->is_quoted())
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

std::string ATwriteAFunToString(const AFun &fun)
{
  std::ostringstream oss;
  assert(fun.number()<AFun::at_lookup_table.size());
  _SymEntry* entry = AFun::at_lookup_table[fun.number()];
  char* id = entry->name;

  if (entry->is_quoted())
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

AFun::AFun(const char* name, const size_t arity, const bool quoted)
{
  const ShortHashNumber hnr = AT_hashAFun(name, arity) & afun_table_mask;

  if (arity >= MAX_ARITY)
  {
    throw std::runtime_error("cannot handle symbols with arity " + to_string(arity) + "(max=" + to_string(MAX_ARITY-1) + ")");
  }

  /* Find symbol in table */
  size_t cur = afun_hashtable[hnr];
  while (cur!=size_t(-1) && !(at_lookup_table[cur]->arity()==arity &&
                              at_lookup_table[cur]->is_quoted()==quoted &&
                              streq(at_lookup_table[cur]->name, name)))
  {
    cur = at_lookup_table[cur]->next;
  }

  if (cur == size_t(-1))
  {
    const size_t free_entry = first_free;
    assert(at_lookup_table.size()<afun_table_size);
    assert(at_lookup_table.size()<afun_table_size); // There is a free places in the hash table.

    if (free_entry!=size_t(-1)) // There is a free place in at_lookup_table to store an AFun.
    { 
      assert(first_free<at_lookup_table.size());
      cur=first_free;
      first_free = (size_t)at_lookup_table[first_free]->id;
      assert(first_free==size_t(-1) || first_free<at_lookup_table.size());
      assert(free_entry<at_lookup_table.size());
      at_lookup_table[cur]->id = cur;
      assert(at_lookup_table[cur]->reference_count==0);
      at_lookup_table[cur]->reference_count=0;
      at_lookup_table[cur]->header = _SymEntry::make_header(arity,quoted);
      at_lookup_table[cur]->count = 0;
      at_lookup_table[cur]->index = -1;
    }
    else 
    { 
      cur = at_lookup_table.size();
      at_lookup_table.push_back(new _SymEntry(arity,quoted,cur,0,size_t(-1)));
    }


    at_lookup_table[cur]->name = _strdup(name);
    if (at_lookup_table[cur]->name == NULL)
    {
      throw std::runtime_error("Construct AFun: no room for name of length " + to_string(strlen(name)));
    }

    at_lookup_table[cur]->next = afun_hashtable[hnr];
    afun_hashtable[hnr] = cur;
  }

  at_lookup_table[cur]->reference_count++;
  if (at_lookup_table.size()>=afun_table_size) 
  {
    resize_table();
  }
  m_number=cur;
}

/*}}}  */
/*{{{  void AT_freeAFun(SymEntry sym) */

/**
 * Free a symbol
 */

void at_free_afun(const size_t n)
{
  _SymEntry* sym=AFun::at_lookup_table[n];
  
  assert(sym->name);
  assert(sym->id==n);

  /* Calculate hashnumber */
  const ShortHashNumber hnr = AT_hashAFun(sym->name, sym->arity()) & afun_table_mask;

  /* Update hashtable */
  if (afun_hashtable[hnr] == n)
  {
    afun_hashtable[hnr] = sym->next;
  }
  else
  {
    size_t cur;
    size_t prev = afun_hashtable[hnr];
    for (cur = AFun::at_lookup_table[prev]->next; cur != n; prev = cur, cur = AFun::at_lookup_table[cur]->next)
    {
      assert(cur != size_t(-1));
    }
    AFun::at_lookup_table[prev]->next = AFun::at_lookup_table[cur]->next;
  }

  /* Free symbol name */
  free(sym->name);
  sym->name = NULL;

  assert(n<AFun::at_lookup_table.size());
  AFun::at_lookup_table[n]->id = AFun::first_free;
  AFun::first_free = n;
}

/*}}}  */

/*{{{  void ATprotectAFun(AFun sym) */

/**
  * Protect a symbol.
  */

void ATprotectAFun(const AFun &)
{
  // protected_symbols.insert(sym);
}

/*}}}  */
/*{{{  void ATunprotectAFun(AFun sym) */

/**
  * Unprotect a symbol.
  */

void ATunprotectAFun(const AFun &)
{
  // Remove only one occurrence of sym: erase cannot be used.
/*  const std::multiset < AFun >::const_iterator i=protected_symbols.find(sym);
  if (i!=protected_symbols.end())
  {
    protected_symbols.erase(i);
  }
  else assert(0); // A non protected symbol is being unprotected.
*/
}

/*}}}  */
/*{{{  void AT_markProtectedAFuns() */

/**
 * Mark all symbols that were protected previously using ATprotectAFun.
 */

void AT_markProtectedAFuns()
{
  fprintf(stderr,"NO PROTECTED AFUNS ARE EXPLLICITLY MARKED ANYMORE\n");

  /* for(std::multiset < AFun >::const_iterator i=protected_symbols.begin(); i!=protected_symbols.end(); ++i)
  {
    assert(at_lookup_table.size()> i->number());
    SET_MARK(at_lookup_table[*i]->header);
  } */
}

/*}}}  */

} // namespace aterm
