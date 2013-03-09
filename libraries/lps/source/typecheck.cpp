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


action mcrl2::lps::multi_action_type_checker::RewrAct(const std::map<core::identifier_string,sort_expression> &Vars, const action &ProcTerm)
{
  action Result;
  core::identifier_string Name(ProcTerm[0]);
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

  size_t nFactPars=aterm_cast<aterm_list>(ProcTerm[1]).size();
  const std::string msg="action";

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
    throw mcrl2::runtime_error("no " + msg + " " + std::string(Name)
                    + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                    + " is declared (while typechecking " + pp(ProcTerm) + ")");
  }

  if (ParList.size()==1)
  {
    Result=MakeAction(Name,ParList.front(),aterm_cast<data_expression_list>(ProcTerm[1]));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=MakeAction(Name,GetNotInferredList(ParList),aterm_cast<data_expression_list>(ProcTerm[1]));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  sort_expression_list PosTypeList=aterm_cast<sort_expression_list>(aterm_cast<aterm_appl>(Result[0])[1]);

  data_expression_list NewPars;
  sort_expression_list NewPosTypeList;
  for (aterm_list Pars=aterm_cast<aterm_list>(ProcTerm[1]); !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
  {
    data_expression Par=Pars.front();
    sort_expression PosType=PosTypeList.front();

    aterm_appl NewPosType;
    try
    {
      NewPosType=TraverseVarConsTypeD(Vars,Vars,Par,PosType); 
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + pp(Par) + " as type " + pp(ExpandNumTypesDown(PosType)) + " (while typechecking " + pp(ProcTerm) + ")");
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
    PosTypeList=aterm_cast<sort_expression_list>(aterm_cast<aterm_appl>(Result[0])[1]);
    aterm_list Pars=NewPars;
    NewPars=data_expression_list();
    sort_expression_list CastedPosTypeList;
    for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
    {
      data_expression Par=Pars.front();
      sort_expression PosType=PosTypeList.front();
      sort_expression NewPosType=NewPosTypeList.front();

      aterm_appl CastedNewPosType;
      try
      { 
        CastedNewPosType=UpCastNumericType(PosType,NewPosType,Par);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + pp(NewPosType) + " to " + pp(PosType) + "(while typechecking " + pp(Par) + " in " + pp(ProcTerm));
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
      throw mcrl2::runtime_error("no " + msg + " " + std::string(Name) + "with type " + pp(NewPosTypeList) + " is declared (while typechecking " + pp(ProcTerm) + ")");
    }
  }

  if (IsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + std::string(Name));
  }

  Result=MakeAction(Name,PosTypeList,NewPars);
  return Result;
}



action mcrl2::lps::multi_action_type_checker::TraverseAct(const action &ma)
{
  size_t n = ma.size();
  if (n==0)
  {
    return ma;
  }

  if (gsIsParamId(ma))
  {
    const std::map<core::identifier_string,sort_expression> Vars;
    return RewrAct(Vars,ma);
  }

  throw mcrl2::runtime_error("Internal error. Action " + pp(ma) + " fails to match process.");
}


void mcrl2::lps::multi_action_type_checker::ReadInActs(const action_label_list &Acts)
{
  for (lps::action_label_list::const_iterator i=Acts.begin(); i!=Acts.end(); ++i)
  {
    action_label Act= *i;
    // core::identifier_string ActName=aterm_cast<core::identifier_string>(Act[0]);
    core::identifier_string ActName=Act.name();
    sort_expression_list ActType=Act.sorts();

    IsSortExprListDeclared(ActType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=actions.find(ActName);
    term_list<sort_expression_list> Types;
    if (j==actions.end())
    {
      // Types=make_list<sort_expression_list>(ActType);
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


mcrl2::lps::multi_action_type_checker::multi_action_type_checker(
            const data::data_specification &data_spec, 
            const action_label_list& action_decls)
  : data_type_checker(data_spec)
{
  mCRL2log(debug) << "type checking multiaction..." << std::endl;
  //check correctness of the multi-action in mult_act using
  //the process specification or LPS in spec
  // assert (gsIsProcSpec(spec) || gsIsLinProcSpec(spec));
  try
  {
    ReadInActs(action_decls);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nreading from LPS failed");
  }
}

multi_action mcrl2::lps::multi_action_type_checker::operator()(const multi_action &ma)
{
  try
  {
    action_list r;
    
    for (action_list::const_iterator l=ma.actions().begin(); l!=ma.actions().end(); ++l)
    {
      action o= *l;
      assert(gsIsParamId(o));
      o=TraverseAct(o);
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

/*************************   Here starts the state_formula_typechecker  ************************************/


regular_formula mcrl2::state_formulas::state_formula_type_checker::TraverseRegFrm(
          const std::map<core::identifier_string,sort_expression> &Vars, 
          const regular_formula &RegFrm)
{
  mCRL2log(debug) << "TraverseRegFrm: " << pp(RegFrm) << "" << std::endl;
  if (gsIsRegNil(RegFrm))
  {
    return RegFrm;
  }

  if (gsIsRegSeq(RegFrm) || gsIsRegAlt(RegFrm))
  {
    regular_formula NewArg1=TraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    regular_formula NewArg2=TraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[1]));
    return RegFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsRegTrans(RegFrm) || gsIsRegTransOrNil(RegFrm))
  {
    regular_formula NewArg=TraverseRegFrm(Vars,aterm_cast<aterm_appl>(RegFrm[0]));
    return RegFrm.set_argument(NewArg,0);
  }

  if (gsIsActFrm(RegFrm))
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

  if (gsIsActTrue(ActFrm) || gsIsActFalse(ActFrm))
  {
    return ActFrm;
  }

  if (action_formulas::is_not(ActFrm))
  {
    const not_ f=aterm_cast<const not_>(ActFrm);
    action_formula NewArg=TraverseActFrm(Vars,f.operand());
    return action_formulas::not_(NewArg); 
  }

  if (gsIsActAnd(ActFrm) || gsIsActOr(ActFrm) || gsIsActImp(ActFrm))
  {
    aterm_appl NewArg1=TraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[0]));
    aterm_appl NewArg2=TraverseActFrm(Vars,aterm_cast<aterm_appl>(ActFrm[1]));
    return ActFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsActForall(ActFrm) || gsIsActExists(ActFrm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    variable_list VarList=aterm_cast<variable_list>(ActFrm[0]);
    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,VarList,NewVars);
    
    aterm_appl NewArg2=TraverseActFrm(NewVars,aterm_cast<aterm_appl>(ActFrm[1]));
    return ActFrm.set_argument(NewArg2,1);
  }

  if (gsIsActAt(ActFrm))
  {
    action_formula NewArg1=TraverseActFrm(Vars,aterm_cast<action_formula>(ActFrm[0]));

    data_expression Time=aterm_cast<aterm_appl>(ActFrm[1]);
    aterm_appl NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(sort_real::real_()));

    sort_expression temp;
    if (!TypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
      }
    }
    return ActFrm.set_argument(NewArg1,0).set_argument(Time,1);
  }

  if (gsIsMultAct(ActFrm))
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

  if (gsIsDataExpr(ActFrm))
  {
    data_expression d(ActFrm);
    aterm_appl Type=TraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return d;
  }

  throw mcrl2::runtime_error("Internal error. The action formula " + pp(ActFrm) + " fails to match any known form in typechecking case analysis");
}


state_formula mcrl2::state_formulas::state_formula_type_checker::TraverseStateFrm(
                const std::map<core::identifier_string,sort_expression> &Vars, 
                const std::map<core::identifier_string,sort_expression_list> &StateVars, 
                const state_formula &StateFrm)
{
  mCRL2log(debug) << "TraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

  if (gsIsStateTrue(StateFrm) || gsIsStateFalse(StateFrm) || gsIsStateDelay(StateFrm) || gsIsStateYaled(StateFrm))
  {
    return StateFrm;
  }

  if (gsIsStateNot(StateFrm))
  {
    state_formula NewArg=TraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    return StateFrm.set_argument(NewArg,0);
  }

  if (gsIsStateAnd(StateFrm) || gsIsStateOr(StateFrm) || gsIsStateImp(StateFrm))
  {
    state_formula NewArg1=TraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[0]));
    state_formula NewArg2=TraverseStateFrm(Vars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    return StateFrm.set_argument(NewArg1,0).set_argument(NewArg2,1);
  }

  if (gsIsStateForall(StateFrm) || gsIsStateExists(StateFrm))
  {
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);

    variable_list VarList=aterm_cast<variable_list>(StateFrm[0]);
    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(CopyVars,VarList,NewVars);

    aterm_appl NewArg2=TraverseStateFrm(NewVars,StateVars,aterm_cast<aterm_appl>(StateFrm[1]));
    return StateFrm.set_argument(NewArg2,1);
  }

  if (is_may(StateFrm))
  {
    const may& f=aterm_cast<const may>(StateFrm);
    const regular_formula RegFrm=TraverseRegFrm(Vars,f.formula());
    const state_formula NewArg2=TraverseStateFrm(Vars,StateVars,f.operand());
    return may(RegFrm,NewArg2);
  }

  if (is_must(StateFrm))
  {
    const must& f=aterm_cast<const must>(StateFrm);
    const regular_formula RegFrm=TraverseRegFrm(Vars,f.formula());
    const state_formula NewArg2=TraverseStateFrm(Vars,StateVars,f.operand());
    return must(RegFrm,NewArg2);
  }

  if (gsIsStateDelayTimed(StateFrm) || gsIsStateYaledTimed(StateFrm))
  {
    data_expression Time=aterm_cast<aterm_appl>(StateFrm[0]);
    sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(sort_real::real_()));

    sort_expression temp;
    if (!TypeMatchA(sort_real::real_(),NewType,temp))
    {
      //upcasting
      aterm_appl CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(sort_real::real_(),NewType,Time);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
      }
    }
    return StateFrm.set_argument(Time,0);
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

  if (gsIsStateNu(StateFrm) || gsIsStateMu(StateFrm))
  {
    std::map<core::identifier_string,sort_expression_list> CopyStateVars(StateVars);

    // Make the new state variable:
    std::map<core::identifier_string,sort_expression> FormPars;
    assignment_list r;
    sort_expression_list t;
    for (assignment_list l=aterm_cast<data::assignment_list>(StateFrm[1]); !l.empty(); l=l.tail())
    {
      assignment o=l.front();

      core::identifier_string VarName=aterm_cast<core::identifier_string>(aterm_cast<aterm_appl>(o[0])[0]);
      if (FormPars.count(VarName)>0)
      {
        throw mcrl2::runtime_error("non-unique formal parameter " + pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
      }

      sort_expression VarType=aterm_cast<aterm_appl>(aterm_cast<aterm_appl>(o[0])[1]);
      try
      {
        IsSortExprDeclared(VarType);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
      }

      FormPars[VarName]=VarType;

      data_expression VarInit=aterm_cast<aterm_appl>(o[1]);
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

      r.push_front(o.set_argument(VarInit,1));
      t.push_front(VarType);
    }

    state_formula NewStateFrm=StateFrm.set_argument(reverse(r),1);
    std::map<core::identifier_string,sort_expression> CopyVars(Vars);
    CopyVars.insert(FormPars.begin(),FormPars.end());
    

    CopyStateVars[aterm_cast<core::identifier_string>(NewStateFrm[0])]=reverse(t);
    
    aterm_appl NewArg;
    try
    {
      NewArg=TraverseStateFrm(CopyVars,CopyStateVars,aterm_cast<aterm_appl>(NewStateFrm[2]));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking TERM TEMPORARILY IN ATERM FORMAT " + mcrl2::utilities::to_string(StateFrm) /* + pp(StateFrm)*/ );
    }
    return NewStateFrm.set_argument(NewArg,2);
  }

  if (gsIsDataExpr(StateFrm))
  {
    data_expression d(StateFrm);
    aterm_appl Type=TraverseVarConsTypeD(Vars, Vars, d, sort_bool::bool_());
    return d;
  }

  throw mcrl2::runtime_error("Internal error. The state formula " + pp(StateFrm) + " fails to match any known form in typechecking case analysis");
}




mcrl2::state_formulas::state_formula_type_checker::state_formula_type_checker(
       const data::data_specification &data_spec, 
       const action_label_list& action_decls)
  :  lps::multi_action_type_checker(data_spec,action_decls)
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

