#define  NAME      "libgsparse"
#define  LVERSION  "0.1.18"
#define  AUTHOR    "Aad Mathijssen"

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
}
#endif

#include "libgsparse.h"
#include "gsdataimpl.h"
#include "gsfunc.h"

//external declarations
extern ATermAppl gsParse(FILE *SpecFile);/* declared in lexer.l */

//local declarations
ATermAppl gsTypeCheck(ATermAppl Spec);
/*Pre: spec represents a specification that adheres to the initial internal
 *     ATerm structure.
 *Post:spec is type checked.
 *Ret: if the type checking went ok, an equivalent version of spec is returned
 *     that adheres to the internal ATerm structure after type checking.
 *     if something went wrong, an appriopriate error message is printed and
 *     NULL is returned.
 */

ATermAppl gsLinearise(ATermAppl Spec);
/*Pre: spec represents a specification that adheres to the internal ATerm
 *     structure after the data implementation phase.
 *Post:The processes of spec are linearised.
 *Ret: if the linearisation went ok, an equivalent version spec is
 *     returned that adheres to the internal ATerm structure after
 *     linearisation.
 *     If something went wrong, an appropriate error message is printed and
 *     NULL is returned.
 */

void gsPrintPart(FILE *OutStream, ATermAppl Part, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Spec is an ATermAppl containing a part of a GenSpect specification
       PrecLevel indicates the precedence level of the context of Part
       0 <= PrecLevel
  Post:A textual representation of the part is written to OutStream. In this
       textual representation, the top level symbol is parenthesized if its
       precedence level is lower than PrecLevel. 
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

bool gsPrintSpecification(FILE *OutStream, ATermAppl Spec)
{
  bool Result = true;
  //check preconditions
  if (Spec == NULL) {
    ThrowVM(false, "specification may not be empty\n");
  }
  //print specification
  gsPrintPart(OutStream, Spec, 0);
finally:
  gsDebugMsg("return %s\n", Result?"true":"false");
  return Result;
}

ATermAppl gsTypeCheck(ATermAppl spec)
{
  gsVerboseMsg("type checking is not yet implemented\n");
  return spec;
}

ATermAppl gsLinearise(ATermAppl spec)
{
  gsVerboseMsg("linearisation is not yet implemented\n");
  return spec;
}

void gsPrintPart(FILE *OutStream, ATermAppl Part, int PrecLevel)
{
  if (ATisQuoted(ATgetAFun(Part)) == ATtrue) {
    //print string
    fprintf(OutStream, ATgetName(ATgetAFun(Part)));
  } else if (gsIsSpecV1(Part)) {
    //print specification
    gsDebugMsg("printing specification\n");
    for (int i = 0; i < 7; i++) {
      gsPrintPart(OutStream, ATAgetArgument(Part, i), 0);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(SortDecls);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        fprintf(OutStream, "%s ", (i == 0)?"sort":"    ");
        gsPrintPart(OutStream, ATAelementAt(SortDecls, i), 0);
        fprintf(OutStream, ";\n");
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsConsSpec(Part)) {
    //print constructor operation specification
    gsDebugMsg("printing constructor operation specification\n");
    ATermList ConsDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ConsDecls);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        ATermAppl OpId = ATAelementAt(ConsDecls, i);
        fprintf(OutStream, "%s ", (i == 0)?"cons":"    ");
        gsPrintPart(OutStream, ATAgetArgument(OpId, 0), 0);
        fprintf(OutStream, ": ");
        gsPrintPart(OutStream, ATAgetArgument(OpId, 1), 0);
        fprintf(OutStream, ";\n");
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList MapDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(MapDecls);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        ATermAppl OpId = ATAelementAt(MapDecls, i);
        fprintf(OutStream, "%s ", (i == 0)?"map ":"    ");
        gsPrintPart(OutStream, ATAgetArgument(OpId, 0), 0);
        fprintf(OutStream, ": ");
        gsPrintPart(OutStream, ATAgetArgument(OpId, 1), 0);
        fprintf(OutStream, ";\n");
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    ATermList DataEqns = ATLgetArgument(Part, 0);
    int n = ATgetLength(DataEqns);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        gsPrintPart(OutStream, ATAelementAt(DataEqns, i), 0);
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ActDecls);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        ATermAppl ActId = ATAelementAt(ActDecls, i);
        fprintf(OutStream, "%s ", (i == 0)?"act ":"    ");
        gsPrintPart(OutStream, ATAgetArgument(ActId, 0), 0);
        ATermList SortExprs = ATLgetArgument(ActId, 1);
        int m = ATgetLength(SortExprs);
        if (m > 0) {
          fprintf(OutStream, ": ");
          for (int j = 0; j < m; j++) {
            if (j > 0) ATfprintf(OutStream, " # ");
            gsPrintPart(OutStream, ATAelementAt(SortExprs, j), 0);
          }
        }
        fprintf(OutStream, ";\n");
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    ATermList ProcDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ProcDecls);
    if (n > 0) {
      for (int i = 0; i < n; i++) {
        fprintf(OutStream, "%s ", (i == 0)?"proc":"    ");
        gsPrintPart(OutStream, ATAelementAt(ProcDecls, i), 0);
        fprintf(OutStream, ";\n");
      }
      fprintf(OutStream, "\n");
    }
  } else if (gsIsInit(Part)) {
    //print initialisation
    gsDebugMsg("printing initialisation\n");
    fprintf(OutStream, "init "); 
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, ";\n");
  } else if (gsIsSortId(Part)) {
    //print sort identifier
    gsDebugMsg("printing standard sort identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    gsDebugMsg("printing sort reference declaration\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
  } else if (gsIsOpId(Part)) {
    //print operation identifier
    gsDebugMsg("printing operation identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsDataEqn(Part)) {
    //print data equation
    gsDebugMsg("printing data equation\n");
    ATermList DataVarIds = ATLgetArgument(Part, 0);
    int n = ATgetLength(DataVarIds);
    for (int i = 0; i < n; i++) {
      ATermAppl DataVarId = ATAelementAt(DataVarIds, i);
      fprintf(OutStream, "%s ", (i == 0)?"var ":"    ");
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 0), 0);
      fprintf(OutStream, ": ");
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 1), 0);
      fprintf(OutStream, ";\n");
    }
    fprintf(OutStream, "eqn  ");
    ATermAppl Condition = ATAgetArgument(Part, 1);
    if (!gsIsNil(Condition)) {
      gsPrintPart(OutStream, Condition, 0);
      fprintf(OutStream, " -> ");
    }
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), 0);
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 3), 0);
    fprintf(OutStream, ";\n");
  } else if (gsIsDataVarId(Part)) {
    //print data variable
    gsDebugMsg("printing data variable\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsActId(Part)) {
    //print action identifier
    gsDebugMsg("printing action identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsProcEqn(Part)) {
    //print process equation
    gsDebugMsg("printing process equation\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    ATermList DataVarIds = ATLgetArgument(Part, 1);
    int n = ATgetLength(DataVarIds);
    if (n > 0) {
      fprintf(OutStream, "(");
      for (int i = 0; i < n; i++) {
        if (i > 0) fprintf(OutStream, ", ");
        ATermAppl DataVarId = ATAelementAt(DataVarIds, i);
        gsPrintPart(OutStream, ATAgetArgument(DataVarId, 0), 0);
        fprintf(OutStream, ": ");
        gsPrintPart(OutStream, ATAgetArgument(DataVarId, 1), 0);
      }
      fprintf(OutStream, ")");
    }
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), 0);
  } else if (gsIsProcVarId(Part)) {
    //print process variable
    gsDebugMsg("printing process variable\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsSortList(Part)) {
    //print list sort
    gsDebugMsg("printing list sort\n");
    fprintf(OutStream, "List(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortSet(Part)) {
    //print set sort
    gsDebugMsg("printing set sort\n");
    fprintf(OutStream, "Set(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortBag(Part)) {
    //print bag sort
    gsDebugMsg("printing bag sort\n");
    fprintf(OutStream, "Bag(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, ")");
  } else if (gsIsSortStruct(Part)) {
    //print structured sort
    gsDebugMsg("printing structured sort\n");
    if (PrecLevel > 2) fprintf(OutStream, "(");
    fprintf(OutStream, "struct ");
    ATermList StructConss = ATLgetArgument(Part, 0);
    int n = ATgetLength(StructConss);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " | ");
      gsPrintPart(OutStream, ATAelementAt(StructConss, i), 0);
    }
    if (PrecLevel > 2) fprintf(OutStream, ")");
  } else if (gsIsSortArrowProd(Part)) {
    //print product arrow sort
    gsDebugMsg("printing product arrow sort\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    ATermList Domain = ATLgetArgument(Part, 0);
    int n = ATgetLength(Domain);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " # ");
      gsPrintPart(OutStream, ATAelementAt(Domain, i), 2);
    }
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 1);
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsSortArrow(Part)) {
    //print product arrow sort
    gsDebugMsg("printing arrow sort\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 2);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 1);
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsStructCons(Part)) {
    //print structured sort constructor
    gsDebugMsg("printing structured sort constructor\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    ATermList StructProjs = ATLgetArgument(Part, 1);
    int n = ATgetLength(StructProjs);
    if (n > 0) {
      fprintf(OutStream, "(");
      for (int i = 0; i < n; i++) {
        if (i > 0) fprintf(OutStream, ", ");
        gsPrintPart(OutStream, ATAelementAt(StructProjs, i), 0);
      }
      fprintf(OutStream, ")");
    }
    ATermAppl Recogniser = ATAgetArgument(Part, 2);
    if (!gsIsNil(Recogniser)) {
      fprintf(OutStream, "?");
      gsPrintPart(OutStream, Recogniser, 0);
    }
  } else if (gsIsStructProj(Part)) {
    //print structured sort projection
    gsDebugMsg("printing structured sort projection\n");
    ATermAppl Projection = ATAgetArgument(Part, 0);
    if (!gsIsNil(Projection)) {
      gsPrintPart(OutStream, Projection, 0);
      fprintf(OutStream, ": ");
    }
    ATermList Domain = ATLgetArgument(Part, 1);
    int n = ATgetLength(Domain);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " # ");
      gsPrintPart(OutStream, ATAelementAt(Domain, i), 2);
    }
  } else if (gsIsDataVarIdOpId(Part)) {
    //print data variable or operation id
    gsDebugMsg("printing data variable or operation identifier\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsDataApplProd(Part)) {
    //print product data application
    gsDebugMsg("printing product data application\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    ATermList Args = ATLgetArgument(Part, 1);
    int n = ATgetLength(Args);
    fprintf(OutStream, "(");
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(Args, i), 0);
    }
    fprintf(OutStream, ")");
  } else if (gsIsDataAppl(Part)) {
    //print data application
    gsDebugMsg("printing data application\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
    fprintf(OutStream, ")");
  } else if (gsIsNumber(Part)) {
    //print number
    gsDebugMsg("printing number\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsListEnum(Part)) {
    //print list enumeration
    gsDebugMsg("printing list enumeration\n");
    ATermList Elts = ATLgetArgument(Part, 0);
    int n = ATgetLength(Elts);
    fprintf(OutStream, "[");
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(Elts, i), 0);
    }
    fprintf(OutStream, "]");
  } else if (gsIsSetEnum(Part)) {
    //print set enumeration
    gsDebugMsg("printing set enumeration\n");
    ATermList Elts = ATLgetArgument(Part, 0);
    int n = ATgetLength(Elts);
    fprintf(OutStream, "{");
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(Elts, i), 0);
    }
    fprintf(OutStream, "}");
  } else if (gsIsBagEnum(Part)) {
    //print bag enumeration
    gsDebugMsg("printing bag enumeration\n");
    ATermList Elts = ATLgetArgument(Part, 0);
    int n = ATgetLength(Elts);
    fprintf(OutStream, "{");
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(Elts, i), 0);
    }
    fprintf(OutStream, "}");
  } else if (gsIsSetBagComp(Part)) {
    //print set/bag comprehension
    gsDebugMsg("printing set/bag comprehension\n");
    fprintf(OutStream, "{ ");
    ATermAppl DataVarId = ATAgetArgument(Part, 0);
    gsPrintPart(OutStream, ATAgetArgument(DataVarId, 0), 0);
    fprintf(OutStream, ": ");
    gsPrintPart(OutStream, ATAgetArgument(DataVarId, 1), 0);
    fprintf(OutStream, " | ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
    fprintf(OutStream, " }");
  } else if (gsIsForall(Part) || gsIsExists(Part) || gsIsLambda(Part)) {
    //print universal/existential quantification or lambda abstraction
    gsDebugMsg(
      "printing universal/existential quantification or lambda abstraction\n");
    if (PrecLevel > 2) fprintf(OutStream, "(");
    if (gsIsForall(Part)) {
      fprintf(OutStream, "forall ");
    } else if (gsIsExists(Part)) {
      fprintf(OutStream, "exists ");
    } else {
      fprintf(OutStream, "lambda ");
    }
    ATermList DataVarIds = ATLgetArgument(Part, 0);
    int n = ATgetLength(DataVarIds);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      ATermAppl DataVarId = ATAelementAt(DataVarIds, i);
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 0), 0);
      fprintf(OutStream, ": ");
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 1), 0);
    }
    fprintf(OutStream, ". ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 2);
    if (PrecLevel > 2) fprintf(OutStream, ")");
  } else if (gsIsWhr(Part)) {
    //print where clause
    gsDebugMsg("printing where clause\n");
    if (PrecLevel > 1) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 2);
    ATermList WhrDecls = ATLgetArgument(Part, 1);
    int n = ATgetLength(WhrDecls);
    fprintf(OutStream, " whr ");
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(WhrDecls, i), 0);
    }
    fprintf(OutStream, " end");
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsBagEnumElt(Part)) {
    //print bag enumeration element
    gsDebugMsg("printing bag enumeration element\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, ": ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
  } else if (gsIsWhrDecl(Part)) {
    //print where declaration element
    gsDebugMsg("printing where declaration\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, " = ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
  } else if (gsIsActionProcess(Part) || gsIsAction(Part) || gsIsProcess(Part)) {
    //print action or process reference
    gsDebugMsg("printing action or process reference\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    ATermList Args = ATLgetArgument(Part, 1);
    int n = ATgetLength(Args);
    if (n > 0) {
      fprintf(OutStream, "(");
      for (int i = 0; i < n; i++) {
        if (i > 0) fprintf(OutStream, ", ");
        gsPrintPart(OutStream, ATAelementAt(Args, i), 0);
      }
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
    if (PrecLevel > 1) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 1);
    fprintf(OutStream, " + ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 2);
    if (PrecLevel > 1) fprintf(OutStream, ")");
  } else if (gsIsSum(Part)) {
    //print summation
    gsDebugMsg("printing summation\n");
    if (PrecLevel > 2) fprintf(OutStream, "(");
    fprintf(OutStream, "sum ");
    ATermList DataVarIds = ATLgetArgument(Part, 0);
    int n = ATgetLength(DataVarIds);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      ATermAppl DataVarId = ATAelementAt(DataVarIds, i);
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 0), 0);
      fprintf(OutStream, ": ");
      gsPrintPart(OutStream, ATAgetArgument(DataVarId, 1), 0);
    }
    fprintf(OutStream, ". ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 2);
    if (PrecLevel > 2) fprintf(OutStream, ")");
  } else if (gsIsMerge(Part) || gsIsLMerge(Part)) {
    //print merge of left merge
    gsDebugMsg("printing merge or left merge\n");
    if (PrecLevel > 3) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 3);
    if (gsIsMerge(Part)) {
      fprintf(OutStream, " || ");
    } else {
      fprintf(OutStream, " ||_ ");
    }
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 4);
    if (PrecLevel > 3) fprintf(OutStream, ")");
  } else if (gsIsBInit(Part)) {
    //print bounded initialisation
    gsDebugMsg("printing bounded initialisation\n");
    if (PrecLevel > 4) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 4);
    fprintf(OutStream, " << ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 5);
    if (PrecLevel > 4) fprintf(OutStream, ")");
  } else if (gsIsCond(Part)) {
    //print conditional
    gsDebugMsg("printing conditional\n");
    if (PrecLevel > 5) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 6);
    fprintf(OutStream, ", ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 2), 6);
    if (PrecLevel > 5) fprintf(OutStream, ")");
  } else if (gsIsSeq(Part)) {
    //print sequential composition
    gsDebugMsg("printing sequential composition\n");
    if (PrecLevel > 6) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 6);
    fprintf(OutStream, " . ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 7);
    if (PrecLevel > 6) fprintf(OutStream, ")");
  } else if (gsIsAtTime(Part)) {
    //print at expression
    gsDebugMsg("printing at expression\n");
    if (PrecLevel > 7) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 7);
    fprintf(OutStream, " @ ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
    if (PrecLevel > 7) fprintf(OutStream, ")");
  } else if (gsIsSync(Part)) {
    //print sync
    gsDebugMsg("printing sync\n");
    if (PrecLevel > 8) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 8);
    fprintf(OutStream, " | ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
    if (PrecLevel > 8) fprintf(OutStream, ")");
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
    ATermList Elts = ATLgetArgument(Part, 0);
    int n = ATgetLength(Elts);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, ", ");
      gsPrintPart(OutStream, ATAelementAt(Elts, i), 0);
    }
    fprintf(OutStream, "}, ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
    fprintf(OutStream, ")");
  } else if (gsIsMultActName(Part)) {
    //print multi action name
    gsDebugMsg("printing multi action name\n");
    ATermList ActNames = ATLgetArgument(Part, 0);
    int n = ATgetLength(ActNames);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " | ");
      gsPrintPart(OutStream, ATAelementAt(ActNames, i), 0);
    }
  } else if (gsIsRenameExpr(Part)) {
    //print renaming expression
    gsDebugMsg("printing renaming expression\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
  } else if (gsIsCommExpr(Part)) {
    //print communication expression
    gsDebugMsg("printing communication expression\n");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    ATermAppl CommResult = ATAgetArgument(Part, 1);
    if (!gsIsNil(CommResult)) {
      fprintf(OutStream, " -> ");
      gsPrintPart(OutStream, CommResult, 0);
    }
  }
}

void gsTest(void)
{
}
