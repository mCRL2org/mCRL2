#ifdef __cplusplus
extern "C" {
#endif

#define NAME      "alpha"
#define LVERSION  "0.1"
#define AUTHOR    "Yaroslav S. Usenko and Muck van Weerdenburg"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libprint_types.h"
#include "libprint_c.h"
#include "libgsalpha.h"

extern ATermAppl gsParse(FILE *);
extern ATermAppl gsTypeCheck(ATermAppl);

void PrintUsage(FILE *f, char *Name)
{
	fprintf(f,"Usage: %s OPTIONS [SPECFILE [OUTFILE]]\n",Name);
        fprintf(f,"Apply alphabet axioms to processes in SPECFILE and save the result to OUTFILE.\n"
                  "If OUTFILE is not present, stdout is used. If SPECFILE is not present, stdin is\n"
                  "used. To use stdin and save the output into a file, use '-' for SPECFILE.\n"
                  "\n"
                  "The OPTIONS that can be used are:\n"
		  "-a, --read-aterm         SPECFILE is an ATerm\n"
		  "-w, --write-aterm        OUTFILE should be an ATerm\n"
		  "-h, --help             display this help\n"
		  "    --version          display version information\n"
		  "-q, --quiet            do not display warning messages\n"
		  "-v, --verbose          turn on the display of short intermediate messages\n"
		  "-d, --debug            turn on the display of detailed intermediate messages\n"
	       );
}

void PrintMoreInfo(FILE *Stream, char *Name) {
  fprintf(Stream, "Try \'%s --help\' for more information.\n", Name);
}

void PrintVersion(FILE *Stream) {
  fprintf(Stream, "%s %s\nWritten by %s.\n",
    NAME, LVERSION, AUTHOR);
}


int main(int argc, char **argv)
{
	FILE *SpecStream, *OutStream;
	ATerm stackbot;
	ATermAppl Spec;
        #define sopts      "aw:hqvd"
        #define VersionOption     CHAR_MAX + 1
	struct option lopts[] = {
	  { "read-aterm" , no_argument,	      NULL, 'a' },
	  { "write-aterm", no_argument,	      NULL, 'w' },
	  { "help"       , no_argument,       NULL, 'h'},
	  { "version"    , no_argument,       NULL, VersionOption},
	  { "quiet"      , no_argument,       NULL, 'q'},
	  { "verbose"    , no_argument,       NULL, 'v'},
	  { "debug"      , no_argument,       NULL, 'd'},
	  { 0, 0, 0, 0 }
	};
	int opt,read_aterm,write_aterm;

	ATinit(argc,argv,&stackbot);

	read_aterm = 0;
	write_aterm = 0;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	  {
	    switch ( opt )
	      {
	      case 'a':
		read_aterm = 1;
		break;
	      case 'w':
		write_aterm = 1;
		break;
	      case 'h':
		PrintUsage(stdout, argv[0]);
		return 0;
	      case VersionOption:
		PrintVersion(stdout);
		return 0;
	      case 'q':
		gsSetQuietMsg();
		break;
	      case 'v':
		gsSetVerboseMsg();
		break;
	      case 'd':
		gsSetDebugMsg();
		break;
	      default:
		break;
	      }
	  }

	SpecStream = stdin;
	if ( optind < argc && strcmp(argv[optind],"-") )
	{
		if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	OutStream = stdout;
	if ( optind+1 < argc )
	{
		if ( (OutStream = fopen(argv[optind+1],"w")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	}

	if ( read_aterm )
	{
		gsEnableConstructorFunctions();
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		gsEnableConstructorFunctions();
		Spec = gsParse(SpecStream);
		Spec = gsTypeCheck(Spec);
		fclose(SpecStream);
	}
	if ( Spec == NULL )
	{
		return 1;
	}

	Spec = Alpha(Spec);

	if ( write_aterm )
	{
		ATwriteToTextFile((ATerm) Spec,OutStream);
	} else {
		PrintPart_C(OutStream, (ATerm) Spec, ppAdvanced);
	}
	if ( OutStream != stdout )
	{
		fclose(OutStream);
	}
}

#ifdef __cplusplus
}
#endif
