// Author(s): Jan Friso Groote. Based on pbesconstelm by Wieger Wesselink, Simon Janssen, Tim Willemse, 
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presconstelm.cpp

#include "mcrl2/pres/constelm.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pres/pres_rewriter_tool.h"
#include "mcrl2/pres/pres_input_tool.h"
#include "mcrl2/pres/pres_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pres_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pres_system::tools::pres_input_tool;
using pres_system::tools::pres_output_tool;
using pres_system::tools::pres_rewriter_tool;
using data::tools::rewriter_tool;

class pres_constelm_tool: public pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool> > > >
{
  protected:
    using super = pres_input_tool<pres_output_tool<pres_rewriter_tool<rewriter_tool<input_output_tool>>>>;

    bool m_compute_conditions = false;
    bool m_remove_redundant_equations = false;
    bool m_check_quantifiers = false;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_compute_conditions = parser.options.count("compute-conditions") > 0;
      m_remove_redundant_equations = parser.options.count("remove-equations") > 0;
      m_check_quantifiers = parser.options.count("check-quantifiers") > 0;
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("compute-conditions", "compute propagation conditions", 'c');
      desc.add_option("remove-equations", "remove redundant equations", 'e');
      desc.add_option("check-quantifiers", "also analyse which quantified parameters are constant", 'a');
    }

  public:
    pres_constelm_tool()
      : super(
        "presconstelm",
        "Jan Friso Groote, Wieger Wesselink, Simon Janssen, Tim Willemse", 
        "remove constant parameters from a PRES",
        "Reads a file containing a PRES, and applies constant parameter elimination to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "presconstelm parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  compute conditions: " << std::boolalpha << m_compute_conditions << std::endl;
      mCRL2log(verbose) << "  remove redundant equations: " << std::boolalpha << m_remove_redundant_equations << std::endl;

      // load the pres
      pres p;
      load_pres(p, input_filename(), pres_input_format());

      constelm(p, rewrite_strategy(), rewriter_type(), m_compute_conditions, m_remove_redundant_equations, m_check_quantifiers);

      // save the result
      save_pres(p, output_filename(), pres_output_format());


      return true;
    }

};

int main(int argc, char* argv[])
{
  return pres_constelm_tool().execute(argc, argv);
}
