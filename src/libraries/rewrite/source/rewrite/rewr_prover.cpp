#define NAME "rewr_prover"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include <memory.h>
#include <assert.h>
#include <liblowlevel.h>
#include <libstruct.h>
#include <libprint_c.h>
#include <lpe/data_specification.h>
#include <prover/bdd_prover.h>
#include "librewrite.h"
#include "rewr_prover.h"

RewriterProver::RewriterProver(lpe::data_specification DataSpec, RewriteStrategy strat)
{
  prover_obj = new BDD_Prover(DataSpec,strat);
  rewr_obj = prover_obj->get_rewriter();
}

RewriterProver::~RewriterProver()
{
  delete prover_obj;
}

bool RewriterProver::addRewriteRule(ATermAppl Rule)
{
  return rewr_obj->addRewriteRule(Rule);
}

bool RewriterProver::removeRewriteRule(ATermAppl Rule)
{
  return rewr_obj->removeRewriteRule(Rule);
}

ATerm RewriterProver::rewriteInternal(ATerm Term)
{
  return rewr_obj->toRewriteFormat(
                rewrite(
                  rewr_obj->fromRewriteFormat(Term)
                  )
                );
}

ATermAppl RewriterProver::rewrite(ATermAppl Term)
{
  if ( gsGetSort(Term) == gsMakeSortExprBool() )
  {
    prover_obj->set_formula(Term);
    return prover_obj->get_bdd();
  } else {
    return rewr_obj->rewrite(Term);
  }
}

ATerm RewriterProver::toRewriteFormat(ATermAppl Term)
{
  return rewr_obj->toRewriteFormat(Term);
}

ATermAppl RewriterProver::fromRewriteFormat(ATerm Term)
{
  return rewr_obj->fromRewriteFormat(Term);
}

void RewriterProver::setSubstitution(ATermAppl Var, ATerm Expr)
{
  return rewr_obj->setSubstitution(Var,Expr);
}

ATerm RewriterProver::getSubstitution(ATermAppl Var)
{
  return rewr_obj->getSubstitution(Var);
}

void RewriterProver::clearSubstitution(ATermAppl Var)
{
  return rewr_obj->clearSubstitution(Var);
}

void RewriterProver::clearSubstitutions()
{
  return rewr_obj->clearSubstitutions();
}

RewriteStrategy RewriterProver::getStrategy()
{
  switch ( rewr_obj->getStrategy() )
  {
    case GS_REWR_INNER:
      return GS_REWR_INNER_P;
    case GS_REWR_JITTY:
      return GS_REWR_JITTY_P;
    case GS_REWR_INNERC:
      return GS_REWR_INNERC_P;
    case GS_REWR_JITTYC:
      return GS_REWR_JITTYC_P;
    default:
      return GS_REWR_INVALID;
  }
}

