#ifndef BAFIO_H
#define BAFIO_H

#include "aterm1.h"

extern "C"
{

void   AT_initBafIO(int argc, char* argv[]);
void   AT_getBafVersion(int* major, int* minor);
bool AT_interpretBaf(FILE* input, FILE* output);
ATerm  AT_readFromBinaryFile(FILE* f);

} // extern "C"

#endif /* BAFIO_H */
