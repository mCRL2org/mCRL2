#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stdarg.h>
#include <assert.h>
#include <aterm2.h>

#include <workarounds.h>

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

//Message printing options
//------------------------

void gsSetQuietMsg(void);
//Post: Printing of warnings, verbose information and extended debugging
//      information during program execution is disabled.

void gsSetNormalMsg(void);
//Post: Printing of warnings during program execution is enabled. Printing of
//      verbose information and extended debugging information is disabled.

void gsSetVerboseMsg(void);
//Post: Printing of warnings and verbose information during program execution
//      is enabled. Printing of extended debugging information is disabled.

void gsSetDebugMsg(void);
//Post: Printing of warnings, verbose information and extended debugging
//      information during program execution is enabled.

extern bool gsQuiet;
extern bool gsWarning;
extern bool gsVerbose;
extern bool gsDebug;

// Type for message distinction (by purpose)
enum gsMessageType {gs_notice, gs_warning, gs_error};

// Replaces message_handler by the function pointer passed as argument
void gsSetCustomMessageHandler(void (*)(gsMessageType, char*));

//ATerm library workarounds
//--------------------------
//
//To eliminate downcasts in the rest of the code, we introduce wrappers around
//ATerm library functions.
//This is caused by a bad interface design of the ATerm library.

ATermAppl ATAelementAt(ATermList List, int Index);
ATermList ATLelementAt(ATermList List, int Index);
ATermAppl ATAgetArgument(ATermAppl Appl, int Nr);
ATermList ATLgetArgument(ATermAppl Appl, int Nr);
ATermAppl ATAgetFirst(ATermList List);
ATermList ATLgetFirst(ATermList List);
ATermAppl ATAtableGet(ATermTable Table, ATerm Key);
ATermList ATLtableGet(ATermTable Table, ATerm Key);
#ifndef ATprotectAppl
void      ATprotectAppl(ATermAppl *PAppl);
#endif
#ifndef ATprotectList
void      ATprotectList(ATermList *PList);
#endif
#ifndef ATprotectInt
void      ATprotectInt(ATermInt *PInt);
#endif
#ifndef ATunprotectAppl
void      ATunprotectAppl(ATermAppl *PAppl);
#endif
#ifndef ATunprotectList
void      ATunprotectList(ATermList *PList);
#endif
#ifndef ATunprotectInt
void      ATunprotectInt(ATermInt *PInt);
#endif

inline ATermList ATinsertUnique(ATermList list, ATerm el)
//Ret: el ++ list if not el in list,
//     list if el in list
{
  if (ATindexOf(list, el, 0) == -1) return ATinsert(list, el);
  else return list;
}

inline bool ATisApplOrNull(ATerm t)
//Ret: t is NULL or an ATermAppl
{
  if (t == NULL) return true;
  else return ATgetType(t) == AT_APPL;
}

inline bool ATisListOrNull(ATerm t)
//Ret: t is NULL or an ATermList
{
  if (t == NULL) return true;
  else return ATgetType(t) == AT_LIST;
}

inline ATermAppl ATAelementAt(ATermList List, int Index) {
  ATerm Result = ATelementAt(List, Index);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

inline ATermList ATLelementAt(ATermList List, int Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

inline ATermAppl ATAgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

inline ATermList ATLgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

inline ATermAppl ATAgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

inline ATermList ATLgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

inline ATermAppl ATAtableGet(ATermTable Table, ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

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

ATermAppl gsMakeSubst(ATerm OldValue, ATerm NewValue);
//Ret: a substitution, i.e. an ATermAppl of the form 'subst(OldValue, NewValue)'

//ATermAppl variant of gsMakeSubst
inline ATermAppl gsMakeSubst_Appl(ATermAppl OldValue, ATermAppl NewValue) {
  return gsMakeSubst((ATerm) OldValue, (ATerm) NewValue);
}

//ATermList variant of gsMakeSubst
inline ATermAppl gsMakeSubst_List(ATermList OldValue, ATermList NewValue) {
  return gsMakeSubst((ATerm) OldValue, (ATerm) NewValue);
}

ATerm gsSubstValues(ATermList Substs, ATerm Term, bool Recursive);
//Pre: Substs is a list containing substitutions only
//     Term is an ATerm containing ATermAppl's and ATermList's only
//Ret: Term, in which all substitutions are applied to the top level of Term,
//     from head to tail; if Recursive and there was no match, the
//     substitutions are distributed over the arguments/elements of Term

//ATermAppl variant of gsSubstValues
inline ATermAppl gsSubstValues_Appl(ATermList Substs, ATermAppl Appl, bool Recursive) {
  return (ATermAppl) gsSubstValues(Substs, (ATerm) Appl, Recursive);
}

//ATermList variant of gsSubstValues
inline ATermList gsSubstValues_List(ATermList Substs, ATermList List, bool Recursive) {
  return (ATermList) gsSubstValues(Substs, (ATerm) List, Recursive);
}

ATerm gsSubstValuesTable(ATermTable Substs, ATerm Term, bool Recursive);
//Pre: Substs is a table containing substitutions from ATerm's to ATerm's
//     Term is an ATerm consisting of ATermAppl's and ATermList's only
//Ret: Term in which all substitutions from Substs are performed recursively

ATermList gsAddSubstToSubsts(ATermAppl Subst, ATermList Substs);
//Pre: Subst is a substitution
//     Substs is a list of substitions
//Ret: a list of substitutions with:
//     - Subst as the head
//     - Substs, in which Subst is performed on the RHS's, as the tail

//Occurrences of ATerm's
//----------------------

bool gsOccurs(ATerm Elt, ATerm Term);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//Ret: Elt occurs in Term
//Note that this is a faster implementation than gsCount(Elt, Term) > 0 because
//it is used at a crucial point in the rewriter

int gsCount(ATerm Elt, ATerm Term);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//Ret: the number of times Elt occurs in Term

//String representations of numbers
//---------------------------------

char *gsStringDiv2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the smallest string representation of n div 2
//     Note that the result is created with malloc, so it has to be freed
     
int gsStringMod2(const char *n);
//Pre: n is of the form "0 | [1-9][0-9]*"
//Ret: the value of n mod 2

char *gsStringDub(const char *n, const int inc);
//Pre: n is of the form "0 | [1-9][0-9]*"
//     0 <= inc <= 1
//Ret: the smallest string representation of 2*n + inc,
//     Note that the result is created with malloc, so it has to be freed

int NrOfChars(int n);
//Ret: the number of characters of the decimal representation of n

#ifdef __cplusplus
}
#endif

#endif
