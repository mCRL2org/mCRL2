#define  NAME      "libgsparse"
#define  LVERSION   "0.1.0"
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
      ATprintf("(MCparseModalFormula): return %t\n", Result);
    } else {
      ATprintf("(MCparseModalFormula): return NULL\n");
    }
  }
  return Result;
}

void gsTest(void)
{  
}
