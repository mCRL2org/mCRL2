// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/data/untyped_possible_sorts.h"
#include "mcrl2/process/typecheck.h"

using namespace mcrl2;
using namespace mcrl2::process;

template <class T>
data::sort_expression_list get_sorts(const atermpp::term_list<T>& l)
{
  std::vector<data::sort_expression> v;
  for(typename atermpp::term_list<T>::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    v.push_back(i->sort());
  }
  return data::sort_expression_list(v.begin(),v.end());
}

static bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2);

static bool MActIn(core::identifier_string_list MAct, action_name_multiset_list MActs)
{
  //returns true if MAct is in MActs
  for (action_name_multiset_list::const_iterator i=MActs.begin(); i!=MActs.end(); ++i)
    if (MActEq(MAct,i->names()))
    {
      return true;
    }

  return false;
}

static bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2)
{
  //returns true if the two multiactions are equal.
  if (MAct1.size()!=MAct2.size())
  {
    return false;
  }
  if (MAct1.empty())
  {
    return true;
  }
  core::identifier_string Act1=MAct1.front();
  MAct1=MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  core::identifier_string_list NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    core::identifier_string Act2=MAct2.front();
    if (Act1==Act2)
    {
      MAct2=atermpp::reverse(NewMAct2)+MAct2.tail();
      return MActEq(MAct1,MAct2);
    }
    else
    {
      NewMAct2.push_front(Act2);
    }
  }
  return false;
}




static core::identifier_string_list list_minus(const core::identifier_string_list &l, const core::identifier_string_list &m)
{
  core::identifier_string_list n;
  for (core::identifier_string_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    if (std::find(m.begin(),m.end(),*i) == m.end())
    {
      n.push_front(*i);
    }
  }
  return atermpp::reverse(n);
}

process_expression process_type_checker::MakeActionOrProc(
             bool is_action,
             const core::identifier_string &Name,
             const data::sort_expression_list &FormParList,
             const data::data_expression_list FactParList)
{
  if (is_action)
  {
    return action(action_label(Name,FormParList),FactParList);
  }
  else
  {
    assert(proc_pars.count(std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(FormParList)))>0);
    const data::variable_list& FormalVars=proc_pars[std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(FormParList))];
    return process_instance(process_identifier(Name,FormalVars),FactParList);
  }
}

process_equation_list mcrl2::process::process_type_checker::WriteProcs(const process_equation_vector &oldprocs)
{
  process_equation_list Result;
  for (process_equation_vector::const_reverse_iterator i=oldprocs.rbegin(); i!=oldprocs.rend(); ++i)
  {
    const process_identifier& ProcVar=i->identifier();
    if (ProcVar==initial_process())
    {
      continue;
    }
    Result.push_front(process_equation(ProcVar, ProcVar.variables(),proc_bodies[std::pair<core::identifier_string,data::sort_expression_list>(ProcVar.name(),UnwindType(get_sorts(ProcVar.variables())))]));
  }
  return Result;
}


atermpp::term_list<data::sort_expression_list> mcrl2::process::process_type_checker::TypeListsIntersect(
                     const atermpp::term_list<data::sort_expression_list> &TypeListList1,
                     const atermpp::term_list<data::sort_expression_list> &TypeListList2)
{
  // returns the intersection of the 2 type list lists

  atermpp::term_list<data::sort_expression_list> Result;

  for (atermpp::term_list<data::sort_expression_list>::const_iterator i=TypeListList2.begin(); i!=TypeListList2.end(); ++i)
  {
    const data::sort_expression_list TypeList2= *i;
    if (InTypesL(TypeList2,TypeListList1))
    {
      Result.push_front(TypeList2);
    }
  }
  return atermpp::reverse(Result);
}


data::sort_expression_list mcrl2::process::process_type_checker::GetNotInferredList(const atermpp::term_list<data::sort_expression_list> &TypeListList)
{
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1

  data::sort_expression_list Result;
  size_t nFormPars=(TypeListList.front()).size();
  std::vector<data::sort_expression_list> Pars(nFormPars);
  for (size_t i=0; i<nFormPars; i++)
  {
    Pars[i]=data::sort_expression_list();
  }

  for (atermpp::term_list<data::sort_expression_list>::const_iterator j=TypeListList.begin(); j!=TypeListList.end(); ++j)
  {
    data::sort_expression_list TypeList=*j;
    for (size_t i=0; i<nFormPars; TypeList=TypeList.tail(),i++)
    {
      Pars[i]=InsertType(Pars[i],TypeList.front());
    }
  }

  for (size_t i=nFormPars; i>0; i--)
  {
    data::sort_expression Sort;
    if (Pars[i-1].size()==1)
    {
      Sort=Pars[i-1].front();
    }
    else
    {
      Sort=data::untyped_possible_sorts(data::sort_expression_list(atermpp::reverse(Pars[i-1])));
    }
    Result.push_front(Sort);
  }
  return Result;
}

bool mcrl2::process::process_type_checker::IsTypeAllowedA(const data::sort_expression &Type, const data::sort_expression &PosType)
{
  //Checks if Type is allowed by PosType
  if (data::is_untyped_sort(data::sort_expression(PosType)))
  {
    return true;
  }
  if (is_untyped_possible_sorts(PosType))
  {
    const data::untyped_possible_sorts& s=atermpp::down_cast<data::untyped_possible_sorts>(PosType);
    return InTypesA(Type,s.sorts());
  }

  //PosType is a normal type
  return EqTypesA(Type,PosType);
}


bool mcrl2::process::process_type_checker::IsTypeAllowedL(const data::sort_expression_list &TypeList, const data::sort_expression_list PosTypeList)
{
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(TypeList.size()==PosTypeList.size());
  data::sort_expression_list::const_iterator j=PosTypeList.begin();
  for (data::sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i,++j)
    if (!IsTypeAllowedA(*i,*j))
    {
      return false;
    }
  return true;
}


data::sort_expression_list mcrl2::process::process_type_checker::InsertType(const data::sort_expression_list TypeList, const data::sort_expression Type)
{
  for (data::sort_expression_list OldTypeList=TypeList; !OldTypeList.empty(); OldTypeList=OldTypeList.tail())
  {
    if (EqTypesA(OldTypeList.front(),Type))
    {
      return TypeList;
    }
  }
  data::sort_expression_list result=TypeList;
  result.push_front(Type);
  return result;
}


std::pair<bool,data::sort_expression_list> mcrl2::process::process_type_checker::AdjustNotInferredList(
            const data::sort_expression_list &PosTypeList,
            const atermpp::term_list<data::sort_expression_list> &TypeListList)
{
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  //if PosTypeList has only normal types -- check if it is in TypeListList,
  //if so return PosTypeList, otherwise return false.
  if (!IsNotInferredL(PosTypeList))
  {
    if (InTypesL(PosTypeList,TypeListList))
    {
      return std::make_pair(true,PosTypeList);
    }
    else
    {
      return std::make_pair(false, data::sort_expression_list());
    }
  }

  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  atermpp::term_list<data::sort_expression_list> NewTypeListList;
  for (atermpp::term_list<data::sort_expression_list>::const_iterator i=TypeListList.begin();
                    i!=TypeListList.end(); ++i)
  {
    data::sort_expression_list TypeList= *i;
    if (IsTypeAllowedL(TypeList,PosTypeList))
    {
      NewTypeListList.push_front(TypeList);
    }
  }
  if (NewTypeListList.empty())
  {
    return std::make_pair(false, data::sort_expression_list());
  }
  if (NewTypeListList.size()==1)
  {
    return std::make_pair(true,NewTypeListList.front());
  }

  // otherwise return not inferred.
  return std::make_pair(true,GetNotInferredList(atermpp::reverse(NewTypeListList)));
}



process_expression mcrl2::process::process_type_checker::RewrActProc(
               const std::map<core::identifier_string,data::sort_expression> &Vars,
               const core::identifier_string& Name,
               const data::data_expression_list& pars)
{
  process_expression Result;
  atermpp::term_list<data::sort_expression_list> ParList;

  bool action=false;

  const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=actions.find(Name);

  if (j!=actions.end())
  {
    ParList=j->second;
    action=true;
  }
  else
  {
    const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=processes.find(Name);
    if (j!=processes.end())
    {
      ParList=j->second;
      action=false;
    }
    else
    {
      throw mcrl2::runtime_error("action or process " + core::pp(Name) + " not declared");
    }
  }
  assert(!ParList.empty());

  size_t nFactPars=pars.size();
  const std::string msg=(action)?"action":"process";

  //filter the list of lists ParList to keep only the lists of lenth nFactPars
  {
    atermpp::term_list <data::sort_expression_list> NewParList;
    for (; !ParList.empty(); ParList=ParList.tail())
    {
      data::sort_expression_list Par=ParList.front();
      if (Par.size()==nFactPars)
      {
        NewParList.push_front(Par);
      }
    }
    ParList=atermpp::reverse(NewParList);
  }

  if (ParList.empty())
  {
    throw mcrl2::runtime_error("no " + msg + " " + core::pp(Name)
                    + " with " + atermpp::to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                    + " is declared (while typechecking " + core::pp(Name) + "(" + data::pp(pars) + "))");
  }

  if (ParList.size()==1)
  {
    Result=MakeActionOrProc(action,Name,ParList.front(),pars);
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=MakeActionOrProc(action,Name,GetNotInferredList(ParList),pars);
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  data::sort_expression_list PosTypeList=is_action(Result)?
                atermpp::down_cast<const process::action>(Result).label().sorts():
                get_sorts(atermpp::down_cast<const process_instance>(Result).identifier().variables());
  data::data_expression_list NewPars;
  data::sort_expression_list NewPosTypeList;
  for (data::data_expression_list Pars=pars; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
  {
    data::data_expression Par=Pars.front();
    data::sort_expression PosType=PosTypeList.front();

    data::sort_expression NewPosType;
    try
    {
      NewPosType=TraverseVarConsTypeD(Vars,Vars,Par,PosType);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(Par) + " as type " + data::pp(ExpandNumTypesDown(PosType)) + " (while typechecking " + core::pp(Name) +
            "(" + data::pp(pars) + "))");
    }
    NewPars.push_front(Par);
    NewPosTypeList.push_front(NewPosType);
  }
  NewPars=atermpp::reverse(NewPars);
  NewPosTypeList=atermpp::reverse(NewPosTypeList);

  std::pair<bool,data::sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
  PosTypeList=p.second;

  if (!p.first)
  {
    PosTypeList=is_action(Result)?
                     atermpp::down_cast<const process::action>(Result).label().sorts():
                     get_sorts(atermpp::down_cast<const process_instance>(Result).identifier().variables());
    data::data_expression_list Pars=NewPars;
    NewPars=data::data_expression_list();
    data::sort_expression_list CastedPosTypeList;
    for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
    {
      data::data_expression Par=Pars.front();
      data::sort_expression PosType=PosTypeList.front();
      data::sort_expression NewPosType=NewPosTypeList.front();

      data::sort_expression CastedNewPosType;
      try
      {
        std::map<core::identifier_string,data::sort_expression> dummy_table;
        CastedNewPosType=UpCastNumericType(PosType,NewPosType,Par,Vars,Vars,dummy_table,false);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + data::pp(NewPosType) + " to " + data::pp(PosType) + "(while typechecking " + data::pp(Par) + " in " +
                   core::pp(Name) + "(" + data::pp(pars) + ")");
      }

      NewPars.push_front(Par);
      CastedPosTypeList.push_front(CastedNewPosType);
    }
    NewPars=atermpp::reverse(NewPars);
    NewPosTypeList=atermpp::reverse(CastedPosTypeList);

    std::pair<bool,data::sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
    PosTypeList=p.second;

    if (!p.first)
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(Name) + "with type " + data::pp(NewPosTypeList) + " is declared (while typechecking " +
              core::pp(Name) + "(" + data::pp(pars) + "))");
    }
  }

  if (IsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(Name));
  }

  return MakeActionOrProc(action,Name,PosTypeList,NewPars);
}


process_expression mcrl2::process::process_type_checker::TraverseActProcVarConstP(
           const std::map<core::identifier_string,data::sort_expression> &Vars,
           const process_expression &ProcTerm)
{
  size_t n = ProcTerm.size();
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (is_untyped_process_assignment(ProcTerm))
  {
    const untyped_process_assignment& t=atermpp::down_cast<const untyped_process_assignment>(ProcTerm);
    mCRL2log(log::debug) << "typechecking a process call with short-hand assignments " << t << "" << std::endl;
    const core::identifier_string& Name=t.name();
    const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=processes.find(Name);

    if (j==processes.end())
    {
      throw mcrl2::runtime_error("process " + core::pp(Name) + " not declared");
    }

    atermpp::term_list <data::sort_expression_list> ParList=j->second;
    // Put the assignments into a table
    std::map <core::identifier_string,data::data_expression> As;    // data::variable -> expression (both untyped, still)
    const data::untyped_identifier_assignment_list &al=t.assignments();
    for (data::untyped_identifier_assignment_list::const_iterator l=al.begin(); l!=al.end(); ++l)
    {
      const data::untyped_identifier_assignment& a= *l;
      const std::map <core::identifier_string,data::data_expression>::const_iterator i=As.find(a.lhs());
      if (i!=As.end()) // An data::assignment of the shape x:=t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double data::assignment to data::variable " + core::pp(a.lhs()) + " (detected assigned values are " + data::pp(i->second) + " and " + core::pp(a.rhs()) + ")");
      }
      As[a.lhs()]=a.rhs();
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      atermpp::term_list <data::sort_expression_list> NewParList;
      assert(!ParList.empty());
      core::identifier_string Culprit; // Variable used for more intelligible error messages.
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        data::sort_expression_list Par=ParList.front();

        // get the formal parameter names
        assert(proc_pars.count(std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(Par)))>0);
        data::variable_list FormalPars=proc_pars[std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(Par))];
        // we only need the names of the parameters, not the types
        core::identifier_string_list FormalParNames;
        for (const data::variable& par: FormalPars)
        {
          FormalParNames.push_front(par.name());
        }

        core::identifier_string_list As_lhss;
        for(std::map <core::identifier_string,data::data_expression> ::const_iterator i=As.begin(); i!=As.end(); ++i)
        {
          As_lhss.push_front(i->first);
        }
        core::identifier_string_list l=list_minus(As_lhss,FormalParNames);
        if (l.empty())
        {
          NewParList.push_front(Par);
        }
        else
        {
          Culprit=l.front();
        }
      }
      ParList=atermpp::reverse(NewParList);

      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no process " + core::pp(Name) + " containing all assignments in " + process::pp(t) + ".\n" + "Problematic data::variable is " + core::pp(Culprit) + ".");
      }
      if (!ParList.tail().empty())
      {
        throw mcrl2::runtime_error("ambiguous process " + core::pp(Name) + " containing all assignments in " + process::pp(t) + ".");
      }
    }

    // get the formal parameter names
    data::data_expression_list ActualPars;
    assert(proc_pars.count(std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(ParList.front())))>0);
    const data::variable_list& FormalPars=proc_pars[std::pair<core::identifier_string,data::sort_expression_list>(Name,UnwindType(ParList.front()))];
    {
      // we only need the names of the parameters, not the types
      for (const data::variable& par: FormalPars)
      {
        const core::identifier_string& FormalParName=par.name();
        data::data_expression ActualPar;
        const std::map <core::identifier_string,data::data_expression> ::const_iterator i=As.find(FormalParName);
        if (i==As.end())  // Not found.
        {
          ActualPar=data::untyped_identifier(FormalParName);
        }
        else
        {
          ActualPar=i->second;
        }
        ActualPars.push_front(ActualPar);
      }
      ActualPars=atermpp::reverse(ActualPars);
    }

    process_expression TypeCheckedProcTerm;
    try
    {
      TypeCheckedProcTerm=RewrActProc(Vars, Name,ActualPars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking the process call with short-hand assignments " + process::pp(t));
    }

    //atermpp::reverse the assignments
    As.clear();
    std::map <core::identifier_string,data::assignment> As_new;
    data::variable_list m=FormalPars;
    data::data_expression_list l=atermpp::down_cast<const process_instance>(TypeCheckedProcTerm).actual_parameters();
    for ( ; !l.empty(); l=l.tail(),m=m.tail())
    {
      const data::data_expression act_par=l.front();
      const data::variable form_par=m.front();
      /* if (form_par==act_par) This removal is unsound. Consider P(x:D)=sum x:D.a(x).P(x=x); Here the x=x cannot be removed, as the first
                                x refers to the parameter of the process, and the second refers to the bound x in the sum.
      {
        continue;  //parameter does not change
      } */
      As_new[form_par.name()]=data::assignment(form_par,act_par);
    }

    data::assignment_list TypedAssignments;
    for (data::untyped_identifier_assignment_list::const_iterator l=t.assignments().begin(); l!=t.assignments().end(); ++l)
    {
      const data::untyped_identifier_assignment a= *l;
      const std::map <core::identifier_string,data::assignment> ::const_iterator i=As_new.find(a.lhs());
      if (i==As_new.end())
      {
        continue;
      }
      TypedAssignments.push_front(i->second);
    }
    TypedAssignments=atermpp::reverse(TypedAssignments);

    return process_instance_assignment(atermpp::down_cast<const process_instance>(TypeCheckedProcTerm).identifier(),TypedAssignments);
  }
  //Here the section dealing with assignments ends.

  if (is_untyped_parameter_identifier(ProcTerm))
  {
    const untyped_parameter_identifier& t=atermpp::down_cast<const untyped_parameter_identifier>(ProcTerm);
    process_expression result= RewrActProc(Vars,t.name(), t.arguments());
    return result;
  }

  if (is_hide(ProcTerm))
  {
    const hide& t=atermpp::down_cast<const hide>(ProcTerm);
    const core::identifier_string_list& act_list=t.hide_set();
    if (act_list.empty())
    {
      mCRL2log(log::warning) << "Hiding empty set of actions (typechecking " << t << ")" << std::endl;
    }

    std::set<core::identifier_string> Acts;
    for (core::identifier_string_list::const_iterator a=act_list.begin(); a!=act_list.end(); ++a)
    {
      //Actions must be declared
      if (actions.count(*a)==0)
      {
        throw mcrl2::runtime_error("Hiding an undefined action " + core::pp(*a) + " (typechecking " + core::pp(t) + ")");
      }
      if (!Acts.insert(*a).second)  // The action was already in the set.
      {
        mCRL2log(log::warning) << "Hiding action " << *a << " twice (typechecking " << t << ")" << std::endl;
      }
    }
    return hide(act_list, TraverseActProcVarConstP(Vars,t.operand()));
  }

  if (is_block(ProcTerm))
  {
    const block& t=atermpp::down_cast<const block>(ProcTerm);
    const core::identifier_string_list& act_list=t.block_set();
    if (act_list.empty())
    {
      mCRL2log(log::warning) << "Blocking empty set of actions (typechecking " << t << ")" << std::endl;
    }

    std::set<core::identifier_string> Acts;
    for (core::identifier_string_list::const_iterator a=act_list.begin(); a!=act_list.end(); ++a)
    {
      //Actions must be declared
      if (actions.count(*a)==0)
      {
        throw mcrl2::runtime_error("Blocking an undefined action " + core::pp(*a) + " (typechecking " + core::pp(t) + ")");
      }
      if (!Acts.insert(*a).second)  // The action was already in the set.
      {
        mCRL2log(log::warning) << "Blocking action " << *a << " twice (typechecking " << t << ")" << std::endl;
      }
    }
    return block(act_list,TraverseActProcVarConstP(Vars,t.operand()));
  }

  //rename
  if (is_rename(ProcTerm))
  {
    const rename& t=atermpp::down_cast<const rename>(ProcTerm);
    const rename_expression_list& RenList=t.rename_set();

    if (RenList.empty())
    {
      mCRL2log(log::warning) << "renaming empty set of actions (typechecking " << ProcTerm << ")" << std::endl;
    }

    std::set<core::identifier_string> ActsFrom;

    for (rename_expression_list::const_iterator r=RenList.begin(); r!=RenList.end(); ++r)
    {
      const rename_expression& Ren= *r;
      const core::identifier_string& ActFrom=Ren.source();
      const core::identifier_string& ActTo=Ren.target();

      if (ActFrom==ActTo)
      {
        mCRL2log(log::warning) << "renaming action " << ActFrom << " into itself (typechecking " << ProcTerm << ")" << std::endl;
      }

      //Actions must be declared and of the same types
      atermpp::term_list<data::sort_expression_list> TypesFrom,TypesTo;
      const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j_from=actions.find(ActFrom);
      if (j_from==actions.end())
      {
        throw mcrl2::runtime_error("renaming an undefined action " + core::pp(ActFrom) + " (typechecking " + process::pp(ProcTerm) + ")");
      }
      TypesFrom=j_from->second;
      const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j_to=actions.find(ActFrom);
      if (j_to==actions.end())
      {
        throw mcrl2::runtime_error("renaming into an undefined action " + core::pp(ActTo) + " (typechecking " + process::pp(ProcTerm) + ")");
      }
      TypesTo=j_to->second;

      TypesTo=TypeListsIntersect(TypesFrom,TypesTo);
      if (TypesTo.empty())
      {
        throw mcrl2::runtime_error("renaming action " + core::pp(ActFrom) + " into action " + core::pp(ActTo) + ": these two have no common type (typechecking " + process::pp(ProcTerm) + ")");
      }

      if (!ActsFrom.insert(ActFrom).second) // The element was already in the set.
      {
        throw mcrl2::runtime_error("renaming action " + core::pp(ActFrom) + " twice (typechecking " + process::pp(ProcTerm) + ")");
      }
    }
    return rename(RenList,TraverseActProcVarConstP(Vars,t.operand()));
  }

  //comm: like renaming multiactions (with the same parameters) to action/tau
  if (is_comm(ProcTerm))
  {
    const comm& t=atermpp::down_cast<const comm>(ProcTerm);
    const communication_expression_list& CommList=t.comm_set();

    if (CommList.empty())
    {
      mCRL2log(log::warning) << "synchronizing empty set of (multi)actions (typechecking " << ProcTerm << ")" << std::endl;
    }
    else
    {
      core::identifier_string_list ActsFrom;

      for (communication_expression_list::const_iterator c=CommList.begin(); c!=CommList.end(); ++c)
      {
        const communication_expression& Comm= *c;
        const core::identifier_string_list MActFrom=Comm.action_name().names();
        assert(!MActFrom.empty());
        core::identifier_string ActTo=Comm.name();

        if (MActFrom.size()==1)
        {
          throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + core::pp(MActFrom.front()) + " into " + core::pp(ActTo) + " (typechecking " + process::pp(ProcTerm) + ")");
        }

        //Actions must be declared
        atermpp::term_list<data::sort_expression_list> ResTypes;

        if (!core::is_nil(ActTo))
        {
          const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=actions.find(ActTo);
          if (j==actions.end())
          {
            throw mcrl2::runtime_error("synchronizing to an undefined action " + core::pp(ActTo) + " (typechecking " + process::pp(ProcTerm) + ")");
          }
          ResTypes=j->second;
        }

        for (core::identifier_string_list::const_iterator i=MActFrom.begin(); i!=MActFrom.end(); ++i)
        {
          const core::identifier_string& Act= *i;
          const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=actions.find(Act);
          atermpp::term_list<data::sort_expression_list> Types;
          if (j==actions.end())
          {
            throw mcrl2::runtime_error("synchronizing an undefined action " + core::pp(Act) + " in (multi)action " + core::pp(MActFrom) + " (typechecking " + process::pp(ProcTerm) + ")");
          }
          Types=j->second;
          ResTypes=TypeListsIntersect(ResTypes,Types);
          if (ResTypes.empty())
          {
            throw mcrl2::runtime_error("synchronizing action " + core::pp(Act) + " from (multi)action " + core::pp(MActFrom) +
                              " into action " + core::pp(ActTo) + ": these have no common type (typechecking " + process::pp(ProcTerm) + ")");
          }
        }

        //the multiactions in the lhss of comm should not intersect.
        //make the list of unique actions
        core::identifier_string_list Acts;
        for (core::identifier_string_list::const_iterator i=MActFrom.begin(); i!=MActFrom.end(); ++i)
        {
          const core::identifier_string& Act= *i;
          if (std::find(Acts.begin(),Acts.end(),Act)==Acts.end())
          {
            Acts.push_front(Act);
          }
        }
        for (core::identifier_string_list::const_iterator a=Acts.begin(); a!=Acts.end(); ++a)
        {
          const core::identifier_string& Act= *a;
          if (std::find(ActsFrom.begin(),ActsFrom.end(),Act)!=ActsFrom.end())
          {
            throw mcrl2::runtime_error("synchronizing action " + core::pp(Act) + " in different ways (typechecking " + process::pp(ProcTerm) + ")");
          }
          else
          {
            ActsFrom.push_front(Act);
          }
        }
      }
    }
    return comm(CommList,TraverseActProcVarConstP(Vars,t.operand()));
  }

  //allow
  if (is_allow(ProcTerm))
  {
    const allow& t=atermpp::down_cast<const allow>(ProcTerm);
    const action_name_multiset_list& MActList=t.allow_set();

    if (MActList.empty())
    {
      mCRL2log(log::warning) << "allowing empty set of (multi) actions (typechecking " << ProcTerm << ")" << std::endl;
    }
    else
    {
       action_name_multiset_list MActs;

      for (action_name_multiset_list::const_iterator i=MActList.begin(); i!=MActList.end(); ++i)
      {
        core::identifier_string_list MAct=i->names();

        //Actions must be declared
        for (core::identifier_string_list::const_iterator j=MAct.begin(); j!=MAct.end(); ++j)
        {
          core::identifier_string Act= *j;
          if (actions.count(Act)==0)
          {
            throw mcrl2::runtime_error("allowing an undefined action " + core::pp(Act) + " in (multi)action " + core::pp(MAct) + " (typechecking " + process::pp(ProcTerm) + ")");
          }
        }

        if (MActIn(MAct,MActs))
        {
          mCRL2log(log::warning) << "allowing (multi)action " << MAct << " twice (typechecking " << ProcTerm << ")" << std::endl;
        }
        else
        {
          MActs.push_front(MAct);
        }
      }
    }
    return allow(MActList, TraverseActProcVarConstP(Vars,t.operand()));
  }

  if (is_sync(ProcTerm))
  {
    const sync& t=atermpp::down_cast<const sync>(ProcTerm);
    return sync(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_seq(ProcTerm))
  {
    const seq& t=atermpp::down_cast<const seq>(ProcTerm);
    return seq(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_bounded_init(ProcTerm))
  {
    const bounded_init& t=atermpp::down_cast<const bounded_init>(ProcTerm);
    return bounded_init(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_merge(ProcTerm))
  {
    const merge& t=atermpp::down_cast<const merge>(ProcTerm);
    return merge(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_left_merge(ProcTerm))
  {
    const left_merge& t=atermpp::down_cast<const left_merge>(ProcTerm);
    return left_merge(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_choice(ProcTerm))
  {
    const choice& t=atermpp::down_cast<const choice>(ProcTerm);
    return choice(TraverseActProcVarConstP(Vars,t.left()),TraverseActProcVarConstP(Vars,t.right()));
  }

  if (is_at(ProcTerm))
  {
    const at& t=atermpp::down_cast<const at>(ProcTerm);
    const process_expression NewProc=TraverseActProcVarConstP(Vars,t.operand());
    data::data_expression Time=t.time_stamp();
    const data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

    data::sort_expression temp;
    if (!TypeMatchA(data::sort_real::real_(),NewType,temp))
    {
      //upcasting
      data::sort_expression CastedNewType;
      try
      {
        std::map<core::identifier_string,data::sort_expression> dummy_table;
        CastedNewType=UpCastNumericType(data::sort_real::real_(),NewType,Time,Vars,Vars,dummy_table,false);
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + data::pp(Time) + " to type Real");
      }
    }

    return at(NewProc,Time);
  }

  if (is_if_then(ProcTerm))
  {
    const if_then& t=atermpp::down_cast<const if_then>(ProcTerm);
    data::data_expression Cond=t.condition();
    TraverseVarConsTypeD(Vars,Vars,Cond,data::sort_bool::bool_());
    const process_expression NewThen=TraverseActProcVarConstP(Vars,t.then_case());
    return if_then(Cond,NewThen);
  }

  if (is_if_then_else(ProcTerm))
  {
    const if_then_else& t=atermpp::down_cast<const if_then_else>(ProcTerm);
    data::data_expression Cond=t.condition();
    TraverseVarConsTypeD(Vars,Vars,Cond,data::sort_bool::bool_());
    const process_expression NewThen=TraverseActProcVarConstP(Vars,t.then_case());
    const process_expression NewElse=TraverseActProcVarConstP(Vars,t.else_case());
    return if_then_else(Cond,NewThen,NewElse);
  }

  if (is_sum(ProcTerm))
  {
    const sum& t=atermpp::down_cast<const sum>(ProcTerm);
    std::map<core::identifier_string,data::sort_expression> CopyVars;
    CopyVars=Vars;

    std::map<core::identifier_string,data::sort_expression> NewVars;
    process_expression NewProc;
    try
    {
      AddVars2Table(CopyVars, t.variables());
      NewVars = CopyVars;
      NewProc=TraverseActProcVarConstP(NewVars,t.operand());
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(ProcTerm));
    }
    return sum(t.variables(),NewProc);
  }
  if (is_stochastic_operator(ProcTerm))
  {
    const stochastic_operator& t=atermpp::down_cast<const stochastic_operator>(ProcTerm);
    std::map<core::identifier_string,data::sort_expression> CopyVars;
    CopyVars=Vars;

    data::data_expression distribution=t.distribution();
    process_expression NewProc;
    try
    {
      AddVars2Table(CopyVars,t.variables());
      std::map<core::identifier_string,data::sort_expression> NewVars = CopyVars;
      TraverseVarConsTypeD(NewVars,NewVars,distribution,data::sort_real::real_());
      NewProc=TraverseActProcVarConstP(NewVars,t.operand());
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(ProcTerm));
    }
    return stochastic_operator(t.variables(),distribution,NewProc);
  }

  throw mcrl2::runtime_error("Internal error. Process " + process::pp(ProcTerm) + " fails to match known processes.");
}


void mcrl2::process::process_type_checker::TransformActProcVarConst(void)
{
  std::map<core::identifier_string,data::sort_expression> Vars;

  //process and data terms in processes and init
  assert(proc_pars.size()==proc_bodies.size());
  for (std::map <std::pair<core::identifier_string,data::sort_expression_list>,data::variable_list>::const_iterator i=proc_pars.begin(); i!=proc_pars.end(); ++i)
  {
    Vars=glob_vars;
    AddVars2Table(Vars, i->second);
    assert(proc_bodies.count(i->first)>0);
    const process_expression NewProcTerm=TraverseActProcVarConstP(Vars,proc_bodies[i->first]);
    proc_bodies[i->first]=NewProcTerm;
  }
}



void mcrl2::process::process_type_checker::ReadInProcsAndInit(const std::vector<process_equation>& Procs, const process_expression &Init)
{
  for (std::vector<process_equation>::const_iterator i=Procs.begin(); i!=Procs.end(); ++i)
  {
    const process_equation &Proc= *i;
    core::identifier_string ProcName=Proc.identifier().name();

    if (actions.count(ProcName)>0)
    {
      throw mcrl2::runtime_error("declaration of both process and action " + std::string(ProcName));
    }

    const data::sort_expression_list &ProcType=get_sorts(Proc.identifier().variables());
    check_sort_list_is_declared(ProcType);

    const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=processes.find(ProcName);
    atermpp::term_list<data::sort_expression_list> Types;
    if (j==processes.end())
    {
      Types = { ProcType };
    }
    else
    {
      Types=j->second;
      // the table processes contains a list of types for each
      // process name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (InTypesL(ProcType, Types))
      {
        throw mcrl2::runtime_error("double declaration of process " + std::string(ProcName));
      }
      else
      {
        Types = Types + atermpp::term_list<data::sort_expression_list>({ ProcType });
      }
    }
    processes[ProcName]=Types;

    //check that all formal parameters of the process are unique.
    const data::variable_list ProcVars=Proc.formal_parameters();
    if (!VarsUnique(ProcVars))
    {
      throw mcrl2::runtime_error("the formal variables in process " + process::pp(Proc) + " are not unique");
    }

    std::pair<core::identifier_string,data::sort_expression_list> p(Proc.identifier().name(),UnwindType(get_sorts(Proc.identifier().variables())));
    proc_pars[p]=UnwindType(Proc.formal_parameters());
    proc_bodies[p]=Proc.expression();
  }
  std::pair<core::identifier_string,data::sort_expression_list> p(initial_process().name(),
                                                      UnwindType(get_sorts(initial_process().variables())));
  proc_pars[p]=data::variable_list();
  proc_bodies[p]=Init;

}

void mcrl2::process::process_type_checker::ReadInActs(const action_label_list &Acts)
{
  for (process::action_label_list::const_iterator i=Acts.begin(); i!=Acts.end(); ++i)
  {
    action_label Act= *i;
    core::identifier_string ActName=Act.name();
    data::sort_expression_list ActType=Act.sorts();

    check_sort_list_is_declared(ActType);

    const std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> >::const_iterator j=actions.find(ActName);
    atermpp::term_list<data::sort_expression_list> Types;
    if (j==actions.end())
    {
      Types = { ActType };
    }
    else
    {
      Types=j->second;
      // the table actions contains a list of types for each
      // action name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add

      if (InTypesL(ActType, Types))
      {
        throw mcrl2::runtime_error("double declaration of action " + core::pp(ActName));
      }
      else
      {
        Types = Types + atermpp::term_list<data::sort_expression_list>({ ActType });
      }
    }
    actions[ActName]=Types;
  }
}



mcrl2::process::process_type_checker::process_type_checker(const process_specification &proc_spec)
  : data_type_checker(proc_spec.data())
{
  mCRL2log(log::verbose) << "type checking process specification..." << std::endl;


  mCRL2log(log::debug) << "type checking phase started: " << process::pp(proc_spec) << "" << std::endl;

  ReadInActs(proc_spec.action_labels());

  const std::set<data::variable> glob_vars_set = proc_spec.global_variables();
  AddVars2Table(glob_vars, data::variable_list(glob_vars_set.begin(),glob_vars_set.end()));

  ReadInProcsAndInit(proc_spec.equations(), proc_spec.init());

  mCRL2log(log::debug) << "type checking read-in phase finished" << std::endl;

  TransformActProcVarConst();

  mCRL2log(log::debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;


  mCRL2log(log::debug) << "type checking phase finished" << std::endl;

  process_equation_list type_checked_process_equations=WriteProcs(proc_spec.equations());

  type_checked_process_spec=
                process_specification(type_checked_data_spec,
                                      proc_spec.action_labels(),
                                      data::variable_list(proc_spec.global_variables().begin(),proc_spec.global_variables().end()),
                                      type_checked_process_equations,
                                      proc_bodies[std::pair<core::identifier_string,data::sort_expression_list>(initial_process().name(),
                                                      get_sorts(initial_process().variables()))]
                                     );

  normalize_sorts(type_checked_process_spec,type_checked_process_spec.data());
}

process_expression mcrl2::process::process_type_checker::operator()(const process_expression &d)
{
  return TraverseActProcVarConstP(glob_vars,d);
}

process_specification mcrl2::process::process_type_checker::operator()()
{
  return type_checked_process_spec;
}
