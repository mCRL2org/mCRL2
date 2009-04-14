#ifndef GC_H
#define GC_H

#include "abool.h"
#include "aterm2.h"

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

void AT_initGC(int argc, char *argv[], ATerm *bottomOfStack);
void AT_setBottomOfStack(ATerm *bottomOfStack);
void AT_cleanupGC();
void AT_collect_minor();

extern void AT_init_gc_parameters(ATbool low_memory);
extern int gc_min_number_of_blocks;
extern int max_freeblocklist_size;
extern int min_nb_minor_since_last_major;
extern int good_gc_ratio;
extern int small_allocation_rate_ratio;
extern int old_increase_rate_ratio;

void AT_collect();

extern AFun at_parked_symbol;
#define PARK_SYMBOL(afun) at_parked_symbol = (afun)

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif
