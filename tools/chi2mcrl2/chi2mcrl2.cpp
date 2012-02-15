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
#include "mcrl2/aterm/aterm2.h"
#include "chilexer.h"
#include "translate.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/exception.h"

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"

#define INFILEEXT ".chi"
#define OUTFILEEXT ".mcrl2"

using namespace mcrl2::log;
using namespace mcrl2::utilities;
using namespace std;
using namespace aterm;

using namespace mcrl2;
using utilities::tools::input_output_tool;
using namespace mcrl2::utilities::tools;

//Functions used by the main program
static ATermAppl translate_file(t_options& options);

// Main
class chi2mcrl2_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

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

  protected:

    t_options options;

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      options.no_statepar = false;
      if (parser.options.count("no-state"))
      {
        options.no_statepar = true;
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
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

      mCRL2log(debug) << "Set options" << std::endl;

      asttransform.set_options(options);

      mCRL2log(debug) << "Transforming AST to mcrl2 specification" << std::endl;
      if (asttransform.translator(result))
      {
        mcrl2spec = asttransform.getResult();
      }

      FILE* OutStream = stdout;

      if (!output_filename().empty())
      {
        OutStream = fopen(output_filename().c_str(),"w");

        if (OutStream == 0)
        {
          throw mcrl2::runtime_error("cannot open file '" + output_filename() + "' for writing\n");
        }
      }

      fputs(mcrl2spec.c_str(), OutStream);

      fclose(OutStream);

      return true;
    }

};

ATermAppl translate_file(t_options& options)
{
  ATermAppl result = NULL;

  //parse specification
  if (options.infilename == "")
  {
    //parse specification from stdin
    mCRL2log(verbose) << "Parsing input from stdin..." << std::endl;
    result = parse_stream(cin);
  }
  else
  {
    //parse specification from infilename
    ifstream instream(options.infilename.c_str());
    if (!instream.is_open())
    {
      throw mcrl2::runtime_error("cannot open input file '" + options.infilename + "'");
    }
    mCRL2log(verbose) << "Parsing input file '" <<  options.infilename << "'..." << std::endl;
    result = parse_stream(instream);
    instream.close();
  }

  if (result == NULL)
  {
    throw mcrl2::runtime_error("parsing failed");
  }
  return result;
}

class chi2mcrl2_gui_tool: public mcrl2_gui_tool<chi2mcrl2_tool>
{
  public:
    chi2mcrl2_gui_tool()
    {
      m_gui_options["no-state"] = create_checkbox_widget();
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return chi2mcrl2_gui_tool().execute(argc,argv);
}
