#ifndef GC_H
#define GC_H

#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

void AT_initGC(int argc, char* argv[], ATerm* bottomOfStack);
void AT_setBottomOfStack(ATerm* bottomOfStack);
void AT_cleanupGC();
// void AT_collect_minor();

extern void AT_init_gc_parameters();
// extern size_t gc_min_number_of_blocks;
extern size_t max_freeblocklist_size;
// extern size_t min_nb_minor_since_last_major;
// extern size_t good_gc_ratio;
// extern size_t small_allocation_rate_ratio;
// extern size_t old_increase_rate_ratio;

void AT_collect();

extern AFun at_parked_symbol;

inline
void PARK_SYMBOL(const AFun afun)
{
  at_parked_symbol = afun;
}

}

#endif
