#include <string>
#include <ostream>
#include <sstream>
#include "gsfunc.h"
#include "gslowlevel.h"

using namespace std; 

static void PrintPart(ostream &OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel);
static void PrintParts(ostream &OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator);
static void PrintEqns(ostream &OutStream, const ATermList Eqns, bool ShowSorts,
  int PrecLevel);
static void PrintProcEqns(ostream &OutStream, const ATermList ProcEqns, bool ShowSorts,
  int PrecLevel);
static bool gsHasConsistentContext(const ATermTable DataVarDecls,
  const ATermAppl Part);
static bool gsHasConsistentContextList(const ATermTable DataVarDecls,
  const ATermList Parts);
static ATermList gsGroupDeclsBySort(const ATermList Decls);
static void PrintDecls(ostream &OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator);
static void PrintDecl(ostream &OutStream, const ATermAppl Decl, const bool ShowSorts);
static bool gsIsListEnumImpl(ATermAppl DataExpr);
static void PrintListEnumElts(ostream &OutStream, const ATermAppl DataExpr,
  bool ShowSorts);
static void PrintPos(ostream &OutStream, const ATermAppl PosExpr, int PrecLevel);
static void PrintPosMult(ostream &OutStream, const ATermAppl PosExpr, int PrecLevel,
  char *Mult);

void DataExpressionToStream(ostream &Stream, ATermAppl Expr)
{
	PrintPart(Stream,Expr,false,0);
}


string DataExpressionToString(ATermAppl Expr)
{
	stringstream ss;

	DataExpressionToStream(ss,Expr);

	return ss.str();
}

static void PrintPart(ostream &OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel)
{
  if (ATisQuoted(ATgetAFun(Part)) == ATtrue) {
    //print string
    OutStream << ATgetName(ATgetAFun(Part));
  } else if (gsIsSpecV1(Part)) {
    //print specification
    gsDebugMsg("printing specification\n");
    for (int i = 0; i < 7; i++) {
      PrintPart(OutStream, ATAgetArgument(Part, i), ShowSorts, PrecLevel);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SortDecls) > 0) {
      OutStream << "sort ";
      PrintParts(OutStream, SortDecls, ShowSorts, PrecLevel, ";\n", "     ");
      OutStream << endl;
    }
  } else if (gsIsConsSpec(Part) || gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList OpIds = ATLgetArgument(Part, 0);
    if (ATgetLength(OpIds) > 0) {
      OutStream << (gsIsConsSpec(Part)?"cons ":"map  ");
      PrintDecls(OutStream, OpIds, ";\n", "     ");
      OutStream << endl;
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    PrintEqns(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActIds = ATLgetArgument(Part, 0);
    if (ATgetLength(ActIds) > 0) {
      OutStream << "act  ";
      PrintDecls(OutStream, ActIds, ";\n", "     ");
      OutStream << endl;
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    PrintEqns(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsLPE(Part)) {
    //print LPE
    gsDebugMsg("printing LPE\n");
    //print global variables
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      OutStream << "var  ";
      PrintDecls(OutStream, Vars, ";\n", "     ");
    }
    //print process name and variable declarations
    ATermList VarDecls = ATLgetArgument(Part, 1);
    int VarDeclsLength = ATgetLength(VarDecls);
    OutStream << "proc P";
    if (VarDeclsLength > 0) {
      OutStream << "(";
      PrintDecls(OutStream, VarDecls, NULL, ", ");
      OutStream << ")";
    }
    OutStream << " =";
    //print summations
    ATermList Summands = ATLgetArgument(Part, 2);
    int SummandsLength = ATgetLength(Summands);
    if (SummandsLength == 0) {
      OutStream << " delta\n";
    } else {
      //SummandsLength > 0
      OutStream << "\n       ";
      PrintParts(OutStream, Summands, ShowSorts, PrecLevel,
        NULL, "\n     + ");
      OutStream << ";\n";
    }
    OutStream << endl;
  } else if (gsIsInit(Part)) {
    //print initialisation
    gsDebugMsg("printing initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      OutStream << "var  ";
      PrintDecls(OutStream, Vars, ";\n", "     ");
      OutStream << endl;
    }
    OutStream << "init "; 
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
    OutStream << ";\n";
  } else if (gsIsLPEInit(Part)) {
    //print LPE initialisation
    gsDebugMsg("printing LPE initialisation\n");
    ATermList Vars = ATLgetArgument(Part, 0);
    if (ATgetLength(Vars) > 0) {
      OutStream << "var  ";
      PrintDecls(OutStream, Vars, ";\n", "     ");
    }
    OutStream << "init P"; 
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
      OutStream << "(";
      PrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      OutStream << ")";
    }
    OutStream << ";\n";
  } else if (gsIsSortId(Part)) {
    //print sort identifier
    gsDebugMsg("printing standard sort identifier\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    gsDebugMsg("printing sort reference declaration\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    OutStream << " = ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsDataEqn(Part)) {
    //print data equation (without variables)
    gsDebugMsg("printing data equation\n");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      PrintPart(OutStream, Condition, ShowSorts, 0);
      OutStream << "  ->  ";
    }
    PrintPart(OutStream, ATAgetArgument(Part, 2), ShowSorts, 0);
    OutStream << "  =  ";
    PrintPart(OutStream, ATAgetArgument(Part, 3), ShowSorts, 0);
  } else if (gsIsActId(Part)) {
    //print action identifier
    gsDebugMsg("printing action identifier\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        OutStream << ": ";
        PrintParts(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsProcEqn(Part)) {
    //print process equation (without free variables)
    gsDebugMsg("printing process equation\n");
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
    ATermList DataVarIds = ATLgetArgument(Part, 2);
    if (ATgetLength(DataVarIds) > 0) {
      OutStream << "(";
      PrintDecls(OutStream, DataVarIds, NULL, ", ");
      OutStream << ")";
    }
    OutStream << " = ";
    PrintPart(OutStream, ATAgetArgument(Part, 3), ShowSorts, 0);
  } else if (gsIsProcVarId(Part)) {
    //print process variable
    gsDebugMsg("printing process variable\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    if (ShowSorts) {
      ATermList SortExprs = ATLgetArgument(Part, 1);
      if (ATgetLength(SortExprs) > 0) {
        OutStream << ": ";
        PrintParts(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    }
  } else if (gsIsLPESummand(Part)) {
    //print LPE summand
    gsDebugMsg("printing LPE summand\n");
    //print data summations
    ATermList SumVarDecls = ATLgetArgument(Part, 0);
    if (ATgetLength(SumVarDecls) > 0) {
      OutStream << "sum ";
      PrintDecls(OutStream, SumVarDecls, NULL, ",");
      OutStream << ". ";
    }
    //print condition
    ATermAppl Cond = ATAgetArgument(Part, 1);
    if (!gsIsNil(Cond)) {
      PrintPart(OutStream, Cond, ShowSorts, 0);
      OutStream << " -> ";
    }
    //print multiaction
    bool IsTimed = !gsIsNil(ATAgetArgument(Part, 3));
    PrintPart(OutStream, ATAgetArgument(Part, 2), ShowSorts, (IsTimed)?6:5);
    //print time
    if (IsTimed) {
      OutStream << " @ ";
      PrintPart(OutStream, ATAgetArgument(Part, 3), ShowSorts, 12);
    }
    OutStream << " . ";
    //print process reference
    ATermList Assignments = ATLgetArgument(Part, 4);
    int AssignmentsLength = ATgetLength(Assignments);
    OutStream << "P";
    if (AssignmentsLength > 0) {
      OutStream << "(";
      PrintParts(OutStream, Assignments, ShowSorts, PrecLevel, NULL, ", ");
      OutStream << ")";
    }
  } else if (gsIsMultAct(Part)) {
    //print multiaction
    gsDebugMsg("printing multiaction\n");
    ATermList Actions = ATLgetArgument(Part, 0);
    int ActionsLength = ATgetLength(Actions);
    if (ActionsLength == 0) {
      OutStream << "tau";
    } else {
      //ActionsLength > 0
      if (PrecLevel > 7) OutStream << "(";
      PrintParts(OutStream, Actions, ShowSorts, PrecLevel, NULL, "|");
      if (PrecLevel > 7) OutStream << ")";
    }
  } else if (gsIsAssignment(Part)) {
    //print assignment
    gsDebugMsg("printing assignment\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    OutStream << " := ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsSortList(Part)) {
    //print list sort
    gsDebugMsg("printing list sort\n");
    OutStream << "List(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    OutStream << ")";
  } else if (gsIsSortSet(Part)) {
    //print set sort
    gsDebugMsg("printing set sort\n");
    OutStream << "Set(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    OutStream << ")";
  } else if (gsIsSortBag(Part)) {
    //print bag sort
    gsDebugMsg("printing bag sort\n");
    OutStream << "Bag(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    OutStream << ")";
  } else if (gsIsSortStruct(Part)) {
    //print structured sort
    gsDebugMsg("printing structured sort\n");
    if (PrecLevel > 1) OutStream << "(";
    OutStream << "struct ";
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, PrecLevel,
      NULL, " | ");
    if (PrecLevel > 1) OutStream << ")";
  } else if (gsIsSortArrowProd(Part)) {
    //print product arrow sort
    gsDebugMsg("printing product arrow sort\n");
    if (PrecLevel > 0) OutStream << "(";
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 1, NULL, " # ");
    OutStream << " -> ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    if (PrecLevel > 0) OutStream << ")";
  } else if (gsIsSortArrow(Part)) {
    //print arrow sort
    gsDebugMsg("printing arrow sort\n");
    if (PrecLevel > 0) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 1);
    OutStream << " -> ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    if (PrecLevel > 0) OutStream << ")";
  } else if (gsIsStructCons(Part)) {
    //print structured sort constructor
    gsDebugMsg("printing structured sort constructor\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList StructProjs = ATLgetArgument(Part, 1);
    if (ATgetLength(StructProjs) > 0) {
      OutStream << "(";
      PrintParts(OutStream, StructProjs, ShowSorts, PrecLevel, NULL, ", ");
      OutStream << ")";
    }
    ATermAppl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser)) {
      OutStream << "?";
      PrintPart(OutStream, Recogniser, ShowSorts, PrecLevel);
    }
  } else if (gsIsStructProj(Part)) {
    //print structured sort projection
    gsDebugMsg("printing structured sort projection\n");
    ATermAppl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection)) {
      PrintPart(OutStream, Projection, ShowSorts, PrecLevel);
      OutStream << ": ";
    }
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
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
      OutStream << "[";
      PrintListEnumElts(OutStream, Part, ShowSorts);
      OutStream << "]";
    } else if (gsIsOpIdPrefix(Head) && ArgsLength == 1) {
      //print prefix expression
      gsDebugMsg("printing prefix expression\n");
      if (PrecLevel > 12) OutStream << "(";
      PrintPart(OutStream, Head, ShowSorts, PrecLevel);
      PrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, 12);
      if (PrecLevel > 12) OutStream << ")";
    } else if (gsIsOpIdInfix(Head) && ArgsLength == 2) {
      //print infix expression
      gsDebugMsg("printing infix expression\n");
      ATermAppl HeadName = ATAgetArgument(Head, 0);
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) OutStream << "(";
      PrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts,
        gsPrecOpIdInfixLeft(HeadName));
      OutStream << " ";
      PrintPart(OutStream, Head, ShowSorts, PrecLevel);
      OutStream << " ";
      PrintPart(OutStream, ATAelementAt(Args, 1), ShowSorts,
        gsPrecOpIdInfixRight(HeadName));
      if (PrecLevel > gsPrecOpIdInfix(HeadName)) OutStream << ")";
   } else if (ATisEqual(Head, gsMakeOpId1()) ||
        (ATisEqual(Head, gsMakeOpIdCDub()) && ArgsLength == 2)) {
      //print positive number
      gsDebugMsg("printing positive number %t\n", Part);
      PrintPos(OutStream, Part, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpId0())) {
      //print 0
      OutStream << "0";
    } else if ((ATisEqual(Head, gsMakeOpIdCNat()) ||
        ATisEqual(Head, gsMakeOpIdCInt())) && ArgsLength == 1) {
      //print argument (ArgsLength == 1)
      PrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, PrecLevel);
    } else if (ATisEqual(Head, gsMakeOpIdCNeg()) && ArgsLength == 1) {
      //print negation (ArgsLength == 1)
      gsDebugMsg("printing negation\n");
      OutStream << "-";
      PrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, 12);
    } else if (gsIsDataVarIdOpId(Part)) {
      //print untyped data variable or operation identifier
      gsDebugMsg("printing untyped data variable or operation identifier\n");
      PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    } else if (gsIsOpId(Part) || gsIsDataVarId(Part)) {
      //print data variable or operation identifier
      gsDebugMsg("printing data variable or operation identifier\n");
      PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
      if (ShowSorts) {
        OutStream << ": ";
        PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
      }
    } else {
      //print data application
      gsDebugMsg("printing data application\n");
      if (PrecLevel > 13) OutStream << "(";
      PrintPart(OutStream, Head, ShowSorts, 13);
      OutStream << "(";
      PrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      OutStream << ")";
      if (PrecLevel > 13) OutStream << ")";
    }
  } else if (gsIsNumber(Part)) {
    //print number
    gsDebugMsg("printing number\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
  } else if (gsIsListEnum(Part)) {
    //print list enumeration
    gsDebugMsg("printing list enumeration\n");
    OutStream << "[";
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    OutStream << "]";
  } else if (gsIsSetEnum(Part) || gsIsBagEnum(Part)) {
    //print set/bag enumeration
    gsDebugMsg("printing set/bag enumeration\n");
    OutStream << "{";
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    OutStream << "}";
  } else if (gsIsSetBagComp(Part)) {
    //print set/bag comprehension
    gsDebugMsg("printing set/bag comprehension\n");
    OutStream << "{ ";
    PrintDecl(OutStream, ATAgetArgument(Part, 0), true);
    OutStream << " | ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    OutStream << " }";
  } else if (gsIsForall(Part) || gsIsExists(Part)) {
    //print universal/existential quantification
    gsDebugMsg("printing universal/existential quantification\n");
    if (PrecLevel > 12) OutStream << "(";
    OutStream << (gsIsForall(Part)?"forall ":"exists ");
    PrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    OutStream << ". ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 12);
    if (PrecLevel > 12) OutStream << ")";
  } else if (gsIsLambda(Part)) {
    //print lambda abstraction
    gsDebugMsg("printing lambda abstraction\n");
    if (PrecLevel > 1) OutStream << "(";
    OutStream << "lambda ";
    PrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    OutStream << ". ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
    if (PrecLevel > 1) OutStream << ")";
  } else if (gsIsWhr(Part)) {
    //print where clause
    gsDebugMsg("printing where clause\n");
    if (PrecLevel > 0) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    OutStream << " whr ";
    PrintParts(OutStream, ATLgetArgument(Part, 1), ShowSorts, PrecLevel,
      NULL, ", ");
    OutStream << " end";
    if (PrecLevel > 0) OutStream << ")";
  } else if (gsIsBagEnumElt(Part)) {
    //print bag enumeration element
    gsDebugMsg("printing bag enumeration element\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 0);
    OutStream << ": ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsWhrDecl(Part)) {
    //print where declaration element
    gsDebugMsg("printing where declaration\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    OutStream << " = ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
  } else if (gsIsActionProcess(Part) || gsIsAction(Part) || gsIsProcess(Part)) {
    //print action or process reference
    gsDebugMsg("printing action or process reference\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermList Args = ATLgetArgument(Part, 1);
    if (ATgetLength(Args) > 0) {
      OutStream << "(";
      PrintParts(OutStream, Args, ShowSorts, 0, NULL, ", ");
      OutStream << ")";
    }
  } else if (gsIsDelta(Part)) {
    //print delta
    gsDebugMsg("printing delta\n");
    OutStream << "delta";
  } else if (gsIsTau(Part)) {
    //print tau
    gsDebugMsg("printing tau\n");
    OutStream << "tau";
  } else if (gsIsChoice(Part)) {
    //print choice
    gsDebugMsg("printing choice\n");
    if (PrecLevel > 0) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 1);
    OutStream << " + ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    if (PrecLevel > 0) OutStream << ")";
  } else if (gsIsSum(Part)) {
    //print summation
    gsDebugMsg("printing summation\n");
    if (PrecLevel > 1) OutStream << "(";
    OutStream << "sum ";
    PrintDecls(OutStream, ATLgetArgument(Part, 0), NULL, ", ");
    OutStream << ". ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 1);
    if (PrecLevel > 1) OutStream << ")";
  } else if (gsIsMerge(Part) || gsIsLMerge(Part)) {
    //print merge of left merge
    gsDebugMsg("printing merge or left merge\n");
    if (PrecLevel > 2) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 3);
    if (gsIsMerge(Part)) {
      OutStream << " || ";
    } else {
      OutStream << " ||_ ";
    }
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 2);
    if (PrecLevel > 2) OutStream << ")";
  } else if (gsIsBInit(Part)) {
    //print bounded initialisation
    gsDebugMsg("printing bounded initialisation\n");
    if (PrecLevel > 3) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 4);
    OutStream << " << ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 3);
    if (PrecLevel > 3) OutStream << ")";
  } else if (gsIsCond(Part)) {
    //print conditional
    gsDebugMsg("printing conditional\n");
    if (PrecLevel > 4) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 12);
    OutStream << " -> ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 5);
    ATermAppl PartElse = ATAgetArgument(Part, 2);
    if (!gsIsDelta(PartElse)) {
      OutStream << ", ";
      PrintPart(OutStream, PartElse, ShowSorts, 5);
    }
    if (PrecLevel > 4) OutStream << ")";
  } else if (gsIsSeq(Part)) {
    //print sequential composition
    gsDebugMsg("printing sequential composition\n");
    if (PrecLevel > 5) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 6);
    OutStream << " . ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 5);
    if (PrecLevel > 5) OutStream << ")";
  } else if (gsIsAtTime(Part)) {
    //print at expression
    gsDebugMsg("printing at expression\n");
    if (PrecLevel > 6) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 6);
    OutStream << " @ ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 12);
    if (PrecLevel > 6) OutStream << ")";
  } else if (gsIsSync(Part)) {
    //print sync
    gsDebugMsg("printing sync\n");
    if (PrecLevel > 7) OutStream << "(";
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, 8);
    OutStream << " | ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 7);
    if (PrecLevel > 7) OutStream << ")";
  } else if (gsIsRestrict(Part) || gsIsHide(Part) || gsIsRename(Part) ||
      gsIsComm(Part) || gsIsAllow(Part)) {
    //print process quantification
    gsDebugMsg("printing process quantification\n");
    if (gsIsRestrict(Part)) {
      OutStream << "restrict";
    } else if (gsIsHide(Part)) {
      OutStream << "hide";
    } else if (gsIsRename(Part)) {
      OutStream << "rename";
    } else if (gsIsComm(Part)) {
      OutStream << "comm";
    } else {
      OutStream << "allow";
    }
    OutStream << "({";
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, ", ");
    OutStream << "}, ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, 0);
    OutStream << ")";
  } else if (gsIsMultActName(Part)) {
    //print multi action name
    gsDebugMsg("printing multi action name\n");
    PrintParts(OutStream, ATLgetArgument(Part, 0), ShowSorts, 0, NULL, " | ");
  } else if (gsIsRenameExpr(Part)) {
    //print renaming expression
    gsDebugMsg("printing renaming expression\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    OutStream << " -> ";
    PrintPart(OutStream, ATAgetArgument(Part, 1), ShowSorts, PrecLevel);
  } else if (gsIsCommExpr(Part)) {
    //print communication expression
    gsDebugMsg("printing communication expression\n");
    PrintPart(OutStream, ATAgetArgument(Part, 0), ShowSorts, PrecLevel);
    ATermAppl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult)) {
      OutStream << " -> ";
      PrintPart(OutStream, CommResult, ShowSorts, PrecLevel);
    }
  } else if (gsIsNil(Part)) {
    //print nil
    gsDebugMsg("printing nil\n");
    OutStream << "nil";
  } else if (gsIsUnknown(Part)) {
    //print unknown
    gsDebugMsg("printing unknown\n");
    OutStream << "unknown";
  } else {
    gsErrorMsg("the term %t is not part of the internal format\n", Part);
  }
}

static void PrintParts(ostream &OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator)
{
  ATermList l = Parts;
  while (!ATisEmpty(l)) {
    if (!ATisEqual(l, Parts) && Separator != NULL) {
      OutStream << Separator;
    }
    PrintPart(OutStream, ATAgetFirst(l), ShowSorts, PrecLevel);
    if (Terminator != NULL) {
      OutStream << Terminator;
    }
    l = ATgetNext(l);
  }
}

static void PrintEqns(ostream &OutStream, const ATermList Eqns, bool ShowSorts,
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
          OutStream << "var  ";
          PrintDecls(OutStream, gsGroupDeclsBySort(ATreverse(VarDecls)),
            ";\n", "     ");
        }
        if (gsIsDataEqn(Eqn)) {
          OutStream << "eqn  ";
        } else { //gsIsProcEqn(Eqn)
          OutStream << "proc ";
        }
        PrintParts(OutStream,
          ATgetSlice(Eqns, StartPrefix, i), ShowSorts, PrecLevel,
            ";\n", "     ");
        if (i < EqnsLength) {
          OutStream << endl;
          StartPrefix = i;
          ATtableReset(VarDeclTable);
        }
      }
    }
    //finalisation after printing all (>0) equations
    OutStream << endl;
    ATtableDestroy(VarDeclTable);
  }
}

static void PrintProcEqns(ostream &OutStream, const ATermList ProcEqns, bool ShowSorts,
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
          OutStream << "var  ";
          PrintDecls(OutStream, gsGroupDeclsBySort(ATreverse(VarDecls)),
            ";\n", "     ");
        }
        OutStream << "eqn  ";
        PrintParts(OutStream,
          ATgetSlice(ProcEqns, StartPrefix, i), ShowSorts, PrecLevel,
          ";\n", "     ");
        if (i < ProcEqnsLength) {
          OutStream << endl;
          StartPrefix = i;
          ATtableReset(VarDeclTable);
        }
      }
    }
    //finalisation after printing all (>0) process equations
    OutStream << endl;
    ATtableDestroy(VarDeclTable);
  }
}

static bool gsHasConsistentContext(const ATermTable DataVarDecls,
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

static bool gsHasConsistentContextList(const ATermTable DataVarDecls,
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

static ATermList gsGroupDeclsBySort(const ATermList Decls)
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

static void PrintDecls(ostream &OutStream, const ATermList Decls,
  const char *Terminator, const char *Separator)
{
  int n = ATgetLength(Decls);
  if (n > 0) {
    for (int i = 0; i < n-1; i++) {
      ATermAppl Decl = ATAelementAt(Decls, i);
      //check if sorts of Decls(i) and Decls(i+1) are equal
      if (ATisEqual(ATgetArgument(Decl, 1),
          ATgetArgument(ATelementAt(Decls, i+1), 1))) {
        PrintDecl(OutStream, Decl, false);
        OutStream << ",";
      } else {
        PrintDecl(OutStream, Decl, true);
        if (Terminator  != NULL) OutStream << Terminator;
        if (Separator  != NULL) OutStream << Separator;
      }
    }
    PrintDecl(OutStream, ATAelementAt(Decls, n-1), true);
    if (Terminator  != NULL) OutStream << Terminator;
  }
}

static void PrintDecl(ostream &OutStream, const ATermAppl Decl, const bool ShowSorts)
{
  PrintPart(OutStream, ATAgetArgument(Decl, 0), ShowSorts, 0);
  if (ShowSorts) {
    if (gsIsActId(Decl)) {
      ATermList SortExprs = ATLgetArgument(Decl, 1);
      if (ATgetLength(SortExprs) > 0) {
        OutStream << ": ";
        PrintParts(OutStream, SortExprs, ShowSorts, 2, NULL, " # ");
      }
    } else {
      OutStream << ": ";
      PrintPart(OutStream, ATAgetArgument(Decl, 1), ShowSorts, 0);
    }
  }
}

static bool gsIsListEnumImpl(ATermAppl DataExpr) {
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

static void PrintListEnumElts(ostream &OutStream, const ATermAppl DataExpr,
  bool ShowSorts)
{
  ATermAppl HeadName = ATAgetArgument(gsGetDataExprHead(DataExpr), 0);
  if (ATisEqual(HeadName, gsMakeOpIdNameCons())) {
    ATermList Args = gsGetDataExprArgs(DataExpr);
    PrintPart(OutStream, ATAelementAt(Args, 0), ShowSorts, 0);
    ATermAppl Arg1 = ATAelementAt(Args, 1);
    if (ATisEqual(ATAgetArgument(gsGetDataExprHead(Arg1), 0),
      gsMakeOpIdNameCons()))
    {
      OutStream << ", ";
      PrintListEnumElts(OutStream, Arg1, ShowSorts);
    }
  }
}

static void PrintPos(ostream &OutStream, const ATermAppl PosExpr, int PrecLevel)
{
  if (gsIsPosConstant(PosExpr)) {
    char *PosValue = gsPosValue(PosExpr);
    OutStream << PosValue;
    free(PosValue);
  } else {
    PrintPosMult(OutStream, PosExpr, PrecLevel, "1");
  }
}

static void PrintPosMult(ostream &OutStream, const ATermAppl PosExpr, int PrecLevel,
  char *Mult)
{
  ATermAppl Head = gsGetDataExprHead(PosExpr);
  ATermList Args = gsGetDataExprArgs(PosExpr);
  if (ATisEqual(PosExpr, gsMakeOpId1())) {
    //PosExpr is 1; print Mult
    OutStream << Mult;
  } else if (ATisEqual(Head, gsMakeOpIdCDub())) {
    //PosExpr is of the form cDub(b,p); print (Mult*2)*v(p) + Mult*v(b)
    ATermAppl BoolArg = ATAelementAt(Args, 0);
    ATermAppl PosArg = ATAelementAt(Args, 1);
    char *NewMult = gsStringDub(Mult, 0);
    if (ATisEqual(BoolArg, gsMakeDataExprFalse())) {
      //Mult*v(b) = 0
      PrintPosMult(OutStream, PosArg, PrecLevel, NewMult);
    } else {
      //Mult*v(b) > 0
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdNameAdd())) {
        OutStream << "(";
      }
      //print (Mult*2)*v(p)
      PrintPosMult(OutStream, PosArg, 
        gsPrecOpIdInfixLeft(gsMakeOpIdNameAdd()), NewMult);
      OutStream << " + ";
      if (ATisEqual(BoolArg, gsMakeDataExprTrue())) {
        //Mult*v(b) = Mult
        OutStream << Mult;
      } else if (strcmp(Mult, "1") == 0) {
        //Mult*v(b) = v(b)
        PrintPart(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdNameAdd()));
      } else {
        //print Mult*v(b)
        OutStream << Mult << "*";
        PrintPart(OutStream, BoolArg, false,
          gsPrecOpIdInfixRight(gsMakeOpIdNameMult()));
      }
      if (PrecLevel > gsPrecOpIdInfix(gsMakeOpIdNameAdd())) {
        OutStream << ")";
      }
    }
    free(NewMult);
  } else {
    //PosExpr is not a Pos constructor
    if (strcmp(Mult, "1") == 0) {
      PrintPart(OutStream, PosExpr, false, PrecLevel);
    } else {
      OutStream << Mult << "*";
      PrintPart(OutStream, PosExpr, false,
        gsPrecOpIdInfixRight(gsMakeOpIdNameMult()));
    }
  }
}
