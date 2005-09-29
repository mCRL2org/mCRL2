#ifdef __cplusplus
extern "C" {
#endif

#define NAME "lperewr"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gslexer.h"
#include "gstypecheck.h"
#include "gsdataimpl.h"
#include "libprint_c.h"
#include "libgsrewrite.h"

static bool gsIsLPESpec(ATermAppl Spec)
{
	return gsIsLPE(ATAgetArgument(Spec,5));
}

static ATermAppl rewrite_proc(ATermAppl p)
{
	ATermList l,m;

	if ( gsIsAction(p) || gsIsProcess(p) )
	{
		return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) gsRewriteTerms(ATLgetArgument(p,1)));
	} else if ( gsIsAtTime(p) )
	{
		return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) gsRewriteTerm(ATAgetArgument(p,1)));
	} else if ( gsIsSum(p) || gsIsAllow(p) || gsIsRestrict(p) || gsIsHide(p) || gsIsRename(p) || gsIsComm(p) )
	{
		return ATmakeAppl2(ATgetAFun(p),ATgetArgument(p,0),(ATerm) rewrite_proc(ATAgetArgument(p,1)));
	} else if ( gsIsCond(p) )
	{
		return gsMakeCond(gsRewriteTerm(ATAgetArgument(p,0)),rewrite_proc(ATAgetArgument(p,1)),rewrite_proc(ATAgetArgument(p,2)));
	} else {
		l = ATgetArguments(p);
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,(ATerm) rewrite_proc(ATAgetFirst(l)));
		}
		return ATmakeApplList(ATgetAFun(p),ATreverse(m));
	}
}

static ATermAppl rewrite_nolpe(ATermAppl Spec)
{
	ATermAppl a;
	ATermList l,m;

	l = ATLgetArgument(ATAgetArgument(Spec,5),0);
	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = ATAgetArgument(ATAgetFirst(l),3);
		a = rewrite_proc(a);
		m = ATinsert(m,(ATerm) ATsetArgument(ATAgetFirst(l),(ATerm) a,3));
	}
	m = ATreverse(m);
	Spec = ATsetArgument(Spec,(ATerm) gsMakeProcEqnSpec(m),5);

	a = ATAgetArgument(ATAgetArgument(Spec,6),1);
	a = rewrite_proc(a);
	Spec = ATsetArgument(Spec,(ATerm) gsMakeInit(ATLgetArgument(ATAgetArgument(Spec,6),0),a),6);

	return Spec;
}

static ATermAppl rewrite_lpe(ATermAppl Spec)
{
	ATermAppl a,b,c;
	ATermList l,m,n,o,d;

	l = ATLgetArgument(ATAgetArgument(Spec,5),2);
	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = ATAgetArgument(ATAgetFirst(l),1);
		a = gsRewriteTerm(a);
		b = ATAgetArgument(ATAgetFirst(l),2);
		if ( gsIsMultAct(b) )
		{
			n = ATLgetArgument(b,0);
			o = ATmakeList0();
			for (; !ATisEmpty(n); n=ATgetNext(n))
			{
				o = ATinsert(o,(ATerm) rewrite_proc(ATAgetFirst(n)));
			}
			b = gsMakeMultAct(ATreverse(o));
		}
		c = ATAgetArgument(ATAgetFirst(l),3);
		if ( !gsIsNil(c) )
		{
			c = rewrite_proc(c);
		}
		d = ATLgetArgument(ATAgetFirst(l),4);
		n = ATmakeList0();
		for (; !ATisEmpty(d); d=ATgetNext(d))
		{
			n = ATinsert(n,(ATerm) gsMakeAssignment(ATAgetArgument(ATAgetFirst(d),0),gsRewriteTerm(ATAgetArgument(ATAgetFirst(d),1))));
		}
		d = ATreverse(n);
		m = ATinsert(m,(ATerm) gsMakeLPESummand(ATLgetArgument(ATAgetFirst(l),0),a,b,c,d));
	}
	m = ATreverse(m);
	Spec = ATsetArgument(Spec,(ATerm) ATsetArgument(ATAgetArgument(Spec,5),(ATerm) m,2),5);

	l = ATLgetArgument(ATAgetArgument(Spec,6),1);
	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATinsert(m,(ATerm) gsMakeAssignment(ATAgetArgument(ATAgetFirst(l),0),gsRewriteTerm(ATAgetArgument(ATAgetFirst(l),1))));
	}
	Spec = ATsetArgument(Spec,(ATerm) gsMakeLPEInit(ATLgetArgument(ATAgetArgument(Spec,6),0),ATreverse(m)),6);

	return Spec;
}

static void print_help(FILE *f, char *Name)
{
	fprintf(f,"Usage: %s OPTIONS [INFILE [OUTFILE]]\n",Name);
	fprintf(f,"Rewrite data expressions in INFILE and save the result to\n"
	          "OUTFILE. If OUTFILE is not present, stdout is used. If\n"
	          "INFILE is not present, stdin is used. To use stdin and\n"
	          "save the output into a file, use '-' for INFILE.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               display this help message\n"
	          "-q, --quiet              do not print any unrequested\n"
		  "                         information\n"
	          "-v, --verbose            print information about the\n"
		  "                         rewriting process\n"
	          "-b, --benchmark [num]    rewrites specification num times\n"
	          "                         (default is 1000 times)\n"
	          "-R, --rewriter name      use rewriter 'name' (default inner3)\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *InStream, *OutStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "hqvb::R:"
	struct option lopts[] = {
		{ "help",		no_argument,		NULL,	'h' },
		{ "quiet",		no_argument,		NULL,	'q' },
		{ "verbose",		no_argument,		NULL,	'b' },
		{ "benchmark",		optional_argument,	NULL,	'b' },
		{ "rewriter",		required_argument,	NULL,	'R' },
		{ 0, 0, 0, 0 }
	};
	int opt,i,bench_times;
	RewriteStrategy strat;
	bool quiet,verbose,benchmark;

	ATinit(argc,argv,&stackbot);

	quiet = false;
	verbose = false;
	benchmark = false;
	bench_times = 1000;
	strat = GS_REWR_INNER3;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr, argv[0]);
				return 0;
			case 'q':
				quiet = true;
				break;
			case 'v':
				verbose = true;
				break;
			case 'b':
				benchmark = true;
				if ( optarg != NULL && (optarg[0] >= '0') && (optarg[0] <= '9') )
				{
					bench_times = strtoul(optarg,NULL,0);
				}
				break;
			case 'R':
				strat = RewriteStrategyFromString(optarg);
				if ( strat == GS_REWR_INVALID )
				{
					gsErrorMsg("invalid rewrite strategy '%s'\n",optarg);
					return 1;
				}
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
		gsSetQuietMsg();
	if ( verbose )
		gsSetVerboseMsg();

	InStream = stdin;
	if ( optind < argc && strcmp(argv[optind],"-") )
	{
		if ( (InStream = fopen(argv[optind],"r")) == NULL )
		{
			gsErrorMsg("cannot open '%s' for reading\n",argv[optind]);
			return 1;
		}
	}

	OutStream = stdout;
	if ( !benchmark )
	{
		if ( optind+1 < argc )
		{
			if ( (OutStream = fopen(argv[optind+1],"w")) == NULL )
			{
				gsErrorMsg("cannot open '%s' for writing\n",argv[optind+1]);
				return 1;
			}
		}
	}

	gsEnableConstructorFunctions();

	if ( InStream == stdin )
		gsVerboseMsg("reading input from stdin...\n");
	else
		gsVerboseMsg("reading input from '%s'...\n",argv[optind]);
	Spec = (ATermAppl) ATreadFromFile(InStream);
	if ( Spec == NULL )
	{
		gsErrorMsg("input is not valid\n");
		return 1;
	}

	gsVerboseMsg("initialising rewriter...\n");
	gsRewriteInit(ATAgetArgument(Spec,3),strat);

	if ( benchmark )
	{
		gsVerboseMsg("benchmarking %i times...\n",bench_times);
		if ( gsIsLPESpec(Spec) )
		{
			for(i=0; i<bench_times; i++)
			{
				rewrite_lpe(Spec);
			}
		} else {
			for(i=0; i<bench_times; i++)
			{
				rewrite_nolpe(Spec);
			}
		}
	} else {
		gsVerboseMsg("rewriting input...\n",bench_times);
		if ( gsIsLPESpec(Spec) )
		{
			Spec = rewrite_lpe(Spec);
		} else {
			Spec = rewrite_nolpe(Spec);
		}

		if ( OutStream == stdout )
			gsVerboseMsg("writing result to stdout...\n");
		else
			gsVerboseMsg("writing result to '%s'...\n",argv[optind+1]);
		ATwriteToBinaryFile((ATerm) Spec,OutStream);
		if ( OutStream != stdout )
		{
			fclose(OutStream);
		}
	}
}

#ifdef __cplusplus
}
#endif
