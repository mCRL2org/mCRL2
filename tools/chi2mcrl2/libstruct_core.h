// Author(s): Aad Mathijssen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libstruct_core.h

#ifndef MCRL2_LIBSTRUCT_CORE_H
#define MCRL2_LIBSTRUCT_CORE_H

//This file describes the functions that can be used for the internal ATerm
//structure.

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <aterm2.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

//Global precondition: the ATerm library has been initialised

// TODO: remove this function.
inline
void gsEnableCoreConstructorFunctions()
{}

//--- begin generated code
// DataVarID
inline
AFun initAFunDataVarID(AFun& f)
{
  f = ATmakeAFun("DataVarID", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunDataVarID()
{
  static AFun AFunDataVarID = initAFunDataVarID(AFunDataVarID);
  return AFunDataVarID;
}

inline
bool gsIsDataVarID(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunDataVarID();
}

// Nil
inline
AFun initAFunNil(AFun& f)
{
  f = ATmakeAFun("Nil", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunNil()
{
  static AFun AFunNil = initAFunNil(AFunNil);
  return AFunNil;
}

inline
bool gsIsNil(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunNil();
}

// ProcSpec
inline
AFun initAFunProcSpec(AFun& f)
{
  f = ATmakeAFun("ProcSpec", 2, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunProcSpec()
{
  static AFun AFunProcSpec = initAFunProcSpec(AFunProcSpec);
  return AFunProcSpec;
}

inline
bool gsIsProcSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunProcSpec();
}

// Skip
inline
AFun initAFunSkip(AFun& f)
{
  f = ATmakeAFun("Skip", 0, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunSkip()
{
  static AFun AFunSkip = initAFunSkip(AFunSkip);
  return AFunSkip;
}

inline
bool gsIsSkip(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunSkip();
}

// Type
inline
AFun initAFunType(AFun& f)
{
  f = ATmakeAFun("Type", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunType()
{
  static AFun AFunType = initAFunType(AFunType);
  return AFunType;
}

inline
bool gsIsType(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunType();
}

// VarSpec
inline
AFun initAFunVarSpec(AFun& f)
{
  f = ATmakeAFun("VarSpec", 1, ATfalse);
  ATprotectAFun(f);
  return f;
}

inline
AFun gsAFunVarSpec()
{
  static AFun AFunVarSpec = initAFunVarSpec(AFunVarSpec);
  return AFunVarSpec;
}

inline
bool gsIsVarSpec(ATermAppl Term)
{
  return ATgetAFun(Term) == gsAFunVarSpec();
}

inline
ATermAppl gsMakeDataVarID(ATermAppl String_0, ATermAppl TypeExp_1)
{
  return ATmakeAppl2(gsAFunDataVarID(), (ATerm) String_0, (ATerm) TypeExp_1);
}

inline
ATermAppl gsMakeNil()
{
  return ATmakeAppl0(gsAFunNil());
}

inline
ATermAppl gsMakeProcSpec(ATermAppl VarSpec_0, ATermAppl StatementSpec_1)
{
  return ATmakeAppl2(gsAFunProcSpec(), (ATerm) VarSpec_0, (ATerm) StatementSpec_1);
}

inline
ATermAppl gsMakeSkip()
{
  return ATmakeAppl0(gsAFunSkip());
}

inline
ATermAppl gsMakeType(ATermAppl String_0)
{
  return ATmakeAppl1(gsAFunType(), (ATerm) String_0);
}

inline
ATermAppl gsMakeVarSpec(ATermList VarID_0)
{
  return ATmakeAppl1(gsAFunVarSpec(), (ATerm) VarID_0);
}
//--- end generated code

//Conversion between strings and quoted ATermAppl's
//-------------------------------------------------

inline
ATermAppl gsString2ATermAppl(const char *s)
//Ret: quoted constant s, if s != NULL
//     unquoted constant Nil, if s == NULL
{
  if (s != NULL) {
    return ATmakeAppl0(ATmakeAFun(s, 0, ATtrue));
  } else {
    return gsMakeNil();   
  }
}

inline
bool gsIsString(ATermAppl term)
//Ret: term is a quoted constant
{
  AFun head = ATgetAFun(term);
  return ((ATgetArity(head) == 0) && (ATisQuoted(head) == ATtrue));
}

inline
char *gsATermAppl2String(ATermAppl term)
//Ret: string s, if term is a quoted constant s
//     NULL, otherwise
{
  if (gsIsString(term)) {
    return ATgetName(ATgetAFun(term));
  } else {
    return NULL;
  }
}

inline
bool gsIsNumericString(const char* s)
//Ret: true if s is of form "0 | -? [1-9][0-9]*", false otherwise
{
  if (s == NULL) return false;
  if (s[0] == '\0') return false;
  if (s[0] == '-') ++s;
  if (s[0] == '\0') return false;
  if (s[0] == '0') {
    ++s;
    if (s[0] == '\0') return true;
    else return false;
  }
  for (; s[0] != '\0'; ++s)
    if(!isdigit(s[0])) return false;
  return true;
}

ATermAppl gsFreshString2ATermAppl(const char *s, ATerm Term, bool TryNoSuffix);
//Pre: Term is an ATerm containing ATermAppl's and ATermList's only
//     s is not NULL
//Ret: "s", if it does not occur in Term, and TryNoSuffix holds
//     "sk" as a quoted ATermAppl constant, where k is the smallest natural
//     number such that "sk" does not occur in Term, otherwise

#ifdef __cplusplus
}
#endif

#endif // MCRL2_LIBSTRUCT_CORE_H
