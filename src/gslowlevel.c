#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>

#include "gslowlevel.h"
#include "libgsparse.h"

//String manipulation
//-------------------

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
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

int gsprintf(const char *format, ...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = gsvfprintf(stdout, format, args);
  va_end(args);

  return result;
}

int gsfprintf(FILE *stream, const char *format, ...)
{
  int             result = 0;
  va_list         args;

  va_start(args, format);
  result = gsvfprintf(stream, format, args);
  va_end(args);

  return result;
}

int gsvfprintf(FILE *stream, const char *format, va_list args)
{
  //code copied from the ATerm library in which '%T' is added to the format to
  //enable pretty printing of ATerm's

  const char     *p;
  char           *s;
  char            fmt[16];
  int             result = 0;
  ATerm           t;
  ATermList       l;

  for (p = format; *p; p++)
  {
    if (*p != '%')
    {
      fputc(*p, stream);
      continue;
    }

    s = fmt;
    while (!isalpha((int) *p))	/* parse formats %-20s, etc. */
      *s++ = *p++;
    *s++ = *p;
    *s = '\0';

    switch (*p)
    {
      case 'c':
      case 'd':
      case 'i':
      case 'o':
      case 'u':
      case 'x':
      case 'X':
	fprintf(stream, fmt, va_arg(args, int));
	break;

      case 'e':
      case 'E':
      case 'f':
      case 'g':
      case 'G':
	fprintf(stream, fmt, va_arg(args, double));
	break;

      case 'p':
	fprintf(stream, fmt, va_arg(args, void *));
	break;

      case 's':
	fprintf(stream, fmt, va_arg(args, char *));
	break;

	/*
	 * MCRL2 specifics start here: "%T" to prettiprint an ATerm
	 */
      case 'T':
	PrettyPrint(stream, va_arg(args, ATerm));
	break;
	/*
	 * ATerm specifics start here: "%t" to print an ATerm; "%l" to
	 * print a list; "%y" to print a Symbol; "%n" to print a single
	 * ATerm node
	 */
      case 't':
	ATwriteToTextFile(va_arg(args, ATerm), stream);
	break;
      case 'l':
	l = va_arg(args, ATermList);
	fmt[strlen(fmt) - 1] = '\0';	/* Remove 'l' */
	while (!ATisEmpty(l))
	{
	  ATwriteToTextFile(ATgetFirst(l), stream);
	  /*
	   * ATfprintf(stream, "\nlist node: %n\n", l);
	   * ATfprintf(stream, "\nlist element: %n\n", ATgetFirst(l));
	   */
	  l = ATgetNext(l);
	  if (!ATisEmpty(l))
	    fputs(fmt + 1, stream);
	}
	break;
      case 'a':
      case 'y':
	AT_printSymbol(va_arg(args, Symbol), stream);
	break;
      case 'n':
	t = va_arg(args, ATerm);
	switch (ATgetType(t))
	{
	  case AT_INT:
	  case AT_REAL:
	  case AT_BLOB:
	    ATwriteToTextFile(t, stream);
	    break;

	  case AT_PLACEHOLDER:
	    fprintf(stream, "<...>");
	    break;

	  case AT_LIST:
	    fprintf(stream, "[...(%d)]", ATgetLength((ATermList) t));
	    break;

	  case AT_APPL:
	    if (AT_isValidSymbol(ATgetAFun(t))) {
	      AT_printSymbol(ATgetAFun(t), stream);
	      fprintf(stream, "(...(%d))",
		      GET_ARITY(t->header));
	    } else {
	      fprintf(stream, "<sym>(...(%d))",
		      GET_ARITY(t->header));
	    }
	    if (HAS_ANNO(t->header)) {
	      fprintf(stream, "{}");
	    }
	    break;
	  case AT_FREE:
	    fprintf(stream, "@");
	    break;
	  default:
	    fprintf(stream, "#");
	    break;
	}
	break;

      case 'h':
	{
	  unsigned char *digest = ATchecksum(va_arg(args, ATerm));
	  int i;
	  for (i=0; i<16; i++) {
	    fprintf(stream, "%02x", digest[i]);
	  }
	}
	break;


      default:
	fputc(*p, stream);
	break;
    }
  }
  return result;
}

bool gsWarning = true; //indicates if warning should be printed
bool gsVerbose = false;//indicates if verbose messages should be printed
bool gsDebug   = false;//indicates if debug messages should be printed

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

#ifndef ATprotectAppl
void ATprotectAppl(ATermAppl *PAppl)
{
  ATprotect((ATerm *) PAppl);
}
#endif

#ifndef ATprotectList
void ATprotectList(ATermList *PList)
{
  ATprotect((ATerm *) PList);
}
#endif

#ifndef ATprotectInt
void ATprotectInt(ATermInt *PInt)
{
  ATprotect((ATerm *) PInt);
}
#endif

#ifndef ATunprotectAppl
void ATunprotectAppl(ATermAppl *PAppl)
{
  ATunprotect((ATerm *) PAppl);
}
#endif

#ifndef ATunprotectList
void ATunprotectList(ATermList *PList)
{
  ATunprotect((ATerm *) PList);
}
#endif

#ifndef ATunprotectInt
void ATunprotectInt(ATermInt *PInt)
{
  ATunprotect((ATerm *) PInt);
}
#endif

//Substitutions on ATerm's
//------------------------

static AFun substafun;
static bool substafun_notset = true;
ATermAppl gsMakeSubst(ATerm OldValue, ATerm NewValue)
{
  if ( substafun_notset )
  {
	  substafun = ATmakeAFun("subst", 2, ATfalse);
	  ATprotectAFun(substafun);
	  substafun_notset = false;
  }
  return ATmakeAppl2(substafun, OldValue, NewValue);
}

ATermAppl gsMakeSubst_Appl(ATermAppl OldValue, ATermAppl NewValue)
{
  return gsMakeSubst((ATerm) OldValue, (ATerm) NewValue);
}

ATermAppl gsMakeSubst_List(ATermList OldValue, ATermList NewValue)
{
  return gsMakeSubst((ATerm) OldValue, (ATerm) NewValue);
}

ATerm gsSubstValues(ATermList Substs, ATerm Term, bool Recursive)
{
  ATermList l = Substs;
  while (!ATisEmpty(l)) {
    ATermAppl Subst = ATAgetFirst(l);
    if (ATisEqual(ATgetArgument(Subst, 0), Term))
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
      	DECL_A(Args,ATerm,NrArgs);
        for (int i = 0; i < NrArgs; i++) {
          Args[i] = gsSubstValues(Substs, ATgetArgument((ATermAppl) Term, i),
            Recursive);
        }
        ATerm a = (ATerm) ATmakeApplArray(Head, Args);
        FREE_A(Args);
        return a;
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
      return Term;
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
  //add Subst to Substs in which Subst is performed on the RHS's
  ATermList Result = ATmakeList0();
  while (!ATisEmpty(Substs)) {
    ATermAppl SubstsElt = ATAgetFirst(Substs);
    Result = ATinsert(Result, (ATerm) ATsetArgument(SubstsElt,
      (ATerm) gsSubstValues(ATmakeList1((ATerm) Subst),
        ATgetArgument(SubstsElt, 1) , true), 1));
    Substs = ATgetNext(Substs);
  }
  return ATinsert(ATreverse(Result), (ATerm) Subst);
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
    } else if (ATgetType(Term) == AT_LIST) {
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
    return ceil(log10((double) n));
  else if (n == 0)
    return 1;
  else //n < 0
    return ceil(log10((double) abs(n))) + 1;
}

#ifdef __cplusplus
}
#endif
