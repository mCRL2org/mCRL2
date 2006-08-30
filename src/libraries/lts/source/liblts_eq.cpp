#include <vector>
#include <set>
#include <stack>
#include "libprint_c.h"
#include "liblts.h"
#include "detail/liblts_bisim.h"

using namespace std;

namespace mcrl2
{
namespace lts
{

void set_eq_options_defaults(lts_eq_options &opts)
{
  opts.reduce.add_class_to_state = false;
}

bool lts::reduce(lts_equivalence eq, lts_eq_options &opts)
{
  switch ( eq )
  {
    case lts_eq_none:
      return true;
    case lts_eq_strong:
      return bisimulation_reduce(*this,false,opts.reduce.add_class_to_state);
    case lts_eq_branch:
      return bisimulation_reduce(*this,true,opts.reduce.add_class_to_state);
    case lts_eq_trace:
    case lts_eq_obs_trace:
    default:
      return false;
  }
}
 
bool lts::compare(lts &l, lts_equivalence eq, lts_eq_options &opts)
{
  switch ( eq )
  {
    case lts_eq_none:
      return false;
    case lts_eq_strong:
      return bisimulation_compare(*this,l,false);
    case lts_eq_branch:
      return bisimulation_compare(*this,l,true);
    case lts_eq_trace:
    case lts_eq_obs_trace:
    default:
      gsErrorMsg("comparision for this equivalence is not yet implemented\n");
      return false;
  }
}
 
}
}
