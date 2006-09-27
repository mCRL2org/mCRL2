#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "lts/liblts.h"
#include "setup.h"

// Squadt protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>

/* Constants for identifiers of options and objects */
enum identifiers {
    lts_file_for_input // Main input file that contains an lts
};

#endif

#define NAME "ltsinfo"
#define VERSION "0.1"
#include "mcrl2_revision.h"

using namespace std;
using namespace mcrl2::lts;

/* An lts structure that stores the LTS that was last loaded */
lts l;

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
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -f, --formats         list accepted formats\n"
    "  -i, --in=FORMAT       use FORMAT as the input format\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %i)\n", REVISION);
}

void parse_command_line(int argc, char** argv) {
  #define ShortOptions      "hqvi:f"
  #define VersionOption     0x1
  struct option LongOptions[] = { 
    {"help"      , no_argument,         NULL, 'h'},
    {"version"   , no_argument,         NULL, VersionOption},
    {"quiet"     , no_argument,         NULL, 'q'},
    {"verbose"   , no_argument,         NULL, 'v'},
    {"in"        , required_argument,   NULL, 'i'},
    {"formats"   , no_argument,         NULL, 'f'},
    {0, 0, 0, 0}
  };

  bool verbose = false;
  bool quiet = false;
  lts_type intype = lts_none;
  int opt;
  while ( (opt = getopt_long(argc, argv, ShortOptions, LongOptions, NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr,argv[0]);
        exit (0);
      case VersionOption:
        print_version(stderr);
        exit (0);
      case 'v':
        verbose = true;
        break;
      case 'q':
        quiet = true;
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
        exit (0);
      default:
        break;
    }
  }

  if ( quiet && verbose )
  {
    gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");

    exit (1);
  }
  if ( quiet )
  {
    gsSetQuietMsg();
  }
  if ( verbose )
  {
    gsSetVerboseMsg();
  }

  bool use_stdin = (optind >= argc);

  string infile;
  if ( !use_stdin )
  {
    infile = argv[optind];
  }

  if ( use_stdin )
  {
    gsVerboseMsg("reading LTS from stdin...\n");
    if ( !l.read_from(cin,intype) )
    {
      gsErrorMsg("cannot read LTS from stdin\n");

      exit (1);
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

        exit (1);
      }
    }
  }
}

#ifdef ENABLE_SQUADT_CONNECTIVITY
/* Validates a configuration */
bool try_to_accept_configuration(sip::tool::communicator& tc) {
  sip::configuration& configuration = tc.get_configuration();

  if (configuration.object_exists(lts_file_for_input)) {
    /* The input object is present, verify whether the specified format is supported */
    sip::object::sptr input_object = configuration.get_object(lts_file_for_input);

    lts_type t = lts::parse_format(input_object->get_format().c_str());

    if (t == lts_none) {
      tc.send_status_report(sip::report::error, boost::str(boost::format("Invalid configuration: unsupported type `%s' for main input") % lts::string_for_type(t)));

      return (false);
    }
    if (!boost::filesystem::exists(boost::filesystem::path(input_object->get_location()))) {
      tc.send_status_report(sip::report::error, std::string("Invalid configuration: input object does not exist"));

      return (false);
    }
  }
  else {
    return (false);
  }

  tc.send_accept_configuration();

  return (true);
}
#endif

int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

#ifdef ENABLE_SQUADT_CONNECTIVITY
  sip::tool::communicator tc;

  /* Get tool capabilities object associated to the communicator in order to modify settings */
  sip::tool::capabilities& cp = tc.get_tool_capabilities();

  /* The tool operates on LTSes (stored in some different formats) and its function can be characterised as reporting */
  cp.add_input_combination(lts_file_for_input, "Reporting", "aut");
#ifdef MCRL2_BCG
  cp.add_input_combination(lts_file_for_input, "Reporting", "bcg");
#endif
  cp.add_input_combination(lts_file_for_input, "Reporting", "svc");

  /* On purpose we do not catch exceptions */
  if (tc.activate(argc,argv)) {
    bool valid_configuration_present = false;
    bool termination_requested       = false;

    /* Initialise utility pseudo-library */
    squadt_utility::initialise(tc);

    /* Main event loop for incoming messages from squadt */
    while (!termination_requested) {
      sip::message_ptr m = tc.await_message(sip::message_any);

      assert(m.get() != 0);

      switch (m->get_type()) {
        case sip::message_offer_configuration:

          /* Insert configuration in tool communicator object */
          valid_configuration_present = try_to_accept_configuration(tc);

          break;
        case sip::message_signal_start:
          if (valid_configuration_present) {
            using namespace sip;
            using namespace sip::layout;
            using namespace sip::layout::elements;

            sip::object::sptr input_object = tc.get_configuration().get_object(lts_file_for_input);

            lts_type t = lts::parse_format(input_object->get_format().c_str());

            layout::tool_display::sptr display(new layout::tool_display);

            /* Create and add the top layout manager */
            layout::manager::aptr top = layout::vertical_box::create();

            /* First column */
            layout::vertical_box* left_column = new layout::vertical_box();

            layout::vertical_box::alignment a = layout::left;

            if (l.read_from(input_object->get_location(), t)) {
              left_column->add(new label("States (#):"), a);
              left_column->add(new label("Labels (#):"), a);
              left_column->add(new label("Transitions (#):"), a);
              left_column->add(new label(""), a);
              left_column->add(new label("State information:"), a);
              left_column->add(new label("Label information:"), a);
              left_column->add(new label(""), a);
              left_column->add(new label("Created by:"), a);
             
              /* Second column */
              layout::vertical_box* right_column = new layout::vertical_box();
             
              boost::format c("%u");
             
              right_column->add(new label(boost::str(c % l.num_states())), a);
              right_column->add(new label(boost::str(c % l.num_labels())), a);
              right_column->add(new label(boost::str(c % l.num_transitions())), a);
              right_column->add(new label(""), a);
              right_column->add(new label(l.has_state_info() ? "present" : "not present"), a);
              right_column->add(new label(l.has_label_info() ? "present" : "not present"), a);
              right_column->add(new label(""), a);
              right_column->add(new label(l.get_creator()), a);
             
              /* Create and add a layout manager for the columns */
              layout::manager* columns = new layout::horizontal_box();

              /* Attach columns */
              columns->add(left_column, margins(0,5,0,5));
              columns->add(right_column, margins(0,5,0,20));
             
              c = boost::format("Input read from `%s' (in %s format)");

              top->add(new label(boost::str(c % boost::filesystem::path(input_object->get_location()).leaf() % lts::string_for_type(t))), margins(5,0,5,0));
              top->add(columns);

              display->set_top_manager(top);
             
              tc.send_display_layout(display);
             
              /* Signal that the job is finished */
              tc.send_signal_done(true);
            }
            else {
              tc.send_status_report(sip::report::error, "Failure reading input from file.");
            }
          }
          else {
            /* Send error report */
            tc.send_status_report(sip::report::error, "Start signal received without valid configuration.");
          }
          break;
        case sip::message_request_termination:
          termination_requested = true;

          tc.send_signal_termination();
          break;
        default:
          /* Messages with a type that do not need to be handled */
          break;
      }
    }
  }
  else {
#endif
    parse_command_line(argc, argv);

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
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
