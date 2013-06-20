// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/lps/typecheck.h"

using namespace atermpp;
using namespace mcrl2;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;
using namespace mcrl2::lps;
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


lps::action mcrl2::lps::action_type_checker::RewrAct(const std::map<core::identifier_string,sort_expression> &Vars, const lps::untyped_action &act)
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



lps::action mcrl2::lps::action_type_checker::TraverseAct(const std::map<core::identifier_string,sort_expression> &Vars, const lps::untyped_action &ma)
{
  return RewrAct(Vars,ma);
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

lps::multi_action mcrl2::lps::action_type_checker::operator()(const lps::untyped_multi_action &ma)
{
  try
  {
    action_list r;

    for (auto l=ma.actions().begin(); l!=ma.actions().end(); ++l)
    {
      untyped_action o= *l;
      const  std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      r.push_front(TraverseAct(NewDeclaredVars,o));
    }
// TODO: check if this is still needed
//    if (ma.has_time())
//    {
//      const std::map<core::identifier_string,sort_expression> Vars;
//      data_expression time=static_cast<data_type_checker>(*this)(ma.time(),Vars);
//      return multi_action(reverse(r),time);
//    }
    return lps::multi_action(reverse(r));
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
    const basic_sort& bsort = core::static_down_cast<const basic_sort&>(*i);
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
    lps::untyped_action Left(atermpp::aterm_cast<atermpp::aterm>(Rule.lhs()));

    //extra check requested by Tom: actions in the LHS can only come from the LPS
    actions.swap(actions_from_lps);
    action new_action_at_the_left=TraverseAct(DeclaredVars,Left);
    actions_from_lps.swap(actions);

    data_expression Cond=Rule.condition();
    TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Cond,sort_bool::bool_());

    action_rename_rule_rhs Right=Rule.rhs();
    if (!Right.is_delta() && !Right.is_tau())
    {
      Right = TraverseAct(DeclaredVars, atermpp::aterm_cast<lps::untyped_action>(Right.act()));
    }

    new_rules.push_back(action_rename_rule(VarList,Cond,new_action_at_the_left,Right));
  }

  mCRL2log(debug) << "type checking transform VarConstTypeData phase finished" << std::endl;

  return action_rename_specification(data_spec,action_labels,new_rules);
}
