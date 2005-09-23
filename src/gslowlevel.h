#ifndef __LOWLEVEL_H
#define __LOWLEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdarg.h>
#include <aterm2.h>

#ifdef _MSC_VER
#include <nt.h>
#endif

//Global precondition: the ATerm library has been initialised

//String manipulation
//-------------------
//
//Re-implementation of strdup (because it is not part of the C99 standard)
#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED || defined __APPLE__ || defined _MSC_VER)
extern char *strdup(const char *s);
#endif

//General definitions
//-------------------
//

//Declare a local array NAME of type TYPE and SIZE elements (where SIZE
//is not a constant value)
#ifdef _MSC_VER
#define DECL_A(NAME,TYPE,SIZE)  TYPE *NAME = (TYPE *) malloc((SIZE)*sizeof(TYPE))
#define FREE_A(NAME)            free(NAME);
#else
#define DECL_A(NAME,TYPE,SIZE)  TYPE NAME[SIZE]
#define FREE_A(NAME)            
#endif

//Make sure __func__ works (as well as possible)
#ifndef __func__
#ifdef __FUNCTION__
#define __func__ __FILE__
#else
#define __func__ __FILE__
#endif
#endif

//Message printing
//----------------

int gsprintf(const char *format, ...);
int gsfprintf(FILE *stream, const char *format, ...);
int gsvfprintf(FILE *stream, const char *format, va_list args);
//Pretty print versions of the printf functions
//If '%P' is used in the format, the corresponding ATerm argument is pretty
//printed
//If '%T' is used in the format, the corresponding ATerm argument is printed
//in the ATerm text format 
//If '%F' is used in the format, the name of the corresponding AFun argument
//is printed

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
//      information during program executation is enabled.

extern bool gsWarning;
extern bool gsVerbose;
extern bool gsDebug;

inline static void gsErrorMsg(char *Format, ...)
//Post: "error: " is printed to stderr followed by Format, where the remaining
//      parameters are used as gsprintf arguments to Format.
{
  fprintf(stderr, "error: ");
  va_list Args;
  va_start(Args, Format);
  gsvfprintf(stderr, Format, Args);
  va_end(Args);
}

inline static void gsWarningMsg(char *Format, ...)
//Post: If the printing of warning messages is enabled, "warning: " is printed
//      to stderr followed by Format, where the remaining parameters are used
//      as gsprintf arguments to Format.
{
  if (gsWarning) {
    fprintf(stderr, "warning: ");
    va_list Args;
    va_start(Args, Format);
    gsvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

inline static void gsVerboseMsg(char *Format, ...)
//Post: If the printing of verbose information is enabled, Format is printed to
//      stderr, where the remaining parameters are used as gsprintf arguments
//      to Format.
{
  if (gsVerbose) {
    va_list Args;
    va_start(Args, Format);
    gsvfprintf(stderr, Format, Args);
    va_end(Args);
  }
}

#define GS_DEBUG_MSG_FUNC(FuncName,Format) \
  if (gsDebug) { \
    fprintf(stderr, "(%s): ", FuncName); \
    va_list Args; \
    va_start(Args, Format); \
    gsvfprintf(stderr, Format, Args); \
    va_end(Args); \
  }

#ifdef _MSC_VER
inline static void gsDebugMsg(char *Format,...)
{
	GS_DEBUG_MSG_FUNC("unknown",Format)
}
#else
#define gsDebugMsg(...)        gsDebugMsgFunc(__func__, __VA_ARGS__)
#endif
//Post: If the printing of debug messages is enabled, the name of the current
//      function is printed to stderr, followed by the first parameter with the
//      remaining parameters as gsprintf arguments.

inline static void gsDebugMsgFunc(const char *FuncName, char *Format, ...)
//Post: If the printing of debug messages is enabled, the name of FuncName is
//      printed to stderr, followed by Format where  the remaining parameters
//      are used as gsprintf arguments to Format.
{
  GS_DEBUG_MSG_FUNC(FuncName,Format)
}

//ATerm library work arounds
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

//Substitutions on ATerm's
//------------------------

ATermAppl gsMakeSubst(ATerm OldValue, ATerm NewValue);
//Ret: a substitution, i.e. an ATermAppl of the form 'subst(OldValue, NewValue)'

ATermAppl gsMakeSubst_Appl(ATermAppl OldValue, ATermAppl NewValue);
//ATermAppl variant of gsMakeSubst

ATermAppl gsMakeSubst_List(ATermList OldValue, ATermList NewValue);
//ATermList variant of gsMakeSubst

ATerm gsSubstValues(ATermList Substs, ATerm Term, bool Recursive);
//Pre: Substs is a list containing substitutions only
//     Term is an ATerm containing ATermAppl's and ATermList's only
//Ret: Term, in which all substitutions are applied to the top level of Term,
//     from head to tail; if Recursive and there was no match, the
//     substitutions are distributed over the arguments/elements of Term

ATermAppl gsSubstValues_Appl(ATermList Substs, ATermAppl Appl,
  bool Recursive);
//ATermAppl variant of gsSubstValues

ATermList gsSubstValues_List(ATermList Substs, ATermList List,
  bool Recursive);
//ATermList variant of gsSubstValues

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
