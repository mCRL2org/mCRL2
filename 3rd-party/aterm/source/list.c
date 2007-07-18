/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "memory.h"
#include "list.h"
#include "aterm2.h"
#include "debug.h"

/*}}}  */
/*{{{  defines */

#define MAGIC_K	1999

/*}}}  */
/*{{{  variables */

char list_id[] = "$Id: list.c 23071 2007-07-02 10:06:17Z eriks $";

/*}}}  */

/*{{{  void AT_initList(int argc, char *argv[]) */

/**
 * Initialize list operations
 */

void AT_initList(int argc, char *argv[])
{
  /* Suppress unused arguments warning */
  (void) argc;
  (void) argv;
}

/*}}}  */
/*{{{  ATermList ATgetTail(ATermList list, int start) */

ATermList ATgetTail(ATermList list, int start)
{
  if (start < 0) {
    start = ATgetLength(list) + start;
  }

  while (start > 0) {
    assert(!ATisEmpty(list));
    list = ATgetNext(list);
    start--;
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATreplaceTail(ATermList list, ATermList newtail, int start) */

ATermList ATreplaceTail(ATermList list, ATermList newtail, int startpos)
{
  ATerm* buffer;
  unsigned int i, start;

  if (startpos < 0) {
    start = ATgetLength(list) + startpos;
  }
  else {
    start = startpos;
  }

  buffer = AT_alloc_protected(start);
  if (!buffer) {
    ATerror("ATreplaceTail: out of memory");
  }

  for (i=0; i<start; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for (i=start; i>0; i--) {
    newtail = ATinsert(newtail, buffer[i-1]);
  }
  
  AT_free_protected(buffer);

  /* Preserve annotations */
  if (AT_getAnnotations((ATerm)list) != NULL) {
    newtail = (ATermList)AT_setAnnotations((ATerm)newtail,
					   AT_getAnnotations((ATerm)list));
  }

  return newtail;
}

/*}}}  */
/*{{{  ATermList ATgetPrefix(ATermList list) */

/**
 * Build a new list containing all elements of list, except the last one.
 */

ATermList ATgetPrefix(ATermList list)
{
  ATerm* buffer;
  unsigned int i, size = ATgetLength(list);
  ATermList result = ATmakeList0();
  
  if (size<=1)
     return result;
     
  size -= 1;
  
  buffer = AT_alloc_protected(size);
  if (!buffer) {
    ATerror("ATgetPrefix: out of memory");
  }
  
  for(i=0; i<size; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for(i=size; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);

  return result;
}

/*}}}  */
/*{{{  ATerm ATgetLast(ATermList list) */

/**
 * Retrieve the last element of 'list'.
 * When 'list' is the empty list, NULL is returned.
 */

ATerm ATgetLast(ATermList list)
{
  ATermList old = ATempty;

  while(!ATisEmpty(list)) {
    old = list;
    list = ATgetNext(list);
  }
  return ATgetFirst(old);
}

/*}}}  */
/*{{{  ATermList ATgetSlice(ATermList list, unsigned int start, unsigned int end) */

/**
 * Retrieve a slice of elements from list.
 * The first element in the slice is the element at position start.
 * The last element is the element at end-1.
 */

ATermList ATgetSlice(ATermList list, unsigned int start, unsigned int end)
{
  ATerm* buffer;
  unsigned int i, size;
  ATermList result = ATmakeList0();
 
  if (end<=start)
    return result;
  
  size = end-start;
  
  buffer = AT_alloc_protected(size);
  if (!buffer) {
    ATerror("ATgetSlice: out of memory");
  }

  for(i=0; i<start; i++)
    list = ATgetNext(list);

  for(i=0; i<size; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for(i=size; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return result;
}

/*}}}  */
/*{{{  ATermList ATinsertAt(ATermList list, ATerm el, unsigned int index) */

/**
 * Insert 'el' at position 'index' in 'list'.
 */

ATermList ATinsertAt(ATermList list, ATerm el, unsigned int index)
{
  ATerm* buffer;
  unsigned int i;
  ATermList result;
  
  buffer = AT_alloc_protected(index);
  if (!buffer) {
    ATerror("ATinsertAt: out of memory");
  }

  /* First collect the prefix in buffer */
  for(i=0; i<index; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  /* Build a list consisting of 'el' and the postfix of the list */
  result = ATinsert(list, el);

  /* Insert elements before 'index' */
  for(i=index; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);

  return result;
}

/*}}}  */
/*{{{  ATermlist ATappend(ATermList list, ATerm el) */

/**
 * Append 'el' to the end of 'list'
 */

ATermList ATappend(ATermList list, ATerm el)
{
  ATerm* buffer;
  unsigned int i, len = ATgetLength(list);
  ATermList result;
  
  buffer = AT_alloc_protected(len);
  if (!buffer) {
    ATerror("ATappend: out of memory");
  }

  /* Collect all elements of list in buffer */
  for(i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  result = ATmakeList1(el);

  /* Insert elements at the front of the list */
  for(i=len; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return result;
}

/*}}}  */
/*{{{  ATermList ATconcat(ATermList list1, ATermList list2) */

/**
 * Concatenate list2 to the end of list1.
 */

ATermList ATconcat(ATermList list1, ATermList list2)
{
  ATerm* buffer;
  unsigned int i, len = ATgetLength(list1);
  ATermList result = list2;

  if(len == 0)
    return list2;
  if(ATisEqual(list2, ATempty))
    return list1;

  buffer = AT_alloc_protected(len);
  if (!buffer) {
    ATerror("ATconcat: out of memory");
  }

  /* Collect the elements of list1 in buffer */
  for(i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list1);
    list1 = ATgetNext(list1);
  }

  /* Insert elements at the front of the list */
  for(i=len; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return result;
}

/*}}}  */
/*{{{  int ATindexOf(ATermList list, ATerm el, int start) */

/**
 * Return the index of the first occurence of 'el' in 'list',
 * Start searching at index 'start'.
 * Return -1 when 'el' is not present after 'start'.
 * Note that 'start' must indicate a valid position in 'list'.
 */

int ATindexOf(ATermList list, ATerm el, int startpos)
{
  unsigned int i, start;

  if(startpos < 0)
    start = startpos + ATgetLength(list) + 1;
  else
    start = startpos;

  for(i=0; i<start; i++)
    list = ATgetNext(list);

  while(!ATisEmpty(list) && !ATisEqual(ATgetFirst(list), el)) {
    list = ATgetNext(list);
    ++i;
  }

  return (ATisEmpty(list) ? -1 : (int)i);
}

/*}}}  */
/*{{{  int ATlastIndexOf(ATermList list, ATerm el, int start) */

/**
 * Search backwards for 'el' in 'list'. Start searching at
 * index 'start'. Return the index of the first occurence of 'l'
 * encountered, or -1 when 'el' is not present before 'start'.
 */

int ATlastIndexOf(ATermList list, ATerm el, int startpos)
{
  ATerm* buffer;
  unsigned int i, len, start;

  if(startpos < 0)
    start = startpos + ATgetLength(list);
  else
    start = startpos;
    
  len = start+1;

  buffer = AT_alloc_protected(len);
  if (!buffer) {
    ATerror("ATlastIndexOf: out of memory");
  }

  for (i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for (i=len; i>0; i--) {
    if (ATisEqual(buffer[i-1], el)) {
      AT_free_protected(buffer);
      return i-1;
    }
  }
  
  AT_free_protected(buffer);

  return -1;
}

/*}}}  */
/*{{{  ATerm ATelementAt(ATermList list, int index) */

/**
 * Retrieve the element at 'index' from 'list'.
 * Return NULL when index not in list.
 */

ATerm ATelementAt(ATermList list, unsigned int index)
{
  for(; index > 0 && !ATisEmpty(list); index--)
    list = ATgetNext(list);

  if(ATisEmpty(list))
    return NULL;

  return ATgetFirst(list);
}

/*}}}  */
/*{{{  ATermList ATremoveElement(ATermList list, ATerm el) */

/**
 * Remove one occurence of an element from a list.
 */

ATermList ATremoveElement(ATermList list, ATerm t)
{
  ATerm* buffer;
  unsigned int i = 0;
  ATerm el = NULL;
  ATermList l = list;
  
  buffer = AT_alloc_protected_minmax(0, ATgetLength(list));
  if (!buffer) {
    ATerror("ATremoveElement: out of memory");
  }

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    buffer = AT_grow_protected(buffer, i+1);
    if (!buffer) {
      ATerror("ATremoveElement: out of memory");
    }
    buffer[i++] = el;
    if(ATisEqual(el, t))
      break;
  }

  if(!ATisEqual(el, t)) {
    AT_free_protected(buffer);
    return list;
  }

  list = l; /* Skip element to be removed */

  /* We found the element. Add all elements prior to this 
     one to the tail of the list. */
  for(i-=1; i>0; i--) {
    list = ATinsert(list, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return list;
}

/*}}}  */
/*{{{  ATermList ATremoveElementAt(ATermList list, int idx) */

/**
 * Remove an element from a specific position in a list.
 */

ATermList ATremoveElementAt(ATermList list, unsigned int idx)
{
  ATerm* buffer;
  unsigned int i;
  
  buffer = AT_alloc_protected(idx);
  if (!buffer) {
    ATerror("ATremoveElementAt: out of memory");
  }

  for(i=0; i<idx; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  list = ATgetNext(list);
  for(i=idx; i>0; i--) {
    list = ATinsert(list, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return list;
}

/*}}}  */
/*{{{  ATermList ATremoveAll(ATermList list, ATerm t) */

/**
 * Remove all occurences of an element from a list
 */

ATermList ATremoveAll(ATermList list, ATerm t)
{
  ATerm* buffer;
  unsigned int i = 0;
  ATerm el = NULL;
  ATermList l = list;
  ATermList result = ATempty;
  ATbool found = ATfalse;

  buffer = AT_alloc_protected_minmax(0, ATgetLength(list));
  if (!buffer) {
    ATerror("ATremoveAll: out of memory");
  }

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    if(!ATisEqual(el, t)) {
      buffer = AT_grow_protected(buffer, i+1);
      if (!buffer) {
        ATerror("ATremoveAll: out of memory");
      }
      buffer[i++] = el;
    } else
      found = ATtrue;
  }

  if(!found) {
    AT_free_protected(buffer);
    return list;
  }

  /* Add all elements prior to this one to the tail of the list. */
  while (i>0) {
    result = ATinsert(result, buffer[i-1]);
    i--;
  }

  AT_free_protected(buffer);
  return result;
}

/*}}}  */
/*{{{  ATermList ATreplace(ATermList list, ATerm el, int idx) */

/**
 * Replace one element of a list.
 */

ATermList ATreplace(ATermList list, ATerm el, unsigned int idx)
{
  ATerm* buffer;
  unsigned int i;
  
  buffer = AT_alloc_protected(idx);
  if (!buffer) {
    ATerror("ATreplace: out of memory");
  }

  for(i=0; i<idx; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }
  /* Skip the old element */
  list = ATgetNext(list);
  /* Add the new element */
  list = ATinsert(list, el);
  /* Add the prefix */
  for(i=idx; i>0; i--) {
    list = ATinsert(list, buffer[i-1]);
  }
  
  AT_free_protected(buffer);
  
  return list;
}

/*}}}  */
/*{{{  ATermList ATreverse(ATermList list) */

/**
 * Reverse a list
 */

ATermList ATreverse(ATermList list)
{
  ATermList result = ATempty;

  while(!ATisEmpty(list)) {
    result = ATinsert(result, ATgetFirst(list));
    list = ATgetNext(list);
  }

  return result;
}

/*}}}  */

/*{{{  ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2)) */

static int (*compare_func)(const ATerm t1, const ATerm t2);

static int compare_terms(const ATerm *t1, const ATerm *t2)
{
  return compare_func(*t1, *t2);
}

ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2))
{
  ATerm* buffer;
  unsigned int idx, len = ATgetLength(list);

  buffer = AT_alloc_protected(len);
  if (!buffer) {
    ATerror("ATsort: out of memory");
  }

  idx = 0;
  while (!ATisEmpty(list)) {
    buffer[idx++] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  compare_func = compare;
  qsort(buffer, len, sizeof(ATerm),
	(int (*)(const void *, const void *))compare_terms);

  list = ATempty;
  for (idx=len; idx>0; idx--) {
    list = ATinsert(list, buffer[idx-1]);
  }
  
  AT_free_protected(buffer);
  
  return list;
}

/*}}}  */
/*{{{  ATerm ATdictCreate() */

/**
 * Create a new dictionary.
 */

ATerm ATdictCreate()
{
  return (ATerm)ATempty;
}

/*}}}  */
/*{{{  ATerm ATdictPut(ATerm dict, ATerm key, ATerm value) */

/**
 * Set an element of a 'dictionary list'. This is a list consisting
 * of [key,value] pairs.
 */

ATerm ATdictPut(ATerm dict, ATerm key, ATerm value)
{
  ATerm* buffer;
  unsigned int i = 0;
  ATermList pair, tmp = (ATermList)dict;

  buffer = AT_alloc_protected_minmax(0, ATgetLength(tmp));
  if (!buffer) {
    ATerror("ATdictPut: out of memory");
  }

  /* Search for the key */
  while(!ATisEmpty(tmp)) {
    pair = (ATermList)ATgetFirst(tmp);
    tmp = ATgetNext(tmp);
    if(ATisEqual(ATgetFirst(pair), key)) {
      pair = ATmakeList2(key, value);
      tmp = ATinsert(tmp, (ATerm)pair);
      while (i>0) {
	tmp = ATinsert(tmp, buffer[i-1]);
        i--;
      }
      AT_free_protected(buffer);
      return (ATerm)tmp;
    } else {
      buffer = AT_grow_protected(buffer, i+1);
      if (!buffer) {
        ATerror("ATdictPut: out of memory");
      }
      buffer[i++] = (ATerm)pair;
    }
  }
  
  AT_free_protected(buffer);

  /* The key is not in the dictionary */
  pair = ATmakeList2(key, value);
  return (ATerm)ATinsert((ATermList)dict, (ATerm)pair);
}

/*}}}  */
/*{{{  ATerm ATdictGet(ATerm dict, ATerm key) */

/**
 * Retrieve a value from a dictionary list.
 */

ATerm ATdictGet(ATerm dict, ATerm key)
{
  ATermList pair;
  ATermList tmp = (ATermList)dict;

  /* Search for the key */
  while(!ATisEmpty(tmp)) {
    pair = (ATermList)ATgetFirst(tmp);

    if(ATisEqual(ATgetFirst(pair), key))
      return ATgetFirst(ATgetNext(pair));

    tmp = ATgetNext(tmp);
  }

  /* The key is not in the dictionary */
  return NULL;
}

/*}}}  */
/*{{{  ATerm ATdictRemove(ATerm dict, ATerm key) */

/**
 * Remove a [key,value] pair from a dictionary list.
 */

ATerm ATdictRemove(ATerm dict, ATerm key)
{
  unsigned int idx = 0;
  ATermList tmp = (ATermList)dict;
  ATermList pair;

  /* Search for the key */
  while(!ATisEmpty(tmp)) {
    pair = (ATermList)ATgetFirst(tmp);
    if(ATisEqual(ATgetFirst(pair), key))
      return (ATerm)ATremoveElementAt((ATermList)dict, idx);

    tmp = ATgetNext(tmp);
    idx++;
  }

  /* The key is not in the dictionary */
  return dict;
}

/*}}}  */
/*{{{  ATermList ATfilter(ATermList list, ATbool (*predicate)(ATerm)) */

/**
 * Filter elements from a list.
 */

ATermList ATfilter(ATermList list, ATbool (*predicate)(ATerm))
{
  ATerm* buffer;
  unsigned int i = 0;
  ATerm el = NULL;
  ATermList l = list;
  ATermList result = ATempty;
  ATbool found = ATfalse;

  buffer = AT_alloc_protected_minmax(0, ATgetLength(list));
  if (!buffer) {
    ATerror("ATfilter: out of memory");
  }

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    if(predicate(el)) {
      buffer = AT_grow_protected(buffer, i+1);
      if (!buffer) {
        ATerror("ATfilter: out of memory");
      }
      buffer[i++] = el;
    } else {
      found = ATtrue;
    }
  }

  if(!found) {
    AT_free_protected(buffer); 
    return list;
  }

  /* Add all elements prior to this one to the tail of the list. */
  while (i>0) {
    result = ATinsert(result, buffer[i-1]);
    i--;
  }

  AT_free_protected(buffer); 
  
  return result;
}

/*}}}  */

/*{{{  ATermList ATgetArguments(ATermAppl appl) */

/**
 * Retrieve the list of arguments of a function application.
 * This function facilitates porting of old aterm-lib or ToolBus code.
 */

ATermList ATgetArguments(ATermAppl appl)
{
  ATerm* buffer;
  Symbol s = ATgetSymbol(appl);
  unsigned int i, len = ATgetArity(s);
  ATermList result = ATempty;

  buffer = AT_alloc_protected(len);
  if (!buffer) {
    ATerror("ATsort: out of memory");
  }

  for(i=0; i<len; i++)
    buffer[i] = ATgetArgument(appl, i);

  for(i=len; i>0; i--) {
    result = ATinsert(result, buffer[i-1]);
  }
  
  AT_free_protected(buffer);

  return result;
}

/*}}}  */

/*{{{  unsigned int ATgetLength(ATermList list) */

unsigned int ATgetLength(ATermList list) {
  unsigned int length = ((unsigned int)GET_LENGTH((list)->header));
  
  if (length < MAX_LENGTH-1)
    return length;
    
  /* Length of the list exceeds the size that can be stored in the header
     Count the length of the list.
  */
  
  while (1) {
    list = ATgetNext(list);
    if ((unsigned int)GET_LENGTH((list)->header) < (MAX_LENGTH-1)) break;
    length += 1;
  };
  
  return length;
}

/*
int     ATgetBlobSize(ATermBlob blob) {
  return ((int)GET_LENGTH((blob)->header));
}
*/

/*}}}  */
