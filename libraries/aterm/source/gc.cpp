#include "mcrl2/aterm/encoding.h"

namespace aterm
{


void ATmarkTerm(const ATerm & )
{
  fprintf(stderr,"No marking is performed anymore\n");  // No mark and sweep is performed anymore.
} 

/*}}}  */

/*{{{  void AT_collect() */

void AT_collect()
{
  fprintf(stderr,"Mark/sweep garbage collection is disabled\n");
}

/*}}}  */

} // namespace aterm
