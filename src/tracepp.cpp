#define NAME "tracepp"
#define VERSION "0.1"

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <assert.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libprint.h"
#include "libtrace.h"

#include "mcrl2_revision.h"

using namespace std;

enum output_type { otPlain, otMcrl2, otDot, otAut, /*otSvc,*/ otNone };

static void print_state(ostream &os, ATermAppl state)
{
  int arity = ATgetArity(ATgetAFun(state));

  os << "(";
  for (int i=0; i<arity; i++)
  {
    if ( i > 0 )
    {
      os << ",";
    }
    PrintPart_CXX(os,ATgetArgument(state,i),ppDefault);
  }
  os << ")";
}

static void trace2dot(ostream &os, Trace &trace, char *name)
{
  os << "digraph \"" << name << "\" {" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  ATermAppl act;
  int i = 0;
  while ( (act = trace.getAction()) != NULL )
  {
    os << i << " [label=\"";
    if ( trace.getState() != NULL )
    {
      print_state(os,trace.getState());
    }
    if ( i == 0 )
    {
      os << "\",peripheries=2];" << endl;
    } else {
      os << "\"];" << endl;
    }
    os << i << " -> " << i+1 << " [label=\"";
    if ( gsIsMultAct(act) )
    {
      PrintPart_CXX(os,(ATerm) act,ppDefault);
    } else {
      // needed because trace library cannot parse strings
      os << ATgetName(ATgetAFun(act));
    }
    os << "\"];" << endl;
    i++;
  }
  os << i << " [label=\"";
  if ( trace.getState() != NULL )
  {
    print_state(os,trace.getState());
  }
  os << "\"];" << endl;
  os << "}" << endl;
}

static void trace2aut(ostream &os, Trace &trace)
{
  os << "des (0," << trace.getLength() << "," << trace.getLength()+1 << ")" << endl;
  ATermAppl act;
  int i = 0;
  while ( (act = trace.getAction()) != NULL )
  {
    os << "(" << i << ",\"";
    if ( gsIsMultAct(act) )
    {
      PrintPart_CXX(os,(ATerm) act,ppDefault);
    } else {
      // needed because trace library cannot parse strings
      os << ATgetName(ATgetAFun(act));
    }
    i++;
    os << "\"," << i << ")" << endl;
  }
}

/*static void trace2svc(ostream &os, Trace &trace)
{
  // SVC library does not accept ostreams
}*/

static void print_help(FILE *f, char *Name)
{
  fprintf(f,
    "Usage: %s [OPTION]... [INFILE [OUTFILE]]\n"
    "Convert the trace in INFILE and save it in another format to OUTFILE. If OUTFILE\n"
    "is not present, stdout is used. If INFILE is not present, stdin is used.\n"
    "\n"
    "  -h, --help            display this help message\n"
    "      --version         display version information\n"
    "  -q, --quiet           do not print any unrequested information\n"
    "  -v, --verbose         print information about the rewriting process\n"
    "\n"
    "The output formats that can be chosen are the following. By default --plain is\n"
    "assumed.\n"
    "\n"
    "  -p, --plain           write trace in plain format (default)\n"
    "  -m, --mcrl2           write trace in mCRL2 trace format\n"
    "  -d, --dot             write trace in dot format\n"
    "  -a, --aut             write trace in aut format\n"
/*    "  -s, --svc             write trace in svc format\n"*/,
    Name
  );
}

static void print_version(FILE *f)
{
  fprintf(f, NAME " " VERSION " (revision %i)\n", REVISION);
}

int main(int argc, char **argv)
{
  #define sopts "hqvpmda"
//  #define sopts "hqvpmdas"
  struct option lopts[] = {
    { "help",     no_argument,  NULL,  'h' },
    { "version",  no_argument,  NULL,  0x1 },
    { "quiet",    no_argument,  NULL,  'q' },
    { "verbose",  no_argument,  NULL,  'v' },
    { "plain",    no_argument,  NULL,  'p' },
    { "mcrl2",    no_argument,  NULL,  'm' },
    { "dot",      no_argument,  NULL,  'd' },
    { "aut",      no_argument,  NULL,  'a' },
//    { "svc",      no_argument,  NULL,  's' },
    { 0, 0, 0, 0 }
  };

  ATerm stackbot;
  ATinit(argc,argv,&stackbot);
  gsEnableConstructorFunctions();

  bool quiet = false;
  bool verbose = false;
  output_type outtype = otNone;
  int opt;
  while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
  {
    switch ( opt )
    {
      case 'h':
        print_help(stderr, argv[0]);
        return 0;
      case 0x1:
        print_version(stderr);
        return 0;
      case 'q':
        quiet = true;
        break;
      case 'v':
        verbose = true;
        break;
      case 'p':
        if ( outtype != otNone )
        {
          gsErrorMsg("cannot set multiple output formats\n");
          return 1;
        }
        outtype = otPlain;
        break;
      case 'm':
        if ( outtype != otNone )
        {
          gsErrorMsg("cannot set multiple output formats\n");
          return 1;
        }
        outtype = otMcrl2;
        break;
      case 'd':
        if ( outtype != otNone )
        {
          gsErrorMsg("cannot set multiple output formats\n");
          return 1;
        }
        outtype = otDot;
        break;
      case 'a':
        if ( outtype != otNone )
        {
          gsErrorMsg("cannot set multiple output formats\n");
          return 1;
        }
        outtype = otAut;
        break;
/*      case 's':
        if ( outtype != otNone )
        {
          gsErrorMsg("cannot set multiple output formats\n");
          return 1;
        }
        outtype = otSvc;
        break;*/
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
    gsSetQuietMsg();
  if ( verbose )
    gsSetVerboseMsg();

  if ( outtype == otNone )
    outtype = otPlain;

  istream *InStream = &cin;
  char *InFileName = NULL;
  if ( optind < argc )
  {
    InFileName = argv[optind];
    InStream = new ifstream(InFileName,ifstream::binary|ifstream::in);
    if ( !InStream->good() )
    {
      gsErrorMsg("could not open input file '%s' for reading\n", InFileName);
      delete InStream;
      return 1;
    }
  }

  ostream *OutStream = &cout;
  char *OutFileName = NULL;
  if ( optind+1 < argc )
  {
    OutFileName = argv[optind+1];
    OutStream = new ofstream(OutFileName,ofstream::binary|ofstream::out|ofstream::trunc);
    if ( !OutStream->good() )
    {
      gsErrorMsg("could not open output file '%s' for writing\n", OutFileName);
      delete OutStream;
      return 1;
    }
  }


  if ( InStream == &cin )
    gsVerboseMsg("reading input from stdin...\n");
  else
    gsVerboseMsg("reading input from '%s'...\n", InFileName);

  Trace trace(*InStream);

  if ( InStream != &cin )
  {
    ((ifstream *) InStream)->close();
    delete InStream;
  }

  if ( OutStream == &cout )
    gsVerboseMsg("writing result to stdout...\n");
  else
    gsVerboseMsg("writing result to '%s'...\n", OutFileName);

  switch ( outtype )
  {
    case otPlain:
      gsVerboseMsg("writing result in plain text...\n");
      trace.save(*OutStream,tfPlain);
      break;
    case otMcrl2:
      gsVerboseMsg("writing result in mCRL2 trace format...\n");
      trace.save(*OutStream,tfMcrl2);
      break;
    case otDot:
      gsVerboseMsg("writing result in dot format...\n");
      if ( InFileName == NULL )
      {
        trace2dot(*OutStream,trace,"stdin");
      } else {
        char *s = strrchr(InFileName,'.');
        if ( s != NULL )
        {
          *s = 0;
        }
        trace2dot(*OutStream,trace,InFileName);
      }
      break;
    case otAut:
      gsVerboseMsg("writing result in aut format...\n");
      trace2aut(*OutStream,trace);
      break;
/*      gsVerboseMsg("writing result in svc format...\n");
      case otSvc:
      trace2svc(*OutStream,trace);
      break;*/
    default:
      break;
  }

  if ( OutStream != &cout )
  {
    ((ofstream *) OutStream)->close();
    delete OutStream;
  }
}
