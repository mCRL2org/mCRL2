/*{{{  includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/aterm/memory.h"
#include "mcrl2/aterm/aterm2.h"

/*}}}  */

namespace aterm
{

/*{{{  defines */

static const size_t MAGIC_K = 1999;

/*}}}  */
/*{{{  variables */

char list_id[] = "$Id$";

/*}}}  */

/*{{{  ATermList ATgetTail(ATermList list, int start) */

ATermList ATgetTail(ATermList list, const int start0)
{
  size_t start;
  if (start0 < 0)
  {
    start = ATgetLength(list) + start0;
  }
  else
  {
    start=start0;
  }

  while (start > 0)
  {
    assert(!ATisEmpty(list));
    list = ATgetNext(list);
    start--;
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATgetSlice(ATermList list, size_t start, size_t end) */

/**
 * Retrieve a slice of elements from list.
 * The first element in the slice is the element at position start.
 * The last element is the element at end-1.
 */

ATermList ATgetSlice(ATermList list, const size_t start, const size_t end)
{
  size_t i, size;
  ATermList result = ATmakeList0();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,(end<=start?0:end-start));

  if (end<=start)
  {
    return result;
  }

  size = end-start;

  for (i=0; i<start; i++)
  {
    list = ATgetNext(list);
  }

  for (i=0; i<size; i++)
  {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  for (i=size; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
  }

  return result;
}

/*}}}  */
/*{{{  ATermlist ATappend(ATermList list, ATerm el) */

/**
 * Append 'el' to the end of 'list'
 */

ATermList ATappend(ATermList list, const ATerm el)
{
  size_t i, len = ATgetLength(list);
  ATermList result;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,len);

  /* Collect all elements of list in buffer */
  for (i=0; i<len; i++)
  {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  result = ATmakeList1(el);

  /* Insert elements at the front of the list */
  for (i=len; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
  }

  return result;
}

/*}}}  */
/*{{{  ATermList ATconcat(ATermList list1, ATermList list2) */

/**
 * Concatenate list2 to the end of list1.
 */

ATermList ATconcat(ATermList list1, const ATermList list2)
{
  size_t i, len = ATgetLength(list1);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,len);
  ATermList result = list2;

  if (list2==ATempty)
  {
    return list1;
  }

  if (len == 0)
  {
    return list2;
  }

  /* Collect the elements of list1 in buffer */
  for (i=0; i<len; i++)
  {
    buffer[i] = ATgetFirst(list1);
    list1 = ATgetNext(list1);
  }

  /* Insert elements at the front of the list */
  for (i=len; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
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

size_t ATindexOf(ATermList list, const ATerm el, const int startpos)
{
  size_t i, start;

  if (startpos < 0)
  {
    start = startpos + ATgetLength(list) + 1;
  }
  else
  {
    start = startpos;
  }

  for (i=0; i<start; i++)
  {
    list = ATgetNext(list);
  }

  while (!ATisEmpty(list) && !ATisEqual(ATgetFirst(list), el))
  {
    list = ATgetNext(list);
    ++i;
  }

  return (ATisEmpty(list) ? ATERM_NON_EXISTING_POSITION : i);
}

/*}}}  */
/*{{{  ATerm ATelementAt(ATermList list, int index) */

/**
 * Retrieve the element at 'index' from 'list'.
 * Return NULL when index not in list.
 */

ATerm ATelementAt(ATermList list, size_t index)
{
  for (; index > 0 && !ATisEmpty(list); --index)
  {
    list = ATgetNext(list);
  }

  if (ATisEmpty(list))
  {
    return ATerm();
  }

  return ATgetFirst(list);
}

/*}}}  */
/*{{{  ATermList ATremoveElement(ATermList list, ATerm el) */

/**
 * Remove one occurence of an element from a list.
 */

ATermList ATremoveElement(ATermList list, const ATerm t)
{
  size_t i = 0;
  ATerm el;
  ATermList l = list;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,ATgetLength(list));

  while (!ATisEmpty(l))
  {
    el = ATgetFirst(l);
    l = ATgetNext(l);
    buffer[i++] = el;
    if (ATisEqual(el, t))
    {
      break;
    }
  }

  if (!ATisEqual(el, t))
  {
    return list;
  }

  list = l; /* Skip element to be removed */

  /* We found the element. Add all elements prior to this
     one to the tail of the list. */
  for (i-=1; i>0; i--)
  {
    list = ATinsert(list, buffer[i-1]);
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATremoveElementAt(ATermList list, int idx) */

/**
 * Remove an element from a specific position in a list.
 */

ATermList ATremoveElementAt(ATermList list, const size_t idx)
{
  size_t i;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,idx);

  for (i=0; i<idx; i++)
  {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  list = ATgetNext(list);
  for (i=idx; i>0; i--)
  {
    list = ATinsert(list, buffer[i-1]);
  }

  return list;
}

/*}}}  */
/*{{{  ATermList ATreplace(ATermList list, ATerm el, int idx) */

/**
 * Replace one element of a list.
 */

ATermList ATreplace(ATermList list, const ATerm el, const size_t idx)
{
  size_t i;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,idx);

  for (i=0; i<idx; i++)
  {
    buffer[i] = ATgetFirst(list);
    list = ATgetNext(list);
  }
  /* Skip the old element */
  list = ATgetNext(list);
  /* Add the new element */
  list = ATinsert(list, el);
  /* Add the prefix */
  for (i=idx; i>0; i--)
  {
    list = ATinsert(list, buffer[i-1]);
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

  while (!ATisEmpty(list))
  {
    result = ATinsert(result, ATgetFirst(list));
    list = ATgetNext(list);
  }

  return result;
}

/*}}}  */

/*{{{  ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2)) */

static int (*compare_func)(const ATerm t1, const ATerm t2);

static int compare_terms(const ATerm* t1, const ATerm* t2)
{
  return compare_func(*t1, *t2);
}

ATermList ATsort(ATermList list, int (*compare)(const ATerm t1, const ATerm t2))
{
  size_t idx, len = ATgetLength(list);
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,len);

  idx = 0;
  while (!ATisEmpty(list))
  {
    buffer[idx++] = ATgetFirst(list);
    list = ATgetNext(list);
  }

  compare_func = compare;
  qsort(buffer, len, sizeof(ATerm),
        (int (*)(const void*, const void*))compare_terms);

  list = ATempty;
  for (idx=len; idx>0; idx--)
  {
    list = ATinsert(list, buffer[idx-1]);
  }

  return list;
}

/*}}}  */

/*{{{  ATermList ATgetArguments(ATermAppl appl) */

/**
 * Retrieve the list of arguments of a function application.
 * This function facilitates porting of old aterm-lib or ToolBus code.
 */

ATermList ATgetArguments(const ATermAppl appl)
{
  AFun s = ATgetAFun(appl);
  size_t i, len = ATgetArity(s);
  ATermList result = ATempty;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,ATerm,len);

  for (i=0; i<len; i++)
  {
    buffer[i] = ATgetArgument(appl, i);
  }

  for (i=len; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
  }

  return result;
}

/*}}}  */

/*{{{  size_t ATgetLength(ATermList list) */

size_t ATgetLength(ATermList list)
{
  size_t length = ((size_t)GET_LENGTH((list)->header));

  if (length < MAX_LENGTH-1)
  {
    return length;
  }

  /* Length of the list exceeds the size that can be stored in the header
     Count the length of the list.
  */

  while (1)
  {
    list = ATgetNext(list);
    if ((size_t)GET_LENGTH((list)->header) < (MAX_LENGTH-1))
    {
      break;
    }
    length += 1;
  };

  return length;
}

/*}}}  */

} // namespace aterm
