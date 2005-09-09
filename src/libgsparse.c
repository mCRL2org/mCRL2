#define  NAME      "libgsparse"
#define  LVERSION  "0.1.37"
#define  AUTHOR    "Aad Mathijssen"

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>


#include "libgsparse.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"
#include "gsfunc.h"
#include "gslowlevel.h"

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

void gsPrintParts(FILE *OutStream, const ATermList Parts, bool ShowSorts,
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

//implementation

ATermAppl gsParseSpecification (FILE *SpecStream)
{
  ATermAppl Result = NULL;
  //check preconditions
  if (SpecStream == NULL) {
    gsErrorMsg("formula stream may not be empty\n");
  } else {
    //enable constructor functions
    gsEnableConstructorFunctions();
    //parse specification using bison
    gsVerboseMsg("parsing specification from stream\n");
    Result = gsParse(SpecStream);
    //Result = (ATermAppl) ATreadFromFile(SpecStream);
    if (Result == NULL) {
      gsErrorMsg("parsing failed\n");
    } else {
      //type check specification
      gsVerboseMsg("type checking specification\n");
      Result = gsTypeCheck(Result);
      if (Result == NULL) {
        gsErrorMsg("type checking failed\n");
      } else {
        //implement standard data types and type constructors
        gsVerboseMsg("implementing standard data types and type constructors\n");
        Result = gsImplementData(Result);
        if (Result == NULL) {
          gsErrorMsg("data implementation failed\n");
        } else {
          //linearise processes
          gsVerboseMsg("linearising processes\n");
          Result = gsLinearise(Result);
          if (Result == NULL) {
            gsErrorMsg("linearisation failed\n");
          }
        }
      }
    }
  }
  if (Result != NULL) {
    gsDebugMsg("return %t\n", Result);
  } else {
    gsDebugMsg("return NULL\n");
  }
  return Result;
}

void gsPrintSpecification(FILE *OutStream, const ATermAppl Spec)
{
  assert(Spec != NULL);
  //enable constructor functions
  gsEnableConstructorFunctions();
  //print Spec to OutStream
  gsPrintPart(OutStream, Spec, false, 0);
}

ATermAppl gsLinearise(ATermAppl spec)
{
  gsVerboseMsg("linearisation is not yet implemented\n");
  return spec;
}

#define GS_PRINT_FILE
#include "libprint_common.h"

void gsPrintPart(FILE *OutStream, const ATermAppl Part, bool ShowSorts,
  int PrecLevel)
{
	PrintPartFile(OutStream,Part,ShowSorts,PrecLevel);
}

void gsPrintParts(FILE *OutStream, const ATermList Parts, bool ShowSorts,
  int PrecLevel, const char *Terminator, const char *Separator)
{
	PrintPartsFile(OutStream,Parts,ShowSorts,PrecLevel,Terminator,Separator);
}

void f(ATermList *pl)
{
  *pl = ATconcat(*pl, *pl);
}

void gsTest(void)
{
  //initialise ATerm library
  ATerm StackBottom;
  ATinit(0, NULL, &StackBottom);
  //enable constructor functions
  gsEnableConstructorFunctions();
  //build positive constant
/*
  ATermList t = ATmakeList3(
    (ATerm) gsMakeDataExprInt_int(1337),
    (ATerm) gsMakeDataExprInt_int(7331),
    (ATerm) gsMakeDataExprInt_int(8668));
  int n = gsIntValue_int(ATAelementAt(t, 0));
  int m = gsIntValue_int(ATAelementAt(t, 1));
  int k = gsIntValue_int(ATAelementAt(t, 2));
  fprintf(stderr, "%d + %d = %d\n", n, m, k);
  ATermList Substs = ATmakeList1((ATerm)
    gsMakeSubst_Appl(gsMakeDataExprFalse(), gsMakeDataExprTrue()));
  t = (ATermList) gsSubstValues(Substs, (ATerm) t, false);
  n = gsIntValue_int(ATAelementAt(t, 0));
  m = gsIntValue_int(ATAelementAt(t, 1));
  k = gsIntValue_int(ATAelementAt(t, 2));
  fprintf(stderr, "%d + %d = %d\n", n, m, k);
*/
  ATermAppl t = gsMakeDataExprPos_int(1337);
  int n = gsPosValue_int(t);
  fprintf(stderr, "%d\n", n);
}

#ifdef __cplusplus
}
#endif
