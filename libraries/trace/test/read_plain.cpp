#include <boost/test/minimal.hpp>
#include <sstream>
#include <aterm2.h>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/trace/trace.h"

using namespace std;
using namespace mcrl2::trace;

static const char *trace_str = "a\nb(1,true)\nc\n";

static bool read_trace(Trace &t, const char *s)
{
  stringstream ss(s);

  try
  {
    t.load(ss);
  } catch ( ... )
  {
    return false;
  }

  return true;
}

void test_next_action(Trace &t, const char *s)
{
  ATermAppl a = t.nextAction();
  BOOST_CHECK( (a != NULL) && (ATgetType((ATerm) a) == AT_APPL) );
  if ( (a != NULL) && (ATgetType((ATerm) a) == AT_APPL) )
  {
    BOOST_CHECK( ATgetArity(ATgetAFun(a)) == 0 );
    BOOST_CHECK( !strcmp(ATgetName(ATgetAFun(a)),s) );
  }
}

int test_main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc,argv);

  Trace t;

  BOOST_REQUIRE( read_trace(t,trace_str) );

  BOOST_REQUIRE( t.getLength() == 3 );

  test_next_action(t,"a");
  test_next_action(t,"b(1,true)");
  test_next_action(t,"c");

  return 0;
}
