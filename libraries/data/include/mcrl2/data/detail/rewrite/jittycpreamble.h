#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"
#include "mcrl2/utilities/detail/memory_utility.h"

using namespace mcrl2::data::detail;
using namespace atermpp;

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // _MSC_VER

extern "C" {
  DLLEXPORT void rewrite_init(RewriterCompilingJitty *r);
  DLLEXPORT void rewrite_cleanup();
  DLLEXPORT atermpp::aterm_appl rewrite_external(const atermpp::aterm_appl t);
}


static inline atermpp::aterm_appl rewrite(const atermpp::aterm_appl &t);

static inline atermpp::aterm_appl makeAppl1(const atermpp::function_symbol &a, const atermpp::aterm &h, const atermpp::aterm_appl &t1) 
              { return atermpp::aterm_appl(a,h,reinterpret_cast<const atermpp::aterm &>(t1)); } 
static inline atermpp::aterm_appl makeAppl2(const atermpp::function_symbol &a, const atermpp::aterm &h, const atermpp::aterm_appl &t1, 
                            const atermpp::aterm_appl &t2) 
              { return atermpp::aterm_appl(a,h,t1, t2); }


