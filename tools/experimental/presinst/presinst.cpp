// Author(s): Jan Friso Groote. Based on pbesinst.cpp by Alexander van Dam, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file presinst.cpp
/// \brief Add your file description here.

//#define MCRL2_PRES_EXPRESSION_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_BUILDER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG
//#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <boost/algorithm/string.hpp>

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

#include "mcrl2/pres/pres_input_output_tool.h"
#include "mcrl2/pres/normalize.h"
#include "mcrl2/pres/presinst_finite_algorithm.h"
#include "mcrl2/pres/presinst_algorithm.h"
#include "mcrl2/pres/presinst_strategy.h"

using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::log;
using namespace mcrl2::pres_system;
using pres_system::tools::pres_input_output_tool;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;
using utilities::command_line_parser;
using utilities::interface_description;
using utilities::make_optional_argument;
using utilities::make_enum_argument;

/// The presinst tool.
class presinst_tool: public rewriter_tool<pres_input_output_tool<input_output_tool> >
{
  protected:
    using super = rewriter_tool<pres_input_output_tool<input_output_tool>>;

    presinst_strategy m_strategy = presinst_lazy_strategy;
    std::string m_finite_parameter_selection;
    bool m_remove_redundant_equations = false;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);

      m_strategy = parser.option_argument_as<presinst_strategy>("strategy");
      m_remove_redundant_equations = parser.options.count("remove-equations") > 0;

      if (parser.options.count("select") > 0)
      {
        m_finite_parameter_selection = parser.option_argument("select");
        boost::trim(m_finite_parameter_selection);
        if (m_strategy != presinst_finite_strategy)
        {
          mCRL2log(log::warning) << "Warning: the option --select only has an effect when used together with --strategy=finite." << std::endl;
        }
      }

      if (parser.options.count("equation_limit") > 0)
      {
        int limit = parser.option_argument_as<int>("equation_limit");
        pres_system::detail::set_res_equation_limit(limit);
      }
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.
      add_option("strategy",
                 make_enum_argument<presinst_strategy>("NAME")
                 .add_value(presinst_lazy_strategy)
                 .add_value(presinst_finite_strategy, true),
                 "compute the BES using strategy NAME:", 's').
      add_option("select",
                 make_optional_argument("PARAMS", ""),
                 "select finite parameters that need to be expanded. "
                 "Examples: --select=X1(b:Bool,c:Bool);X2(b:Bool) or --select=*(*:Bool). "
                 "Note: this option only has effect when used together with --strategy=finite.",
                 'f');
      desc.add_hidden_option("equation_limit",
                             make_optional_argument("NAME", "-1"),
                             "Set a limit to the number of generated RES equations",
                             'l');
      desc.add_option("remove-equations", "remove redundant equations", 'e');
    }

  public:
    /// Constructor.
    presinst_tool()
      : super(
        "presinst",
        "Wieger Wesselink; Alexander van Dam and Tim Willemse",
        "compute a BES out of a PRES",
        "Transforms the PRES from INFILE into an equivalent BES and writes it to OUTFILE. "
        "If INFILE is not present, standard input is used. If OUTFILE is not present,   "
        "standard output is used."
        "The format of OUTFILE is determined by its extension (unless it is specified "
        "by an option). The supported formats are:\n"
        "  'pres' for the mCRL2 PRES format,\n"
        "  'res'  for the mCRL2 BES format,\n"
      )
    {}

    /// Runs the algorithm.
    bool run() override
    {
      using namespace mcrl2::pres_system;

      mCRL2log(verbose) << "parameters of presinst:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  strategy:           " << m_strategy << std::endl;
      mCRL2log(verbose) << "  output format:      " << pres_output_format() << std::endl;
      mCRL2log(verbose) << "  remove redundant equations: " << std::boolalpha << m_remove_redundant_equations << std::endl;
      if (m_strategy == presinst_finite_strategy)
      {
        mCRL2log(verbose) << "  parameter selection: " << m_finite_parameter_selection << std::endl;
      }

      // load the pres
      pres p;
      load_pres(p, input_filename(), pres_input_format());

      if (!p.is_closed())
      {
        mCRL2log(log::error) << "The PRES is not closed. Pres2res cannot handle this kind of PRESs"
                             << std::endl << "Computation aborted." << std::endl;
        return false;
      }

      if (m_strategy == presinst_lazy_strategy)
      {
        // TODO: let presinst handle ! and => properly
        if (!is_normalized(p))
        {
          algorithms::normalize(p);
        }
        presinst_algorithm algorithm(p.data(), m_rewrite_strategy, false);
        algorithm.run(p);
        p = algorithm.get_result();
      }
      else if (m_strategy == presinst_finite_strategy)
      {
        try
        {
          algorithms::presinst_finite(p, m_rewrite_strategy, m_finite_parameter_selection);
        }
        catch (const empty_parameter_selection&)
        {
          throw mcrl2::runtime_error("No parameters were selected with the -f option!");
        }
      }

      if (log::logger::get_reporting_level() >= log::verbose)
      {
        if (algorithms::is_res(p))
        {
          mCRL2log(log::debug) << "The result is a BES.\n";
        }
        else
        {
           mCRL2log(log::debug) << "The result is a PRES.\n";
        }
      }

      if (m_remove_redundant_equations)
      {
        std::vector<propositional_variable> V = algorithms::remove_unreachable_variables(p);
        mCRL2log(log::verbose) << algorithms::print_removed_equations(V);
      }

      // save the result
      pres_system::save_pres(p, output_filename(), pres_output_format());

      return true;
    }
};

int main(int argc, char* argv[])
{
  return presinst_tool().execute(argc, argv);
}
