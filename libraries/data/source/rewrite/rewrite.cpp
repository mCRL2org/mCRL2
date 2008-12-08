// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <string>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/detail/rewrite/inner.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#ifdef MCRL2_INNERC_AVAILABLE
#include "mcrl2/data/detail/rewrite/innerc.h"
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
#include "mcrl2/data/detail/rewrite/jittyc.h"
#endif
#include "mcrl2/data/detail/rewrite/with_prover.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace std;

Rewriter::Rewriter()
{
	substs = NULL;
	substs_size = 0;
}

Rewriter::~Rewriter()
{
	if ( substs_size > 0 )
	{
		ATunprotectArray(substs);
	}
	free(substs);
}

ATermList Rewriter::rewriteList(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,(ATerm) rewrite((ATermAppl) ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

ATerm Rewriter::toRewriteFormat(ATermAppl Term)
{
	return (ATerm) Term;
}

ATermAppl Rewriter::fromRewriteFormat(ATerm Term)
{
	return (ATermAppl) Term;
}

ATerm Rewriter::rewriteInternal(ATerm Term)
{
	return (ATerm) rewrite((ATermAppl) Term);
}

ATermList Rewriter::rewriteInternalList(ATermList Terms)
{
	ATermList l = ATmakeList0();
	for (; !ATisEmpty(Terms); Terms=ATgetNext(Terms))
	{
		l = ATinsert(l,rewriteInternal(ATgetFirst(Terms)));
	}
	return ATreverse(l);
}

bool Rewriter::addRewriteRule(ATermAppl /*Rule*/)
{
	return false;
}

bool Rewriter::removeRewriteRule(ATermAppl /*Rule*/)
{
	return false;
}

void Rewriter::setSubstitution(ATermAppl Var, ATermAppl Expr)
{
	setSubstitutionInternal(Var,toRewriteFormat(Expr));
}

void Rewriter::setSubstitutionList(ATermList Substs)
{
	for (; !ATisEmpty(Substs); Substs=ATgetNext(Substs))
	{
		ATermAppl h = (ATermAppl) ATgetFirst(Substs);
		setSubstitutionInternal((ATermAppl) ATgetArgument(h,0),toRewriteFormat((ATermAppl) ATgetArgument(h,1)));
	}
}

void Rewriter::setSubstitutionInternal(ATermAppl Var, ATerm Expr)
{
	long n = ATgetAFun(ATgetArgument(Var,0));
	
	if ( n >= substs_size )
	{
		long newsize;
		
		if ( n >= 2*substs_size )
		{
			if ( n < 1024 )
			{
				newsize = 1024;
			} else {
				newsize = n+1;
			}
		} else {
			newsize = 2*substs_size;
		}
		
		if ( substs_size > 0 )
		{
			ATunprotectArray(substs);
		}
		substs = (ATerm *) realloc(substs,newsize*sizeof(ATerm));
		
		if ( substs == NULL )
		{
			gsErrorMsg("Failed to increase the size of a substitution array to %d\n",newsize);
			exit(1);
		}
		
		for (long i=substs_size; i<newsize; i++)
		{
			substs[i]=NULL;
		}

		ATprotectArray(substs,newsize);
		substs_size = newsize;
	}

	substs[n] = Expr;
}

void Rewriter::setSubstitutionInternalList(ATermList Substs)
{
	for (; !ATisEmpty(Substs); Substs=ATgetNext(Substs))
	{
		ATermAppl h = (ATermAppl) ATgetFirst(Substs);
		setSubstitutionInternal((ATermAppl) ATgetArgument(h,0),ATgetArgument(h,1));
	}
}

ATermAppl Rewriter::getSubstitution(ATermAppl Var)
{
	return fromRewriteFormat(lookupSubstitution(Var));
}

ATerm Rewriter::getSubstitutionInternal(ATermAppl Var)
{
	return lookupSubstitution(Var);
}

void Rewriter::clearSubstitution(ATermAppl Var)
{
	long n = ATgetAFun(ATgetArgument(Var,0));

	if ( n < substs_size )
	{
		substs[n] = NULL;
	}
}

void Rewriter::clearSubstitutions()
{
	for (long i=0; i<substs_size; i++)
	{
		substs[i] = NULL;
	}
}

void Rewriter::clearSubstitutions(ATermList Vars)
{
	for (; !ATisEmpty(Vars); Vars=ATgetNext(Vars))
	{
		clearSubstitution((ATermAppl) ATgetFirst(Vars));
	}
}

ATerm Rewriter::lookupSubstitution(ATermAppl Var)
{
	long n = ATgetAFun(ATgetArgument(Var,0));
	
	if ( n >= substs_size )
	{
		return (ATerm) Var;
	}
	
	ATerm r = substs[n];
	
	if ( r == NULL )
	{
		return (ATerm) Var;
	}
	
	return r;
}


Rewriter *createRewriter(data_specification DataSpec, RewriteStrategy Strategy)
{
	switch ( Strategy )
	{
		case GS_REWR_INNER:
			return new RewriterInnermost(DataSpec);
		case GS_REWR_JITTY:
			return new RewriterJitty(DataSpec);
#ifdef MCRL2_INNERC_AVAILABLE
		case GS_REWR_INNERC:
			return new RewriterCompilingInnermost(DataSpec);
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
		case GS_REWR_JITTYC:
			return new RewriterCompilingJitty(DataSpec);
#endif
		case GS_REWR_INNER_P:
			return new RewriterProver(DataSpec,GS_REWR_INNER);
		case GS_REWR_JITTY_P:
			return new RewriterProver(DataSpec,GS_REWR_JITTY);
#ifdef MCRL2_INNERC_AVAILABLE
		case GS_REWR_INNERC_P:
			return new RewriterProver(DataSpec,GS_REWR_INNERC);
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
		case GS_REWR_JITTYC_P:
			return new RewriterProver(DataSpec,GS_REWR_JITTYC);
#endif
		default:
			return NULL;
	}
}

//Prototype
static void checkVars(ATermAppl Expr, ATermList Vars, ATermList *UsedVars = NULL);

static void checkVars(ATermList Exprs, ATermList Vars, ATermList *UsedVars = NULL)
{
        assert(ATgetType(Exprs) == AT_LIST);

        for( ; !ATisEmpty(Exprs); Exprs = ATgetNext(Exprs))
        {
                checkVars((ATermAppl) ATAgetFirst(Exprs),Vars,UsedVars);
        }
}

static void checkVars(ATermAppl Expr, ATermList Vars, ATermList *UsedVars)
{
	assert(ATgetType(Expr) == AT_APPL);

	if ( gsIsDataAppl(Expr) )
	{
		checkVars((ATermAppl) ATgetArgument(Expr,0),Vars,UsedVars);
		checkVars((ATermList) ATLgetArgument(Expr,1),Vars,UsedVars);
	} else if ( gsIsDataVarId(Expr) )
	{
		if ( (UsedVars != NULL) && (ATindexOf(*UsedVars,(ATerm) Expr,0) < 0) )
		{
			*UsedVars = ATinsert(*UsedVars,(ATerm) Expr);
		}

		if ( ATindexOf(Vars,(ATerm) Expr,0) == -1 )
		{
			throw Expr;
		}
	}
}

//Prototype
static void checkPattern(ATermAppl p);

static void checkPattern(ATermList l)
{
        for( ; !ATisEmpty(l); l = ATgetNext(l) )
        {
                checkPattern(ATAgetFirst(l));
        }
}

static void checkPattern(ATermAppl p)
{
	if ( gsIsDataAppl(p) )
        {
		if ( gsIsDataVarId(ATAgetArgument(p,0)) )
		{
			throw string("variable "+PrintPart_CXX(ATgetArgument(p,0),ppDefault)+" is used as head symbol in an application, which is not supported");
		}
		checkPattern(ATAgetArgument(p,0));
		checkPattern(ATLgetArgument(p,1));
	}
}

void CheckRewriteRule(ATermAppl DataEqn)
{
	assert(gsIsDataEqn(DataEqn));

	ATermList rule_vars = ATLgetArgument(DataEqn,0);

	// collect variables from lhs and check that they are in rule_vars
	ATermList lhs_vars = ATmakeList0();
	try
	{
		checkVars(ATAgetArgument(DataEqn,2),rule_vars,&lhs_vars);
	} catch ( ATermAppl var )
	{
		// This should never occur if DataEqn is a valid data equation
		assert(0);
		throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in left-hand side of equation but is not defined (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+")");
	}

	// check that variables from the condition occur in the lhs
	try
	{
		checkVars(ATAgetArgument(DataEqn,1),lhs_vars);
	} catch ( ATermAppl var )
	{
		throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in condition of equation but not in left-hand side (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+"); equation cannot be used as rewrite rule");
	}

	// check that variables from the rhs are occur in the lhs
	try
	{
		checkVars(ATAgetArgument(DataEqn,3),lhs_vars);
	} catch ( ATermAppl var )
	{
		throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in right-hand side of equation but not in left-hand side (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+"); equation cannot be used as rewrite rule");
	}

	// check that the lhs is a supported pattern
	if ( gsIsDataVarId(ATAgetArgument(DataEqn,2)) )
	{
		throw runtime_error("left-hand side of equation is a variable; this is not allowed for rewriting");
	}
	try
	{
		checkPattern(ATAgetArgument(DataEqn,2));
	} catch ( string &s )
	{
		throw runtime_error(s+" (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+"); equation cannot be used as rewrite rule");
	}
}

bool isValidRewriteRule(ATermAppl DataEqn)
{
  try
  {
    CheckRewriteRule(DataEqn);
    return true;
  } catch ( runtime_error &e )
  {
    return false;
  }
}

void PrintRewriteStrategy(FILE *stream, RewriteStrategy strat)
{
  if (strat == GS_REWR_INNER) {
    fprintf(stream, "inner");
#ifdef MCRL2_INNERC_AVAILABLE
  } else if (strat == GS_REWR_INNERC) {
    fprintf(stream, "innerc");
#endif
  } else if (strat == GS_REWR_JITTY) {
    fprintf(stream, "jitty");
#ifdef MCRL2_JITTYC_AVAILABLE
  } else if (strat == GS_REWR_JITTYC) {
    fprintf(stream, "jittyc");
#endif
  } else if (strat == GS_REWR_INNER_P) {
    fprintf(stream, "innerp");
#ifdef MCRL2_INNERC_AVAILABLE
  } else if (strat == GS_REWR_INNERC_P) {
    fprintf(stream, "innercp");
#endif
  } else if (strat == GS_REWR_JITTY_P) {
    fprintf(stream, "jittyp");
#ifdef MCRL2_JITTYC_AVAILABLE
  } else if (strat == GS_REWR_JITTYC_P) {
    fprintf(stream, "jittycp");
#endif
  } else {
    fprintf(stream, "invalid");
  }
}

RewriteStrategy RewriteStrategyFromString(const char *s)
{
  static RewriteStrategy strategies[9] = { GS_REWR_INVALID,
#ifdef MCRL2_INNERC_AVAILABLE
          GS_REWR_INNER, GS_REWR_INNERC, GS_REWR_INNER_P, GS_REWR_INNERC_P,
#else
          GS_REWR_INNER, GS_REWR_INVALID, GS_REWR_INNER_P, GS_REWR_INVALID,
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
          GS_REWR_JITTY, GS_REWR_JITTYC, GS_REWR_JITTY_P, GS_REWR_JITTYC_P };
#else
          GS_REWR_JITTY, GS_REWR_INVALID, GS_REWR_JITTY_P, GS_REWR_INVALID };
#endif

  size_t main_strategy = 0; // default invalid

  if (std::strncmp(&s[0], "inner", 5) == 0) { // not jitty{,c,cp} inner{,c,cp}
    main_strategy = 1;
  }
  else if (std::strncmp(&s[0], "jitty", 5) == 0) { // jitty{,c,cp}
    main_strategy = 5;
  }

  if (s[5] == '\0') { // interpreting
    return strategies[main_strategy];
  }
  else if (s[6] == '\0') {
    if (s[5] == 'c') { // compiling
      return strategies[main_strategy + 1];
    }
    else if (s[5] == 'p') { // with prover
      return strategies[main_strategy + 2];
    }
  }
  else if (s[5] == 'c' && s[6] == 'p' && s[7] == '\0') { // compiling with prover
    return strategies[main_strategy + 3];
  }

  return GS_REWR_INVALID;
}
