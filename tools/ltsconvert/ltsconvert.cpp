// Author(s): Muck van Weerdenburg
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ltsconvert.cpp

#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "mcrl2/struct.h"
#include "mcrl2/lts/liblts.h"
#include "mcrl2/setup.h"
#include "mcrl2/print/messaging.h"

#define NAME "ltsconvert"
#define VERSION "July 2007"

using namespace ::mcrl2::lts;
using namespace ::mcrl2::utilities;

bool read_lts_from_file(lts&, std::string const&, lts_type, std::string const&, bool perform_reachability_check);
bool write_lts_to_stdout(lts&, lts_type outtype, std::string const&, bool);
bool write_lts_to_file(lts&, std::string const&, lts_type outtype, std::string const&, bool);

// SQuADT protocol interface
#ifdef ENABLE_SQUADT_CONNECTIVITY
#include <mcrl2/utilities/squadt_interface.h>

class squadt_interactor : public mcrl2::utilities::squadt::mcrl2_tool_interface {

  private:

    static const char*  lts_file_for_input;  ///< file containing an LTS that can be imported using the LTS library
    static const char*  lts_file_for_output; ///< file used to write the output to
    static const char*  lps_file_auxiliary;  ///< LPS file needed for some conversion operations

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
      minimisation_modulo_trace_equivalence,      ///< minimisation modulo trace equivalence
      minimisation_modulo_weak_trace_equivalence, ///< minimisation modulo weak trace equivalence
      determinisation                             ///< determinisation
    };

    static const char* option_selected_transformation;               ///< the selected transformation method
    static const char* option_selected_output_format;                ///< the selected output format
    static const char* option_no_reachability_check;                 ///< do not check reachability of input LTS
    static const char* option_no_state_information;                  ///< dot format output specific option to not save state information
    static const char* option_tau_actions;                           ///< the actions that should be recognised as tau
    static const char* option_add_bisimulation_equivalence_class;    ///< adds bisimulation equivalence class to the state information of a state instead of actually reducing modulo bisimulation [mCRL2 SVC specific]

  private:

    boost::shared_ptr < tipi::datatype::enumeration > transformation_method_enumeration;
    boost::shared_ptr < tipi::datatype::enumeration > output_format_enumeration;

  public:

    /** \brief constructor */
    squadt_interactor();

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
const char* squadt_interactor::lts_file_for_output = "lts_out";
const char* squadt_interactor::lps_file_auxiliary  = "lps_aux";

const char* squadt_interactor::option_selected_transformation            = "selected_transformation";
const char* squadt_interactor::option_selected_output_format             = "selected_output_format";
const char* squadt_interactor::option_no_reachability_check              = "no_reachability_check";
const char* squadt_interactor::option_no_state_information               = "no_state_information";
const char* squadt_interactor::option_tau_actions                        = "tau_actions";
const char* squadt_interactor::option_add_bisimulation_equivalence_class = "add_bisimulation_equivalence_class";

squadt_interactor::squadt_interactor() {
  transformation_method_enumeration.reset(new tipi::datatype::enumeration("none"));

  transformation_method_enumeration->add_value("modulo_strong_bisimulation");
  transformation_method_enumeration->add_value("modulo_branching_bisimulation");
  transformation_method_enumeration->add_value("modulo_trace_equivalence");
  transformation_method_enumeration->add_value("modulo_weak_trace_equivalence");
  transformation_method_enumeration->add_value("determinise");

  output_format_enumeration.reset(new tipi::datatype::enumeration("Aldebaran"));

  output_format_enumeration->add_value("SVC_mCRL");
  output_format_enumeration->add_value("SVC_mCRL2");
  output_format_enumeration->add_value("BCG");
  output_format_enumeration->add_value("FSM");
  output_format_enumeration->add_value("dot");
}

void squadt_interactor::set_capabilities(tipi::tool::capabilities& c) const {
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("aut", tipi::mime_type::text), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl2", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc+mcrl", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("svc", tipi::mime_type::application), tipi::tool::category::conversion);
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("fsm", tipi::mime_type::text), tipi::tool::category::conversion);
#ifdef MCRL2_BCG
  c.add_input_configuration(lts_file_for_input, tipi::mime_type("bcg", tipi::mime_type::application), tipi::tool::category::conversion);
#endif
}

void squadt_interactor::user_interactive_configuration(tipi::configuration& c) {
  using namespace tipi;
  using namespace tipi::layout;
  using namespace tipi::layout::elements;

  /* Create display */
  tipi::layout::tool_display d;

  layout::vertical_box& m = d.create< vertical_box >().set_default_margins(margins(0, 5, 0, 5));

  /* Helper for format selection */
  mcrl2::utilities::squadt::radio_button_helper < lts_output_format > format_selector(d);

  m.append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Output format : ")).
                append(format_selector.associate(aldebaran, "Aldebaran",true)).
                append(format_selector.associate(svc_mcrl, "SVC/mCRL")).
                append(format_selector.associate(svc_mcrl2, "SVC/mCRL2")).
#ifdef MCRL2_BCG
                append(format_selector.associate(bcg, "BCG")).
#endif
                append(format_selector.associate(fsm, "FSM")).
                append(format_selector.associate(dot, "dot")),
           margins(0,5,0,5));
  
  text_field& lps_file_field        = d.create< text_field >();
  checkbox&   check_reachability    = d.create< checkbox >();
  checkbox&   add_state_information = d.create< checkbox >();

  m.append(d.create< label >()).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("LPS file name : ")).
                append(lps_file_field)).
    append(check_reachability.set_label("Perform reachability check").set_status(true)).
    append(add_state_information.set_label("Add state information").set_status(true));

//  mcrl2::utilities::squadt::change_visibility_on_toggle(format_selector.get_button(dot), top.get(), add_state_information);
//  format_selector.get_button(dot).on_change(boost::bind(::mcrl2::utilities::squadt::change_visibility_on_toggle, format_selector.get_button(dot), top.get(), add_state_information));

  /* Helper for transformation selection */
  mcrl2::utilities::squadt::radio_button_helper < transformation_options > transformation_selector(d);

  m.append(d.create< label >().set_text("LTS transformation:")).
    append(transformation_selector.associate(no_transformation, "none", true)).
    append(transformation_selector.associate(minimisation_modulo_strong_bisimulation, "reduction modulo strong bisimulation")).
    append(transformation_selector.associate(minimisation_modulo_branching_bisimulation, "reduction modulo branching bisimulation")).
    append(transformation_selector.associate(minimisation_modulo_trace_equivalence, "reduction modulo trace equivalence")).
    append(transformation_selector.associate(minimisation_modulo_weak_trace_equivalence, "reduction modulo weak trace equivalence")).
    append(transformation_selector.associate(determinisation, "determinisation"));
  
  checkbox&   bisimulation_add_eq_classes = d.create< checkbox >();
  text_field& tau_field                   = d.create< text_field >();

  m.append(d.create< label >()).
    append(bisimulation_add_eq_classes.set_label("Add equivalence classes to state instead of reducing LTS")).
    append(d.create< horizontal_box >().
                append(d.create< label >().set_text("Internal (tau) actions : ")).
                append(tau_field.set_text("tau")));

  /* Attach events */
//  transformation_selector.get_button(no_transformation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(no_transformation), top.get(), tau_field));
//  transformation_selector.get_button(minimisation_modulo_strong_bisimulation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(minimisation_modulo_strong_bisimulation), top.get(), bisimulation_add_eq_classes));
//  transformation_selector.get_button(minimisation_modulo_branching_bisimulation).
//        on_change(boost::bind(mcrl2::utilities::squadt::change_visibility_on_toggle,
//              transformation_selector.get_button(minimisation_modulo_branching_bisimulation), top.get(), bisimulation_add_eq_classes));

  button& okay_button = d.create< button >().set_label("OK");

  // Add some default values for existing options in the current configuration
  if (c.option_exists(option_selected_output_format)) {
    format_selector.set_selection(static_cast < lts_output_format > (
        c.get_option_argument< size_t >(option_selected_output_format, 0)));
  }
  if (c.option_exists(option_selected_transformation)) {
    transformation_selector.set_selection(static_cast < transformation_options > (
        c.get_option_argument< size_t >(option_selected_transformation, 0)));
  }
  if (c.input_exists(lps_file_auxiliary)) {
      lps_file_field.set_text(c.get_input(lps_file_auxiliary).get_location());
  }
  if (c.option_exists(option_no_reachability_check)) {
    check_reachability.set_status(c.get_option_argument< bool >(option_no_reachability_check));
  }
  if (c.option_exists(option_no_state_information)) {
    add_state_information.set_status(c.get_option_argument< bool >(option_no_state_information));
  }
  if (c.option_exists(option_add_bisimulation_equivalence_class)) {
    bisimulation_add_eq_classes.set_status(c.get_option_argument< bool >(option_add_bisimulation_equivalence_class));
  }
  if (c.option_exists(option_tau_actions)) {
    tau_field.set_text(c.get_option_argument< std::string >(option_tau_actions));
  }

  send_display_layout(d.set_manager(m.append(okay_button, layout::top)));

  /* Wait until the ok button was pressed */
  okay_button.await_change();
  
  static char const* extensions[6] = {
    lts::extension_for_type(lts_aut),
    lts::extension_for_type(lts_mcrl),
    lts::extension_for_type(lts_mcrl2),
#ifdef MCRL2_BCG
    lts::extension_for_type(lts_bcg),
#endif
    lts::extension_for_type(lts_fsm),
    lts::extension_for_type(lts_dot)
  };

  /* Add output file to the configuration */
  if (c.output_exists(lts_file_for_output)) {
    std::string  extension(extensions[format_selector.get_selection()]);
    tipi::object& output_file = c.get_output(lts_file_for_output);
 
    output_file.set_mime_type(tipi::mime_type(extension));
    output_file.set_location(c.get_output_name("." + extension));
  }
  else {
    std::string  extension(extensions[format_selector.get_selection()]);

    c.add_output(lts_file_for_output, tipi::mime_type(extension), c.get_output_name("." + extension));
  }

  /* Add lps file when output is FSM format or when the output is mCRL2 and the input is Aldebaran or mCRL */
  if ((format_selector.get_selection() == fsm && (
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "svc" ||
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "svc+mcrl" ||
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "svc+mcrl2"))
   || (format_selector.get_selection() == svc_mcrl2 && (
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "aut" ||
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "svc" ||
         c.get_input(lts_file_for_input).get_mime_type().get_sub_type() == "svc+mcrl"))) {

    if (c.input_exists(lps_file_auxiliary)) {
      c.get_input(lps_file_auxiliary).set_location(lps_file_field.get_text());
    }
    else {
      c.add_input(lps_file_auxiliary, tipi::mime_type("lps", tipi::mime_type::application), lps_file_field.get_text());
    }
  }
  else {
    c.remove_input(lps_file_auxiliary);
  }

  transformation_options selected_transformation = static_cast < transformation_options > (transformation_selector.get_selection());

  c.add_option(option_selected_transformation).append_argument(transformation_method_enumeration, selected_transformation);
  c.add_option(option_selected_output_format).append_argument(output_format_enumeration, static_cast < lts_output_format > (format_selector.get_selection()));
  
  if ((selected_transformation == minimisation_modulo_strong_bisimulation ||
       selected_transformation == minimisation_modulo_branching_bisimulation)) {

    c.add_option(option_add_bisimulation_equivalence_class).
        set_argument_value< 0, tipi::datatype::boolean >(bisimulation_add_eq_classes.get_status());
  }
  else {
    c.remove_option(option_add_bisimulation_equivalence_class);
  }

  c.add_option(option_no_reachability_check).set_argument_value< 0, tipi::datatype::boolean >(check_reachability.get_status());

  if (format_selector.get_selection() == dot) {
    c.add_option(option_no_state_information).
       set_argument_value< 0, tipi::datatype::boolean >(add_state_information.get_status());
  }
  else {
    c.remove_option(option_no_state_information);
  }

  if (!tau_field.get_text().empty()) {
    c.add_option(option_tau_actions).set_argument_value< 0, tipi::datatype::string >(tau_field.get_text());
  }
  else {
    c.remove_option(option_tau_actions);
  }

  send_clear_display();
}

bool squadt_interactor::check_configuration(tipi::configuration const& c) const {
  bool result = true;

  result &= c.input_exists(lts_file_for_input);
  result &= c.output_exists(lts_file_for_output);
  result &= c.option_exists(option_selected_transformation);

  if (c.option_exists(option_tau_actions)) {
    lts_eq_options eq_opts;

    /* Need to detect whether the next operation completes successfully or not, exceptions anyone? */
    lts_reduce_add_tau_actions(eq_opts,(boost::any_cast < std::string > (c.get_option_argument(option_tau_actions)).c_str()));
  }

  return (result);
}

bool squadt_interactor::perform_task(tipi::configuration& c) {
  lts l;

  std::string lps_path;

  if (c.input_exists(lps_file_auxiliary)) {
    lps_path = c.get_input(lps_file_auxiliary).get_location();
  }

  if ( !read_lts_from_file(l, c.get_input(lts_file_for_input).get_location(),lts_none,lps_path,!c.option_exists(option_no_reachability_check)) ) {

    send_error("Fatal: error reading input from `" + c.get_input(lts_file_for_input).get_location() + "'!");

    return (false);
  }
 
  transformation_options method = static_cast < transformation_options > (c.get_option_argument< size_t >(option_selected_transformation));

  if (method != no_transformation) {
    lts_equivalence equivalence = lts_eq_none;
    bool determinise = false;

    lts_eq_options  eq_opts;

    set_eq_options_defaults(eq_opts);

    switch (method) {
      case minimisation_modulo_strong_bisimulation:
        equivalence = lts_eq_strong;
        break;
      case minimisation_modulo_branching_bisimulation:
        equivalence = lts_eq_branch;
        break;
      case minimisation_modulo_trace_equivalence:
        equivalence = lts_eq_trace;
        break;
      case minimisation_modulo_weak_trace_equivalence:
        equivalence = lts_eq_weak_trace;
        break;
      case determinisation:
        determinise = true;
        break;
      default:
        break;
    }

    if (c.option_exists(option_add_bisimulation_equivalence_class)) {
      eq_opts.reduce.add_class_to_state = c.get_option_argument< bool >(option_add_bisimulation_equivalence_class);
    }

    if (c.option_exists(option_tau_actions)) {
      lts_reduce_add_tau_actions(eq_opts, c.get_option_argument< std::string >(option_tau_actions).c_str());
    }

    if ( determinise )
    {
      gsVerboseMsg("determinising LTS..\n");

      gsVerboseMsg("before determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
      l.determinise();
      gsVerboseMsg("after determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
    } else {
      gsVerboseMsg("reducing LTS...\n");

      gsVerboseMsg("before reduction: %lu states and %lu transitions \n",l.num_states(),l.num_labels(),l.num_transitions());
      if (!l.reduce(equivalence,eq_opts)) {
        return (false);
      }
      gsVerboseMsg("after reduction: %lu states and %lu transitions\n",l.num_states(),l.num_labels(),l.num_transitions());
    }
  }
 
  bool result = true;

  result = write_lts_to_file(l, c.get_output(lts_file_for_output).get_location(),
                 lts::parse_format(c.get_output(lts_file_for_output).get_mime_type().get_sub_type().c_str()), lps_path, c.option_exists(option_no_state_information));

  send_clear_display();

  return (result);
}
#endif

using namespace std;

static ATermAppl get_lps(string const& filename)
{
  if ( filename == "" )
  {
    return NULL;
  }

  FILE* file = fopen(filename.c_str(),"rb");
  if ( file == NULL )
  {
    gsErrorMsg("unable to open LPS-file '%s'\n",filename.c_str());
    return NULL;
  }
  
  ATerm t = ATreadFromFile(file);
  fclose(file);
  
  if ( (t == NULL) || (ATgetType(t) != AT_APPL) || !(gsIsSpecV1((ATermAppl) t) || !strcmp(ATgetName(ATgetAFun((ATermAppl) t)),"spec2gen")) )
  {
    gsErrorMsg("invalid LPS-file '%s'\n",filename.c_str());
    return NULL;
  }

  return (ATermAppl) t;
}

static lts_extra get_extra(lts_type type, string const &lps_file, bool print_dot_states, string const &base_name)
{
  if ( type == lts_dot )
  {
    lts_dot_options opts;
    opts.name = new string(base_name); // XXX Ugh!
    opts.print_states = print_dot_states;
    return lts_extra(opts);
  } else {
    if ( !lps_file.empty() )
    {
      ATermAppl spec = get_lps(lps_file);

      if ( spec != NULL )
      {
        if ( gsIsSpecV1(spec) )
        {
          return lts_extra(new lps::specification(spec)); // XXX Ugh!
        } else {
          return lts_extra((ATerm) spec);
        }
      }
    }
    return lts_extra();
  }
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
    "  fsm     .fsm  Finite State Machine format\n"
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
    "Options:\n"
    "  -h, --help             display this help message and terminate\n"
    "      --version          display version information and terminate\n"
    "  -q, --quiet            do not display any unrequested information\n"
    "  -v, --verbose          display concise intermediate messages\n"
    "  -d, --debug            display detailed intermediate messages\n"
    "  -f, --formats          list accepted formats\n"
    "  -iFORMAT, --in=FORMAT  use FORMAT as the input format\n"
    "  -oFORMAT, --out=FORMAT use FORMAT as the output format\n"
    "  -lFILE, --lps=FILE     use FILE as the LPS from which the input LTS was\n"
    "                         generated; this is needed to store the correct\n"
    "                         parameter names of states when saving in fsm format and\n"
    "                         to convert non-mCRL2 LTSs to a mCRL2 LTS\n"
    "      --no-reach         do not perform a reachability check on the input LTS\n"
    "  -n, --no-state         leave out state information when saving in dot format\n"
    "  -D, --determinise      determinise LTS\n"
    "\n"
    "Minimisation options:\n"
    "      --none             do not minimise (default)\n"
    "  -s, --strong           minimise using strong bisimulation\n"
    "  -b, --branching        minimise using branching bisimulation\n"
    "  -t, --trace            determinise and then minimise using trace equivalence\n"
    "  -u, --weak-trace       determinise and then minimise using weak trace equivalence\n"
    "  -a, --add              do not minimise but save a copy of the original LTS\n"
    "                         extended with a state parameter indicating the\n"
    "                         bisimulation class a state belongs to (only for mCRL2)\n"
    "      --tau=ACTNAMES     consider actions with a name in the comma separated\n"
    "                         list ACTNAMES to be internal (tau) actions in\n"
    "                         addition to those defined as such by the input\n",
    Name);
}

static void print_version(FILE *f)
{
  fprintf(f,NAME " " VERSION " (revision %s)\n", REVISION);
}

void reachability_check(lts &l)
{
  gsVerboseMsg("checking reachability of input LTS...\n");
  if ( !l.reachability_check() )
  {
    gsWarningMsg("not all states of the input LTS are reachable from the initial state; this will invoke unspecified behaviour in LTS tools (including this one)!\n");
  }
}

bool read_lts_from_stdin(lts& l, lts_type intype, std::string const& lpsfile, bool perform_reachability_check) {
  gsVerboseMsg("reading LTS from stdin...\n");
  
  lts_extra extra = get_extra(intype, lpsfile, false, "");
  
  if ( !l.read_from(cin,intype,extra) )
  {
    gsErrorMsg("cannot read LTS from stdin\n");
    gsErrorMsg("use -v/--verbose for more information\n");
    return (false);
  }
  
  if ( perform_reachability_check )
  {
    reachability_check(l);
  }

  return (true);
}

bool read_lts_from_file(lts& l, std::string const& infile, lts_type intype, std::string const& lpsfile, bool perform_reachability_check) {
  gsVerboseMsg("reading LTS from '%s'...\n",infile.c_str());
  
  lts_extra extra = get_extra(intype, lpsfile, false, "");
 
  if ( !l.read_from(infile,intype,extra) )
  {
    bool b = true;
    if ( intype == lts_none ) // XXX really do this?
    {
      gsVerboseMsg("reading failed; trying to force format by extension...\n");
      intype = lts::guess_format(infile);
      if ( intype == lts_none )
      {
        gsVerboseMsg("unsupported input format extension\n");
      } else {
        extra = get_extra(intype, lpsfile, false, "");
        if ( l.read_from(infile,intype,extra) )
        {
          b = false;
        } else {
          gsVerboseMsg("reading based on format extension failed as well\n");
        }
      }
    }
    if ( b )
    {
      gsErrorMsg("cannot read LTS from file '%s'\n",infile.c_str());
      gsErrorMsg("use -v/--verbose for more information\n");
      return (false);
    }
  }

  if ( perform_reachability_check )
  {
    reachability_check(l);
  }

  return (true);
}

bool write_lts_to_stdout(lts& l, lts_type outtype, std::string const& lpsfile, bool print_dot_states) {
  gsVerboseMsg("writing LTS to stdout...\n");

  lts_extra extra = get_extra(outtype, lpsfile, print_dot_states, "stdout");

  if ( !l.write_to(cout,outtype,extra) )
  {
    gsErrorMsg("cannot write LTS to stdout\n");
    gsErrorMsg("use -v/--verbose for more information\n");
    return false;
  }

  return true;
}

bool write_lts_to_file(lts& l, std::string const& outfile, lts_type outtype, std::string const& lpsfile, bool print_dot_states) {
  gsVerboseMsg("writing LTS to '%s'...\n",outfile.c_str());
  
  lts_extra extra = get_extra(outtype, lpsfile, print_dot_states, get_base(outfile));
  
  if ( !l.write_to(outfile,outtype,extra) )
  {
    gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
    gsErrorMsg("use -v/--verbose for more information\n");
    return false;
  }

  return true;
}

int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

  #define ShortOptions      "hqvdi:o:fl:nsbtuaD"
  #define VersionOption     0x1
  #define NoneOption        0x2
  #define TauOption         0x3
  #define NoReachOption     0x4
  struct option LongOptions[] = { 
    {"help"        , no_argument,         NULL, 'h'},
    {"version"     , no_argument,         NULL, VersionOption},
    {"verbose"     , no_argument,         NULL, 'v'},
    {"quiet"       , no_argument,         NULL, 'q'},
    {"debug"       , no_argument,         NULL, 'd'},
    {"in"          , required_argument,   NULL, 'i'},
    {"out"         , required_argument,   NULL, 'o'},
    {"formats"     , no_argument,         NULL, 'f'},
    {"lps"         , required_argument,   NULL, 'l'},
    {"no-state"    , no_argument,         NULL, 'n'},
    {"strong"      , no_argument,         NULL, 's'},
    {"none"        , no_argument,         NULL, NoneOption},
    {"branching"   , no_argument,         NULL, 'b'},
    {"trace"       , no_argument,         NULL, 't'},
    {"weak-trace"  , no_argument,         NULL, 'u'},
    {"tau"         , required_argument,   NULL, TauOption},
    {"add"         , no_argument,         NULL, 'a'},
    {"determinise" , no_argument,         NULL, 'D'},
    {"no-reach"    , no_argument,         NULL, NoReachOption},
    {0, 0, 0, 0}
  };

  bool verbose = false;
  bool quiet = false;
  bool debug = false;
  lts_type intype = lts_none;
  lts_type outtype = lts_none;
  int opt;
  string lpsfile;
  bool print_dot_state = true;
  lts_equivalence equivalence = lts_eq_none;
  lts_eq_options eq_opts; set_eq_options_defaults(eq_opts);
  bool determinise = false;
  bool check_reach = true;

#ifdef ENABLE_SQUADT_CONNECTIVITY
  if (!mcrl2::utilities::squadt::interactor< squadt_interactor >::free_activation(argc, argv)) {
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
        case 'o':
          if ( outtype != lts_none )
          {
            fprintf(stderr,"warning: output format has already been specified; extra option ignored\n");
          } else {
            outtype = lts::parse_format(optarg);
            if ( outtype == lts_none )
            {
              fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
            }
          }
          break;
        case 'f':
          print_formats(stderr);
          return 0;
        case 'l':
          if ( lpsfile != "" )
          {
            fprintf(stderr,"warning: LPS file has already been specified; extra option ignored\n");
          }
          lpsfile = optarg;
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
          equivalence = lts_eq_weak_trace;
          break;
        case TauOption:
          lts_reduce_add_tau_actions(eq_opts,optarg);
          break;
        case 'a':
          eq_opts.reduce.add_class_to_state = true;
          break;
        case 'D':
          determinise = true;
          break;
        case NoReachOption:
          check_reach = false;
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
    if ( quiet && debug )
    {
      gsErrorMsg("options -q/--quiet and -d/--debug cannot be used together\n");
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
    if ( debug )
    {
      gsSetDebugMsg();
    }

    if ( determinise && (equivalence != lts_eq_none) )
    {
      gsErrorMsg("cannot use option -D/--determinise together with LTS reduction options\n");
      return 1;
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
      if ( outtype == lts_none )
      {
          gsVerboseMsg("trying to detect output format by extension...\n");
          outtype = lts::guess_format(outfile);
          if ( outtype == lts_none )
          {
            if ( lpsfile != "" )
            {
              gsWarningMsg("no output format set; using fsm because --lps was used\n");
              outtype = lts_fsm;
            } else {
              gsWarningMsg("no output format set or detected; using default (mcrl2)\n");
              outtype = lts_mcrl2;
            }
          } else if ( (outtype == lts_fsm) && (lpsfile == "") )
          {
            gsWarningMsg("parameter names are unknown (use -l/--lps option)\n");
          }
      }
    } else {
      if ( outtype == lts_none )
      {
        if ( lpsfile != "" )
        {
          gsWarningMsg("no output format set; using fsm because --lps was used\n");
          outtype = lts_fsm;
        } else {
          gsWarningMsg("no output format set; using default (aut)\n");
          outtype = lts_aut;
        }
      }
    }

    lts l;
 
    if (!(use_stdin ? read_lts_from_stdin(l, intype, lpsfile, check_reach) : read_lts_from_file(l, infile, intype, lpsfile, check_reach)))
    {
      return (1);
    }
/*    if ( (lpsfile != "") && !(
               ((outtype == lts_mcrl2) && (l.get_type() != lts_mcrl2)) ||
               ((outtype == lts_mcrl) && (l.get_type() != lts_fsm)) ||
               ((outtype == lts_fsm) && ((l.get_type() == lts_mcrl2) || (l.get_type() == lts_mcrl)))
               ) )
    {
      gsWarningMsg("ignoring --lps option as it is not usable with this input/output combination\n");
      lpsfile = "";
    }*/

 
    if ( equivalence != lts_eq_none )
    {
      gsVerboseMsg("reducing LTS...\n");
      gsVerboseMsg("before reduction: %lu states and %lu transitions \n",l.num_states(),l.num_transitions());
      l.reduce(equivalence,eq_opts);
      gsVerboseMsg("after reduction: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
    }

    if ( determinise )
    {
      gsVerboseMsg("determinising LTS...\n");
      gsVerboseMsg("before determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
      l.determinise();
      gsVerboseMsg("after determinisation: %lu states and %lu transitions\n",l.num_states(),l.num_transitions());
    }
 
    if ( use_stdout )
    {
      if (!write_lts_to_stdout(l, outtype, lpsfile, print_dot_state))
      {
        return (1);
      }
    } else {
      if (!write_lts_to_file(l, outfile, outtype, lpsfile, print_dot_state))
      {
        return (1);
      }
    }
#ifdef ENABLE_SQUADT_CONNECTIVITY
  }
#endif

  return 0;
}
