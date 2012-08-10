#include <boost/test/minimal.hpp>
#include <exception>
#include <sstream>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/core/print.h"

using namespace std;
using namespace mcrl2::trace;

// data generated from data/abc.mcrl2


static char trace_data[] =
"\x6d\x43\x52\x4c\x32\x54\x72\x61\x63\x65\x01\x00\x00\x8b\xaf\x83"
"\x00\x13\x35\x12\x3c\x6c\x69\x73\x74\x20\x63\x6f\x6e\x73\x74\x72"
"\x75\x63\x74\x6f\x72\x3e\x02\x01\x17\x06\x12\x00\x09\x06\x0f\x0a"
"\x02\x00\x01\x0c\x3c\x65\x6d\x70\x74\x79\x20\x6c\x69\x73\x74\x3e"
"\x00\x01\x01\x05\x41\x63\x74\x49\x64\x02\x01\x03\x03\x04\x05\x03"
"\x02\x00\x01\x01\x61\x00\x01\x01\x01\x63\x00\x01\x01\x01\x62\x00"
"\x01\x01\x06\x53\x6f\x72\x74\x49\x64\x01\x01\x02\x02\x08\x07\x03"
"\x50\x6f\x73\x00\x01\x01\x04\x42\x6f\x6f\x6c\x00\x01\x01\x08\x44"
"\x61\x74\x61\x41\x70\x70\x6c\x02\x01\x03\x01\x0a\x01\x00\x04\x4f"
"\x70\x49\x64\x02\x01\x04\x04\x0d\x11\x0c\x0e\x02\x0b\x06\x09\x53"
"\x6f\x72\x74\x41\x72\x72\x6f\x77\x02\x01\x01\x01\x00\x01\x06\x05"
"\x40\x63\x44\x75\x62\x00\x01\x01\x04\x74\x72\x75\x65\x00\x01\x01"
"\x03\x40\x63\x31\x00\x01\x01\x06\x41\x63\x74\x69\x6f\x6e\x02\x01"
"\x03\x01\x02\x02\x00\x01\x03\x4e\x69\x6c\x00\x01\x01\x05\x66\x61"
"\x6c\x73\x65\x00\x01\x01\x04\x70\x61\x69\x72\x02\x01\x03\x01\x00"
"\x01\x10\x00\x80\xa3\x45\x15\x04\x08\x35\x05\x94\x44\x46\x34\x08"
"\xc4\x12\xa9\x40\x23\x22\x51\xa8\xe6\x06\x6a\x25\x40\x2b\x85\x50"
"\x98\x65\x4d\x5c\x02\x22\x1b\x15\x2a\x03\x3c\x98\xea\x8a\x50";


static size_t trace_data_size = sizeof(trace_data);

static bool read_trace(Trace& t, char* buf, size_t size)
{
  stringstream is(ios_base::in | ios_base::out | ios_base::binary);
  is.write(buf,size);

  try
  {
    t.load(is,tfMcrl2);
  }
  catch (runtime_error e)
  {
    BOOST_ERROR(e.what());
    return false;
  }

  return true;
}

void test_next_action(Trace& t, const char* s)
{
  mcrl2::lps::multi_action a = t.currentAction();
  t.increasePosition();
  BOOST_CHECK((a != mcrl2::lps::multi_action()));
  if (a != mcrl2::lps::multi_action())
  {
    string action(pp(a));
    BOOST_CHECK(action == s);
  }
}

int test_main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc,argv);

  Trace t;

  BOOST_REQUIRE(read_trace(t,trace_data,trace_data_size));

  BOOST_REQUIRE(t.number_of_actions() == 3);

  test_next_action(t,"a");
  test_next_action(t,"b(1, true)");
  test_next_action(t,"c");

  return 0;
}
