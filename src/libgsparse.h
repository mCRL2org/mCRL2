#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include "aterm2.h"

//Global preconditions:
//- the ATerm library has been initialised

ATermAppl gsParseSpecification(FILE *SpecStream);
/*Pre: SpecStream points to a stream from which can be read
  Post:if SpecStream contains GenSpect specification, this specification is
       parsed and returned in the ATerm format; otherwise NULL is returned and
       an appropriate error message is printed to stderr.
*/ 

bool gsPrintSpecification(FILE *OutStream, ATermAppl Spec);
/*Pre: OutStream points to a stream to which can be written
       Spec is an ATermAppl containing a GenSpect specification
  Post:A textual representation of the GenSpect specification is written to
       OutStream.
  Ret: true is everything went ok.
       false, otherwise.
*/

void gsTest(void);
//will be removed in the final version

#ifdef __cplusplus
}
#endif
