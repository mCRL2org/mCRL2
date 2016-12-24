// Author(s): Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesinst.cpp
/// \brief Add your file description here.

//#define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <stdexcept>
#include <iostream>
#include <string>
#include <utility>

#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/bes/pbes_input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbesinst_algorithm.h"
#include "mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h"
#include "mcrl2/pbes/pbesinst_finite_algorithm.h"
#include "mcrl2/pbes/pbesinst_strategy.h"
#include "mcrl2/bes/io.h"
#include "mcrl2/utilities/logger.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using bes::tools::pbes_input_tool;
using bes::tools::bes_output_tool;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::make_enum_argument;

/// The pbesinst tool.
class pbesinst_tool: public rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > >
{
  protected:
    typedef rewriter_tool<pbes_input_tool<bes_output_tool<input_output_tool> > > super;

    pbesinst_strategy m_strategy;
    std::string m_finite_parameter_selection;
    bool m_remove_redundant_equations;
    search_strategy m_search_strategy;
    transformation_strategy m_transformation_strategy;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      if (parser.options.count("select") > 0)
      {
        m_finite_parameter_selection = parser.option_argument("select");
        boost::trim(m_finite_parameter_selection);
      }

      if (parser.options.count("equation_limit") > 0)
      {
        int limit = parser.option_argument_as<int>("equation_limit");
        pbes_system::detail::set_bes_equation_limit(limit);
      }

      m_strategy = parser.option_argument_as<pbesinst_strategy>("strategy");
      m_remove_redundant_equations = parser.options.count("remove-equations") > 0;
      m_search_strategy = parser.option_argument_as<search_strategy>("search");
      m_transformation_strategy = parser.option_argument_as<transformation_strategy>("transformation");
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("strategy",
                 make_enum_argument<pbesinst_strategy>("NAME")
                 .add_value(pbesinst_lazy_strategy)
                 .add_value(pbesinst_alternative_lazy_strategy, true)
                 .add_value(pbesinst_finite_strategy),
                 "compute the BES using strategy NAME:", 's').
      add_option("search",
                 make_enum_argument<search_strategy>("NAME")
                 .add_value(breadth_first, true)
                 .add_value(depth_first)
                 .add_value(breadth_first_short)
                 .add_value(depth_first_short),
                 "search the state space using strategy NAME:", 'z').
      add_option("transformation",
                 make_enum_argument<transformation_strategy>("NAME")
                 .add_value(lazy, true)
                 .add_value(optimize)
                 .add_value(on_the_fly)
                 .add_value(on_the_fly_with_fixed_points),
                 "optimize the BES using strategy NAME:", 'O').
      add_option("select",
                 make_optional_argument("PARAMS", ""),
                 "select finite parameters that need to be expanded. "
                 "Examples: --select=X1(b:Bool,c:Bool);X2(b:Bool) or --select=*(*:Bool)",
                 'f');
      desc.add_hidden_option("equation_limit",
                             make_optional_argument("NAME", "-1"),
                             "Set a limit to the number of generated BES equations",
                             'l');
      desc.add_option("remove-equations", "remove redundant equations", 'e');
    }

  public:
    /// Constructor.
    pbesinst_tool()
      : super(
        "pbesinst",
        "Wieger Wesselink; Alexander van Dam and Tim Willemse",
        "compute a BES out of a PBES",
        "Transforms the PBES from INFILE into an equivalent BES and writes it to OUTFILE. "
        "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
        "standard output is used."
        "The format of OUTFILE is determined by its extension (unless it is specified "
        "by an option). The supported formats are:\n"
        "  'pbes' for the mCRL2 PBES format,\n"
        "  'bes'  for the mCRL2 BES format,\n"
        "  'cwi'  for the CWI BES format\n"
      ),
      m_strategy(pbesinst_lazy_strategy)
    {}

    /// Runs the algorithm.
    bool run()
    {
      using namespace mcrl2::pbes_system;

      mCRL2log(verbose) << "parameters of pbesinst:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  strategy:           " << m_strategy << std::endl;
      mCRL2log(verbose) << "  output format:      " << pbes_output_format() << std::endl;
      mCRL2log(verbose) << "  remove redundant equations: " << std::boolalpha << m_remove_redundant_equations << std::endl;
      if (m_strategy == pbesinst_finite_strategy)
      {
        mCRL2log(verbose) << "  parameter selection: " << m_finite_parameter_selection << std::endl;
      }

      // load the pbes
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());

      if (!p.is_closed())
      {
        mCRL2log(log::error) << "The PBES is not closed. Pbes2bes cannot handle this kind of PBESs"
                             << std::endl << "Computation aborted." << std::endl;
        return false;
      }

      if (m_strategy == pbesinst_lazy_strategy)
      {
        // TODO: let pbesinst handle ! and => properly
        if (!is_normalized(p))
        {
          algorithms::normalize(p);
        }
        pbesinst_algorithm algorithm(p.data(), m_rewrite_strategy, false);
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else if (m_strategy == pbesinst_alternative_lazy_strategy)
      {
        if (!is_normalized(p))
        {
          algorithms::normalize(p);
        }
        mcrl2::data::rewriter datar(p.data(),m_rewrite_strategy);
        pbesinst_alternative_lazy_algorithm algorithm(p.data(), datar, m_search_strategy, m_transformation_strategy);
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else if (m_strategy == pbesinst_finite_strategy)
      {
        try
        {
          algorithms::pbesinst_finite(p, m_rewrite_strategy, m_finite_parameter_selection);
        }
        catch (const empty_parameter_selection&)
        {
          throw mcrl2::runtime_error("No parameters were selected with the -f option!");
        }
      }

      if (log::mcrl2_logger::get_reporting_level() >= log::verbose)
      {
        if (algorithms::is_bes(p))
        {
          mCRL2log(log::debug) << "The result is a BES.\n";
        }
        else
        {
           mCRL2log(log::debug) << "The result is a PBES.\n";
        }
      }

      if (m_remove_redundant_equations)
      {
        std::vector<propositional_variable> V = algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << algorithms::print_removed_equations(V);
      }

      // save the result
      bes::save_pbes(p, output_filename(), pbes_output_format());

      return true;
    }
};

int main(int argc, char* argv[])
{
  return pbesinst_tool().execute(argc, argv);
}
