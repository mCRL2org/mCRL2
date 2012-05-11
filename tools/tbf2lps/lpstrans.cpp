// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpstrans.cpp

/* <muCRL LPE> ::= spec2gen(<DataSpec>,<LPE>)
 * <LPE>       ::= initprocspec(<DataExpr>*,<Var>*,<Sum>*)
 * <DataSpec>  ::= d(<DataDecl>,<EqnDecl>*)
 * <DataDecl>  ::= s(<Id>*,<Func>*,<Func>*)
 * <EqnDecl>   ::= e(<Var>*,<DataExpr>,<DataExpr>)
 * <Sum>       ::= smd(<Var>*,<Id>,<Id>*,<NextState>,<DataExpr>)
 * <NextState> ::= i(<DataExpr>*)
 * <Func>      ::= f(<Id>,<Id>*,<Id>)
 * <Var>       ::= v(<Id>,<Id>)
 * <DataExpr>  ::= <Id> | <Id>(<Id>,...,<Id>)
 * <Id>        ::= <String>
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mcrl2/aterm/aterm.h"
#include <assert.h>
#include <string>
#include <sstream>
#include "lpstrans.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/assignment.h"

using namespace mcrl2::log;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

static bool remove_bools = true;
static bool remove_standard_functions = true;
static bool has_func_T = false;
static ATermList typelist = NULL;

bool is_mCRL_spec(ATermAppl spec)
{
  return ATgetAFun(spec) == AFun("spec2gen", 2, false).number();
}


static bool remove_sort_decl(ATermAppl sort)
{
  return remove_bools && !strcmp("Bool",ATgetName(ATgetAFun(sort)));
}

static bool remove_func_decl(ATermAppl func)
{
  const char* name = ATgetName(ATgetAFun(ATAgetArgument(func,0)));
  if (remove_bools && (!strcmp("T#",name) ||
                       !strcmp("F#",name)))
  {
    return true;
  }
  if (remove_standard_functions)
  {
    if (!strcmp("and#Bool#Bool",name))
    {
      return true;
//    } else if ( !strncmp("if#Bool#",name,8) )
//    {
//      const char *s = strchr(name+8,'#');
//      if ( s != NULL && !strncmp(name+8,s+1,s-name) && s[1+(s-name)] == '\0' )
//      {
//        return true;
//      }
    }
    else if (!strncmp("eq#",name,3))
    {
      const char* s = strchr(name+3,'#');
      if (s != NULL && !strncmp(name+3,s+1,s-name-3) && s[1+(s-name-3)] == '\0')
      {
        return true;
      }
    }
  }
  return false;
}


static void add_id(ATermList* ids, ATermAppl id)
{
  if (ATindexOf(*ids,(ATerm) id,0) == ATERM_NON_EXISTING_POSITION)
  {
    *ids = ATappend(*ids,(ATerm) id);
  }
}

static bool is_domain(ATermList args, ATermAppl sort)
{
  if (!is_basic_sort(mcrl2::data::sort_expression(sort)))
  {
    ATermList dom = ATLgetArgument(sort,0);
    if (ATgetLength(args) != ATgetLength(dom))
    {
      return false;
    }
    else
    {
      for (; !ATisEmpty(dom); dom=ATgetNext(dom),args=ATgetNext(args))
      {
        if (!ATisEqual(static_cast<ATermAppl>(mcrl2::data::data_expression(ATAgetFirst(args)).sort()),ATgetFirst(dom)))
        {
          return false;
        }
      }
      return true;
    }
  }
  if (ATisEmpty(args))
  {
    return true;
  }
  else
  {
    return false;
  }
}

static ATermAppl find_type(ATermAppl a, ATermList args, ATermList types = NULL)
{
  if (types == NULL)
  {
    types = typelist;
  }
  for (ATermList l=types; !ATisEmpty(l); l=ATgetNext(l))
  {
    if (!strcmp(ATgetName(ATgetAFun(a)),ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(l),0)))))
    {
      if (is_domain(args,ATAgetArgument(ATAgetFirst(l),1)))
      {
        return ATAgetArgument(ATAgetFirst(l),1);
      }
    }
  }

  return NULL;
}

static ATermAppl dataterm2ATermAppl(ATermAppl t, ATermList args)
{
  using namespace mcrl2::data;

  ATermList l = ATgetArguments(t);
  ATermAppl t2 = ATmakeAppl0(AFun(ATgetName(ATgetAFun(t)),0,true));

  if (ATisEmpty(l))
  {
    ATermAppl r = find_type(t,ATmakeList0(),args);
    if (r == NULL)
    {
      return mcrl2::data::function_symbol(mcrl2::core::identifier_string(t2),sort_expression(find_type(t,mcrl2::data::sort_expression_list())));
    }
    else
    {
      return variable(mcrl2::core::identifier_string(t2),sort_expression(r));
    }
  }
  else
  {
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      m = ATappend(m,(ATerm) dataterm2ATermAppl(ATAgetFirst(l),args));
    }\

    return application(mcrl2::data::function_symbol(mcrl2::core::identifier_string(t2),sort_expression(find_type(t,m))), atermpp::convert<data_expression_list>(atermpp::aterm_list(m)));
  }
}

static ATermList get_lps_acts(ATermAppl lps, ATermList* ids)
{
  ATermList acts = ATmakeList0();
  ATermList sums = ATLgetArgument(lps,1);
  for (; !ATisEmpty(sums); sums=ATgetNext(sums))
  {
    if (!ATisEmpty(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)))
    {
      ATermAppl a = ATAgetArgument(ATAgetFirst(ATLgetArgument(ATAgetArgument(ATAgetFirst(sums),2),0)),0);
      if (ATindexOf(acts,(ATerm) a,0) == ATERM_NON_EXISTING_POSITION)
      {
        acts = ATinsert(acts,(ATerm) a);
        add_id(ids,ATAgetArgument(a,0));
      }
    }
  }

  return ATreverse(acts);
}

static ATermList get_substs(ATermList ids)
{
  std::set < ATerm > used;
  ATermList substs = ATmakeList0();

  used.insert((ATerm) ATmakeAppl0(AFun("if",0,true)));

  for (; !ATisEmpty(ids); ids=ATgetNext(ids))
  {
    char s[100], *t;

    t = ATgetName(ATgetAFun(ATAgetFirst(ids)));
    if ((t[0] >= '0' && t[0] <= '9') || t[0] == '\'')
    {
      s[0] = '_';
      strncpy(s+1,t,99);
    }
    else
    {
      strncpy(s,t,100);
    }

    s[99] = '#';
    for (t=s; *t && (*t)!='#'; t++)
    {
      if (!((*t >= 'A' && *t <= 'Z') ||
            (*t >= 'a' && *t <= 'z') ||
            (*t >= '0' && *t <= '9') ||
            *t == '_' || *t == '\''))
      {
        *t = '_';
      }
    }
    *t = 0;

    size_t i = 0;
    ATermAppl new_id = 0;
    while (!is_user_identifier(s) ||
           (used.count((ATerm)(new_id = ATmakeAppl0(AFun(s,0,true)))) >0))
    {
      sprintf(t,"%zu",i);
      i++;
    }

    used.insert((ATerm) new_id);

    substs = ATinsert(substs,(ATerm) gsMakeSubst(ATgetFirst(ids),(ATerm) new_id));
  }

  return substs;
}




/*****************************************************
 ************* Main conversion functions *************
 *****************************************************/

static ATermList convert_sorts(ATermAppl spec, ATermList* ids)
{
  ATermList sorts = ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),0);
  ATermList r;

  r = ATmakeList0();
  for (; !ATisEmpty(sorts); sorts=ATgetNext(sorts))
  {
    if (!remove_sort_decl(ATAgetFirst(sorts)))
    {
      add_id(ids,ATAgetFirst(sorts));
      r = ATinsert(r,(ATerm) static_cast<ATermAppl>(mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetFirst(sorts)))));
    }
  }

  return ATreverse(r);
}

static ATermList convert_funcs(ATermList funcs, ATermList* ids, bool funcs_are_cons = false)
{
  ATermList r,l, sorts;
  ATermAppl sort;

  r = ATmakeList0();
  for (; !ATisEmpty(funcs); funcs=ATgetNext(funcs))
  {
    if (funcs_are_cons && !strcmp("T#",ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(funcs),0)))))
    {
      has_func_T = true;
    }

    l = ATreverse(ATLgetArgument(ATAgetFirst(funcs),1));
    sorts = ATmakeList0();
    sort = static_cast<ATermAppl>(mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(funcs),2))));
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      sorts = ATinsert(sorts, (ATerm) static_cast<ATermAppl>(mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetFirst(l)))));
    }
    if (!ATisEmpty(sorts))
    {
      sort = mcrl2::data::function_sort(atermpp::convert<mcrl2::data::sort_expression_list>(sorts), mcrl2::data::sort_expression(sort));
    }

    ATerm f = (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(funcs),0)),
                            mcrl2::data::sort_expression(sort)));

    if (!remove_func_decl(ATAgetFirst(funcs)))
    {
      if (funcs_are_cons && remove_bools && !strcmp("Bool",ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(funcs),2)))))
      {
        std::string name(ATgetName(ATgetAFun(ATAgetArgument(ATAgetFirst(funcs),0))));
        mCRL2log(error) << "constructor " << name.substr(0, name.find_last_of('#')) << " of sort Bool found (only T and F are allowed)" << std::endl;
        exit(1);
      }
      add_id(ids,ATAgetArgument(ATAgetFirst(funcs),0));
      r = ATinsert(r,f);
    }

    typelist = ATinsert(typelist,f);
  }

  return ATreverse(r);
}

static ATermList convert_cons(ATermAppl spec, ATermList* ids)
{
  return convert_funcs(ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),1),ids, true);
}

static ATermList convert_maps(ATermAppl spec, ATermList* ids)
{
  return convert_funcs(ATLgetArgument(ATAgetArgument(ATAgetArgument(spec,0),0),2),ids);
}

static ATermList convert_datas(ATermAppl spec, ATermList* ids)
{
  ATermList eqns = ATLgetArgument(ATAgetArgument(spec,0),1);
  ATermList l,args,r;
  ATermAppl lhs,rhs;

  r = ATmakeList0();
  for (; !ATisEmpty(eqns); eqns=ATgetNext(eqns))
  {
    l = ATLgetArgument(ATAgetFirst(eqns),0);
    args = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      args = ATappend(args,(ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),0)),
                    mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),1))))));
      add_id(ids,ATAgetArgument(ATAgetFirst(l),0));
    }
    ATermAppl lhs_before_translation=ATAgetArgument(ATAgetFirst(eqns),1);
    if (strcmp(ATgetName(ATgetAFun(lhs_before_translation)),"eq#Bool#Bool"))
    {
      // No match.
      lhs = dataterm2ATermAppl(lhs_before_translation,args);
      rhs = dataterm2ATermAppl(ATAgetArgument(ATAgetFirst(eqns),2),args);
      r = ATappend(r,(ATerm) gsMakeDataEqn(args,sort_bool::true_(),lhs,rhs));
    }
  }

  return r;
}

static ATermAppl convert_lps(ATermAppl spec, ATermList* ids)
{
  ATermList vars = ATLgetArgument(ATAgetArgument(spec,1),1);
  ATermList sums = ATLgetArgument(ATAgetArgument(spec,1),2);
  ATermList pars = ATmakeList0();
  ATermList smds = ATmakeList0();

  for (; !ATisEmpty(vars); vars=ATgetNext(vars))
  {
    ATermAppl v = ATAgetFirst(vars);
    pars = ATinsert(pars,
                    (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(ATAgetArgument(v,0)),
                                    mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(v,1)))))
                   );
    add_id(ids,ATAgetArgument(v,0));
  }
  pars = ATreverse(pars);

  for (; !ATisEmpty(sums); sums=ATgetNext(sums))
  {
    ATermAppl s = ATAgetFirst(sums);

    ATermList l = ATreverse(ATLgetArgument(s,0));
    ATermList m = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      m = ATinsert(m,
                   (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),0)),
                               mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),1)))))
                  );
      add_id(ids,ATAgetArgument(ATAgetFirst(l),0));
    }

    l = ATLgetArgument(s,0);
    ATermList o = pars;
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      o = ATinsert(o,
                   (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),0)),
                        mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(ATAgetFirst(l),1)))))
                  );
    }
    ATermAppl c = dataterm2ATermAppl(ATAgetArgument(s,4),o);
    if (! remove_bools)
    {
      assert(has_func_T);
      if (!has_func_T)
      {
        mCRL2log(error) << "cannot convert summand condition due to the absence of boolean constructor T" << std::endl;
        exit(1);
      }
      c = mcrl2::data::equal_to(mcrl2::data::data_expression(c), mcrl2::data::function_symbol("T",mcrl2::data::sort_bool::bool_()));
    }

    l = ATreverse(ATLgetArgument(s,2));
    ATermList al = ATmakeList0();
    ATermList as = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      al = ATinsert(al,
                    (ATerm) dataterm2ATermAppl(ATAgetFirst(l),o)
                   );
      as = ATinsert(as,(ATerm) static_cast<ATermAppl>(mcrl2::data::data_expression(ATAgetFirst(al)).sort()));
    }
    ATermAppl a = gsMakeAction(gsMakeActId(ATAgetArgument(s,1),as),al);
    if (ATisEmpty(as) && !strcmp("tau",ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(a,0),0)))))
    {
      a = gsMakeMultAct(ATmakeList0());
    }
    else
    {
      a = gsMakeMultAct(ATmakeList1((ATerm) a));
    }

    l = ATLgetArgument(ATAgetArgument(s,3),0);
    ATermList o2 = pars;
    ATermList n = ATmakeList0();
    for (; !ATisEmpty(l); l=ATgetNext(l),o2=ATgetNext(o2))
    {
      ATermAppl par = ATAgetFirst(o2);
      ATermAppl val = dataterm2ATermAppl(ATAgetFirst(l),o);
      if (!ATisEqual(par,val))
      {
        n = ATinsert(n,(ATerm) static_cast<ATermAppl>(mcrl2::data::assignment(mcrl2::data::variable(par),mcrl2::data::data_expression(val))));
      }
    }
    n = ATreverse(n);

    smds = ATinsert(smds,
                    (ATerm) gsMakeLinearProcessSummand(m,c,a,gsMakeNil(),n)
                   );
  }

  return gsMakeLinearProcess(pars,ATreverse(smds));
}

static ATermList convert_init(ATermAppl spec, ATermList* /*ids*/)
{
  ATermList vars = ATLgetArgument(ATAgetArgument(spec,1),1);
  ATermList vals = ATLgetArgument(ATAgetArgument(spec,1),0);
  ATermList l = ATmakeList0();

  for (; !ATisEmpty(vars); vars=ATgetNext(vars),vals=ATgetNext(vals))
  {
    ATermAppl v = ATAgetFirst(vars);
    l = ATinsert(l,
                 (ATerm) static_cast<ATermAppl>(mcrl2::data::assignment(
                       mcrl2::data::variable(mcrl2::core::identifier_string(ATAgetArgument(v,0)),mcrl2::data::basic_sort(mcrl2::core::identifier_string(ATAgetArgument(v,1)))),
                       mcrl2::data::data_expression(dataterm2ATermAppl(ATAgetFirst(vals),ATmakeList0())))
                                               )
                );
  }

  return ATreverse(l);
}



/*****************************************************
 ******************* Main function *******************
 *****************************************************/

ATermAppl translate(ATermAppl spec, bool convert_bools, bool convert_funcs)
{
  assert(is_mCRL_spec(spec));
  ATermAppl sort_spec,cons_spec,map_spec,data_eqn_spec,data_spec,act_spec,lps,init;
  ATermList ids;

  ids = ATmakeList0();
  remove_bools = convert_bools;
  remove_standard_functions = convert_funcs;
  has_func_T = false;
  typelist = ATmakeList0();
  // ATprotectList(&typelist);

  mCRL2log(verbose) << "converting sort declarations..." << std::endl;
  sort_spec = gsMakeSortSpec(convert_sorts(spec,&ids));

  mCRL2log(verbose) << "converting constructor function declarations..." << std::endl;
  cons_spec = gsMakeConsSpec(convert_cons(spec,&ids));

  mCRL2log(verbose) << "converting mapping declarations..." << std::endl;
  map_spec = gsMakeMapSpec(convert_maps(spec,&ids));

  mCRL2log(verbose) << "converting data equations..." << std::endl;
  data_eqn_spec = gsMakeDataEqnSpec(convert_datas(spec,&ids));

  data_spec = gsMakeDataSpec(sort_spec, cons_spec, map_spec, data_eqn_spec);

  mCRL2log(verbose) << "converting initial LPE state..." << std::endl;
  init = gsMakeLinearProcessInit(convert_init(spec,&ids));

  mCRL2log(verbose) << "converting LPE..." << std::endl;
  lps = convert_lps(spec,&ids);

  mCRL2log(verbose) << "constructing action declarations..." << std::endl;
  act_spec = gsMakeActSpec(get_lps_acts(lps,&ids));

  ATermAppl r = gsMakeLinProcSpec(data_spec, act_spec, gsMakeGlobVarSpec(ATmakeList0()), lps, init);

  ATermList substs = ATmakeList0();

  if (convert_bools)
  {
    substs = ATinsert(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("F#",mcrl2::data::sort_bool::bool_())),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::false_())
                      )
                     );
    substs = ATinsert(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("T#", mcrl2::data::sort_bool::bool_())),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::true_())
                      )
                     );
  }

  if (convert_funcs)
  {
    ATermAppl bool_func_sort = mcrl2::data::sort_bool::and_().sort();

    substs = ATinsert(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("and#Bool#Bool",mcrl2::data::sort_expression(bool_func_sort))),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::and_())
                      )
                     );

    mcrl2::data::sort_expression s_bool(mcrl2::data::sort_bool::bool_());
    for (ATermList l=ATLgetArgument(sort_spec,0); !ATisEmpty(l); l=ATgetNext(l))
    {
      mcrl2::data::sort_expression s(ATAgetFirst(l));
      const char* sort_name = ATgetName(ATgetAFun(ATAgetArgument(s,0)));
      substs = ATinsert(substs,(ATerm) gsMakeSubst(
                          (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol(std::string("eq#")+sort_name+"#"+sort_name,
                              mcrl2::data::make_function_sort(s,s,s_bool))),
                          (ATerm) static_cast<ATermAppl>(mcrl2::data::equal_to(s))
                        ));
    }
  }

  r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);


  substs = get_substs(ids);

  r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);


  // ATunprotectList(&typelist);

  return r;
}
