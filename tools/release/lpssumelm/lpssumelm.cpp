// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssumelm.cpp

#include "mcrl2/lps/io.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/sumelm.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using namespace mcrl2::log;

class sumelm_tool: public input_output_tool
{
  protected:

    using super = input_output_tool;
    bool m_decluster = false;

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("decluster", "first decluster disjunctive conditions", 'c');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_decluster = 0 < parser.options.count("decluster");
    }

  public:

    sumelm_tool()
      : super(
        "lpssumelm",
        "Jeroen Keiren",
        "remove superfluous summations from an LPS",
        "Remove superfluous summations from the linear process specification (LPS) in "
        "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
        "If OUTFILE is not present, stdout is used.")
    {}

    /// Reads a specification from input_file,
    /// applies sum elimination to it and writes the result to output_file.
    bool run() override
    {      
      stochastic_specification spec;
      load_lps(spec, input_filename());

      sumelm_algorithm<stochastic_specification>(spec, m_decluster).run();

      mCRL2log(log::debug) << "Sum elimination completed, saving to " <<  output_filename() << std::endl;
      save_lps(spec, output_filename());
      return true;
    }

};

int main(int argc, char** argv)
{
  return sumelm_tool().execute(argc, argv);
}
