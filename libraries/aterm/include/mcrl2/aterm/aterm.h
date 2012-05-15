// Author(s): Aad Matthijsen, Jan Friso Groote, and many others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/indexed_set.h
/// \brief Indexed set.

#ifndef MCRL2_ATERM_ATERM_H
#define MCRL2_ATERM_ATERM_H

#include "mcrl2/atermpp/aterm_int.h"
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_io.h"
#include "mcrl2/atermpp/table.h"
#include "mcrl2/atermpp/indexed_set.h"

// This file contains definitions for old style ATerm terms,
// in terms of those found in the atermpp library.
// The use of these definitions is deprecated.

namespace aterm_deprecated
{


using namespace atermpp;

/* Typedefs */

typedef function_symbol AFun;
typedef aterm ATerm;
typedef aterm_appl ATermAppl;
typedef aterm_list ATermList;
typedef aterm_int ATermInt;

/* AFun related functions */
inline
char* ATgetName(const AFun &sym)
{
  return AFun::at_lookup_table[sym.number()]->name;
}

inline
size_t ATgetArity(const AFun &sym)
{
  return sym.arity();
}


inline
bool ATisQuoted(const AFun &sym)
{
  return sym.is_quoted();
}

inline
size_t ATgetAFun(const ATerm &t)
{
  return t.function().number();
}

inline 
size_t ATgetType(const ATerm &t)
{
  return t.type();
}

inline
bool ATisEqual(const ATerm &t1, const ATerm &t2)
{
  return t1==t2;
}

/* ATermInt related functions */
inline
ATermInt ATmakeInt(const int value)
{
  return ATermInt(value);
}

inline
int ATgetInt(const ATermInt &t)
{
  return t.value();
}

/* ATermAppl related functions */
template <class TERM_ITERATOR>
inline
ATermAppl ATmakeAppl_iterator(const AFun &sym, const TERM_ITERATOR begin, const TERM_ITERATOR end)
{
  return term_appl<ATerm>(sym,begin,end);
}

inline
ATermAppl ATmakeAppl0(const AFun &sym)
{
  return term_appl<ATerm>(sym);
}

inline
ATermAppl ATmakeAppl1(const AFun &sym, const ATerm &arg0)
{
  return term_appl<ATerm>(sym,arg0);
}

inline
ATermAppl ATmakeAppl2(const AFun &sym, const ATerm &arg0, const ATerm &arg1)
{
  return term_appl<ATerm>(sym,arg0,arg1);
}

inline
ATermAppl ATmakeAppl3(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2)
{
  return term_appl<ATerm>(sym,arg0,arg1,arg2);
}

inline
ATermAppl ATmakeAppl4(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3)
{
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3);
}

inline
ATermAppl ATmakeAppl5(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3, const ATerm &arg4)
{
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3,arg4);
}

inline
ATermAppl ATmakeAppl6(const AFun &sym, const ATerm &arg0, const ATerm &arg1, const ATerm &arg2,
                      const ATerm &arg3, const ATerm &arg4, const ATerm &arg5)
{
  return term_appl<ATerm>(sym,arg0,arg1,arg2,arg3,arg4,arg5);
}

inline
ATermAppl ATmakeApplList(const AFun &sym, const ATermList args)
{
  return term_appl<aterm>(sym,args.begin(), args.end());
}

inline
ATermAppl ATmakeAppl_varargs(const AFun &sym, ...)
{
  va_list args;
  size_t arity = sym.arity();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,detail::_aterm*,arity);

  va_start(args, sym);
  for (size_t i=0; i<arity; i++)
  {
    detail::_aterm* arg = va_arg(args, detail::_aterm *);
    CHECK_TERM(arg);
    buffer[i] = arg;
  }
  return aterm_appl(sym,buffer,buffer+arity);
}

inline
const ATerm ATgetArgument(const ATermAppl &appl, const size_t idx)
{
  return appl(idx);
}

inline
ATermAppl ATsetArgument(const ATermAppl &appl, const ATerm &arg, const size_t n)
{
  return appl.set_argument(arg,n);
}

/* ATermList related functions */

inline
size_t ATgetLength(const ATermList &list)
{
  return list.size();
}

inline
ATerm ATgetFirst(const ATermList &l)
{
  return static_cast<ATerm>(l->head);
}

inline
ATermList ATgetNext(const ATermList &l)
{
  return static_cast<ATermList>(l->tail);
}

const ATermList ATempty = aterm_list();

inline
bool ATisEmpty(const ATermList &l)
{
  return l == ATempty;
}

ATermList ATgetTail(const ATermList &list, const int &start);
ATermList ATgetSlice(const ATermList &list, const size_t start, const size_t end);

inline
ATermList ATinsert(const ATermList &list, const ATerm &el)
{
  return push_front(list,el);
}

inline
ATermList ATmakeList0()
{
  return term_list<ATerm>();
}

inline
ATermList ATmakeList1(const ATerm &el0)
{
  return ATinsert(term_list<ATerm>(),el0);
}

inline
ATermList ATmakeList2(const ATerm &el0, const ATerm &el1)
{
  return ATinsert(ATmakeList1(el1), el0);
}

inline
ATermList ATmakeList3(const ATerm &el0, const ATerm &el1, const ATerm &el2)
{
  return ATinsert(ATmakeList2(el1, el2), el0);
}

inline
ATermList ATmakeList4(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3)
{
  return ATinsert(ATmakeList3(el1, el2, el3), el0);
}

inline
ATermList ATmakeList5(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4)
{
  return ATinsert(ATmakeList4(el1, el2, el3, el4), el0);
}

inline
ATermList ATmakeList6(const ATerm &el0, const ATerm &el1, const ATerm &el2, const ATerm &el3, const ATerm &el4, const ATerm &el5)
{
  return ATinsert(ATmakeList5(el1, el2, el3, el4, el5), el0);
}

inline
ATerm ATelementAt(const ATermList &l, size_t m)
{
  return element_at(l,m);
}

inline
ATermList ATconcat(const ATermList &l1, const ATermList &l2)
{
  return l1+l2;
}

inline
ATermList ATreverse(const ATermList &l)
{
  return reverse(l);
}

inline
ATermList ATreplace(const ATermList &list_in, const ATerm &el, const size_t idx) // Replace one element of a list.
{
  ATermList list=list_in;
  size_t i;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,detail::_aterm*,idx);

  for (i=0; i<idx; i++)
  {
    buffer[i] = &*ATgetFirst(list);
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

inline
ATermList ATappend(const ATermList &list_in, const ATerm &el)   // Append 'el' to the end of 'list'
{
  ATermList list=list_in;
  size_t i, len = ATgetLength(list);
  ATermList result;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,detail::_aterm*,len);

  /* Collect all elements of list in buffer */
  for (i=0; i<len; i++)
  {
    buffer[i] = &*ATgetFirst(list);
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

/**
 *  * Retrieve the list of arguments of a function application.
 *   * This function facilitates porting of old aterm-lib or ToolBus code.
 *    */

inline
ATermList ATgetArguments(const ATermAppl &appl)
{
  AFun s = ATgetAFun(appl);
  size_t i, len = ATgetArity(s);
  ATermList result = ATempty;
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,detail::_aterm*,len);

  for (i=0; i<len; i++)
  {
    buffer[i] = &*ATgetArgument(appl, i);
  }

  for (i=len; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
  }

  return result;
}

/**
 *  * Retrieve a slice of elements from list.
 *   * The first element in the slice is the element at position start.
 *    * The last element is the element at end-1.
 *     */

inline
ATermList ATgetSlice(const ATermList &list_in, const size_t start, const size_t end)
{
  size_t i, size;
  ATermList result = ATmakeList0();
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(buffer,detail::_aterm*,(end<=start?0:end-start));

  if (end<=start)
  {
    return result;
  }

  size = end-start;

  ATermList list=list_in;
  for (i=0; i<start; i++)
  {
    list = ATgetNext(list);
  }

  for (i=0; i<size; i++)
  {
    buffer[i] = &*ATgetFirst(list);
    list = ATgetNext(list);
  }

  for (i=size; i>0; i--)
  {
    result = ATinsert(result, buffer[i-1]);
  }

  return result;
}



/**
 *  * Return the index of the first occurence of 'el' in 'list',
 *   * Start searching at index 'start'.
 *    * Return -1 when 'el' is not present after 'start'.
 *     * Note that 'start' must indicate a valid position in 'list'.
 *      */

inline
size_t ATindexOf(const ATermList &list_in, const ATerm &el, const int startpos)
{
  assert(startpos==0);

  ATermList list=list_in;
  // size_t i, start;

  /* if (startpos < 0)
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
  } */

  size_t i=0;
  while (!ATisEmpty(list) && !ATisEqual(ATgetFirst(list), el))
  {
    list = ATgetNext(list);
    ++i;
  }

  return (ATisEmpty(list) ? ATERM_NON_EXISTING_POSITION : i);
}

/* ATtable and ATindexedSet */

typedef table ATermTable;

inline
ATermTable ATtableCreate(const size_t initial_size, const unsigned int max_load_pct)
{
  return table(initial_size,max_load_pct);
}

inline
void       ATtableDestroy(const ATermTable &)
{
}

inline
void       ATtableReset(ATermTable &table)
{
  table.reset();
}

inline
void       ATtablePut(ATermTable &table, const ATerm &key, const ATerm &value)
{
  table.put(key,value);
}

inline
ATerm      ATtableGet(const ATermTable &table, const ATerm &key)
{
  return table.get(key);
}

inline
bool     ATtableRemove(ATermTable &table, const ATerm &key) /* Returns true if removal was successful. */
{
  return table.remove(key);
}

inline
ATermList  ATtableKeys(const ATermTable &table)
{
  return table.keys();
} 

inline
ATermList  ATtableValues(const ATermTable &table)
{
  return table.values();
} 

typedef indexed_set ATermIndexedSet;

inline
ATermIndexedSet ATindexedSetCreate(size_t initial_size, unsigned int max_load_pct)
{
  return indexed_set(initial_size,max_load_pct);
}

inline
void       ATindexedSetDestroy(ATermIndexedSet &)
{
}

inline
void       ATindexedSetReset(ATermIndexedSet &set)
{
  set.reset();
}

inline
size_t     ATindexedSetPut(ATermIndexedSet &set, const ATerm &elem, bool* isnew)
{
  std::pair<size_t, bool> p= set.put(elem);
  *isnew=p.second;
  return p.first;
}

inline
ssize_t    ATindexedSetGetIndex(const ATermIndexedSet &set, const ATerm &elem) /* A negative value represents non existence. */
{
  return set.index(elem);
}

inline
bool     ATindexedSetRemove(ATermIndexedSet &set, const ATerm &elem)   /* Returns true if removal was successful. */
{
  return set.remove(elem);
}

inline
ATerm      ATindexedSetGetElem(const ATermIndexedSet &set, size_t index)
{
  return set.get(index);
}

/* File and string IO */

inline
ATerm ATreadFromFile(FILE* file)
{
  return read_from_file(file);
}

inline
ATerm ATreadFromString(const char* s)
{
  return read_from_string(std::string(s));
}

int
ATfprintf(FILE* stream, const char* format,...);

/* ATerm extensions */
/**
 * \brief Conditional prepend operation on ATermList
 * \param[in] list an ATerm list
 * \param[in] el the aterm to prepend
 * \return el ++ list if not el in list, list if el in list
 **/
inline ATermList ATinsertUnique(const ATermList &list, const ATerm &el)
{
  if (ATindexOf(list, el, 0) == size_t(-1))
  {
    return ATinsert(list, el);
  }
  return list;
}

/**
 * \brief Condition on an ATerm
 * \param[in] t an ATerm
 * \return t is an ATermAppl
 */
inline bool ATisAppl(const ATerm &t)
{
  return ATgetType(t) == AT_APPL;
}

/**
 * \brief Condition on an ATerm
 * \param[in] t an ATerm
 * \return t is an ATermList
 */
inline bool ATisList(const ATerm &t)
{
  return ATgetType(t) == AT_LIST;
}

/**
 * \brief Condition on an ATerm
 * \param[in] t an ATerm
 * \return t is an ATermInt
 */
inline bool ATisInt(const ATerm &t)
{
  return ATgetType(t) == AT_INT;
}

/**
 * \brief Condition on an Aterm
 * \param[in] t an ATerm
 * \return t is NULL or an ATermAppl
 **/
inline bool ATisApplOrNull(const ATerm &t)
{
  return (t == ATerm()) || ATisAppl(t);
}

/**
 * \brief Condition on an Aterm
 * \param[in] t an ATerm
 * \return t is NULL or an ATermList
 **/
inline bool ATisListOrNull(const ATerm &t)
{
  return (t == ATerm()) || ATisList(t);
}

/**
 * \brief Gets an ATermAppl at a specified position in a list
 **/
inline const ATermAppl ATAelementAt(const ATermList &List, const size_t Index)
{
  const ATerm Result = ATelementAt(List, Index);
  assert(ATisApplOrNull(Result));
  return static_cast<const ATermAppl>(Result);
}

/**
 * \brief Gets an ATermList at a specified position in a list
 **/
inline ATermList ATLelementAt(const ATermList List, const size_t Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the argument as ATermAppl at the specified position
 **/
inline ATermAppl ATAgetArgument(const ATermAppl Appl, const size_t Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the argument as ATermList at the specified position
 **/
inline ATermList ATLgetArgument(const ATermAppl Appl, const size_t Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the first argument as ATermAppl
 **/
inline ATermAppl ATAgetFirst(const ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the first argument as ATermList
 **/
inline ATermList ATLgetFirst(const ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the term associated with a key as ATermAppl
 **/
inline ATermAppl ATAtableGet(const ATermTable &Table, const ATerm &Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the term associated with a key as ATermList
 **/
inline ATermList ATLtableGet(const ATermTable &Table, const ATerm &Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

inline
ATermList ATinsertA(const ATermList l, const ATermAppl t)
{
  return ATinsert(l, t);
}

//Substitutions on ATerm's
//------------------------

/**
 * \brief Creates a new substitution
 *
 * \param[in] old_value the Aterm that to replace
 * \param[in] new_value the Aterm to replace with
 *
 * \return a substitution, i.e. an ATermAppl of the form 'subst(old_value, new_value)'
 **/
ATermAppl gsMakeSubst(const ATerm old_value, const ATerm new_value);

/**
 * \brief Creates a new substitution
 *
 * \param[in] old_value the Aterm that to replace
 * \param[in] new_value the Aterm to replace with
 *
 * \return a substitution, i.e. an ATermAppl of the form 'subst(old_value, new_value)'
 * \note ATermAppl variant of gsMakeSubst
 **/
inline ATermAppl gsMakeSubst_Appl(const ATermAppl old_value, const ATermAppl new_value)
{
  return gsMakeSubst(old_value, new_value);
}

/**
 * \brief Creates a new substitution
 *
 * \param[in] old_value the Aterm that to replace
 * \param[in] new_value the Aterm to replace with
 *
 * \return a substitution, i.e. an ATermAppl of the form 'subst(old_value, new_value)'
 * \note ATermList variant of gsMakeSubst
 **/
inline ATermAppl gsMakeSubst_List(const ATermList old_value, const ATermList new_value)
{
  return gsMakeSubst(old_value, new_value);
}

/**
 * \brief Applies a list of substitutions to a term
 *
 * \param[in] substs a table of substitutions from Aterm to Aterm
 * \param[in] term an Aterm
 * \param[in] recursive flag that triggers recursive traversal
 *
 * \pre substs is a list containing substitutions only
 *     term is an ATerm containing ATermAppl's and ATermList's only
 * \return term, in which all substitutions are applied to the top level of term,
 *     from head to tail; if recursive and there was no match, the
 *     substitutions are distributed over the arguments/elements of term
 **/
ATerm gsSubstValues(const ATermList substs, ATerm term, const bool recursive);

/**
 * \brief Applies a list of substitutions to a term
 *
 * \param[in] substs a table of substitutions from Aterm to Aterm
 * \param[in] appl an Aterm
 * \param[in] recursive flag that triggers recursive traversal
 *
 * \pre substs is a list containing substitutions only
 *     term is an ATerm containing ATermAppl's and ATermList's only
 * \return term, in which all substitutions are applied to the top level of term,
 *     from head to tail; if recursive and there was no match, the
 *     substitutions are distributed over the arguments/elements of term
 * \note This is the ATermAppl variant of gsSubstValues
 **/
inline ATermAppl gsSubstValues_Appl(const ATermList substs, ATermAppl appl, bool recursive)
{
  return (ATermAppl) gsSubstValues(substs, appl, recursive);
}

/**
 * \brief Applies a list of substitutions to a term
 *
 * \param[in] substs a table of substitutions from Aterm to Aterm
 * \param[in] list an Aterm list
 * \param[in] recursive flag that triggers recursive traversal
 *
 * \pre substs is a list containing substitutions only
 *     term is an ATerm containing ATermAppl's and ATermList's only
 * \return term, in which all substitutions are applied to the top level of term,
 *     from head to tail; if recursive and there was no match, the
 *     substitutions are distributed over the arguments/elements of term
 * \note This is the ATermList variant of gsSubstValues
 **/
inline ATermList gsSubstValues_List(const ATermList substs, ATermList list, const bool recursive)
{
  return (ATermList) gsSubstValues(substs, list, recursive);
}

/**
 * \brief Adds a substitution to a list of substitutions
 *
 * \param[in] substs a table of substitutions from Aterm to Aterm
 * \param[in] term an Aterm
 * \param[in] recursive flag that triggers recursive traversal
 * \pre substs is a table containing substitutions from ATerm's to ATerm's
 *     Term is an ATerm consisting of ATermAppl's and ATermList's only
 * \return Term in which all substitutions from substs are performed recursively
 **/
ATerm gsSubstValuesTable(const ATermTable &substs, ATerm term, const bool recursive);

//Occurrences of ATerm's
//----------------------

/**
 * \brief Counts the number of times that an Aterm occurs in another Aterm
 * \param[in] elt a term of which to count instances
 * \param[in] term a term to search inside
 * \pre term is an ATerm containing ATermAppl's and ATermList's only
 * \return elt occurs in term
 * \note that this is a faster implementation than gsCount(elt, term) > 0 because
 *       it is used at a crucial point in the rewriter
 **/
bool gsOccurs(const ATerm elt, ATerm term);

} // namespace aterm_deprecated


#endif // MCRL2_ATERM_ATERM_H
