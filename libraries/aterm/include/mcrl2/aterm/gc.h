#ifndef GC_H
#define GC_H

#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

void AT_initGC(ATerm* bottomOfStack);
void AT_setBottomOfStack(ATerm* bottomOfStack);
void AT_cleanupGC();

const size_t max_freeblocklist_size = 100;

void AT_collect();

extern AFun at_parked_symbol;
extern size_t total_nodes;

inline
void PARK_SYMBOL(const AFun afun)
{
  at_parked_symbol = afun;
}

}

#endif
