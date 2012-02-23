// Author(s): Muck van Weerdenburg, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert.cpp

#define NAME "ltsconvert"
#define AUTHOR "Muck van Weerdenburg, Jan Friso Groote"

#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/detail/lts_convert.h"
#include "mcrl2/lts/lts_algorithm.h"

using namespace mcrl2::lts;
using namespace mcrl2::lts::detail;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::log;

static const std::set<lts_equivalence> &initialise_allowed_eqs()
{
  static std::set<lts_equivalence> s;
  s.insert(lts_eq_none);
  s.insert(lts_eq_bisim);
  s.insert(lts_eq_branching_bisim);
  s.insert(lts_eq_divergence_preserving_branching_bisim);
  s.insert(lts_eq_sim);
  s.insert(lts_eq_trace);
  s.insert(lts_eq_weak_trace);
  s.insert(lts_red_tau_star);
  return s;
}
static const std::set<lts_equivalence> &allowed_eqs()
{
  static const std::set<lts_equivalence> &s = initialise_allowed_eqs();
  return s;
} 

static inline std::string get_base(std::string const& s)
{
  return s.substr(0, s.find_last_of('.'));
}

class t_tool_options
{
  public:
    std::string     infilename;
    std::string     outfilename;
    std::string     lpsfile;
    lts_type        intype;
    lts_type        outtype;
    lts_equivalence equivalence;
    std::vector<std::string> tau_actions;   // Actions with these labels must be considered equal to tau.
    bool            print_dot_state;
    bool            determinise;
    bool            check_reach;

    inline t_tool_options() : intype(lts_none), outtype(lts_none), equivalence(lts_eq_none),
      print_dot_state(true), determinise(false), check_reach(true)
    {
    }

    inline std::string source_string() const
    {
      return (infilename.empty()) ? std::string("standard input") :
             std::string("'" + infilename + "'");
    }

    inline std::string target_string() const
    {
      return (outfilename.empty()) ? std::string("standard output") :
             std::string("'" + outfilename + "'");
    }

    void set_source(std::string const& filename)
    {
      infilename = filename;
    }

    void set_target(std::string const& filename)
    {
      outfilename = filename;

      if (outtype == lts_none)
      {
        mCRL2log(verbose) << "trying to detect output format by extension..." << std::endl;

        outtype = mcrl2::lts::detail::guess_format(outfilename);

        if (outtype == lts_none)
        {
          if (!lpsfile.empty())
          {
            mCRL2log(warning) << "no output format set; using fsm because --lps was used" << std::endl;
            outtype = lts_fsm;
          }
          else
          {
            mCRL2log(warning) << "no output format set or detected; using default (mcrl2)" << std::endl;
            outtype = lts_lts;
          }
        }
      }
    }

};

using namespace std;

typedef input_output_tool ltsconvert_base;
class ltsconvert_tool : public ltsconvert_base
{
  private:
    t_tool_options tool_options;

  public:
    ltsconvert_tool() :
      ltsconvert_base(NAME,AUTHOR,
                      "convert and optionally minimise an LTS",
                      "Convert the labelled transition system (LTS) from INFILE to OUTFILE in the\n"
                      "requested format after applying the selected minimisation method (default is\n"
                      "none). If OUTFILE is not supplied, stdout is used. If INFILE is not supplied,\n"
                      "stdin is used.\n"
                      "\n"
                      "The output format is determined by the extension of OUTFILE, whereas the input\n"
                      "format is determined by the content of INFILE. Options --in and --out can be\n"
                      "used to force the input and output formats. The supported formats are:\n"
                      + mcrl2::lts::detail::supported_lts_formats_text(lts_lts)
                     )
    {
    }

  private:

    template < class LTS_TYPE >
    bool load_convert_and_save()
    {
      using namespace mcrl2::lts;
      using namespace mcrl2::lts::detail;

      LTS_TYPE l;
      l.load(tool_options.infilename);
      l.hide_actions(tool_options.tau_actions);

      if (tool_options.equivalence != lts_eq_none)
      {
        mCRL2log(verbose) << "reducing LTS (modulo " <<  name_of_equivalence(tool_options.equivalence) << ")..." << std::endl;
        mCRL2log(verbose) << "before reduction: " << l.num_states() << "u states and " << l.num_transitions() << "u transitions " << std::endl;
        reduce(l,tool_options.equivalence);
        mCRL2log(verbose) << "after reduction: " << l.num_states() << "u states and " << l.num_transitions() << "u transitions" << std::endl;
      }

      if (tool_options.determinise)
      {
        mCRL2log(verbose) << "determinising LTS..." << std::endl;
        mCRL2log(verbose) << "before determinisation: " << l.num_states() << "u states and " << l.num_transitions() << "u transitions" << std::endl;
        determinise(l);
        mCRL2log(verbose) << "after determinisation: " << l.num_states() << "u states and " << l.num_transitions() << "u transitions" << std::endl;
      }

      mcrl2::lps::specification spec;

      if (!tool_options.lpsfile.empty())
      {
        // No lpsfile is given. Only straightforward translations are possible.
        spec.load(tool_options.lpsfile);
      }


      switch (tool_options.outtype)
      {
        case lts_lts:
        {
          lts_lts_t l_out;
          lts_convert(l,l_out,spec.data(),spec.action_labels(),spec.process().process_parameters(),!tool_options.lpsfile.empty());
          l_out.save(tool_options.outfilename);
          return true;
        }
        case lts_none:
          mCRL2log(warning) << "Cannot determine type of output. Assuming .aut.\n";
        case lts_aut:
        {
          lts_aut_t l_out;
          lts_convert(l,l_out,spec.data(),spec.action_labels(),spec.process().process_parameters(),!tool_options.lpsfile.empty());
          l_out.save(tool_options.outfilename);
          return true;
        }
        case lts_fsm:
        {
          lts_fsm_t l_out;
          lts_convert(l,l_out,spec.data(),spec.action_labels(),spec.process().process_parameters(),!tool_options.lpsfile.empty());
          l_out.save(tool_options.outfilename);
          return true;
        }
#ifdef USE_BCG
        case lts_bcg:
        {
          lts_bcg_t l_out;
          lts_convert(l,l_out,spec.data(),spec.action_labels(),spec.process().process_parameters(),!tool_options.lpsfile.empty());
          l_out.save(tool_options.outfilename);
          return true;
        }
#endif
        case lts_dot:
        {
          lts_dot_t l_out;
          lts_convert(l,l_out,spec.data(),spec.action_labels(),spec.process().process_parameters(),!tool_options.lpsfile.empty());
          l_out.save(tool_options.outfilename);
          return true;
        }
      }
      return true;
    }


  public:
    bool run()
    {

      if (tool_options.intype==lts_none)
      {
        tool_options.intype = mcrl2::lts::detail::guess_format(tool_options.infilename);
      }
      switch (tool_options.intype)
      {
        case lts_lts:
        {
          return load_convert_and_save<lts_lts_t>();
        }
        case lts_none:
          mCRL2log(warning) << "Cannot determine type of input. Assuming .aut.\n";
        case lts_aut:
        {
          return load_convert_and_save<lts_aut_t>();
        }
        case lts_fsm:
        {
          return load_convert_and_save<lts_fsm_t>();
        }
#ifdef USE_BCG
        case lts_bcg:
        {
          return load_convert_and_save<lts_bcg_t>();
        }
#endif
        case lts_dot:
        {
          return load_convert_and_save<lts_dot_t>();
        }
      }
      return true;
    }

  protected:
    void add_options(interface_description& desc)
    {
      ltsconvert_base::add_options(desc);

      desc.add_option("no-reach",
                      "do not perform a reachability check on the input LTS");
      desc.add_option("no-state",
                      "leave out state information when saving in dot format", 'n');
      desc.add_option("determinise", "determinise LTS", 'D');
      desc.add_option("lps", make_mandatory_argument("FILE"),
                      "use FILE as the LPS from which the input LTS was generated; this might "
                      "be needed to store the correct parameter names of states when saving "
                      "in fsm format and to convert non-mCRL2 LTSs to a mCRL2 LTS", 'l');
      desc.add_option("in", make_mandatory_argument("FORMAT"),
                      "use FORMAT as the input format", 'i').
      add_option("out", make_mandatory_argument("FORMAT"),
                 "use FORMAT as the output format", 'o');
      desc.add_option("equivalence", make_mandatory_argument("NAME"),
                      "generate an equivalent LTS, preserving equivalence NAME:\n"
                      +supported_lts_equivalences_text(allowed_eqs())
                      , 'e');
      desc.add_option("tau", make_mandatory_argument("ACTNAMES"),
                      "consider actions with a name in the comma separated list ACTNAMES to "
                      "be internal (tau) actions in addition to those defined as such by "
                      "the input");
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

    void parse_options(const command_line_parser& parser)
    {
      if (parser.options.count("lps"))
      {
        if (1 < parser.options.count("lps"))
        {
          mCRL2log(warning) << "multiple LPS files specified; can only use one\n";
        }

        tool_options.lpsfile = parser.option_argument("lps");
      }
      if (parser.options.count("in"))
      {
        if (1 < parser.options.count("in"))
        {
          mCRL2log(warning) << "multiple input formats specified; can only use one\n";
        }

        tool_options.intype = mcrl2::lts::detail::parse_format(parser.option_argument("in"));

        if (tool_options.intype == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("in") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }
      if (parser.options.count("out"))
      {
        if (1 < parser.options.count("out"))
        {
          mCRL2log(warning) << "multiple output formats specified; can only use one\n";
        }

        tool_options.outtype = mcrl2::lts::detail::parse_format(parser.option_argument("out"));

        if (tool_options.outtype == lts_none)
        {
          mCRL2log(warning) << "format '" << parser.option_argument("out") <<
                    "' is not recognised; option ignored" << std::endl;
        }
      }

      if (parser.options.count("equivalence"))
      {

        tool_options.equivalence = parse_equivalence(
                                     parser.option_argument("equivalence"));

        if (allowed_eqs().count(tool_options.equivalence) == 0)
        {
          parser.error("option -e/--equivalence has illegal argument '" +
                       parser.option_argument("equivalence") + "'");
        }
      }

      if (parser.options.count("tau"))
      {
        set_tau_actions(tool_options.tau_actions, parser.option_argument("tau"));
      }

      tool_options.determinise                       = 0 < parser.options.count("determinise");
      tool_options.check_reach                       = parser.options.count("no-reach") == 0;
      tool_options.print_dot_state                   = parser.options.count("no-state") == 0;

      if (tool_options.determinise && (tool_options.equivalence != lts_eq_none))
      {
        parser.error("cannot use option -D/--determinise together with LTS reduction options\n");
      }

      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      }
      else
      {
        if (0 < parser.arguments.size())
        {
          tool_options.set_source(parser.arguments[0]);
        }
        else
        {
          if (tool_options.intype == lts_none)
          {
            mCRL2log(warning) << "cannot detect format from stdin and no input format specified; assuming aut format" << std::endl;
            tool_options.intype = lts_aut;
          }
        }
        if (1 < parser.arguments.size())
        {
          tool_options.set_target(parser.arguments[1]);
        }
        else
        {
          if (tool_options.outtype == lts_none)
          {
            if (!tool_options.lpsfile.empty())
            {
              mCRL2log(warning) << "no output format set; using fsm because --lps was used" << std::endl;
              tool_options.outtype = lts_fsm;
            }
            else
            {
              mCRL2log(warning) << "no output format set or detected; using default (aut)" << std::endl;
              tool_options.outtype = lts_aut;
            }
          }
        }
      }
    }

};

class ltsconvert_gui_tool: public mcrl2_gui_tool<ltsconvert_tool>
{
  public:
    ltsconvert_gui_tool()
    {

      std::vector<std::string> values;

      m_gui_options["determinise"] = create_checkbox_widget();

      values.clear();
      values.push_back("none");
      values.push_back("bisim");
      values.push_back("branching-bisim");
      values.push_back("dpbranching-bisim");
      values.push_back("sim");
      values.push_back("trace");
      values.push_back("weak-trace");
      values.push_back("tau-star");
      m_gui_options["equivalence"] = create_radiobox_widget(values);
      m_gui_options["lps"] = create_filepicker_widget("LPS files (*.lps)|*.lps|All Files (*.*)|*.*");
      m_gui_options["no-state"] = create_checkbox_widget();
      m_gui_options["no-reach"] = create_checkbox_widget();
      m_gui_options["tau"] = create_textctrl_widget();

      //-iFORMAT, --in=FORMAT    use FORMAT as the input format
      //-oFORMAT, --out=FORMAT   use FORMAT as the output format

    }
};





int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltsconvert_gui_tool().execute(argc,argv);
}
