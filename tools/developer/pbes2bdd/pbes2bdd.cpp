// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bdd.cpp

#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/bdd.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities::tools;

class pbes2bdd_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

    bool unary_encoding = false;

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      unary_encoding = parser.has_option("unary-encoding");
    }

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("unary-encoding", "use a unary encoding of the predicate variables", 'u');
      desc.add_option("use-new-implementation", "use the new implementation based on SRF format", 'n');
    }

  public:
    pbes2bdd_tool()
      : super("pbes2bdd",
              "Wieger Wesselink",
              "transforms a restricted class of PBESs (in boolean format) to BDD",
              "Transform the PBES in INFILE and write the result to OUTFILE. If OUTFILE "
              "is not present, stdout is used. If INFILE is not present, stdin is used."
             )
    {}

    bool run() override
    {
      pbes_system::pbes pbesspec = pbes_system::detail::load_pbes(input_filename());
      std::string result;
      result = pbes_system::bdd::pbes2bdd(pbesspec, unary_encoding);
      utilities::detail::write_text(output_filename(), result);
      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbes2bdd_tool().execute(argc, argv);
}
