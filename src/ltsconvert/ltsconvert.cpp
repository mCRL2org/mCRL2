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

#define NAME "ltsconvert"
#define VERSION "0.1"
#include "mcrl2_revision.h"

using namespace std;
using namespace mcrl2::lts;

enum alt_lts_type { alt_lts_none, alt_lts_fsm, alt_lts_dot };

static ATerm get_lpe(string &filename)
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

static string get_base(string &s)
{
  string::size_type pos = s.find_last_of('.');
  
  if ( pos == string::npos )
  {
    return s;
  } else {
    return s.substr(0,pos);
  }
}

static alt_lts_type get_alt_extension(string &s)
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

static alt_lts_type get_alt_format(char *s)
{
  if ( !strcmp(s,"fsm") )
  {
    return alt_lts_fsm;
  } else if ( !strcmp(s,"dot") )
  {
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
    "requested format. If OUTFILE is not supplied, stdout is used. If INFILE is not\n"
    "supplied, stdin is used.\n"
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
    "                        generated; this is needed to store the correct parameter\n"
    "                        names of states when saving in fsm format\n"
    "  -n, --no-state        leave out state information when saving in dot format\n",
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

  #define ShortOptions      "hqvi:o:fl:nsbtu"
  #define VersionOption     0x1
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
    {"branching" , no_argument,         NULL, 'b'},
    {"trace"     , no_argument,         NULL, 't'},
    {"obs-trace" , no_argument,         NULL, 'u'},
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
  lts_reduction reduction = lts_red_none;
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
      case 's':
	reduction = lts_red_strong;
	break;
      case 'b':
	reduction = lts_red_branch;
	break;
      case 't':
	reduction = lts_red_trace;
	break;
      case 'u':
	reduction = lts_red_obs_trace;
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
      if ( lpefile != "" )
      {
        gsVerboseMsg("no output format set; using fsm because --lpe was used\n");
        alt_outtype = alt_lts_fsm;
        use_alt_outtype = true;
      } else {
        gsVerboseMsg("trying to detect output format by extension...\n");
        outtype = lts::guess_format(outfile);
        if ( outtype == lts_none )
        {
          alt_outtype = get_alt_extension(outfile);
          if ( alt_outtype == alt_lts_none )
          {
            gsVerboseMsg("no output format set or detected; using default (mcrl2)\n");
            outtype = lts_mcrl2;
          } else {
            use_alt_outtype = true;
            if ( alt_outtype == alt_lts_fsm )
            {
              gsWarningMsg("parameter names are unknown (use -l/--lpe option)\n");
            }
          }
        }
      }
    }
  } else {
      if ( lpefile != "" )
      {
        gsVerboseMsg("no output format set; using fsm because --lpe was used\n");
        alt_outtype = alt_lts_fsm;
        use_alt_outtype = true;
      } else {
        gsVerboseMsg("no output format set; using default (aut)\n");
        outtype = lts_aut;
      }
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
        intype = lts::guess_format(infile);
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

  if ( reduction != lts_red_none )
  {
    gsVerboseMsg("reducing LTS...\n");
    l.reduce(reduction);
  }

  if ( use_stdout )
  {
    gsVerboseMsg("writing LTS to stdout...\n");
    if ( use_alt_outtype )
    {
      switch ( alt_outtype )
      {
        case alt_lts_fsm:
          if ( !write_lts_to_fsm(l,cout,get_lpe(lpefile)) )
          {
            gsErrorMsg("cannot write LTS to stdout\n");
            return 1;
          }
          break;
        case alt_lts_dot:
          {
            string str_stdout = "stdout";
            if ( !write_lts_to_dot(l,cout,str_stdout,print_dot_state) )
            {
              gsErrorMsg("cannot write LTS to stdout\n");
              return 1;
            }
            break;
          }
        default:
          assert(0);
          gsErrorMsg("do not know how to handle output format\n");
          return 1;
      }
    } else {
      if ( !l.write_to(cout,outtype) )
      {
        gsErrorMsg("cannot write LTS to stdout\n");
        return 1;
      }
    }
  } else {
    gsVerboseMsg("writing LTS to '%s'...\n",outfile.c_str());
    if ( use_alt_outtype )
    {
      switch ( alt_outtype )
      {
        case alt_lts_fsm:
          if ( !write_lts_to_fsm(l,outfile,get_lpe(lpefile)) )
          {
            gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
            return 1;
          }
          break;
        case alt_lts_dot:
          {
            string str_base = get_base(outfile);
            if ( !write_lts_to_dot(l,outfile,str_base,print_dot_state) )
            {
              gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
              return 1;
            }
            break;
          }
        default:
          assert(0);
          gsErrorMsg("do not know how to handle output format\n");
          return 1;
      }
    } else {
      if ( !l.write_to(outfile,outtype) )
      {
        gsErrorMsg("cannot write LTS to file '%s'\n",outfile.c_str());
        return 1;
      }
    }
  }

  return 0;
}
