#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <aterm2.h>

#define GS_PRINT_C
#include "libprint_common.h"

void PrintPart_C(FILE *OutStream, const ATerm Part)
{
  PrintPart__C(OutStream, Part);
}

#ifdef __cplusplus
}
#endif
