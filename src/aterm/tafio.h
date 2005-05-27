#ifndef TAFIO_H
#define TAFIO_H

#include "aterm1.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

#define START_OF_SHARED_TEXT_FILE '!'

extern ATerm AT_readFromSharedTextFile(int *c, FILE *f);

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif /* BAFIO_H */
