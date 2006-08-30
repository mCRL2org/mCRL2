#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "liblts.h"
#include "liblts_fsm.h"
#include "liblts_dot.h"
#include "setup.h"

#ifdef MCRL2_BCG
#include "bcg_user.h"
#endif

#define NAME "ltscompare"
#define VERSION "0.1"
#include "mcrl2_revision.h"

using namespace std;
using namespace mcrl2::lts;

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
    "                        parameter names of states when saving in fsm format\n"
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

int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

#ifdef MCRL2_BCG
  BCG_INIT();
#endif

  #define ShortOptions      "hqvi:j:fsbtu"
  #define VersionOption     0x1
  #define TauOption         0x2
  struct option LongOptions[] = { 
    {"help"      , no_argument,         NULL, 'h'},
    {"version"   , no_argument,         NULL, VersionOption},
    {"quiet"     , no_argument,         NULL, 'q'},
    {"verbose"   , no_argument,         NULL, 'v'},
    {"in1"       , required_argument,   NULL, 'i'},
    {"in2"       , required_argument,   NULL, 'j'},
    {"formats"   , no_argument,         NULL, 'f'},
    {"strong"    , no_argument,         NULL, 's'},
    {"branching" , no_argument,         NULL, 'b'},
    {"trace"     , no_argument,         NULL, 't'},
    {"obs-trace" , no_argument,         NULL, 'u'},
    {"tau"       , required_argument,   NULL, TauOption},
    {0, 0, 0, 0}
  };

  bool verbose = false;
  bool quiet = false;
  lts_type in1type = lts_none;
  lts_type in2type = lts_none;
  int opt;
  lts_equivalence equivalence = lts_eq_strong;
  lts_eq_options eq_opts; set_eq_options_defaults(eq_opts);
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
        if ( in1type != lts_none )
        {
          fprintf(stderr,"warning: first input format has already been specified; extra option ignored\n");
        } else {
          in1type = lts::parse_format(optarg);
          if ( in1type == lts_none )
          {
            fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
          }
        }
        break;
      case 'j':
        if ( in2type != lts_none )
        {
          fprintf(stderr,"warning: sceond input format has already been specified; extra option ignored\n");
        } else {
          in2type = lts::parse_format(optarg);
          if ( in2type == lts_none )
          {
            fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
          }
        }
        break;
      case 'f':
        print_formats(stderr);
        return 0;
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

  if ( optind >= argc )
  {
    gsErrorMsg("%s needs at least one file as argument; see --help\n",NAME);
    return 1;
  }
  bool use_stdin = (optind+1 >= argc);

  string in1file;
  string in2file;
  if ( !use_stdin )
  {
    in1file = argv[optind];
    in2file = argv[optind+1];
  } else {
    in2file = argv[optind];
  }

  lts l1,l2;

  if ( use_stdin )
  {
    gsVerboseMsg("reading first LTS from stdin...\n");
    if ( !l1.read_from(cin,in1type) )
    {
      gsErrorMsg("cannot read LTS from stdin\n");
      gsErrorMsg("use -v/--verbose for more information\n");
      return 1;
    }
  } else {
    gsVerboseMsg("reading first LTS from '%s'...\n",in1file.c_str());
    if ( !l1.read_from(in1file,in1type) )
    {
      bool b = true;
      if ( in1type == lts_none ) // XXX really do this?
      {
        gsVerboseMsg("reading failed; trying to force format by extension...\n");
        in1type = lts::guess_format(in1file);
        if ( (in1type != lts_none) && l1.read_from(in1file,in1type) )
        {
          b = false;
        }
      }
      if ( b )
      {
        gsErrorMsg("cannot read LTS from file '%s'\n",in1file.c_str());
        gsErrorMsg("use -v/--verbose for more information\n");
        return 1;
      }
    }
  }
  gsVerboseMsg("reading second LTS from '%s'...\n",in2file.c_str());
  if ( !l2.read_from(in2file,in2type) )
  {
    bool b = true;
    if ( in2type == lts_none ) // XXX really do this?
    {
      gsVerboseMsg("reading failed; trying to force format by extension...\n");
      in2type = lts::guess_format(in2file);
      if ( (in2type != lts_none) && l2.read_from(in2file,in2type) )
      {
        b = false;
      }
    }
    if ( b )
    {
      gsErrorMsg("cannot read LTS from file '%s'\n",in2file.c_str());
      gsErrorMsg("use -v/--verbose for more information\n");
      return 1;
    }
  }

  gsVerboseMsg("comparing LTSs...\n");
  if ( l1.compare(l2,equivalence,eq_opts) )
  {
    gsVerboseMsg("LTSs are equivalent\n");
    return 0;
  } else {
    gsVerboseMsg("LTSs are not equivalent\n");
    return 2;
  }
}
