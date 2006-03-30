#include <string>
#include <getopt.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "liblts.h"
#include "setup.h"

#ifdef MCRL2_BCG
#include <bcg_user.h>
#endif

#define NAME "ltsinfo"
#define VERSION "0.1"
#include "mcrl2_revision.h"

using namespace std;
using namespace mcrl2::lts;

static string string_from_type(lts_type type)
{
  switch ( type )
  {
    case lts_aut:
      return "AUT";
    case lts_mcrl:
      return "mCRL";
    case lts_mcrl2:
      return "mCRL2";
    case lts_svc:
      return "SVC";
#ifdef MCRL2_BCG
    case lts_bcg:
      return "BCG";
#endif
    default:
      return "unknown";
  }
}

static lts_type get_extension(string &s)
{
  string::size_type pos = s.find_last_of('.');
  
  if ( pos != string::npos )
  {
    string ext = s.substr(pos+1);

    if ( ext == "aut" )
    {
      gsVerboseMsg("detected AUT extension\n");
      return lts_aut;
    } else if ( ext == "svc" )
    {
      gsVerboseMsg("detected SVC extension; assuming mCRL2 format\n");
      return lts_mcrl2;
#ifdef MCRL2_BCG
    } else if ( ext == "bcg" )
    {
      gsVerboseMsg("detected BCG extension\n");
      return lts_bcg;
#endif
    }
  }

  return lts_none;
}

static lts_type get_format(char *s)
{
  if ( !strcmp(s,"aut") )
  {
    return lts_aut;
  } else if ( !strcmp(s,"mcrl") )
  {
    return lts_mcrl;
  } else if ( !strcmp(s,"mcrl2") )
  {
    return lts_mcrl2;
#ifdef MCRL2_BCG
  } else if ( !strcmp(s,"bcg") )
  {
    return lts_bcg;
#endif
  }

  return lts_none;
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

int main(int argc, char **argv)
{
  ATerm bot;
  ATinit(argc,argv,&bot);
  gsEnableConstructorFunctions();

#ifdef MCRL2_BCG
  BCG_INIT();
#endif

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
          intype = get_format(optarg);
          if ( intype == lts_none )
          {
            fprintf(stderr,"warning: format '%s' is not recognised; option ignored\n",optarg);
          }
        }
        break;
      case 'f':
        print_formats(stderr);
        return 0;
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

  string infile;
  if ( !use_stdin )
  {
    infile = argv[optind];
  }

  lts l;

  if ( use_stdin )
  {
    gsVerboseMsg("reading LTS from stdin...\n");
    if ( !l.read_from(cin,intype) )
    {
      gsErrorMsg("cannot read LTS from stdin\n");
      return 1;
    }
  } else {
    gsVerboseMsg("reading LTS from '%s'...\n",infile.c_str());
    if ( !l.read_from(infile,intype) )
    {
      bool b = true;
      if ( intype == lts_none ) // XXX really do this?
      {
        gsVerboseMsg("reading failed; trying to force format by extension...\n");
        intype = get_extension(infile);
        if ( (intype != lts_none) && l.read_from(infile,intype) )
        {
          b = false;
        }
      }
      if ( b )
      {
        gsErrorMsg("cannot read LTS from file '%s'\n",infile.c_str());
        return 1;
      }
    }
  }

  cout << "LTS format: " << string_from_type(l.get_type()) << endl
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

  return 0;
}
