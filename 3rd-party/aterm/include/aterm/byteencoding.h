#ifndef BYTEENCODING_H_
#define BYTEENCODING_H_

#include "atypes.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

int BEserializeMultiByteInt(int i, char *c);

/* void BEserializeDouble(double d, char *c); */


int BEdeserializeMultiByteInt(char *c, size_t *i);

/* double BEdeserializeDouble(char *c); */

#ifdef __cplusplus
}
#endif /* __cplusplus */ 

#endif /*BYTEENCODING_H_*/
