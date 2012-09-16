// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsinfo.cpp

#define NAME "ltsinfo"
#define AUTHOR "Muck van Weerdenburg"

#include <string>

#include <boost/lexical_cast.hpp>

#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_algorithm.h"

#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/lts/lts_aut.h"
#include "mcrl2/lts/lts_fsm.h"
#include "mcrl2/lts/lts_bcg.h"
#include "mcrl2/lts/lts_dot.h"


using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::log;

typedef input_tool ltsinfo_base;
class ltsinfo_tool : public ltsinfo_base
{

  private:

    std::string                 infilename;
    mcrl2::lts::lts_type        intype;

  public:

    ltsinfo_tool() :
      ltsinfo_base(NAME,AUTHOR,
                   "display basic information about an LTS",
                   "Print information about the labelled transition system (LTS) in INFILE. "
                   "If INFILE is not supplied, stdin is used.\n"
                   "\n"
                   "The format of INFILE is determined by its contents. "
                   "The option --in can be used to force the format for INFILE. "
                   "The supported formats are:\n"
                   +mcrl2::lts::detail::supported_lts_formats_text()
                  ),
      intype(mcrl2::lts::lts_none)
    {
    }

  protected:

    void add_options(interface_description& desc)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::add_options(desc);

      desc.
      add_option("in", make_mandatory_argument("FORMAT"),
                 "use FORMAT as the input format", 'i');
    }

    void parse_options(const command_line_parser& parser)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::parse_options(parser);

      if (0 < parser.arguments.size())
      {
        infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size())
      {
        throw parser.error("too many file arguments");
      }

      if (parser.options.count("in"))
      {
        if (1 < parser.options.count("in"))
        {
          throw parser.error("multiple input formats specified; can only use one");
        }

        intype = mcrl2::lts::detail::parse_format(parser.option_argument("in"));
        if (intype == lts_none || intype == lts_dot)
        {
          throw parser.error("option -i/--in has illegal argument '" +
                       parser.option_argument("in") + "'");
        }
      }
    }

    template < class LTS_TYPE >
    bool provide_information() const
    {
      LTS_TYPE l;
      l.load(infilename);

      std::cout
          << "Number of states: " << l.num_states() << std::endl
          << "Number of state labels: " << l.num_state_labels() << std::endl
          << "Number of action labels: " << l.num_action_labels() << std::endl
          << "Number of transitions: " << l.num_transitions() << std::endl;

      if (l.has_state_info())
      {
        std::cout << "Has state labels." << std::endl;
      }
      else
      {
        std::cout << "Does not have state labels." << std::endl;
      }
      std::cout << "Has action labels." << std::endl;

      mCRL2log(verbose) << "checking reachability..." << std::endl;
      if (!reachability_check(l))
      {
        std::cout << "Warning: some states are not reachable from the initial state! (This might result in unspecified behaviour of LTS tools.)" << std::endl;
      }

      mCRL2log(verbose) << "check whether lts is deterministic..." << std::endl;
      std::cout << "LTS is ";
      if (!is_deterministic(l))
      {
        std::cout << "not ";
      }
      std::cout << "deterministic." << std::endl;

      return true;
    }



  public:

    bool run()
    {
      using namespace mcrl2::lts;
      using namespace mcrl2::lts::detail;

      if (intype==lts_none)
      {
        intype = guess_format(infilename);
      }

      switch (intype)
      {
        case lts_lts:
        {
          lts_lts_t l;
          l.load(infilename);
          return provide_information<lts_lts_t>();
        }
        case lts_none:
          mCRL2log(warning) << "No input format is specified. Assuming .aut format.\n";
        case lts_aut:
        {
          return provide_information<lts_aut_t>();
        }
        case lts_fsm:
        {
          return provide_information<lts_fsm_t>();
        }
#ifdef USE_BCG
        case lts_bcg:
        {
          return provide_information<lts_bcg_t>();
        }
#endif
        case lts_dot:
        {
          return provide_information<lts_dot_t>();
        }
      }
      return true;
    }
};

class ltsinfo_gui_tool: public mcrl2_gui_tool<ltsinfo_tool>
{
  public:
    ltsinfo_gui_tool()
    {
      //-iFORMAT, --in1=FORMAT   use FORMAT as the format for INFILE1 (or stdin)
    }
};

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltsinfo_gui_tool().execute(argc, argv);
}
