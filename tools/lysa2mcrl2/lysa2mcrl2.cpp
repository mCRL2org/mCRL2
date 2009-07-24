// Author(s): Egbert Teeselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lysa2mcrl2.cpp
/// \brief Add your file description here.

#define NAME "lysa2mcrl2"
#define AUTHOR "Egbert Teeselink"

#ifdef _WIN32
#include <conio.h>
#endif
#ifndef _WIN32
bool _getch() { return true; }
#endif

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "mcrl2/core/messaging.h"
#include "mcrl2/exception.h"
#include "mcrl2/utilities/input_output_tool.h"

#include "lysa.h"
#include "lysaparser.h"
#include "lysalexer.h"
#include "lysaconverter.h"
#include "strategy.h"

using namespace mcrl2::utilities::tools;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace std;

class lysa2mcrl2_tool: public input_output_tool
{
  public:
    lysa2mcrl2_tool()
      : input_output_tool(NAME, AUTHOR,
          "Convert Typed LySa to mCRL2",
          "Converts a security protocol specified in Typed LySa in INFILE into an mCRL2 process "
          "specification in OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not "
          "present, stdin is used."
        ),
        to_lysa(false)
    {}

    bool run()
    {
      //read lysa input
      boost::shared_ptr<lysa::Expression> e;
      if(input_filename().empty())
      {
      	gsVerboseMsg("parsing input from stdin...\n");
      	e = parse_stream(cin, options);
      }
      else
      {
      	ifstream infile(input_filename().c_str());
        if (!infile.is_open()) {
          throw mcrl2::runtime_error("cannot open input file '" + input_filename() + "'");
        }
      	gsVerboseMsg("parsing input from '%s'...\n", input_filename().c_str());
      	e = parse_stream(infile, options);
        infile.close();
      }
      if(!e.get())
      {
      	throw mcrl2::runtime_error("parsing failed.");
      }

      string converted_spec;
      if(to_lysa)
      {
        gsVerboseMsg("converting to LySa...\n");
        converted_spec = e->typed_lysa_to_lysa();
      }
      else
      {
        //write mCRL2 output
        gsVerboseMsg("converting to mCRL2...\n");
        converted_spec = lysa::Converter::to_mcrl2(e);
      }
      if(output_filename().empty())
      {
      	gsVerboseMsg("saving result to stdout...\n");
      	cout << converted_spec;
      }
      else
      {
      	gsVerboseMsg("saving result to '%s'...\n", output_filename().c_str());
      	ofstream outfile(output_filename().c_str());
      	outfile << converted_spec;
      	outfile.close();
      }
      delete options.strategy;
      return true;
    }

  protected:
    bool to_lysa;
    lysa::lysa_options options;

    void add_options(interface_description& desc)
    {
      input_output_tool::add_options(desc);
      desc.add_option("strategy", 
        make_optional_argument("STRATEGY", "symbolic"), 
        "Apply conversion using the specified strategy:\n"
        "  'symbolic' for a symbolic conversion (default), or\n"
        "  'straightforward' for a straightforward conversion.", 
        's');
      
      desc.add_option("attacker-index", 
        make_mandatory_argument("NUM"), 
        "Assume that the attacker may be a legitimate (but dishonest) agent participating "
        "in the protocol, corresponding to meta-level index number NUM. The effect of setting "
        "this option is that the attacker's crypto-point CPDY is added to all dest/orig "
        "clauses where one or more of the current meta-variables equal NUM. This option "
        "corresponds to the attackerIndex option of the LySa tool.",
        'a');

      desc.add_option("prefix-idents", 
        make_optional_argument("PREFIX", "_"), 
        "Prefixes all identifiers found in the Typed LySa process in INPUT with an "
        "underscore or with PREFIX to prevent clashes with mCRL2 keywords or "
        "identifiers used in the preamble.", 
        'i');

      desc.add_option("zero-action",
        "Generates a 'zero' action before deadlocking when Typed LySa's empty process (0) "
        "is encountered. This is a valid action in the supplied preambles. This option may "
        "help to differentiate between a deadlock and a correct protocol run termination. ",
        'z');

      desc.add_option("lysa",
        "Converts a Typed LySa process to LySa and not to mCRL2. Makes all other non-standard "
        "options illegal.",
        'l');
    }

    void parse_options(const command_line_parser& parser)
    {
      input_output_tool::parse_options(parser);
      
      options.strategy = lysa::Strategy::get(parser.option_argument("strategy"));

      options.make_symbolic = options.strategy->makeSymbolicAttacker();

      if(parser.options.count("prefix-idents"))
      {
        options.prefix = parser.option_argument("prefix-idents");
      }

      if(parser.options.count("attacker-index"))
      {
        options.attacker_index = parser.option_argument("attacker-index");
      }	

      if(parser.options.count("zero-action"))
      {
        options.zero_action = "zero";
      }

      if(parser.options.count("lysa"))
      {
        to_lysa = true;
      }
    }
};

int main(int argc, char *argv[])
{
  lysa2mcrl2_tool tool;  
  int ret = tool.execute(argc, argv);
  if(getenv("DEBUGGER")) _getch();
  return ret;
}
