#include <stdbool.h>
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

#define throw                      goto finally
//model C++ throw by a 'goto finally' statement

#define ThrowV(x)                  Result = x; throw
//store x in result and throw an exception

#define ThrowM0(s)                 fprintf(stderr, s); throw
//print message s and throw an exception

#define ThrowVM0(x, s)             fprintf(stderr, s); ThrowV(x)
//print message s and throw an exception with value x

#define ThrowVM1(x, s, a1)         fprintf(stderr, s, a1); ThrowV(x)
//print message s with argument a1, and throw an exception with value x

#define ThrowVM2(x, s, a1, a2)     fprintf(stderr, s, a1, a2); ThrowV(x)
//print message s with argument a1 and a2,  and throw an exception with value x

#define ThrowVM3(x, s, a1, a2, a3) fprintf(stderr, s, a1, a2, a3); ThrowV(x)
//print message s with argument a1, a2 and a3,  and throw an exception with
//value x

//ATmake extensions
//-----------------
//
//Implements the making of all elements of the structure of ATerms to improve 
//readability and to minimise type casts in the rest of the code

ATermAppl gsMakeSpec(ATermList SpecElts);
ATermAppl gsMakeSortSpec(ATermList SortDecls);

ATermAppl gsMakeName(char *name);
ATermAppl gsMakeVar(ATermAppl vName, ATermAppl vSort);
ATermAppl gsMakeAct(ATermAppl action);
ATermAppl gsMakeT(void);
ATermAppl gsMakeF(void);
ATermAppl gsMakeNot(ATermAppl form);
ATermAppl gsMakeAnd(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeOr(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeImp(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeEq(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeForall(ATermAppl vName, ATermAppl vSort, ATermAppl form);
ATermAppl gsMakeExists(ATermAppl vName, ATermAppl vSort, ATermAppl form);
ATermAppl gsMakeNil(void);
ATermAppl gsMakeConcat(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeChoice(ATermAppl form0, ATermAppl form1);
ATermAppl gsMakeTrClose(ATermAppl form);
ATermAppl gsMakeTClose(ATermAppl form);
ATermAppl gsMakeFormRec(ATermAppl form);
ATermAppl gsMakeForm(ATermAppl form);
ATermAppl gsMakeRec(ATermAppl fpVO);
ATermAppl gsMakeMay(ATermAppl regFrm, ATermAppl modFrm);
ATermAppl gsMakeMust(ATermAppl regFrm, ATermAppl modFrm);
ATermAppl gsMakeLoop(ATermAppl form);
ATermAppl gsMakeMu(ATermAppl fpVar, ATermList vdList, ATermAppl modFrm, ATermList iList);
ATermAppl gsMakeNu(ATermAppl fpVar, ATermList vdList, ATermAppl modFrm, ATermList iList);

//strcmp extensions
//------------------
//
//Implements the comparison of the function names of all elements of the 
//structure of ATerms to improve readability

bool gsIsVar(char *s);
bool gsIsAct(char *s);
bool gsIsT(char *s);
bool gsIsF(char *s);
bool gsIsNot(char *s);
bool gsIsAnd(char *s);
bool gsIsOr(char *s);
bool gsIsImp(char *s);
bool gsIsEq(char *s);
bool gsIsForall(char *s);
bool gsIsExists(char *s);
bool gsIsNil(char *s);
bool gsIsConcat(char *s);
bool gsIsChoice(char *s);
bool gsIsTrClose(char *s);
bool gsIsTClose(char *s);
bool gsIsFormRec(char *s);
bool gsIsForm(char *s);
bool gsIsRec(char *s);
bool gsIsMay(char *s);
bool gsIsMust(char *s);
bool gsIsLoop(char *s);
bool gsIsMu(char *s);
bool gsIsNu(char *s);

//ATerm library work arounds
//--------------------------
//
//To eliminate ridiculous type casts in the rest of the code, we introducde
//wrappers around functions ATelementAt and ATgetArgument.
//This is caused by a bad interface design of the ATerm library

ATermAppl ATAelementAt(ATermList list, int index);
ATermList ATLelementAt(ATermList list, int index);
ATermInt  ATIelementAt(ATermList list, int index);
ATermAppl ATAgetArgument(ATermAppl appl, int nr);
ATermList ATLgetArgument(ATermAppl appl, int nr);
ATermInt  ATIgetArgument(ATermAppl appl, int nr);

#ifdef __cplusplus
}
#endif
