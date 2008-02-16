#include <iostream>
#include <boost/test/minimal.hpp>
#include <aterm2.h>
#include <mcrl2/utilities/aterm_ext.h>
#include <mcrl2/core/messaging.h>
#include <mcrl2/utilities/numeric_string.h>
#include <mcrl2/core/struct.h>

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

int test_main(int argc, char** argv)
{
  MCRL2_ATERM_INIT(argc, argv);

  ATermAppl x = gsString2ATermAppl("x");
  ATermList xl = ATmakeList1((ATerm) x);
  int index = ATindexOf(xl, (ATerm) x, -1);
  BOOST_CHECK(index == 0);
}
