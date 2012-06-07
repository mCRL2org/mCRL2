#ifndef BAFIO_H
#define BAFIO_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{

void AT_getBafVersion(int* major, int* minor);
bool AT_interpretBaf(FILE* input, FILE* output);
aterm ATreadFromBinaryFile(FILE* file);
bool ATwriteToNamedBinaryFile(aterm t, const char* name);

} // namespace atermpp

#endif /* BAFIO_H */
