// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsinfo.cpp

#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/setup.h"
#include "mcrl2/core/messaging.h"

#include <boost/lexical_cast.hpp>

using namespace mcrl2::utilities;

#define NAME "ltsinfo"
#define VERSION "July 2007"

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  lts_file_for_input;  ///< file containing an LTS that can be imported

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

const char* squadt_interactor::lts_file_for_input  = "lts_in";

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::reporting);
#ifdef MCRL2_BCG
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::reporting);
#endif
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("fsm", tipi::mime_type::text), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl2", tipi::mime_type::application), tipi::tool::category::reporting);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lts_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  using namespace mcrl2::lts;
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  tipi::object& input_object = c.get_input(lts_file_for_input);

  lts l;
  lts_type t = lts::parse_format(input_object.get_mime_type().get_sub_type().c_str());

  if (l.read_from(input_object.get_location(), t)) {
    /* Create and add the top layout manager */
    tipi::layout::tool_display d;
 
    layout::horizontal_box& m = d.create< horizontal_box >();

    m.append(d.create< vertical_box >().set_default_alignment(layout::left).
                append(d.create< label >().set_text("States (#):")).
                append(d.create< label >().set_text("Labels (#):")).
                append(d.create< label >().set_text("Transitions (#):")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("State information:")).
                append(d.create< label >().set_text("Label information:")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text("Created by:")),
             margins(0,5,0,5));
   
    /* Second column */
    m.append(d.create< vertical_box >().set_default_alignment(layout::right).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_states()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_labels()))).
                append(d.create< label >().set_text(boost::lexical_cast < std::string > (l.num_transitions()))).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.has_state_info() ? "present" : "not present")).
                append(d.create< label >().set_text(l.has_label_info() ? "present" : "not present")).
                append(d.create< label >().set_text("")).
                append(d.create< label >().set_text(l.get_creator())),
             margins(0,5,0,5));
   
    layout::vertical_box& n = d.create< vertical_box >();

    n.append(m).
        append(d.create< label >().
             set_text("Input read from " + input_object.get_location() + " (" + lts::string_for_type(t) + " format)"),
                        margins(5,0,5,20));

    gsVerboseMsg("checking reachability...\n");
    if (!l.reachability_check()) {
        n.append(d.create< label >().set_text("Warning: some states are not reachable from the initial state!")).
          append(d.create< label >().set_text("(This might result in unspecificied behaviour of LTS tools.)"));
    }

    send_display_layout(d.set_manager(n));
  }
  else {
    send_error("Could not read `" + c.get_input(lts_file_for_input).get_location() + "', corruption or incorrect format?\n");

    return (false);
  }

  return (true);
}
#endif

static void print_formats(FILE *f)
{
  fprintf(f,
    "The following formats are accepted by " NAME ":\n"
    "\n"
    "  format  ext.  description                       remarks\n"
    "  -----------------------------------------------------------\n"
    "  aut     .aut  Aldebaran format (CADP)\n"
#ifdef MCRL2_BCG
    "  bcg     .bcg  Binary Coded Graph format (CADP)\n"
#endif
    "  fsm     .fsm  Finite State Machine format\n"
    "  mcrl    .svc  mCRL SVC format\n"
    "  mcrl2   .svc  mCRL2 SVC format                  default\n"
    "\n"
    );
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE]\n"
    "Print information about the labelled transition system (LTS) in INFILE. If\n"
    "INFILE is not supplied, stdin is used.\n"
    "\n"
    "The format of INFILE is determined by its contents. The option --in can be used\n"
    "to force the format for INFILE.\n"
    "\n"
    "Options:\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display any unrequested information\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -d, --debug           display detailed intermediate messages\n"
    "  -f, --formats         list accepted formats\n"
    "  -iFORMAT, --in=FORMAT use FORMAT as the input format\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %s)\n", REVISION);
}

bool parse_command_line(int argc, char** argv, mcrl2::lts::lts& l) {
  using namespace mcrl2::lts;

  #define ShortOptions      "hqvdi:f"
  #define VersionOption     0x1
  struct option LongOptions[] = { 
    {"help"      , no_argument,         NULL, 'h'},
    {"version"   , no_argument,         NULL, VersionOption},
    {"quiet"     , no_argument,         NULL, 'q'},
    {"verbose"   , no_argument,         NULL, 'v'},
    {"debug"     , no_argument,         NULL, 'd'},
    {"in"        , required_argument,   NULL, 'i'},
    {"formats"   , no_argument,         NULL, 'f'},
    {0, 0, 0, 0}
  };

  bool verbose = false;
  bool quiet = false;
  bool debug = false;
  lts_type intype = lts_none;
  int opt;
  while ( (opt = getopt_long(argc, argv, ShortOptions, LongOptions, NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr,argv[0]);
        return (false);
      case VersionOption:
        print_version(stderr);
        return (false);
      case 'v':
        verbose = true;
        break;
      case 'q':
        quiet = true;
        break;
      case 'd':
        debug = true;
        break;
      case 'i':
        if ( intype != lts_none )
        {
          fprintf(stderr,"warning: input format has already been specified; extra option ignored\n");
        } else {
          intype = lts::parse_format(optarg);
          if ( intype == lts_none )
          {
            fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
          }
        }
        break;
      case 'f':
        print_formats(stderr);
        return (false);
      default:
        break;
    }
  }

  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");

    return (false);
  }
  if ( quiet && debug )
  {
    gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");

    return (false);
  }
  if ( quiet )
  {
    gsSetQuietMsg();
  }
  if ( verbose )
  {
    gsSetVerboseMsg();
  }
  if ( debug )
  {
    gsSetDebugMsg();
  }

  bool use_stdin = (optind >= argc);

  std::string infile;

  if ( !use_stdin )
  {
    infile = argv[optind];
  }

  if ( use_stdin )
  {
    gsVerboseMsg("reading LTS from stdin...\n");
    if ( !l.read_from(std::cin,intype) )
    {
      gsErrorMsg("cannot read LTS from stdin\n");

      return (false);
    }
  } else {
    gsVerboseMsg("reading LTS from '%s'...\n",infile.c_str());
    if ( !l.read_from(infile,intype) )
    {
      bool b = true;

      if ( intype == lts_none ) // XXX really do this?
      {
        gsVerboseMsg("reading failed; trying to force format by extension...\n");
        intype = lts::guess_format(infile);
        if ( (intype != lts_none) && l.read_from(infile,intype) )
        {
          b = false;
        }
      }
      if ( b )
      {
        gsErrorMsg("cannot read LTS from file '%s'\n",infile.c_str());

        return (false);
      }
    }
  }

  return (true);
}

int main(int argc, char **argv) {
  ATerm       bottom;

  ATinit(argc,argv,&bottom);
  
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
#endif
    using namespace std;
    using namespace mcrl2::lts;

    lts l;

    if (parse_command_line(argc, argv, l)) {

      cout << "LTS format: " << lts::string_for_type(l.get_type()) << endl
           << "Number of states: " << l.num_states() << endl
           << "Number of labels: " << l.num_labels() << endl
           << "Number of transitions: " << l.num_transitions() << endl;
     
      if ( l.has_state_info() )
      {
        cout << "Has state information." << endl;
      } else {
        cout << "Does not have state information." << endl;
      }
      if ( l.has_label_info() )
      {
        cout << "Has label information." << endl;
      } else {
        cout << "Does not have label information." << endl;
      }
      if ( l.has_creator() )
      {
        cout << "Created by: " << l.get_creator() << endl;
      }
      gsVerboseMsg("checking reachability...\n");
      if ( !l.reachability_check() )
      {
        cout << "Warning: some states are not reachable from the initial state! (This might result in unspecificied behaviour of LTS tools.)" << endl;
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
