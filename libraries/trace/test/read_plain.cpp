#include <boost/test/minimal.hpp>
#include <sstream>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/trace/trace.h"

using namespace std;
using namespace mcrl2::trace;

static const char* trace_str = "a\nb(1,true)\nc\n";

static bool read_trace(Trace& t, const char* s)
{
  stringstream ss(s);

  try
  {
    t.load(ss);
  }
  catch (...)
  {
    return false;
  }

  return true;
}

void test_next_action(Trace& t, const char* s)
{
  mcrl2::lps::multi_action a = t.nextAction();
  BOOST_CHECK(a != mcrl2::lps::multi_action());
  if (a != mcrl2::lps::multi_action())
  {
    BOOST_CHECK(a.actions().size() == 1);
    BOOST_CHECK(pp(a)==s);
  }
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc,argv);

  Trace t;

  BOOST_REQUIRE(read_trace(t,trace_str));

  BOOST_REQUIRE(t.number_of_actions() == 3);

  test_next_action(t,"a");
  test_next_action(t,"b(1,true)");
  test_next_action(t,"c");

  return 0;
}
