#include <cstdlib>
#include <cstring>
#include <cassert>
#include "mcrl2/utilities/detail/memory_utility.h"
#include "mcrl2/utilities/toolset_version_const.h"
#include "mcrl2/data/detail/rewrite/jitty_jittyc.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"

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

static inline mcrl2::data::data_expression rewrite(const mcrl2::data::data_expression& t);



