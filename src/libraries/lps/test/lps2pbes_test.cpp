// Test program for timed lps2pbes.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "lps/pbes.h"
#include "lps/detail/tools.h"
#include "lps/detail/read_text.h"
#include "test_specifications.h"

using namespace std;
using namespace lps;
using namespace lps::detail;
namespace fs = boost::filesystem;

int test_main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  BOOST_CHECK(argc > 1);
  
  // The dummy file test.test is used to extract the full path of the test directory.
  fs::path dummy_path = fs::system_complete( fs::path( argv[1], fs::native ) );
  fs::path dir = dummy_path.branch_path();
  BOOST_CHECK(fs::is_directory(dir));

  fs::directory_iterator end_iter;
  for ( fs::directory_iterator dir_itr(dir); dir_itr != end_iter; ++dir_itr )
  {
    if ( fs::is_regular( dir_itr->status() ) )
    {
      std::string filename = dir_itr->path().file_string();
      if (boost::ends_with(filename, std::string(".form")))
      {       
        std::string timed_result_file   = filename.substr(0, filename.find_last_of('.') + 1) + "expected_timed_result";
        std::string untimed_result_file = filename.substr(0, filename.find_last_of('.') + 1) + "expected_untimed_result";
        std::string formula = read_text(filename);
        if (fs::exists(timed_result_file))
        {
          pbes result = lps2pbes(SPEC1, formula, true);
          pbes expected_result;
          expected_result.load(timed_result_file);
          bool cmp = (result == expected_result);
          if (!cmp)
            cerr << "ERROR: test " << timed_result_file << " failed!" << endl;
          BOOST_CHECK(cmp);
        }
        if (fs::exists(untimed_result_file))
        {
          pbes result = lps2pbes(SPEC1, formula, false);
          pbes expected_result;
          expected_result.load(untimed_result_file);
          bool cmp = (result == expected_result);
          if (!cmp)
            cerr << "ERROR: test " << untimed_result_file << " failed!" << endl;
          BOOST_CHECK(cmp);
        }
      }
    }
  }
  return 0;
}
