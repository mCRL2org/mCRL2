#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <aterm2.h>
#include <stdbool.h>
#include "libgsparse.h"
#include "gslowlevel.h"

#define GS_PRINT_FILE
#include "libprint_common.h"

void PrettyPrint(FILE *OutStream, const ATerm Part)
{
  if (ATgetType(Part) == AT_APPL) {
    PrintPartFile(OutStream, (ATermAppl) Part, false, 0);
  } else if (ATgetType(Part) == AT_LIST) {
    fprintf(OutStream, "[");
    PrintPartsFile(OutStream, (ATermList) Part, false, 0, "", ", ");
    fprintf(OutStream, "]");
  } else {
    gsErrorMsg("ATerm Part is not an ATermAppl or an ATermList\n");
  }
}

#ifdef __cplusplus
}
#endif
