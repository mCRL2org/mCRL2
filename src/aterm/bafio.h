#ifndef BAFIO_H
#define BAFIO_H

#include "aterm1.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

void   AT_initBafIO(int argc, char *argv[]);
void   AT_getBafVersion(int *major, int *minor);
ATbool AT_interpretBaf(FILE *input, FILE *output);
ATerm  AT_readFromBinaryFile(FILE *f);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif /* BAFIO_H */
