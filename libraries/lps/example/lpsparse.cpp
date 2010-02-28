// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsparse.cpp
/// \brief Tool for testing the mCRL2 parser.

#include <fstream>
#include "mcrl2/core/detail/algorithms.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/atermpp/aterm_appl.h"

using namespace mcrl2;
using mcrl2::utilities::tools::input_tool;

class lps_parse_tool: public input_tool
{
  protected:
    typedef input_tool super;

  public:
    lps_parse_tool()
      : super(
          "lpsparse",
          "Wieger Wesselink",
          "test parsing of an mCRL2 specification",
          "Parse an mCRL2 specification in two different ways and compare the results."
          "If INFILE is not present, standard input is used"
        )
    {}

    atermpp::aterm_appl parse_old(std::string filename)
    {
    	std::ifstream from(filename.c_str());
      return core::detail::parse_process_specification(from);
    }

    atermpp::aterm_appl parse_new(std::string filename)
    {
    	std::ifstream from(filename.c_str());
      return core::detail::parse_process_specification(from);
    }

    bool run()
    {
    	atermpp::aterm_appl t1 = parse_old(input_filename());
    	atermpp::aterm_appl t2 = parse_new(input_filename());
    	std::cout << (t1 == t2 ? "EQUAL" : "NOT EQUAL") << std::endl;
      return true;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps_parse_tool().execute(argc, argv);
}

