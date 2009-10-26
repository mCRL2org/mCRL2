// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltscompare.cpp

#define NAME "ltscompare"
#define AUTHOR "Muck van Weerdenburg"

#include <string>
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/lts/lts.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/tool.h"
#include "mcrl2/exception.h"

using namespace std;
using namespace mcrl2::lts;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

static const char *equivalent_string(lts_equivalence eq)
{
  switch ( eq )
  {
    case lts_eq_bisim:
      return "strongly bisimilar";
    case lts_eq_branching_bisim:
      return "branching bisimilar";
    case lts_eq_divergence_preserving_branching_bisim:
      return "divergence preserving branching bisimilar";
    case lts_eq_sim:
      return "strongly simulation equivalent";
    case lts_eq_trace:
      return "strongly trace equivalent";
    case lts_eq_weak_trace:
      return "weak trace equivalent";
    default:
      return "equivalent";
  }
}

static const char *preorder_string(lts_preorder pre)
{
  switch ( pre )
  {
    case lts_pre_sim:
      return "strongly simulated by";
    case lts_pre_trace:
      return "strongly trace-included in";
    case lts_pre_weak_trace:
      return "weakly trace-included in";
    default:
      return "included in";
  }
}

static const std::set<lts_equivalence> &initialise_allowed_eqs()
{
  static std::set<lts_equivalence> s;
  s.insert(lts_eq_bisim);
  s.insert(lts_eq_branching_bisim);
  s.insert(lts_eq_divergence_preserving_branching_bisim);
  s.insert(lts_eq_sim);
  s.insert(lts_eq_trace);
  s.insert(lts_eq_weak_trace);
  return s;
}
static const std::set<lts_equivalence> &allowed_eqs()
{
  static const std::set<lts_equivalence> &s = initialise_allowed_eqs();
  return s;
}

struct t_tool_options {
  std::string     name_for_first;
  std::string     name_for_second;
  lts_type        format_for_first;
  lts_type        format_for_second;
  lts_equivalence equivalence;
  lts_preorder    preorder;
  lts_eq_options  eq_opts;
};

typedef tool ltscompare_base;
class ltscompare_tool : public ltscompare_base
{
  private:
    t_tool_options tool_options;

  public:
    ltscompare_tool() :
      ltscompare_base(NAME,AUTHOR,
        "compare two LTSs",
        "Determine whether or not the labelled transition systems (LTSs) in INFILE1 and INFILE2 are related by some equivalence or preorder. "
        "If INFILE1 is not supplied, stdin is used.\n"
        "\n"
        "The input formats are determined by the contents of INFILE1 and INFILE2. "
        "Options --in1 and --in2 can be used to force the input format of INFILE1 and INFILE2, respectively. "
        "The supported formats are:\n"
        +lts::supported_lts_formats_text()
      )
    {
    }

    bool run()
    {
      lts l1,l2;

      if ( tool_options.name_for_first.empty() ) {
        gsVerboseMsg("reading first LTS from stdin...\n");

        if ( !l1.read_from(std::cin, tool_options.format_for_first) ) {
          throw mcrl2::runtime_error("cannot read LTS from stdin\nretry with -v/--verbose for more information");
        }
      } else {
        gsVerboseMsg("reading first LTS from '%s'...\n", tool_options.name_for_first.c_str());

        if ( !l1.read_from(tool_options.name_for_first, tool_options.format_for_first) ) {
          throw mcrl2::runtime_error("cannot read LTS from file '" + tool_options.name_for_first + "'\nretry with -v/--verbose for more information");
        }
      }

      gsVerboseMsg("reading second LTS from '%s'...\n", tool_options.name_for_second.c_str());

      if ( !l2.read_from(tool_options.name_for_second, tool_options.format_for_second) ) {
        throw mcrl2::runtime_error("cannot read LTS from file '" + tool_options.name_for_second + "'\nretry with -v/--verbose for more information");
      }

      bool result = true;
      if ( tool_options.equivalence != lts_eq_none )
      {
        gsVerboseMsg("comparing LTSs using %s...\n",
            lts::name_of_equivalence(tool_options.equivalence).c_str());

        result = l1.compare(l2,tool_options.equivalence,tool_options.eq_opts);

        gsMessage("LTSs are %s%s\n",
            ((result) ? "" : "not "),
            equivalent_string(tool_options.equivalence));
      }

      if ( tool_options.preorder != lts_pre_none )
      {
        gsVerboseMsg("comparing LTSs using %s...\n",
            lts::name_of_preorder(tool_options.preorder).c_str());

        result = l1.compare(l2,tool_options.preorder,tool_options.eq_opts);

        gsMessage("LTS in %s is %s%s LTS in %s\n",
            tool_options.name_for_first.c_str(),
            ((result) ? "" : "not "),
            preorder_string(tool_options.preorder),
            tool_options.name_for_second.c_str() );
      }

      return result;
    }

  private:
    std::string synopsis() const
    {
      return "[OPTION]... [INFILE1] INFILE2";
    }

    void check_positional_options(const command_line_parser &parser)
    {
      if (2 < parser.arguments.size())
      {
        parser.error("too many file arguments");
      } else if (1 > parser.arguments.size())
      {
        parser.error("too few file arguments");
      }
    }

    void add_options(interface_description &desc)
    {
      ltscompare_base::add_options(desc);

      desc.
        add_option("in1", make_mandatory_argument("FORMAT"),
          "use FORMAT as the format for INFILE1 (or stdin)", 'i').
        add_option("in2", make_mandatory_argument("FORMAT"),
          "use FORMAT as the format for INFILE2", 'j').
        add_option("equivalence", make_mandatory_argument("NAME"),
          "use equivalence NAME:\n"
          +lts::supported_lts_equivalences_text(allowed_eqs())+"\n"
          "(not allowed in combination with -p/--preorder)"
          , 'e').
        add_option("preorder", make_mandatory_argument("NAME"),
          "use preorder NAME:\n"
          +lts::supported_lts_preorders_text()+"\n"
          "(not allowed in combination with -e/--equivalence)"
          , 'p').
        add_option("tau", make_mandatory_argument("ACTNAMES"),
          "consider actions with a name in the comma separated list ACTNAMES to "
          "be internal (tau) actions in addition to those defined as such by "
          "the input");
    }

    void parse_options(const command_line_parser &parser)
    {
      ltscompare_base::parse_options(parser);

      if (parser.options.count("equivalence") > 1)
      {
        parser.error("multiple use of option -e/--equivalence; only one occurrence is allowed");
      }
  
      if (parser.options.count("preorder") > 1)
      {
        parser.error("multiple use of option -p/--preorder; only one occurrence is allowed");
      }
  
      if (parser.options.count("equivalence") + parser.options.count("preorder") > 1)
      {
        parser.error("options -e/--equivalence and -p/--preorder cannot be used simultaneously");
      }
  
      if (parser.options.count("equivalence") + parser.options.count("preorder") < 1)
      {
        parser.error("one of the options -e/--equivalence and -p/--preorder must be used");
      }
  
      tool_options.equivalence = lts_eq_none;
  
      if (parser.options.count("equivalence")) {
  
        tool_options.equivalence = lts::parse_equivalence(
            parser.option_argument("equivalence"));
  
        if ( allowed_eqs().count(tool_options.equivalence) == 0 )
        {
          parser.error("option -e/--equivalence has illegal argument '" +
              parser.option_argument("equivalence") + "'");
        }
      }
  
      tool_options.preorder = lts_pre_none;
  
      if (parser.options.count("preorder")) {
  
        tool_options.preorder = lts::parse_preorder(
            parser.option_argument("preorder"));
  
        if (tool_options.preorder == lts_pre_none)
        {
          parser.error("option -p/--preorder has illegal argument '" +
              parser.option_argument("preorder") + "'");
        }
      }
  
      if (parser.options.count("tau")) {
        lts_reduce_add_tau_actions(tool_options.eq_opts, parser.option_argument("tau"));
      }
  
      if (parser.arguments.size() == 1) {
        tool_options.name_for_second = parser.arguments[0];
      } else { // if (parser.arguments.size() == 2)
        tool_options.name_for_first  = parser.arguments[0];
        tool_options.name_for_second = parser.arguments[1];
      }
  
      if (parser.options.count("in1")) {
        if (1 < parser.options.count("in1")) {
          std::cerr << "warning: multiple input formats specified for first LTS; can only use one\n";
        }
  
        tool_options.format_for_first = lts::parse_format(parser.option_argument("in1"));
  
        if (tool_options.format_for_first == lts_none) {
          std::cerr << "warning: format '" << parser.option_argument("in1") <<
                       "' is not recognised; option ignored" << std::endl;
        }
      }
      else if (!tool_options.name_for_first.empty()) {
        tool_options.format_for_first = lts::guess_format(tool_options.name_for_first);
      } else {
        gsWarningMsg("cannot detect format from stdin and no input format specified; assuming aut format\n");
        tool_options.format_for_first = lts_aut;
      }
      if (parser.options.count("in2")) {
        if (1 < parser.options.count("in2")) {
          std::cerr << "warning: multiple input formats specified for second LTS; can only use one\n";
        }
  
        tool_options.format_for_second = lts::parse_format(parser.option_argument("in2"));
  
        if (tool_options.format_for_second == lts_none) {
          std::cerr << "warning: format '" << parser.option_argument("in2") <<
                       "' is not recognised; option ignored" << std::endl;
        }
      }
      else {
        tool_options.format_for_second = lts::guess_format(tool_options.name_for_second);
      }
    }
};

int main(int argc, char **argv) {
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltscompare_tool().execute(argc,argv);
}
