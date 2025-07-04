// Author(s): Frank Stappers, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./lpsinfo.cpp

#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::utilities::tools;

class lpsinfo_tool: public input_tool
{
  protected:
    using super = input_tool;

  public:
    lpsinfo_tool()
      : super(
        "lpsinfo",
        "Wieger Wesselink and Frank Stappers",
        "display basic information about an LPS",
        "Print basic information on the linear process specification (LPS) in INFILE.")
    {}

  public:

    bool run() override
    {
      stochastic_specification spec;
      load_lps(spec, input_filename());
      lps::detail::specification_property_map<stochastic_specification> info(spec);
      std::cout << input_file_message() << "\n\n";
      std::cout << info.info();
      return true;
    }
};

int main(int argc, char** argv)
{
  return lpsinfo_tool().execute(argc, argv);
}
