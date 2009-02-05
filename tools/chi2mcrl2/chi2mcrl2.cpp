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

#define NAME "chi2mcrl2"
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
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/exception.h"

#define INFILEEXT ".chi"
#define OUTFILEEXT ".mcrl2"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace std;


//Functions used by the main program
static ATermAppl translate_file(t_options &options);

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/mcrl2_squadt_interface.h>

const char* chi_file_for_input = "chi_in";
const char* mcrl2_file_for_output  = "mcrl2_out";

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    /** \brief compiles a t_lin_options instance from a configuration */
    bool extract_task_options(tipi::configuration const& c, t_options&) const;

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(tipi::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(tipi::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(tipi::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(tipi::configuration&);
};

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(chi_file_for_input, tipi::mime_type("chi", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
}

bool squadt_interactor::extract_task_options(tipi::configuration const& c, t_options& task_options) const {
  bool result = true;

  if (c.input_exists(chi_file_for_input)) {
    task_options.infilename = c.get_input(chi_file_for_input).location();
  }
  else {
    send_error("Configuration does not contain an input object\n");

    result = false;
  }

  if (c.output_exists(mcrl2_file_for_output) ) {
    task_options.outfilename = c.get_output(mcrl2_file_for_output).location();
  }
  else {
    send_error("Configuration does not contain an output object\n");

    result = false;
  }

  return (result);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  if (!c.output_exists(mcrl2_file_for_output)) {
    c.add_output(mcrl2_file_for_output, tipi::mime_type("mcrl2", tipi::mime_type::text), c.get_output_name(".mcrl2"));
  }
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result |= c.input_exists(chi_file_for_input);
  result |= c.output_exists(mcrl2_file_for_output);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  tipi::tool_display d;
  t_options options;
  CAsttransform asttransform;

  bool result = true;

  extract_task_options(c, options);

  label& message = d.create< label >();

  d.manager(d.create< vertical_box >().
                        append(message.set_text("Translation in progress"), layout::left));

  send_display_layout(d);

  std::string mcrl2spec;

  ATermAppl ast_result = translate_file(options);

  if (ast_result == NULL) {
    message.set_text("Reading input file failed");
    result = false;
  }

  if (asttransform.translator(ast_result) && result )
  {
      mcrl2spec = asttransform.getResult();

      if(mcrl2spec.empty())
      {

        message.set_text("Reading input file failed");
        result = false;

      }
   }

  if(result)
  {
    //store the result

   FILE *outstream = fopen(options.outfilename.c_str(), "w");
   if (outstream != NULL) {
       fputs (mcrl2spec.c_str(), outstream);
       fclose(outstream);
    }
    else {
      send_error(str(boost::format("cannot open output file '%s'\n") % options.outfilename));
      result = false;
    }

    if (result) {
      message.set_text("Translation finished");
    }
  }
  send_display_layout(d);

  return true;
}
#endif

bool parse_command_line(int argc, char *argv[], t_options& options)
{
  interface_description clinterface(argv[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Translates the Chi specifiation in INFILE and writes the resulting mCRL2 "
    "OUTFILE. if OUTFILE is not present, stdout is used. If INFILE is not present "
    "stdin is used.");

  clinterface.add_option("no-state", "no state parameters are generated when translating Chi", 'n');

  command_line_parser parser(clinterface, argc, argv);

  if (parser.continue_execution()) {
    options.no_statepar = false;

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        options.infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        options.outfilename = parser.arguments[1];
      }
    }
  }

  return parser.continue_execution();  // main continues
}

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

// Main

int main(int argc, char *argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
#ifdef ENABLE_SQUADT_CONNECTIVITY
    if (mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
      return EXIT_SUCCESS;
    }
#endif

    t_options options;

    if (parse_command_line(argc,argv, options)) {

      std::string mcrl2spec;
      CAsttransform asttransform;

      ATermAppl result = translate_file(options);

      gsDebugMsg("Set options");
      asttransform.set_options(options);

      gsDebugMsg("Transforming AST to mcrl2 specification\n");
      if (asttransform.translator(result))
        {
          mcrl2spec = asttransform.getResult();
        }

      //store the result
      if (options.outfilename == "") {
        gsVerboseMsg("saving result to stdout...\n");
        printf("%s",mcrl2spec.c_str());
      } else { //outfilename != NULL
        //open output filename
        FILE *outstream = fopen(options.outfilename.c_str(), "w");
        if (outstream == NULL) {
          throw mcrl2::runtime_error("cannot open output file '" + options.outfilename + "'");
        }
        gsVerboseMsg("saving result to '%s'...\n", options.outfilename.c_str());
        fputs (mcrl2spec.c_str(), outstream);
        fclose(outstream);
      }
    }

    return EXIT_SUCCESS;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}

