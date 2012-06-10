/*{{{  includes */

#include <ctype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdexcept>
#include <set>
#include <vector>
#include <string.h>
#include <sstream>

#include "mcrl2/atermpp/detail/util.h"
#include "mcrl2/atermpp/function_symbol.h"

/*}}}  */

namespace atermpp
{

/*{{{  defines */

static const size_t INITIAL_AFUN_TABLE_CLASS = 14;

static const size_t MAGIC_PRIME = 7;

/*}}}  */
/*{{{  globals */

char afun_id[] = "$Id$";

static size_t afun_table_class = INITIAL_AFUN_TABLE_CLASS;
static size_t afun_table_size  = AT_TABLE_SIZE(INITIAL_AFUN_TABLE_CLASS);
static size_t afun_table_mask  = AT_TABLE_MASK(INITIAL_AFUN_TABLE_CLASS);

static std::vector < size_t > afun_hashtable(afun_table_size,size_t(-1));

size_t function_symbol::first_free = size_t(-1);

std::vector < _SymEntry* > function_symbol::at_lookup_table;

const function_symbol AS_UNDEFINED("<undefined>", 0);
const function_symbol AS_INT("<int>", 0);
const function_symbol AS_LIST("[_,_]", 2);
const function_symbol AS_EMPTY_LIST("[]", 0);

// static std::multiset < function_symbol > protected_symbols;


/*}}}  */

/*{{{  function declarations */

static HashNumber AT_hashAFun(const std::string &name, const size_t arity);

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char* _strdup(const char* s);
#endif

/*}}}  */

/*{{{  static void resize_table() */

static void resize_table()
{
  afun_table_class = afun_table_class+1;

  afun_table_size  = AT_TABLE_SIZE(afun_table_class);
  afun_table_mask  = AT_TABLE_MASK(afun_table_class);

  afun_hashtable.clear();
  afun_hashtable.resize(afun_table_size,size_t(-1));

  for (size_t i=0; i<function_symbol::at_lookup_table.size(); i++)
  {
    _SymEntry* entry = function_symbol::at_lookup_table[i];
    assert(entry->reference_count>0);
    
    HashNumber hnr = AT_hashAFun(entry->name, entry->arity() );
    hnr &= afun_table_mask;
    entry->next = afun_hashtable[hnr];
    afun_hashtable[hnr] = i;
  }
}

/*}}}  */

/*{{{  int AT_printAFun(function_symbol sym, FILE *f) */

/**
  * Print an afun.
  */

size_t AT_printAFun(const size_t fun, FILE* f)
{
  assert(fun<function_symbol::at_lookup_table.size());
  _SymEntry* entry = function_symbol::at_lookup_table[fun];
  std::string::const_iterator id = entry->name.begin();
  size_t size = 0;

  if (entry->is_quoted())
  {
    /* This function symbol needs quotes */
    fputc('"', f);
    size++;
    while (id!=entry->name.end())
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
    fputs(entry->name.c_str(), f);
    size += entry->name.size();
  }
  return size;
}

/*}}}  */

std::string ATwriteAFunToString(const function_symbol &fun)
{
  std::ostringstream oss;
  assert(fun.number()<function_symbol::at_lookup_table.size());
  _SymEntry* entry = function_symbol::at_lookup_table[fun.number()];
  std::string::const_iterator id = entry->name.begin();

  if (entry->is_quoted())
  {
    /* This function symbol needs quotes */
    oss << "\"";
    while (id!=entry->name.end())
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
    oss << entry->name;
  }

  return oss.str();
}


/**
 * Calculate the hash value of a symbol.
 */

static HashNumber AT_hashAFun(const std::string &name, const size_t arity)
{
  HashNumber hnr = arity*3;

  for (std::string::const_iterator i=name.begin(); i!=name.end(); i++)
  {
    hnr = 251 * hnr + *i;
  }

  return hnr*MAGIC_PRIME;
}


/*}}}  */

/*{{{  function_symbol ATmakeAFun(const char *name, int arity, ATbool quoted) */

function_symbol::function_symbol(const std::string &name, const size_t arity, const bool quoted)
{
  const HashNumber hnr = AT_hashAFun(name, arity) & afun_table_mask;

  /* Find symbol in table */
  size_t cur = afun_hashtable[hnr];
  while (cur!=size_t(-1) && !(at_lookup_table[cur]->arity()==arity &&
                              at_lookup_table[cur]->is_quoted()==quoted &&
                              at_lookup_table[cur]->name==name))
  {
    cur = at_lookup_table[cur]->next;
  }

  if (cur == size_t(-1))
  {
    const size_t free_entry = first_free;
    assert(at_lookup_table.size()<afun_table_size);

    if (free_entry!=size_t(-1)) // There is a free place in at_lookup_table to store an function_symbol.
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


    at_lookup_table[cur]->name = name;

    at_lookup_table[cur]->next = afun_hashtable[hnr];
    afun_hashtable[hnr] = cur;
  }

  m_number=cur;
  increase_reference_count<false>(m_number);

  if (at_lookup_table.size()>=afun_table_size) 
  {
    resize_table();
  }
}

/*}}}  */
/*{{{  void AT_freeAFun(SymEntry sym) */

/**
 * Free a symbol
 */

void at_free_afun(const size_t n)
{
  _SymEntry* sym=function_symbol::at_lookup_table[n];
  
  assert(!sym->name.empty());
  assert(sym->id==n);

  /* Calculate hashnumber */
  const HashNumber hnr = AT_hashAFun(sym->name, sym->arity()) & afun_table_mask;

  /* Update hashtable */
  if (afun_hashtable[hnr] == n)
  {
    afun_hashtable[hnr] = sym->next;
  }
  else
  {
    size_t cur;
    size_t prev = afun_hashtable[hnr];
    for (cur = function_symbol::at_lookup_table[prev]->next; cur != n; prev = cur, cur = function_symbol::at_lookup_table[cur]->next)
    {
      assert(cur != size_t(-1));
    }
    function_symbol::at_lookup_table[prev]->next = function_symbol::at_lookup_table[cur]->next;
  }

  assert(n<function_symbol::at_lookup_table.size());
  function_symbol::at_lookup_table[n]->id = function_symbol::first_free;
  function_symbol::first_free = n;
}

/*}}}  */

} // namespace atermpp
