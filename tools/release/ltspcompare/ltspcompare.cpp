// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltscompare.cpp
/// \brief A tool to calculate whether probabilistic transition systems are equival

#include "mcrl2/utilities/input_tool.h"

#include "mcrl2/lts/lts_probabilistic_equivalence.h"
#include "mcrl2/lts/detail/liblts_pbisim_grv.h"
#include "mcrl2/lts/detail/liblts_pbisim_bem.h"
#include "mcrl2/lts/lts_io.h"

// #include "mcrl2/lps/exploration_strategy.h"

using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::log;

struct t_tool_options
{
  std::string                   name_for_first;
  std::string                   name_for_second;
  lts_type                      format_for_first;
  lts_type                      format_for_second;
  lts_probabilistic_equivalence equivalence;
  lts_probabilistic_preorder    preorder;
  std::vector<std::string>      tau_actions;   // Actions with these labels must be considered equal to tau.
  
  t_tool_options():
    name_for_first(""),
    name_for_second(""),
    format_for_first(lts_none),
    format_for_second(lts_none),
    equivalence(lts_probabilistic_eq_none),
    preorder(lts_probabilistic_pre_none)
  {}
};

typedef  input_tool ltscompare_base;
class ltspcompare_tool : public ltscompare_base
{
  private:
    t_tool_options tool_options;

    // Check whether preconditions w.r.t. equivalence and pre-order are satisfied.
    // This is needed to make sure wrappers can call this tool without the --help,
    // --equivalence or --preorder options
    void check_preconditions()
    {
      if (tool_options.equivalence != lts_probabilistic_eq_none && tool_options.preorder != lts_probabilistic_pre_none)
      {
        throw mcrl2::runtime_error("options -e/--equivalence and -p/--preorder cannot be used simultaneously");
      }

      if (tool_options.equivalence == lts_probabilistic_eq_none && tool_options.preorder == lts_probabilistic_pre_none)
      {
        throw mcrl2::runtime_error("one of the options -e/--equivalence and -p/--preorder must be used");
      }

      if (tool_options.name_for_first.empty() && tool_options.name_for_second.empty())
      {
        throw mcrl2::runtime_error("too few file arguments");
      }
    }

  public:
    ltspcompare_tool() :
      ltscompare_base("ltspcompare","Jan Friso Groote",
                      "compare two LTSs",
                      "Determine whether the labelled transition systems in INFILE1 and INFILE2 are related by some probabilistic equivalence or preorder. "
                      "If INFILE1 is not supplied, stdin is used.\n"
                      "\n"
                      "The input formats are determined by the contents of INFILE1 and INFILE2. "
                      "Options --in1 and --in2 can be used to force the input format of INFILE1 and INFILE2, respectively. "
                      "The supported formats are:\n"
                      + mcrl2::lts::detail::supported_lts_formats_text()
                     )
    {
    }

  private:

    template <class LTS_TYPE>
    bool lts_probabilistic_compare(void)
    {
      LTS_TYPE l1,l2;
      l1.load(tool_options.name_for_first);
      l2.load(tool_options.name_for_second);

      l1.record_hidden_actions(tool_options.tau_actions);
      l2.record_hidden_actions(tool_options.tau_actions);

      bool result = true;
      if (tool_options.equivalence != lts_probabilistic_eq_none)
      {
        mCRL2log(verbose) << "comparing LTSs using " <<
                     tool_options.equivalence << "..." << std::endl;

        if (tool_options.equivalence==lts_probabilistic_bisim_bem)
        {
          result=destructive_probabilistic_bisimulation_compare_bem(l1, l2, timer());
        }
        else if (tool_options.equivalence==lts_probabilistic_bisim_grv)
        {
          result=destructive_probabilistic_bisimulation_compare_grv(l1, l2, timer());
        }
        else 
        {
          throw mcrl2::runtime_error("Unknown equivalence to compare probabilistic transition systems:" + 
                                                  description(tool_options.equivalence) + ".");
        }

        mCRL2log(info) << "LTSs are " << ((result) ? "" : "not ")
                       << "equal ("
                       << description(tool_options.equivalence) << ")\n";
      }

      if (tool_options.preorder != lts_probabilistic_pre_none)
      {
        mCRL2log(verbose) << "comparing LTSs using the equivalence " <<
                     description(tool_options.preorder) << ".\n";

        assert(0); // As it stands there is no probabilistic preorder. 

        mCRL2log(info) << "The LTS in " << tool_options.name_for_first
                       << " is " << ((result) ? "" : "not ")
                       << "included in"
                       << " the LTS in " << tool_options.name_for_second
                       << " (using " << description(tool_options.preorder)
                       << ")." << std::endl;
      }

      return true; // The tool terminates in a correct way.
    }

  public:
    bool run() override
    {
      check_preconditions();

      if (tool_options.format_for_first==lts_none)
      {
        tool_options.format_for_first = guess_format(tool_options.name_for_first);
      }

      if (tool_options.format_for_second==lts_none)
      {
        tool_options.format_for_second = guess_format(tool_options.name_for_second);
      }

      if (tool_options.format_for_first!=tool_options.format_for_second)
      {
        throw mcrl2::runtime_error("The input labelled transition systems have different types");
      }

      switch (tool_options.format_for_first)
      {
        case lts_lts:
        {
          return lts_probabilistic_compare<probabilistic_lts_lts_t>();
        }
        case lts_none:
          mCRL2log(mcrl2::log::warning) << "No input format is specified. Assuming .aut format.\n";
          [[fallthrough]];
        case lts_aut:
        {
          return lts_probabilistic_compare<probabilistic_lts_aut_t>();
        }
        case lts_fsm:
        {
          return lts_probabilistic_compare<probabilistic_lts_fsm_t>();
        }
        case lts_dot:
        {
          throw mcrl2::runtime_error("Reading the .dot format is not supported anymore.");
        }
      }

      return true;
    }

  protected:
    std::string synopsis() const override
    {
      return "[OPTION]... [INFILE1] INFILE2";
    }

    void check_positional_options(const command_line_parser& parser) override
    {
      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
    }

    void set_tau_actions(std::vector <std::string>& tau_actions, std::string const& act_names)
    {
      std::string::size_type lastpos = 0, pos;
      while ((pos = act_names.find(',',lastpos)) != std::string::npos)
      {
        tau_actions.push_back(act_names.substr(lastpos,pos-lastpos));
        lastpos = pos+1;
      }
      tau_actions.push_back(act_names.substr(lastpos));
    }

    void add_options(interface_description& desc) override
    {
      ltscompare_base::add_options(desc);

      desc.
      add_option("in1", make_mandatory_argument("FORMAT"),
                 "use FORMAT as the format for INFILE1 (or stdin)", 'i').
      add_option("in2", make_mandatory_argument("FORMAT"),
                 "use FORMAT as the format for INFILE2", 'j').
      add_option("equivalence", make_enum_argument<lts_probabilistic_equivalence>("NAME)")
                 .add_value(lts_probabilistic_eq_none, true)
                 .add_value(lts_probabilistic_bisim_grv)
                 .add_value(lts_probabilistic_bisim_bem),
                 "use equivalence NAME (not allowed in combination with -p/--preorder):", 'e').
      add_option("preorder", make_enum_argument<lts_probabilistic_preorder>("NAME")
                 .add_value(lts_probabilistic_pre_none, true),
                 "use preorder NAME (not allowed in combination with -e/--equivalence):", 'p').
      add_option("tau", make_mandatory_argument("ACTNAMES"),
                 "consider actions with a name in the comma separated list ACTNAMES to "
                 "be internal (tau) actions in addition to those defined as such by "
                 "the input");
    }

    void parse_options(const command_line_parser& parser) override
    {
      ltscompare_base::parse_options(parser);

      tool_options.equivalence = parser.option_argument_as<lts_probabilistic_equivalence>("equivalence");
      tool_options.preorder = parser.option_argument_as<lts_probabilistic_preorder>("preorder");

      if (parser.has_option("tau"))
      {
        set_tau_actions(tool_options.tau_actions, parser.option_argument("tau"));
      }

      if (parser.arguments.size() == 1)
      {
        tool_options.name_for_second = parser.arguments[0];
      }
      else if (parser.arguments.size() == 2)
      {
        tool_options.name_for_first  = parser.arguments[0];
        tool_options.name_for_second = parser.arguments[1];
      } // else something strange going on, caught in check_preconditions()

      if (parser.has_option("in1"))
      {
        tool_options.format_for_first = mcrl2::lts::detail::parse_format(parser.option_argument("in1"));

        if (tool_options.format_for_first == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("in1") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }
      else if (!tool_options.name_for_first.empty())
      {
        tool_options.format_for_first = mcrl2::lts::detail::guess_format(tool_options.name_for_first);
      }
      else
      {
        mCRL2log(warning) << "cannot detect format from stdin and no input format specified; assuming aut format" << std::endl;
        tool_options.format_for_first = lts_aut;
      }
      if (parser.has_option("in2"))
      {
        tool_options.format_for_second = mcrl2::lts::detail::parse_format(parser.option_argument("in2"));

        if (tool_options.format_for_second == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("in2") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }
      else if (!tool_options.name_for_first.empty())
      {
        tool_options.format_for_second = mcrl2::lts::detail::guess_format(tool_options.name_for_second);
      }
    }

};

int main(int argc, char** argv)
{
  return ltspcompare_tool().execute(argc,argv);
}
