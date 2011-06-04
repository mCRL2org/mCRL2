#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
// #include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"

using namespace aterm;
using namespace mcrl2::data::detail;

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // _MSC_VER

extern "C" {
  DLLEXPORT ATermAppl rewrite(ATermAppl);
  DLLEXPORT void set_subst(ATermAppl Var, ATerm Expr);
  DLLEXPORT void clear_subst(ATermAppl Var);
  DLLEXPORT void clear_substs();
  DLLEXPORT void rewrite_init(RewriterCompilingJitty *r);
  DLLEXPORT void rewrite_cleanup();
  DLLEXPORT ATermAppl rewrite(const ATermAppl t);
}

static inline ATermAppl rewrite(const ATerm t) { return rewrite((ATermAppl)t); }

static inline ATermAppl makeAppl0(AFun a, ATerm h) { return ATmakeAppl1(a,h); }
static inline ATermAppl makeAppl1(AFun a, ATerm h, ATermAppl t) { return ATmakeAppl2(a,h,(ATerm) t); }
static inline ATermAppl makeAppl2(AFun a, ATerm h, ATermAppl t1, ATermAppl t2) { return ATmakeAppl3(a,h,(ATerm) t1, (ATerm) t2); }
static inline ATermAppl makeAppl3(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3) { return ATmakeAppl4(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3); }
static inline ATermAppl makeAppl4(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4) { return ATmakeAppl5(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4); }
static inline ATermAppl makeAppl5(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4, ATermAppl t5) { return ATmakeAppl6(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4, (ATerm) t5); }


