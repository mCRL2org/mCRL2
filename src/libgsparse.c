#define  NAME      "libgsparse"
#define  LVERSION  "0.1.17"
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
  gsVerboseMsg("printing specification to stream\n");
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
      if (i > 0) {
        fprintf(OutStream, "\n");
      }
      gsPrintPart(OutStream, ATAgetArgument(Part, i), 0);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(SortDecls);
    for (int i = 0; i < n; i++) {
      fprintf(OutStream, "%s ", (i == 0)?"sort":"    ");
      gsPrintPart(OutStream, ATAelementAt(SortDecls, i), 0);
      fprintf(OutStream, ";\n");
    }
  } else if (gsIsConsSpec(Part)) {
    //print constructor operation specification
    gsDebugMsg("printing constructor operation specification\n");
    ATermList ConsDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ConsDecls);
    for (int i = 0; i < n; i++) {
      ATermAppl OpId = ATAelementAt(ConsDecls, i);
      fprintf(OutStream, "%s ", (i == 0)?"cons":"    ");
      gsPrintPart(OutStream, ATAgetArgument(OpId, 0), 0);
      fprintf(OutStream, ": ");
      gsPrintPart(OutStream, ATAgetArgument(OpId, 1), 0);
      fprintf(OutStream, ";\n");
    }
  } else if (gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList MapDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(MapDecls);
    for (int i = 0; i < n; i++) {
      ATermAppl OpId = ATAelementAt(MapDecls, i);
      fprintf(OutStream, "%s ", (i == 0)?"map ":"    ");
      gsPrintPart(OutStream, ATAgetArgument(OpId, 0), 0);
      fprintf(OutStream, ": ");
      gsPrintPart(OutStream, ATAgetArgument(OpId, 1), 0);
      fprintf(OutStream, ";\n");
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    ATermList DataEqns = ATLgetArgument(Part, 0);
    int n = ATgetLength(DataEqns);
    for (int i = 0; i < n; i++) {
      gsPrintPart(OutStream, ATAelementAt(DataEqns, i), 0);
    }
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ActDecls);
    for (int i = 0; i < n; i++) {
      ATermAppl ActId = ATAelementAt(ActDecls, i);
      fprintf(OutStream, "%s ", (i == 0)?"act ":"    ");
      gsPrintPart(OutStream, ATAgetArgument(ActId, 0), 0);
      fprintf(OutStream, ": ");
      ATermList SortExprs = ATLgetArgument(ActId, 1);
      int m = ATgetLength(SortExprs);
      for (int j = 0; j < m; j++) {
        if (j > 0) ATfprintf(OutStream, ", ");
        gsPrintPart(OutStream, ATAelementAt(SortExprs, i), 0);
      }
      fprintf(OutStream, ";\n");
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    ATermList ProcDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ProcDecls);
    for (int i = 0; i < n; i++) {
      fprintf(OutStream, "%s ", (i == 0)?"proc":"    ");
      gsPrintPart(OutStream, ATAelementAt(ProcDecls, i), 0);
      fprintf(OutStream, ";\n");
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
    if (PrecLevel >= 3) fprintf(OutStream, "(");
    fprintf(OutStream, "struct ");
    ATermList StructConss = ATLgetArgument(Part, 0);
    int n = ATgetLength(StructConss);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " | ");
      gsPrintPart(OutStream, ATAelementAt(StructConss, i), 3);
    }
    if (PrecLevel >= 3) fprintf(OutStream, ")");
  } else if (gsIsSortArrowProd(Part)) {
    //print product arrow sort
    gsDebugMsg("printing product arrow sort\n");
    if (PrecLevel >= 2) fprintf(OutStream, "(");
    ATermList Domain = ATLgetArgument(Part, 0);
    int n = ATgetLength(Domain);
    for (int i = 0; i < n; i++) {
      if (i > 0) fprintf(OutStream, " # ");
      gsPrintPart(OutStream, ATAelementAt(Domain, i), 2);
    }
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 1);
    if (PrecLevel >= 2) fprintf(OutStream, ")");
  } else if (gsIsSortArrow(Part)) {
    //print product arrow sort
    gsDebugMsg("printing arrow sort\n");
    if (PrecLevel >= 2) fprintf(OutStream, "(");
    gsPrintPart(OutStream, ATAgetArgument(Part, 0), 2);
    fprintf(OutStream, " -> ");
    gsPrintPart(OutStream, ATAgetArgument(Part, 1), 1);
    if (PrecLevel >= 2) fprintf(OutStream, ")");
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
  }
}

void gsTest(void)
{
}
