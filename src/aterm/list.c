/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "memory.h"
#include "list.h"
#include "aterm2.h"
#include "debug.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/*}}}  */
/*{{{  defines */

#define DEFAULT_LIST_BUFFER 256
#define RESIZE_BUFFER(n) if(n > buffer_size) resize_buffer(n)
#define MAGIC_K	1999

/*}}}  */
/*{{{  variables */

char list_id[] = "$Id: list.c,v 1.41 2005/08/17 10:00:41 jong Exp $";

static int buffer_size = 0;
static ATerm *buffer;

/*}}}  */

/*{{{  void AT_initList(int argc, char *argv[]) */

/**
 * Initialize list operations
 */

void AT_initList(int argc, char *argv[])
{
  buffer_size = DEFAULT_LIST_BUFFER;
  buffer = (ATerm *) calloc(buffer_size, sizeof(ATerm));

  if(!buffer) {
    ATerror("AT_initLists: cannot allocate list buffer of size %d\n", 
	    DEFAULT_LIST_BUFFER);
  }

  ATprotectArray(buffer, buffer_size);
}

/*}}}  */
/*{{{  static void resize_buffer(int n) */

/**
 * Make sure the buffer is large enough for a particular operation
 */

static void resize_buffer(int n)
{
  int old_size = buffer_size;
  ATunprotectArray(buffer);

  buffer_size = n;
  buffer = (ATerm *)realloc(buffer, buffer_size*sizeof(ATerm));

  memset(buffer+old_size, 0, (buffer_size - old_size) * sizeof(ATerm));

  if(!buffer) {
    ATerror("resize_buffer: cannot allocate list buffer of size %d\n",
	    DEFAULT_LIST_BUFFER);
  }

  ATprotectArray(buffer, buffer_size);
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

ATermList ATreplaceTail(ATermList list, ATermList newtail, int start)
{
  int i, size;

  if (start < 0) {
    start = ATgetLength(list) + start;
  }

  size = start;
  RESIZE_BUFFER(size);

  for (i=0; i<start; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for (i=start-1; i>=0; i--) {
    newtail = ATinsert(newtail, buffer[i]);
    buffer[i] = NULL;
  }

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
  int i, size = ATgetLength(list)-1;
  ATermList result = ATmakeList0();

  RESIZE_BUFFER(size);
  for(i=0; i<size; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

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
/*{{{  ATermList ATgetSlice(ATermList list, int start, int end) */

/**
 * Retrieve a slice of elements from list.
 * The first element in the slice is the element at position start.
 * The last element is the element at end-1.
 */

ATermList ATgetSlice(ATermList list, int start, int end)
{
  int i, size = end-start;
  ATermList result = ATmakeList0();

  RESIZE_BUFFER(size);

  for(i=0; i<start; i++)
    list = ATgetNext(list);

  for(i=0; i<size; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

  return result;
}

/*}}}  */
/*{{{  ATermList ATinsertAt(ATermList list, ATerm el, int index) */

/**
 * Insert 'el' at position 'index' in 'list'.
 */

ATermList ATinsertAt(ATermList list, ATerm el, int index)
{
  int i;
  ATermList result;

  RESIZE_BUFFER(index);

  /* First collect the prefix in buffer */
  for(i=0; i<index; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  /* Build a list consisting of 'el' and the postfix of the list */
  result = ATinsert(list, el);

  /* Insert elements before 'index' */
  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

  return result;
}

/*}}}  */
/*{{{  ATermlist ATappend(ATermList list, ATerm el) */

/**
 * Append 'el' to the end of 'list'
 */

ATermList ATappend(ATermList list, ATerm el)
{
  int i, len = ATgetLength(list);
  ATermList result;

  RESIZE_BUFFER(len);
  /* Collect all elements of list in buffer */
  for(i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  result = ATmakeList1(el);

  /* Insert elements at the front of the list */
  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

  return result;
}

/*}}}  */
/*{{{  ATermList ATconcat(ATermList list1, ATermList list2) */

/**
 * Concatenate list2 to the end of list1.
 */

ATermList ATconcat(ATermList list1, ATermList list2)
{
  int i, len = ATgetLength(list1);
  ATermList result = list2;

  if(len == 0)
    return list2;
  if(ATisEqual(list2, ATempty))
    return list1;

  RESIZE_BUFFER(len);

  /* Collect the elments of list1 in buffer */
  for(i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list1);
    list1 = ATgetNext(list1);
  }

  /* Insert elements at the front of the list */
  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

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

int ATindexOf(ATermList list, ATerm el, int start)
{
  int i;

  if(start < 0)
    start += (ATgetLength(list) + 1);

  for(i=0; i<start; i++)
    list = ATgetNext(list);

  while(!ATisEmpty(list) && !ATisEqual(ATgetFirst(list), el)) {
    list = ATgetNext(list);
    ++i;
  }

  return (ATisEmpty(list) ? -1 : i);
}

/*}}}  */
/*{{{  int ATlastIndexOf(ATermList list, ATerm el, int start) */

/**
 * Search backwards for 'el' in 'list'. Start searching at
 * index 'start'. Return the index of the first occurence of 'l'
 * encountered, or -1 when 'el' is not present before 'start'.
 */

int ATlastIndexOf(ATermList list, ATerm el, int start)
{
  int i, len;

  if(start < 0)
    start += ATgetLength(list);
  len = start+1;

  RESIZE_BUFFER(len);

  for(i=0; i<len; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }
  for(--i; i>=0; i--) {
    if(ATisEqual(buffer[i], el)) {
      int result = i;
      while(i>=0)
	buffer[i--] = NULL;
      return result;
    }
    buffer[i] = NULL;
  }

  return -1;
}

/*}}}  */
/*{{{  ATerm ATelementAt(ATermList list, int index) */

/**
 * Retrieve the element at 'index' from 'list'.
 * Return NULL when index not in list.
 */

ATerm ATelementAt(ATermList list, int index)
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
  int i = 0;
  ATerm el = NULL;
  ATermList l = list;

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    if(i >= buffer_size) 
      resize_buffer(i*2);
    buffer[i++] = el;
    if(ATisEqual(el, t))
      break;
  }


  if(!ATisEqual(el, t))
    return list;

  list = l; /* Skip element to be removed */

  /* We found the element. Add all elements prior to this 
     one to the tail of the list. */
  for(i-=2; i>=0; i--) {
    list = ATinsert(list, buffer[i]);
    buffer[i] = NULL;
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATremoveElementAt(ATermList list, int idx) */

/**
 * Remove an element from a specific position in a list.
 */

ATermList ATremoveElementAt(ATermList list, int idx)
{
  int i;

  RESIZE_BUFFER(idx);
  for(i=0; i<idx; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  list = ATgetNext(list);
  for(--i; i>=0; i--) {
    list = ATinsert(list, buffer[i]);
    buffer[i] = NULL;
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATremoveAll(ATermList list, ATerm t) */

/**
 * Remove all occurences of an element from a list
 */

ATermList ATremoveAll(ATermList list, ATerm t)
{
  int i = 0;
  ATerm el = NULL;
  ATermList l = list;
  ATbool found = ATfalse;

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    if(!ATisEqual(el, t)) {
      if(i >= buffer_size) 
	resize_buffer(i*2);
      buffer[i++] = el;
    } else
      found = ATtrue;
  }

  if(!found)
    return list;

  /* Add all elements prior to this one to the tail of the list. */
  list = ATempty;
  for(--i; i>=0; i--) {
    list = ATinsert(list, buffer[i]);
    buffer[i] = NULL;
  }

  return list;	
}

/*}}}  */
/*{{{  ATermList ATreplace(ATermList list, ATerm el, int idx) */

/**
 * Replace one element of a list.
 */

ATermList ATreplace(ATermList list, ATerm el, int idx)
{
  int i;

  RESIZE_BUFFER(idx);

  for(i=0; i<idx; i++) {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }
  /* Skip the old element */
  list = ATgetNext(list);
  /* Add the new element */
  list = ATinsert(list, el);
  /* Add the prefix */
  for(--i; i>=0; i--) {
    list = ATinsert(list, buffer[i]);
    buffer[i] = NULL;
  }

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
  int idx, len;
  ATerm *buffer;

  len = ATgetLength(list);
  buffer = calloc(len, sizeof(ATerm));
  if (buffer == NULL) {
    ATerror("ATsort: cannot allocate array for %d terms.\n", len);
  }
  ATprotectArray(buffer, len);

  idx = 0;
  while (!ATisEmpty(list)) {
    buffer[idx++] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  compare_func = compare;
  qsort(buffer, len, sizeof(ATerm),
	(int (*)(const void *, const void *))compare_terms);

  list = ATempty;
  for (idx=len-1; idx>=0; idx--) {
    list = ATinsert(list, buffer[idx]);
  }

  ATunprotectArray(buffer);
  free(buffer);

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
  int i = 0;
  ATermList pair, tmp = (ATermList)dict;

  /* Search for the key */
  while(!ATisEmpty(tmp)) {
    pair = (ATermList)ATgetFirst(tmp);
    tmp = ATgetNext(tmp);
    if(ATisEqual(ATgetFirst(pair), key)) {
      pair = ATmakeList2(key, value);
      tmp = ATinsert(tmp, (ATerm)pair);
      for(--i; i>=0; i--) {
	tmp = ATinsert(tmp, buffer[i]);
	buffer[i] = NULL;
      }
      return (ATerm)tmp;
    } else {
      if(i >= buffer_size)
	resize_buffer(i*2);
      buffer[i++] = (ATerm)pair;
    }
  }

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
  int idx = 0;
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
  int i = 0;
  ATerm el = NULL;
  ATermList l = list;
  ATbool found = ATfalse;

  while(!ATisEmpty(l)) {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    if(predicate(el)) {
      if(i >= buffer_size)  {
	resize_buffer(i*2);
      }
      buffer[i++] = el;
    } else {
      found = ATtrue;
    }
  }

  if(!found)
    return list;

  /* Add all elements prior to this one to the tail of the list. */
  list = ATempty;
  for(--i; i>=0; i--) {
    list = ATinsert(list, buffer[i]);
    buffer[i] = NULL;
  }

  return list;		
}

/*}}}  */

/*{{{  ATermList ATgetArguments(ATermAppl appl) */

/**
 * Retrieve the list of arguments of a function application.
 * This function facilitates porting of old aterm-lib or ToolBus code.
 */

ATermList ATgetArguments(ATermAppl appl)
{
  Symbol s = ATgetSymbol(appl);
  int i, len = ATgetArity(s);
  ATermList result = ATempty;

  RESIZE_BUFFER(len);
  for(i=0; i<len; i++)
    buffer[i] = ATgetArgument(appl, i);

  for(--i; i>=0; i--) {
    result = ATinsert(result, buffer[i]);
    buffer[i] = NULL;
  }

  return result;
}

/*
int ATgetLength(ATermList list) {
  return ((int)GET_LENGTH((list)->header));
}

int     ATgetBlobSize(ATermBlob blob) {
  return ((int)GET_LENGTH((blob)->header));
}
*/

/*}}}  */
