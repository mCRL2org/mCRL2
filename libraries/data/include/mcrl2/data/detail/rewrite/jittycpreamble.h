#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/aterm/aterm.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"
#include "mcrl2/utilities/detail/memory_utility.h"

using namespace aterm_deprecated;
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
  DLLEXPORT atermpp::aterm_appl rewrite_external(const atermpp::aterm_appl &t);
}


static inline atermpp::aterm_appl rewrite(const atermpp::aterm_appl &t);

/* static inline atermpp::aterm_appl makeAppl0(AFun a, atermpp::aterm h) 
              { return ATmakeAppl1(a,(ATerm)h); }
static inline atermpp::aterm_appl makeAppl1(AFun a, atermpp::aterm h, atermpp::aterm_appl t1) 
              { return ATmakeAppl2(a,(ATerm)h,(ATerm)(ATermAppl) t1); }
static inline atermpp::aterm_appl makeAppl2(AFun a, atermpp::aterm h, atermpp::aterm_appl t1, atermpp::aterm_appl t2) 
              { return ATmakeAppl3(a,(ATerm)h,(ATerm)(ATermAppl) t1, (ATerm)(ATermAppl) t2); }
static inline atermpp::aterm_appl makeAppl3(AFun a, atermpp::aterm h, atermpp::aterm_appl t1, atermpp::aterm_appl t2, atermpp::aterm_appl t3) 
              { return ATmakeAppl4(a,(ATerm)h,(ATerm)(ATermAppl) t1, (ATerm)(ATermAppl) t2, (ATerm)(ATermAppl) t3); }
static inline atermpp::aterm_appl makeAppl4(AFun a, atermpp::aterm h, atermpp::aterm_appl t1, atermpp::aterm_appl t2, atermpp::aterm_appl t3, atermpp::aterm_appl t4) 
              { return ATmakeAppl5(a,(ATerm)h,(ATerm)(ATermAppl) t1, (ATerm)(ATermAppl) t2, (ATerm)(ATermAppl) t3, (ATerm)(ATermAppl) t4); }
static inline atermpp::aterm_appl makeAppl5(AFun a, atermpp::aterm h, atermpp::aterm_appl t1, atermpp::aterm_appl t2, atermpp::aterm_appl t3, atermpp::aterm_appl t4, atermpp::aterm_appl t5) 
              { return ATmakeAppl6(a,(ATerm)h,(ATerm)(ATermAppl) t1, (ATerm)(ATermAppl) t2, (ATerm)(ATermAppl) t3, (ATerm)(ATermAppl) t4, (ATerm)(ATermAppl) t5); } */


