#ifndef GC_H
#define GC_H

#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

void AT_initGC(int argc, char* argv[], ATerm* bottomOfStack);
void AT_setBottomOfStack(ATerm* bottomOfStack);
void AT_cleanupGC();

const size_t max_freeblocklist_size = 100;

void AT_collect();

extern AFun at_parked_symbol;

inline
void PARK_SYMBOL(const AFun afun)
{
  at_parked_symbol = afun;
}

}

#endif
