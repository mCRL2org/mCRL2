// Author(s): Frank Stappers
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file chi2mcrl2.cpp
/// \brief Add your file description here.

#include "boost.hpp" // precompiled headers

#define TOOLNAME "chi2mcrl2"
#define AUTHOR "Frank Stappers"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <string>
#include <iostream>
#include "aterm2.h"
#include "chilexer.h"
#include "translate.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/exception.h"

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/squadt_tool.h"

#define INFILEEXT ".chi"
#define OUTFILEEXT ".mcrl2"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace std;

using namespace mcrl2;
using utilities::tools::input_output_tool;
using namespace mcrl2::utilities::tools;

//Functions used by the main program
static ATermAppl translate_file(t_options &options);

// Main
class chi2mcrl2_tool: public squadt_tool< input_output_tool> 
{
  protected:
    typedef squadt_tool< input_output_tool> super;

  public:
    chi2mcrl2_tool()
      : super(
          TOOLNAME,
          AUTHOR,
          "convert a Chi (v1.0) model to an mCRL2 specification",
          "Convert a Chi model in INFILE to an mCRL2 specification, and write it to "
          "OUTFILE. If INFILE is not present, stdin is used. If OUTFILE is not present, "
          "stdout is used. INFILE is supposed to conform to v1.0 without time and continious "
          "behaviour."
          "\n\n")
    {}

  private:   

    t_options options;

    void parse_options(const command_line_parser& parser)
    { super::parse_options(parser);
      
      options.no_statepar = false;
      if (parser.options.count("no-state")) {
        options.no_statepar = true;
      }
    }

    void add_options(interface_description& desc)
    { super::add_options(desc);
      desc.add_option("no-state",
                    "no state parameters are generated when translating Chi "
                    "a model", 'n');
    }

  public:
    bool run()
    { 

      std::string mcrl2spec;
      CAsttransform asttransform;
      options.infilename = input_filename().c_str();

      ATermAppl result = translate_file(options);

      gsDebugMsg("Set options");

      asttransform.set_options(options);
 
      gsDebugMsg("Transforming AST to mcrl2 specification\n");
      if (asttransform.translator(result))
        {
          mcrl2spec = asttransform.getResult();
        }

      FILE* OutStream = stdout;

      if (!output_filename().empty()) {
        OutStream = fopen(output_filename().c_str(),"w");
    
        if (OutStream == 0) {
          throw mcrl2::runtime_error("cannot open file '" + output_filename() + "' for writing\n");
        }
      }

      fputs (mcrl2spec.c_str(), OutStream);

      fclose(OutStream);
     
      return result;
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
    void set_capabilities(tipi::tool::capabilities& c) const 
    {
      c.add_input_configuration("main-input", tipi::mime_type("chi", tipi::mime_type::text), tipi::tool::category::transformation);
    }

    void user_interactive_configuration(tipi::configuration& c) {
      /* set the squadt configuration to be sent back, such
       * that mcrl22lps can be restarted later with exactly
       * the same parameters
       */
      if (c.fresh()) {
        if (!c.output_exists("main-output")) {
          c.add_output("main-output", tipi::mime_type("mcrl2", tipi::mime_type::text), c.get_output_name(".mcrl2"));
        }
      }
    }
    
    bool check_configuration(tipi::configuration const& c) const 
    {
      return c.input_exists("main-input") ||
             c.output_exists("main-output");
    }
    
    bool perform_task(tipi::configuration& c) 
    { using namespace tipi;
    
      m_input_filename  = c.get_input("main-input").location();
      m_output_filename = c.get_output("main-output").location(); 
    
      return run();
    }
#endif

};

ATermAppl translate_file(t_options &options)
{
  ATermAppl result = NULL;

  //parse specification
  if (options.infilename == "")
  {
    //parse specification from stdin
    gsVerboseMsg("Parsing input from stdin...\n");
    result = parse_stream(cin);
  } else {
    //parse specification from infilename
    ifstream instream(options.infilename.c_str());
    if (!instream.is_open()) {
      throw mcrl2::runtime_error("cannot open input file '" + options.infilename + "'");
    }
    gsVerboseMsg("Parsing input file '%s'...\n", options.infilename.c_str());
	result = parse_stream(instream);
    instream.close();
  }

  if (result == NULL)
  {
    throw mcrl2::runtime_error("parsing failed");
  }
  return result;
}

int main(int argc, char *argv[])
{
  MCRL2_ATERM_INIT(argc, argv)
  return chi2mcrl2_tool().execute(argc,argv);
}
