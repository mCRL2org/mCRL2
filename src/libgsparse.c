#define  NAME      "libgsparse"
#define  LVERSION   "0.1.7"
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
#include "gsfunc.h"

//global declarations
bool gsDebug = false;	                 /* print debug information, if true */

//external declarations
extern ATermAppl gsParse(FILE *SpecFile);/* declared in lexer.l */

//local declarations
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

ATermAppl gsParseSpecification (FILE *SpecStream, int VbLevel)
{
  ATermAppl Result = NULL;
  //check preconditions
  if (VbLevel < 0 || VbLevel > 3) {
    ThrowVM0(NULL, "error: illegal value for the level of verbosity\n");
  }
  if (SpecStream == NULL) {
    ThrowVM0(NULL, "error: formula stream may not be empty\n");
  }
  //set global debug flag
  gsDebug = (VbLevel == 3);
  //parse specification using bison
  if (VbLevel > 1) {
    printf("parsing specification from stream\n");
  }
  Result = gsParse(SpecStream);
  if (Result == NULL) {
    ThrowM0("error: parsing failed\n");
  }
finally:
  if (gsDebug) {
    if (Result != NULL) {
      ATprintf("(gsParseSpecification): return %t\n", Result);
    } else {
      ATprintf("(gsParseSpecification): return NULL\n");
    }
  }
  return Result;
}

bool gsPrintSpecification(FILE *OutStream, ATermAppl Spec, int VbLevel)
{
  bool Result = false;
  //check preconditions
  if (VbLevel < 0 || VbLevel > 3) {
    ThrowVM0(NULL, "error: illegal value for the level of verbosity\n");
  }
  if (Spec == NULL) {
    ThrowVM0(NULL, "error: specification may not be empty\n");
  }
  //set global debug flag
  gsDebug = (VbLevel == 3);
  //print specification
  if (VbLevel > 1) {
    printf("printing specification to stream\n");
  }
  Result = gsPrintPart(OutStream, Spec, 0);
  if (!Result) {
    ThrowM0("error: printing failed\n");
  }
finally:
  if (gsDebug) {
    printf("(gsPrintSpecification): return %s\n", Result?"true":"false");
  }
  return Result;
}


bool gsPrintPart(FILE *OutStream, ATermAppl Part, int PrecLevel)
{
  bool Result = true;
  AFun Head = ATgetAFun(Part);
  char *HeadName = ATgetName(Head);
  if (gsIsSpec(HeadName)) {
    //print specification
    if (gsDebug) { printf("(gsPrintPart): printing specification\n"); }
    ATermList SpecElts = ATLgetArgument(Part, 0);
    int n = ATgetLength(SpecElts);
    for (int i = 0; i < n && Result; i++) {
      if (i > 0) {
        fprintf(OutStream, "\n");
      }
      Result = gsPrintPart(OutStream, ATAelementAt(SpecElts, i), 0);
    }
  } else if (gsIsSortSpec(HeadName)) {
    //print sort specification
    if (gsDebug) { printf("(gsPrintPart): printing sort specification\n"); }
    ATermList SortDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(SortDecls);
    fprintf(OutStream, "sort\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(SortDecls, i), 0);
    }
  } else if (gsIsActSpec(HeadName)) {
    //print action specification
    if (gsDebug) { printf("(gsPrintPart): printing action specification\n"); }
    ATermList ActDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ActDecls);
    fprintf(OutStream, "act\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ActDecls, i), 0);
    }
  } else if (gsIsConsSpec(HeadName)) {
    //print constructor operation specification
    if (gsDebug) { printf("(gsPrintPart): printing constructor operation specification\n"); }
    ATermList ConsDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ConsDecls);
    fprintf(OutStream, "cons\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ConsDecls, i), 0);
    }
  } else if (gsIsMapSpec(HeadName)) {
    //print operation specification
    if (gsDebug) { printf("(gsPrintPart): printing operation specification\n"); }
    ATermList MapDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(MapDecls);
    fprintf(OutStream, "map\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(MapDecls, i), 0);
    }
  } else if (gsIsEqnSpec(HeadName)) {
    //print equation specification
    if (gsDebug) { printf("(gsPrintPart): printing equation specification\n"); }
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
  } else if (gsIsProcSpec(HeadName)) {
    //print process specification
    if (gsDebug) { printf("(gsPrintPart): printing process specification\n"); }
    ATermList ProcDecls = ATLgetArgument(Part, 0);
    int n = ATgetLength(ProcDecls);
    fprintf(OutStream, "proc\n"); 
    for (int i = 0; i < n && Result; i++) {
      fprintf(OutStream, "  "); 
      Result = gsPrintPart(OutStream, ATAelementAt(ProcDecls, i), 0);
    }
  } else if (gsIsInit(HeadName)) {
    //print initialisation
    if (gsDebug) { printf("(gsPrintPart): printing initialisation\n"); }
    fprintf(OutStream, "init\n"); 
    fprintf(OutStream, "  "); 
    Result = gsPrintPart(OutStream, ATAgetArgument(Part, 0), 0);
  } else if (gsIsSortDeclStandard(HeadName)) {
    //print standard sort declaration
    if (gsDebug) { printf("(gsPrintPart): printing standard sort declaration\n"); }
    ATermList Ids = ATLgetArgument(Part, 0);
    int n = ATgetLength(Ids);
    for (int i = 0; i < n && Result; i++) {
      if (i > 0) {
	fprintf(OutStream, ", ");
      }
      Result = gsPrintPart(OutStream, ATAelementAt(Ids, i), 0);
    }
    if (Result) {
      fprintf(OutStream, ";\n");
    }
  } else if (gsIsSortDeclRef(HeadName)) {
    //print sort reference declaration
    if (gsDebug) { printf("(gsPrintPart): printing sort reference declaration\n"); }
    ATermList Ids = ATLgetArgument(Part, 0);
    int n = ATgetLength(Ids);
    for (int i = 0; i < n && Result; i++) {
      if (i > 0) {
	fprintf(OutStream, ", ");
      }
      Result = gsPrintPart(OutStream, ATAelementAt(Ids, i), 0);
    }
    if (Result) {
      fprintf(OutStream, " = ");
      Result = gsPrintPart(OutStream, ATAgetArgument(Part, 1), 0);
      if (Result) {
        fprintf(OutStream, ";\n");
      }
    }
  } else if (gsIsSortStruct(HeadName)) {
    //print structured sort declaration
    if (gsDebug) { printf("(gsPrintPart): printing structured sort declaration\n"); }
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
finally:
  return Result;
}

void gsTest(void)
{  
}
