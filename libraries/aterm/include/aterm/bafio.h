#ifndef BAFIO_H
#define BAFIO_H

#include "aterm1.h"

namespace aterm
{

  void   AT_initBafIO(int argc, char* argv[]);
  void   AT_getBafVersion(int* major, int* minor);
  ATbool AT_interpretBaf(FILE* input, FILE* output);
  ATerm  AT_readFromBinaryFile(FILE* f);

} // namespace aterm

#endif /* BAFIO_H */
