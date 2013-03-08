// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/lps/typecheck.h"


using namespace atermpp;
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


action mcrl2::lps::multi_action_type_checker::RewrAct(const action &ProcTerm)
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
      const std::map<core::identifier_string,sort_expression> Vars;
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

std::cerr << "Form of an action " << ma << "\n";
  size_t n = ma.size();
  if (n==0)
  {
    return ma;
  }

  if (gsIsParamId(ma))
  {
    return RewrAct(ma);
  }

  /* if (gsIsSync(ProcTerm))
  {
    aterm_appl NewLeft=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    aterm_appl NewRight=gstcTraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    aterm_appl a=ProcTerm.set_argument(NewLeft,0).set_argument(NewRight,1);
    return a;
  } */

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

