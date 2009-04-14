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

#ifndef MCRL2_REVISION
#define MCRL2_REVISION "31337"
#endif

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
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h and rewrite.h

#include "lysa.h"
#include "lysaparser.hpp"
#include "lysalexer.h"
#include "lysaconverter.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace std;

typedef boost::shared_ptr<lysa::Expression> Expression_ptr;

struct tool_options : public lysa::lysa_options {
  string input_file_name; // Name of the file to read input from
  string output_file_name; // Name of the file to write output to (or stdout)
  bool to_lysa;
};

tool_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR,
    "Convert Typed LySa to mCRL2",
    "[OPTION]... [INFILE [OUTFILE]]\n",
    "Converts a security protocol specified in Typed LySa in INFILE into an mCRL2 process "
    "specification in OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not "
    "present, stdin is used.");

  clinterface.add_option("no-attacker", 
    "Produces a specification without support for a symbolic attacker. "
    "This makes the specification significantly simpler and the state space significantly "
    "smaller. However, no Dolev-Yao attacker is included so no outside attacks will be "
    "found. Changes the default values of -p and -f.", 
    'n');

  clinterface.add_option("preamble", 
    make_mandatory_argument("FILENAME"), 
    "Prepend the mCRL2 preamble in FILENAME, instead of using the built-in preamble. "
    "The preamble contains all mCRL2 code that is not directly dependent on the input "
    "protocol, including all data expressions (except the Name and Ciphertext sorts), "
    "the attacker process and auxiliary processes. Defaults to preamble.mcrl2 (or "
    "preamble_straightforward.mcrl2 if -n is present).", 
    'p');

  clinterface.add_option("fmt-file", 
    make_mandatory_argument("FILENAME"), 
    "Use the format strings in FILENAME to build mCRL2 expressions. Defaults to "
    "symbolic.fmt (or straightforward.fmt is -n is present).",
    'f');

  clinterface.add_option("attacker-index", 
    make_mandatory_argument("NUM"), 
    "Assume that the attacker may be a legitimate (but dishonest) agent participating "
    "in the protocol, corresponding to meta-level index number NUM. The effect of setting "
    "this option is that the attacker's crypto-point CPDY is added to all dest/orig "
    "clauses where one or more of the current meta-variables equal NUM. This option "
    "corresponds to the attackerIndex option of the LySa tool.",
    'a');

  clinterface.add_option("prefix-idents", 
    make_optional_argument("PREFIX", "_"), 
    "Prefixes all identifiers found in the Typed LySa process in INPUT with an "
    "underscore or with PREFIX to prevent clashes with mCRL2 keywords or "
    "identifiers used in the preamble", 
    'i');

  clinterface.add_option("zero-action",
    make_optional_argument("ACTION", "zero"), 
    "Generates ACTION before deadlocking when Typed LySa's empty process (0) is "
    "encountered. Defaults to 'zero' when specified without argument. This is a "
    "valid action in the supplied preambles.",
    'z');

  clinterface.add_option("lysa",
    "Converts a Typed LySa process to LySa and not to mCRL2. Makes all other non-standard "
    "options illegal.",
    'l');

  command_line_parser parser(clinterface, ac, av);
	
  tool_options options;
  if(parser.options.count("no-attacker"))
  {
	  options.make_symbolic = false;
    options.preamble_file_name = "preamble_straightforward.mcrl2";
    options.fmt_file_name = "straightforward.fmt";
  }
  else
  {
    options.make_symbolic = true;
    options.preamble_file_name = "preamble.mcrl2";
    options.fmt_file_name = "symbolic.fmt";
  }

  if(parser.options.count("preamble"))
	{
		options.preamble_file_name = parser.option_argument("preamble");
	}

  if(parser.options.count("fmt-file"))
	{
		options.fmt_file_name = parser.option_argument("fmt-file");
	}

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
		options.zero_action = parser.option_argument("zero-action");
	}

  if(parser.options.count("lysa"))
  {
	  options.to_lysa = true;
  }
  else
  {
    options.to_lysa = false;
  }

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      options.input_file_name = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      options.output_file_name = parser.arguments[1];
    }
  }

  if(!parser.continue_execution())
  {
	exit(EXIT_SUCCESS);
  }

  return options;
}

int main(int argc, char *argv[])
{

  try {
    tool_options options = parse_command_line(argc,argv);

		//read lysa input
		Expression_ptr e;
		if(options.input_file_name == "")
		{
			gsVerboseMsg("reading input from stdin...\n");
			e = parse_stream(cin, options);
		}
		else
		{
			gsVerboseMsg("reading input from '%s'...\n", options.input_file_name.c_str());
			ifstream infile(options.input_file_name.c_str());
			e = parse_stream(infile, options);
		}
		if(!e.get())
		{
			throw "parsing failed.";
		}

    string converted_spec;
    if(options.to_lysa)
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
		if(options.output_file_name == "")
		{
			gsVerboseMsg("saving result to stdout...\n");
			cout << converted_spec;
		}
		else
		{
			gsVerboseMsg("saving result to '%s'...\n", options.output_file_name.c_str());
			ofstream outfile(options.output_file_name.c_str());
			outfile << converted_spec;
			outfile.close();
		}

		if(getenv("DEBUGGER")) _getch();
    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
	catch (std::string s) {
    gsErrorMsg(s.c_str());
  }
	catch (char* s) {
    gsErrorMsg(s);
  }
	if(getenv("DEBUGGER")) _getch();
  return EXIT_FAILURE;
}
