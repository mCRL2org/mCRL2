#define  NAME      "libgsparse"
#define  LVERSION  "0.1.29"
#define  AUTHOR    "Aad Mathijssen"

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#ifdef __cplusplus
}
#endif

#include "libgsparse.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"
#include "gsfunc.h"

//external declarations
extern ATermAppl gsParse(FILE *SpecFile);/* declared in lexer.l */

//local declarations
ATermAppl gsLinearise(ATermAppl Spec);
/*Pre:Spec represents a specification that adheres to the internal ATerm
      structure after the data implementation phase.
 Post:The processes of spec are linearised.
 Ret: if the linearisation went ok, an equivalent version spec is
      returned that adheres to the internal ATerm structure after
      linearisation.
      if something went wrong, an appropriate error message is printed and
      NULL is returned.
 */

void gsPrintPart(FILE *OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATermAppl containing a part of a GenSpect specification
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of the part is written to OutStream. In this
       textual representation:
       - the top level symbol is parenthesized if PrecLevel is greater than its
         precedence level
       - sorts are shown for all elements, if ShowSorts is true;
         otherwise, sorts are only shown when necessary
*/

void gsPrintParts(FILE *OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator);
/*Pre: OutStream points to a stream to which can be written
       Parts is an ATermList containing parts of a GenSpect specification
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the parts
       0 <= PrecLevel
  Post:A textual representation of the parts is written to OutStream, in which:
       - PrecLevel and ShowSort are distributed over the parts
       - each part is terminated by Terminator, if it is not NULL
       - two successive parts are separated by Separator, if it is not NULL
*/

void gsPrintDataEqns(FILE *OutStream, const ATermList DataEqns, bool ShowSorts,
  int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       DataEqns is an ATermList containing data equations from a GenSpect
       specification
       ShowSorts indicates if sorts should be shown for each equation
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Post:A textual representation of the parts is written to OutStream, in which:
       - data equations are grouped in data equation sections, i.e. variable
         declarations apply to groups of equations
       - PrecLevel and ShowSort are distributed over the equations
*/

ATermList gsGroupDeclsBySort(const ATermList Decls);
/*Pre: Decls is an ATermList containing declarations of the form
       Decl(Name, Sort) from a GenSpect specification
  Ret: a list containing the declarations from Decls, where declarations of the
       same sort are placed in sequence
*/

void gsPrintDecls(FILE *OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator);
/*Pre: Decls is an ATermList containing action, operation, or variable
       declarations from a GenSpect specification
  Ret: A textual representation of the declarations is written to OutStream,
       in which:
       - of two consecutive declarations Decl(x, S) and Decl(y, T), the first
         is printed as:
         + "x,", if S = T
         + "x: S", followed by Terminator and Separator, if S != T
       - the last declaration Decl(x, S) is printed as "x: S", followed by
         Terminator
*/

void gsPrintDecl(FILE *OutStream, const ATermAppl Decl, const bool ShowSorts);
/*Pre: Decl is an ATermAppl that represents an action, operation, or variable
       declaration from a GenSpect specification
       ShowSorts indicates if the sort of the declaration should be shown
  Ret: A textual representation of the declaration, say Decl(x, S), is written
       to OutStream, i.e.:
       - "x: S", if ShowSorts
       - "x", otherwise
*/

bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part);
/*Pre: DataVarDecls represent the variables from a data equation section,
 *     where the keys are the variable names and the values are the
 *     corresponding variables
 *     Part is an ATermAppl containing a data equation of a GenSpect
 *     specification, or the elements it consists of
 *Ret: all operations occurring in Part are consistent with the variables from
 *     the context
 */

bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts);
/*Pre: DataVarDecls represent the variables from a data equation section,
 *     where the keys are the variable names and the values are the
 *     corresponding variables
 *     Parts is an ATermList containing elements of a data equation of a
 *     GenSpect specification
 *Ret: all operations occurring in Parts are consistent with the variables from
 *     the context
 */

void gsPrintPos(FILE *OutStream, const ATermAppl PosExpr, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       PosExpr is a data expression of sort Pos
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
  Ret: A textual representation of the expression is written to OutStream, in
       in which:
       - PrecLevel is taken into account
       - each constant is represented by its corresponding positive number
       - each non-constant is of the form cDub(b_1)(...(cDub(b_n)(p))) and is
         represented by 2^n*p + 2^(n-1)*b_n + ... + b1
*/

void gsPrintPosMult(FILE *OutStream, const ATermAppl PosExpr, int PrecLevel,
  char *Mult);
/*Pre: OutStream points to a stream to which can be written
       PosExpr is a data expression of sort Pos
       PrecLevel indicates the precedence level of the context of the part
       0 <= PrecLevel
       Mult is the string representation of a natural number
  Ret: A textual representation of Mult * PosExpr is written to OutStream, i.e.
       if PosExpr is the form cDub(b_1)(...(cDub(b_n)(p))), then it is
       represented by (Mult*2^n)*p + (Mult*2^(n-1))*b_n + ... + Mult*b1
       Also PrecLevel is taken into account
*/

//implementation

ATermAppl gsParseSpecification (FILE *SpecStream)
{
  ATermAppl Result = NULL;
  //check preconditions
  if (SpecStream == NULL) {
    ThrowVM(NULL, "formula stream may not be empty\n");
  }
  //enable constructor functions
  gsEnableConstructorFunctions();
  //parse specification using bison
  gsVerboseMsg("parsing specification from stream\n");
  Result = gsParse(SpecStream);
  if (Result == NULL) {
    ThrowM("parsing failed\n");
  }
  //type check specification
  gsVerboseMsg("type checking specification\n");
  Result = gsTypeCheck(Result);
  if (Result == NULL) {
    ThrowM("type checking failed\n");
  }
  //implement standard data types and type constructors
  gsVerboseMsg("implementing standard data types and type constructors\n");
  Result = gsImplementData(Result);
  if (Result == NULL) {
    ThrowM("data implementation failed\n");
  }
  //linearise processes
  gsVerboseMsg("linearising processes\n");
  Result = gsLinearise(Result);
  if (Result == NULL) {
    ThrowM("linearisation failed\n");
  }
finally:
  if (Result != NULL) {
    gsDebugMsg("return %t\n", Result);
  } else {
    gsDebugMsg("return NULL\n");
  }
  return Result;
}

bool gsPrintSpecification(FILE *OutStream, const ATermAppl Spec)
{
  bool Result = true;
  //check preconditions
  if (Spec == NULL) {
    ThrowVM(false, "specification may not be empty\n");
  }
  //print specification
  gsPrintPart(OutStream, Spec, false, 0);
finally:
  gsDebugMsg("return %s\n", Result?"true":"false");
  return Result;
}

ATermAppl gsLinearise(ATermAppl spec)
{
  gsVerboseMsg("linearisation is not yet implemented\n");
  return spec;
}

void gsPrintPart(FILE *OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel)
{
  if (ATisQuoted(ATgetAFun(Part)) == ATtrue) {
    //print string
    fprintf(OutStream, ATgetName(ATgetAFun(Part)));
  } else if (gsIsSpecV1(Part)) {
    //print specification
    gsDebugMsg("printing specification\n");
    for (int i = 0; i < 7; i++) {
      gsPrintPart(OutStream, ATAgetArgument(Part, i), ShowSorts, PrecLevel);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SortDecls) > 0) {
      fprintf(OutStream, "sort ");
      gsPrintParts(OutStream, SortDecls, ShowSorts, PrecLevel, ";\n", "     ");
      fprintf(OutStream, "\n");
    }
  } else if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList OpIds = ATLgetArgument(Part, 0);
    if (ATgetLength(OpIds) > 0) {
      fprintf(OutStream, gsIsConsSpec(Part)?"cons ":"map  ");
      gsPrintDecls(OutStream, OpIds, ";\n", "     ");
      fprintf(OutStream, "\n");
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    gsPrintDataEqns(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActIds = ATLgetArgument(Part, 0);
    if (ATgetLength(ActIds) > 0) {
      fprintf(OutStream, "act  ");
      gsPrintDecls(OutStream, ActIds, ";\n", "     ");
      fprintf(OutStream, "\n");
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    ATermList ProcDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(ProcDecls) > 0) {
      fprintf(OutStream, "proc ");
      gsPrintParts(OutStream, ProcDecls, ShowSorts, PrecLevel, ";\n", "     ");
      fprintf(OutStream, "\n");
    }
  } else if (gsIsLPE(Part)) {
    //print LPE
    gsDebugMsg("printing LPE\n");
    //print process name and variable declarations
    ATermList VarDecls = ATLgetArgument(Part, 0);
    int VarDeclsLength = ATgetLength(VarDecls);
    fprintf(OutStream, "proc P");
    if (VarDeclsLength > 0) {
      fprintf(OutStream, "(");
      gsPrintDecls(OutStream, VarDecls, NULL, ", ");
      fprintf(OutStream, ")");
    }
    fprintf(OutStream, " =");
    //print summations
    ATermList Summands = ATLgetArgument(Part, 1);
    int SummandsLength = ATgetLength(Summands);
    if (SummandsLength == 0) {
      fprintf(OutStream, " delta\n");
    } else {
      //SummandsLength > 0
      fprintf(OutStream, "\n       ");
      gsPrintParts(OutStream, Summands, ShowSorts, PrecLevel,
        NULL, "\n     + ");
      fprintf(OutStream, ";\n");
    }
    fprintf(OutStream, "\n");
  } else if (gsIsInit(Part)) {
    //print initialisation
    gsDebugMsg("printing initialisation\n");
    fprintf(OutStream, "init "); 
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    fprintf(OutStream, ";\n");
  } else if (gsIsLPEInit(Part)) {
    //print LPE initialisation
    gsDebugMsg("printing LPE initialisation\n");
    fprintf(OutStream, "init P"); 
    ATermList Args = ATLgetArgument(Part, 0);
    if (ATgetLength(Args) > 0) {
      fprintf(OutStream, "(");
      gsPrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      fprintf(OutStream, ")");
    }
    fprintf(OutStream, ";\n");
  } else if (gsIsSortId(Part)) {
    //print sort identifier
    gsDebugMsg("printing standard sort identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    gsDebugMsg("printing sort reference declaration\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsDataEqn(Part)) {
    //print data equation (without variables)
    gsDebugMsg("printing data equation\n");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      gsPrintPart(OutStream, Condition, ShowSorts, 0);
      fprintf(OutStream, "  ->  ");
    }
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), ShowSorts, 0);
    fprintf(OutStream, "  =  ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 3), ShowSorts, 0);
  } else if (gsIsActId(Part)) {
    //print action identifier
    gsDebugMsg("printing action identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        fprintf(OutStream, ": ");
        gsPrintParts(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsProcEqn(Part)) {
    //print process equation
    gsDebugMsg("printing process equation\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList DataVarIds = ATLgetArgument(Part, 1);
    if (ATgetLength(DataVarIds) > 0) {
      fprintf(OutStream, "(");
      gsPrintDecls(OutStream, DataVarIds, NULL, ", ");
      fprintf(OutStream, ")");
    }
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), ShowSorts, 0);
  } else if (gsIsProcVarId(Part)) {
    //print process variable
    gsDebugMsg("printing process variable\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        fprintf(OutStream, ": ");
        gsPrintParts(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsLPESummand(Part)) {
    //print LPE summand
    gsDebugMsg("printing LPE summand\n");
    //print data summations
    ATermList SumVarDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SumVarDecls) > 0) {
      fprintf(OutStream, "sum ");
      gsPrintDecls(OutStream, SumVarDecls, NULL, ",");
      fprintf(OutStream, ". ");
    }
    //print condition
    ATermAppl Cond = ATAgetArgument(Part, 1);
    if (!gsIsNil(Cond)) {
      gsPrintPart(OutStream, Cond, ShowSorts, 0);
      fprintf(OutStream, " -> ");
    }
    //print multiaction
    bool IsTimed = !gsIsNil(ATAgetArgument(Part, 3));
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), ShowSorts, (IsTimed)?6:5);
    //print time
    if (IsTimed) {
      fprintf(OutStream, " @ ");
      gsPrintPart(OutStream, ATAgetArgument(Part, 3), ShowSorts, 11);
    }
    fprintf(OutStream, " . ");
    //print process reference
    ATermList Assignments = ATLgetArgument(Part, 4);
    int AssignmentsLength = ATgetLength(Assignments);
    fprintf(OutStream, "P");
    if (AssignmentsLength > 0) {
      fprintf(OutStream, "(");
      gsPrintParts(OutStream, Assignments, ShowSorts, PrecLevel, NULL, ", ");
      fprintf(OutStream, ")");
    }
  } else if (gsIsMultAct(Part)) {
    //print multiaction
    gsDebugMsg("printing multiaction\n");
    ATermList Actions = ATLgetArgument(Part, 0);
    int ActionsLength = ATgetLength(Actions);
    if (ActionsLength == 0) {
      fprintf(OutStream, "tau");
    } else {
      //ActionsLength > 0
      if (PrecLevel > 7) fprintf(OutStream, "(");
      gsPrintParts(OutStream, Actions, ShowSorts, PrecLevel, NULL, "|");
      if (PrecLevel > 7) fprintf(OutStream, ")");
    }
  } else if (gsIsAssignment(Part)) {
    //print assignment
    gsDebugMsg("printing assignment\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    fprintf(OutStream, " := ");
    ATermAppl NewValue = ATAgetArgument(Part, 1);
    if (gsIsNil(NewValue)) {
      fprintf(OutStream, "<DC>");
    } else {
      gsPrintPart(OutStream, NewValue, ShowSorts, 0);
    }
  } else if (gsIsSortList(Part)) {
    //print list sort
    gsDebugMsg("printing list sort\n");
    fprintf(OutStream, "List(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortSet(Part)) {
    //print set sort
    gsDebugMsg("printing set sort\n");
    fprintf(OutStream, "Set(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortBag(Part)) {
    //print bag sort
    gsDebugMsg("printing bag sort\n");
    fprintf(OutStream, "Bag(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortStruct(Part)) {
    //print structured sort
    gsDebugMsg("printing structured sort\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    fprintf(OutStream, "struct ");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel,
      NULL, " | ");
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsSortArrowProd(Part)) {
    //print product arrow sort
    gsDebugMsg("printing product arrow sort\n");
    if (PrecLevel > 0) fprintf(OutStream, "(");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 1, NULL, " # ");
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    if (PrecLevel > 0) fprintf(OutStream, ")");
  } else if (gsIsSortArrow(Part)) {
    //print arrow sort
    gsDebugMsg("printing arrow sort\n");
    if (PrecLevel > 0) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 1);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    if (PrecLevel > 0) fprintf(OutStream, ")");
  } else if (gsIsStructCons(Part)) {
    //print structured sort constructor
    gsDebugMsg("printing structured sort constructor\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList StructProjs = ATLgetArgument(Part, 1);
    if (ATgetLength(StructProjs) > 0) {
      fprintf(OutStream, "(");
      gsPrintParts(OutStream, StructProjs, ShowSorts, PrecLevel, NULL, ", ");
      fprintf(OutStream, ")");
    }
    ATermAppl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser)) {
      fprintf(OutStream, "?");
      gsPrintPart(OutStream, Recogniser, ShowSorts, PrecLevel);
    }
  } else if (gsIsStructProj(Part)) {
    //print structured sort projection
    gsDebugMsg("printing structured sort projection\n");
    ATermAppl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection)) {
      gsPrintPart(OutStream, Projection, ShowSorts, PrecLevel);
      fprintf(OutStream, ": ");
    }
    gsPrintParts(OutStream, ATLgetArgument(Part, 1), ShowSorts, 2, NULL, " # ");
  } else if (gsIsDataVarIdOpId(Part) || gsIsOpId(Part) || gsIsDataVarId(Part) ||
      gsIsDataAppl(Part) || gsIsDataApplProd(Part)) {
    //print data expression, possibly in the external format
    ATermAppl Head;
    ATermList Args;
    if (!gsIsDataApplProd(Part)) {
      Head = gsGetDataExprHead(Part);
      Args = gsGetDataExprArgs(Part);
    } else {
      Head = ATAgetArgument(Part, 0);
      Args = ATLgetArgument(Part, 1);
    }
    int ArgsLength = ATgetLength(Args);
    if (gsIsOpIdPrefix(Head) && ArgsLength == 1) {
      //print prefix expression
      gsDebugMsg("printing prefix expression\n");
      if (PrecLevel > 11) fprintf(OutStream, "(");
      gsPrintPart(OutStream, Head, ShowSorts, PrecLevel);
      gsPrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, 11);
      if (PrecLevel > 11) fprintf(OutStream, ")");
    } else if (gsIsOpIdInfix(Head) && ArgsLength == 2) {
      //print infix expression
      gsDebugMsg("printing infix expression\n");
      if (PrecLevel > gsPrecOpIdInfix(Head)) fprintf(OutStream, "(");
      gsPrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts,
        gsPrecOpIdInfixLeft(Head));
      fprintf(OutStream, " ");
      gsPrintPart(OutStream, Head, ShowSorts, PrecLevel);
      fprintf(OutStream, " ");
      gsPrintPart(OutStream, ATAelementAt(Args, 1), ShowSorts,
        gsPrecOpIdInfixRight(Head));
      if (PrecLevel > gsPrecOpIdInfix(Head)) fprintf(OutStream, ")");
   } else if (ATisEqual(Head, gsMakeOpId1()) ||
        (ATisEqual(Head, gsMakeOpIdCDub()) && ArgsLength == 2)) {
      //print positive number
      gsDebugMsg("printing positive number %t\n", Part);
      gsPrintPos(OutStream, Part, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpId0())) {
      //print 0
      fprintf(OutStream, "0");
    } else if ((ATisEqual(Head, gsMakeOpIdCNat()) ||
        ATisEqual(Head, gsMakeOpIdCInt())) && ArgsLength == 1) {
      //print argument (ArgsLength == 1)
      gsPrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpIdCNeg()) && ArgsLength == 1) {
      //print negation (ArgsLength == 1)
      gsDebugMsg("printing negation\n");
      fprintf(OutStream, "-");
      gsPrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, 11);
    } else if (gsIsDataVarIdOpId(Part)) {
      //print untyped data variable or operation identifier
      gsDebugMsg("printing untyped data variable or operation identifier\n");
      gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    } else if (gsIsOpId(Part) || gsIsDataVarId(Part)) {
      //print data variable or operation identifier
      gsDebugMsg("printing data variable or operation identifier\n");
      gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
      if (ShowSorts) {
        fprintf(OutStream, ": ");
        gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
      }
    } else {
      //print data application
      gsDebugMsg("printing data application\n");
      if (PrecLevel > 12) fprintf(OutStream, "(");
      gsPrintPart(OutStream, Head, ShowSorts, 12);
      fprintf(OutStream, "(");
      gsPrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      fprintf(OutStream, ")");
      if (PrecLevel > 12) fprintf(OutStream, ")");
    }
  } else if (gsIsNumber(Part)) {
    //print number
    gsDebugMsg("printing number\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsListEnum(Part)) {
    //print list enumeration
    gsDebugMsg("printing list enumeration\n");
    fprintf(OutStream, "[");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    fprintf(OutStream, "]");
  } else if (gsIsSetEnum(Part) || gsIsBagEnum(Part)) {
    //print set/bag enumeration
    gsDebugMsg("printing set/bag enumeration\n");
    fprintf(OutStream, "{");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    fprintf(OutStream, "}");
  } else if (gsIsSetBagComp(Part)) {
    //print set/bag comprehension
    gsDebugMsg("printing set/bag comprehension\n");
    fprintf(OutStream, "{ ");
    gsPrintDecl(OutStream, ATAgetArgument(Part, 0), true);
    fprintf(OutStream, " | ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    fprintf(OutStream, " }");
  } else if (gsIsForall(Part) || gsIsExists(Part)) {
    //print universal/existential quantification
    gsDebugMsg("printing universal/existential quantification\n");
    if (PrecLevel > 11) fprintf(OutStream, "(");
    fprintf(OutStream, (gsIsForall(Part))?"forall ":"exists ");
    gsPrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    fprintf(OutStream, ". ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 11);
    if (PrecLevel > 11) fprintf(OutStream, ")");
  } else if (gsIsLambda(Part)) {
    //print lambda abstraction
    gsDebugMsg("printing lambda abstraction\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    fprintf(OutStream, "lambda ");
    gsPrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    fprintf(OutStream, ". ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsWhr(Part)) {
    //print where clause
    gsDebugMsg("printing where clause\n");
    if (PrecLevel > 0) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, " whr ");
    gsPrintParts(OutStream, ATLgetArgument(Part, 1), ShowSorts, PrecLevel,
      NULL, ", ");
    fprintf(OutStream, " end");
    if (PrecLevel > 0) fprintf(OutStream, ")");
  } else if (gsIsBagEnumElt(Part)) {
    //print bag enumeration element
    gsDebugMsg("printing bag enumeration element\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, ": ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsWhrDecl(Part)) {
    //print where declaration element
    gsDebugMsg("printing where declaration\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsActionProcess(Part) || gsIsAction(Part) || gsIsProcess(Part)) {
    //print action or process reference
    gsDebugMsg("printing action or process reference\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
      fprintf(OutStream, "(");
      gsPrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      fprintf(OutStream, ")");
    }
  } else if (gsIsDelta(Part)) {
    //print delta
    gsDebugMsg("printing delta\n");
    fprintf(OutStream, "delta");
  } else if (gsIsTau(Part)) {
    //print tau
    gsDebugMsg("printing tau\n");
    fprintf(OutStream, "tau");
  } else if (gsIsChoice(Part)) {
    //print choice
    gsDebugMsg("printing choice\n");
    if (PrecLevel > 0) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    fprintf(OutStream, " + ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
    if (PrecLevel > 0) fprintf(OutStream, ")");
  } else if (gsIsSum(Part)) {
    //print summation
    gsDebugMsg("printing summation\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    fprintf(OutStream, "sum ");
    gsPrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    fprintf(OutStream, ". ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsMerge(Part) || gsIsLMerge(Part)) {
    //print merge of left merge
    gsDebugMsg("printing merge or left merge\n");
    if (PrecLevel > 2) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 2);
    if (gsIsMerge(Part)) {
      fprintf(OutStream, " || ");
    } else {
      fprintf(OutStream, " ||_ ");
    }
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 3);
    if (PrecLevel > 2) fprintf(OutStream, ")");
  } else if (gsIsBInit(Part)) {
    //print bounded initialisation
    gsDebugMsg("printing bounded initialisation\n");
    if (PrecLevel > 3) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 3);
    fprintf(OutStream, " << ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 4);
    if (PrecLevel > 3) fprintf(OutStream, ")");
  } else if (gsIsCond(Part)) {
    //print conditional
    gsDebugMsg("printing conditional\n");
    if (PrecLevel > 4) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 11);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 5);
    ATermAppl PartElse = ATAgetArgument(Part, 2);
    if (!gsIsDelta(PartElse)) {
      fprintf(OutStream, ", ");
      gsPrintPart(OutStream, PartElse, ShowSorts, 5);
    }
    if (PrecLevel > 4) fprintf(OutStream, ")");
  } else if (gsIsSeq(Part)) {
    //print sequential composition
    gsDebugMsg("printing sequential composition\n");
    if (PrecLevel > 5) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 5);
    fprintf(OutStream, " . ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 6);
    if (PrecLevel > 5) fprintf(OutStream, ")");
  } else if (gsIsAtTime(Part)) {
    //print at expression
    gsDebugMsg("printing at expression\n");
    if (PrecLevel > 6) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 6);
    fprintf(OutStream, " @ ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 11);
    if (PrecLevel > 6) fprintf(OutStream, ")");
  } else if (gsIsSync(Part)) {
    //print sync
    gsDebugMsg("printing sync\n");
    if (PrecLevel > 7) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 7);
    fprintf(OutStream, " | ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 8);
    if (PrecLevel > 7) fprintf(OutStream, ")");
  } else if (gsIsRestrict(Part) || gsIsHide(Part) || gsIsRename(Part) ||
      gsIsComm(Part) || gsIsAllow(Part)) {
    //print process quantification
    gsDebugMsg("printing process quantification\n");
    if (gsIsRestrict(Part)) {
      fprintf(OutStream, "restrict");
    } else if (gsIsHide(Part)) {
      fprintf(OutStream, "hide");
    } else if (gsIsRename(Part)) {
      fprintf(OutStream, "rename");
    } else if (gsIsComm(Part)) {
      fprintf(OutStream, "comm");
    } else {
      fprintf(OutStream, "allow");
    }
    fprintf(OutStream, "({");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    fprintf(OutStream, "}, ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    fprintf(OutStream, ")");
  } else if (gsIsMultActName(Part)) {
    //print multi action name
    gsDebugMsg("printing multi action name\n");
    gsPrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, " | ");
  } else if (gsIsRenameExpr(Part)) {
    //print renaming expression
    gsDebugMsg("printing renaming expression\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsCommExpr(Part)) {
    //print communication expression
    gsDebugMsg("printing communication expression\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermAppl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult)) {
      fprintf(OutStream, " -> ");
      gsPrintPart(OutStream, CommResult, ShowSorts, PrecLevel);
    }
  }
}

void gsPrintParts(FILE *OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator)
{
  int n = ATgetLength(Parts);
  for (int i = 0; i < n; i++) {
    if (i > 0 && Separator != NULL) fprintf(OutStream, Separator);
    gsPrintPart(OutStream, ATAelementAt(Parts, i), ShowSorts, PrecLevel);
    if (Terminator != NULL) fprintf(OutStream, Terminator);
  }
}

void gsPrintDataEqns(FILE *OutStream, const ATermList DataEqns, bool ShowSorts,
  int PrecLevel)
{
  int DataEqnsLength = ATgetLength(DataEqns);
  if (DataEqnsLength > 0) {
    int StartPrefix = 0;
    ATermTable VarDeclTable = ATtableCreate(63, 50);
    //VarDeclTable is a hash table with variable declarations as values, where
    //the name of each variable declaration is used a key.
    //Note that the hash table will be increased if at least 32 values are added,
    //This can be avoided by increasing the initial size.
    int i = 0;
    while (i < DataEqnsLength) {
      //StartPrefix represents the start index of the maximum consistent prefix
      //of variable declarations in DataEqns to which DataEqns(i) belongs
      //VarDeclTable represents the variable declarations of DataEqns
      //from StartPrefix up to i.
      //Check consistency of DataEqns(i) with VarDeclTable and add newly
      //declared variables to VarDeclTable.
      ATermAppl DataEqn = ATAelementAt(DataEqns, i);
      bool Consistent = gsHasConsistentContext(VarDeclTable, DataEqn);
      if (Consistent) {
        //add new variables from DataEqns(i) to VarDeclTable
        ATermList VarDecls = ATLgetArgument(DataEqn, 0);
        int VarDeclsLength = ATgetLength(VarDecls);
        for (int j = 0; j < VarDeclsLength; j++) {
          ATermAppl VarDecl = ATAelementAt(VarDecls, j);
          ATermAppl VarDeclName = ATAgetArgument(VarDecl, 0);
          if (ATtableGet(VarDeclTable, (ATerm) VarDeclName) == NULL) {
            ATtablePut(VarDeclTable, (ATerm) VarDeclName, (ATerm) VarDecl);
          }
        }
        i++;
      }
      if (!Consistent || (i == DataEqnsLength)) {
        //VarDeclTable represents the maximum consistent prefix of variable
        //declarations of DataEqns starting at StartPrefix. Print this prefix
        //and the corresponding equations,and if necessary, update StartPrefix
        //and reset VarDeclTable.
        fprintf(OutStream, "var  ");
        gsPrintDecls(OutStream,
          gsGroupDeclsBySort(ATreverse(ATtableValues(VarDeclTable))),
          ";\n", "     ");
        fprintf(OutStream, "eqn  ");
        gsPrintParts(OutStream,
          ATgetSlice(DataEqns, StartPrefix, i), ShowSorts, PrecLevel,
          ";\n", "     ");
        if (i < DataEqnsLength) {
          fprintf(OutStream, "\n");
          StartPrefix = i;
          ATtableReset(VarDeclTable);
        }
      }
    }
    //finalisation after printing all (>0) data equations
    fprintf(OutStream, "\n");
    ATtableDestroy(VarDeclTable);
  }
}

bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part)
{
  if (gsIsDataEqn(Part)) {
    //check consistency of DataVarDecls with the variable declarations, the
    //condition and the lhs and rhs of the data equation
    bool Result = true;
    ATermList VarDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(VarDecls);
    for (int i = 0; i < n && Result; i++) {
      //check consistency of variable VarDecls(j) with VarDeclTable
      ATermAppl VarDecl = ATAelementAt(VarDecls, i);
      ATermAppl CorVarDecl =
        ATAtableGet(DataVarDecls, ATgetArgument(VarDecl, 0));
      if (CorVarDecl != NULL) {
        //check consistency of VarDecl with CorVarDecl
        Result = (ATisEqual(VarDecl, CorVarDecl) == ATtrue);
      }
    }
    if (Result) {
      Result = 
        gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 1)) &&
        gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 2)) &&
        gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 3));
    }
    return Result;
  } else if (gsIsOpId(Part) || gsIsDataVarIdOpId(Part))  {
    //Part may be an operation; check that its name does not occur in
    //DataVarDecls
    return (ATtableGet(DataVarDecls, ATgetArgument(Part, 0)) == NULL);
  } else if (gsIsDataApplProd(Part)) {
    //check consistency of the head and all arguments
    return
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 0)) &&
      gsHasConsistentContextList(DataVarDecls, ATLgetArgument(Part, 1));
  } else if (gsIsDataAppl(Part)) {
    //check consistency of the head and the argument
    return
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 0)) &&
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 1));
  } else if (gsIsListEnum(Part) || gsIsSetEnum(Part) || gsIsBagEnum(Part)) {
    //check consistency of all elements
    return gsHasConsistentContextList(DataVarDecls, ATLgetArgument(Part, 0));
  } else if (gsIsSetBagComp(Part) || gsIsForall(Part) || gsIsExists(Part) ||
      gsIsLambda(Part)) {
    //check consistency of the body
    return gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 1));
  } else if (gsIsWhr(Part)) {
    //check consistency of the body and the where clauses
    return
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 0)) &&
      gsHasConsistentContextList(DataVarDecls, ATLgetArgument(Part, 1));
  } else if (gsIsBagEnumElt(Part)) {
    //check consistency of the element and the multiplicity
    return
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 0)) &&
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 1));
  } else if (gsIsWhrDecl(Part)) {
    //check consistency of the rhs
    return
      gsHasConsistentContext(DataVarDecls, ATAgetArgument(Part, 1));
  } else {
    //Part is a variable, a number or nil
    return true;
  }
}

bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts)
{
  bool Result = true;
  int n = ATgetLength(Parts);
  for (int i = 0; i < n && Result; i++) {
    Result = gsHasConsistentContext(DataVarDecls, ATAelementAt(Parts, i));
  }
  return Result;
}

ATermList gsGroupDeclsBySort(const ATermList Decls)
{
  int DeclsLength = ATgetLength(Decls);
  if (DeclsLength > 0) {
    ATermTable SortDeclsTable = ATtableCreate(2*DeclsLength, 50);
    //Add all variable declarations from Decls to hash table
    //SortDeclsTable
    for (int i = 0; i < DeclsLength; i++) {
      ATermAppl Decl = ATAelementAt(Decls, i);
      ATermAppl DeclSort = ATAgetArgument(Decl, 1);
      ATermList CorDecls = ATLtableGet(SortDeclsTable,
        (ATerm) DeclSort);
      if (CorDecls == NULL) {
        ATtablePut(SortDeclsTable, (ATerm) DeclSort,
          (ATerm) ATmakeList1((ATerm) Decl));
      } else {
        ATtablePut(SortDeclsTable, (ATerm) DeclSort,
          (ATerm) ATappend(CorDecls, (ATerm) Decl));
      }
    }
    //Return the hash table as a list of variable declarations
    ATermList Result = ATmakeList0();
    ATermList DeclSorts = ATtableKeys(SortDeclsTable);
    int DeclSortsLength = ATgetLength(DeclSorts);
    for (int i = 0; i < DeclSortsLength; i++) {
      Result = ATconcat(
        ATLtableGet(SortDeclsTable, ATelementAt(DeclSorts, i)), Result);
    }
    ATtableDestroy(SortDeclsTable);
    return Result;
  } else {
    //Decls is empty
    return Decls;
  }
}

void gsPrintDecls(FILE *OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator)
{
  int n = ATgetLength(Decls);
  if (n > 0) {
    for (int i = 0; i < n-1; i++) {
      ATermAppl Decl = ATAelementAt(Decls, i);
      //check if sorts of Decls(i) and Decls(i+1) are equal
      if (ATisEqual(ATgetArgument(Decl, 1),
          ATgetArgument(ATelementAt(Decls, i+1), 1))) {
        gsPrintDecl(OutStream, Decl, false);
        fprintf(OutStream, ",");
      } else {
        gsPrintDecl(OutStream, Decl, true);
        if (Terminator  != NULL) fprintf(OutStream, Terminator);
        if (Separator  != NULL) fprintf(OutStream, Separator);
      }
    }
    gsPrintDecl(OutStream, ATAelementAt(Decls, n-1), true);
    if (Terminator  != NULL) fprintf(OutStream, Terminator);
  }
}

void gsPrintDecl(FILE *OutStream, const ATermAppl Decl, const bool ShowSorts)
{
  gsPrintPart(OutStream, ATAgetArgument(Decl, 0), ShowSorts, 0);
  if (ShowSorts) {
    fprintf(OutStream, ": ");
    if (gsIsActId(Decl)) {
      gsPrintParts(OutStream, ATLgetArgument(Decl, 1), ShowSorts, 2,
        NULL, " # ");
    } else {
      gsPrintPart(OutStream, ATAgetArgument(Decl, 1), ShowSorts, 0);
    }
  }
}

void gsPrintPos(FILE *OutStream, const ATermAppl PosExpr, int PrecLevel)
{
  if (gsIsPosConstant(PosExpr)) {
    char *PosValue = gsPosValue(PosExpr);
    fprintf(OutStream, PosValue);
    free(PosValue);
  } else {
    gsPrintPosMult(OutStream, PosExpr, PrecLevel, "1");
  }
}

void gsPrintPosMult(FILE *OutStream, const ATermAppl PosExpr, int PrecLevel,
  char *Mult)
{
  ATermAppl Head = gsGetDataExprHead(PosExpr);
  ATermList Args = gsGetDataExprArgs(PosExpr);
  if (ATisEqual(PosExpr, gsMakeOpId1())) {
    //PosExpr is 1; print Mult
    fprintf(OutStream, Mult);
  } else if (ATisEqual(Head, gsMakeOpIdCDub())) {
    //PosExpr is of the form cDub(b,p); print (Mult*2)*v(p) + Mult*v(b)
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl PosArg = ATAelementAt(Args, 1);
    char *NewMult = gsStringDub(Mult, 0);
    if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      //Mult*v(b) = 0
      gsPrintPosMult(OutStream, PosArg, PrecLevel, NewMult);
    } else {
      //Mult*v(b) > 0
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdAdd(gsMakeSortExprPos()))) {
        fprintf(OutStream, "(");
      }
      //print (Mult*2)*v(p)
      gsPrintPosMult(OutStream, PosArg, 
        gsPrecOpIdInfixLeft(gsMakeOpIdAdd(gsMakeSortExprPos())), NewMult);
      fprintf(OutStream, " + ");
      if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
        //Mult*v(b) = Mult
        fprintf(OutStream, Mult);
      } else if (strcmp(Mult, "1") == 0) {
        //Mult*v(b) = v(b)
        gsPrintPart(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdAdd(gsMakeSortExprPos())));
      } else {
        //print Mult*v(b)
        fprintf(OutStream, "%s*", Mult);
        gsPrintPart(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdMult(gsMakeSortExprPos())));
      }
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdAdd(gsMakeSortExprPos()))) {
        fprintf(OutStream, ")");
      }
    }
    free(NewMult);
  } else {
    //PosExpr is not a Pos constructor
    if (strcmp(Mult, "1") == 0) {
      gsPrintPart(OutStream, PosExpr, false, PrecLevel);
    } else {
      fprintf(OutStream, "%s*", Mult);
      gsPrintPart(OutStream, PosExpr, false,
        gsPrecOpIdInfixRight(gsMakeOpIdMult(gsMakeSortExprPos())));
    }
  }
}

void gsTest(void)
{
  //initialise ATerm library
  ATerm StackBottom;
  ATinit(0, NULL, &StackBottom);
  //enable constructor functions
  gsEnableConstructorFunctions();
  //build positive constant
  ATermAppl t = gsMakeDataExprInt_int(1337);
  int n = gsIntValue_int(t);
  fprintf(stderr, "%d\n", n);
}
