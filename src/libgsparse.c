#define  NAME      "libgsparse"
#define  LVERSION  "0.1.14"
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

bool gsPrintPart(FILE *OutStream, ATermAppl Part, int PrecLevel);
/*Pre: OutStream points to a stream to which can be written
       Spec is an ATermAppl containing a part of a GenSpect specification
       PrecLevel indicates the precedence level of the context of Part
       0 <= PrecLevel
  Post:A textual representation of the part is written to OutStream. In this
       textual representation, the top level symbol is parenthesized if its
       precedence level is lower than PrecLevel. 
  Ret: true if everything went ok, false otherwise.
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
  bool Result = false;
  //check preconditions
  if (Spec == NULL) {
    ThrowVM(NULL, "specification may not be empty\n");
  }
  //print specification
  gsVerboseMsg("printing specification to stream\n");
  Result = gsPrintPart(OutStream, Spec, 0);
  if (!Result) {
    ThrowM("printing failed\n");
  }
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

bool gsPrintPart(FILE *OutStream, ATermAppl Part, int PrecLevel)
{
  bool Result = true;
  if (gsIsSpecV1(Part)) {
    //print specification
    gsDebugMsg("printing specification\n");
    ATermList SpecElts = ATLgetArgument(Part, 0);
    int n = ATgetLength(SpecElts);
    for (int i = 0; i < n && Result; i++) {
      if (i > 0) {
        fprintf(OutStream, "\n");
      }
      Result = gsPrintPart(OutStream, ATAelementAt(SpecElts, i), 0);
    }
  } else if (gsIsSortSpec(Part)) {
    //print sort specification
    gsDebugMsg("printing sort specification\n");
    ATermList SortDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(SortDecls);
    fprintf(OutStream, "sort\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(SortDecls, i), 0);
    }
  } else if (gsIsConsSpec(Part)) {
    //print constructor operation specification
    gsDebugMsg("printing constructor operation specification\n");
    ATermList ConsDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ConsDecls);
    fprintf(OutStream, "cons\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ConsDecls, i), 0);
    }
  } else if (gsIsMapSpec(Part)) {
    //print operation specification
    gsDebugMsg("printing operation specification\n");
    ATermList MapDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(MapDecls);
    fprintf(OutStream, "map\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(MapDecls, i), 0);
    }
  } else if (gsIsDataEqnSpec(Part)) {
    //print equation specification
    gsDebugMsg("printing equation specification\n");
    ATermList VarDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(VarDecls);
    if (n > 0) {
      fprintf(OutStream, "var\n"); 
      for (int i = 0; i < n && Result; i++) {
        fprintf(OutStream, "  "); 
        Result = gsPrintPart(OutStream, ATAelementAt(VarDecls, i), 0);
      }
    }
    if (Result) {
      ATermList EqnDecls = ATLgetArgument(Part, 1);
      n = ATgetLength(EqnDecls);
      fprintf(OutStream, "eqn\n"); 
      for (int i = 0; i < n && Result; i++) {
        fprintf(OutStream, "  "); 
        Result = gsPrintPart(OutStream, ATAelementAt(EqnDecls, i), 0);
      }
    }
  } else if (gsIsActSpec(Part)) {
    //print action specification
    gsDebugMsg("printing action specification\n");
    ATermList ActDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ActDecls);
    fprintf(OutStream, "act\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ActDecls, i), 0);
    }
  } else if (gsIsProcEqnSpec(Part)) {
    //print process specification
    gsDebugMsg("printing process specification\n");
    ATermList ProcDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ProcDecls);
    fprintf(OutStream, "proc\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ProcDecls, i), 0);
    }
  } else if (gsIsInit(Part)) {
    //print initialisation
    gsDebugMsg("printing initialisation\n");
    fprintf(OutStream, "init\n"); 
    fprintf(OutStream, "  "); 
    Result = gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsSortId(Part)) {
    //print sort identifier
    gsDebugMsg("printing standard sort declaration\n");
    ATfprintf(OutStream, "%t", ATgetArgument(Part, 0));
    Result = true;
  } else if (gsIsSortRef(Part)) {
    //print sort reference
    gsDebugMsg("printing sort reference declaration\n");
    ATfprintf(OutStream, "%t = %t", ATgetArgument(Part, 0),
      ATgetArgument(Part, 1));
    Result = true;
  } else if (gsIsSortStruct(Part)) {
    //print structured sort declaration
    gsDebugMsg("printing structured sort declaration\n");
    Result = gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
    if (Result) {
      fprintf(OutStream, " = struct ");
      ATermList ConstrDecls = ATLgetArgument(Part, 1);
      int n = ATgetLength(ConstrDecls);
      for (int i = 0; i < n && Result; i++) {
        if (i > 0) {
  	fprintf(OutStream, " | ");
        }
        Result = gsPrintPart(OutStream, ATAelementAt(ConstrDecls, i), 0);
      }
      if (Result) {
        fprintf(OutStream, ";\n");
      }
    }
  }
  return Result;
}

void gsTest(void)
{
}
