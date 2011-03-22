#ifndef AFUN_H
#define AFUN_H

#include "atypes.h"
#include "encoding.h"
#include "stdio.h"

#define AS_INT          0
#define AS_REAL         1
#define AS_BLOB         2
#define AS_PLACEHOLDER  3
#define AS_LIST         4
#define AS_EMPTY_LIST   5


  typedef size_t AFun;

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

  /* defined on SymEntry */
#define SYM_IS_FREE(sym)          (((MachineWord)(sym) & 1) == 1)

#define AT_markAFun(s)       (at_lookup_table[(s)]->header |= MASK_AGE_MARK)
#define AT_markAFun_young(s) if(!IS_OLD(at_lookup_table[(s)]->header)) AT_markAFun(s)


#define AT_unmarkAFun(s) (at_lookup_table[(s)]->header &= ~MASK_MARK)

  union _ATerm;
  extern union _ATerm** at_lookup_table_alias;
  extern SymEntry* at_lookup_table;

  MachineWord AT_symbolTableSize();
  void AT_initAFun(int argc, char* argv[]);
  size_t AT_printAFun(AFun sym, FILE* f);

#define AT_isValidAFun(sym) ((AFun)sym != (AFun)(-1) && (MachineWord)(AFun)sym < AT_symbolTableSize() \
                              && !SYM_IS_FREE(at_lookup_table[(AFun)sym]))


#define AT_isMarkedAFun(sym) IS_MARKED(at_lookup_table[(AFun)sym]->header)

  void  AT_freeAFun(SymEntry sym);
  void AT_markProtectedAFuns();
  void AT_markProtectedAFuns_young();

  size_t AT_hashAFun(const char* name, size_t arity);
  bool AT_findAFun(char* name, size_t arity, bool quoted);
  void AT_unmarkAllAFuns();

#endif
