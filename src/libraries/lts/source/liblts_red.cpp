#include <vector>
#include <set>
#include <stack>
#include "liblts.h"

using namespace std;

namespace mcrl2
{
namespace lts
{

bool lts::reduce(lts_reduction red)
{
  switch ( red )
  {
    case lts_red_none:
      return true;
    case lts_red_trace:
    case lts_red_obs_trace:
    case lts_red_strong:
    case lts_red_branch:
    default:
      return false;
  }
}
 
}
}
