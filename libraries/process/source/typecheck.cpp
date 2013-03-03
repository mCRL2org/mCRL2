// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/process/typecheck.h"


using namespace atermpp;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::log;
using namespace mcrl2::lps;
using namespace mcrl2::data;
using namespace mcrl2::process;

static bool MActEq(identifier_string_list MAct1, identifier_string_list MAct2);

static bool MActIn(identifier_string_list MAct, action_name_multiset_list MActs)
{
  //returns true if MAct is in MActs
  for (action_name_multiset_list::const_iterator i=MActs.begin(); i!=MActs.end(); ++i)
    if (MActEq(MAct,i->names()))
    {
      return true;
    }

  return false;
}

static bool MActEq(identifier_string_list MAct1, identifier_string_list MAct2)
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
  identifier_string Act1=MAct1.front();
  MAct1=MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  identifier_string_list NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    identifier_string Act2=MAct2.front();
    if (Act1==Act2)
    {
      MAct2=reverse(NewMAct2)+MAct2.tail();
      return MActEq(MAct1,MAct2);
    }
    else
    {
      NewMAct2.push_front(Act2);
    }
  }
  return false;
}



static bool IsNotInferredL(sort_expression_list TypeList)
{
  for (; !TypeList.empty(); TypeList=TypeList.tail())
  {
    sort_expression Type=TypeList.front();
    if (is_unknown_sort(Type) || is_multiple_possible_sorts(Type))
    {
      return true;
    }
  }
  return false;
}


static identifier_string_list list_minus(const identifier_string_list &l, const identifier_string_list &m)
{
  identifier_string_list n;
  for (identifier_string_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    if (std::find(m.begin(),m.end(),*i) == m.end())
    {
      n.push_front(*i);
    }
  }
  return reverse(n);
}

static aterm_appl MakeActionOrProc(bool action, aterm_appl Name,
    aterm_list FormParList, aterm_list FactParList)
{
  return (action)?gsMakeAction(gsMakeActId(Name,FormParList),FactParList)
         :gsMakeProcess(gsMakeProcVarId(Name,FormParList),FactParList);
}

term_list<sort_expression_list> mcrl2::process::process_expression_checker::TypeListsIntersect(
                     const term_list<sort_expression_list> &TypeListList1, 
                     const term_list<sort_expression_list> &TypeListList2)
{
  // returns the intersection of the 2 type list lists

  term_list<sort_expression_list> Result;

  for (term_list<sort_expression_list>::const_iterator i=TypeListList2.begin(); i!=TypeListList2.end(); ++i)
  {
    const sort_expression_list TypeList2= *i;
    if (InTypesL(TypeList2,TypeListList1))
    {
      Result.push_front(TypeList2);
    }
  }
  return reverse(Result);
}


sort_expression_list mcrl2::process::process_expression_checker::GetNotInferredList(const term_list<sort_expression_list> &TypeListList)
{
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1

  sort_expression_list Result;
  size_t nFormPars=((aterm_list)TypeListList.front()).size();
  std::vector<sort_expression_list> Pars(nFormPars);
  for (size_t i=0; i<nFormPars; i++)
  {
    Pars[i]=sort_expression_list();
  }

  for (term_list<sort_expression_list>::const_iterator j=TypeListList.begin(); j!=TypeListList.end(); ++j)
  {
    sort_expression_list TypeList=*j;
    for (size_t i=0; i<nFormPars; TypeList=TypeList.tail(),i++)
    {
      Pars[i]=InsertType(Pars[i],TypeList.front());
    }
  }

  for (size_t i=nFormPars; i>0; i--)
  {
    aterm_appl Sort;
    if (Pars[i-1].size()==1)
    {
      Sort=Pars[i-1].front();
    }
    else
    {
      Sort=multiple_possible_sorts(sort_expression_list(reverse(Pars[i-1])));
    }
    Result.push_front(Sort);
  }
  return Result;
}

bool mcrl2::process::process_expression_checker::IsTypeAllowedA(const sort_expression &Type, const sort_expression &PosType)
{
  //Checks if Type is allowed by PosType
  if (data::is_unknown_sort(data::sort_expression(PosType)))
  {
    return true;
  }
  if (gsIsSortsPossible(PosType))
  {
    return InTypesA(Type,aterm_cast<const sort_expression_list>(PosType[0]));
  }

  //PosType is a normal type
  return EqTypesA(Type,PosType);
}


bool mcrl2::process::process_expression_checker::IsTypeAllowedL(const sort_expression_list &TypeList, const sort_expression_list PosTypeList)
{
  //Checks if TypeList is allowed by PosTypeList (each respective element)
  assert(TypeList.size()==PosTypeList.size());
  sort_expression_list::const_iterator j=PosTypeList.begin();
  for (sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i,++j)
    if (!IsTypeAllowedA(*i,*j))
    {
      return false;
    }
  return true;
}


sort_expression_list mcrl2::process::process_expression_checker::InsertType(const sort_expression_list TypeList, const sort_expression Type)
{
  for (sort_expression_list OldTypeList=TypeList; !OldTypeList.empty(); OldTypeList=OldTypeList.tail())
  {
    if (EqTypesA(OldTypeList.front(),Type))
    {
      return TypeList;
    }
  }
  sort_expression_list result=TypeList;
  result.push_front(Type);
  return result;
}


std::pair<bool,sort_expression_list> mcrl2::process::process_expression_checker::AdjustNotInferredList(
            const sort_expression_list &PosTypeList, 
            const term_list<sort_expression_list> &TypeListList)
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
      return std::make_pair(false, sort_expression_list());
    }
  }

  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  term_list<sort_expression_list> NewTypeListList;
  for (term_list<sort_expression_list>::const_iterator i=TypeListList.begin();
                    i!=TypeListList.end(); ++i)
  {
    sort_expression_list TypeList= *i;
    if (IsTypeAllowedL(TypeList,PosTypeList))
    {
      NewTypeListList.push_front(TypeList);
    }
  }
  if (NewTypeListList.empty())
  {
    return std::make_pair(false, sort_expression_list());
  }
  if (NewTypeListList.size()==1)
  {
    return std::make_pair(true,NewTypeListList.front());
  }

  // otherwise return not inferred.
  return std::make_pair(true,GetNotInferredList(reverse(NewTypeListList)));
}



process_expression mcrl2::process::process_expression_checker::RewrActProc(const std::map<core::identifier_string,sort_expression> &Vars, process_expression ProcTerm)
{
  aterm_appl Result;
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
    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=processes.find(Name);
    if (j!=processes.end()) 
    {
      ParList=j->second;
      action=false;
    }
    else
    {
      throw mcrl2::runtime_error("action or process " + pp(Name) + " not declared");
    }
  }
  assert(!ParList.empty());

  size_t nFactPars=aterm_cast<aterm_list>(ProcTerm[1]).size();
  const std::string msg=(action)?"action":"process";

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
    throw mcrl2::runtime_error("no " + msg + " " + pp(Name)
                    + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                    + " is declared (while typechecking " + pp(ProcTerm) + ")");
  }

  if (ParList.size()==1)
  {
    Result=MakeActionOrProc(action,Name,ParList.front(),aterm_cast<aterm_list>(ProcTerm[1]));
  }
  else
  {
    // we need typechecking to find the correct type of the action.
    // make the list of possible types for the parameters
    Result=MakeActionOrProc(action,Name,GetNotInferredList(ParList),aterm_cast<aterm_list>(ProcTerm[1]));
  }

  //process the arguments

  //possible types for the arguments of the action. (not inferred if ambiguous action).
  sort_expression_list PosTypeList=aterm_cast<sort_expression_list>(aterm_cast<aterm_appl>(Result[0])[1]);

  aterm_list NewPars;
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
    NewPars=variable_list();
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
      throw mcrl2::runtime_error("no " + msg + " " + pp(Name) + "with type " + pp(NewPosTypeList) + " is declared (while typechecking " + pp(ProcTerm) + ")");
    }
  }

  if (IsNotInferredL(PosTypeList))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + pp(Name));
  }

  Result=MakeActionOrProc(action,Name,PosTypeList,NewPars);

  return Result;
}


process_expression mcrl2::process::process_expression_checker::TraverseActProcVarConstP(
           const std::map<core::identifier_string,sort_expression> &Vars, 
           const process_expression &ProcTerm)
{
  size_t n = ProcTerm.size();
  if (n==0)
  {
    return ProcTerm;
  }

  //Here the code for short-hand assignments begins.
  if (is_process_instance_assignment(ProcTerm))
  {
    const process_instance_assignment proc_assignment_term(ProcTerm);
    mCRL2log(debug) << "typechecking a process call with short-hand assignments " << proc_assignment_term << "" << std::endl;
    core::identifier_string Name=proc_assignment_term.identifier().name();
    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=processes.find(Name);
    
    if (j==processes.end())
    {
      throw mcrl2::runtime_error("process " + pp(Name) + " not declared");
    }

    term_list <sort_expression_list> ParList=j->second;
    // Put the assignments into a table
    std::map <variable,data_expression> As;    // variable -> expression (both untyped, still)
    const assignment_list &al=proc_assignment_term.assignments();
    for (assignment_list::const_iterator l=al.begin(); l!=al.end(); ++l)
    {
      const assignment a= *l;
      const std::map <variable,data_expression>::const_iterator i=As.find(a.lhs());
      if (i!=As.end()) // An assignment of the shape x:=t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double assignment to variable " + pp(aterm_cast<aterm_appl>(a[0])) + " (detected assigned values are " + pp(i->second) + " and " + pp(aterm_cast<aterm_appl>(a[1])) + ")");
      }
      As[aterm_cast<aterm_appl>(a[0])]=aterm_cast<aterm_appl>(a[1]);
    }

    {
      // Now filter the ParList to contain only the processes with parameters in this process call with assignments
      term_list <sort_expression_list> NewParList;
      assert(!ParList.empty());
      aterm_appl Culprit; // Variable used for more intelligible error messages.
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        sort_expression_list Par=ParList.front();

        // get the formal parameter names
        variable_list FormalPars=proc_pars[process_identifier(Name,Par)];
        // we only need the names of the parameters, not the types
        identifier_string_list FormalParNames;
        for (; !FormalPars.empty(); FormalPars=FormalPars.tail())
        {
          FormalParNames.push_front(FormalPars.front().name());
        }

        identifier_string_list As_lhss;
        for(std::map <variable,data_expression> ::const_iterator i=As.begin(); i!=As.end(); ++i)
        {
          As_lhss.push_front(i->first.name());
        }
        identifier_string_list l=list_minus(As_lhss,FormalParNames);
        if (l.empty())
        {
          NewParList.push_front(Par);
        }
        else
        {
          Culprit=l.front();
        }
      }
      ParList=reverse(NewParList);

      if (ParList.empty())
      {
        throw mcrl2::runtime_error("no process " + pp(Name) + " containing all assignments in " + pp(proc_assignment_term) + ".\n" + "Problematic variable is " + pp(Culprit) + ".");
      }
      if (!ParList.tail().empty())
      {
        throw mcrl2::runtime_error("ambiguous process " + pp(Name) + " containing all assignments in " + pp(proc_assignment_term) + ".");
      }
    }

    // get the formal parameter names
    variable_list ActualPars;
    variable_list FormalPars=proc_pars[process_identifier(Name,ParList.front())];
    {
      // we only need the names of the parameters, not the types
      for (variable_list l=FormalPars; !l.empty(); l= l.tail())
      {
        identifier_string FormalParName=l.front().name();
        aterm_appl ActualPar;
        const std::map <variable,data_expression> ::const_iterator i=As.find(FormalParName);
        if (i==As.end())  // Not found.
        {
          ActualPar=gsMakeId(FormalParName);
        }
        else
        { 
          ActualPar=i->second;
        }
        ActualPars.push_front(ActualPar);
      }
      ActualPars=reverse(ActualPars);
    }

    aterm_appl TypeCheckedProcTerm;
    try 
    {
      TypeCheckedProcTerm=RewrActProc(Vars, gsMakeParamId(Name,ActualPars));
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking the process call with short-hand assignments " + pp(proc_assignment_term));
    }

    //reverse the assignments
    As.clear();
    for (variable_list l=aterm_cast<variable_list>(TypeCheckedProcTerm[1]),m=FormalPars; !l.empty(); l=l.tail(),m=m.tail())
    {
      variable act_par=l.front();
      variable form_par=m.front();
      /* if (form_par==act_par) This removal is unsound. Consider P(x:D)=sum x:D.a(x).P(x=x); Here the x=x cannot be removed, as the first
                                x refers to the parameter of the process, and the second refers to the bound x in the sum.
      {
        continue;  //parameter does not change 
      } */
      As[aterm_cast<variable>(form_par[0])]=gsMakeDataVarIdInit(form_par,act_par);
    }

    assignment_list TypedAssignments;
    for (assignment_list::const_iterator l=proc_assignment_term.assignments().begin(); l!=proc_assignment_term.assignments().end(); ++l)
    {
      const assignment a= *l;
      const std::map <variable,data_expression> ::const_iterator i=As.find(aterm_cast<aterm_appl>(a[0]));
      if (i==As.end())
      {
        continue;
      }
      TypedAssignments.push_front(i->second);
    }
    TypedAssignments=reverse(TypedAssignments);

    return gsMakeProcessAssignment(aterm_cast<aterm_appl>(TypeCheckedProcTerm[0]),TypedAssignments);
  }
  //Here the section dealing with assignments ends.

  if (gsIsParamId(ProcTerm))
  {
    return RewrActProc(Vars,ProcTerm);
  }

  if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm) ||
      gsIsRename(ProcTerm) || gsIsComm(ProcTerm) || gsIsAllow(ProcTerm))
  {

    //block & hide
    if (gsIsBlock(ProcTerm) || gsIsHide(ProcTerm))
    {
      const std::string msg=gsIsBlock(ProcTerm)?"Blocking":"Hiding";
      core::identifier_string_list ActList=aterm_cast<core::identifier_string_list>(ProcTerm[0]);
      if (ActList.empty())
      {
        mCRL2log(warning) << msg << " empty set of actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }

      std::set<aterm_appl> Acts;
      for (; !ActList.empty(); ActList=ActList.tail())
      {
        core::identifier_string Act=ActList.front();

        //Actions must be declared
        if (actions.count(Act)==0)
        {
          throw mcrl2::runtime_error(msg + " an undefined action " + pp(Act) + " (typechecking " + pp(ProcTerm) + ")");
        }
        if (!Acts.insert(Act).second)  // The action was already in the set.
        {
          mCRL2log(warning) << msg << " action " << pp(Act) << " twice (typechecking " << pp(ProcTerm) << ")" << std::endl;
        }
      }
    }

    //rename
    if (gsIsRename(ProcTerm))
    {
      rename_expression_list RenList=aterm_cast<rename_expression_list>(ProcTerm[0]);

      if (RenList.empty())
      {
        mCRL2log(warning) << "renaming empty set of actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }

      std::set<aterm_appl> ActsFrom;

      for (; !RenList.empty(); RenList=RenList.tail())
      {
        rename_expression Ren=RenList.front();
        core::identifier_string ActFrom=aterm_cast<core::identifier_string>(Ren[0]);
        aterm_appl ActTo=aterm_cast<aterm_appl>(Ren[1]);

        if (ActFrom==ActTo)
        {
          mCRL2log(warning) << "renaming action " << pp(ActFrom) << " into itself (typechecking " << pp(ProcTerm) << ")" << std::endl;
        }

        //Actions must be declared and of the same types
        term_list<sort_expression_list> TypesFrom,TypesTo;
        const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j_from=actions.find(ActFrom);
        if (j_from==actions.end())
        {
          throw mcrl2::runtime_error("renaming an undefined action " + pp(ActFrom) + " (typechecking " + pp(ProcTerm) + ")");
        }
        TypesFrom=j_from->second;
        const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j_to=actions.find(ActFrom);
        if (j_to==actions.end())
        {
          throw mcrl2::runtime_error("renaming into an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
        }
        TypesTo=j_to->second;

        TypesTo=TypeListsIntersect(TypesFrom,TypesTo);
        if (TypesTo.empty())
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " into action " + pp(ActTo) + ": these two have no common type (typechecking " + pp(ProcTerm) + ")");
        }

        if (!ActsFrom.insert(ActFrom).second) // The element was already in the set.
        {
          throw mcrl2::runtime_error("renaming action " + pp(ActFrom) + " twice (typechecking " + pp(ProcTerm) + ")");
        }
      }
    }

    //comm: like renaming multiactions (with the same parameters) to action/tau
    if (gsIsComm(ProcTerm))
    {
      communication_expression_list CommList=aterm_cast<communication_expression_list>(ProcTerm[0]);

      if (CommList.empty())
      {
        mCRL2log(warning) << "synchronizing empty set of (multi)actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }
      else
      {
        aterm_list ActsFrom;

        for (; !CommList.empty(); CommList=CommList.tail())
        {
          communication_expression Comm=CommList.front();
          aterm_list MActFrom=aterm_cast<aterm_list>(aterm_cast<aterm_appl>(Comm[0])[0]);
          aterm_list BackupMActFrom=MActFrom;
          assert(!MActFrom.empty());
          core::identifier_string ActTo=aterm_cast<core::identifier_string>(Comm[1]);

          if (MActFrom.size()==1)
          {
            throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + pp(MActFrom.front()) + " into " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
          }

          //Actions must be declared
          term_list<sort_expression_list> ResTypes;

          if (!gsIsNil(ActTo))
          {
            const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=actions.find(ActTo);
            if (j==actions.end())
            {
              throw mcrl2::runtime_error("synchronizing to an undefined action " + pp(ActTo) + " (typechecking " + pp(ProcTerm) + ")");
            }
            ResTypes=j->second;
          }

          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            core::identifier_string Act=aterm_cast<core::identifier_string>(MActFrom.front());
            const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=actions.find(Act);
            term_list<sort_expression_list> Types;
            if (j==actions.end())
            {
              throw mcrl2::runtime_error("synchronizing an undefined action " + pp(Act) + " in (multi)action " + pp(MActFrom) + " (typechecking " + pp(ProcTerm) + ")");
            }
            Types=j->second;
            ResTypes=TypeListsIntersect(ResTypes,Types);
            if (ResTypes.empty())
            {
              throw mcrl2::runtime_error("synchronizing action " + pp(Act) + " from (multi)action " + pp(BackupMActFrom) + " into action " + pp(ActTo) + ": these have no common type (typechecking " + pp(ProcTerm) + ")");
            }
          }
          MActFrom=BackupMActFrom;

          //the multiactions in the lhss of comm should not intersect.
          //make the list of unique actions
          aterm_list Acts;
          for (; !MActFrom.empty(); MActFrom=MActFrom.tail())
          {
            aterm_appl Act=aterm_cast<aterm_appl>(MActFrom.front());
            if (std::find(Acts.begin(),Acts.end(),Act)==Acts.end())
            {
              Acts.push_front(Act);
            }
          }
          for (; !Acts.empty(); Acts=Acts.tail())
          {
            aterm_appl Act=aterm_cast<aterm_appl>(Acts.front());
            if (std::find(ActsFrom.begin(),ActsFrom.end(),Act)!=ActsFrom.end())
            {
              throw mcrl2::runtime_error("synchronizing action " + pp(Act) + " in different ways (typechecking " + pp(ProcTerm) + ")");
            }
            else
            {
              ActsFrom.push_front(Act);
            }
          }
        }
      }
    }

    //allow
    if (is_allow(ProcTerm))
    {
      const allow &t=aterm_cast<const allow>(ProcTerm);
      action_name_multiset_list MActList=t.allow_set();

      if (MActList.empty())
      {
        mCRL2log(warning) << "allowing empty set of (multi) actions (typechecking " << pp(ProcTerm) << ")" << std::endl;
      }
      else
      {
         action_name_multiset_list MActs;

        for (action_name_multiset_list::const_iterator i=MActList.begin(); i!=MActList.end(); ++i)
        {
          identifier_string_list MAct=i->names();

          //Actions must be declared
          for (identifier_string_list::const_iterator j=MAct.begin(); j!=MAct.end(); ++j)
          {
            identifier_string Act= *j;
            if (actions.count(Act)==0)
            {
              throw mcrl2::runtime_error("allowing an undefined action " + pp(Act) + " in (multi)action " + pp(MAct) + " (typechecking " + pp(ProcTerm) + ")");
            }
          }

          if (MActIn(MAct,MActs))
          {
            mCRL2log(warning) << "allowing (multi)action " << pp(MAct) << " twice (typechecking " << pp(ProcTerm) << ")" << std::endl;
          }
          else
          {
            MActs.push_front(MAct);
          }
        }
      }
    }

    aterm_appl NewProc=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    process_expression a=ProcTerm;
    a.set_argument(NewProc,1);
    return a;
  }

  if (gsIsSync(ProcTerm) || gsIsSeq(ProcTerm) || gsIsBInit(ProcTerm) ||
      gsIsMerge(ProcTerm) || gsIsLMerge(ProcTerm) || gsIsChoice(ProcTerm))
  {
    process_expression NewLeft=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    process_expression NewRight=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    process_expression a=ProcTerm;
    a.set_argument(NewLeft,0).set_argument(NewRight,1);
    return a;
  }

  if (gsIsAtTime(ProcTerm))
  {
    aterm_appl NewProc=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[0]));
    data_expression Time=aterm_cast<data_expression>(ProcTerm[1]);
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
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real");
      }
    }

    return gsMakeAtTime(NewProc,Time);
  }

  if (gsIsIfThen(ProcTerm))
  {
    data_expression Cond=aterm_cast<aterm_appl>(ProcTerm[0]);
    aterm_appl NewType=TraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    aterm_appl NewThen=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(ProcTerm[1]));
    return gsMakeIfThen(Cond,NewThen);
  }

  if (is_if_then_else(ProcTerm))
  {
    const if_then_else t=aterm_cast<const if_then_else>(ProcTerm);
    data_expression Cond=t.condition();
    data_expression NewType=TraverseVarConsTypeD(Vars,Vars,Cond,sort_bool::bool_());
    process_expression NewThen=TraverseActProcVarConstP(Vars,t.then_case());
    process_expression NewElse=TraverseActProcVarConstP(Vars,t.else_case());
    return if_then_else(Cond,NewThen,NewElse);
  }

  if (is_sum(ProcTerm))
  {
    const sum t=aterm_cast<const sum>(ProcTerm);
    std::map<identifier_string,sort_expression> CopyVars;
    CopyVars=Vars;

    std::map<identifier_string,sort_expression> NewVars;
    try 
    {
      AddVars2Table(CopyVars,t.bound_variables(),NewVars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pp(ProcTerm));
    }
    process_expression NewProc;
    try
    {
      NewProc=TraverseActProcVarConstP(NewVars,t.operand());
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pp(ProcTerm));
    }
    return sum(t.bound_variables(),NewProc);
  }

  throw mcrl2::runtime_error("Internal error. Process " + pp(ProcTerm) + " fails to match known processes.");
}


void mcrl2::process::process_expression_checker::TransformActProcVarConst(void)
{
  std::map<core::identifier_string,sort_expression> Vars;

  //process and data terms in processes and init
  for (std::map <process_identifier,variable_list>::const_iterator i=proc_pars.begin(); i!=proc_pars.end(); ++i)
  {
    const process_identifier ProcVar=i->first;

    Vars=glob_vars;

    std::map<core::identifier_string,sort_expression> NewVars;
    AddVars2Table(Vars,i->second,NewVars);
    Vars=NewVars;

    const process_expression NewProcTerm=TraverseActProcVarConstP(Vars,aterm_cast<aterm_appl>(proc_bodies[ProcVar]));
    proc_bodies[ProcVar]=NewProcTerm;
  }
}



void mcrl2::process::process_expression_checker::ReadInProcsAndInit(const std::vector<process_equation>& Procs, const process_expression &Init)
{
  for (std::vector<process_equation>::const_iterator i=Procs.begin(); i!=Procs.end(); ++i)
  {
    const process_equation &Proc= *i;
    core::identifier_string ProcName=Proc.identifier().name();

    if (actions.count(ProcName)>0)
    {
      throw mcrl2::runtime_error("declaration of both process and action " + pp(ProcName));
    }

    const sort_expression_list &ProcType=Proc.identifier().sorts();

    IsSortExprListDeclared(ProcType);

    const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=processes.find(ProcName);
    term_list<sort_expression_list> Types;
    if (j==processes.end())
    {
      Types=make_list<sort_expression_list>(ProcType);
    }
    else
    {
      Types=j->second;
      // the table processes contains a list of types for each
      // process name. We need to check if there is already such a type
      // in the list. If so -- error, otherwise -- add
      if (InTypesL(ProcType, Types))
      {
        throw mcrl2::runtime_error("double declaration of process " + pp(ProcName));
      }
      else
      {
        Types=Types+make_list<sort_expression_list>(ProcType);
      }
    }
    processes[ProcName]=Types;

    //check that all formal parameters of the process are unique.
    const variable_list ProcVars=Proc.formal_parameters();
    if (!VarsUnique(ProcVars))
    {
      throw mcrl2::runtime_error("the formal variables in process " + pp(Proc) + " are not unique");
    }

    proc_pars[Proc.identifier()]=Proc.formal_parameters();
    proc_bodies[Proc.identifier()]=Proc.expression();
  }
  proc_pars[initial_process()]=variable_list();
  proc_bodies[initial_process()]=Init;

}

void mcrl2::process::process_expression_checker::ReadInActs(const action_label_list &Acts)
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
        throw mcrl2::runtime_error("double declaration of action " + pp(ActName));
      }
      else
      {
        Types=Types+make_list<sort_expression_list>(ActType);
      }
    }
    actions[ActName]=Types;
  }
}



mcrl2::process::process_expression_checker::process_expression_checker(const process_specification &proc_spec)
  : data_expression_checker(proc_spec.data())
{
  mCRL2log(verbose) << "type checking process specification..." << std::endl;


  mCRL2log(debug) << "type checking phase started: " << pp(proc_spec) << "" << std::endl;

  // Check sorts for loops
  // Unwind sorts to enable equiv and subtype relations

  ReadInActs(proc_spec.action_labels());
 
  const std::set<data::variable> glob_vars_set = proc_spec.global_variables();
  std::map<core::identifier_string,sort_expression> dummy;
  AddVars2Table(glob_vars, variable_list(glob_vars_set.begin(),glob_vars_set.end()),dummy);
 
  ReadInProcsAndInit(proc_spec.equations(), proc_spec.init());
 
  mCRL2log(debug) << "type checking read-in phase finished" << std::endl;

  TransformActProcVarConst();

  mCRL2log(debug) << "type checking transform ActProc+VarConst phase finished" << std::endl;

  // aterm_appl Result;
  // data_spec=aterm_cast<aterm_appl>(proc_spec[0]);
  // data_spec=data_spec.set_argument(gsMakeDataEqnSpec(equations),3);
  // Result=proc_spec.set_argument(data_spec,0);
  // Result=Result.set_argument(gsMakeProcEqnSpec(gstcWriteProcs(aterm_cast<aterm_list>(aterm_cast<aterm_appl>(proc_spec[3])[0]))),3);
  // Result=Result.set_argument(gsMakeProcessInit(aterm_cast<aterm_appl>(proc_bodies[initial_process()])),4);

  // Result=gstcFoldSortRefs(Result);

  mCRL2log(debug) << "type checking phase finished" << std::endl;


  // return Result;
}

process_expression mcrl2::process::process_expression_checker::operator()(const process_expression &d)
{
  return TraverseActProcVarConstP(glob_vars,d);
}
