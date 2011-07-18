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
#include <cstring>
#include "mcrl2/aterm/aterm2.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/print.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/data/detail/rewrite/jitty.h"
#ifdef MCRL2_JITTYC_AVAILABLE
#include "mcrl2/data/detail/rewrite/jittyc.h"
#endif
using namespace mcrl2::data::detail;
#include "mcrl2/data/detail/rewrite/with_prover.h"

#include "mcrl2/data/fresh_variable_generator.h"
#include "mcrl2/data/detail/enum/standard.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/rewrite/nfs_array.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace std;
using namespace mcrl2::log;

namespace mcrl2
{
namespace data
{
namespace detail
{

Rewriter::Rewriter()
{
}

Rewriter::~Rewriter()
{
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

ATerm Rewriter::internal_existential_quantifier_enumeration(ATerm ATermInInnerFormat)
{
  /* Get Body of Exists */
  ATerm t1 = ATgetArgument(ATermInInnerFormat,1);
  data_expression d(fromRewriteFormat(t1));

  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();

  static data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var");

  /* Create for each of the sorts for enumeration a new variable*/
  variable_vector vv;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i)
  {
    variable v(generator(*i));
    vv.push_back(v);
  }

  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);

  /* Find A solution*/
  const variable_list vl=atermpp::convert< variable_list >(vv);

  EnumeratorSolutionsStandard sol(vl,
      toRewriteFormat(application (d, atermpp::convert< data_expression_list >(vv))),
      true,&ES,100);

  /* Create ATermList to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  bool has_exact_solution = false;
  bool has_no_solution =true;
  bool solution_possible=true;

  size_t loop_upperbound=5;
  while (loop_upperbound>0 && sol.next(has_exact_solution,x,solution_possible) && !has_exact_solution)
  {
    has_no_solution = false;
    loop_upperbound--;
  }

  if (solution_possible)
  {
    if (has_exact_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return true (exist)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::true_());
    }
    else if (has_no_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return false (exist)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::false_());
    }
  }
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
  std::cerr << "An existential quantifier could not be eliminated and remains unchanged.\n";
#endif

 return ATermInInnerFormat;
}

ATerm Rewriter::internal_universal_quantifier_enumeration(ATerm ATermInInnerFormat)
{
  /* Get Body of forall */
  ATerm t1 = ATgetArgument(ATermInInnerFormat,1);
  data_expression d(fromRewriteFormat(t1));
  
  /* Get Sort for enumeration from Body*/
  sort_expression_list fsdomain = function_sort(d.sort()).domain();
  
  data::fresh_variable_generator<> generator;
  generator.add_identifiers(find_identifiers(d));
  generator.set_hint("var");
  
  /* Create for each of the sorts for enumeration a new variable*/
  variable_vector vv;
  for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i)
  {
    variable v(generator(*i));
    vv.push_back(v);
  }
  
  /* Create Enumerator */
  EnumeratorStandard ES(m_data_specification_for_enumeration, this);
  
  /* Find A solution*/
  const variable_list vl=atermpp::convert< variable_list >(vv);
  EnumeratorSolutionsStandard sol(vl,
      toRewriteFormat(application (d, atermpp::convert< data_expression_list >(vv))),
      false,&ES,100);
  
  /* Create ATermList to store solutions */
  atermpp::term_list<atermpp::aterm_appl> x;
  bool has_exact_solution = false;
  bool has_no_solution =true;
  bool solution_possible=true;
  
  size_t loop_upperbound=5;
  
  while (loop_upperbound>0 && sol.next(has_exact_solution,x,solution_possible) && !has_exact_solution)
  {
    has_no_solution = false;
    loop_upperbound--;
  }

  if (solution_possible)
  {
    if (has_exact_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return false (forall)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::false_());
    }
    else if (has_no_solution)
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      std::cerr << "  return true (forall)\n";
#endif
      return toRewriteFormat(mcrl2::data::sort_bool::true_());
    }
  }
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
  std::cerr << "  A universal quantifier could not be eliminated and remains unchanged.\n";
#endif
  return ATermInInnerFormat;  // We were unable to remove the universal quantifier.
}

ATerm Rewriter::internal_quantifier_enumeration(ATerm ATermInInnerFormat)
{

#ifndef MCRL2_DISABLE_QUANTIFIER_ENUMERATION
  if (ATisAppl(ATermInInnerFormat))
  {
    ATerm arg = ATgetArgument(ATermInInnerFormat,0);

    /* Make sure that we have indeed a rewrite rule */
    if (ATisInt(arg))
    {
      /* Convert internal rewrite number to ATerm representation*/
      ATermAppl a = get_int2term(ATgetInt((ATermInt) arg));

      if (is_function_symbol(a))
      {
        /* Check for universal quantifier */
        if (function_symbol(a).name() == forall_function_symbol())
        {
          ATermInInnerFormat = internal_universal_quantifier_enumeration(ATermInInnerFormat);
        }
        /* Check for existential quantifier */
        if (function_symbol(a).name() == exists_function_symbol())
        {
          ATermInInnerFormat = internal_existential_quantifier_enumeration(ATermInInnerFormat);
        }
      }
    }
  }
#endif
  return ATermInInnerFormat;
}


Rewriter* createRewriter(const data_specification& DataSpec, const RewriteStrategy Strategy, const bool add_rewrite_rules)
{
  switch (Strategy)
  {
    case GS_REWR_JITTY:
      return new RewriterJitty(DataSpec,add_rewrite_rules);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC:
      return new RewriterCompilingJitty(DataSpec,add_rewrite_rules);
#endif
    case GS_REWR_JITTY_P:
      return new RewriterProver(DataSpec,mcrl2::data::rewriter::jitty,add_rewrite_rules);
#ifdef MCRL2_JITTYC_AVAILABLE
    case GS_REWR_JITTYC_P:
      return new RewriterProver(DataSpec,data::rewriter::jitty_compiling,add_rewrite_rules);
#endif
    default:
      return NULL;
  }
}

//Prototype
static void checkVars(ATermAppl Expr, ATermList Vars, ATermList* UsedVars = NULL);

static void checkVars(ATermList Exprs, ATermList Vars, ATermList* UsedVars = NULL)
{
  assert(ATgetType(Exprs) == AT_LIST);

  for (; !ATisEmpty(Exprs); Exprs = ATgetNext(Exprs))
  {
    checkVars((ATermAppl) ATAgetFirst(Exprs),Vars,UsedVars);
  }
}

static void checkVars(ATermAppl Expr, ATermList Vars, ATermList* UsedVars)
{
  assert(ATgetType(Expr) == AT_APPL);

  if (gsIsDataAppl(Expr))
  {
    checkVars((ATermAppl) ATgetArgument(Expr,0),Vars,UsedVars);
    checkVars((ATermList) ATLgetArgument(Expr,1),Vars,UsedVars);
  }
  else if (gsIsDataVarId(Expr))
  {
    if ((UsedVars != NULL) && (ATindexOf(*UsedVars,(ATerm) Expr,0) == ATERM_NON_EXISTING_POSITION))
    {
      *UsedVars = ATinsert(*UsedVars,(ATerm) Expr);
    }

    if (ATindexOf(Vars,(ATerm) Expr,0) == ATERM_NON_EXISTING_POSITION)
    {
      throw Expr;
    }
  }
}

//Prototype
static void checkPattern(ATermAppl p);

static void checkPattern(ATermList l)
{
  for (; !ATisEmpty(l); l = ATgetNext(l))
  {
    checkPattern(ATAgetFirst(l));
  }
}

static void checkPattern(ATermAppl p)
{
  if (gsIsDataAppl(p))
  {
    if (gsIsDataVarId(ATAgetArgument(p,0)))
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
  }
  catch (ATermAppl var)
  {
    // This should never occur if DataEqn is a valid data equation
    mCRL2log(log::error) << "Data Equation: " << atermpp::aterm_appl(DataEqn) << std::endl;
    assert(0);
    throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in left-hand side of equation but is not defined (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+")");
  }

  // check that variables from the condition occur in the lhs
  try
  {
    checkVars(ATAgetArgument(DataEqn,1),lhs_vars);
  }
  catch (ATermAppl var)
  {
    throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in condition of equation but not in left-hand side (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+"); equation cannot be used as rewrite rule");
  }

  // check that variables from the rhs are occur in the lhs
  try
  {
    checkVars(ATAgetArgument(DataEqn,3),lhs_vars);
  }
  catch (ATermAppl var)
  {
    throw runtime_error("variable "+PrintPart_CXX((ATerm) var,ppDefault)+" occurs in right-hand side of equation but not in left-hand side (in equation: "+PrintPart_CXX((ATerm) DataEqn,ppDefault)+"); equation cannot be used as rewrite rule");
  }

  // check that the lhs is a supported pattern
  if (gsIsDataVarId(ATAgetArgument(DataEqn,2)))
  {
    throw runtime_error("left-hand side of equation is a variable; this is not allowed for rewriting");
  }
  try
  {
    checkPattern(ATAgetArgument(DataEqn,2));
  }
  catch (string& s)
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
  }
  catch (runtime_error&)
  {
    return false;
  }
}

void PrintRewriteStrategy(FILE* stream, RewriteStrategy strat)
{
  if (strat == GS_REWR_JITTY)
  {
    fprintf(stream, "jitty");
#ifdef MCRL2_JITTYC_AVAILABLE
  }
  else if (strat == GS_REWR_JITTYC)
  {
    fprintf(stream, "jittyc");
#endif
  }
  else if (strat == GS_REWR_JITTY_P)
  {
    fprintf(stream, "jittyp");
#ifdef MCRL2_JITTYC_AVAILABLE
  }
  else if (strat == GS_REWR_JITTYC_P)
  {
    fprintf(stream, "jittycp");
#endif
  }
  else
  {
    fprintf(stream, "invalid");
  }
}

RewriteStrategy RewriteStrategyFromString(const char* s)
{
  static RewriteStrategy strategies[9] = { GS_REWR_INVALID,
#ifdef MCRL2_JITTYC_AVAILABLE
                                         GS_REWR_JITTY, GS_REWR_JITTYC, GS_REWR_JITTY_P, GS_REWR_JITTYC_P
                                         };
#else
                                         GS_REWR_JITTY, GS_REWR_INVALID, GS_REWR_JITTY_P, GS_REWR_INVALID
                                         };
#endif

  size_t main_strategy = 0; // default invalid

  if (std::strncmp(&s[0], "jitty", 5) == 0)   // jitty{,c,cp}
  {
    main_strategy = 1;
  
    if (s[5] == '\0')   // interpreting
    {
      return strategies[main_strategy];
    }
    else if (s[6] == '\0')
    {
      if (s[5] == 'c')   // compiling
      {
        return strategies[main_strategy + 1];
      }
      else if (s[5] == 'p')   // with prover
      {
        return strategies[main_strategy + 2];
      }
    }
    else if (s[5] == 'c' && s[6] == 'p' && s[7] == '\0')   // compiling with prover
    {
      return strategies[main_strategy + 3];
    }
  }

  return GS_REWR_INVALID;
}

std::vector <AFun> apples;

/* static size_t num_apples = 0;
static AFun* apples = NULL;

AFun get_appl_afun_value(size_t arity)
{
  if (arity >= num_apples)
  {
    size_t old_num = num_apples;
    num_apples = arity+1;
    apples = (AFun*) realloc(apples,num_apples*sizeof(AFun));
    if (apples == NULL)
    {
      throw mcrl2::runtime_error("Cannot allocate enough memory.");
    }

    for (; old_num < num_apples; old_num++)
    {
      apples[old_num] = ATmakeAFun("#REWR#",old_num,false);
      ATprotectAFun(apples[old_num]);
    }
  }
  return apples[arity];
}

ATermAppl Apply(ATermList l)
{
  const size_t n=ATgetLength(l);
  return ATmakeApplList(get_appl_afun_value(n),l);
}

ATermAppl ApplyArray(const size_t size, ATerm *l)
{
  return ATmakeApplArray(get_appl_afun_value(size),l);
}

ATermAppl Apply0(const ATerm head)
{
 return ATmakeAppl1(get_appl_afun_value(1),head);
}

ATermAppl Apply1(const ATerm head, const ATerm arg1)
{
 return ATmakeAppl2(get_appl_afun_value(2),head,arg1);
}

ATermAppl Apply2(const ATerm head, const ATerm arg1, const ATerm arg2)
{
 return ATmakeAppl3(get_appl_afun_value(3),head,arg1,arg2);
} */

/*************  Below the functions toInner and fromInner are being defined *********************/


atermpp::map< ATerm, ATermInt > &term2int()
{ 
  static atermpp::map< ATerm, ATermInt > term2int;
  return term2int;
}

atermpp::map< ATerm, ATermInt >::const_iterator term2int_begin()
{ 
  return term2int().begin();
}

atermpp::map< ATerm, ATermInt >::const_iterator term2int_end()
{ 
  return term2int().end();
}

atermpp::vector < ATermAppl > &int2term()
{ 
  static atermpp::vector < ATermAppl > int2term;
  return int2term;
}

size_t get_num_opids()
{
  return int2term().size();
}

ATermAppl get_int2term(const size_t n)
{
  assert(n<int2term().size());
  return int2term()[n];
}

void set_int2term(const size_t n, const ATermAppl t)
{
  if (n>=int2term().size())
  {
    int2term().resize(n+1);
  }
  int2term()[n]=t;
}

size_t getArity(ATermAppl op)
{
  ATermAppl sort = ATAgetArgument(op,1);
  size_t arity = 0;

  while (is_function_sort(sort_expression(sort)))
  {
    ATermList sort_dom = ATLgetArgument(sort, 0);
    arity += ATgetLength(sort_dom);
    sort = ATAgetArgument(sort, 1);
  }
  return arity;
}

void initialize_internal_translation_table_rewriter()
{
}

ATerm OpId2Int(ATermAppl Term, bool add_opids)
{
  atermpp::map< ATerm, ATermInt >::iterator f = term2int().find((ATerm) Term);
  if (f == term2int().end())
  {
    if (!add_opids)
    {
      return (ATerm) Term;
    }
    const size_t num_opids=get_num_opids();
    ATermInt i = ATmakeInt(num_opids);
    term2int()[(ATerm) Term] =  i;
    int arity = getArity(Term);
    if (arity > NF_MAX_ARITY)
    {
      arity = NF_MAX_ARITY;
    }
    assert(int2term().size()==num_opids);
    int2term().push_back(Term);
    size_t num_aux = (1 << (arity+1)) - arity - 2; // 2^(arity+1) - arity - 2; Reserve extra space
                                                   // to accomodate function symbols that represent
                                                   // terms with partially normalized arguments.
    if (arity <= NF_MAX_ARITY)
    {
      for(size_t k=0; k<num_aux; ++k)
      {
        int2term().push_back((ATermAppl)atermpp::aterm_appl()); 
      }
    }
    return (ATerm) i;
  }

  ATermInt j = f->second;
  return (ATerm) j;
} 

ATerm toInner(ATermAppl Term, bool add_opids)
{
  if (gsIsDataAppl(Term))
  {
    ATermList l = ATmakeList0();
    for (ATermList args = ATLgetArgument((ATermAppl) Term, 1) ; !ATisEmpty(args) ; args = ATgetNext(args))
    {
      l = ATinsert(l,(ATerm) toInner((ATermAppl) ATgetFirst(args),add_opids));
    }

    l = ATreverse(l);

    ATerm arg0 = toInner(ATAgetArgument((ATermAppl) Term, 0), add_opids);
    if (ATisList(arg0))
    {
      l = ATconcat((ATermList) arg0, l);
    }
    else
    {
      l = ATinsert(l, arg0);
    }
    return (ATerm) l;
  }
  else
  {
    if (gsIsOpId(Term))
    {
      return (ATerm) OpId2Int(Term,add_opids);
    }
    else
    {
      return (ATerm) Term;
    }
  }

}

ATermAppl toInnerc(ATermAppl Term, const bool add_opids)
{

  if (gsIsNil(Term) || gsIsDataVarId(Term))
  {
    return Term;
  }

  ATermList l = ATmakeList0();

  if (!gsIsDataAppl(Term))
  {
    if (gsIsOpId(Term))
    {
      // l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
      return Apply0(OpId2Int(Term,add_opids));
    }
    else
    {
      return Apply0((ATerm) Term);
    }
  }
  else
  {
    ATermAppl arg0 = toInnerc(ATAgetArgument(Term, 0), add_opids);
    // Reflect the way of encoding the other arguments!
    if (gsIsNil(arg0) || gsIsDataVarId(arg0))
    {
      l = ATinsert(l, (ATerm) arg0);
    }
    else
    {
      size_t arity = ATgetArity(ATgetAFun(arg0));
      for (size_t i = 0; i < arity; ++i)
      {
        l = ATinsert(l, ATgetArgument(arg0, i));
      }
    }
    for (ATermList args = ATLgetArgument((ATermAppl) Term,1); !ATisEmpty(args) ; args = ATgetNext(args))
    {
      l = ATinsert(l,(ATerm) toInnerc((ATermAppl) ATgetFirst(args),add_opids));
    }
    l = ATreverse(l);
  }
  return Apply(l);
}

ATermAppl fromInner(ATerm Term)
{
  ATermAppl a;

  if (gsIsDataVarId((ATermAppl)Term))
  {
    return (ATermAppl)Term;
  }

  size_t arity = ATgetArity(ATgetAFun(Term));
  ATerm t = ATgetArgument(Term,0);
  if (ATisInt(t))
  {
    a = get_int2term(ATgetInt((ATermInt) t));
  }
  else
  {
    a = (ATermAppl) t;
  }

  if (gsIsOpId(a) || gsIsDataVarId(a))
  {
    size_t i = 1;
    ATermAppl sort = ATAgetArgument(a, 1);
    while (is_function_sort(sort_expression(sort)) && (i < arity))
    {
      ATermList sort_dom = ATLgetArgument(sort, 0);
      ATermList list = ATmakeList0();
      while (!ATisEmpty(sort_dom))
      {
        assert(i < arity);
        list = ATinsert(list, (ATerm) fromInner(ATgetArgument(Term,i)));
        sort_dom = ATgetNext(sort_dom);
        ++i;
      }
      list = ATreverse(list);
      a = gsMakeDataAppl(a, list);
      sort = ATAgetArgument(sort, 1);
    }
  }
  return a;
} 


}
}
}
