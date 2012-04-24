#ifndef GC_H
#define GC_H

#include "mcrl2/aterm/aterm2.h"

namespace aterm
{

const size_t max_freeblocklist_size = 100;

void AT_collect();

}

#endif
