#include <boost/test/minimal.hpp>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

std::string SMALLSPEC =
  "act a,b;                 \n"
  "                         \n"
  "proc X = a.tau.tau.b.X;  \n"
  "                         \n"
  "init X;                  \n"
  ;

void test_bisimulation(const std::string& lps_spec)
{
  specification spec = linearise(lps_spec);
  pbes<> bb  = branching_bisimulation(spec, spec);
  pbes<> sb  = strong_bisimulation(spec, spec);
  pbes<> wb  = weak_bisimulation(spec, spec);
  pbes<> sbe = branching_simulation_equivalence(spec, spec);

  BOOST_CHECK(bb.is_well_typed());
  BOOST_CHECK(sb.is_well_typed());
  BOOST_CHECK(wb.is_well_typed());
  BOOST_CHECK(sbe.is_well_typed());
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)
  test_bisimulation(lps::detail::ABP_SPECIFICATION());
  core::garbage_collect();
  test_bisimulation(SMALLSPEC);
  core::garbage_collect();

  return 0;
}
