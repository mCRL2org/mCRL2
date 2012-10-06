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
  return spec.function() == AFun("spec2gen", 2).number();
}


static bool remove_sort_decl(ATermAppl sort)
{
  return remove_bools && "Bool"==sort.function().name();
}

static bool remove_func_decl(ATermAppl func)
{
  const std::string name = func(0).function().name();
  if (remove_bools && ("T#"==name || "F#"==name))
  {
    return true;
  }
  if (remove_standard_functions)
  {
    if ("and#Bool#Bool"==name)
    {
      return true;
    }
    else if ("eq#"==name.substr(0,2) && name.size()>2)
    {
      std::string sorts=name.substr(3);
      size_t second_hash_position=sorts.find_first_of("#");
      std::string first_sort=name.substr(3,second_hash_position);
      if (name.size()>second_hash_position)
      {  
        sorts=name.substr(second_hash_position);
        if (sorts==first_sort)
        {
          return true;
        }
      }
    }
  }
  return false;
}


static void add_id(ATermList* ids, ATermAppl id)
{
  if (std::find(ids->begin(),ids->end(),(ATerm) id) == ids->end())
  {
    *ids = (*ids)+push_front<aterm>(aterm_list(),id);
  }
}

static bool is_domain(ATermList args, ATermAppl sort)
{
  if (!is_basic_sort(mcrl2::data::sort_expression(sort)))
  {
    ATermList dom = aterm_cast<aterm_list>(sort(0));
    if (args.size() != dom.size())
    {
      return false;
    }
    else
    {
      for (; !dom.empty(); dom=dom.tail(),args=args.tail())
      {
        if (static_cast<ATermAppl>(mcrl2::data::data_expression(aterm_cast<aterm_appl>(args.front())).sort())!=dom.front())
        {
          return false;
        }
      }
      return true;
    }
  }
  if (args.empty())
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
  for (ATermList l=types; !l.empty(); l=l.tail())
  {
    if (a.function().name()==(aterm_cast<aterm_appl>(l.front()))(0).function().name())
    {
      if (is_domain(args,aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(1))))
      {
        return aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(1));
      }
    }
  }

  return ATermAppl();
}

static ATermAppl dataterm2ATermAppl(ATermAppl t, ATermList args)
{
  using namespace mcrl2::data;

  ATermAppl t2 = aterm_appl(AFun(t.function().name(),0));

  if (t.size()==0)
  {
    ATermAppl r = find_type(t,aterm_list(),args);
    if (!r.defined())
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
    ATermList m;
    for (aterm_appl::const_iterator l=t.begin(); l!=t.end(); ++l)
    {
      m = m+push_front<aterm>(aterm_list(),dataterm2ATermAppl(aterm_cast<aterm_appl>(*l),args));
    }

    return application(mcrl2::data::function_symbol(mcrl2::core::identifier_string(t2),sort_expression(find_type(t,m))), atermpp::convert<data_expression_list>(atermpp::aterm_list(m)));
  }
}

static ATermList get_lps_acts(ATermAppl lps, ATermList* ids)
{
  ATermList acts;
  ATermList sums = aterm_cast<aterm_list>(lps(1));
  for (; !sums.empty(); sums=sums.tail())
  {
    if (!(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(sums.front())(2))(0))).empty())
    {
      ATermAppl a = aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(sums.front())(2))(0)).front())(0));
      if (std::find(acts.begin(),acts.end(),a) == acts.end())
      {
        acts = push_front<aterm>(acts,a);
        add_id(ids,aterm_cast<aterm_appl>(a(0)));
      }
    }
  }

  return reverse(acts);
}

static ATermList get_substs(ATermList ids)
{
  std::set < ATerm > used;
  ATermList substs;

  used.insert((ATerm) aterm_appl(AFun("if",0)));

  for (; !ids.empty(); ids=ids.tail())
  {
    char s[100], t1[100], *t;
    t=&t1[0];
    strncpy(t,aterm_cast<aterm_appl>(ids.front()).function().name().c_str(),100);
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
    ATermAppl new_id;
    while (!is_user_identifier(s) ||
           (used.count((ATerm)(new_id = aterm_appl(AFun(s,0)))) >0))
    {
      sprintf(t,"%zu",i);
      i++;
    }

    used.insert((ATerm) new_id);

    substs = push_front<aterm>(substs,gsMakeSubst(ids.front(),(ATerm) new_id));
  }

  return substs;
}




/*****************************************************
 ************* Main conversion functions *************
 *****************************************************/

static ATermList convert_sorts(ATermAppl spec, ATermList* ids)
{
  ATermList sorts = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(spec(0))(0))(0));
  ATermList r;

  r = aterm_list();
  for (; !sorts.empty(); sorts=sorts.tail())
  {
    if (!remove_sort_decl(aterm_cast<aterm_appl>(sorts.front())))
    {
      add_id(ids,aterm_cast<aterm_appl>(sorts.front()));
      r = push_front<aterm>(r,mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(sorts.front()))));
    }
  }

  return reverse(r);
}

static ATermList convert_funcs(ATermList funcs, ATermList* ids, bool funcs_are_cons = false)
{
  ATermList r,l, sorts;
  ATermAppl sort;

  r = aterm_list();
  for (; !funcs.empty(); funcs=funcs.tail())
  {
    if (funcs_are_cons && "T#"==aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(0)).function().name())
    {
      has_func_T = true;
    }

    l = reverse(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(funcs.front())(1)));
    sorts = aterm_list();
    sort = static_cast<ATermAppl>(mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(2)))));
    for (; !l.empty(); l=l.tail())
    {
      sorts = push_front<aterm>(sorts, mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(l.front()))));
    }
    if (!sorts.empty())
    {
      sort = mcrl2::data::function_sort(atermpp::convert<mcrl2::data::sort_expression_list>(sorts), mcrl2::data::sort_expression(sort));
    }

    ATerm f = (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(0))),
                            mcrl2::data::sort_expression(sort)));

    if (!remove_func_decl(aterm_cast<aterm_appl>(funcs.front())))
    {
      if (funcs_are_cons && remove_bools && "Bool"==aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(2)).function().name())
      {
        std::string name(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(0)).function().name());
        mCRL2log(error) << "constructor " << name.substr(0, name.find_last_of('#')) << " of sort Bool found (only T and F are allowed)" << std::endl;
        exit(1);
      }
      add_id(ids,aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(funcs.front())(0)));
      r = push_front<aterm>(r,f);
    }

    typelist = push_front<aterm>(typelist,f);
  }

  return reverse(r);
}

static ATermList convert_cons(ATermAppl spec, ATermList* ids)
{
  return convert_funcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(spec(0))(0))(1)),ids, true);
}

static ATermList convert_maps(ATermAppl spec, ATermList* ids)
{
  return convert_funcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(spec(0))(0))(2)),ids);
}

static ATermList convert_datas(ATermAppl spec, ATermList* ids)
{
  ATermList eqns = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(0))(1));
  ATermList l,args,r;
  ATermAppl lhs,rhs;

  r = aterm_list();
  for (; !eqns.empty(); eqns=eqns.tail())
  {
    l = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(eqns.front())(0));
    args = aterm_list();
    for (; !l.empty(); l=l.tail())
    {
      args = args+push_front<aterm>(aterm_list(),mcrl2::data::variable(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(0))),
                    mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(1))))));
      add_id(ids,aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(0)));
    }
    ATermAppl lhs_before_translation=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(eqns.front())(1));
    if (lhs_before_translation.function().name()!="eq#Bool#Bool")
    {
      // No match.
      lhs = dataterm2ATermAppl(lhs_before_translation,args);
      rhs = dataterm2ATermAppl(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(eqns.front())(2)),args);
      r = r+push_front<aterm>(aterm_list(),gsMakeDataEqn(args,sort_bool::true_(),lhs,rhs));
    }
  }

  return r;
}

static ATermAppl convert_lps(ATermAppl spec, ATermList* ids)
{
  ATermList vars = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(1))(1));
  ATermList sums = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(1))(2));
  ATermList pars;
  ATermList smds;

  for (; !vars.empty(); vars=vars.tail())
  {
    ATermAppl v = aterm_cast<aterm_appl>(vars.front());
    pars = push_front<aterm>(pars,
                    (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(v(0))),
                                    mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(v(1))))))
                   );
    add_id(ids,aterm_cast<aterm_appl>(v(0)));
  }
  pars = reverse(pars);

  for (; !sums.empty(); sums=sums.tail())
  {
    ATermAppl s = aterm_cast<aterm_appl>(sums.front());

    ATermList l = reverse(aterm_cast<aterm_list>(s(0)));
    ATermList m;
    for (; !l.empty(); l=l.tail())
    {
      m = push_front<aterm>(m,
                   (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(0))),
                               mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(1))))))
                  );
      add_id(ids,aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(0)));
    }

    l = aterm_cast<aterm_list>(s(0));
    ATermList o = pars;
    for (; !l.empty(); l=l.tail())
    {
      o = push_front<aterm>(o,
                   (ATerm) static_cast<ATermAppl>(mcrl2::data::variable(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(0))),
                        mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(l.front())(1))))))
                  );
    }
    ATermAppl c = dataterm2ATermAppl(aterm_cast<aterm_appl>(s(4)),o);
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

    l = reverse(aterm_cast<aterm_list>(s(2)));
    ATermList al;
    ATermList as;
    for (; !l.empty(); l=l.tail())
    {
      al = push_front<aterm>(al,
                    (ATerm) dataterm2ATermAppl(aterm_cast<aterm_appl>(l.front()),o)
                   );
      as = push_front<aterm>(as,(ATerm) static_cast<ATermAppl>(mcrl2::data::data_expression(aterm_cast<aterm_appl>(al.front())).sort()));
    }
    ATermAppl a = gsMakeAction(gsMakeActId(aterm_cast<aterm_appl>(s(1)),as),al);
    if (as.empty() && "tau"==aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(a(0))(0)).function().name())
    {
      a = gsMakeMultAct(aterm_list());
    }
    else
    {
      a = gsMakeMultAct(push_front<aterm>(aterm_list(),a));
    }

    l = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(s(3))(0));
    ATermList o2 = pars;
    ATermList n;
    for (; !l.empty(); l=l.tail(),o2=o2.tail())
    {
      ATermAppl par = aterm_cast<aterm_appl>(o2.front());
      ATermAppl val = dataterm2ATermAppl(aterm_cast<aterm_appl>(l.front()),o);
      if (par!=val)
      {
        n = push_front<aterm>(n,(ATerm) static_cast<ATermAppl>(mcrl2::data::assignment(mcrl2::data::variable(par),mcrl2::data::data_expression(val))));
      }
    }
    n = reverse(n);

    smds = push_front<aterm>(smds,gsMakeLinearProcessSummand(m,c,a,gsMakeNil(),n));
  }

  return gsMakeLinearProcess(pars,reverse(smds));
}

static ATermList convert_init(ATermAppl spec, ATermList* /*ids*/)
{
  ATermList vars = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(1))(1));
  ATermList vals = aterm_cast<aterm_list>(aterm_cast<aterm_appl>(spec(1))(0));
  ATermList l;

  for (; !vars.empty(); vars=vars.tail(),vals=vals.tail())
  {
    ATermAppl v = aterm_cast<aterm_appl>(vars.front());
    l = push_front<aterm>(l,
                 (ATerm) static_cast<ATermAppl>(mcrl2::data::assignment(
                       mcrl2::data::variable(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(v(0))),mcrl2::data::basic_sort(mcrl2::core::identifier_string(aterm_cast<aterm_appl>(v(1))))),
                       mcrl2::data::data_expression(dataterm2ATermAppl(aterm_cast<aterm_appl>(vals.front()),aterm_list())))
                                               )
                );
  }

  return reverse(l);
}



/*****************************************************
 ******************* Main function *******************
 *****************************************************/

ATermAppl translate(ATermAppl spec, bool convert_bools, bool convert_funcs)
{
  assert(is_mCRL_spec(spec));
  ATermAppl sort_spec,cons_spec,map_spec,data_eqn_spec,data_spec,act_spec,lps,init;
  ATermList ids;

  ids = aterm_list();
  remove_bools = convert_bools;
  remove_standard_functions = convert_funcs;
  has_func_T = false;
  typelist = aterm_list();
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

  ATermAppl r = gsMakeLinProcSpec(data_spec, act_spec, gsMakeGlobVarSpec(aterm_list()), lps, init);

  ATermList substs;

  if (convert_bools)
  {
    substs = push_front<aterm>(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("F#",mcrl2::data::sort_bool::bool_())),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::false_())
                      )
                     );
    substs = push_front<aterm>(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("T#", mcrl2::data::sort_bool::bool_())),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::true_())
                      )
                     );
  }

  if (convert_funcs)
  {
    ATermAppl bool_func_sort = mcrl2::data::sort_bool::and_().sort();

    substs = push_front<aterm>(substs,
                      (ATerm) gsMakeSubst(
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol("and#Bool#Bool",mcrl2::data::sort_expression(bool_func_sort))),
                        (ATerm) static_cast<ATermAppl>(mcrl2::data::sort_bool::and_())
                      )
                     );

    mcrl2::data::sort_expression s_bool(mcrl2::data::sort_bool::bool_());
    for (ATermList l=aterm_cast<aterm_list>(sort_spec(0)); !l.empty(); l=l.tail())
    {
      mcrl2::data::sort_expression s(aterm_cast<aterm_appl>(l.front()));
      const char* sort_name = aterm_cast<aterm_appl>(s(0)).function().name().c_str();
      substs = push_front<aterm>(substs,(ATerm) gsMakeSubst(
                          (ATerm) static_cast<ATermAppl>(mcrl2::data::function_symbol(std::string("eq#")+sort_name+"#"+sort_name,
                              mcrl2::data::make_function_sort(s,s,s_bool))),
                          (ATerm) static_cast<ATermAppl>(mcrl2::data::equal_to(s))
                        ));
    }
  }

  r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);


  substs = get_substs(ids);

  r = (ATermAppl) gsSubstValues(substs,(ATerm) r,true);

  return r;
}
