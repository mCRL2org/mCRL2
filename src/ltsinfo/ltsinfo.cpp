#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "lts/liblts.h"
#include "setup.h"

#include <boost/lexical_cast.hpp>

#define NAME "ltsinfo"
#define VERSION "0.1"

// Squadt protocol interface and utility pseudo-library
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <utility/squadt_utility.h>

class squadt_interactor : public squadt_tool_interface {

  private:

    static const char*  lts_file_for_input;  ///< file containing an LTS that can be imported

  public:

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

const char* squadt_interactor::lts_file_for_input  = "lts_in";

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lts_file_for_input, sip::mime_type("aut", sip::mime_type::text), sip::tool::category::reporting);
#ifdef MCRL2_BCG
  c.add_input_combination(lts_file_for_input, sip::mime_type("bcg"), sip::tool::category::reporting);
#endif
  c.add_input_combination(lts_file_for_input, sip::mime_type("svc"), sip::tool::category::reporting);
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lts_file_for_input);

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  using namespace mcrl2::lts;
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  sip::object& input_object = c.get_input(lts_file_for_input);

  /* Create and add the top layout manager */
  layout::manager::aptr top = layout::vertical_box::create();

  /* First column */
  layout::vertical_box* left_column = new layout::vertical_box();

  layout::vertical_box::alignment a = layout::left;

  lts l;
  lts_type t = lts::parse_format(input_object.get_mime_type().get_sub_type().c_str());

  if (l.read_from(input_object.get_location(), t)) {
    left_column->add(new label("States (#):"), a);
    left_column->add(new label("Labels (#):"), a);
    left_column->add(new label("Transitions (#):"), a);
    left_column->add(new label(""), a);
    left_column->add(new label("State information:"), a);
    left_column->add(new label("Label information:"), a);
    left_column->add(new label(""), a);
    left_column->add(new label("Created by:"), a);
   
    a = layout::right;

    /* Second column */
    layout::vertical_box* right_column = new layout::vertical_box();
   
    right_column->add(new label(boost::lexical_cast < std::string > (l.num_states())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (l.num_labels())), a);
    right_column->add(new label(boost::lexical_cast < std::string > (l.num_transitions())), a);
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
   
    boost::format c = boost::format("Input read from `%s' (in %s format)");

    top->add(new label("Input read from " + input_object.get_location() + " (" + lts::string_for_type(t) + " format)"), margins(5,0,5,0));
    top->add(columns);

    send_display_layout(top);
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
  fprintf(f,NAME " " VERSION " (revision %s)\n", REVISION);
}

bool parse_command_line(int argc, char** argv, mcrl2::lts::lts& l) {
  using namespace mcrl2::lts;

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
  if ( quiet )
  {
    gsSetQuietMsg();
  }
  if ( verbose )
  {
    gsSetVerboseMsg();
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
  squadt_interactor c;

  if (!c.try_interaction(argc, argv)) {
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
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
