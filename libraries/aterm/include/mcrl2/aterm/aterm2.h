/**
  * aterm2.h: Definition of the level 2 interface
  * of the ATerm library.
  */

#ifndef ATERM2_H
#define ATERM2_H

/**
  * The level 2 interface is a strict superset
  * of the level 1 interface.
  */

#include <unistd.h>
#include "mcrl2/aterm/aterm1.h"
#include "mcrl2/aterm/afun.h"

/* The largest size_t is used as an indicator that an element does not exist.
   This is used as a replacement of a negative number as an indicator of non
   existence */


namespace aterm
{

/**
  * We define some new datatypes.
  */

struct __ATermInt
{
  header_type header;
  ATerm       next;
  union
  {
    int value;
    MachineWord reserved; /* Only use lower 32 bits as int. The value is used ambiguously
                             as integer and as MachineWord. For all cases using bitwise
                             operations, the MachineWord version must be used,
                             as failing to do so may lead to improper initialisation
                             of the last 32 bits during casting. */
  };

};

static const size_t TERM_SIZE_INT = sizeof(struct __ATermInt)/sizeof(size_t);

typedef union _ATermInt
{
  header_type        header;
  struct __ATermInt  aterm;
}* ATermInt;

struct __ATermAppl
{
  header_type header;
  ATerm       next;
  ATerm       arg[1000];   /* This value 1000 is completely arbitrary, and should not be used
                              (therefore it is excessive). Using mallocs an array of the
                              appropriate length is declared, where it is possible that
                              the array has size 0, i.e. is absent. If the value is small
                              (it was 1), the clang compiler provides warnings. */

};

typedef union _ATermAppl
{
  header_type         header;
  struct __ATermAppl  aterm;
}* ATermAppl;

struct __ATermList
{
  header_type       header;
  ATerm             next;
  ATerm             head;
  union _ATermList* tail;
};

static const size_t TERM_SIZE_LIST = sizeof(struct __ATermList)/sizeof(size_t);

typedef union _ATermList
{
  header_type         header;
  struct __ATermList  aterm;
}* ATermList;

struct _ATermTable;

typedef struct _ATermTable* ATermIndexedSet;
typedef struct _ATermTable* ATermTable;


/* Convenience macro's to circumvent gcc's (correct) warning:
 *   "dereferencing type-punned pointer will break strict-aliasing rules"
 * example usage: ATprotectList(&ATempty);
 */
inline
void ATprotectTerm(const ATerm* p)
{
  ATprotect(p);
}

inline
void ATprotectList(const ATermList* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATprotectAppl(const ATermAppl* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATprotectInt(const ATermInt* p)
{
  ATprotect((const ATerm*) p);
}

inline
void ATunprotectTerm(const ATerm* p)
{
  ATunprotect(p);
}

inline
void ATunprotectList(const ATermList* p)
{
  ATunprotect((const ATerm*) p);
}

inline
void ATunprotectAppl(const ATermAppl* p)
{
  ATunprotect((const ATerm*) p);
}

inline
void ATunprotectInt(const ATermInt* p)
{
  ATunprotect((const ATerm*) p);
}

/** The following functions implement the operations of
  * the 'standard' ATerm interface, and should appear
  * in some form in every implementation of the ATerm
  * datatype.
  */

template <typename TermType1, typename TermType2>
inline
bool ATisEqual(const TermType1 t1, const TermType2 t2)
{
  return ATisEqual((ATerm) t1, (ATerm) t2);
}

/* The ATermInt type */
ATermInt ATmakeInt(const int value);

inline
int ATgetInt(const ATermInt t)
{
  return t->aterm.value;
}

/* The ATermAppl type */
ATermAppl ATmakeAppl(const AFun sym, ...);

/* The implementation of the function below can be found in memory.h */
template <class TERM_ITERATOR>
ATermAppl ATmakeAppl(const AFun sym, const TERM_ITERATOR begin, const TERM_ITERATOR end);

ATermAppl ATmakeAppl0(const AFun sym);
ATermAppl ATmakeAppl1(const AFun sym, const ATerm arg0);
ATermAppl ATmakeAppl2(const AFun sym, const ATerm arg0, const ATerm arg1);
ATermAppl ATmakeAppl3(const AFun sym, const ATerm arg0, const ATerm arg1, const ATerm arg2);
ATermAppl ATmakeAppl4(const AFun sym, const ATerm arg0, const ATerm arg1, const ATerm arg2,
                      const ATerm arg3);
ATermAppl ATmakeAppl5(const AFun sym, const ATerm arg0, const ATerm arg1, const ATerm arg2,
                      const ATerm arg4, const ATerm arg5);
ATermAppl ATmakeAppl6(const AFun sym, const ATerm arg0, const ATerm arg1, const ATerm arg2,
                      const ATerm arg4, const ATerm arg5, const ATerm arg6);




/*AFun    ATgetAFun(ATermAppl appl);*/
inline
AFun ATgetAFun(const ATermAppl appl)
{
  return GET_SYMBOL(appl->header);
}

// TODO: Remove
inline
AFun ATgetAFun(const ATerm t)
{
  return ATgetAFun((ATermAppl)t);
}

/* ATerm     ATgetArgument(ATermAppl appl, size_t arg); */
inline
ATerm& ATgetArgument(const ATermAppl appl, const size_t idx)
{
  return appl->aterm.arg[idx];
}

// TODO: Remove
inline
ATerm& ATgetArgument(const ATerm t, const size_t idx)
{
  return ATgetArgument((ATermAppl)t, idx);
}

ATermAppl ATsetArgument(const ATermAppl appl, const ATerm arg, const size_t n);

/* Portability */
ATermList ATgetArguments(const ATermAppl appl);
ATermAppl ATmakeApplList(const AFun sym, const ATermList args);
ATermAppl ATmakeApplArray(const AFun sym, const ATerm args[]);

size_t ATgetLength(ATermList list);

inline
ATerm& ATgetFirst(const ATermList l)
{
  return l->aterm.head;
}

/* ATermList ATgetNext(ATermList list);*/
inline
ATermList& ATgetNext(const ATermList l)
{
  return l->aterm.tail;
}

/*ATbool ATisEmpty(ATermList list);*/
inline
bool ATisEmpty(const ATermList l)
{
  return l->aterm.head == NULL && l->aterm.tail == NULL;
}

// XXX Remove
inline
bool ATisEmpty(const ATerm l)
{
  return ATisEmpty((ATermList)l);
}

ATermList ATgetTail(ATermList list, const int start);
ATermList ATgetSlice(ATermList list, const size_t start, const size_t end);
ATermList ATinsert(const ATermList list, const ATerm el);
ATermList ATappend(ATermList list, const ATerm el);
ATermList ATconcat(ATermList list1, const ATermList list2);
size_t    ATindexOf(ATermList list, const ATerm el, const int start);
ATerm     ATelementAt(ATermList list, size_t index);
ATermList ATremoveElement(ATermList list, const ATerm el);
ATermList ATremoveElementAt(ATermList list, const size_t idx);
ATermList ATreplace(ATermList list, const ATerm el, const size_t idx);
ATermList ATreverse(ATermList list);
ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2));

/* The ATermList type */
extern ATermList ATempty;

/* ATermList ATmakeList0(); */
inline
ATermList ATmakeList0()
{
  return ATempty;
}

ATermList ATmakeList1(const ATerm el0);

inline
ATermList ATmakeList2(const ATerm el0, const ATerm el1)
{
  return ATinsert(ATmakeList1(el1), el0);
}

inline
ATermList ATmakeList3(const ATerm el0, const ATerm el1, const ATerm el2)
{
  return ATinsert(ATmakeList2(el1, el2), el0);
}

inline
ATermList ATmakeList4(const ATerm el0, const ATerm el1, const ATerm el2, const ATerm el3)
{
  return ATinsert(ATmakeList3(el1, el2, el3), el0);
}

inline
ATermList ATmakeList5(const ATerm el0, const ATerm el1, const ATerm el2, const ATerm el3, const ATerm el4)
{
  return ATinsert(ATmakeList4(el1, el2, el3, el4), el0);
}

inline
ATermList ATmakeList6(const ATerm el0, const ATerm el1, const ATerm el2, const ATerm el3, const ATerm el4, const ATerm el5)
{
  return ATinsert(ATmakeList5(el1, el2, el3, el4, el5), el0);
}

ATermTable ATtableCreate(const size_t initial_size, const unsigned int max_load_pct);
void       ATtableDestroy(ATermTable table);
void       ATtableReset(ATermTable table);
void       ATtablePut(ATermTable table, ATerm key, ATerm value);
ATerm      ATtableGet(ATermTable table, ATerm key);
bool     ATtableRemove(ATermTable table, ATerm key); /* Returns true if removal was successful. */
ATermList  ATtableKeys(ATermTable table);
ATermList  ATtableValues(ATermTable table);

ATermIndexedSet
ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct);
void       ATindexedSetDestroy(ATermIndexedSet set);
void       ATindexedSetReset(ATermIndexedSet set);
size_t     ATindexedSetPut(ATermIndexedSet set, ATerm elem, bool* isnew);
ssize_t    ATindexedSetGetIndex(ATermIndexedSet set, ATerm elem); /* A negative value represents non existence. */
bool     ATindexedSetRemove(ATermIndexedSet set, ATerm elem);   /* Returns true if removal was successful. */
ATermList  ATindexedSetElements(ATermIndexedSet set);
ATerm      ATindexedSetGetElem(ATermIndexedSet set, size_t index);

AFun  ATmakeAFun(const char* name, const size_t arity, const bool quoted);

inline
char* ATgetName(const AFun sym)
{
  return at_lookup_table[sym]->name;
}

inline
size_t ATgetArity(const AFun sym)
{
  return GET_LENGTH(at_lookup_table[sym]->header);
}

inline
bool ATisQuoted(const AFun sym)
{
  return IS_QUOTED(at_lookup_table[sym]->header);
}

void    ATprotectAFun(const AFun sym);
void    ATunprotectAFun(const AFun sym);

/* Compare two ATerms. This is a complete stable ordering on ATerms.
 * They are compared 'lexicographically', function names before the
 * arguments. Function names are compared
 * using strcmp, integers and reals are compared
 * using integer and double comparison, blobs are compared using memcmp.
 * If the types of the terms are different the integer value of ATgetType
 * is used.
 */
int ATcompare(const ATerm t1, const ATerm t2);

extern size_t at_gc_count;

inline
size_t ATgetGCCount()
{
  return at_gc_count;
}

inline
size_t ATgetAFunId(const AFun afun)
{
  return afun;
}

} // namespace aterm

#endif
