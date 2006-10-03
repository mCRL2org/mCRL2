#include <cstdlib>
#include <string>
#include <boost/test/minimal.hpp>
#include "atermpp/aterm.h"
#include "lpe/specification.h"

using namespace std;
using namespace atermpp;
using namespace lpe;

specification read_specification(const string& filename)
{
  specification spec;
  if (!spec.load(filename))
  {
    cerr << "could not load " << filename << endl;
    std::exit(1);
  }

  return spec;
}

int test_main(int, char*[])
{
  ATerm bottom_of_stack;
  ATinit(0, 0, &bottom_of_stack);
  gsEnableConstructorFunctions(); 

  specification spec = read_specification("data/abp_b.lpe");
  LPE_summand s = spec.lpe().summands().front();
  BOOST_CHECK(!s.has_time());
  
  LPE lpe = spec.lpe();
  BOOST_CHECK(lpe.is_well_typed()); 
  BOOST_CHECK(lpe.is_name_clash_free()); 

  return 0;
}
