#include <stdbool.h>
#include <stdarg.h>
#include "aterm2.h"

#ifdef __cplusplus
extern "C" {
#endif


//Global precondition: the ATerm library has been initialised

//String manipulation
//-------------------
//
//Re-implementation of strdup (because it is not part of the C99 standard)
#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
extern char *strdup(const char *s);
#endif

//Exception handling macro's
//--------------------------
//
//When these are used the label 'finally' should be declared.
//If a value 'x' should be returned, the variable 'result' has to be declared, 
//and the type of 'x' should be convertible to that of 'result'.
//If a message, should be printed, the message has to be able to be used by
//function printf.

#define throw             goto finally
//model C++ throw by a 'goto finally' statement

#define ThrowV(x)         Result = x; throw
//store x in result and throw an exception

#define ThrowM(...)       gsErrorMsg(__VA_ARGS__); throw
//print error message supplied by the first parameter with the remaining
//parameters as arguments

#define ThrowVM(x, ...)   gsErrorMsg(__VA_ARGS__); ThrowV(x)
//print error message supplied by the first parameter with the remaining
//parameters as arguments, and throw an exception with value x

//Message printing
//----------------

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

inline void gsErrorMsg(char *Format, ...);
//Post: "error: " is printed to stderr followed by Format, where the remaining
//      parameters are used as ATprintf arguments to Format.

inline void gsWarningMsg(char *Format, ...);
//Post: If the printing of warning messages is enabled, "warning: " is printed
//      to stderr followed by Format, where the remaining parameters are used
//      as ATprintf arguments to Format.

inline void gsVerboseMsg(char *Format, ...);
//Post: If the printing of verbose information is enabled, Format is printed to
//      stderr, where the remaining parameters are used as ATprintf arguments
//      to Format.

#define gsDebugMsg(...)   gsDebugMsgFunc(__func__, __VA_ARGS__)
//Post: If the printing of debug messages is enabled, the name of the current
//      function is printed to stderr, followed by the first parameter with the
//      remaining parameters as ATprintf arguments.

inline void gsDebugMsgFunc(const char *FuncName, char *Format, ...);
//Post: If the printing of debug messages is enabled, the name of FuncName is
//      printed to stderr, followed by Format where  the remaining parameters
//      are used as ATprintf arguments to Format.

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
void      ATprotectAppl(ATermAppl *PAppl);
void      ATprotectList(ATermAppl *PList);
void      ATunprotectAppl(ATermAppl *PAppl);
void      ATunprotectList(ATermAppl *PList);

//Substitutions on ATerm's
//------------------------

ATermAppl gsMakeSubst(ATermAppl OldValue, ATermAppl NewValue);
//Ret: a substitution, i.e. an ATermAppl of the form 'subst(OldValue, NewValue)'

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
//     - Substs, in which Subst is performed, as the tail

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
