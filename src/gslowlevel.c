#ifdef __cplusplus
extern "C" {
#endif
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#ifdef __cplusplus
}
#endif

#include "gslowlevel.h"

//String manipulation
//-------------------

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
char *strdup(const char *s)
{
    size_t len;
    char *p;

    len = strlen(s);
    if((p = (char *)malloc(len + 1)) == NULL)
      return NULL;
    return strcpy(p, s);
}
#endif

//Message printing
//----------------

static bool gsWarning = true; //indicates if warning should be printed
static bool gsVerbose = false;//indicates if verbose messages should be printed
static bool gsDebug   = false;//indicates if debug messages should be printed

void gsSetQuietMsg(void)
{
  gsWarning = false;
  gsVerbose = false;
  gsDebug   = false;
}

void gsSetNormalMsg(void)
{
  gsWarning = true;
  gsVerbose = false;
  gsDebug   = false;
}

void gsSetVerboseMsg(void)
{
  gsWarning = true;
  gsVerbose = true;
  gsDebug   = false;
}

void gsSetDebugMsg(void)
{
  gsWarning = true;
  gsVerbose = true;
  gsDebug   = true;
}

void gsErrorMsg(char *Format, ...)
{
  fprintf(stderr, "error: ");
  va_list Args;
  va_start(Args, Format);
  ATvfprintf(stderr, Format, Args);
  va_end(Args);
}

void gsWarningMsg(char *Format, ...)
{
  if (gsWarning) {
    fprintf(stderr, "warning: ");
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

void gsVerboseMsg(char *Format, ...)
{
  if (gsVerbose) {
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

void gsDebugMsgFunc(const char *FuncName, char *Format, ...)
{
  if (gsDebug) {
    fprintf(stderr, "(%s): ", FuncName);
    va_list Args;
    va_start(Args, Format);
    ATvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

//ATerm libary work arounds
//-------------------------

bool      ATisApplOrNull(ATerm t)
//Ret: t is NULL or an ATermAppl
{
  if (t == NULL) return true;
  else return ATgetType(t) == AT_APPL;
}

bool      ATisListOrNull(ATerm t)
//Ret: t is NULL or an ATermList
{
  if (t == NULL) return true;
  else return ATgetType(t) == AT_LIST;
}

ATermAppl ATAelementAt(ATermList List, int Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

ATermList ATLelementAt(ATermList List, int Index)
{
  ATerm Result = ATelementAt(List, Index);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

ATermAppl ATAgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

ATermList ATLgetArgument(ATermAppl Appl, int Nr)
{
  ATerm Result = ATgetArgument(Appl, Nr);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

ATermAppl ATAgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

ATermList ATLgetFirst(ATermList List)
{
  ATerm Result = ATgetFirst(List);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

ATermAppl ATAtableGet(ATermTable Table, ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisApplOrNull(Result));
  return (ATermAppl) Result;
}

ATermList ATLtableGet(ATermTable Table, ATerm Key)
{
  ATerm Result = ATtableGet(Table, Key);
  assert(ATisListOrNull(Result));
  return (ATermList) Result;
}

void ATprotectAppl(ATermAppl *PAppl)
{
  ATprotect((ATerm *) PAppl);
}

void ATprotectList(ATermList *PList)
{
  ATprotect((ATerm *) PList);
}

void ATunprotectAppl(ATermAppl *PAppl)
{
  ATunprotect((ATerm *) PAppl);
}

void ATunprotectList(ATermList *PList)
{
  ATunprotect((ATerm *) PList);
}

//Substitutions on ATerm's
//------------------------

ATermAppl gsMakeSubst(ATermAppl OldValue, ATermAppl NewValue)
{
  return ATmakeAppl2(ATmakeAFun("subst", 2, ATfalse),
    (ATerm) OldValue, (ATerm) NewValue);
}

ATerm gsSubstValues(ATermList Substs, ATerm Term, bool Recursive)
{
  ATermList l = Substs;
  while (!ATisEmpty(l)) {
    ATermAppl Subst = ATAgetFirst(l);
    ATerm OldValue = ATgetArgument(Subst, 0);
    if (ATisEqual(OldValue, Term))
    {
      Term = ATgetArgument(Subst, 1);
      Recursive = false;
    }
    l = ATgetNext(l);
  }
  if (!Recursive) {
    return Term;
  } else {
    //Recursive; distribute substitutions over the arguments/elements of Term
    if (ATgetType(Term) == AT_APPL) {
      //Term is an ATermAppl; distribute substitutions over the arguments
      AFun Head = ATgetAFun((ATermAppl) Term);
      int NrArgs = ATgetArity(Head);
      if (NrArgs > 0) {
        ATerm Args[NrArgs];
        for (int i = 0; i < NrArgs; i++) {
          Args[i] = gsSubstValues(Substs, ATgetArgument((ATermAppl) Term, i),
            Recursive);
        }
        return (ATerm) ATmakeApplArray(Head, Args);
      } else {
        return Term;
      }
    } else if (ATgetType(Term) == AT_LIST) {
      //Term is an ATermList; distribute substitutions over the elements
      ATermList Result = ATmakeList0();
      while (!ATisEmpty((ATermList) Term)) {
        Result = ATinsert(Result,
          gsSubstValues(Substs, ATgetFirst((ATermList) Term), Recursive));
        Term = (ATerm) ATgetNext((ATermList) Term);
      }
      return (ATerm) ATreverse(Result);
    } else {
      return NULL;
    }
  }
}

ATermAppl gsSubstValues_Appl(ATermList Substs, ATermAppl Appl, bool Recursive)
{
  return (ATermAppl) gsSubstValues(Substs, (ATerm) Appl, Recursive);
}

ATermList gsSubstValues_List(ATermList Substs, ATermList List, bool Recursive)
{
  return (ATermList) gsSubstValues(Substs, (ATerm) List, Recursive);
}

ATermList gsAddSubstToSubsts(ATermAppl Subst, ATermList Substs)
{
  return ATinsert(
    gsSubstValues_List(ATmakeList1((ATerm) Subst), Substs, true),
    (ATerm) Subst);
}

//Occurrences of ATerm's
//----------------------

bool gsOccurs(ATerm Elt, ATerm Term)
{
  bool Result = false;
  if (ATisEqual(Elt, Term)) {
    Result = true;
  } else {
    //check occurrences of Elt in the arguments/elements of Term
    if (ATgetType(Term) == AT_APPL) {
      AFun Head = ATgetAFun((ATermAppl) Term);
      int NrArgs = ATgetArity(Head);
      for (int i = 0; i < NrArgs && !Result; i++) {
        Result = gsOccurs(Elt, ATgetArgument((ATermAppl) Term, i));
      }
    } else { //ATgetType(Term) == AT_LIST
      while (!ATisEmpty((ATermList) Term) && !Result)
      {
        Result = gsOccurs(Elt, ATgetFirst((ATermList) Term));
        Term = (ATerm) ATgetNext((ATermList) Term);
      }
    }
  }
  return Result;
}

//String representations of numbers
//---------------------------------

inline int gsChar2Int(char c)
//Pre: '0' <= c < '9'
//Ret: integer value corresponding to c
{
  assert(c >= '0' && c <= '9');
  return c - '0';
}

inline char gsInt2Char(int n)
//Pre: 0 <= n <= 9
//Ret: character corresponding to the value of n
{
  assert(n >= 0 && n <= 9);
  return n + '0';
}

char *gsStringDiv2(const char *n)
{
  assert(strlen(n) > 0);
  int l = strlen(n); //length of n
  char *r = (char *) malloc((l+1) * sizeof(char)); //result char*
  //calculate r[0]
  r[0] = gsInt2Char(gsChar2Int(n[0])/2);
  //declare counter for the elements of r
  int j = (r[0] == '0')?0:1;
  //calculate remaining indices of r
  for (int i=1; i<l; i++)
  {
    //r[j] = 5*(n[i-1] mod 2) + n[i] div 2
    r[j] = gsInt2Char(5*(gsChar2Int(n[i-1])%2) + gsChar2Int(n[i])/2);
    //update j
    j = j+1;
  }
  //terminate string
  r[j] = '\0';
  return r;
}

int gsStringMod2(const char *n)
{
  assert(strlen(n) > 0);
  return gsChar2Int(n[strlen(n)-1]) % 2;
}

char *gsStringDub(const char *n, const int inc)
{
  assert(strlen(n) > 0);
  int l = strlen(n); //length of n
  char *r = (char *) malloc((l+2) * sizeof(char)); //result char*
  //calculate r[0]: n[0] div 5
  r[0] = gsInt2Char(gsChar2Int(n[0])/5);
  //declare counter for the elements of r
  int j = (r[0] == '0')?0:1;
  //calculate remaining indices of r
  for (int i=0; i<l-1; i++)
  {
    //r[j] = 2*(n[i-1] mod 5) + n[i] div 5
    r[j] = gsInt2Char(2*(gsChar2Int(n[i])%5) + gsChar2Int(n[i+1])/5);
    //update j
    j = j+1;
  }
  //calculate last index of r
  r[j] = gsInt2Char(2*(gsChar2Int(n[l-1])%5) + inc);
  j = j+1;
  //terminate string
  r[j] = '\0';
  return r;
}

int NrOfChars(int n)
{
  if (n > 0)
    return ceil(log10(n));
  else if (n == 0)
    return 1;
  else //n < 0
    return ceil(log10(abs(n))) + 1;
}
