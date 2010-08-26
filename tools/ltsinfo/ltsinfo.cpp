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

#include "aterm2.h"
#include "mcrl2/lts/lts_io.h"
#include "mcrl2/lts/lts_algorithm.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;

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

    void add_options(interface_description &desc)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::add_options(desc);

      desc.
        add_option("in", make_mandatory_argument("FORMAT"),
          "use FORMAT as the input format", 'i');
    }

    void parse_options(const command_line_parser &parser)
    {
      using namespace mcrl2::lts;

      ltsinfo_base::parse_options(parser);

      if (0 < parser.arguments.size()) {
        infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        parser.error("too many file arguments");
      }

      if (parser.options.count("in")) {
        if (1 < parser.options.count("in")) {
          parser.error("multiple input formats specified; can only use one");
        }

        intype = mcrl2::lts::detail::parse_format(parser.option_argument("in"));
        if (intype == lts_none || intype == lts_dot)  {
          parser.error("option -i/--in has illegal argument '" +
            parser.option_argument("in") + "'");
        }
      }
    }

  public:

    bool run() 
    {
      using namespace mcrl2::lts;
      using namespace mcrl2::lts::detail;

      mcrl2::lts::lts l;
      if (infilename.empty()) 
      {
        gsVerboseMsg("reading LTS from stdin...\n");

        try 
        { mcrl2::lts::lts l_temp(std::cin, intype);
          l.swap(l_temp);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string("cannot read LTS from stdin\nretry with -v/--verbose for more information.\n") +
                                      e.what());
        }
      }
      else 
      {
        gsVerboseMsg("reading LTS from '%s'...\n",infilename.c_str());

        try 
        { mcrl2::lts::lts l_temp(infilename,intype);
          l.swap(l_temp);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string("cannot read LTS from file '") + infilename +
                                             "'\nretry with -v/--verbose for more information.\n" +
                                             e.what());
        }
      }

      std::cout << "LTS format: " << string_for_type(l.get_type()) << std::endl
           << "Number of states: " << l.num_states() << std::endl
           << "Number of labels: " << l.num_labels() << std::endl
           << "Number of transitions: " << l.num_transitions() << std::endl;

      if ( l.has_state_info() )
      {
        std::cout << "Has state information." << std::endl;
      } else {
        std::cout << "Does not have state information." << std::endl;
      }
      if ( l.has_label_info() )
      {
        std::cout << "Has label information." << std::endl;
      } else {
        std::cout << "Does not have label information." << std::endl;
      }
      if ( l.has_creator() )
      {
        std::cout << "Created by: " << l.get_creator() << std::endl;
      }
      gsVerboseMsg("checking reachability...\n");
      if ( !reachability_check(l) )
      {
        std::cout << "Warning: some states are not reachable from the initial state! (This might result in unspecified behaviour of LTS tools.)" << std::endl;
      }

      gsVerboseMsg("deterministic check...\n");
      std::cout << "LTS is ";
      if ( !is_deterministic(l) )
      {
        std::cout << "not ";
      }
      std::cout << "deterministic." << std::endl;

      return true;
    }

};

class ltsinfo_gui_tool: public mcrl2_gui_tool<ltsinfo_tool> {
public:
	ltsinfo_gui_tool() {
		//-iFORMAT, --in1=FORMAT   use FORMAT as the format for INFILE1 (or stdin)
	}
};

int main(int argc, char **argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return ltsinfo_gui_tool().execute(argc, argv);
}
