// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/typecheck.h"
#include "mcrl2/modal_formula/monotonicity.h"


using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;
using namespace mcrl2::regular_formulas;
using namespace mcrl2::action_formulas;
using namespace mcrl2::data;


static action MakeAction(
               const identifier_string &Name,
               const sort_expression_list &FormParList, 
               const data_expression_list &FactParList)
{
  return action(action_label(Name,FormParList),FactParList);
}

static std::map<identifier_string,sort_expression> list_minus(const std::map<identifier_string,sort_expression> &l, const std::map<identifier_string,sort_expression> &m)
{
  std::map<identifier_string,sort_expression> n;
  for (std::map<identifier_string,sort_expression>::const_reverse_iterator i=l.rbegin(); i!=l.rend(); ++i)
  {
    if (m.count(i->first)==0)
    {
      n.insert(*i);
    }
  }
  return n;
}


action mcrl2::lps::action_type_checker::RewrAct(const std::map<core::identifier_string,sort_expression> &Vars, const process::parameter_identifier &act)
{
  action Result;
  core::identifier_string Name(act.name());
  term_list<sort_expression_list> ParList;

  bool action=false;

  const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=actions.find(Name);

  if (j!=actions.end())
  {
    ParList=j->second;
    action=true;
  }
  else
  {
    throw mcrl2::runtime_error("action " + std::string(Name) + " not declared");
  }

  assert(!ParList.empty());

  size_t nFactPars=act.arguments().size();

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    term_list <sort_expression_list> NewParList;
    for (; !ParList.empty(); ParList=ParList.tail())
    {
      sort_expression_list Par=ParList.front();
      if (Par.size()==nFactPars)
      {
        NewParList.push_front(Par);
      }
    }
    ParList=reverse(NewParList);
  }

  if (ParList.empty())
  {
    throw mcrl2::runtime_error("no action " + std::string(Name)
                    + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                    + " is declared (while typechecking " + pp(act) + ")");
  }

  if (ParList.size()==1)
  {
    Result=MakeAction(Name,ParList.front(),aterm_cast<data_expression_list>(act.arguments()));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=MakeAction(Name,GetNotInferredList(ParList),aterm_cast<data_expression_list>(act.arguments()));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  sort_expression_list PosTypeList=Result.label().sorts(); 

  data_expression_list NewPars;
  sort_expression_list NewPosTypeList;
  for (data_expression_list::const_iterator Pars=act.arguments().begin(); Pars!=act.arguments().end(); ++Pars,PosTypeList=PosTypeList.tail())
  {
    data_expression Par= *Pars;
    sort_expression PosType= PosTypeList.front();

    sort_expression NewPosType;
    try
    {
      NewPosType=TraverseVarConsTypeD(Vars,Vars,Par,PosType); 
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + pp(Par) + " as type " + pp(ExpandNumTypesDown(PosType)) + " (while typechecking " + pp(act) + ")");
    }
    NewPars.push_front(Par);
    NewPosTypeList.push_front(NewPosType);
  }
  NewPars=reverse(NewPars);
  NewPosTypeList=reverse(NewPosTypeList);

  std::pair<bool,sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
  PosTypeList=p.second;

  if (!p.first)
  {
    PosTypeList=Result.label().sorts();
    data_expression_list Pars=NewPars;
    NewPars=data_expression_list();
    sort_expression_list CastedPosTypeList;
    for (data_expression_list::const_iterator Pars=NewPars.begin(); Pars!=NewPars.end(); ++Pars,PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
    {
      data_expression Par= *Pars;
      sort_expression PosType= PosTypeList.front();
      sort_expression NewPosType=NewPosTypeList.front();

      sort_expression CastedNewPosType;
      try
      { 
        CastedNewPosType=UpCastNumericType(PosType,NewPosType,Par);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + pp(NewPosType) + " to " + pp(PosType) + "(while typechecking " + pp(Par) + " in " + pp(act));
      }

      NewPars.push_front(Par);
      CastedPosTypeList.push_front(CastedNewPosType);
    }
    NewPars=reverse(NewPars);
    NewPosTypeList=reverse(CastedPosTypeList);

    std::pair<bool,sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
    PosTypeList=p.second;

    if (!p.first)
    {
      throw mcrl2::runtime_error("no action " + std::string(Name) + "with type " + pp(NewPosTypeList) + " is declared (while typechecking " + pp(act) + ")");
    }
  }

  if (IsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous action " + std::string(Name));
  }

  Result=MakeAction(Name,PosTypeList,NewPars);
  return Result;
}



action mcrl2::lps::action_type_checker::TraverseAct(const std::map<core::identifier_string,sort_expression> &Vars, const process::parameter_identifier &ma)
// last argument should have type action; but this does not allow for a ParamId.
{
  size_t n = ma.size();
  if (n==0)
  {
    return ma;
  }

  if (gsIsParamId(ma))
  {
    return RewrAct(Vars,ma);
  }

  throw mcrl2::runtime_error("Internal error. Action " + pp(ma) + " fails to match process.");
}


void mcrl2::lps::action_type_checker::ReadInActs(const action_label_list &Acts)
{
  for (lps::action_label_list::const_iterator i=Acts.begin(); i!=Acts.end(); ++i)
  {
    action_label Act= *i;
    core::identifier_string ActName=Act.name();
    sort_expression_list ActType=Act.sorts();

    IsSortExprListDeclared(ActType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=actions.find(ActName);
    term_list<sort_expression_list> Types;
    if (j==actions.end())
    {
      Types=make_list<sort_expression_list>(ActType);
    }
    else
    {
      Types=j->second;
      // the table actions contains a list of types for each
      // action name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
    
      if (InTypesL(ActType, Types))
      {
        throw mcrl2::runtime_error("double declaration of action " + std::string(ActName));
      }
      else
      {
        Types=Types+make_list<sort_expression_list>(ActType);
      }
    }
    actions[ActName]=Types;
  }
}


mcrl2::lps::action_type_checker::action_type_checker(
            const data::data_specification &data_spec, 
            const action_label_list& action_decls)
  : data_type_checker(data_spec)
{
  mCRL2log(debug) << "type checking multiaction..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  try
  {
    ReadInActs(action_decls);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading from LPS failed");
  }
}

multi_action mcrl2::lps::action_type_checker::operator()(const multi_action &ma)
{
  try
  {
    action_list r;
    
    for (action_list::const_iterator l=ma.actions().begin(); l!=ma.actions().end(); ++l)
    {
      action o= *l;
      assert(gsIsParamId(o));
      const  std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      o=TraverseAct(NewDeclaredVars,o);
      r.push_front(o);
    }
    if (ma.has_time())
    { 
      const std::map<core::identifier_string,sort_expression> Vars;
      data_expression time=static_cast<data_type_checker>(*this)(ma.time(),Vars);
      return multi_action(reverse(r),time);
    }
    return multi_action(reverse(r));
  }
  catch (mcrl2::runtime_error &e)
  { 
    throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of multiaction failed (" + pp(ma) + ")");
  }
}

/*************************   Here starts the action_rename_typechecker  ************************************/

action_rename_specification mcrl2::lps::action_type_checker::operator()(const action_rename_specification &ar_spec)
{
  mCRL2log(verbose) << "type checking action rename specification..." << std::endl;

  //check precondition

  mCRL2log(debug) << "type checking phase started" << std::endl;

  std::map<core::identifier_string,term_list<sort_expression_list> > actions_from_lps;

  data_specification data_spec = ar_spec.data();
  
  std::map<core::identifier_string,sort_expression> LPSSorts=defined_sorts; // remember the sorts from the LPS.
  sort_expression_vector sorts=data_spec.user_defined_sorts();
  for (sort_expression_vector::const_iterator i=sorts.begin(); i!=sorts.end(); ++i)
  {
    assert(is_basic_sort(*i));
    const basic_sort &bsort(*i);
    add_basic_sort(bsort);
    basic_sorts.insert(bsort.name());
  }

  alias_vector aliases=data_spec.user_defined_aliases();
  for (alias_vector::const_iterator i=aliases.begin(); i!=aliases.end(); ++i)
  {
    add_basic_sort(i->name());
    defined_sorts[i->name().name()]=i->reference();
  }

  mCRL2log(debug) << "type checking of action rename specification read-in phase of rename file sorts finished" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations
  const std::map<core::identifier_string,sort_expression> difference_sorts_set=list_minus(defined_sorts, LPSSorts);
  ReadInConstructors(difference_sorts_set.begin(),difference_sorts_set.end());

  ReadInFuncs(data_spec.user_defined_constructors(),data_spec.user_defined_mappings());
  
  TransformVarConsTypeData(data_spec);
  
  //Save the actions from LPS only for later use.
  actions_from_lps=actions;
  const action_label_list action_labels=ar_spec.action_labels();
  ReadInActs(action_labels);
  
  // Result=gstcFoldSortRefs(Result);

  // now the action renaming rules themselves will be typechecked.
  std::vector <action_rename_rule> ActionRenameRules=ar_spec.rules();
  std::vector <action_rename_rule> new_rules;

  std::map<core::identifier_string,sort_expression> DeclaredVars;
  std::map<core::identifier_string,sort_expression> FreeVars;

  for (std::vector <action_rename_rule>::const_iterator l=ActionRenameRules.begin(); l!=ActionRenameRules.end(); ++l)
  {
    action_rename_rule Rule= *l;

    variable_list VarList=Rule.variables();
    if (!VarsUnique(VarList))
    {
      throw mcrl2::runtime_error("the variables " + pp(VarList) + " in action rename rule " + pp(Rule.condition()) + " -> " + 
                               pp(Rule.lhs()) + " => " + (Rule.rhs().is_tau()?"tau":(Rule.rhs().is_delta()?"delta":pp(Rule.rhs().act()))) + " are not unique");
    }

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    AddVars2Table(DeclaredVars,VarList,NewDeclaredVars);
    
    DeclaredVars=NewDeclaredVars;
    process::parameter_identifier Left=Rule.lhs();
    
    //extra check requested by Tom: actions in the LHS can only come from the LPS
    actions.swap(actions_from_lps);
    action new_action_at_the_left=TraverseAct(DeclaredVars,Left);
    actions_from_lps.swap(actions);

    data_expression Cond=Rule.condition();
    TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Cond,sort_bool::bool_());

    action_rename_rule_rhs Right=Rule.rhs();
    if (!Right.is_delta() && !Right.is_tau())
    { 
      Right=TraverseAct(DeclaredVars,Right.act());
    }

    new_rules.push_back(action_rename_rule(VarList,Cond,new_action_at_the_left,Right));
  }

  mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;
  
  return action_rename_specification(data_spec,action_labels,new_rules);
}



/*************************   Here starts the state_formula_typechecker  ************************************/


regular_formula mcrl2::state_formulas::state_formula_type_checker::TraverseRegFrm(
          const std::map<core::identifier_string,sort_expression> &Vars, 
          const regular_formula &RegFrm)
{
  mCRL2log(debug) << "TraverseRegFrm: " << pp(RegFrm) << "" << std::endl;
  if (regular_formulas::is_nil(RegFrm))
  {
    return RegFrm;
  }

  if (is_seq(RegFrm))
  {
    const seq t=aterm_cast<const seq>(RegFrm);
    return seq(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
  }

  if (is_alt(RegFrm))
  {
    const alt t=aterm_cast<const alt>(RegFrm);
    return alt(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
  }

  if (is_trans(RegFrm))
  {
    const trans t=aterm_cast<const trans>(RegFrm);
    return trans(TraverseRegFrm(Vars,t.operand()));
  }

  if (is_trans_or_nil(RegFrm))
  {
    const trans t=aterm_cast<const trans>(RegFrm);
    return trans_or_nil(TraverseRegFrm(Vars,t.operand()));
  }

  if (is_action_formula(RegFrm))
  {

    return TraverseActFrm(Vars, RegFrm);
  }

  throw mcrl2::runtime_error("Internal error. The regularformula " + pp(RegFrm) + " fails to match any known form in typechecking case analysis");
}

action_formula mcrl2::state_formulas::state_formula_type_checker::TraverseActFrm(
             const std::map<core::identifier_string,sort_expression> &Vars, 
             const action_formula &ActFrm)
{
  using namespace action_formulas;
  mCRL2log(debug) << "TraverseActFrm: " << pp(ActFrm) << std::endl;

  if (action_formulas::is_true(ActFrm) || action_formulas::is_false(ActFrm))
  {
    return ActFrm;
  }

  if (action_formulas::is_not(ActFrm))
  {
    const not_& f=aterm_cast<const not_>(ActFrm);
    return action_formulas::not_(TraverseActFrm(Vars,f.operand()));
  }

  if (action_formulas::is_and(ActFrm))
  {
    const action_formulas::and_& t=aterm_cast<action_formulas::and_>(ActFrm);
    return action_formulas::and_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
  }

  if (action_formulas::is_or(ActFrm))
  {
    const action_formulas::or_& t=aterm_cast<action_formulas::or_>(ActFrm);
    return action_formulas::or_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
  }

  if (action_formulas::is_imp(ActFrm))
  {
    const action_formulas::imp& t=aterm_cast<action_formulas::imp>(ActFrm);
    return action_formulas::imp(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
  }

  if (action_formulas::is_forall(ActFrm))
  {
    const action_formulas::forall& t=aterm_cast<const action_formulas::forall>(ActFrm);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    const variable_list& VarList=t.variables();
    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,VarList,NewVars);
    
    return action_formulas::forall(VarList, TraverseActFrm(NewVars,t.body()));
  }

  if (action_formulas::is_exists(ActFrm))
  {
    const action_formulas::exists& t=aterm_cast<const action_formulas::exists>(ActFrm);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    const variable_list& VarList=t.variables();
    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,VarList,NewVars);
    
    return action_formulas::exists(VarList, TraverseActFrm(NewVars,t.body()));
  }

  if (action_formulas::is_at(ActFrm))
  {
    const action_formulas::at& t=aterm_cast<const action_formulas::at>(ActFrm);
    action_formula NewArg1=TraverseActFrm(Vars,t.operand());

    data_expression Time=t.time_stamp();
    sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(sort_real::real_()));

    sort_expression temp;
    if (!TypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      sort_expression CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
      }
    }
    return action_formulas::at(NewArg1,Time);
  }

  if (is_multi_action(ActFrm))
  {
    const multi_action ma(ActFrm);
    action_list r;
    for (action_list::const_iterator l=ma.actions().begin(); l!=ma.actions().end(); ++l)
    {
      action o= *l;
      assert(gsIsParamId(o));
      o=RewrAct(Vars,o);
      assert(!gsIsParamId(o));
      r.push_front(o);
    }
    return multi_action(reverse(r));
  }

  if (is_data_expression(ActFrm))
  {
    data_expression d(ActFrm);
    sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return d;
  }

  throw mcrl2::runtime_error("Internal error. The action formula " + pp(ActFrm) + " fails to match any known form in typechecking case analysis");
}


state_formula mcrl2::state_formulas::state_formula_type_checker::TraverseStateFrm(
                const std::map<core::identifier_string,sort_expression> &Vars, 
                const std::map<core::identifier_string,sort_expression_list> &StateVars, 
                const state_formula &StateFrm)
{
  using namespace state_formulas;

  mCRL2log(debug) << "TraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

  if (state_formulas::is_true(StateFrm) || state_formulas::is_false(StateFrm) || state_formulas::is_delay(StateFrm) || state_formulas::is_yaled(StateFrm))
  {
    return StateFrm;
  }

  if (state_formulas::is_not(StateFrm))
  {
    const  not_& t=aterm_cast<const not_>(StateFrm);
    return not_(TraverseStateFrm(Vars,StateVars,t.operand()));
  }

  if (state_formulas::is_and(StateFrm))
  {
    const and_& t=aterm_cast<const and_>(StateFrm);
    return and_(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
  }

  if (state_formulas::is_or(StateFrm))
  {
    const or_& t=aterm_cast<const or_>(StateFrm);
    return or_(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
  }

  if (state_formulas::is_imp(StateFrm))
  {
    const imp& t=aterm_cast<const imp>(StateFrm);
    return imp(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
  }

  if (state_formulas::is_forall(StateFrm))
  {
    const forall& t=aterm_cast<const forall>(StateFrm);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,t.variables(),NewVars);

    return forall(t.variables(),TraverseStateFrm(NewVars,StateVars,t.body()));
  }

  if (state_formulas::is_exists(StateFrm))
  {
    const exists& t=aterm_cast<const exists>(StateFrm);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,t.variables(),NewVars);

    return exists(t.variables(),TraverseStateFrm(NewVars,StateVars,t.body()));
  }

  if (is_may(StateFrm))
  {
    const may& f=aterm_cast<const may>(StateFrm);
    return may(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,StateVars,f.operand()));
  }

  if (is_must(StateFrm))
  {
    const must& f=aterm_cast<const must>(StateFrm);
    return must(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,StateVars,f.operand()));
  }

  if (state_formulas::is_delay_timed(StateFrm))
  {
    const delay_timed& f=aterm_cast<const delay_timed>(StateFrm);
    data_expression Time=f.time_stamp();
    sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(sort_real::real_()));

    sort_expression temp;
    if (!TypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      sort_expression CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
      }
    }
    return delay_timed(Time);
  }

  if (state_formulas::is_yaled_timed(StateFrm))
  {
    const yaled_timed& f=aterm_cast<const yaled_timed>(StateFrm);
    data_expression Time=f.time_stamp();
    sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(sort_real::real_()));

    sort_expression temp;
    if (!TypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      sort_expression CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
      }
    }
    return yaled_timed(Time);
  }

  if (state_formulas::is_variable(StateFrm))
  {
    state_formulas::variable v=aterm_cast<const state_formulas::variable>(StateFrm);
    core::identifier_string StateVarName=v.name();
    std::map<core::identifier_string,sort_expression_list>::const_iterator i=StateVars.find(StateVarName);
    if (i==StateVars.end())
    {
      throw mcrl2::runtime_error("undefined state variable " + to_string(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
    }
    sort_expression_list TypeList=i->second;

    const data_expression_list Pars=v.arguments();
    if (TypeList.size()!=Pars.size())
    {
      throw mcrl2::runtime_error("incorrect number of parameters for state variable " + pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
    }

    data_expression_list r;
    
    for (data_expression_list::const_iterator i=Pars.begin(); i!=Pars.end(); ++i, TypeList=TypeList.tail())
    {
      data_expression Par= *i;
      sort_expression ParType=TypeList.front();
      sort_expression NewParType;
      try 
      { 
        NewParType=TraverseVarConsTypeD(Vars,Vars,Par,ExpandNumTypesDown(ParType));
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm));
      }

      sort_expression temp;
      if (!TypeMatchA(ParType,NewParType,temp))
      {
        //upcasting
        try
        {
          NewParType=UpCastNumericType(ParType,NewParType,Par);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(Par) + " to type " + pp(ParType) + " (typechecking state formula " + pp(StateFrm) + ")");
        }
      }

      r.push_front(Par);
    }

    return state_formulas::variable(StateVarName,reverse(r));

  }

  if (state_formulas::is_nu(StateFrm))
  {
    const nu& f=aterm_cast<const nu>(StateFrm);
    std::map<core::identifier_string,sort_expression_list> CopyStateVars(StateVars);

    // Make the new state variable:
    std::map<core::identifier_string,sort_expression> FormPars;
    assignment_list r;
    sort_expression_list t;
    for (assignment_list::const_iterator l=f.assignments().begin(); l!=f.assignments().end(); ++l)
    {
      const assignment& o= *l;

      core::identifier_string VarName=o.lhs().name();
      if (FormPars.count(VarName)>0)
      {
        throw mcrl2::runtime_error("non-unique formal parameter " + pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
      }

      sort_expression VarType=o.lhs().sort();
      try
      {
        IsSortExprDeclared(VarType);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
      }

      FormPars[VarName]=VarType;

      data_expression VarInit=o.rhs();
      sort_expression VarInitType;
      try
      {
        VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
      }

      sort_expression temp;
      if (!TypeMatchA(VarType,VarInitType,temp))
      {
        //upcasting
        try 
        {
          VarInitType=UpCastNumericType(VarType,VarInitType,VarInit);
        }
        catch (mcrl2::runtime_error &e)
        { 
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
        }
      }

      r.push_front(assignment(o.lhs(),VarInit));
      t.push_front(VarType);
    }

    std::map<core::identifier_string,sort_expression> CopyVars(Vars);
    CopyVars.insert(FormPars.begin(),FormPars.end());
    

    CopyStateVars[f.name()]=reverse(t);
    
    try
    {
      return nu(f.name(),reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking TEMPORARILY PRINT AN ATERM " + to_string(f));
    }
  }

  if (state_formulas::is_mu(StateFrm))
  {
    const mu& f=aterm_cast<const mu>(StateFrm);
    std::map<core::identifier_string,sort_expression_list> CopyStateVars(StateVars);

    // Make the new state variable:
    std::map<core::identifier_string,sort_expression> FormPars;
    assignment_list r;
    sort_expression_list t;
    for (assignment_list::const_iterator l=f.assignments().begin(); l!=f.assignments().end(); ++l)
    {
      const assignment& o= *l;

      core::identifier_string VarName=o.lhs().name();
      if (FormPars.count(VarName)>0)
      {
        throw mcrl2::runtime_error("non-unique formal parameter " + pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
      }

      sort_expression VarType=o.lhs().sort();
      try
      {
        IsSortExprDeclared(VarType);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
      }

      FormPars[VarName]=VarType;

      data_expression VarInit=o.rhs();
      sort_expression VarInitType;
      try
      {
        VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
      }

      sort_expression temp;
      if (!TypeMatchA(VarType,VarInitType,temp))
      {
        //upcasting
        try 
        {
          VarInitType=UpCastNumericType(VarType,VarInitType,VarInit);
        }
        catch (mcrl2::runtime_error &e)
        { 
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
        }
      }

      r.push_front(assignment(o.lhs(),VarInit));
      t.push_front(VarType);
    }

    std::map<core::identifier_string,sort_expression> CopyVars(Vars);
    CopyVars.insert(FormPars.begin(),FormPars.end());
    

    CopyStateVars[f.name()]=reverse(t);
    
    try
    {
      return mu(f.name(),reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking TEMPORARILY PRINT AN ATERM " + to_string(f));
    }
  }

  if (is_data_expression(StateFrm))
  {
    data_expression d(StateFrm);
    sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return d;
  }

  throw mcrl2::runtime_error("Internal error. The state formula " + pp(StateFrm) + " fails to match any known form in typechecking case analysis");
}


mcrl2::state_formulas::state_formula_type_checker::state_formula_type_checker(
       const data::data_specification &data_spec, 
       const action_label_list& action_decls)
  :  lps::action_type_checker(data_spec,action_decls)
{
}
     

state_formula mcrl2::state_formulas::state_formula_type_checker::operator()(
            const state_formula &formula, 
            bool check_monotonicity)
{
  
  //check correctness of the state formula in state_formula using
  //the process specification or LPS in spec as follows:
  //1) determine the types of actions according to the definitions
  //   in spec
  //2) determine the types of data expressions according to the
  //   definitions in spec
  //3) check for name conflicts of data variable declarations in
  //   forall, exists, mu and nu quantifiers
  //4) check for monotonicity of fixpoint variables

  mCRL2log(verbose) << "type checking state formula..." << std::endl;

  std::map<core::identifier_string,sort_expression> Vars;
  std::map<core::identifier_string,sort_expression_list> StateVars;
  state_formula result=TraverseStateFrm(Vars,StateVars,formula);
  if (check_monotonicity && !is_monotonous(result))
  {
    throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(result));
  } 
  return result;
}

