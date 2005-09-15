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
#include "libgsparse.h"
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

void print_help(FILE *f)
{
	fprintf(f,"Usage: %s OPTIONS [SPECFILE [OUTFILE]]\n",NAME);
	fprintf(f,"Rewrite data expressions in SPECFILE and save the result to\n"
	          "OUTFILE. If OUTFILE is not present, stdout is used. If\n"
	          "SPECFILE is not present, stdin is used. To use stdin and\n"
	          "save the output into a file, use '-' for SPECFILE.\n"
	          "\n"
	          "The OPTIONS that can be used are:\n"
	          "-h, --help               display this help message\n"
	          "-q, --quiet              do not print any unrequested\n"
		  "                         information\n"
	          "-b, --benchmark [num]    rewrites specification num times\n"
	          "                         (default is 1000 times)\n"
	          "-a, --read-aterm         SPECFILE is an ATerm\n"
	          "-w, --write-aterm        OUTFILE should be an ATerm\n"
	          "-i, --inner              Use innermost rewriter (default)\n"
	          "-2, --inner2             Use another innermost rewriter\n"
	          "-3, --inner3             Use yet another innermost rewriter\n"
	          "-c, --innerc             Use compiling innermost rewriter\n"
	          "-j, --jitty              Use compiling innermost rewriter\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream, *OutStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "hqbawi23cj"
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	'h' },
		{ "quiet",		no_argument,	NULL,	'q' },
		{ "benchmark",		no_argument,	NULL,	'b' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "write-aterm",	no_argument,	NULL,	'w' },
		{ "inner",		no_argument,	NULL,	'i' },
		{ "inner2",		no_argument,	NULL,	'2' },
		{ "inner3",		no_argument,	NULL,	'3' },
		{ "innerc",		no_argument,	NULL,	'c' },
		{ "jitty",		no_argument,	NULL,	'j' },
		{ 0, 0, 0, 0 }
	};
	int opt,read_aterm,write_aterm,strat,benchmark,i,bench_times;
	bool quiet;

	ATinit(argc,argv,&stackbot);

	quiet = false;
	benchmark = 0;
	bench_times = 1000;
	read_aterm = 0;
	write_aterm = 0;
	strat = GS_REWR_INNER3;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr);
				return 0;
			case 'q':
				quiet = true;
				break;
			case 'b':
				benchmark = 1;
				// XXX optional argument hack
				// optional_argument doesn't seem to work
				if ( argv[optind] != NULL && (argv[optind][0] >= '0') && (argv[optind][0] <= '9') )
				{
					bench_times = strtoul(argv[optind],NULL,0);
					optind++;
				}
				break;
			case 'a':
				read_aterm = 1;
				break;
			case 'w':
				write_aterm = 1;
				break;
			case 'i':
				strat = GS_REWR_INNER;
				break;
			case '2':
				strat = GS_REWR_INNER2;
				break;
			case '3':
				strat = GS_REWR_INNER3;
				break;
			case 'c':
				strat = GS_REWR_INNERC;
				break;
			case 'j':
				strat = GS_REWR_JITTY;
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
			if ( !quiet )
			{
				perror(NAME);
			}
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
				if ( !quiet )
				{
					perror(NAME);
				}
				return 1;
			}
		}
	}

	if ( read_aterm )
	{
		gsEnableConstructorFunctions();
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}
	if ( Spec == NULL )
	{
		return 1;
	}

	gsRewriteInit(ATAgetArgument(Spec,3),strat);


	if ( benchmark )
	{
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
		if ( gsIsLPESpec(Spec) )
		{
			Spec = rewrite_lpe(Spec);
		} else {
			Spec = rewrite_nolpe(Spec);
		}

		if ( write_aterm )
		{
			ATwriteToTextFile((ATerm) Spec,OutStream);
		} else {
			gsPrintSpecification(OutStream,Spec);
		}
		if ( OutStream != stdout )
		{
			fclose(OutStream);
		}
	}
}

#ifdef __cplusplus
}
#endif
