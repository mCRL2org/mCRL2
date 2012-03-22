#ifndef AFUN_H
#define AFUN_H

#include <assert.h>
#include <string>
#include <cstdio>
#include <vector>
#include "mcrl2/aterm/atypes.h"
#include "mcrl2/aterm/encoding.h"


namespace aterm
{

typedef size_t AFun;

const size_t AS_INT = 0;
// const size_t AS_REAL = 1;
// const size_t AS_BLOB = 2;
// const size_t AS_PLACEHOLDER = 3;
const size_t AS_LIST = 4;
const size_t AS_EMPTY_LIST = 5;

/* The AFun type */
typedef struct _SymEntry
{
  header_type header;
  struct _SymEntry* next;
  AFun  id;
  char*   name;
  size_t count;  /* used in bafio.c */
  size_t index;  /* used in bafio.c */
}* SymEntry;

static const size_t TERM_SIZE_SYMBOL = sizeof(struct _SymEntry)/sizeof(size_t);

union _ATerm;
extern std::vector < SymEntry > at_lookup_table;

void AT_initAFun(int argc, char** argv);
size_t AT_printAFun(const AFun sym, FILE* f);

inline
bool SYM_IS_FREE(const SymEntry sym)
{
  return ((MachineWord)sym & 1) == 1;
}

inline
void AT_markAFun(const AFun s)
{
  assert(s<at_lookup_table.size());
  at_lookup_table[s]->header |= MASK_AGE_MARK;
}

inline
void AT_markAFun_young(const AFun s)
{
  assert(s<at_lookup_table.size());
  if (!IS_OLD(at_lookup_table[s]->header))
  {
    AT_markAFun(s);
  }
}

inline
void AT_unmarkAFun(const AFun s)
{
  assert(s<at_lookup_table.size());
  at_lookup_table[s]->header &= ~MASK_MARK;
}

inline
bool AT_isValidAFun(const AFun sym)
{
  return (sym != (AFun)(-1) && 
          (MachineWord)sym < at_lookup_table.size() && 
          !SYM_IS_FREE(at_lookup_table[sym]));
}

// XXX Remove
inline
bool AT_isValidAFun(const ATerm sym)
{
  return AT_isValidAFun((AFun)sym);
}

inline
bool AT_isMarkedAFun(const AFun sym)
{
  assert(sym<at_lookup_table.size());
  return IS_MARKED(at_lookup_table[sym]->header);
}

void  AT_freeAFun(SymEntry sym);
void AT_markProtectedAFuns();
void AT_markProtectedAFuns_young();

size_t AT_hashAFun(const char* name, const size_t arity);
bool AT_findAFun(const char* name, const size_t arity, const bool quoted);
void AT_unmarkAllAFuns();

std::string ATwriteAFunToString(const AFun t);

}

#endif
