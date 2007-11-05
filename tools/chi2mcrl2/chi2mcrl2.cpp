// Author(s): Frank Stappers
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file chi2mcrl2.cpp
/// \brief Add your file description here.
// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>
#endif

#include <cassert>
#include <stdbool.h>
#include <getopt.h>
#include <cstdio>
#include <fstream>
#include "aterm2.h"
#include <string>
#include <iostream>
#include "chilexer.h"
#include <print/messaging.h>
#include "translate.h"

#define NAME "chi2mcrl2"
#define VERSION "July 2007"
#define INFILEEXT ".chi"
#define OUTFILEEXT ".mcrl2"

using namespace ::mcrl2::utilities;
using namespace std;

//t_options represents the options of the translator 
struct t_options {
  string infilename;
  string outfilename;
};

//Functions used by the main program
static ATermAppl translate_file(t_options &options);
static void PrintMoreInfo(char *Name);
static void PrintVersion(void);
static void PrintHelp(char *Name);
// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char* chi_file_for_input;     
    static const char* mcrl2_file_for_output;  

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

const char* squadt_interactor::chi_file_for_input = "chi_in";
const char* squadt_interactor::mcrl2_file_for_output  = "mcrl2_out";


void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_combination(chi_file_for_input, tipi::mime_type("chi", tipi::mime_type::text),
                                                            tipi::tool::category::transformation);
}

bool squadt_interactor::extract_task_options(tipi::configuration const& c, t_options& task_options) const {
  bool result = true;

  if (c.input_exists(chi_file_for_input)) {
    task_options.infilename = c.get_input(chi_file_for_input).get_location();
  }
  else {
    send_error("Configuration does not contain an input object\n");

    result = false;
  }

  if (c.output_exists(mcrl2_file_for_output) ) {
    task_options.outfilename = c.get_output(mcrl2_file_for_output).get_location();
  }
  else {
    send_error("Configuration does not contain an output object\n");

    result = false;
  }

  return (result);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
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

  tipi::layout::tool_display d;
  t_options options;
  CAsttransform asttransform;

  bool result = true;

  extract_task_options(c, options);
 
  label& message = d.create< label >();
 
  d.set_manager(d.create< vertical_box >().
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

   FILE *outstream = fopen(options.outfilename.c_str(), "wb");
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

static bool parse_command_line(int argc, char *argv[],t_options &options)
{ 
  //declarations for getopt
  /* empty */

  #define ShortOptions   "hqvd"
  #define VersionOption   0x1
  struct option LongOptions[] = 
  {
    { "help",        no_argument,       NULL, 'h' },
    { "version",     no_argument,       NULL, VersionOption },
    { "quiet",       no_argument,       NULL, 'q' },
    { "verbose",     no_argument,       NULL, 'v' },
    { "debug",       no_argument,       NULL, 'd' },
    // getop termination string 
	{ 0, 0, 0, 0 }
  };
  int Option;
  //parse options
  Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  while (Option != -1) {
    switch (Option){
      case 'h': /* help */
        PrintHelp(argv[0]);
        return false;
      case VersionOption: /* version */
        PrintVersion();
        return false;
      case 'q': /* quiet */
        gsSetQuietMsg();
        break;
      case 'v': /* verbose */
        gsSetVerboseMsg();
        break;
      case 'd': /* debug */
        gsSetDebugMsg();
        break;
      case '?':
      default:
        PrintMoreInfo(argv[0]); 
        return false;
    } 
    Option = getopt_long(argc, argv, ShortOptions, LongOptions, NULL);
  }
  //check for dangerous and illegal option combinations
  /* empty */

  //check for wrong number of arguments
  int noargc; //non-option argument count
  noargc = argc - optind;
  if (noargc > 2) 
  {
    fprintf(stderr, "%s: too many arguments\n", NAME);
    PrintMoreInfo(argv[0]);
    return false;
  } else {
    //noargc >= 0 && noargc <= 2
    if (noargc > 0) 
	{
      options.infilename = argv[optind];
    }
    if (noargc == 2) 
	{
      options.outfilename = argv[optind + 1];
    }
  }

  return true;  // main continues
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
    ifstream instream(options.infilename.c_str(), ifstream::in|ifstream::binary);
    if (!instream.is_open()) 
	{
      gsErrorMsg("cannot open input file '%s'\n", options.infilename.c_str());
      printf("Cannot open input file '%s'\n", options.infilename.c_str());
      return NULL;
    }
    gsVerboseMsg("Parsing input file '%s'...\n", options.infilename.c_str());
	result = parse_stream(instream);
    instream.close();
  }
  
  
  if (result == NULL) 
  {
    gsErrorMsg("parsing failed\n");
    return NULL;
  }

  return result; 
}

void PrintMoreInfo(char *Name)
{
  fprintf(stderr, "Use %s --help for options\n", Name);
}

void PrintVersion(void)
{
  fprintf(stderr,"%s %s (revision %s)\n", NAME, VERSION, REVISION);
}

void PrintHelp(char *Name)
{
  fprintf(stderr,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
	"Translates the Chi specifiation in INFILE and writes the resulting mCRL2 \n"
	"OUTFILE. if OUTFILE is not present, stdout is used. If INFILE is not present\n"
    "stdin is used.\n"
    "\n"
    "  -h, --help            display this help and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n",
    Name);
}

// Main 

int main(int argc, char *argv[])
{
  ATerm         stack_bottom;
  t_options     options;
  std::string mcrl2spec; 
  CAsttransform asttransform;
  
  ATinit(argc,argv,&stack_bottom);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif

    if (parse_command_line(argc,argv,options)) {
      ATermAppl result = translate_file(options);

      if (result == NULL) {
        return 1;
      }
	 
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
        FILE *outstream = fopen(options.outfilename.c_str(), "wb");
        if (outstream == NULL) {
          gsErrorMsg("cannot open output file '%s'\n", options.outfilename.c_str());
          return 1;
        }
        gsVerboseMsg("saving result to '%s'...\n", options.outfilename.c_str());
        fputs (mcrl2spec.c_str(), outstream); 
        fclose(outstream);
      }
    }

#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}

