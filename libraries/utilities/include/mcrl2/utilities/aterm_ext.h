// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/aterm_ext.h
/// \brief Add your file description here.

#ifndef __ATERM_EXT_H__
#define __ATERM_EXT_H__

#ifndef __cplusplus
# include <stdbool.h>
#endif
#include <stdarg.h>
#include <assert.h>
#include <aterm2.h>

#include <workarounds.h>

#ifdef __cplusplus
namespace mcrl2 {
  namespace utilities {
    extern "C" {
#endif

//Global precondition: the ATerm library has been initialised
//ATerm library workarounds
//--------------------------
//
//To eliminate downcasts in the rest of the code, we introduce wrappers around
//ATerm library functions.
//This is caused by a bad interface design of the ATerm library.

/**
 * \brief Conditional prepend operation on ATermList
 * \return el ++ list if not el in list, list if el in list
 **/
inline ATermList ATinsertUnique(ATermList list, ATerm el)
{
  if (ATindexOf(list, el, 0) == -1) return ATinsert(list, el);
  return list;
}

/**
 * \brief Condition on an Aterm
 * \return t is NULL or an ATermAppl
 **/
inline bool ATisApplOrNull(ATerm t)
{
  if (t == NULL) return true;
  return ATgetType(t) == AT_APPL;
}

/**
 * \brief Condition on an Aterm
 * \return t is NULL or an ATermList
 **/
inline bool ATisListOrNull(ATerm t)
{
  if (t == NULL) return true;
  return ATgetType(t) == AT_LIST;
}

/**
 * \brief Gets an ATermAppl at a specified position in a list
 **/
inline ATermAppl ATAelementAt(ATermList List, int Index) {
  ATerm Result = ATelementAt(List, Index);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets an ATermList at a specified position in a list
 **/
inline ATermList ATLelementAt(ATermList List, int Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the argument as ATermAppl at the specified position
 **/
inline ATermAppl ATAgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the argument as ATermList at the specified position
 **/
inline ATermList ATLgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the first argument as ATermAppl
 **/
inline ATermAppl ATAgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the first argument as ATermList
 **/
inline ATermList ATLgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

/**
 * \brief Gets the term associated with a key as ATermAppl
 **/
inline ATermAppl ATAtableGet(ATermTable Table, ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the term associated with a key as ATermList
 **/
inline ATermList ATLtableGet(ATermTable Table, ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

#ifndef ATprotectAppl
inline void ATprotectAppl(ATermAppl *PAppl)
{
  ATprotect((ATerm *) PAppl);
}
#endif

#ifndef ATprotectList
inline void ATprotectList(ATermList *PList)
{
  ATprotect((ATerm *) PList);
}
#endif

#ifndef ATprotectInt
inline void ATprotectInt(ATermInt *PInt)
{
  ATprotect((ATerm *) PInt);
}
#endif

#ifndef ATunprotectAppl
inline void ATunprotectAppl(ATermAppl *PAppl)
{
  ATunprotect((ATerm *) PAppl);
}
#endif

#ifndef ATunprotectList
inline void ATunprotectList(ATermList *PList)
{
  ATunprotect((ATerm *) PList);
}
#endif

#ifndef ATunprotectInt
inline void ATunprotectInt(ATermInt *PInt)
{
  ATunprotect((ATerm *) PInt);
}
#endif

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
ATermAppl gsMakeSubst(ATerm old_value, ATerm new_value);

/**
 * \brief Creates a new substitution
 *
 * \param[in] old_value the Aterm that to replace
 * \param[in] new_value the Aterm to replace with
 *
 * \return a substitution, i.e. an ATermAppl of the form 'subst(old_value, new_value)'
 * \note ATermAppl variant of gsMakeSubst
 **/
inline ATermAppl gsMakeSubst_Appl(ATermAppl old_value, ATermAppl new_value) {
  return gsMakeSubst((ATerm) old_value, (ATerm) new_value);
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
inline ATermAppl gsMakeSubst_List(ATermList old_value, ATermList new_value) {
  return gsMakeSubst((ATerm) old_value, (ATerm) new_value);
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
ATerm gsSubstValues(ATermList substs, ATerm term, bool recursive);

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
inline ATermAppl gsSubstValues_Appl(ATermList substs, ATermAppl appl, bool recursive) {
  return (ATermAppl) gsSubstValues(substs, (ATerm) appl, recursive);
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
inline ATermList gsSubstValues_List(ATermList substs, ATermList list, bool recursive) {
  return (ATermList) gsSubstValues(substs, (ATerm) list, recursive);
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
ATerm gsSubstValuesTable(ATermTable substs, ATerm term, bool recursive);

/**
 * \brief Adds a substitution to a list of substitutions
 *
 * \param[in] subst a substitution specification
 * \param[in] substs a list of substitutions
 * \pre subst is a substitution
 *      substs is a list of substitions
 * \return a list of substitutions with:
 *     - subst as the head
 *     - substs, in which subst is performed on the RHS's, as the tail
 **/
ATermList gsAddSubstToSubsts(ATermAppl subst, ATermList substs);

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
bool gsOccurs(ATerm elt, ATerm term);

/** \brief Counts the number of times a symbol occurs as head of a subterm.
 * \param[in] elt a term of which to count instances
 * \param[in] term a term to search inside
 * \pre term is an ATerm containing ATermAppl's and ATermList's only
 * \return the number of times elt occurs in term
 **/
int gsCount(ATerm elt, ATerm term);

/**
 * \brief Counts the number of times a symbol occurs as head of a subterm.
 * \param[in] fun a symbol
 * \param[in] term a term
 * \pre term is an ATerm containing ATermAppl's and ATermList's only
 * \return the number of fun occurs in term (as an AFun)
 **/
int gsCountAFun(AFun fun, ATerm term);
#ifdef __cplusplus
    }
  }
}
#endif

#endif
