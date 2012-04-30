#ifndef _ATERM_H
#define _ATERM_H

#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

/* typedef struct ProtEntry
{
  struct ProtEntry* next;
  const ATerm*            start;
  size_t     size;
} ProtEntry;

extern ProtEntry** at_prot_table;
extern size_t at_prot_table_size;
extern ProtEntry* at_prot_memory;
extern ATermProtFunc* at_prot_functions;
extern size_t at_prot_functions_count; */

// void AT_markTerm(ATerm t);

// void AT_unmarkIfAllMarked(const ATerm &t);
size_t  AT_calcUniqueAFuns(const ATerm &t);
void AT_cleanup(void);

} // namespace aterm

#endif
