#ifdef GS_PRINT_FILE
#include <stdio.h>
#define GS_PRINT_OUTTYPE FILE*
#define GS_PRINT_FUNC(x) x ## File
#endif

#ifdef GS_PRINT_STREAM
#include <ostream>
#define GS_PRINT_OUTTYPE std::ostream&
#define GS_PRINT_FUNC(x) x ## Stream
#endif

static void GS_PRINT_FUNC(PrintPart)(GS_PRINT_OUTTYPE OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Part is an ATermAppl containing a part of a mCRL2 specification
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

static void GS_PRINT_FUNC(PrintParts)(GS_PRINT_OUTTYPE OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator);
/*Pre: OutStream points to a stream to which can be written
       Parts is an ATermList containing parts of a mCRL2 specification
       ShowSorts indicates if sorts should be shown for the part
       PrecLevel indicates the precedence level of the context of the parts
       0 <= PrecLevel
  Post:A textual representation of the parts is written to OutStream, in which:
       - PrecLevel and ShowSort are distributed over the parts
       - each part is terminated by Terminator, if it is not NULL
       - two successive parts are separated by Separator, if it is not NULL
*/

static void GS_PRINT_FUNC(PrintEqns)(GS_PRINT_OUTTYPE OutStream, const ATermList DataEqns, bool ShowSorts,
  int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Eqns is an ATermList containing equations from a mCRL2 specification
       ShowSorts indicates if sorts should be shown for each equation
       PrecLevel indicates the precedence level of the context of the equations
       0 <= PrecLevel
  Post:A textual representation of the equations is written to OutStream, in
       which:
       - equations are grouped in equation sections, i.e. variable declarations
         apply to groups of equations
       - PrecLevel and ShowSort are distributed over the equations
*/

//static void GS_PRINT_FUNC(PrintProcEqns)(GS_PRINT_OUTTYPE OutStream, const ATermList ProcEqns, bool ShowSorts,
//  int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       ProcEqns is an ATermList containing process equations from a mCRL2
       specification
       ShowSorts indicates if sorts should be shown for each equation
       PrecLevel indicates the precedence level of the context of the equations
       0 <= PrecLevel
  Post:A textual representation of the process equations is written to
       OutStream, in which:
       - process equations are grouped in process equation sections, i.e.
         variable declarations apply to groups of equations
       - PrecLevel and ShowSort are distributed over the equations
*/

ATermList gsGroupDeclsBySort(const ATermList Decls);
/*Pre: Decls is an ATermList containing declarations of the form
       Decl(Name, Sort) from a mCRL2 specification
  Ret: a list containing the declarations from Decls, where declarations of the
       same sort are placed in sequence
*/

static void GS_PRINT_FUNC(PrintDecls)(GS_PRINT_OUTTYPE OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator);
/*Pre: Decls is an ATermList containing action, operation, or variable
       declarations from a mCRL2 specification
  Ret: A textual representation of the declarations is written to OutStream,
       in which:
       - of two consecutive declarations Decl(x, S) and Decl(y, T), the first
         is printed as:
         + "x,", if S = T
         + "x: S", followed by Terminator and Separator, if S != T
       - the last declaration Decl(x, S) is printed as "x: S", followed by
         Terminator
*/

static void GS_PRINT_FUNC(PrintDecl)(GS_PRINT_OUTTYPE OutStream, const ATermAppl Decl, const bool ShowSorts);
/*Pre: Decl is an ATermAppl that represents an action, operation, or variable
       declaration from a mCRL2 specification
       ShowSorts indicates if the sort of the declaration should be shown
  Ret: A textual representation of the declaration, say Decl(x, S), is written
       to OutStream, i.e.:
       - "x: S", if ShowSorts
       - "x", otherwise
*/

bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part);
/*Pre: DataVarDecls represents the variables from an equation section, where
 *     the keys are the variable names and the values are the corresponding
 *     variables
 *     Part is an ATermAppl containing an equation of a mCRL2 specification,
 *     or the elements it consists of
 *Ret: all operations occurring in Part are consistent with the variables from
 *     the context
 */

bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts);
/*Pre: DataVarDecls represents the variables from an equation section, where
 *     the keys are the variable names and the values are the
 *     corresponding variables
 *     Parts is an ATermList containing elements of an equation of a mCRL2
 *     specification
 *Ret: all operations occurring in Parts are consistent with the variables from
 *     the context
 */

bool gsIsListEnumImpl(ATermAppl DataExpr);
//Ret: DataExpr is the implementation of a list enumeration

static void GS_PRINT_FUNC(PrintListEnumElts)(GS_PRINT_OUTTYPE OutStream, const ATermAppl DataExpr,
  bool ShowSorts);
/*Pre: OutStream points to a stream to which can be written
 *     DataExpr is the implementation of a list enumeration
 *     ShowSorts indicates if the sorts of DataExpr should be shown
 *Post:A textual representation of the expression is written to OutStream, in
 *     which:
 *     - ShowSorts is taken into account
 *     - the elements of the list are printed as a comma-separated list
 */

static void GS_PRINT_FUNC(PrintPos)(GS_PRINT_OUTTYPE OutStream, const ATermAppl PosExpr, int PrecLevel);
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

static void GS_PRINT_FUNC(PrintPosMult)(GS_PRINT_OUTTYPE OutStream, const ATermAppl PosExpr, int PrecLevel,
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

static void GS_PRINT_FUNC(PrintPart)(GS_PRINT_OUTTYPE OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel)
{
  if (ATisQuoted(ATgetAFun(Part)) == ATtrue) {
    //print string
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ATgetName(ATgetAFun(Part)));
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ATgetName(ATgetAFun(Part));
#endif
  } else if (gsIsSpecV1(Part)) {
    //print specification
    gsDebugMsg("printing specification\n");
    for (int i = 0; i < 7; i++) {
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, i), ShowSorts, PrecLevel);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SortDecls) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "sort ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "sort ";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, SortDecls, ShowSorts, PrecLevel, ";\n", "     ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << std::endl;
#endif
    }
  } else if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList OpIds = ATLgetArgument(Part, 0);
    if (ATgetLength(OpIds) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, gsIsConsSpec(Part)?"cons ":"map  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << (gsIsConsSpec(Part)?"cons ":"map  ");
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, OpIds, ";\n", "     ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << std::endl;
#endif
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    GS_PRINT_FUNC(PrintEqns)(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActIds = ATLgetArgument(Part, 0);
    if (ATgetLength(ActIds) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "act  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "act  ";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, ActIds, ";\n", "     ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << std::endl;
#endif
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    GS_PRINT_FUNC(PrintEqns)(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsLPE(Part)) {
    //print LPE
    gsDebugMsg("printing LPE\n");
    //print global variables
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "var  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "var  ";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, Vars, ";\n", "     ");
    }
    //print process name and variable declarations
    ATermList VarDecls = ATLgetArgument(Part, 1);
    int VarDeclsLength = ATgetLength(VarDecls);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "proc P");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "proc P";
#endif
    if (VarDeclsLength > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, VarDecls, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " =");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " =";
#endif
    //print summations
    ATermList Summands = ATLgetArgument(Part, 2);
    int SummandsLength = ATgetLength(Summands);
    if (SummandsLength == 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " delta\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " delta" << std::endl;
#endif
    } else {
      //SummandsLength > 0
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "\n       ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << std::endl << "       ";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, Summands, ShowSorts, PrecLevel,
        NULL, "\n     + ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ";\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ";" << std::endl;
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << std::endl;
#endif
  } else if (gsIsInit(Part)) {
    //print initialisation
    gsDebugMsg("printing initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "var  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "var  ";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, Vars, ";\n", "     ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << std::endl;
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "init "); 
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "init "; 
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ";\n");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ";" << std::endl;
#endif
  } else if (gsIsLPEInit(Part)) {
    //print LPE initialisation
    gsDebugMsg("printing LPE initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "var  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "var  ";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, Vars, ";\n", "     ");
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "init P"); 
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "init P"; 
#endif
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, Args, ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ";\n");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ";" << std::endl;
#endif
  } else if (gsIsSortId(Part)) {
    //print sort identifier
    gsDebugMsg("printing standard sort identifier\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    gsDebugMsg("printing sort reference declaration\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " = ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " = ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsDataEqn(Part)) {
    //print data equation (without variables)
    gsDebugMsg("printing data equation\n");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      GS_PRINT_FUNC(PrintPart)(OutStream, Condition, ShowSorts, 0);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "  ->  ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "  ->  ";
#endif
    }
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 2), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "  =  ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "  =  ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 3), ShowSorts, 0);
  } else if (gsIsActId(Part)) {
    //print action identifier
    gsDebugMsg("printing action identifier\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ": ";
#endif
        GS_PRINT_FUNC(PrintParts)(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsProcEqn(Part)) {
    //print process equation (without free variables)
    gsDebugMsg("printing process equation\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
    ATermList DataVarIds = ATLgetArgument(Part, 2);
    if (ATgetLength(DataVarIds) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, DataVarIds, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " = ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " = ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 3), ShowSorts, 0);
  } else if (gsIsProcVarId(Part)) {
    //print process variable
    gsDebugMsg("printing process variable\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ": ";
#endif
        GS_PRINT_FUNC(PrintParts)(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsLPESummand(Part)) {
    //print LPE summand
    gsDebugMsg("printing LPE summand\n");
    //print data summations
    ATermList SumVarDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SumVarDecls) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "sum ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "sum ";
#endif
      GS_PRINT_FUNC(PrintDecls)(OutStream, SumVarDecls, NULL, ",");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ". ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ". ";
#endif
    }
    //print condition
    ATermAppl Cond = ATAgetArgument(Part, 1);
    if (!gsIsNil(Cond)) {
      GS_PRINT_FUNC(PrintPart)(OutStream, Cond, ShowSorts, 0);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " -> ";
#endif
    }
    //print multiaction
    ATermAppl MultAct = ATAgetArgument(Part, 2);
    ATermAppl Time = ATAgetArgument(Part, 3);
    bool IsTimed = !gsIsNil(Time);
    GS_PRINT_FUNC(PrintPart)(OutStream, MultAct, ShowSorts, (IsTimed)?6:5);
    //print time
    if (IsTimed) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " @ ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " @ ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, Time, ShowSorts, 12);
    }
    //print process reference
    if (!gsIsDelta(MultAct)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " . ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " . ";
#endif
      ATermList Assignments = ATLgetArgument(Part, 4);
      int AssignmentsLength = ATgetLength(Assignments);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "P");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "P";
#endif
      if (AssignmentsLength > 0) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << "(";
#endif
        GS_PRINT_FUNC(PrintParts)(OutStream, Assignments, ShowSorts, PrecLevel, NULL, ", ");
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ")";
#endif
      }
    }
  } else if (gsIsMultAct(Part)) {
    //print multiaction
    gsDebugMsg("printing multiaction\n");
    ATermList Actions = ATLgetArgument(Part, 0);
    int ActionsLength = ATgetLength(Actions);
    if (ActionsLength == 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "tau");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "tau";
#endif
    } else {
      //ActionsLength > 0
#ifdef GS_PRINT_FILE
      if (PrecLevel > 7) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 7) OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, Actions, ShowSorts, PrecLevel, NULL, "|");
#ifdef GS_PRINT_FILE
      if (PrecLevel > 7) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 7) OutStream << ")";
#endif
    }
  } else if (gsIsAssignment(Part)) {
    //print assignment
    gsDebugMsg("printing assignment\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " := ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " := ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsSortList(Part)) {
    //print list sort
    gsDebugMsg("printing list sort\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "List(");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "List(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ")";
#endif
  } else if (gsIsSortSet(Part)) {
    //print set sort
    gsDebugMsg("printing set sort\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "Set(");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "Set(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ")";
#endif
  } else if (gsIsSortBag(Part)) {
    //print bag sort
    gsDebugMsg("printing bag sort\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "Bag(");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "Bag(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ")";
#endif
  } else if (gsIsSortStruct(Part)) {
    //print structured sort
    gsDebugMsg("printing structured sort\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << "(";
#endif
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "struct ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "struct ";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel,
      NULL, " | ");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << ")";
#endif
  } else if (gsIsSortArrowProd(Part)) {
    //print product arrow sort
    gsDebugMsg("printing product arrow sort\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, 1, NULL, " # ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " -> ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << ")";
#endif
  } else if (gsIsSortArrow(Part)) {
    //print arrow sort
    gsDebugMsg("printing arrow sort\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 1);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " -> ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << ")";
#endif
  } else if (gsIsStructCons(Part)) {
    //print structured sort constructor
    gsDebugMsg("printing structured sort constructor\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList StructProjs = ATLgetArgument(Part, 1);
    if (ATgetLength(StructProjs) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, StructProjs, ShowSorts, PrecLevel, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
    }
    ATermAppl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "?");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "?";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, Recogniser, ShowSorts, PrecLevel);
    }
  } else if (gsIsStructProj(Part)) {
    //print structured sort projection
    gsDebugMsg("printing structured sort projection\n");
    ATermAppl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection)) {
      GS_PRINT_FUNC(PrintPart)(OutStream, Projection, ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ": ";
#endif
    }
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsDataVarIdOpId(Part) || gsIsOpId(Part) || gsIsDataVarId(Part) ||
      gsIsDataAppl(Part) || gsIsDataApplProd(Part)) {
    //print data expression, if possible in the external format
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
    if (gsIsListEnumImpl(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "[");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "[";
#endif
      GS_PRINT_FUNC(PrintListEnumElts)(OutStream, Part, ShowSorts);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "]");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "]";
#endif
    } else if (gsIsOpIdPrefix(Head) && ArgsLength == 1) {
      //print prefix expression
      gsDebugMsg("printing prefix expression\n");
#ifdef GS_PRINT_FILE
      if (PrecLevel > 12) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 12) OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, Head, ShowSorts, PrecLevel);
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 0), ShowSorts, 12);
#ifdef GS_PRINT_FILE
      if (PrecLevel > 12) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 12) OutStream << ")";
#endif
    } else if (gsIsOpIdInfix(Head) && ArgsLength == 2) {
      //print infix expression
      gsDebugMsg("printing infix expression\n");
      ATermAppl HeadName = ATAgetArgument(Head, 0);
#ifdef GS_PRINT_FILE
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 0), ShowSorts,
        gsPrecOpIdInfixLeft(HeadName));
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, Head, ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 1), ShowSorts,
        gsPrecOpIdInfixRight(HeadName));
#ifdef GS_PRINT_FILE
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) OutStream << ")";
#endif
   } else if (ATisEqual(Head, gsMakeOpId1()) ||
        (ATisEqual(Head, gsMakeOpIdCDub()) && ArgsLength == 2)) {
      //print positive number
      gsDebugMsg("printing positive number %t\n", Part);
      GS_PRINT_FUNC(PrintPos)(OutStream, Part, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpId0())) {
      //print 0
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "0");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "0";
#endif
    } else if ((ATisEqual(Head, gsMakeOpIdCNat()) ||
        ATisEqual(Head, gsMakeOpIdCInt())) && ArgsLength == 1) {
      //print argument (ArgsLength == 1)
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 0), ShowSorts, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpIdCNeg()) && ArgsLength == 1) {
      //print negation (ArgsLength == 1)
      gsDebugMsg("printing negation\n");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "-");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "-";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 0), ShowSorts, 12);
    } else if (gsIsDataVarIdOpId(Part)) {
      //print untyped data variable or operation identifier
      gsDebugMsg("printing untyped data variable or operation identifier\n");
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    } else if (gsIsOpId(Part) || gsIsDataVarId(Part)) {
      //print data variable or operation identifier
      gsDebugMsg("printing data variable or operation identifier\n");
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
      if (ShowSorts) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ": ";
#endif
        GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
      }
    } else {
      //print data application
      gsDebugMsg("printing data application\n");
#ifdef GS_PRINT_FILE
      if (PrecLevel > 13) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 13) OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, Head, ShowSorts, 13);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, Args, ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
#ifdef GS_PRINT_FILE
      if (PrecLevel > 13) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      if (PrecLevel > 13) OutStream << ")";
#endif
    }
  } else if (gsIsNumber(Part)) {
    //print number
    gsDebugMsg("printing number\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsListEnum(Part)) {
    //print list enumeration
    gsDebugMsg("printing list enumeration\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "[");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "[";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "]");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "]";
#endif
  } else if (gsIsSetEnum(Part) || gsIsBagEnum(Part)) {
    //print set/bag enumeration
    gsDebugMsg("printing set/bag enumeration\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "{");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "{";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "}");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "}";
#endif
  } else if (gsIsSetBagComp(Part)) {
    //print set/bag comprehension
    gsDebugMsg("printing set/bag comprehension\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "{ ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "{ ";
#endif
    GS_PRINT_FUNC(PrintDecl)(OutStream, ATAgetArgument(Part, 0), true);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " | ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " | ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " }");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " }";
#endif
  } else if (gsIsForall(Part) || gsIsExists(Part)) {
    //print universal/existential quantification
    gsDebugMsg("printing universal/existential quantification\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 12) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 12) OutStream << "(";
#endif
#ifdef GS_PRINT_FILE
    fprintf(OutStream, (gsIsForall(Part))?"forall ":"exists ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ((gsIsForall(Part))?"forall ":"exists ");
#endif
    GS_PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ". ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ". ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 12);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 12) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 12) OutStream << ")";
#endif
  } else if (gsIsLambda(Part)) {
    //print lambda abstraction
    gsDebugMsg("printing lambda abstraction\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << "(";
#endif
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "lambda ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "lambda ";
#endif
    GS_PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ". ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ". ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << ")";
#endif
  } else if (gsIsWhr(Part)) {
    //print where clause
    gsDebugMsg("printing where clause\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " whr ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " whr ";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 1), ShowSorts, PrecLevel,
      NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " end");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " end";
#endif
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << ")";
#endif
  } else if (gsIsBagEnumElt(Part)) {
    //print bag enumeration element
    gsDebugMsg("printing bag enumeration element\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ": ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsWhrDecl(Part)) {
    //print where declaration element
    gsDebugMsg("printing where declaration\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " = ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " = ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsActionProcess(Part) || gsIsAction(Part) || gsIsProcess(Part)) {
    //print action or process reference
    gsDebugMsg("printing action or process reference\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "(";
#endif
      GS_PRINT_FUNC(PrintParts)(OutStream, Args, ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ")";
#endif
    }
  } else if (gsIsDelta(Part)) {
    //print delta
    gsDebugMsg("printing delta\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "delta");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "delta";
#endif
  } else if (gsIsTau(Part)) {
    //print tau
    gsDebugMsg("printing tau\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "tau");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "tau";
#endif
  } else if (gsIsChoice(Part)) {
    //print choice
    gsDebugMsg("printing choice\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 1);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " + ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " + ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 0) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 0) OutStream << ")";
#endif
  } else if (gsIsSum(Part)) {
    //print summation
    gsDebugMsg("printing summation\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << "(";
#endif
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "sum ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "sum ";
#endif
    GS_PRINT_FUNC(PrintDecls)(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ". ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ". ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 1) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 1) OutStream << ")";
#endif
  } else if (gsIsMerge(Part) || gsIsLMerge(Part)) {
    //print merge of left merge
    gsDebugMsg("printing merge or left merge\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 2) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 2) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 3);
    if (gsIsMerge(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " || ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " || ";
#endif
    } else {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " ||_ ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " ||_ ";
#endif
    }
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 2);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 2) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 2) OutStream << ")";
#endif
  } else if (gsIsBInit(Part)) {
    //print bounded initialisation
    gsDebugMsg("printing bounded initialisation\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 3) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 3) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 4);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " << ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " << ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 3);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 3) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 3) OutStream << ")";
#endif
  } else if (gsIsCond(Part)) {
    //print conditional
    gsDebugMsg("printing conditional\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 4) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 4) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 12);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " -> ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 5);
    ATermAppl PartElse = ATAgetArgument(Part, 2);
    if (!gsIsDelta(PartElse)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ", ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ", ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, PartElse, ShowSorts, 5);
    }
#ifdef GS_PRINT_FILE
    if (PrecLevel > 4) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 4) OutStream << ")";
#endif
  } else if (gsIsSeq(Part)) {
    //print sequential composition
    gsDebugMsg("printing sequential composition\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 5) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 5) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 6);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " . ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " . ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 5);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 5) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 5) OutStream << ")";
#endif
  } else if (gsIsAtTime(Part)) {
    //print at expression
    gsDebugMsg("printing at expression\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 6) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 6) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 6);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " @ ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " @ ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 12);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 6) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 6) OutStream << ")";
#endif
  } else if (gsIsSync(Part)) {
    //print sync
    gsDebugMsg("printing sync\n");
#ifdef GS_PRINT_FILE
    if (PrecLevel > 7) fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 7) OutStream << "(";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, 8);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " | ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " | ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 7);
#ifdef GS_PRINT_FILE
    if (PrecLevel > 7) fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    if (PrecLevel > 7) OutStream << ")";
#endif
  } else if (gsIsRestrict(Part) || gsIsHide(Part) || gsIsRename(Part) ||
      gsIsComm(Part) || gsIsAllow(Part)) {
    //print process quantification
    gsDebugMsg("printing process quantification\n");
    if (gsIsRestrict(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "restrict");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "restrict";
#endif
    } else if (gsIsHide(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "hide");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "hide";
#endif
    } else if (gsIsRename(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "rename");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "rename";
#endif
    } else if (gsIsComm(Part)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "comm");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "comm";
#endif
    } else {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "allow");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << "allow";
#endif
    }
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "({");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "({";
#endif
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "}, ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "}, ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << ")";
#endif
  } else if (gsIsMultActName(Part)) {
    //print multi action name
    gsDebugMsg("printing multi action name\n");
    GS_PRINT_FUNC(PrintParts)(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, " | ");
  } else if (gsIsRenameExpr(Part)) {
    //print renaming expression
    gsDebugMsg("printing renaming expression\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << " -> ";
#endif
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsCommExpr(Part)) {
    //print communication expression
    gsDebugMsg("printing communication expression\n");
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermAppl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult)) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " -> ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " -> ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, CommResult, ShowSorts, PrecLevel);
    }
  } else if (gsIsNil(Part)) {
    //print nil
    gsDebugMsg("printing nil\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "nil");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "nil";
#endif
  } else if (gsIsUnknown(Part)) {
    //print unknown
    gsDebugMsg("printing unknown\n");
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "unknown");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << "unknown";
#endif
  } else {
    gsErrorMsg("the term %t is not part of the internal format\n", Part);
  }
}

static void GS_PRINT_FUNC(PrintParts)(GS_PRINT_OUTTYPE OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator)
{
  ATermList l = Parts;
  while (!ATisEmpty(l)) {
    if (!ATisEqual(l, Parts) && Separator != NULL) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, Separator);
#endif
#ifdef GS_PRINT_STREAM
      OutStream << Separator;
#endif
    }
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetFirst(l), ShowSorts, PrecLevel);
    if (Terminator != NULL) {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, Terminator);
#endif
#ifdef GS_PRINT_STREAM
      OutStream << Terminator;
#endif
    }
    l = ATgetNext(l);
  }
}

static void GS_PRINT_FUNC(PrintEqns)(GS_PRINT_OUTTYPE OutStream, const ATermList Eqns, bool ShowSorts,
  int PrecLevel)
{
  int EqnsLength = ATgetLength(Eqns);
  if (EqnsLength > 0) {
    int StartPrefix = 0;
    ATermTable VarDeclTable = ATtableCreate(63, 50);
    //VarDeclTable is a hash table with variable declarations as values, where
    //the name of each variable declaration is used a key.
    //Note that the hash table will be increased if at least 32 values are added,
    //This can be avoided by increasing the initial size.
    int i = 0;
    while (i < EqnsLength) {
      //StartPrefix represents the start index of the maximum consistent prefix
      //of variable declarations in Eqns to which Eqns(i) belongs
      //VarDeclTable represents the variable declarations of Eqns from
      //StartPrefix up to i.
      //Check consistency of Eqns(i) with VarDeclTable and add newly declared
      //variables to VarDeclTable.
      ATermAppl Eqn = ATAelementAt(Eqns, i);
      bool Consistent = gsHasConsistentContext(VarDeclTable, Eqn);
      if (Consistent) {
        //add new variables from Eqns(i) to VarDeclTable
        ATermList VarDecls = ATLgetArgument(Eqn, 0);
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
      if (!Consistent || (i == EqnsLength)) {
        //VarDeclTable represents the maximum consistent prefix of variable
        //declarations of Eqns starting at StartPrefix. Print this prefixa and
        //the corresponding equations,and if necessary, update StartPrefix and
        //reset VarDeclTable.
        ATermList VarDecls = ATtableValues(VarDeclTable);
        if (ATgetLength(VarDecls) > 0) {
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "var  ");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << "var  ";
#endif
          GS_PRINT_FUNC(PrintDecls)(OutStream, gsGroupDeclsBySort(ATreverse(VarDecls)),
            ";\n", "     ");
        }
        if (gsIsDataEqn(Eqn)) {
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "eqn  ");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << "eqn  ";
#endif
        } else { //gsIsProcEqn(Eqn)
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "proc ");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << "proc ";
#endif
        }
        GS_PRINT_FUNC(PrintParts)(OutStream,
          ATgetSlice(Eqns, StartPrefix, i), ShowSorts, PrecLevel,
            ";\n", "     ");
        if (i < EqnsLength) {
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << std::endl;
#endif
          StartPrefix = i;
          ATtableReset(VarDeclTable);
        }
      }
    }
    //finalisation after printing all (>0) equations
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << std::endl;
#endif
    ATtableDestroy(VarDeclTable);
  }
}

/*static void GS_PRINT_FUNC(PrintProcEqns)(GS_PRINT_OUTTYPE OutStream, const ATermList ProcEqns, bool ShowSorts,
  int PrecLevel)
{
  int ProcEqnsLength = ATgetLength(ProcEqns);
  if (ProcEqnsLength > 0) {
    int StartPrefix = 0;
    ATermTable VarDeclTable = ATtableCreate(63, 50);
    //VarDeclTable is a hash table with variable declarations as values, where
    //the name of each variable declaration is used a key.
    //Note that the hash table will be increased if at least 32 values are added,
    //This can be avoided by increasing the initial size.
    int i = 0;
    while (i < ProcEqnsLength) {
      //StartPrefix represents the start index of the maximum consistent prefix
      //of variable declarations in ProcEqns to which ProcEqns(i) belongs
      //VarDeclTable represents the variable declarations of ProcEqns
      //from StartPrefix up to i.
      //Check consistency of ProcEqns(i) with VarDeclTable and add newly
      //declared variables to VarDeclTable.
      ATermAppl ProcEqn = ATAelementAt(ProcEqns, i);
      bool Consistent = gsHasConsistentContext(VarDeclTable, ProcEqn);
      if (Consistent) {
        //add new variables from ProcEqns(i) to VarDeclTable
        ATermList VarDecls = ATLgetArgument(ProcEqn, 0);
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
      if (!Consistent || (i == ProcEqnsLength)) {
        //VarDeclTable represents the maximum consistent prefix of variable
        //declarations of ProcEqns starting at StartPrefix. Print this prefix
        //and the corresponding equations,and if necessary, update StartPrefix
        //and reset VarDeclTable.
        ATermList VarDecls = ATtableValues(VarDeclTable);
        if (ATgetLength(VarDecls) > 0) {
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "var  ");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << "var  ";
#endif
          GS_PRINT_FUNC(PrintDecls)(OutStream, gsGroupDeclsBySort(ATreverse(VarDecls)),
            ";\n", "     ");
        }
#ifdef GS_PRINT_FILE
        fprintf(OutStream, "eqn  ");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << "eqn  ";
#endif
        GS_PRINT_FUNC(PrintParts)(OutStream,
          ATgetSlice(ProcEqns, StartPrefix, i), ShowSorts, PrecLevel,
          ";\n", "     ");
        if (i < ProcEqnsLength) {
#ifdef GS_PRINT_FILE
          fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
          OutStream << std::endl;
#endif
          StartPrefix = i;
          ATtableReset(VarDeclTable);
        }
      }
    }
    //finalisation after printing all (>0) process equations
#ifdef GS_PRINT_FILE
    fprintf(OutStream, "\n");
#endif
#ifdef GS_PRINT_STREAM
    OutStream << std::endl;
#endif
    ATtableDestroy(VarDeclTable);
  }
}*/

bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part)
{
  bool Result = true;
  if (gsIsDataEqn(Part) || gsIsProcEqn(Part)) {
    //check consistency of DataVarDecls with the variable declarations
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
  } else if (gsIsOpId(Part) || gsIsDataVarIdOpId(Part))  {
    //Part may be an operation; check that its name does not occur in
    //DataVarDecls
    Result = (ATtableGet(DataVarDecls, ATgetArgument(Part, 0)) == NULL);
  }
  //check consistency in the arguments of Part
  if (Result) {
    AFun Head = ATgetAFun(Part);
    int NrArgs = ATgetArity(Head);      
    for (int i = 0; i < NrArgs && Result; i++) {
      ATerm Arg = ATgetArgument(Part, i);
      if (ATgetType(Arg) == AT_APPL)
        Result = gsHasConsistentContext(DataVarDecls, (ATermAppl) Arg);
      else //ATgetType(Arg) == AT_LIST
        Result = gsHasConsistentContextList(DataVarDecls, (ATermList) Arg);
    }
  }
  return Result;
}

bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts)
{
  bool Result = true;
  ATermList l = Parts;
  while (!ATisEmpty(l) && Result) {
    Result = gsHasConsistentContext(DataVarDecls, ATAgetFirst(l));
    l = ATgetNext(l);
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
        ATreverse(ATLtableGet(SortDeclsTable, ATelementAt(DeclSorts, i))),
        Result);
    }
    ATtableDestroy(SortDeclsTable);
    return Result;
  } else {
    //Decls is empty
    return Decls;
  }
}

static void GS_PRINT_FUNC(PrintDecls)(GS_PRINT_OUTTYPE OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator)
{
  int n = ATgetLength(Decls);
  if (n > 0) {
    for (int i = 0; i < n-1; i++) {
      ATermAppl Decl = ATAelementAt(Decls, i);
      //check if sorts of Decls(i) and Decls(i+1) are equal
      if (ATisEqual(ATgetArgument(Decl, 1),
          ATgetArgument(ATelementAt(Decls, i+1), 1))) {
        GS_PRINT_FUNC(PrintDecl)(OutStream, Decl, false);
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ",");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ",";
#endif
      } else {
        GS_PRINT_FUNC(PrintDecl)(OutStream, Decl, true);
#ifdef GS_PRINT_FILE
        if (Terminator  != NULL) fprintf(OutStream, Terminator);
#endif
#ifdef GS_PRINT_STREAM
        if (Terminator  != NULL) OutStream << Terminator;
#endif
#ifdef GS_PRINT_FILE
        if (Separator  != NULL) fprintf(OutStream, Separator);
#endif
#ifdef GS_PRINT_STREAM
        if (Separator  != NULL) OutStream << Separator;
#endif
      }
    }
    GS_PRINT_FUNC(PrintDecl)(OutStream, ATAelementAt(Decls, n-1), true);
#ifdef GS_PRINT_FILE
    if (Terminator  != NULL) fprintf(OutStream, Terminator);
#endif
#ifdef GS_PRINT_STREAM
    if (Terminator  != NULL) OutStream << Terminator;
#endif
  }
}

static void GS_PRINT_FUNC(PrintDecl)(GS_PRINT_OUTTYPE OutStream, const ATermAppl Decl, const bool ShowSorts)
{
  GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Decl, 0), ShowSorts, 0);
  if (ShowSorts) {
    if (gsIsActId(Decl)) {
      ATermList SortExprs = ATLgetArgument(Decl, 1);
      if (ATgetLength(SortExprs) > 0) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ": ";
#endif
        GS_PRINT_FUNC(PrintParts)(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    } else {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ": ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ": ";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, ATAgetArgument(Decl, 1), ShowSorts, 0);
    }
  }
}

bool gsIsListEnumImpl(ATermAppl DataExpr) {
  if (!gsIsDataAppl(DataExpr) && !gsIsOpId(DataExpr)) return false;
  ATermAppl HeadName = ATAgetArgument(gsGetDataExprHead(DataExpr), 0);
  if (ATisEqual(HeadName, gsMakeOpIdNameCons())) {
    ATermList Args = gsGetDataExprArgs(DataExpr);
    if (ATgetLength(Args) == 2) {
      return gsIsListEnumImpl(ATAelementAt(Args, 1));
    } else {
      return false;
    }
  } else {
    return ATisEqual(HeadName, gsMakeOpIdNameEmptyList());
  }
}

static void GS_PRINT_FUNC(PrintListEnumElts)(GS_PRINT_OUTTYPE OutStream, const ATermAppl DataExpr,
  bool ShowSorts)
{
  ATermAppl HeadName = ATAgetArgument(gsGetDataExprHead(DataExpr), 0);
  if (ATisEqual(HeadName, gsMakeOpIdNameCons())) {
    ATermList Args = gsGetDataExprArgs(DataExpr);
    GS_PRINT_FUNC(PrintPart)(OutStream, ATAelementAt(Args, 0), ShowSorts, 0);
    ATermAppl Arg1 = ATAelementAt(Args, 1);
    if (ATisEqual(ATAgetArgument(gsGetDataExprHead(Arg1), 0),
      gsMakeOpIdNameCons()))
    {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, ", ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << ", ";
#endif
      GS_PRINT_FUNC(PrintListEnumElts)(OutStream, Arg1, ShowSorts);
    }
  }
}

static void GS_PRINT_FUNC(PrintPos)(GS_PRINT_OUTTYPE OutStream, const ATermAppl PosExpr, int PrecLevel)
{
  if (gsIsPosConstant(PosExpr)) {
    char *PosValue = gsPosValue(PosExpr);
#ifdef GS_PRINT_FILE
    fprintf(OutStream, PosValue);
#endif
#ifdef GS_PRINT_STREAM
    OutStream << PosValue;
#endif
    free(PosValue);
  } else {
    GS_PRINT_FUNC(PrintPosMult)(OutStream, PosExpr, PrecLevel, "1");
  }
}

static void GS_PRINT_FUNC(PrintPosMult)(GS_PRINT_OUTTYPE OutStream, const ATermAppl PosExpr, int PrecLevel,
  char *Mult)
{
  ATermAppl Head = gsGetDataExprHead(PosExpr);
  ATermList Args = gsGetDataExprArgs(PosExpr);
  if (ATisEqual(PosExpr, gsMakeOpId1())) {
    //PosExpr is 1; print Mult
#ifdef GS_PRINT_FILE
    fprintf(OutStream, Mult);
#endif
#ifdef GS_PRINT_STREAM
    OutStream << Mult;
#endif
  } else if (ATisEqual(Head, gsMakeOpIdCDub())) {
    //PosExpr is of the form cDub(b,p); print (Mult*2)*v(p) + Mult*v(b)
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl PosArg = ATAelementAt(Args, 1);
    char *NewMult = gsStringDub(Mult, 0);
    if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      //Mult*v(b) = 0
      GS_PRINT_FUNC(PrintPosMult)(OutStream, PosArg, PrecLevel, NewMult);
    } else {
      //Mult*v(b) > 0
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdNameAdd())) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, "(");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << "(";
#endif
      }
      //print (Mult*2)*v(p)
      GS_PRINT_FUNC(PrintPosMult)(OutStream, PosArg, 
        gsPrecOpIdInfixLeft(gsMakeOpIdNameAdd()), NewMult);
#ifdef GS_PRINT_FILE
      fprintf(OutStream, " + ");
#endif
#ifdef GS_PRINT_STREAM
      OutStream << " + ";
#endif
      if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
        //Mult*v(b) = Mult
#ifdef GS_PRINT_FILE
        fprintf(OutStream, Mult);
#endif
#ifdef GS_PRINT_STREAM
        OutStream << Mult;
#endif
      } else if (strcmp(Mult, "1") == 0) {
        //Mult*v(b) = v(b)
        GS_PRINT_FUNC(PrintPart)(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdNameAdd()));
      } else {
        //print Mult*v(b)
#ifdef GS_PRINT_FILE
        fprintf(OutStream, "%s*", Mult);
#endif
#ifdef GS_PRINT_STREAM
        OutStream << Mult << "*";
#endif
        GS_PRINT_FUNC(PrintPart)(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdNameMult()));
      }
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdNameAdd())) {
#ifdef GS_PRINT_FILE
        fprintf(OutStream, ")");
#endif
#ifdef GS_PRINT_STREAM
        OutStream << ")";
#endif
      }
    }
    free(NewMult);
  } else {
    //PosExpr is not a Pos constructor
    if (strcmp(Mult, "1") == 0) {
      GS_PRINT_FUNC(PrintPart)(OutStream, PosExpr, false, PrecLevel);
    } else {
#ifdef GS_PRINT_FILE
      fprintf(OutStream, "%s*", Mult);
#endif
#ifdef GS_PRINT_STREAM
      OutStream << Mult << "*";
#endif
      GS_PRINT_FUNC(PrintPart)(OutStream, PosExpr, false,
        gsPrecOpIdInfixRight(gsMakeOpIdNameMult()));
    }
  }
}
