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

bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2);

inline
bool MActIn(core::identifier_string_list MAct, action_name_multiset_list MActs)
{
  //returns true if MAct is in MActs
  for (action_name_multiset_list::const_iterator i=MActs.begin(); i!=MActs.end(); ++i)
    if (MActEq(MAct,i->names()))
    {
      return true;
    }

  return false;
}

inline
bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2)
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

inline
core::identifier_string_list list_minus(const core::identifier_string_list &l, const core::identifier_string_list &m)
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
    assert(m_process_parameters.count(std::pair<core::identifier_string,data::sort_expression_list>(Name, m_data_type_checker.UnwindType(FormParList)))>0);
    const data::variable_list& FormalVars=m_process_parameters[std::pair<core::identifier_string,data::sort_expression_list>(Name,m_data_type_checker.UnwindType(FormParList))];
    return process_instance(process_identifier(Name,FormalVars),FactParList);
  }
}

sorts_list mcrl2::process::process_type_checker::TypeListsIntersect(
                     const sorts_list &TypeListList1,
                     const sorts_list &TypeListList2)
{
  // returns the intersection of the 2 type list lists

  sorts_list Result;

  for (sorts_list::const_iterator i=TypeListList2.begin(); i!=TypeListList2.end(); ++i)
  {
    const data::sort_expression_list TypeList2= *i;
    if (m_data_type_checker.InTypesL(TypeList2,TypeListList1))
    {
      Result.push_front(TypeList2);
    }
  }
  return atermpp::reverse(Result);
}


data::sort_expression_list mcrl2::process::process_type_checker::GetNotInferredList(const sorts_list &TypeListList)
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

  for (sorts_list::const_iterator j=TypeListList.begin(); j!=TypeListList.end(); ++j)
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
    return m_data_type_checker.InTypesA(Type,s.sorts());
  }

  //PosType is a normal type
  return m_data_type_checker.EqTypesA(Type,PosType);
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
    if (m_data_type_checker.EqTypesA(OldTypeList.front(),Type))
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
            const sorts_list &TypeListList)
{
  // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
  // TypeListList -- List of (Lists of Types)
  // returns: PosTypeList, adjusted to the elements of TypeListList
  // NULL if cannot be ajusted.

  //if PosTypeList has only normal types -- check if it is in TypeListList,
  //if so return PosTypeList, otherwise return false.
  if (!IsNotInferredL(PosTypeList))
  {
    if (m_data_type_checker.InTypesL(PosTypeList,TypeListList))
    {
      return std::make_pair(true,PosTypeList);
    }
    else
    {
      return std::make_pair(false, data::sort_expression_list());
    }
  }

  //Filter TypeListList to contain only compatible with TypeList lists of parameters.
  sorts_list NewTypeListList;
  for (sorts_list::const_iterator i=TypeListList.begin();
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
  sorts_list ParList;

  bool action=false;

  const std::map<core::identifier_string,sorts_list >::const_iterator j=m_actions.find(Name);

  if (j!=m_actions.end())
  {
    ParList=j->second;
    action=true;
  }
  else
  {
    const std::map<core::identifier_string,sorts_list >::const_iterator j=m_equation_sorts.find(Name);
    if (j!=m_equation_sorts.end())
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
      NewPosType=m_data_type_checker.TraverseVarConsTypeD(Vars,Vars,Par,PosType);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(Par) + " as type " + data::pp(m_data_type_checker.ExpandNumTypesDown(PosType)) + " (while typechecking " + core::pp(Name) +
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
        CastedNewPosType=m_data_type_checker.UpCastNumericType(PosType,NewPosType,Par,Vars,Vars,dummy_table,false);
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
