/* $Id: rewr.c,v 1.2 2005/03/09 15:46:00 muck Exp $ */

#define NAME "rewr"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "aterm2.h"
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
	} else if ( gsIsSum(p) )
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
		a = rewrite_proc(a);
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
	          "-r, --read-aterm         SPECFILE is an ATerm\n"
	          "-w, --write-aterm        OUTFILE should be an ATerm\n"
	          "-i, --inner              Use innermost rewriter (default)\n"
	       );
}

int main(int argc, char **argv)
{
	FILE *SpecStream, *OutStream;
	ATerm stackbot;
	ATermAppl Spec;
	#define sopts "hawi"
	struct option lopts[] = {
		{ "help",		no_argument,	NULL,	'h' },
		{ "read-aterm",		no_argument,	NULL,	'a' },
		{ "write-aterm",	no_argument,	NULL,	'w' },
		{ "inner",		no_argument,	NULL,	'i' },
		{ 0, 0, 0, 0 }
	};
	int opt,read_aterm,write_aterm,strat;

	ATinit(argc,argv,&stackbot);

	read_aterm = 0;
	write_aterm = 0;
	strat = GS_REWR_INNER;
	while ( (opt = getopt_long(argc,argv,sopts,lopts,NULL)) != -1 )
	{
		switch ( opt )
		{
			case 'h':
				print_help(stderr);
				return 0;
			case 'a':
				read_aterm = 1;
				break;
			case 'w':
				write_aterm = 1;
				break;
			case 'i':
				strat = GS_REWR_INNER;
				break;
			default:
				break;
		}
	}

	if ( optind < argc && strcmp(argv[optind],"-") )
	{
		if ( (SpecStream = fopen(argv[optind],"r")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	} else {
		SpecStream = stdin;
	}

	if ( optind+1 < argc )
	{
		if ( (OutStream = fopen(argv[optind+1],"w")) == NULL )
		{
			perror(NAME);
			return 1;
		}
	} else {
		OutStream = stdout;
	}

	if ( read_aterm )
	{
		gsEnableConstructorFunctions();
		Spec = (ATermAppl) ATreadFromFile(SpecStream);
	} else {
		Spec = gsParseSpecification(SpecStream);
		fclose(SpecStream);
	}

	gsRewriteInit(Spec,strat);

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
}
