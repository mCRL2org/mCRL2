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
#include "aterm1.h"
#include "afun.h"
#include "abool.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

  /* The largest size_t is used as an indicator that an element does not exist.
     This is used as a replacement of a negative number as an indicator of non
     existence */


  /**
    * We define some new datatypes.
    */

  struct __ATermInt
  {
    header_type header;
    ATerm       next;
    MachineWord value; /* Only use lower 32 bits as int, but using "int" here may lead to non
                        initialisation of the other 32 bits; It is conceivable to replace this
                        int by a 64 bit integer */
  };

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

  typedef union _ATermList
  {
    header_type         header;
    struct __ATermList  aterm;
  }* ATermList;

  struct _ATermTable;

  typedef struct _ATermTable* ATermIndexedSet;
  typedef struct _ATermTable* ATermTable;


  /** The following functions implement the operations of
    * the 'standard' ATerm interface, and should appear
    * in some form in every implementation of the ATerm
    * datatype.
    */

  /* The ATermInt type */
  ATermInt ATmakeInt(int value);
  /*int      ATgetInt(ATermInt term);*/
#define ATgetInt(t) ((int)(((ATermInt)t)->aterm.value))

  /* The ATermReal type */
  /* ATermReal ATmakeReal(double value); */
  /*double    ATgetReal(ATermReal term);*/
  /* #define ATgetReal(t) (((ATermReal)t)->aterm.value) */

  /* The ATermAppl type */
  ATermAppl ATmakeAppl(AFun sym, ...);
  ATermAppl ATmakeAppl0(AFun sym);
  ATermAppl ATmakeAppl1(AFun sym, ATerm arg0);
  ATermAppl ATmakeAppl2(AFun sym, ATerm arg0, ATerm arg1);
  ATermAppl ATmakeAppl3(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2);
  ATermAppl ATmakeAppl4(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
                        ATerm arg3);
  ATermAppl ATmakeAppl5(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
                        ATerm arg4, ATerm arg5);
  ATermAppl ATmakeAppl6(AFun sym, ATerm arg0, ATerm arg1, ATerm arg2,
                        ATerm arg4, ATerm arg5, ATerm arg6);

  /*AFun    ATgetAFun(ATermAppl appl);*/
#define ATgetAFun(appl) GET_SYMBOL((appl)->header)

  /* ATerm     ATgetArgument(ATermAppl appl, size_t arg); */
#define ATgetArgument(appl,idx) (((ATermAppl)appl)->aterm.arg[idx])
  ATermAppl ATsetArgument(ATermAppl appl, ATerm arg, size_t n);

  /* Portability */
  ATermList ATgetArguments(ATermAppl appl);
  ATermAppl ATmakeApplList(AFun sym, ATermList args);
  ATermAppl ATmakeApplArray(AFun sym, ATerm args[]);

  /* The ATermList type */
  extern ATermList ATempty;

  ATermList ATmakeList(size_t n, ...);

  /* ATermList ATmakeList0(); */
#define ATmakeList0() (ATempty)

  ATermList ATmakeList1(ATerm el0);

  /* ATermList ATmakeList2(ATerm el0, ATerm el1); */
#define ATmakeList2(el0, el1)           ATinsert(ATmakeList1(el1), el0)
#define ATmakeList3(el0, el1, el2)      ATinsert(ATmakeList2(el1,el2), el0)
#define ATmakeList4(el0, el1, el2, el3) ATinsert(ATmakeList3(el1,el2,el3), el0)
#define ATmakeList5(el0, el1, el2, el3, el4) \
  ATinsert(ATmakeList4(el1,el2,el3,el4), el0)
#define ATmakeList6(el0, el1, el2, el3, el4, el5) \
  ATinsert(ATmakeList5(el1,el2,el3,el4,el5), el0)

  /*size_t ATgetLength(ATermList list);*/
  size_t ATgetLength(ATermList list);

  /* ATerm ATgetFirst(ATermList list);*/
#define   ATgetFirst(l) (((ATermList)l)->aterm.head)

  /* ATermList ATgetNext(ATermList list);*/
#define   ATgetNext(l)  (((ATermList)l)->aterm.tail)

  /*ATbool ATisEmpty(ATermList list);*/
#define ATisEmpty(list) ((ATbool)(((ATermList)list)->aterm.head == NULL \
                                  && ((ATermList)list)->aterm.tail == NULL))

  ATermList ATgetTail(ATermList list, int start);
  ATermList ATgetSlice(ATermList list, size_t start, size_t end);
  ATermList ATinsert(ATermList list, ATerm el);
  ATermList ATappend(ATermList list, ATerm el);
  ATermList ATconcat(ATermList list1, ATermList list2);
  size_t    ATindexOf(ATermList list, ATerm el, int start);
  ATerm     ATelementAt(ATermList list, size_t index);
  ATermList ATremoveElement(ATermList list, ATerm el);
  ATermList ATremoveElementAt(ATermList list, size_t idx);
  ATermList ATreplace(ATermList list, ATerm el, size_t idx);
  ATermList ATreverse(ATermList list);
  ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2));

  ATermTable ATtableCreate(size_t initial_size, unsigned int max_load_pct);
  void       ATtableDestroy(ATermTable table);
  void       ATtableReset(ATermTable table);
  void       ATtablePut(ATermTable table, ATerm key, ATerm value);
  ATerm    ATtableGet(ATermTable table, ATerm key);
  void       ATtableRemove(ATermTable table, ATerm key);
  ATermList  ATtableKeys(ATermTable table);
  ATermList  ATtableValues(ATermTable table);

  ATermIndexedSet
  ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct);
  void       ATindexedSetDestroy(ATermIndexedSet set);
  void       ATindexedSetReset(ATermIndexedSet set);
  size_t     ATindexedSetPut(ATermIndexedSet set, ATerm elem, ATbool* isnew);
  ssize_t     ATindexedSetGetIndex(ATermIndexedSet set, ATerm elem); /* A negative value represents non existence. */
  void       ATindexedSetRemove(ATermIndexedSet set, ATerm elem);
  ATermList  ATindexedSetElements(ATermIndexedSet set);
  ATerm      ATindexedSetGetElem(ATermIndexedSet set, size_t index);

  AFun  ATmakeAFun(const char* name, size_t arity, ATbool quoted);

  /*char   *ATgetName(AFun sym);*/
#define ATgetName(sym) (at_lookup_table[(sym)]->name)
  /*int     ATgetArity(AFun sym);*/
#define ATgetArity(sym) ((size_t)GET_LENGTH(at_lookup_table_alias[(sym)]->header))
  /*ATbool  ATisQuoted(AFun sym);*/
#define ATisQuoted(sym) IS_QUOTED(at_lookup_table_alias[(sym)]->header)

  void    ATprotectAFun(AFun sym);
  void    ATunprotectAFun(AFun sym);
  void ATprotectMemory(void* start, size_t size);
  void ATunprotectMemory(void* start);

  /* Calculate checksum using the
     "RSA Data Security, Inc. MD5 Message-Digest Algorithm" (see RFC1321)
  */
  unsigned char* ATchecksum(ATerm t);
  ATbool ATdiff(ATerm t1, ATerm t2, ATerm* templ, ATerm* diffs);

  /* Compare two ATerms. This is a complete stable ordering on ATerms.
   * They are compared 'lexicographically', function names before the
   * arguments. Function names are compared
   * using strcmp, integers and reals are compared
   * using integer and double comparison, blobs are compared using memcmp.
   * If the types of the terms are different the integer value of ATgetType
   * is used.
   */
  int ATcompare(ATerm t1, ATerm t2);

  void ATsetChecking(ATbool on);
  ATbool ATgetChecking(void);

  extern size_t at_gc_count;
#define ATgetGCCount()    (at_gc_count)

  size_t  ATcalcTextSize(ATerm t);

  void AT_writeToStringBuffer(ATerm t, char* buffer);
#define ATwriteToStringBuffer(t,b) AT_writeToStringBuffer((t),(b))

#define ATgetAFunId(afun) ((afun))

#ifdef __cplusplus
}
#endif/* __cplusplus */

#endif
