#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "lts/liblts.h"
#include "lts/liblts_fsm.h"
#include "lts/liblts_dot.h"
#include "setup.h"

#define NAME "ltsconvert"
#define VERSION "0.1"

using namespace mcrl2::lts;

enum alt_lts_type {
  alt_lts_none,
  alt_lts_fsm,
  alt_lts_dot
};

static alt_lts_type get_alt_format(std::string const& s);

bool read_lts_from_file(lts&, std::string const&, lts_type);
bool write_lts_to_file(lts&, std::string const&, alt_lts_type outtype, std::string const&, bool);
bool write_lts_to_file(lts&, std::string const&, lts_type outtype, std::string const&);

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <squadt_utility.h>

class squadt_interactor : public squadt_tool_interface {

  private:

    enum input_files {
      lts_file_for_input = 0,  ///< file containing an LTS that can be imported using the LTS library
      lts_file_for_output = 1, ///< file used to write the output to
      lpd_file_auxiliary = 2   ///< LPD file needed for some conversion operations
    };

    enum lts_output_format {
      aldebaran,   ///< Aldebaran format (AUT)
      svc_mcrl,    ///< SVC file (mCRL specific)
      svc_mcrl2,   ///< SVC file (mCRL2 specific)
#ifdef MCRL2_BCG
      bcg,         ///< BCG
#endif
      fsm,         ///< FSM
      dot          ///< dot
    };

    enum transformation_options {
      no_transformation,                          ///< copies from one format to the other without transformation
      minimisation_modulo_strong_bisimulation,    ///< minimisation modulo strong bisimulation
      minimisation_modulo_branching_bisimulation, ///< minimisation modulo branching bisimulation
      add_bisimulation_equivalence_class          ///< adds a bisimulation equivalence class to the state information [mCRL2 SVC specific]
    };

    enum further_options {
      option_selected_transformation = 3,                ///< the selected transformation method
      option_selected_output_format = 4,                 ///< the selected output format
      option_no_state_information = 5,                   ///< dot format output specific option to not save state information
      option_tau_actions = 6                             ///< the actions that should be recognised as tau
    };

  private:

    boost::shared_ptr < sip::datatype::enumeration > transformation_method_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

    /** \brief configures tool capabilities */
    void set_capabilities(sip::tool::capabilities&) const;

    /** \brief queries the user via SQuADT if needed to obtain configuration information */
    void user_interactive_configuration(sip::configuration&);

    /** \brief check an existing configuration object to see if it is usable */
    bool check_configuration(sip::configuration const&) const;

    /** \brief performs the task specified by a configuration */
    bool perform_task(sip::configuration&);
};

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration = sip::datatype::enumeration::create("none");

  transformation_method_enumeration->add_value("modulo_strong_bisimulation");
  transformation_method_enumeration->add_value("modulo_branching_bisimulation");
  transformation_method_enumeration->add_value("add_bisimulation_equivalence_class");
}

void squadt_interactor::set_capabilities(sip::tool::capabilities& c) const {
  c.add_input_combination(lts_file_for_input, "Transformation", "aut");
  c.add_input_combination(lts_file_for_input, "Transformation", "svc");
#ifdef MCRL2_BCG
  c.add_input_combination(lts_file_for_input, "Transformation", "bcg");
#endif
}

void squadt_interactor::user_interactive_configuration(sip::configuration& c) {
  using namespace sip;
  using namespace sip::layout;
  using namespace sip::layout::elements;

  layout::tool_display::sptr display(new layout::tool_display);

  /* Create and add the top layout manager */
  layout::vertical_box::aptr top(new layout::vertical_box());

  /* First column */
  layout::manager* h = new layout::horizontal_box();

  h->add(new label("Output format : "));
  
  squadt_utility::radio_button_helper < lts_output_format >
        format_selector(h, aldebaran, "Aldebaran");

  format_selector.associate(h, svc_mcrl, "SVC/mCRL");
  format_selector.associate(h, svc_mcrl2, "SVC/mCRL2");
#ifdef MCRL2_BCG
  format_selector.associate(h, bcg, "BCG");
#endif
  format_selector.associate(h, fsm, "FSM");
  format_selector.associate(h, dot, "dot");

  /* Attach row */
  top->add(h, margins(0,5,0,5));

  h = new layout::horizontal_box();

  h->add(new label("LPD file name : "));
  text_field* lpd_file_field = static_cast < text_field* > (h->add(new text_field(c.get_input_name(".lpe"))));
  top->add(h);

  checkbox* for_dot_omit_state_information = new checkbox("Omit state information (dot only)");
  top->add(for_dot_omit_state_information);

  squadt_utility::radio_button_helper < transformation_options >
        transformation_selector(top, no_transformation, "none");

  transformation_selector.associate(top, minimisation_modulo_strong_bisimulation, "minimisation modulo strong bisimulation");
  transformation_selector.associate(top, minimisation_modulo_branching_bisimulation, "minimisation modulo branching bisimulation");
  transformation_selector.associate(top, add_bisimulation_equivalence_class, "add bisimulation equivalence class");

  h = new layout::horizontal_box();
  h->add(new label("Internal (tau) actions : "));
  text_field* tau_field = static_cast < text_field* > (h->add(new text_field("tau")));
  top->add(h);

  button* okay_button = new button("OK");

  top->add(okay_button, layout::top);

  display->set_top_manager(top);

  m_communicator.send_display_layout(display);

  /* Wait until the ok button was pressed */
  okay_button->await_change();

  std::string lts_input_file_name = c.get_object(lts_file_for_input)->get_location();

  if (c.is_fresh()) {
    if (!c.object_exists(lts_file_for_output)) {
      static char const* extensions[6] = {
        lts::extension_for_type(lts_aut),
        lts::extension_for_type(lts_mcrl),
        lts::extension_for_type(lts_mcrl2),
#ifdef MCRL2_BCG
        lts::extension_for_type(lts_bcg),
#endif
        "fsm",
        "dot"
      };

      /* Add output file to the configuration */
      std::string extension = extensions[format_selector.get_selection()];

      c.add_output(lts_file_for_output, extension, c.get_output_name("." + extension));

      /* The input type should be SVC/mcrl2 instead of just svc */
      if (format_selector.get_selection() == fsm ||
            (format_selector.get_selection() == svc_mcrl2 && c.get_object(lts_file_for_input)->get_format() != "svc")) {

        c.add_input(lpd_file_auxiliary, "lpe", lpd_file_field->get_text());
      }

      c.add_option(option_selected_transformation).append_argument(transformation_method_enumeration,
                 static_cast < transformation_options > (transformation_selector.get_selection()));

      c.add_option(option_selected_output_format).append_argument(sip::datatype::integer::naturals,
                 static_cast < unsigned int > (format_selector.get_selection()));

      if (format_selector.get_selection() == dot && for_dot_omit_state_information->get_status()) {
        c.add_option(option_no_state_information);
      }

      c.add_option(option_tau_actions).append_argument(datatype::string::standard, tau_field->get_text());
    }
  }
}

bool squadt_interactor::check_configuration(sip::configuration const& c) const {
  bool result = true;

  result &= c.object_exists(lts_file_for_input);
  result &= c.object_exists(lts_file_for_output);

  if (result) {
    if (c.get_object(lts_file_for_output)->get_format() == "fsm" ||
        (c.get_object(lts_file_for_output)->get_format() == "svc" && c.get_object(lts_file_for_input)->get_format() != "svc")) {
 
      result &= c.object_exists(lpd_file_auxiliary);
    }
  }

  result &= c.option_exists(option_selected_output_format);
  result &= c.option_exists(option_selected_transformation);

  if (c.option_exists(option_tau_actions)) {
    lts_eq_options eq_opts;

    /* Need to detect whether the next operation completes successfully or not, exceptions anyone? */
    lts_reduce_add_tau_actions(eq_opts,(boost::any_cast < std::string > (c.get_option_value(option_tau_actions)).c_str()));
  }

  return (result);
}

bool squadt_interactor::perform_task(sip::configuration& c) {
  lts l;

  c.get_object(lts_file_for_input)->get_location();

  if (!read_lts_from_file(l, c.get_object(lts_file_for_input)->get_location(),
               lts::parse_format(c.get_object(lts_file_for_input)->get_format().c_str()))) {

    send_error("Fatal: error reading input from `" + c.get_object(lts_file_for_input)->get_location() + "'!");

    return (false);
  }
 
  transformation_options method = static_cast < transformation_options > (boost::any_cast < size_t > (c.get_option_value(option_selected_transformation)));

  if (method != no_transformation) {
    lts_equivalence equivalence = lts_eq_none;

    lts_eq_options  eq_opts;

    set_eq_options_defaults(eq_opts);

    switch (method) {
      case minimisation_modulo_strong_bisimulation:
        equivalence = lts_eq_strong;
        break;
      case minimisation_modulo_branching_bisimulation:
        equivalence = lts_eq_branch;
        break;
      //case:
      //  equivalence = lts_eq_obs_trace
      //case:
      //  equivalence = lts_eq_trace;
      case add_bisimulation_equivalence_class:
        eq_opts.reduce.add_class_to_state = true;
        break;
      default:
        break;
    }

    if (c.option_exists(option_tau_actions)) {
      lts_reduce_add_tau_actions(eq_opts, (boost::any_cast < std::string > (c.get_option_value(option_tau_actions)).c_str()));
    }

    gsVerboseMsg("reducing LTS...\n");

    if (!l.reduce(equivalence,eq_opts)) {
      return (false);
    }
  }
 
  bool result = true;

  std::string lpe_path;

  if (c.object_exists(lpd_file_auxiliary)) {
    lpe_path = c.get_object(lpd_file_auxiliary)->get_location();
  }

  switch (static_cast < lts_output_format > (boost::any_cast < long int > (c.get_option_value(option_selected_output_format)))) {
      case fsm:
      case dot:
        result = write_lts_to_file(l, c.get_object(lts_file_for_output)->get_location(),
                 get_alt_format(c.get_object(lts_file_for_output)->get_format()), lpe_path, c.option_exists(option_no_state_information));
        break;
      case aldebaran:
        result = write_lts_to_file(l, c.get_object(lts_file_for_output)->get_location(), lts_aut, lpe_path);
        break;
#ifdef MCRL2_BCG
      case bcg:
        result = write_lts_to_file(l, c.get_object(lts_file_for_output)->get_location(), lts_bcg, lpe_path);
        break;
#endif
      case svc_mcrl:
        result = write_lts_to_file(l, c.get_object(lts_file_for_output)->get_location(), lts_mcrl, lpe_path);
        break;
      case svc_mcrl2:
      default: 
        result = write_lts_to_file(l, c.get_object(lts_file_for_output)->get_location(), lts_mcrl2, lpe_path);
        break;
  }

  send_hide_display();

  return (result);
}
#endif

using namespace std;

static ATerm get_lpe(string const& filename)
{
  if ( filename == "" )
  {
    return NULL;
  }

  FILE* file = fopen(filename.c_str(),"rb");
  if ( file == NULL )
  {
    gsErrorMsg("unable to open LPE-file '%s'\n",filename.c_str());
    return NULL;
  }
  
  ATerm t = ATreadFromFile(file);
  fclose(file);
  
  if ( (t == NULL) || (ATgetType(t) != AT_APPL) || !(gsIsSpecV1((ATermAppl) t) || !strcmp(ATgetName(ATgetAFun((ATermAppl) t)),"spec2gen")) )
  {
    gsErrorMsg("invalid LPE-file '%s'\n",filename.c_str());
    return NULL;
  }

  return t;
}

static string get_base(string const& s)
{
  string::size_type pos = s.find_last_of('.');
  
  if ( pos == string::npos )
  {
    return s;
  } else {
    return s.substr(0,pos);
  }
}

static alt_lts_type get_alt_extension(string const& s)
{
  string::size_type pos = s.find_last_of('.');
  
  if ( pos != string::npos )
  {
    string ext = s.substr(pos+1);

    if ( ext == "fsm" )
    {
      gsVerboseMsg("detected FSM extension\n");
      return alt_lts_fsm;
    } else if ( ext == "dot" )
    {
      return alt_lts_dot;
    }
  }

  return alt_lts_none;
}

static alt_lts_type get_alt_format(std::string const& s) {
  if (s == "fsm") {
    return alt_lts_fsm;
  }
  else if (s == "dot") {
    return alt_lts_dot;
  }

  return alt_lts_none;
}

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
    "  dot     .dot  GraphViz format                   output only\n"
    "  fsm     .fsm  Finite State Machine format       output only\n"
    "  mcrl    .svc  mCRL SVC format\n"
    "  mcrl2   .svc  mCRL2 SVC format                  default\n"
    "\n"
    );
}

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Convert the labelled transition system (LTS) from INFILE to OUTFILE in the\n"
    "requested format after applying the selected minimisation method (default is\n"
    "none). If OUTFILE is not supplied, stdout is used. If INFILE is not supplied,\n"
    "stdin is used.\n"
    "\n"
    "The output format is determined by the extension of OUTFILE, whereas the input\n"
    "format is determined by the content of INFILE. Options --in and --out can be\n"
    "used to force the input and output formats.\n"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -h, --help            display this help message and terminate\n"
    "      --version         display version information and terminate\n"
    "  -q, --quiet           do not display warning messages\n"
    "  -v, --verbose         display concise intermediate messages\n"
    "  -f, --formats         list accepted formats\n"
    "  -i, --in=FORMAT       use FORMAT as the input format\n"
    "  -o, --out=FORMAT      use FORMAT as the output format\n"
    "  -l, --lpe=FILE        use FILE as the LPE from which the input LTS was\n"
    "                        generated; this is needed to store the correct\n"
    "                        parameter names of states when saving in fsm format and\n"
    "                        to convert non-mCRL2 LTSs to a mCRL2 LTS\n"
    "  -n, --no-state        leave out state information when saving in dot format\n"
    "\n"
    "Minimisation options:\n"
    "      --none            do not minimise (default)\n"
    "  -s, --strong          minimise using strong bisimulation\n"
    "  -b, --branching       minimise using branching bisimulation\n"
    "  -a, --add             do not minimise but save a copy of the original LTS\n"
    "                        extended with a state parameter indicating the\n"
    "                        bisimulation class a state belongs to (only for mCRL2)\n"
    "      --tau=ACTNAMES    consider actions with a name in the comma separated\n"
    "                        list ACTNAMES to be a internal (tau) actions in\n"
    "                        addition to those defined as such by the input\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %i)\n", REVISION);
}

bool read_lts_from_stdin(lts& l, lts_type intype) {
  gsVerboseMsg("reading LTS from stdin...\n");
  if ( !l.read_from(cin,intype) )
  {
    gsErrorMsg("cannot read LTS from stdin\n");
    gsErrorMsg("use -v/--verbose for more information\n");
    return (false);
  }

  return (true);
}

bool read_lts_from_file(lts& l, std::string const& infile, lts_type intype) {
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
      gsErrorMsg("use -v/--verbose for more information\n");
      return (false);
    }
  }

  return (true);
}

bool write_lts_to_stdout(lts& l, alt_lts_type outtype, std::string const& lpefile, bool print_dot_state) {
  switch ( outtype )
  {
    case alt_lts_fsm:
      if ( !write_lts_to_fsm(l,cout,get_lpe(lpefile)) )
      {
        gsErrorMsg("cannot write LTS to stdout\n");
        gsErrorMsg("use -v/--verbose for more information\n");
        return false;
      }
      break;
    case alt_lts_dot:
      {
        string str_stdout = "stdout";
        if ( !write_lts_to_dot(l,cout,str_stdout,print_dot_state) )
        {
          gsErrorMsg("cannot write LTS to stdout\n");
          gsErrorMsg("use -v/--verbose for more information\n");
          return false;
        }
        break;
      }
    default:
      assert(0);
      gsErrorMsg("do not know how to handle output format\n");
      return false;
  }

  return (true);
}

bool write_lts_to_stdout(lts& l, lts_type outtype, std::string const& lpefile, bool print_dot_state) {
  gsVerboseMsg("writing LTS to stdout...\n");

  if ( !l.write_to(cout,outtype) )
  {
    gsErrorMsg("cannot write LTS to stdout\n");
    gsErrorMsg("use -v/--verbose for more information\n");
    return false;
  }

  return (true);
}

bool write_lts_to_file(lts& l, std::string const& outfile, alt_lts_type outtype, std::string const& lpefile, bool print_dot_state) {
  gsVerboseMsg("writing LTS to '%s'...\n",outfile.c_str());

  switch ( outtype )
  {
    case alt_lts_fsm:
      if ( !write_lts_to_fsm(l,outfile,get_lpe(lpefile)) )
      {
        gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
        gsErrorMsg("use -v/--verbose for more information\n");
        return (false);
      }
      break;
    case alt_lts_dot:
      {
        if ( !write_lts_to_dot(l,outfile,get_base(outfile),print_dot_state) )
        {
          gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
          gsErrorMsg("use -v/--verbose for more information\n");
          return (false);
        }
        break;
      }
    default:
      assert(0);
      gsErrorMsg("do not know how to handle output format\n");
      return (false);
  }

  return (true);
}

bool write_lts_to_file(lts& l, std::string const& outfile, lts_type outtype, std::string const& lpefile) {
  gsVerboseMsg("writing LTS to '%s'...\n",outfile.c_str());
  if ( lpefile == "" )
  {
    if ( !l.write_to(outfile,outtype) )
    {
      gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
      gsErrorMsg("use -v/--verbose for more information\n");
      return (false);
    }
  } else {
    lpe::specification spec;
    if ( !spec.load(lpefile) )
    {
      gsErrorMsg("cannot read LPE file '%s'\n",lpefile.c_str());
      gsErrorMsg("use -v/--verbose for more information\n");
      return (false);
    }
    if ( !l.write_to(outfile,outtype,&spec) )
    {
      gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
      gsErrorMsg("use -v/--verbose for more information\n");
      return (false);
    }
  }

  return (true);
}

int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

  #define ShortOptions      "hqvi:o:fl:nsbtua"
  #define VersionOption     0x1
  #define NoneOption        0x2
  #define TauOption         0x3
  struct option LongOptions[] = { 
    {"help"      , no_argument,         NULL, 'h'},
    {"version"   , no_argument,         NULL, VersionOption},
    {"quiet"     , no_argument,         NULL, 'q'},
    {"verbose"   , no_argument,         NULL, 'v'},
    {"in"        , required_argument,   NULL, 'i'},
    {"out"       , required_argument,   NULL, 'o'},
    {"formats"   , no_argument,         NULL, 'f'},
    {"lpe"       , required_argument,   NULL, 'l'},
    {"no-state"  , no_argument,         NULL, 'n'},
    {"strong"    , no_argument,         NULL, 's'},
    {"none"      , no_argument,         NULL, NoneOption},
    {"branching" , no_argument,         NULL, 'b'},
    {"trace"     , no_argument,         NULL, 't'},
    {"obs-trace" , no_argument,         NULL, 'u'},
    {"tau"       , required_argument,   NULL, TauOption},
    {"add"       , no_argument,         NULL, 'a'},
    {0, 0, 0, 0}
  };

  bool verbose = false;
  bool quiet = false;
  lts_type intype = lts_none;
  lts_type outtype = lts_none;
  int opt;
  string lpefile;
  bool use_alt_outtype = false;
  alt_lts_type alt_outtype = alt_lts_none;
  bool print_dot_state = true;
  lts_equivalence equivalence = lts_eq_none;
  lts_eq_options eq_opts; set_eq_options_defaults(eq_opts);

#ifdef ENABLE_SQUADT_CONNECTIVITY
  squadt_interactor c;

  if (!c.try_interaction(argc, argv)) {
#endif
    while ( (opt = getopt_long(argc, argv, ShortOptions, LongOptions, NULL)) != -1 )
    {
      switch ( opt )
      {
        case 'h':
          print_help(stderr,argv[0]);
          return 0;
        case VersionOption:
          print_version(stderr);
          return 0;
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
        case 'o':
          if ( (outtype != lts_none) || use_alt_outtype )
          {
            fprintf(stderr,"warning: output format has already been specified; extra option ignored\n");
          } else {
            outtype = lts::parse_format(optarg);
            if ( outtype == lts_none )
            {
              alt_outtype = get_alt_format(optarg);
              if ( alt_outtype == alt_lts_none )
              {
                fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
              } else {
                use_alt_outtype = true;
              }
            }
          }
          break;
        case 'f':
          print_formats(stderr);
          return 0;
        case 'l':
          if ( lpefile != "" )
          {
            fprintf(stderr,"warning: LPE file has already been specified; extra option ignored\n");
          }
          lpefile = optarg;
          break;
        case 'n':
          print_dot_state = false;
          break;
        case NoneOption:
          equivalence = lts_eq_none;
          break;
        case 's':
          equivalence = lts_eq_strong;
          break;
        case 'b':
          equivalence = lts_eq_branch;
          break;
        case 't':
          equivalence = lts_eq_trace;
          break;
        case 'u':
          equivalence = lts_eq_obs_trace;
          break;
        case TauOption:
          lts_reduce_add_tau_actions(eq_opts,optarg);
          break;
        case 'a':
          eq_opts.reduce.add_class_to_state = true;
          break;
        default:
          break;
      }
    }
 
    if ( quiet && verbose )
    {
      gsErrorMsg("options -q/--quiet and -v/--verbose cannot be used together\n");
      return 1;
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
    bool use_stdout = (optind+1 >= argc);
  
    string infile;
    string outfile;

    if ( !use_stdin )
    {
      infile = argv[optind];
    }
    if ( !use_stdout )
    {
      outfile = argv[optind+1];
      if ( (outtype == lts_none) && !use_alt_outtype )
      {
          gsVerboseMsg("trying to detect output format by extension...\n");
          outtype = lts::guess_format(outfile);
          if ( outtype == lts_none )
          {
            alt_outtype = get_alt_extension(outfile);
            if ( alt_outtype == alt_lts_none )
            {
              if ( lpefile != "" )
              {
                gsWarningMsg("no output format set; using fsm because --lpe was used\n");
                alt_outtype = alt_lts_fsm;
                use_alt_outtype = true;
              } else {
                gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
                outtype = lts_mcrl2;
              }
            } else {
              use_alt_outtype = true;
              if ( (alt_outtype == alt_lts_fsm) && (lpefile == "") )
              {
                gsWarningMsg("parameter names are unknown (use -l/--lpe option)\n");
              }
            }
          }
      }
    } else {
        if ( (outtype == lts_none) && !use_alt_outtype )
        {
          if ( lpefile != "" )
          {
            gsWarningMsg("no output format set; using fsm because --lpe was used\n");
            alt_outtype = alt_lts_fsm;
            use_alt_outtype = true;
          } else {
            gsWarningMsg("no output format set; using default (aut)\n");
            outtype = lts_aut;
          }
        }
    }

    lts l;
 
    if (!(use_stdin ? read_lts_from_stdin(l, intype) : read_lts_from_file(l, infile, intype)))
    {
      return (1);
    }
    if ( (lpefile != "") && !(
               ((outtype == lts_mcrl2) && (l.get_type() != lts_mcrl2)) ||
               (use_alt_outtype && (alt_outtype == alt_lts_fsm) && (l.get_type() == lts_mcrl2))
               ) )
    {
      gsWarningMsg("ignoring --lpe option as it is not usable with this input/output combination\n");
      lpefile = "";
    }

 
    if ( equivalence != lts_eq_none )
    {
      gsVerboseMsg("reducing LTS...\n");
      l.reduce(equivalence,eq_opts);
    }
 
    if ( use_stdout )
    {
      if (!(use_alt_outtype ? write_lts_to_stdout(l, alt_outtype, lpefile, print_dot_state): write_lts_to_stdout(l, outtype, lpefile, print_dot_state)))
      {
        return (1);
      }
    } else {
      if (!(use_alt_outtype ? write_lts_to_file(l, outfile, alt_outtype, lpefile, print_dot_state) : write_lts_to_file(l, outfile, outtype, lpefile)))
      {
        return (1);
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
