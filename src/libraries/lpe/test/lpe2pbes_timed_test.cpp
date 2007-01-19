// Test program for timed lpe2pbes.
// N.B. Tests for timed/untimed are split into different files due to
// limitations of mcrl22lpe and of the test framework.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include "lpe/pbes.h"
#include "lpe/detail/lpe2pbes.h"
#include "lpe/detail/read_text.h"
#include "test_specifications.h"

using namespace std;
using namespace boost::filesystem;
using namespace lpe;
using namespace lpe::detail;

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  BOOST_CHECK(argc > 1);
  std::string result_file = argv[1];
  //BOOST_CHECK(boost::ends_with(result_file, ".expected_timed_result"));
  std::string formula_file = result_file.substr(0, result_file.find_last_of('.') + 1) + "form"; 
  std::string formula = read_text(formula_file);

  if (exists(result_file))
  {
    pbes result = lpe2pbes(SPEC1, formula, false);
    pbes expected_result;
    expected_result.load(result_file);
    BOOST_CHECK(result == expected_result);
  }
  
  return 0;
}
