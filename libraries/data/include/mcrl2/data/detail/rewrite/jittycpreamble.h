#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "mcrl2/atermpp/detail/function_symbol_constants.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/toolset_version_const.h"

using namespace mcrl2::data::detail;
using namespace atermpp;

#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif // _MSC_VER

void rewrite_init(RewriterCompilingJitty*);
void rewrite_cleanup();
mcrl2::data::data_expression rewrite_external(const mcrl2::data::data_expression&);

extern "C" {
  DLLEXPORT bool init(rewriter_interface* i);
}

bool init(rewriter_interface* i)
{
  if (mcrl2::utilities::MCRL2_VERSION != i->caller_toolset_version)
  {
    i->status = "rewriter version does not match the version of the calling application.";
    return false;
  }
  i->rewrite_external = &rewrite_external;
  i->rewrite_cleanup = &rewrite_cleanup;
  rewrite_init(i->rewriter);
  i->status = "rewriter loaded successfully.";
  return true;
}

static inline atermpp::aterm_appl rewrite(const atermpp::aterm_appl& t);

static inline atermpp::aterm_appl makeAppl1(const atermpp::function_symbol& a, const atermpp::aterm& h, const atermpp::aterm_appl& t1) 
              { return atermpp::aterm_appl(a,h,reinterpret_cast<const atermpp::aterm& >(t1)); } 
static inline atermpp::aterm_appl makeAppl2(const atermpp::function_symbol& a, const atermpp::aterm& h, const atermpp::aterm_appl& t1, 
                            const atermpp::aterm_appl& t2) 
              { return atermpp::aterm_appl(a,h,t1, t2); }


