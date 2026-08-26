// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparvalues.cpp
/// \brief The pbesparvalues tool which enumerates the possible parameter values of a PBES

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbesparvalues.h"
#include "mcrl2/utilities/input_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using mcrl2::data::tools::rewriter_tool;


class pbes_parvalues_tool: public rewriter_tool<input_tool>
{
  protected:

    using super = rewriter_tool<input_tool>;

    std::size_t m_maximal_number_of_rounds = std::numeric_limits<std::size_t>::max();
    bool m_expand_finite_sorts = false;

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("max", make_mandatory_argument("NUM"),
                      "limit the number of iterations. This may cause certain parameters in some domains to not be found. "
                      "The estimated number of reachable predicate variable instances can also be too low. ",
                      'l');
      desc.add_option("finite", "expand quantifiers over finite sorts before analysis. "
                      "This may improve precision, but can be expensive for large finite sorts.",
                      'f');
    }

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (0 < parser.options.count("max"))
      {
        m_maximal_number_of_rounds = parser.option_argument_as<std::size_t>("max");
      }
      m_expand_finite_sorts = parser.has_option("finite");
    }


  public:

    pbes_parvalues_tool()
      : super(
        "pbesparvalues",
        "Jore Booy",
        "enumerate the values that the parameters of a PBES can take during exploration",
        "Find the values that the parameters of the predicate variables of the PBES in INFILE "
        "can take during exploration. If enumeration fails, try to use --qlimit=N with N an "
        "increasing value. It can also help to use --finite to instantiate quantifiers over "
        "finite sorts first. If INFILE is not present, stdin is used. "
      )
    {}

    bool run() override
    {
      pbes spec;
      load_pbes(spec, m_input_filename);
      data::rewriter r(spec.data(), rewrite_strategy());

      auto result = pbes_system::pbes_parvalues_algorithm<data::rewriter>(spec,
                                                                           r,
                                                                           m_qlimit,
                                                                           m_maximal_number_of_rounds,
                                                                           m_expand_finite_sorts).run();
      for (const auto& domain: result)
      {
        std::cout << "Parameter (" << core::pp(domain.equation) << ", "
                  << core::pp(domain.parameter.name()) << "): " << pp(domain.parameter.sort())
                  << " := " << core::detail::print_set(domain.values)
                  << (domain.unbounded ? " (unbounded)" : "") << std::endl;
      }

      return true;
    }

};

int main(int argc, char** argv)
{
  return pbes_parvalues_tool().execute(argc, argv);
}
