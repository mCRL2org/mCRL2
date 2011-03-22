#ifndef BYTEENCODING_H_
#define BYTEENCODING_H_

#include "atypes.h"

  size_t BEserializeMultiByteInt(int i, char* c);

  /* void BEserializeDouble(double d, char *c); */


  int BEdeserializeMultiByteInt(char* c, size_t* i);

  /* double BEdeserializeDouble(char *c); */

#endif /*BYTEENCODING_H_*/
