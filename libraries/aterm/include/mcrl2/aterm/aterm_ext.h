// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/aterm_ext.h
/// \brief Collection of convenience functions for ATerms

#ifndef __ATERM_EXT_H__
#define __ATERM_EXT_H__

#include <cstdarg>
#include <cassert>
#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

//Workarounds for the initialisation of the ATerm library
//-------------------------------------------------------

//To initialise the ATerm library either call MCRL2_ATERM_INIT or
//MCRL2_ATERM_INIT_DEBUG from the body of the main program as the first
//statement. Failing to do so may lead to crashes when the garbage collector
//is started.

// \cond INTERNAL_DOCS
#if defined(_MSC_VER) || defined(__MINGW32__)
# define MCRL2_ATERM_INIT_(bottom) \
  ATinit(reinterpret_cast< ATerm* >(&bottom));
#else
# define MCRL2_ATERM_INIT_(bottom) \
  ATinit(reinterpret_cast< ATerm* >(bottom));
#endif
// \endcond

/// MCRL2_ATERM_INIT(argv) initialises the ATerm library using
/// one of the parameters as the bottom of the stack. The parameter that is
/// actually depends on the platform:
/// - &argv on Windows platforms
/// - argv on non-Windows platforms
# define MCRL2_ATERM_INIT(argv) MCRL2_ATERM_INIT_(argv);

//-------------------------------------------------------------------------
//For all functions below we use the precondition the ATerm library has been
//initialised.
//-------------------------------------------------------------------------

//ATerm library workarounds
//--------------------------
//
//To eliminate downcasts in the rest of the code, we introduce wrappers around
//ATerm library functions.
//This is caused by a bad interface design of the ATerm library.

/**
 * \brief Conditional prepend operation on ATermList
 * \param[in] list an ATerm list
 * \param[in] el the aterm to prepend
 * \return el ++ list if not el in list, list if el in list
 **/
inline ATermList ATinsertUnique(const ATermList list, const ATerm el)
{
  if (ATindexOf(list, el, 0) == (size_t)(-1))
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
inline bool ATisAppl(const ATerm t)
{
  return ATgetType(t) == AT_APPL;
}

/**
 * \brief Condition on an ATerm
 * \param[in] t an ATerm
 * \return t is an ATermList
 */
inline bool ATisList(const ATerm t)
{
  return ATgetType(t) == AT_LIST;
}

/**
 * \brief Condition on an ATerm
 * \param[in] t an ATerm
 * \return t is an ATermInt
 */
inline bool ATisInt(const ATerm t)
{
  return ATgetType(t) == AT_INT;
}

/**
 * \brief Condition on an Aterm
 * \param[in] t an ATerm
 * \return t is NULL or an ATermAppl
 **/
inline bool ATisApplOrNull(const ATerm t)
{
  return (t == ATerm()) || ATisAppl(t);
}

/**
 * \brief Condition on an Aterm
 * \param[in] t an ATerm
 * \return t is NULL or an ATermList
 **/
inline bool ATisListOrNull(const ATerm t)
{
  return (t == ATerm()) || ATisList(t);
}

/**
 * \brief Gets an ATermAppl at a specified position in a list
 **/
inline ATermAppl ATAelementAt(const ATermList List, const size_t Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
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
inline ATermAppl ATAtableGet(const ATermTable Table, const ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

/**
 * \brief Gets the term associated with a key as ATermList
 **/
inline ATermList ATLtableGet(const ATermTable Table, const ATerm Key)
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
ATerm gsSubstValuesTable(const ATermTable substs, ATerm term, const bool recursive);

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

}

#endif
