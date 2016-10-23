// Author(s): Yaroslav Usenko, Jan Friso Groote, Wieger Wesselink (2015)
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cctype>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/standard_container_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/untyped_sort.h"
#include "mcrl2/data/untyped_possible_sorts.h"
#include "mcrl2/data/function_update.h"

using namespace mcrl2::log;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace atermpp;

namespace mcrl2
{
namespace data
{
namespace detail
{

static std::map<core::identifier_string,sort_expression> RemoveVars(std::map<core::identifier_string,sort_expression>& Vars, variable_list VarDecls);

static inline bool IsPos(const core::identifier_string& Number)
{
  char c=Number.function().name()[0];
  return isdigit(c) && c>'0';
}
static inline bool IsNat(const core::identifier_string& Number)
{
  return isdigit(Number.function().name()[0]) != 0;
}

static sort_expression_list GetVarTypes(variable_list VarDecls);
static bool HasUnknown(const sort_expression& Type);
static bool IsNumericType(const sort_expression& Type);
static sort_expression MinType(const sort_expression_list& TypeList);
static sort_expression replace_possible_sorts(const sort_expression& Type);

// Insert an element in the list provided, it did not already occur in the list.
template<class S>
inline atermpp::term_list<S> insert_sort_unique(const atermpp::term_list<S>& list, const S& el)
{
  if (std::find(list.begin(),list.end(), el) == list.end())
  {
    atermpp::term_list<S> result=list;
    result.push_front(el);
    return result;
  }
  return list;
}

} // namespace detail
} // namespace data

// ------------------------------  Here starts the new class based data expression checker -----------------------

// The function below is used to check whether a term is well typed.
// It always yields true, but if the dataterm is not properly typed, using the types
// that are included inside the term it calls an assert. This function is useful to check
// whether typing was succesful, using assert(strict_type_check(d)).

bool mcrl2::data::data_type_checker::strict_type_check(const data_expression& d)
{
  if (is_abstraction(d))
  {
    const abstraction& abstr=down_cast<const abstraction>(d);
    assert(abstr.variables().size()>0);
    const binder_type& BindingOperator = abstr.binding_operator();

    if (is_forall_binder(BindingOperator) || is_exists_binder(BindingOperator))
    {
      assert(d.sort()==sort_bool::bool_());
      strict_type_check(abstr.body());
    }

    if (is_lambda_binder(BindingOperator))
    {
      strict_type_check(abstr.body());
    }
    return true;
  }

  if (is_where_clause(d))
  {
    const where_clause& where=down_cast<const where_clause>(d);
    const assignment_expression_list& where_asss=where.declarations();
    for (assignment_expression_list::const_iterator i=where_asss.begin(); i!=where_asss.end(); ++i)
    {
      const assignment_expression WhereElem= *i;
      const assignment& t=down_cast<const assignment>(WhereElem);
      strict_type_check(t.rhs());
    }
    strict_type_check(where.body());
    return true;
  }

  if (is_application(d))
  {
    const application& appl=down_cast<application>(d);
    const data_expression& head = appl.head();

    if (data::is_function_symbol(head))
    {
      core::identifier_string name = function_symbol(head).name();
      if (name == sort_list::list_enumeration_name())
      {
        const sort_expression s=d.sort();
        assert(sort_list::is_list(s));
        const sort_expression s1=container_sort(s).element_sort();

        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          strict_type_check(*i);
          assert(i->sort()==s1);

        }
        return true;
      }
      if (name == sort_set::set_enumeration_name())
      {
        const sort_expression s=d.sort();
        assert(sort_fset::is_fset(s));
        const sort_expression s1=container_sort(s).element_sort();

        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          strict_type_check(*i);
          assert(i->sort()==s1);

        }
        return true;
      }
      if (name == sort_bag::bag_enumeration_name())
      {
        const sort_expression s=d.sort();
        assert(sort_fbag::is_fbag(s));
        const sort_expression s1=container_sort(s).element_sort();

        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          strict_type_check(*i);
          assert(i->sort()==s1);
          // Every second element in a bag enumeration should be of type Nat.
          ++i;
          strict_type_check(*i);
          assert(i->sort()==sort_nat::nat());

        }
        return true;

      }
    }
    strict_type_check(head);
    const sort_expression& s=head.sort();
    assert(is_function_sort(s));
    assert(d.sort()==function_sort(s).codomain());
    sort_expression_list argument_sorts=function_sort(s).domain();
    assert(appl.size()==argument_sorts.size());
    application::const_iterator j=appl.begin();
    for(sort_expression_list::const_iterator i=argument_sorts.begin(); i!=argument_sorts.end(); ++i,++j)
    {
      assert(UnwindType(j->sort())==UnwindType(*i));
      strict_type_check(*j);
    }
    return true;
  }

  if (data::is_function_symbol(d)||is_variable(d))
  {
    return true;
  }

  assert(0); // Unexpected data_expression.
  return true;
}

bool mcrl2::data::data_type_checker::VarsUnique(const variable_list& VarDecls)
{
  std::set<core::identifier_string> Temp;

  for (const variable& VarDecl: VarDecls)
  {
    const core::identifier_string& VarName=VarDecl.name();
    // if already defined -- replace (other option -- warning)
    // if variable name is a constant name -- it has more priority (other options -- warning, error)
    if (!Temp.insert(VarName).second) // The VarName is already in the set.
    {
      return false;
    }
  }

  return true;
}

sort_expression mcrl2::data::data_type_checker::UpCastNumericType(
                      sort_expression NeededType,
                      sort_expression Type,
                      data_expression& Par,
                      const std::map<core::identifier_string,sort_expression>& DeclaredVars,
                      const std::map<core::identifier_string,sort_expression>& AllowedVars,
                      std::map<core::identifier_string,sort_expression>& FreeVars,
                      const bool strictly_ambiguous,
                      bool warn_upcasting,
                      const bool print_cast_error)
{
  // Makes upcasting from Type to Needed Type for Par. Returns the resulting type.
  // Moreover, *Par is extended with the required type transformations.

  if (data::is_untyped_sort(Type))
  {
    return Type;
  }
  if (data::is_untyped_sort(NeededType))
  {
    return Type;
  }

  // Added to make sure that the types are sufficiently unrolled, because this function is not always called
  // with unrolled types.
  NeededType=UnwindType(NeededType);
  Type=UnwindType(Type);

  if (EqTypesA(NeededType,Type))
  {
    return Type;
  }

  if (data::is_untyped_possible_sorts(NeededType))
  {
    untyped_possible_sorts mps(NeededType);
    const sort_expression_list& l=mps.sorts();
    for(sort_expression_list::const_iterator i=l.begin(); i!=l.end(); ++i)
    {
      bool found_solution=true;
      sort_expression r;
      try
      {
        r=UpCastNumericType(*i,Type,Par,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error&)
      {
        found_solution=false;
      }
      if (found_solution)
      {
        return r;
      }
    }
    throw mcrl2::runtime_error("Cannot transform " + data::pp(Type) + " to a number.");
  }

  if (warn_upcasting && data::is_function_symbol(Par) && utilities::is_numeric_string(down_cast<function_symbol>(Par).name().function().name()))
  {
    warn_upcasting=false;
  }

  // Try Upcasting to Pos
  sort_expression temp;
  if (TypeMatchA(NeededType,sort_pos::pos(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      return sort_pos::pos();
    }
  }

  // Try Upcasting to Nat
  if (TypeMatchA(NeededType,sort_nat::nat(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_nat::cnat(),Par);
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Nat by applying Pos2Nat to it." << std::endl;
      }
      return sort_nat::nat();
    }
    if (TypeMatchA(Type,sort_nat::nat(),temp))
    {
      return sort_nat::nat();
    }
  }

  // Try Upcasting to Int
  if (TypeMatchA(NeededType,sort_int::int_(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_int::cint(),application(sort_nat::cnat(),Par));
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Int by applying Pos2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (TypeMatchA(Type,sort_nat::nat(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_int::cint(),Par);
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Int by applying Nat2Int to it." << std::endl;
      }
      return sort_int::int_();
    }
    if (TypeMatchA(Type,sort_int::int_(),temp))
    {
      return sort_int::int_();
    }
  }

  // Try Upcasting to Real
  if (TypeMatchA(NeededType,sort_real::real_(),temp))
  {
    if (TypeMatchA(Type,sort_pos::pos(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_real::creal(),
                              application(sort_int::cint(), application(sort_nat::cnat(),Par)),
                              sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Real by applying Pos2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (TypeMatchA(Type,sort_nat::nat(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_real::creal(),
                             application(sort_int::cint(),Par),
                             sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Real by applying Nat2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (TypeMatchA(Type,sort_int::int_(),temp))
    {
      data_expression OldPar=Par;
      Par=application(sort_real::creal(),Par, sort_pos::c1());
      if (warn_upcasting)
      {
        was_warning_upcasting=true;
        mCRL2log(warning) << "Upcasting " << OldPar << " to sort Real by applying Int2Real to it." << std::endl;
      }
      return sort_real::real_();
    }
    if (TypeMatchA(Type,sort_real::real_(),temp))
    {
      return sort_real::real_();
    }
  }

  // If NeededType and Type are both container types, try to upcast the argument.
  if (is_container_sort(NeededType) && is_container_sort(Type))
  {
    const container_sort needed_container_type(NeededType);
    const container_sort container_type(Type);
    sort_expression needed_argument_type=needed_container_type.element_sort();
    const sort_expression& argument_type=container_type.element_sort();
    if (is_untyped_sort(needed_argument_type))
    {
      needed_argument_type=argument_type;
    }
    const sort_expression needed_similar_container_type=container_sort(container_type.container_name(),needed_argument_type);
    if (needed_similar_container_type==NeededType)
    {
      throw mcrl2::runtime_error("Cannot typecast " + data::pp(Type) + " into " + data::pp(NeededType) + " for data expression " + data::pp(Par) + ".");
    }
    try
    {
      Type=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Par,
                   needed_similar_container_type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      assert(UnwindType(Type)==UnwindType(needed_similar_container_type));

    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nError occurred while trying to match argument types of " + data::pp(NeededType) + " and " +
                          data::pp(Type) + " in data expression " + data::pp(Par) + ".");
    }
  }


  // If is is an fset, try upcasting to a set.
  if (is_container_sort(NeededType) && is_set_container(container_sort(NeededType).container_name()))
  {
    if (is_container_sort(Type) && is_fset_container(container_sort(Type).container_name()))
    {
      Par=sort_set::constructor(container_sort(NeededType).element_sort(),sort_set::false_function(container_sort(NeededType).element_sort()),Par);
      // Do this again to lift argument types if needed. TODO.
      return NeededType;
    }
    else if (is_container_sort(Type) && is_set_container(container_sort(Type).container_name()))
    {
      if (Type==NeededType)
      {
        return Type;
      }
      else
      {
        throw mcrl2::runtime_error("Upcasting " + data::pp(Type) + " to " + data::pp(NeededType) + " fails (1).");
      }
    }
  }

  // If is is an fbag, try upcasting to a bag.
  if (is_container_sort(NeededType) && is_bag_container(container_sort(NeededType).container_name()))
  {
    if (is_container_sort(Type) && is_fbag_container(container_sort(Type).container_name()))
    {
      Par=sort_bag::constructor(container_sort(NeededType).element_sort(),sort_bag::zero_function(container_sort(NeededType).element_sort()),Par);
      // Do this again to lift argument types if needed. TODO.
      return NeededType;
    }
    else if (is_container_sort(Type) && is_bag_container(container_sort(Type).container_name()))
    {
      if (Type==NeededType)
      {
        return Type;
      }
      else
      {
        throw mcrl2::runtime_error("Upcasting " + data::pp(Type) + " to " + data::pp(NeededType) + " fails (1).");
      }
    }
  }

  if (is_function_sort(NeededType))
  {
    const function_sort needed_function_type(NeededType);
    if (is_function_sort(Type))
    {
      // we only deal here with @false_ and @zero (false_function and zero_function).
      if (Par==sort_set::false_function(data::untyped_sort()))
      {
        assert(needed_function_type.domain().size()==1);
        Par=sort_set::false_function(needed_function_type.domain().front());
        return NeededType;
      }
      else if (Par==sort_bag::zero_function(data::untyped_sort()))
      {
        assert(needed_function_type.domain().size()==1);
        Par=sort_bag::zero_function(needed_function_type.domain().front());
        return NeededType;
      }
    }
  }

  throw mcrl2::runtime_error("Upcasting " + data::pp(Type) + " to " + data::pp(NeededType) + " fails (3).");
}


bool mcrl2::data::data_type_checker::UnFSet(sort_expression PosType, sort_expression& result)
{
  //select Set(Type), elements, return their list of arguments.
  if (is_basic_sort(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_fset::is_fset(PosType) || sort_set::is_set(PosType))
  {
    result=down_cast<container_sort>(PosType).element_sort();
    return true;
  }
  if (data::is_untyped_sort(PosType))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (is_untyped_possible_sorts(PosType))
  {
    const untyped_possible_sorts& mps=down_cast<untyped_possible_sorts>(PosType);
    for (sort_expression_list PosTypes=mps.sorts(); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (is_basic_sort(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_fset::is_fset(sort_expression(NewPosType))|| (sort_set::is_set(sort_expression(NewPosType))))
      {
        NewPosType=down_cast<const container_sort>(NewPosType).element_sort();
      }
      else if (!data::is_untyped_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=untyped_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_type_checker::UnFBag(sort_expression PosType, sort_expression& result)
{
  //select Bag(Type), elements, return their list of arguments.
  if (is_basic_sort(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_fbag::is_fbag(sort_expression(PosType)) || (sort_bag::is_bag(sort_expression(PosType))))
  {
    result=down_cast<const container_sort>(PosType).element_sort();
    return true;
  }
  if (data::is_untyped_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (is_untyped_possible_sorts(PosType))
  {
    const untyped_possible_sorts& mps=down_cast<untyped_possible_sorts>(PosType);
    for (sort_expression_list PosTypes=mps.sorts(); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (is_basic_sort(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_fbag::is_fbag(sort_expression(NewPosType))|| (sort_fbag::is_fbag(sort_expression(NewPosType))))
      {
        NewPosType=down_cast<const container_sort>(NewPosType).element_sort();
      }
      else if (!data::is_untyped_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=untyped_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_type_checker::UnList(sort_expression PosType, sort_expression& result)
{
  //select List(Type), elements, return their list of arguments.
  if (is_basic_sort(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (sort_list::is_list(sort_expression(PosType)))
  {
    result=down_cast<const container_sort>(PosType).element_sort();
    return true;
  }
  if (data::is_untyped_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (is_untyped_possible_sorts(PosType))
  {
    const untyped_possible_sorts& mps=down_cast<untyped_possible_sorts>(PosType);
    for (sort_expression_list PosTypes=mps.sorts(); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (is_basic_sort(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (sort_list::is_list(NewPosType))
      {
        NewPosType=down_cast<const container_sort>(NewPosType).element_sort();
      }
      else if (!data::is_untyped_sort(NewPosType))
      {
        continue;
      }
      NewPosTypes.push_front(NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=untyped_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}


bool mcrl2::data::data_type_checker::UnArrowProd(const sort_expression_list& ArgTypes, sort_expression PosType, sort_expression& result)
{
  //Filter PosType to contain only functions ArgTypes -> TypeX
  //result is TypeX if unique, the set of TypeX as NotInferred if many.
  //return true if successful, otherwise false.

  if (is_basic_sort(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (is_function_sort(PosType))
  {
    const function_sort& s=down_cast<const function_sort>(PosType);
    const sort_expression_list& PosArgTypes=s.domain();

    if (PosArgTypes.size()!=ArgTypes.size())
    {
      return false;
    }
    sort_expression_list temp;
    if (TypeMatchL(PosArgTypes,ArgTypes,temp))
    {
      result=s.codomain();
      return true;
    }
    else
    {
      // Lift the argument of PosType.
      TypeMatchL(ArgTypes,ExpandNumTypesUpL(PosArgTypes),temp);
      result=s.codomain();
      return true;
    }
  }
  if (data::is_untyped_sort(data::sort_expression(PosType)))
  {
    result=PosType;
    return true;
  }

  sort_expression_list NewPosTypes;
  if (is_untyped_possible_sorts(PosType))
  {
    const untyped_possible_sorts& mps=down_cast<untyped_possible_sorts>(PosType);
    for (sort_expression_list PosTypes=mps.sorts(); !PosTypes.empty(); PosTypes=PosTypes.tail())
    {
      sort_expression NewPosType=PosTypes.front();
      if (is_basic_sort(NewPosType))
      {
        NewPosType=UnwindType(NewPosType);
      }
      if (is_function_sort(NewPosType))
      {
        const function_sort& s=down_cast<const function_sort>(NewPosType);
        const sort_expression_list& PosArgTypes=s.domain();
        if (PosArgTypes.size()!=ArgTypes.size())
        {
          continue;
        }
        sort_expression_list temp_list;
        if (TypeMatchL(PosArgTypes,ArgTypes,temp_list))
        {
          NewPosType=s.codomain();
        }
      }
      else if (!data::is_untyped_sort(data::sort_expression(NewPosType)))
      {
        continue;
      }
      NewPosTypes=detail::insert_sort_unique(NewPosTypes,NewPosType);
    }
    NewPosTypes=reverse(NewPosTypes);
    result=untyped_possible_sorts(sort_expression_list(NewPosTypes));
    return true;
  }
  return false;
}

bool mcrl2::data::data_type_checker::UnifyMinType(const sort_expression& Type1, const sort_expression& Type2, sort_expression& result)
{
  //Find the minimal type that Unifies the 2. If not possible, return false.
  if (!TypeMatchA(Type1,Type2,result))
  {
    if (!TypeMatchA(Type1,ExpandNumTypesUp(Type2),result))
    {
      if (!TypeMatchA(Type2,ExpandNumTypesUp(Type1),result))
      {
        return false;
      }
    }
  }

  if (is_untyped_possible_sorts(result))
  {
    result=down_cast<untyped_possible_sorts>(result).sorts().front();
  }
  return true;
}

bool mcrl2::data::data_type_checker::MatchIf(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types for if.
  //If some of the parameters are Pos,Nat, or Int do upcasting

  sort_expression_list Args=type.domain();
  sort_expression Res=type.codomain();
  if (Args.size()!=3)
  {
    return false;
  }
  Args=Args.tail();

  if (!UnifyMinType(Res,Args.front(),Res))
  {
    return false;
  }
  Args=Args.tail();
  if (!UnifyMinType(Res,Args.front(),Res))
  {
    return false;
  }

  result = function_sort({ sort_bool::bool_(), Res, Res }, Res);
  return true;
}

bool mcrl2::data::data_type_checker::MatchEqNeqComparison(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types for ==, !=, <, <=, >= and >.
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }
  sort_expression Arg1=Args.front();
  Args=Args.tail();
  sort_expression Arg2=Args.front();

  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result = function_sort({ Arg, Arg },sort_bool::bool_());
  return true;
}

bool mcrl2::data::data_type_checker::MatchSqrt(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types for sqrt. There is only one option: sqrt:Nat->Nat.

  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }
  const sort_expression& Arg=Args.front();

  if (Arg==sort_nat::nat())
  {
    result=function_sort(Args,sort_nat::nat());
    return true;
  }
  return false;
}



bool mcrl2::data::data_type_checker::MatchListOpCons(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_list::is_list(UnwindType(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();
  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }
  sort_expression Arg1=Args.front();
  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_list::is_list(sort_expression(Arg2)))
  {
    return false;
  }
  Arg2=down_cast<container_sort>(Arg2).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=function_sort({ Res,sort_list::list(sort_expression(Res)) },sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_type_checker::MatchListOpSnoc(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_list::is_list(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();
  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }
  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (!sort_list::is_list(sort_expression(Arg1)))
  {
    return false;
  }
  Arg1=down_cast<container_sort>(Arg1).element_sort();

  Args=Args.tail();
  sort_expression Arg2=Args.front();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=function_sort({ sort_list::list(sort_expression(Res)),Res },sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_type_checker::MatchListOpConcat(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Concat operations (List(S)xList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_list::is_list(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();
  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (!sort_list::is_list(sort_expression(Arg1)))
  {
    return false;
  }
  Arg1=down_cast<container_sort>(Arg1).element_sort();

  Args=Args.tail();

  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_list::is_list(sort_expression(Arg2)))
  {
    return false;
  }
  Arg2=down_cast<container_sort>(Arg2).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }

  if (!UnifyMinType(new_result,Arg2,Res))
  {
    return false;
  }

  result=function_sort({ sort_list::list(sort_expression(Res)), sort_list::list(sort_expression(Res)) },
                       sort_list::list(sort_expression(Res)));
  return true;
}

bool mcrl2::data::data_type_checker::MatchListOpEltAt(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of EltAt operations (List(S)xNat->S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  const sort_expression_list& Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (!sort_list::is_list(sort_expression(Arg1)))
  {
    return false;
  }
  Arg1=down_cast<container_sort>(Arg1).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg1,new_result))
  {
    return false;
  }
  Res=new_result;

  result=function_sort({ sort_list::list(sort_expression(Res)), sort_nat::nat() },Res);
  return true;
}

bool mcrl2::data::data_type_checker::MatchListOpHead(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }
  sort_expression Arg=Args.front();
  if (is_basic_sort(Arg))
  {
    Arg=UnwindType(Arg);
  }
  if (!sort_list::is_list(Arg))
  {
    return false;
  }
  Arg=down_cast<container_sort>(Arg).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=function_sort({ sort_expression(sort_list::list(sort_expression(Res))) },Res);
  return true;
}

bool mcrl2::data::data_type_checker::MatchListOpTail(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Cons operations (SxList(S)->List(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_list::is_list(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();
  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }
  sort_expression Arg=Args.front();
  if (is_basic_sort(Arg))
  {
    Arg=UnwindType(Arg);
  }
  if (!sort_list::is_list(sort_expression(Arg)))
  {
    return false;
  }
  Arg=down_cast<container_sort>(Arg).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Res,Arg,new_result))
  {
    return false;
  }
  Res=new_result;

  result=function_sort(sort_expression_list({ sort_expression(sort_list::list(sort_expression(Res))) }),
                   sort_list::list(sort_expression(Res)));
  return true;
}

//Sets
bool mcrl2::data::data_type_checker::MatchSetOpSet2Bag(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Set2Bag (Set(S)->Bag(s))
  //If some of the parameters are Pos,Nat, or Int do upcasting.


  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_bag::is_bag(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();

  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }

  sort_expression Arg=Args.front();
  if (is_basic_sort(Arg))
  {
    Arg=UnwindType(Arg);
  }
  if (!sort_set::is_set(sort_expression(Arg)))
  {
    return false;
  }
  Arg=down_cast<container_sort>(Arg).element_sort();

  sort_expression new_result;
  if (!UnifyMinType(Arg,Res,new_result))
  {
    return false;
  }
  Arg=new_result;

  result=function_sort(sort_expression_list({ sort_expression(sort_set::set_(sort_expression(Arg))) }),
                  sort_bag::bag(sort_expression(Arg)));
  return true;
}

bool mcrl2::data::data_type_checker::MatchSetConstructor(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of @set (Set(S)->Bool)->FSet(s))->Set(S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.
  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_set::is_set(Res))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();

  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (!is_function_sort(sort_expression(Arg1)))
  {
    return false;
  }

  const sort_expression Arg12=down_cast<function_sort>(Arg1).codomain();

  sort_expression new_result;
  if (!UnifyMinType(Arg12,sort_bool::bool_(),new_result))
  {
    return false;
  }

  const sort_expression_list Arg11l=down_cast<function_sort>(Arg1).domain();
  if (Arg11l.size()!=1)
  {
    return false;
  }
  const sort_expression& Arg11=Arg11l.front();

  if (!UnifyMinType(Arg11,Res,new_result))
  {
    return false;
  }


  Args.pop_front();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_fset::is_fset(Arg2))
  {
    return false;
  }
  sort_expression Arg21=down_cast<container_sort>(Arg2).element_sort();

  sort_expression new_result2;
  if (!UnifyMinType(Arg21,new_result,new_result2))
  {
    return false;
  }

  Arg1=function_sort({ new_result2 }, sort_bool::bool_());
  Arg2=sort_fset::fset(new_result2);
  result=function_sort({ Arg1, Arg2 }, sort_set::set_(new_result2));
  return true;
}

bool mcrl2::data::data_type_checker::MatchFalseFunction(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of @false (S->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  result=type;
  return true;



}

bool mcrl2::data::data_type_checker::MatchBagConstructor(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of @bag (Bag(S)->Bool)->FBag(s))->Bag(S)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_bag::is_bag(Res))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();

  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (!is_function_sort(sort_expression(Arg1)))
  {
    return false;
  }

  const sort_expression Arg12=down_cast<function_sort>(Arg1).codomain();

  sort_expression new_result;
  if (!UnifyMinType(Arg12,sort_nat::nat(),new_result))
  {
    return false;
  }

  const sort_expression_list Arg11l=down_cast<function_sort>(Arg1).domain();
  if (Arg11l.size()!=1)
  {
    return false;
  }
  const sort_expression& Arg11=Arg11l.front();

  if (!UnifyMinType(Arg11,Res,new_result))
  {
    return false;
  }


  Args.pop_front();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_fbag::is_fbag(Arg2))
  {
    return false;
  }
  sort_expression Arg21=down_cast<container_sort>(Arg2).element_sort();

  sort_expression new_result2;
  if (!UnifyMinType(Arg21,new_result,new_result2))
  {
    return false;
  }

  Arg1=function_sort(sort_expression_list({ new_result2 }),sort_nat::nat());
  Arg2=sort_fbag::fbag(new_result2);
  result=function_sort(sort_expression_list({ Arg1, Arg2 }), sort_bag::bag(new_result2));
  return true;
}


bool mcrl2::data::data_type_checker::MatchListSetBagOpIn(const function_sort& type, sort_expression& result)
{
  //tries to sort out the type of EltIn (SxList(S)->Bool or SxSet(S)->Bool or SxBag(S)->Bool)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!is_container_sort(Arg2))
  {
    return false;
  }
  sort_expression Arg2s=down_cast<container_sort>(Arg2).element_sort();
  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2s,Arg))
  {
    return false;
  }

  result=function_sort({ Arg, container_sort(down_cast<const container_sort>(Arg2).container_name(),Arg)},
                       sort_bool::bool_());
  return true;
}

bool mcrl2::data::data_type_checker::match_fset_insert(const function_sort& type, sort_expression& result)
{
  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!is_container_sort(Arg2))
  {
    return false;
  }
  sort_expression Arg2s=down_cast<container_sort>(Arg2).element_sort();
  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2s,Arg))
  {
    return false;
  }

  const sort_expression fset_type=container_sort(down_cast<const container_sort>(Arg2).container_name(),Arg);
  result=function_sort({ Arg, fset_type },fset_type);
  return true;
}

bool mcrl2::data::data_type_checker::match_fbag_cinsert(const function_sort& type, sort_expression& result)
{
  sort_expression_list Args=type.domain();
  if (Args.size()!=3)
  {
    return false;
  }

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  Args=Args.tail();
  sort_expression Arg3=Args.front();
  if (is_basic_sort(Arg3))
  {
    Arg3=UnwindType(Arg3);
  }

  sort_expression Arg2r;
  if (!UnifyMinType(Arg2,sort_nat::nat(),Arg2r))
  {
    return false;
  }

  if (!is_container_sort(Arg3))
  {
    return false;
  }

  sort_expression Arg3s=down_cast<container_sort>(Arg3).element_sort();
  sort_expression Arg3r;
  if (!UnifyMinType(Arg1,Arg3s,Arg3r))
  {
    return false;
  }


  const sort_expression fbag_type=container_sort(down_cast<const container_sort>(Arg3).container_name(),Arg3r);
  result=function_sort({ Arg3r, Arg2r, fbag_type },fbag_type);
  return true;
}

bool mcrl2::data::data_type_checker::MatchSetBagOpUnionDiffIntersect(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Set or Bag Union, Diff or Intersect
  //operations (Set(S)xSet(S)->Set(S)). It can also be that this operation is
  //performed on numbers. In this case we do nothing.
  //If some of the parameters are Pos,Nat, or Int do upcasting.
  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (data::detail::IsNumericType(Res))
  {
    result=type;
    return true;
  }
  if (!(sort_set::is_set(sort_expression(Res))||sort_bag::is_bag(sort_expression(Res))||
        sort_fset::is_fset(sort_expression(Res))||sort_fbag::is_fbag(sort_expression(Res))))
  {
    return false;
  }
  sort_expression_list Args=type.domain();
  if (Args.size()!=2)
  {
    return false;
  }

  sort_expression Arg1=Args.front();
  if (is_basic_sort(Arg1))
  {
    Arg1=UnwindType(Arg1);
  }
  if (data::detail::IsNumericType(Arg1))
  {
    result=type;
    return true;
  }
  if (!(sort_set::is_set(sort_expression(Arg1))||sort_bag::is_bag(sort_expression(Arg1))||
        sort_fset::is_fset(sort_expression(Arg1))||sort_fbag::is_fbag(sort_expression(Arg1))))
  {
    return false;
  }

  Args=Args.tail();

  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (detail::IsNumericType(Arg2))
  {
    result=type;
    return true;
  }
  if (!(sort_set::is_set(sort_expression(Arg2))||sort_bag::is_bag(sort_expression(Arg2))||
        sort_fset::is_fset(sort_expression(Arg2))||sort_fbag::is_fbag(sort_expression(Arg2))))
  {
    return false;
  }

  // If one of the argumenst is an fset/fbag and the other a set/bag, lift it to match the bag/set.
  if (sort_set::is_set(sort_expression(Arg1)) && sort_fset::is_fset(sort_expression(Arg2)))
  {
    Arg2=sort_set::set_(container_sort(Arg2).element_sort());
  }

  if (sort_fset::is_fset(sort_expression(Arg1)) && sort_set::is_set(sort_expression(Arg2)))
  {
    Arg1=sort_set::set_(container_sort(Arg1).element_sort());
  }

  if (sort_bag::is_bag(sort_expression(Arg1)) && sort_fbag::is_fbag(sort_expression(Arg2)))
  {
    Arg2=sort_bag::bag(container_sort(Arg2).element_sort());
  }

  if (sort_fbag::is_fbag(sort_expression(Arg1)) && sort_bag::is_bag(sort_expression(Arg2)))
  {
    Arg1=sort_bag::bag(container_sort(Arg1).element_sort());
  }

  sort_expression temp_result;
  if (!UnifyMinType(Res,Arg1,temp_result))
  {
    return false;
  }

  if (!UnifyMinType(temp_result,Arg2,Res))
  {
    return false;
  }

  result=function_sort({ Res,Res },Res);
  return true;
}

bool mcrl2::data::data_type_checker::MatchSetOpSetCompl(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of SetCompl operation (Set(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  // if (detail::IsNumericType(Res))
  if (Res==sort_bool::bool_())
  {
    result=type;
    return true;
  }

  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }

  sort_expression Arg=Args.front();
  if (is_basic_sort(Arg))
  {
    Arg=UnwindType(Arg);
  }
  // if (detail::IsNumericType(Arg))
  if (Arg==sort_bool::bool_())
  {
    result=type;
    return true;
  }
  if (!sort_set::is_set(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();
  if (!sort_set::is_set(sort_expression(Arg)))
  {
    return false;
  }
  Arg=down_cast<container_sort>(Arg).element_sort();

  sort_expression temp_result;
  if (!UnifyMinType(Res,Arg,temp_result))
  {
    return false;
  }
  Res=temp_result;

  result=function_sort({ sort_expression(sort_set::set_(sort_expression(Res))) },sort_set::set_(sort_expression(Res)));
  return true;
}

//Bags
bool mcrl2::data::data_type_checker::MatchBagOpBag2Set(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of Bag2Set (Bag(S)->Set(S))
  //If some of the parameters are Pos,Nat, or Int do upcasting.


  sort_expression Res=type.codomain();
  if (is_basic_sort(Res))
  {
    Res=UnwindType(Res);
  }
  if (!sort_set::is_set(sort_expression(Res)))
  {
    return false;
  }
  Res=down_cast<container_sort>(Res).element_sort();

  const sort_expression_list& Args=type.domain();
  if (Args.size()!=1)
  {
    return false;
  }

  sort_expression Arg=Args.front();
  if (is_basic_sort(Arg))
  {
    Arg=UnwindType(Arg);
  }
  if (!sort_bag::is_bag(sort_expression(Arg)))
  {
    return false;
  }
  Arg=down_cast<container_sort>(Arg).element_sort();

  sort_expression temp_result;
  if (!UnifyMinType(Arg,Res,temp_result))
  {
    return false;
  }
  Arg=temp_result;

  result=function_sort({ sort_expression(sort_bag::bag(sort_expression(Arg))) },sort_set::set_(sort_expression(Arg)));
  return true;
}

bool mcrl2::data::data_type_checker::MatchBagOpBagCount(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of BagCount (SxBag(S)->Nat)
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  //If the second argument is not a Bag, don't match

  if (!is_function_sort(type))
  {
    result=type;
    return true;
  }
  sort_expression_list Args=type.domain();
  if (!(Args.size()==2))
  {
    result=type;
    return true;
  }

  sort_expression Arg1=Args.front();

  Args=Args.tail();
  sort_expression Arg2=Args.front();
  if (is_basic_sort(Arg2))
  {
    Arg2=UnwindType(Arg2);
  }
  if (!sort_bag::is_bag(sort_expression(Arg2)))
  {
    result=type;
    return true;
  }
  Arg2=down_cast<container_sort>(Arg2).element_sort();

  sort_expression Arg;
  if (!UnifyMinType(Arg1,Arg2,Arg))
  {
    return false;
  }

  result=function_sort(sort_expression_list({ Arg,sort_bag::bag(sort_expression(Arg)) }),sort_nat::nat());
  return true;
}


bool mcrl2::data::data_type_checker::MatchFuncUpdate(const function_sort& type, sort_expression& result)
{
  //tries to sort out the types of FuncUpdate ((A->B)xAxB->(A->B))
  //If some of the parameters are Pos,Nat, or Int do upcasting.

  sort_expression_list Args=type.domain();
  if (Args.size()!=3)
  {
    return false;
  }
  function_sort Arg1=down_cast<function_sort>(Args.front());
  Args=Args.tail();
  sort_expression Arg2=Args.front();
  Args=Args.tail();
  sort_expression Arg3=Args.front();
  const sort_expression& Res=type.codomain();
  if (!is_function_sort(Res))
  {
    return false;
  }

  sort_expression temp_result;
  if (!UnifyMinType(Arg1,Res,temp_result))
  {
    return false;
  }
  Arg1 = atermpp::down_cast<function_sort>(UnwindType(temp_result));

  // determine A and B from Arg1:
  sort_expression_list LA=Arg1.domain();
  if (LA.size()!=1)
  {
    return false;
  }
  const sort_expression& A=LA.front();
  sort_expression B=Arg1.codomain();

  if (!UnifyMinType(A,Arg2,temp_result))
  {
    return false;
  }
  if (!UnifyMinType(B,Arg3,temp_result))
  {
    return false;
  }

  result=function_sort(sort_expression_list({ Arg1,A,B}) ,Arg1);
  return true;
}



bool mcrl2::data::data_type_checker::MaximumType(const sort_expression& Type1, const sort_expression& Type2, sort_expression& result)
{
  // if Type1 is convertible into Type2 or vice versa, the most general
  // of these types are returned in result. If no conversion is possible false is returned
  // and result is not changed. Conversions only take place between numerical types
  if (EqTypesA(Type1,Type2))
  {
    result=Type1;
    return true;
  }
  if (data::is_untyped_sort(data::sort_expression(Type1)))
  {
    result=Type2;
    return true;
  }
  if (data::is_untyped_sort(data::sort_expression(Type2)))
  {
    result=Type1;
    return true;
  }
  if (EqTypesA(Type1,sort_real::real_()))
  {
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_int::int_()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type1;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_nat::nat()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_pos::pos()))
    {
      result=Type1;
      return true;
    }
    return false;
  }
  if (EqTypesA(Type1,sort_pos::pos()))
  {
    if (EqTypesA(Type2,sort_real::real_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_int::int_()))
    {
      result=Type2;
      return true;
    }
    if (EqTypesA(Type2,sort_nat::nat()))
    {
      result=Type2;
      return true;
    }
    return false;
  }
  return false;
}

sort_expression_list mcrl2::data::data_type_checker::ExpandNumTypesUpL(const sort_expression_list& type_list)
{
  sort_expression_vector result;
  for(sort_expression_list::const_iterator i=type_list.begin(); i!=type_list.end(); ++i)
  {
    result.push_back(ExpandNumTypesUp(*i));
  }
  return sort_expression_list(result.begin(),result.end());
}

sort_expression mcrl2::data::data_type_checker::ExpandNumTypesUp(sort_expression Type)
{
  //Expand Type to possible bigger types.
  if (data::is_untyped_sort(data::sort_expression(Type)))
  {
    return Type;
  }
  if (EqTypesA(sort_pos::pos(),Type))
  {
    return untyped_possible_sorts(sort_expression_list({ sort_pos::pos(), sort_nat::nat(), sort_int::int_(),sort_real::real_() } ));
  }
  if (EqTypesA(sort_nat::nat(),Type))
  {
    return untyped_possible_sorts(sort_expression_list({ sort_nat::nat(), sort_int::int_(),sort_real::real_() } ));
  }
  if (EqTypesA(sort_int::int_(),Type))
  {
    return untyped_possible_sorts(sort_expression_list({ sort_int::int_(), sort_real::real_() } ));
  }
  if (is_basic_sort(Type))
  {
    return Type;
  }
  if (is_container_sort(Type))
  {
    const container_sort& s=down_cast<container_sort>(Type);
    const container_type& ConsType = s.container_name();
    if (is_list_container(ConsType))
    {
      return container_sort(s.container_name(),ExpandNumTypesUp(s.element_sort()));
    }

    if (is_set_container(ConsType))
    {
      return container_sort(s.container_name(),ExpandNumTypesUp(s.element_sort()));
    }

    if (is_bag_container(ConsType))
    {
      return container_sort(s.container_name(),ExpandNumTypesUp(s.element_sort()));
    }

    if (is_fset_container(ConsType))
    {
      const sort_expression expanded_sorts=ExpandNumTypesUp(s.element_sort());
      return untyped_possible_sorts(sort_expression_list({
                     container_sort(s.container_name(),expanded_sorts),
                     container_sort(set_container(),expanded_sorts) }));
    }

    if (is_fbag_container(ConsType))
    {
      const sort_expression expanded_sorts=ExpandNumTypesUp(s.element_sort());
      return untyped_possible_sorts(sort_expression_list({
                     container_sort(s.container_name(),expanded_sorts),
                     container_sort(bag_container(),expanded_sorts) }));
    }
  }

  if (is_structured_sort(Type))
  {
    return Type;
  }

  if (is_function_sort(Type))
  {
    const function_sort& t=down_cast<const function_sort>(Type);
    //the argument types, and if the resulting type is SortArrow -- recursively
    sort_expression_list NewTypeList;
    for (sort_expression_list TypeList=t.domain(); !TypeList.empty(); TypeList=TypeList.tail())
    {
      NewTypeList.push_front(ExpandNumTypesUp(UnwindType(TypeList.front())));
    }
    const sort_expression& ResultType=t.codomain();
    if (!is_function_sort(ResultType))
    {
      return function_sort(reverse(NewTypeList),ResultType);
    }
    else
    {
      return function_sort(reverse(NewTypeList),ExpandNumTypesUp(UnwindType(ResultType)));
    }
  }

  return Type;
}

sort_expression mcrl2::data::data_type_checker::ExpandNumTypesDown(sort_expression Type)
{
  // Expand Numeric types down
  if (data::is_untyped_sort(Type))
  {
    return Type;
  }
  if (is_basic_sort(Type))
  {
    Type=UnwindType(Type);
  }

  bool function=false;
  sort_expression_list Args;
  if (is_function_sort(Type))
  {
    const function_sort& fs=down_cast<const function_sort>(Type);
    function=true;
    Args=fs.domain();
    Type=fs.codomain();
  }

  if (EqTypesA(sort_real::real_(),Type))
  {
    Type=untyped_possible_sorts(sort_expression_list({ sort_pos::pos(),sort_nat::nat(),sort_int::int_(),sort_real::real_() }));
  }
  if (EqTypesA(sort_int::int_(),Type))
  {
    Type=untyped_possible_sorts(sort_expression_list({ sort_pos::pos(),sort_nat::nat(),sort_int::int_() } ));
  }
  if (EqTypesA(sort_nat::nat(),Type))
  {
    Type=untyped_possible_sorts(sort_expression_list({ sort_pos::pos(),sort_nat::nat() } ));
  }
  if (is_container_sort(Type))
  {
    const container_sort& s=down_cast<container_sort>(Type);
    const container_type& ConsType = s.container_name();
    if (is_list_container(ConsType))
    {
      Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
    }

    if (is_fset_container(ConsType))
    {
      Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
    }

    if (is_fbag_container(ConsType))
    {
      Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
    }

    if (is_set_container(ConsType))
    {
      const sort_expression shrinked_sorts=ExpandNumTypesDown(s.element_sort());
      Type=untyped_possible_sorts(sort_expression_list({
                     container_sort(s.container_name(),shrinked_sorts),
                     container_sort(set_container(),shrinked_sorts) } ));
    }

    if (is_bag_container(ConsType))
    {
      const sort_expression shrinked_sorts=ExpandNumTypesDown(s.element_sort());
      Type=untyped_possible_sorts(sort_expression_list({
                     container_sort(s.container_name(),shrinked_sorts),
                     container_sort(bag_container(),shrinked_sorts) } ));
    }
  }

  return (function)?function_sort(Args,Type):Type;
}


bool mcrl2::data::data_type_checker::InTypesA(const sort_expression& Type, sort_expression_list Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (EqTypesA(Type,Types.front()))
    {
      return true;
    }
  return false;
}

bool mcrl2::data::data_type_checker::EqTypesA(const sort_expression& Type1, const sort_expression& Type2)
{
  if (Type1==Type2)
  {
    return true;
  }

  return UnwindType(Type1)==UnwindType(Type2);
}

bool mcrl2::data::data_type_checker::InTypesL(const sort_expression_list& Type, term_list<sort_expression_list> Types)
{
  for (; !Types.empty(); Types=Types.tail())
    if (EqTypesL(Type,Types.front()))
    {
      return true;
    }
  return false;
}

bool mcrl2::data::data_type_checker::EqTypesL(sort_expression_list Type1, sort_expression_list Type2)
{
  if (Type1==Type2)
  {
    return true;
  }
  if (Type1.size()!=Type2.size())
  {
    return false;
  }
  for (; !Type1.empty(); Type1=Type1.tail(),Type2=Type2.tail())
    if (!EqTypesA(Type1.front(),Type2.front()))
    {
      return false;
    }
  return true;
}

sort_expression mcrl2::data::data_type_checker::determine_allowed_type(const data_expression& d, const sort_expression& proposed_type)
{
  if (is_variable(d))
  {
    variable v(d);
    // Set the type to one option in possible sorts, if there are more options.
    const sort_expression new_type=detail::replace_possible_sorts(proposed_type);
    v=variable(v.name(),new_type);
    return new_type;
  }

  assert(proposed_type.defined());

  sort_expression Type=proposed_type;
  // If d is not a variable it is an untyped name, or a function symbol.
  const core::identifier_string& data_term_name=data::is_untyped_identifier(d)?
                      atermpp::down_cast<const untyped_identifier>(d).name():
                            (down_cast<const data::function_symbol>(d).name());

  if (data::detail::if_symbol()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchIf(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function if has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (data::detail::equal_symbol()==data_term_name
      || data::detail::not_equal_symbol()==data_term_name
      || data::detail::less_symbol()==data_term_name
      || data::detail::less_equal_symbol()==data_term_name
      || data::detail::greater_symbol()==data_term_name
      || data::detail::greater_equal_symbol()==data_term_name
     )
  {
    sort_expression NewType;
    if (!MatchEqNeqComparison(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function " + core::pp(data_term_name) + " has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_nat::sqrt_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchSqrt(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function sqrt has an incorrect argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::cons_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListOpCons(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function |> has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::snoc_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListOpSnoc(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function <| has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::concat_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListOpConcat(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function ++ has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::element_at_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListOpEltAt(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function @ has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::head_name()==data_term_name||
      sort_list::rhead_name()==data_term_name)
  {

    sort_expression NewType;
    if (!MatchListOpHead(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function {R,L}head has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::tail_name()==data_term_name||
      sort_list::rtail_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListOpTail(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function {R,L}tail has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_bag::set2bag_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchSetOpSet2Bag(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function Set2Bag has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_list::in_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchListSetBagOpIn(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function {List,Set,Bag}In has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_set::union_name()==data_term_name||
      sort_set::difference_name()==data_term_name||
      sort_set::intersection_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchSetBagOpUnionDiffIntersect(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function {Set,Bag}{Union,Difference,Intersect} has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }


  if (sort_fset::insert_name()==data_term_name)
  {
    sort_expression NewType;
    if (!match_fset_insert(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Set enumeration has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_fbag::cinsert_name()==data_term_name)
  {
    sort_expression NewType;
    if (!match_fbag_cinsert(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Bag enumeration has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }



  if (sort_set::complement_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchSetOpSetCompl(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function SetCompl has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_bag::bag2set_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchBagOpBag2Set(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function Bag2Set has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_bag::count_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchBagOpBagCount(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("The function BagCount has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }


  if (data::function_update_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchFuncUpdate(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Function update has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_set::constructor_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchSetConstructor(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Set constructor has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }


  if (sort_bag::constructor_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchBagConstructor(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Bag constructor has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_set::false_function_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchFalseFunction(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Bag constructor has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  if (sort_bag::zero_function_name()==data_term_name)
  {
    sort_expression NewType;
    if (!MatchBagConstructor(atermpp::down_cast<function_sort>(Type), NewType))
    {
      throw mcrl2::runtime_error("Bag constructor has incompatible argument types " + data::pp(Type) + " (while typechecking " + data::pp(d) + ").");
    }
    Type=NewType;
  }

  return Type;
}



sort_expression mcrl2::data::data_type_checker::TraverseVarConsTypeDN(
  const std::map<core::identifier_string,sort_expression>& DeclaredVars,
  const std::map<core::identifier_string,sort_expression>& AllowedVars,
  data_expression& DataTerm,
  sort_expression PosType,
  std::map<core::identifier_string,sort_expression>& FreeVars,
  const bool strictly_ambiguous,
  const size_t nFactPars,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  // std::string::npos for nFactPars means the number of arguments is not known.
  if (data::is_untyped_identifier(DataTerm)||data::is_function_symbol(DataTerm))
  {
    core::identifier_string Name=data::is_untyped_identifier(DataTerm)?down_cast<const untyped_identifier>(DataTerm).name():
                                                         atermpp::down_cast<const function_symbol>(DataTerm).name();

    bool variable_=false;
    bool TypeADefined=false;
    sort_expression TypeA;
    std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);

    if (i!=DeclaredVars.end())
    {
      TypeA=i->second;
      TypeADefined=true;
      const sort_expression Type1(UnwindType(TypeA));
      if (is_function_sort(Type1)?(function_sort(Type1).domain().size()==nFactPars):(nFactPars==0))
      {
        variable_=true;
        if (AllowedVars.count(Name)==0)
        {
          throw mcrl2::runtime_error("Variable " + core::pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side.");
        }

        //Add to free variables list
        FreeVars[Name]=TypeA;
      }
      else
      {
        TypeADefined=false;
      }
    }

    sort_expression_list ParList;
    if (nFactPars==0)
    {
      std::map<core::identifier_string,sort_expression>::const_iterator i=DeclaredVars.find(Name);
      if (i!=DeclaredVars.end())
      {
        TypeA=i->second;
        TypeADefined=true;
        sort_expression temp;
        if (!TypeMatchA(TypeA,PosType,temp))
        {
          throw mcrl2::runtime_error("The type " + data::pp(TypeA) + " of variable " + core::pp(Name)
                          + " is incompatible with " + data::pp(PosType) + " (typechecking " + data::pp(DataTerm) + ").");
        }
        DataTerm=variable(Name,TypeA);
        return TypeA;
      }
      else
      {
        std::map<core::identifier_string,sort_expression>::const_iterator i=user_constants.find(Name);
        if (i!=user_constants.end())
        {
          TypeA=i->second;
          TypeADefined=true;
          sort_expression temp;
          if (!TypeMatchA(TypeA,PosType,temp))
          {
            throw mcrl2::runtime_error("The type " + data::pp(TypeA) + " of constant " + core::pp(Name)
                            + " is incompatible with " + data::pp(PosType) + " (typechecking " + data::pp(DataTerm) + ").");
          }
          DataTerm=data::function_symbol(Name,TypeA);
          return TypeA;
        }
        else
        {
          std::map<core::identifier_string,sort_expression_list>::const_iterator j=system_constants.find(Name);

          if (j!=system_constants.end())
          {
            ParList=j->second;
            if (ParList.size()==1)
            {
              sort_expression Type1=ParList.front();
              DataTerm=function_symbol(Name,Type1);
              return Type1;
            }
            else
            {
              DataTerm=data::function_symbol(Name,data::untyped_sort());
              throw  mcrl2::runtime_error("Ambiguous system constant " + core::pp(Name) + ".");
            }
          }
          else
          {
            throw mcrl2::runtime_error("Unknown constant " + core::pp(Name) + ".");
          }
        }
      }
    }

    if (TypeADefined)
    {
      ParList = sort_expression_list({ UnwindType(TypeA) });
    }
    else
    {
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=user_functions.find(Name);
      const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=system_functions.find(Name);

      if (j_context==user_functions.end())
      {
        if (j_gssystem!=system_functions.end())
        {
          ParList=j_gssystem->second; // The function only occurs in the system.
        }
        else // None are defined.
        {
          if (nFactPars!=std::string::npos)
          {
            throw mcrl2::runtime_error("Unknown operation " + core::pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s.":"."));
          }
          else
          {
            throw mcrl2::runtime_error("Unknown operation " + core::pp(Name) + ".");
          }
        }
      }
      else if (j_gssystem==system_functions.end())
      {
        ParList=j_context->second; // only the context sorts are defined.
      }
      else  // Both are defined.
      {
        ParList=j_gssystem->second+j_context->second;
      }
    }

    sort_expression_list CandidateParList=ParList;

    {
      // filter ParList keeping only functions A_0#...#A_nFactPars->A
      sort_expression_list NewParList;
      if (nFactPars!=std::string::npos)
      {
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          const sort_expression& Par=ParList.front();
          if (!is_function_sort(Par))
          {
            continue;
          }
          if (down_cast<function_sort>(Par).domain().size()!=nFactPars)
          {
            continue;
          }
          NewParList.push_front(Par);
        }
        ParList=reverse(NewParList);
      }

      if (!ParList.empty())
      {
        CandidateParList=ParList;
      }

      // filter ParList keeping only functions of the right type
      sort_expression_list BackupParList=ParList;
      NewParList=sort_expression_list();
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        const sort_expression& Par=ParList.front();
        try
        {
          PosType=determine_allowed_type(DataTerm, PosType);  // XXXXXXXXXX
          sort_expression result;
          if (TypeMatchA(Par,PosType,result))
          {
            NewParList=detail::insert_sort_unique(NewParList,result);
          }
        }
        catch (mcrl2::runtime_error&)
        {
          // Ignore the error. Just do not add the type to NewParList
        }
      }
      NewParList=reverse(NewParList);

      if (NewParList.empty())
      {
        //Ok, this looks like a type error. We are not that strict.
        //Pos can be Nat, or even Int...
        //So lets make PosType more liberal
        //We change every Pos to NotInferred(Pos,Nat,Int)...
        //and get the list. Then we take the min of the list.

        ParList=BackupParList;
        PosType=ExpandNumTypesUp(PosType);
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          const sort_expression& Par=ParList.front();
          sort_expression result;
          if (TypeMatchA(Par,PosType,result))
          {
            NewParList=detail::insert_sort_unique(NewParList,result);
          }
        }
        NewParList=reverse(NewParList);
        if (NewParList.size()>1)
        {
          NewParList = sort_expression_list({ detail::MinType(NewParList) });
        }
      }

      if (NewParList.empty())
      {
        //Ok, casting of the arguments did not help.
        //Let's try to be more relaxed about the result, e.g. returning Pos or Nat is not a bad idea for int.

        ParList=BackupParList;

        PosType=ExpandNumTypesDown(ExpandNumTypesUp(PosType));
        for (; !ParList.empty(); ParList=ParList.tail())
        {
          const sort_expression& Par=ParList.front();
          sort_expression result;
          if (TypeMatchA(Par,PosType,result))
          {
            NewParList=detail::insert_sort_unique(NewParList,result);
          }
        }
        NewParList=reverse(NewParList);
        if (NewParList.size()>1)
        {
          NewParList = sort_expression_list({ detail::MinType(NewParList) });
        }
      }

      ParList=NewParList;
    }
    if (ParList.empty())
    {
      //provide some information to the upper layer for a better error message
      sort_expression Sort;
      if (CandidateParList.size()==1)
      {
        Sort=CandidateParList.front();
      }
      else
      {
        Sort=untyped_possible_sorts(sort_expression_list(CandidateParList));
      }
      DataTerm=data::function_symbol(Name,Sort);
      if (nFactPars!=std::string::npos)
      {
        throw mcrl2::runtime_error("Unknown operation/variable " + core::pp(Name)
                        + " with " + to_string(nFactPars) + " argument" + ((nFactPars != 1)?"s":"")
                        + " that matches type " + data::pp(PosType) + ".");
      }
      else
      {
        throw mcrl2::runtime_error("Unknown operation/variable " + core::pp(Name) + " that matches type " + data::pp(PosType) + ".");
      }
    }

    if (ParList.size()==1)
    {
      // replace PossibleSorts by a single possibility.
      sort_expression Type=ParList.front();

      sort_expression OldType=Type;
      bool result=true;
      assert(Type.defined());
      if (detail::HasUnknown(Type))
      {
        sort_expression new_type;
        result=TypeMatchA(Type,PosType,new_type);
        Type=new_type;
      }

      if (detail::HasUnknown(Type) && data::is_function_symbol(DataTerm))
      {
        sort_expression new_type;
        result=TypeMatchA(Type,DataTerm.sort(),new_type);
        Type=new_type;
      }

      if (!result)
      {
        throw mcrl2::runtime_error("Fail to match sort " + data::pp(OldType) + " with " + data::pp(PosType) + ".");
      }

      Type=determine_allowed_type(DataTerm,Type);

      Type=detail::replace_possible_sorts(Type); // Set the type to one option in possible sorts, if there are more options.

      if (variable_)
      {
        DataTerm=variable(Name,Type);
      }
      else if (is_untyped_identifier(DataTerm))
      {
        DataTerm=data::function_symbol(untyped_identifier(DataTerm).name(),Type);
      }
      else
      {
        DataTerm=data::function_symbol(function_symbol(DataTerm).name(),Type);
      }
      assert(Type.defined());
      return Type;
    }
    else
    {
      was_ambiguous=true;
      if (strictly_ambiguous)
      {
        if (nFactPars!=std::string::npos)
        {
          throw mcrl2::runtime_error("Ambiguous operation " + core::pp(Name) + " with " + to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s.":"."));
        }
        else
        {
          throw mcrl2::runtime_error("Ambiguous operation " + core::pp(Name) + ".");
        }
      }
      else
      {
        return data::untyped_sort();
      }
    }
  }
  else
  {
    return TraverseVarConsTypeD(DeclaredVars,AllowedVars,DataTerm,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
  }
}


sort_expression mcrl2::data::data_type_checker::TraverseVarConsTypeD(
  const std::map<core::identifier_string,sort_expression>& DeclaredVars,
  const std::map<core::identifier_string,sort_expression>& AllowedVars,
  data_expression& DataTerm,
  const sort_expression& PosType,
  std::map<core::identifier_string,sort_expression>& FreeVars,
  const bool strictly_ambiguous,
  const bool warn_upcasting,
  const bool print_cast_error)
{
  //Type checks and transforms *DataTerm replacing Unknown datatype with other ones.
  //Returns the type of the term
  //which should match the PosType
  //all the variables should be in AllowedVars
  //if a variable is in DeclaredVars and not in AllowedVars,
  //a different error message is generated.
  //all free variables (if any) are added to FreeVars

  if (is_abstraction(DataTerm))
  {
    const abstraction& abstr=down_cast<const abstraction>(DataTerm);
    //The variable declaration of a binder should have at least 1 declaration
    if (abstr.variables().size()==0)
    {
      throw mcrl2::runtime_error("Binder " + data::pp(DataTerm) + " should have at least one declared variable.");
    }

    const binder_type& BindingOperator = abstr.binding_operator();
    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

    if (is_untyped_set_or_bag_comprehension_binder(BindingOperator) ||
        is_set_comprehension_binder(BindingOperator) ||
        is_bag_comprehension_binder(BindingOperator))
    {
      const variable_list& VarDecls=abstr.variables();

      //A Set/bag comprehension should have exactly one variable declared
      if (VarDecls.size()!=1)
      {
        throw mcrl2::runtime_error("Set/bag comprehension " + data::pp(DataTerm) + " should have exactly one declared variable.");
      }

      const variable& VarDecl=VarDecls.front();
      sort_expression NewType=VarDecl.sort();
      variable_list VarList ({ VarDecl });
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList);
      NewAllowedVars=CopyAllowedVars;

      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList);
      NewDeclaredVars=CopyDeclaredVars;

      data_expression Data=abstr.body();

      sort_expression ResType;
      try
      {
        ResType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,data::untyped_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nThe condition or count of a set/bag comprehension " + data::pp(DataTerm) + " cannot be determined.");
      }
      sort_expression temp;
      if (TypeMatchA(sort_bool::bool_(),ResType,temp))
      {
        NewType=sort_set::set_(sort_expression(NewType));
        DataTerm = abstraction(set_comprehension_binder(),VarDecls,Data);
      }
      else if (TypeMatchA(sort_nat::nat(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        DataTerm = abstraction(bag_comprehension_binder(),VarDecls,Data);
      }
      else if (TypeMatchA(sort_pos::pos(),ResType,temp))
      {
        NewType=sort_bag::bag(sort_expression(NewType));
        Data=application(sort_nat::cnat(),Data);
        DataTerm = abstraction(bag_comprehension_binder(),VarDecls,Data);
      }
      else
      {
        throw mcrl2::runtime_error("The condition or count of a set/bag comprehension is not of sort Bool, Nat or Pos, but of sort " + data::pp(ResType) + ".");
      }

      if (!TypeMatchA(NewType,PosType,NewType))
      {
        throw mcrl2::runtime_error("A set or bag comprehension of type " + data::pp(VarDecl.sort()) + " does not match possible type " +
                            data::pp(PosType) + " (while typechecking " + data::pp(DataTerm) + ").");
      }

      detail::RemoveVars(FreeVars,VarList);
      return NewType;
    }

    if (is_forall_binder(BindingOperator) || is_exists_binder(BindingOperator))
    {
      const variable_list& VarList=abstr.variables();
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList);
      NewAllowedVars=CopyAllowedVars;

      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList);
      NewDeclaredVars=CopyDeclaredVars;

      data_expression Data=abstr.body();
      sort_expression temp;
      if (!TypeMatchA(sort_bool::bool_(),PosType,temp))
      {
        throw mcrl2::runtime_error("The type of an exist/forall for " + data::pp(DataTerm) + " cannot be determined.");
      }
      sort_expression NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,sort_bool::bool_(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

      if (!TypeMatchA(sort_bool::bool_(),NewType,temp))
      {
        throw mcrl2::runtime_error("The type of an exist/forall for " + data::pp(DataTerm) + " cannot be determined.");
      }

      detail::RemoveVars(FreeVars,VarList);

      DataTerm=abstraction(BindingOperator,VarList,Data);
      return sort_bool::bool_();
    }

    if (is_lambda_binder(BindingOperator))
    {
      const variable_list& VarList=abstr.variables();
      std::map<core::identifier_string,sort_expression> NewAllowedVars;
      AddVars2Table(CopyAllowedVars,VarList);
      NewAllowedVars=CopyAllowedVars;

      std::map<core::identifier_string,sort_expression> NewDeclaredVars;
      AddVars2Table(CopyDeclaredVars,VarList);
      NewDeclaredVars=CopyDeclaredVars;

      sort_expression_list ArgTypes=detail::GetVarTypes(VarList);
      sort_expression NewType;
      if (!UnArrowProd(ArgTypes,PosType,NewType))
      {
        throw mcrl2::runtime_error("No functions with arguments " + data::pp(ArgTypes) + " among " + data::pp(PosType) + " (while typechecking " + data::pp(DataTerm) + ").");
      }
      data_expression Data=abstr.body();

      try
      {
        NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,NewType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error& e)
      {
        detail::RemoveVars(FreeVars,VarList);
        throw e;
      }

      detail::RemoveVars(FreeVars,VarList);

      DataTerm=abstraction(BindingOperator,VarList,Data);
      return function_sort(ArgTypes,NewType);
    }
  }

  if (is_where_clause(DataTerm))
  {
    const where_clause& where=down_cast<const where_clause>(DataTerm);
    variable_list WhereVarList;
    assignment_list NewWhereList;
    const assignment_expression_list& where_asss=where.declarations();
    for (assignment_expression_list::const_iterator i=where_asss.begin(); i!=where_asss.end(); ++i)
    {
      const assignment_expression WhereElem= *i;
      data_expression WhereTerm;
      variable NewWhereVar;
      if (data::is_untyped_identifier_assignment(WhereElem))
      {
        const data::untyped_identifier_assignment& t=down_cast<const data::untyped_identifier_assignment>(WhereElem);
        WhereTerm=t.rhs();
        sort_expression WhereType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,data::untyped_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

        // The variable in WhereElem is just a string and needs to be transformed to a DataVarId.
        NewWhereVar=variable(t.lhs(),WhereType);
      }
      else
      {
        const assignment& t=down_cast<const assignment>(WhereElem);
        WhereTerm=t.rhs();
        NewWhereVar=t.lhs();
        // sort_expression WhereType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,data::untyped_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
        sort_expression WhereType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,WhereTerm,NewWhereVar.sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      WhereVarList.push_front(NewWhereVar);
      NewWhereList.push_front(assignment(NewWhereVar,WhereTerm));
    }
    NewWhereList=reverse(NewWhereList);

    std::map<core::identifier_string,sort_expression> CopyAllowedVars(AllowedVars);
    std::map<core::identifier_string,sort_expression> CopyDeclaredVars(DeclaredVars);

    variable_list VarList=reverse(WhereVarList);
    std::map<core::identifier_string,sort_expression> NewAllowedVars;
    AddVars2Table(CopyAllowedVars,VarList);
    NewAllowedVars=CopyAllowedVars;

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    AddVars2Table(CopyDeclaredVars,VarList);
    NewDeclaredVars=CopyDeclaredVars;

    data_expression Data=where.body();
    sort_expression NewType=TraverseVarConsTypeD(NewDeclaredVars,NewAllowedVars,Data,PosType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);

    detail::RemoveVars(FreeVars,VarList);

    DataTerm=where_clause(Data,NewWhereList);
    return NewType;
  }

  if (is_application(DataTerm))
  {
    //arguments
    const application& appl=down_cast<application>(DataTerm);
    size_t nArguments=appl.size();

    //The following is needed to check enumerations
    const data_expression& Arg0 = appl.head();
    if (data::is_function_symbol(Arg0) || data::is_untyped_identifier(Arg0))
    {
      core::identifier_string Name = is_function_symbol(Arg0)?down_cast<function_symbol>(Arg0).name():
                                                              atermpp::down_cast<untyped_identifier>(Arg0).name();
      if (Name == sort_list::list_enumeration_name())
      {
        sort_expression Type;
        if (!UnList(PosType,Type))
        {
          throw mcrl2::runtime_error("It is not possible to cast list to " + data::pp(PosType) + " (while typechecking " + data::pp(data_expression_list(appl.begin(),appl.end())) + ").");
        }

        //First time to determine the common type only!
        data_expression_list NewArguments;
        bool Type_is_stable=true;
        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,false);
          }
          catch (mcrl2::runtime_error&)
          {
            // Try again, but now without Type as the suggestion.
            // If this does not work, it will be caught in the second pass below.
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,data::untyped_sort(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          NewArguments.push_front(Argument);
          Type_is_stable=Type_is_stable && (Type==Type0);
          Type=Type0;
        }
        // Arguments=OldArguments;

        //Second time to do the real work, but only if the elements in the list have different types.
        if (!Type_is_stable)
        {
          NewArguments=data_expression_list();
          for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
          {
            data_expression Argument= *i;
            sort_expression Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
            NewArguments.push_front(Argument);
            Type=Type0;
          }
        }

        Type=sort_list::list(sort_expression(Type));
        DataTerm=sort_list::list_enumeration(sort_expression(Type), data_expression_list(reverse(NewArguments)));
        return Type;
      }
      if (Name == sort_set::set_enumeration_name())
      {
        sort_expression Type;
        if (!UnFSet(PosType,Type))
        {
          throw mcrl2::runtime_error("It is not possible to cast set to " + data::pp(PosType) + " (while typechecking " + data::pp(data_expression_list(appl.begin(),appl.end())) + ").");
        }

        //First time to determine the common type only (which will be NewType)!
        bool NewTypeDefined=false;
        sort_expression NewType;
        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast element to " + data::pp(Type) + " (while typechecking " + data::pp(Argument) + ").");
          }

          sort_expression OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            sort_expression temp;
            if (!MaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Set contains incompatible elements of sorts " + data::pp(OldNewType) + " and " + data::pp(Type0) + " (while typechecking " + data::pp(Argument) + ".");
            }
            NewType=temp;
            NewTypeDefined=true;
          }
        }

        // Type is now the most generic type to be used.
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        data_expression_list NewArguments;
        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast element to " + data::pp(Type) + " (while typechecking " + data::pp(Argument) + ").");
          }
          NewArguments.push_front(Argument);
          Type=Type0;
        }
        DataTerm=sort_set::set_enumeration(sort_expression(Type),data_expression_list(reverse(NewArguments)));
        if (sort_set::is_set(PosType))
        {
          DataTerm=sort_set::constructor(Type, sort_set::false_function(Type),DataTerm);

          return sort_set::set_(Type);
        }
        return sort_fset::fset(Type);
      }
      if (Name == sort_bag::bag_enumeration_name())
      {
        sort_expression Type;
        if (!UnFBag(PosType,Type))
        {
          throw mcrl2::runtime_error("Impossible to cast bag to " + data::pp(PosType) + "(while typechecking " +
                                      data::pp(data_expression_list(appl.begin(),appl.end())) + ").");
        }

        //First time to determine the common type only!
        sort_expression NewType;
        bool NewTypeDefined=false;
        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast element to " + data::pp(Type) + " (while typechecking " + data::pp(Argument0) + ").");
          }
          sort_expression Type1;
          try
          {
            Type1=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast number to " + data::pp(sort_nat::nat()) + " (while typechecking " + data::pp(Argument1) + ").");
            }
            else
            {
              throw e;
            }
          }
          sort_expression OldNewType=NewType;
          if (!NewTypeDefined)
          {
            NewType=Type0;
            NewTypeDefined=true;
          }
          else
          {
            sort_expression temp;
            if (!MaximumType(NewType,Type0,temp))
            {
              throw mcrl2::runtime_error("Bag contains incompatible elements of sorts " + data::pp(OldNewType) + " and " + data::pp(Type0) + " (while typechecking " + data::pp(Argument0) + ").");
            }

            NewType=temp;
            NewTypeDefined=true;
          }
        }
        assert(Type.defined());
        assert(NewTypeDefined);
        Type=NewType;
        // Arguments=OldArguments;

        //Second time to do the real work.
        data_expression_list NewArguments;
        for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
        {
          data_expression Argument0= *i;
          ++i;
          data_expression Argument1= *i;
          sort_expression Type0;
          try
          {
            Type0=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument0,Type,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast element to " + data::pp(Type) + " (while typechecking " + data::pp(Argument0) + ").");
            }
            else
            {
              throw e;
            }
          }
          sort_expression Type1;
          try
          {
            Type1=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Argument1,sort_nat::nat(),FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            if (print_cast_error)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\nImpossible to cast number to " + data::pp(sort_nat::nat()) + " (while typechecking " + data::pp(Argument1) + ").");
            }
            else
            {
              throw e;
            }
          }
          NewArguments.push_front(Argument0);
          NewArguments.push_front(Argument1);
          Type=Type0;
        }
        DataTerm=sort_bag::bag_enumeration(Type, data_expression_list(reverse(NewArguments)));
        if (sort_bag::is_bag(PosType))
        {
          DataTerm=sort_bag::constructor(Type, sort_bag::zero_function(Type),DataTerm);

          return sort_bag::bag(Type);
        }
        return sort_fbag::fbag(Type);
      }
    }
    sort_expression_list NewArgumentTypes;
    data_expression_list NewArguments;
    sort_expression_list argument_sorts;
    for (application::const_iterator i=appl.begin(); i!=appl.end(); ++i)
    {
      data_expression Arg= *i;
      sort_expression Type=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,data::untyped_sort(),FreeVars,false,warn_upcasting,print_cast_error);
      assert(Type.defined());
      NewArguments.push_front(Arg);
      NewArgumentTypes.push_front(Type);
    }
    data_expression_list Arguments=reverse(NewArguments);
    sort_expression_list ArgumentTypes=reverse(NewArgumentTypes);

    //function
    data_expression Data=appl.head();
    sort_expression NewType;
    try
    {
      NewType=TraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                      Data,
                      // data::untyped_sort(), /* function_sort(ArgumentTypes,PosType) */
                      function_sort(ArgumentTypes,PosType),  // XXXXXXXX
                      FreeVars,false,nArguments,warn_upcasting,print_cast_error);
    }
    catch (mcrl2::runtime_error& e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\nType error while trying to cast an application of " +
                            data::pp(Data) + " to arguments " + data::pp(Arguments) + " to type " + data::pp(PosType) + ".");
    }

    //it is possible that:
    //1) a cast has happened
    //2) some parameter Types became sharper.
    //we do the arguments again with the types.


    if (is_function_sort(UnwindType(NewType)))
    {
      sort_expression_list NeededArgumentTypes=down_cast<function_sort>(UnwindType(NewType)).domain();

      if (NeededArgumentTypes.size()!=Arguments.size())
      {
         throw mcrl2::runtime_error("Need argumens of sorts " + data::pp(NeededArgumentTypes) +
                         " which does not match the number of provided arguments "
                            + data::pp(Arguments) + " (while typechecking "
                            + data::pp(DataTerm) + ").");
      }
      //arguments again
      sort_expression_list NewArgumentTypes;
      data_expression_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        assert(!Arguments.empty());
        assert(!NeededArgumentTypes.empty());
        data_expression Arg=Arguments.front();
        const sort_expression& NeededType=NeededArgumentTypes.front();
        sort_expression Type=ArgumentTypes.front();
        if (!EqTypesA(NeededType,Type))
        {
          //upcasting
          try
          {
            Type=UpCastNumericType(NeededType,Type,Arg,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error&)
          {
          }
        }
        if (!EqTypesA(NeededType,Type))
        {
          sort_expression NewArgType;
          if (!TypeMatchA(NeededType,Type,NewArgType))
          {
            if (!TypeMatchA(NeededType,ExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try
          {
            NewArgType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nRequired type " + data::pp(NeededType) + " does not match possible type "
                            + data::pp(Type) + " (while typechecking " + data::pp(Arg) + " in " + data::pp(DataTerm) + ").");
          }
          Type=NewArgType;
        }
        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    //the function again
    try
    {
      NewType=TraverseVarConsTypeDN(DeclaredVars,AllowedVars,
                                        Data,function_sort(ArgumentTypes,PosType),
                                        FreeVars,strictly_ambiguous,nArguments,warn_upcasting,print_cast_error);
    }
    catch (mcrl2::runtime_error& e)
    {
      if (was_ambiguous)
      {
        was_ambiguous=false;
      }
      throw mcrl2::runtime_error(std::string(e.what()) + "\nType error while trying to cast " +
                   data::pp(application(Data,Arguments)) + " to type " + data::pp(PosType) + ".");
    }

    //and the arguments once more
    if (is_function_sort(UnwindType(NewType)))
    {
      sort_expression_list NeededArgumentTypes=down_cast<function_sort>(UnwindType(NewType)).domain();
      sort_expression_list NewArgumentTypes;
      data_expression_list NewArguments;
      for (; !Arguments.empty(); Arguments=Arguments.tail(),
           ArgumentTypes=ArgumentTypes.tail(),NeededArgumentTypes=NeededArgumentTypes.tail())
      {
        data_expression Arg=Arguments.front();
        const sort_expression& NeededType=NeededArgumentTypes.front();
        sort_expression Type=ArgumentTypes.front();

        if (!EqTypesA(NeededType,Type))
        {
          //upcasting
          try
          {
            Type=UpCastNumericType(NeededType,Type,Arg,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error&)
          {
          }
        }
        if (!EqTypesA(NeededType,Type))
        {
          sort_expression NewArgType;
          if (!TypeMatchA(NeededType,Type,NewArgType))
          {
            if (!TypeMatchA(NeededType,ExpandNumTypesUp(Type),NewArgType))
            {
              NewArgType=NeededType;
            }
          }
          try
          {
            NewArgType=TraverseVarConsTypeD(DeclaredVars,AllowedVars,Arg,NewArgType,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          }
          catch (mcrl2::runtime_error& e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nNeeded type " + data::pp(NeededType) + " does not match possible type "
                            + data::pp(Type) + " (while typechecking " + data::pp(Arg) + " in " + data::pp(DataTerm) + ").");
          }
          Type=NewArgType;
        }

        NewArguments.push_front(Arg);
        NewArgumentTypes.push_front(Type);
      }
      Arguments=reverse(NewArguments);
      ArgumentTypes=reverse(NewArgumentTypes);
    }

    DataTerm=application(Data,Arguments);

    if (is_function_sort(UnwindType(NewType)))
    {
      return atermpp::down_cast<function_sort>(UnwindType(NewType)).codomain();
    }

    sort_expression temp_type;
    if (!UnArrowProd(ArgumentTypes,NewType,temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + data::pp(DataTerm) + ".");
    }
    if (detail::HasUnknown(temp_type))
    {
      throw mcrl2::runtime_error("Fail to properly type " + data::pp(DataTerm) + ".");
    }
    return temp_type;
  }

  if (data::is_untyped_identifier(DataTerm)||data::is_function_symbol(DataTerm)||is_variable(DataTerm))
  {
    core::identifier_string Name=
              data::is_untyped_identifier(DataTerm)?down_cast<untyped_identifier>(DataTerm).name():
              is_function_symbol(DataTerm)?down_cast<function_symbol>(DataTerm).name():
                                           atermpp::down_cast<variable>(DataTerm).name();
    if (utilities::is_numeric_string(Name.function().name()))
    {
      sort_expression Sort=sort_int::int_();
      if (detail::IsPos(Name))
      {
        Sort=sort_pos::pos();
      }
      else if (detail::IsNat(Name))
      {
        Sort=sort_nat::nat();
      }
      DataTerm=data::function_symbol(Name,Sort);

      sort_expression temp;
      if (TypeMatchA(Sort,PosType,temp))
      {
        return Sort;
      }

      //upcasting
      sort_expression CastedNewType;
      try
      {
        CastedNewType=UpCastNumericType(PosType,Sort,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nCannot (up)cast number " + data::pp(DataTerm) + " to type " + data::pp(PosType) + ".");
      }
      return CastedNewType;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator it=DeclaredVars.find(Name);
    if (it!=DeclaredVars.end())
    {
      sort_expression Type=it->second;
      DataTerm=variable(Name,Type);

      if (AllowedVars.count(Name)==0)
      {
        throw mcrl2::runtime_error("Variable " + core::pp(Name) + " occurs freely in the right-hand-side or condition of an equation, but not in the left-hand-side.");
      }

      sort_expression NewType;
      if (TypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
      }
      else
      {
        //upcasting
        sort_expression CastedNewType;
        try
        {
          CastedNewType=UpCastNumericType(PosType,Type,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
        }
        catch (mcrl2::runtime_error& e)
        {
          if (print_cast_error)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nCannot (up)cast variable " + data::pp(DataTerm) + " to type " + data::pp(PosType) + ".");
          }
          else
          {
            throw e;
          }
        }

        Type=CastedNewType;
      }

      //Add to free variables list
      FreeVars[Name]=Type;
      return Type;
    }

    std::map<core::identifier_string,sort_expression>::const_iterator i=user_constants.find(Name);
    if (i!=user_constants.end())
    {
      sort_expression Type=i->second;
      sort_expression NewType;
      if (TypeMatchA(Type,PosType,NewType))
      {
        Type=NewType;
        DataTerm=data::function_symbol(Name,Type);
        return Type;
      }
      else
      {
        // The type cannot be unified. Try upcasting the type.
        DataTerm=data::function_symbol(Name,Type);
        try
        {
          return UpCastNumericType(PosType,Type,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nNo constant " + data::pp(DataTerm) + " with type " + data::pp(PosType) + ".");
        }
      }
    }

    std::map<core::identifier_string,sort_expression_list>::const_iterator j=system_constants.find(Name);
    if (j!=system_constants.end())
    {
      sort_expression_list TypeList=j->second;
      sort_expression_list NewParList;
      for (sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i)
      {
        const sort_expression Type=*i;
        sort_expression result;
        if (TypeMatchA(Type,PosType,result))
        {
          DataTerm=data::function_symbol(Name,result);
          NewParList.push_front(result);
        }
      }
      sort_expression_list ParList=reverse(NewParList);
      if (ParList.empty())
      {
        // Try to do the matching again with relaxed typing.
        for (sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i)
        {
          sort_expression Type=*i;
          if (is_untyped_identifier(DataTerm) )
          {
            DataTerm=data::function_symbol(Name,Type);
          }
          Type=UpCastNumericType(PosType,Type,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          // if (EqTypesA(Type,PosType))
          sort_expression result;
          if (TypeMatchA(Type,PosType,result))
          {
            NewParList.push_front(result);
          }
        }
        ParList=reverse(NewParList);
      }

      if (ParList.empty())
      {
        throw mcrl2::runtime_error("No system constant " + data::pp(DataTerm) + " with type " + data::pp(PosType) + ".");
      }

      if (ParList.size()==1)
      {
        const sort_expression& Type=ParList.front();

        if (is_untyped_identifier(DataTerm) )
        {
          assert(0);
          DataTerm=data::function_symbol(Name,Type);
        }
        try
        {
          sort_expression r= UpCastNumericType(PosType,Type,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
          return r;
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nNo constant " + data::pp(DataTerm) + " with type " + data::pp(PosType) + ".");
        }
      }
      else
      {
        DataTerm=data::function_symbol(Name,data::untyped_sort());
        return data::untyped_sort();
      }
    }

    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_context=user_functions.find(Name);
    const std::map <core::identifier_string,sort_expression_list>::const_iterator j_gssystem=system_functions.find(Name);

    sort_expression_list ParList;
    if (j_context==user_functions.end())
    {
      if (j_gssystem!=system_functions.end())
      {
        ParList=j_gssystem->second; // The function only occurs in the system.
      }
      else
      {

        throw mcrl2::runtime_error("Unknown operation " + core::pp(Name) + ".");
      }
    }
    else if (j_gssystem==system_functions.end())
    {
      ParList=j_context->second; // only the context sorts are defined.
    }
    else  // Both are defined.
    {
      ParList=j_gssystem->second+j_context->second;
    }


    if (ParList.size()==1)
    {
      const sort_expression& Type=ParList.front();
      DataTerm=data::function_symbol(Name,Type);
      try
      {
        return UpCastNumericType(PosType,Type,DataTerm,DeclaredVars,AllowedVars,FreeVars,strictly_ambiguous,warn_upcasting,print_cast_error);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nNo constant " + data::pp(DataTerm) + " with type " + data::pp(PosType) + ".");
      }
    }
    else
    {
      return TraverseVarConsTypeDN(DeclaredVars, AllowedVars, DataTerm, PosType, FreeVars, strictly_ambiguous, std::string::npos, warn_upcasting,print_cast_error);
    }
  }

  throw mcrl2::runtime_error("Internal type checking error: " + data::pp(DataTerm) + " does not match any type checking case." );
}

void mcrl2::data::data_type_checker::read_constructors_and_mappings(const function_symbol_vector& constructors, const function_symbol_vector& mappings, const function_symbol_vector& normalized_constructors)
{
  size_t constr_number=constructors.size();
  function_symbol_vector functions_and_constructors=constructors;
  functions_and_constructors.insert(functions_and_constructors.end(),mappings.begin(),mappings.end());
  for (const function_symbol& Func: functions_and_constructors)
  {
    const core::identifier_string& FuncName=Func.name();
    sort_expression FuncType=Func.sort();

    check_sort_is_declared(FuncType);

    //if FuncType is a defined function sort, unwind it
    if (is_basic_sort(FuncType))
    {
      const sort_expression NewFuncType=UnwindType(FuncType);
      if (is_function_sort(NewFuncType))
      {
        FuncType=NewFuncType;
      }
    }

    if (is_function_sort(FuncType))
    {
      add_function(data::function_symbol(FuncName,FuncType),"function");
    }
    else
    {
      try
      {
        add_constant(data::function_symbol(FuncName,FuncType),"constant");
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nCould not add constant.");
      }
    }

    if (constr_number)
    {
      constr_number--;

      //Here checks for the constructors
      sort_expression ConstructorType=FuncType;
      if (is_function_sort(ConstructorType))
      {
        ConstructorType=down_cast<function_sort>(ConstructorType).codomain();
      }
      ConstructorType=UnwindType(ConstructorType);
      if (!is_basic_sort(ConstructorType) ||
          sort_bool::is_bool(sort_expression(ConstructorType)) ||
          sort_pos::is_pos(sort_expression(ConstructorType)) ||
          sort_nat::is_nat(sort_expression(ConstructorType)) ||
          sort_int::is_int(sort_expression(ConstructorType)) ||
          sort_real::is_real(sort_expression(ConstructorType))
          )
      {
        throw mcrl2::runtime_error("Could not add constructor " + core::pp(FuncName) + " of sort " + data::pp(FuncType) + ". Constructors of built-in sorts are not allowed.");
      }
    }
  }

  // Check that the constructors are defined such that they cannot generate an empty sort.
  // E.g. in the specification sort D; cons f:D->D; the sort D must be necessarily empty, which is
  // forbidden. The function below checks whether such malicious specifications occur.

  check_for_empty_constructor_domains(normalized_constructors); // throws exception if not ok.
}

void mcrl2::data::data_type_checker::add_constant(const data::function_symbol& f, const std::string& msg)
{
  const core::identifier_string& Name = f.name();
  const sort_expression& Sort = f.sort();

  if (user_constants.count(Name)>0)
  {
    throw mcrl2::runtime_error("Double declaration of " + msg + " " + core::pp(Name) + ".");
  }

  if (system_constants.count(Name)>0 || system_functions.count(Name)>0)
  {
    throw mcrl2::runtime_error("Attempt to declare a constant with the name that is a built-in identifier (" + core::pp(Name) + ").");
  }

  user_constants[Name]=Sort;
}


bool mcrl2::data::data_type_checker::TypeMatchL(
                     const sort_expression_list& TypeList,
                     const sort_expression_list& PosTypeList,
                     sort_expression_list& result)
{
  if (TypeList.size()!=PosTypeList.size())
  {
    return false;
  }

  sort_expression_list Result;
  sort_expression_list::const_iterator j=PosTypeList.begin();
  for (sort_expression_list::const_iterator i=TypeList.begin(); i!=TypeList.end(); ++i, ++j)
  {
    sort_expression Type;
    if (!TypeMatchA(*i,*j,Type))
    {
      return false;
    }
    Result.push_front(Type);
  }
  result=reverse(Result);
  return true;
}


sort_expression mcrl2::data::data_type_checker::UnwindType(const sort_expression& Type)
{
  // I expect that return normalize_sorts(Type,get_sort_specification()); would also do the job.
  if (is_container_sort(Type))
  {
    const container_sort& cs=down_cast<const container_sort>(Type);
    return container_sort(cs.container_name(),UnwindType(cs.element_sort()));
  }
  if (is_function_sort(Type))
  {
    const function_sort& fs=down_cast<function_sort>(Type);
    sort_expression_list NewArgs;
    for (sort_expression_list::const_iterator i=fs.domain().begin(); i!=fs.domain().end(); ++i)
    {
      NewArgs.push_front(UnwindType(*i));
    }
    NewArgs=reverse(NewArgs);
    return function_sort(NewArgs,UnwindType(fs.codomain()));
  }

  if (is_basic_sort(Type))
  {
    const basic_sort& bs=down_cast<const basic_sort>(Type);
    // std::map<basic_sort, sort_expression>::const_iterator i=m_aliases.find(bs.name()); if (i==m_aliases.end())
    for(const alias& a: get_sort_specification().user_defined_aliases())
    {
      if (bs==a.name())
      {
        return UnwindType(a.reference());
      }
    }
    return Type;
  }

  return Type;
}

variable mcrl2::data::data_type_checker::UnwindType(const variable& v)
{
  return variable(v.name(),UnwindType(v.sort()));
}

bool mcrl2::data::data_type_checker::TypeMatchA(
                 const sort_expression& Type_in,
                 const sort_expression& PosType_in,
                 sort_expression& result)
{
  // Checks if Type and PosType match by instantiating unknown sorts.
  // It returns the matching instantiation of Type in result. If matching fails,
  // it returns false, otherwise true.

  sort_expression Type=Type_in;
  sort_expression PosType=PosType_in;

  if (data::is_untyped_sort(Type))
  {
    result=PosType;
    return true;
  }
  if (data::is_untyped_sort(PosType) || EqTypesA(Type,PosType))
  {
    result=Type;
    return true;
  }
  if (is_untyped_possible_sorts(Type) && !is_untyped_possible_sorts(PosType))
  {
    PosType.swap(Type);
  }
  if (is_untyped_possible_sorts(PosType))
  {
    sort_expression_list NewTypeList;
    const untyped_possible_sorts& mps=down_cast<const untyped_possible_sorts>(PosType);
    for (sort_expression_list::const_iterator i=mps.sorts().begin(); i!=mps.sorts().end(); ++i)
    {
      sort_expression NewPosType= *i;

      sort_expression new_type;
      if (TypeMatchA(Type,NewPosType,new_type))
      {
        NewPosType=new_type;
        // Avoid double insertions.
        if (std::find(NewTypeList.begin(),NewTypeList.end(),NewPosType)==NewTypeList.end())
        {
          NewTypeList.push_front(NewPosType);
        }
      }
    }
    if (NewTypeList.empty())
    {
      return false;
    }
    if (NewTypeList.tail().empty())
    {
      result=NewTypeList.front();
      return true;
    }

    result=untyped_possible_sorts(sort_expression_list(reverse(NewTypeList)));
    return true;
  }

  if (is_basic_sort(Type))
  {
    Type=UnwindType(Type);
  }
  if (is_basic_sort(PosType))
  {
    PosType=UnwindType(PosType);
  }
  if (is_container_sort(Type))
  {
    const container_sort& s=down_cast<container_sort>(Type);
    const container_type& ConsType = s.container_name();
    if (is_list_container(ConsType))
    {
      if (!sort_list::is_list(PosType))
      {
        return false;
      }
      sort_expression Res;
      if (!TypeMatchA(s.element_sort(),down_cast<container_sort>(PosType).element_sort(),Res))
      {
        return false;
      }
      result=sort_list::list(Res);
      return true;
    }

    if (is_set_container(ConsType))
    {
      if (!sort_set::is_set(PosType))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(s.element_sort(),down_cast<container_sort>(PosType).element_sort(),Res))
        {
          return false;
        }
        result=sort_set::set_(Res);
        return true;
      }
    }

    if (is_bag_container(ConsType))
    {
      if (!sort_bag::is_bag(PosType))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(s.element_sort(),down_cast<container_sort>(PosType).element_sort(),Res))
        {
          return false;
        }
        result=sort_bag::bag(Res);
        return true;
      }
    }

    if (is_fset_container(ConsType))
    {
      if (!sort_fset::is_fset(PosType))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(s.element_sort(),down_cast<container_sort>(PosType).element_sort(),Res))
        {
          return false;
        }
        result=sort_fset::fset(Res);
        return true;
      }
    }

    if (is_fbag_container(ConsType))
    {
      if (!sort_fbag::is_fbag(PosType))
      {
        return false;
      }
      else
      {
        sort_expression Res;
        if (!TypeMatchA(s.element_sort(),down_cast<container_sort>(PosType).element_sort(),Res))
        {
          return false;
        }
        result=sort_fbag::fbag(Res);
        return true;
      }
    }
  }

  if (is_function_sort(Type))
  {
    if (!is_function_sort(PosType))
    {
      return false;
    }
    else
    {
      const function_sort& fs=down_cast<const function_sort>(Type);
      const function_sort& posfs=down_cast<const function_sort>(PosType);
      sort_expression_list ArgTypes;
      if (!TypeMatchL(fs.domain(),posfs.domain(),ArgTypes))
      {
        return false;
      }
      sort_expression ResType;
      if (!TypeMatchA(fs.codomain(),posfs.codomain(),ResType))
      {
        return false;
      }
      result=function_sort(ArgTypes,ResType);
      return true;
    }
  }

  return false;
}


void mcrl2::data::data_type_checker::add_system_constant(const data::function_symbol& f)
{
  // append the Type to the entry of the Name of the OpId in system constants table

  const core::identifier_string& OpIdName = f.name();
  const sort_expression& Type = f.sort();

  std::map<core::identifier_string,sort_expression_list>::const_iterator i=system_constants.find(OpIdName);

  sort_expression_list Types;
  if (i!=system_constants.end())
  {
    Types=i->second;
  }
  Types=push_back(Types,Type);
  system_constants[OpIdName]=Types;
}

void mcrl2::data::data_type_checker::add_system_function(const data::function_symbol& f)
{
  //Pre: OpId is an OpId
  // append the Type to the entry of the Name of the OpId in gssystem.functions table
  const core::identifier_string& OpIdName = f.name();
  const sort_expression&  Type = f.sort();
  assert(is_function_sort(Type));

  const std::map <core::identifier_string,sort_expression_list>::const_iterator j=system_functions.find(OpIdName);

  sort_expression_list Types;
  if (j!=system_functions.end())
  {
    Types=j->second;
  }
  Types=Types + sort_expression_list({ Type });  // TODO: Avoid concatenate but the order is essential.
  system_functions[OpIdName]=Types;
}


void mcrl2::data::data_type_checker::initialise_system_defined_functions(void)
{
  //Creation of operation identifiers for system defined operations.
  //Bool
  add_system_constant(sort_bool::true_());
  add_system_constant(sort_bool::false_());
  add_system_function(sort_bool::not_());
  add_system_function(sort_bool::and_());
  add_system_function(sort_bool::or_());
  add_system_function(sort_bool::implies());
  add_system_function(equal_to(data::untyped_sort()));
  add_system_function(not_equal_to(data::untyped_sort()));
  add_system_function(if_(data::untyped_sort()));
  add_system_function(less(data::untyped_sort()));
  add_system_function(less_equal(data::untyped_sort()));
  add_system_function(greater_equal(data::untyped_sort()));
  add_system_function(greater(data::untyped_sort()));
  //Numbers
  add_system_function(sort_nat::pos2nat());
  add_system_function(sort_nat::cnat());
  add_system_function(sort_real::pos2real());
  add_system_function(sort_nat::nat2pos());
  add_system_function(sort_int::nat2int());
  add_system_function(sort_int::cint());
  add_system_function(sort_real::nat2real());
  add_system_function(sort_int::int2pos());
  add_system_function(sort_int::int2nat());
  add_system_function(sort_real::int2real());
  add_system_function(sort_real::creal());
  add_system_function(sort_real::real2pos());
  add_system_function(sort_real::real2nat());
  add_system_function(sort_real::real2int());
  add_system_constant(sort_pos::c1());
  //Square root for the natural numbers.
  add_system_function(sort_nat::sqrt());
  //more about numbers
  add_system_function(sort_real::maximum(sort_pos::pos(),sort_pos::pos()));
  add_system_function(sort_real::maximum(sort_pos::pos(),sort_nat::nat()));
  add_system_function(sort_real::maximum(sort_nat::nat(),sort_pos::pos()));
  add_system_function(sort_real::maximum(sort_nat::nat(),sort_nat::nat()));
  add_system_function(sort_real::maximum(sort_pos::pos(),sort_int::int_()));
  add_system_function(sort_real::maximum(sort_int::int_(),sort_pos::pos()));
  add_system_function(sort_real::maximum(sort_nat::nat(),sort_int::int_()));
  add_system_function(sort_real::maximum(sort_int::int_(),sort_nat::nat()));
  add_system_function(sort_real::maximum(sort_int::int_(),sort_int::int_()));
  add_system_function(sort_real::maximum(sort_real::real_(),sort_real::real_()));
  //more
  add_system_function(sort_real::minimum(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_real::minimum(sort_nat::nat(), sort_nat::nat()));
  add_system_function(sort_real::minimum(sort_int::int_(), sort_int::int_()));
  add_system_function(sort_real::minimum(sort_real::real_(), sort_real::real_()));
  //more
  // add_system_function(sort_real::abs(sort_pos::pos()));
  // add_system_function(sort_real::abs(sort_nat::nat()));
  add_system_function(sort_real::abs(sort_int::int_()));
  add_system_function(sort_real::abs(sort_real::real_()));
  //more
  add_system_function(sort_real::negate(sort_pos::pos()));
  add_system_function(sort_real::negate(sort_nat::nat()));
  add_system_function(sort_real::negate(sort_int::int_()));
  add_system_function(sort_real::negate(sort_real::real_()));
  add_system_function(sort_real::succ(sort_pos::pos()));
  add_system_function(sort_real::succ(sort_nat::nat()));
  add_system_function(sort_real::succ(sort_int::int_()));
  add_system_function(sort_real::succ(sort_real::real_()));
  add_system_function(sort_real::pred(sort_pos::pos()));
  add_system_function(sort_real::pred(sort_nat::nat()));
  add_system_function(sort_real::pred(sort_int::int_()));
  add_system_function(sort_real::pred(sort_real::real_()));
  add_system_function(sort_real::plus(sort_pos::pos(),sort_pos::pos()));
  add_system_function(sort_real::plus(sort_pos::pos(),sort_nat::nat()));
  add_system_function(sort_real::plus(sort_nat::nat(),sort_pos::pos()));
  add_system_function(sort_real::plus(sort_nat::nat(),sort_nat::nat()));
  add_system_function(sort_real::plus(sort_int::int_(),sort_int::int_()));
  add_system_function(sort_real::plus(sort_real::real_(),sort_real::real_()));
  //more
  add_system_function(sort_real::minus(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_real::minus(sort_nat::nat(), sort_nat::nat()));
  add_system_function(sort_real::minus(sort_int::int_(), sort_int::int_()));
  add_system_function(sort_real::minus(sort_real::real_(), sort_real::real_()));
  add_system_function(sort_real::times(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_real::times(sort_nat::nat(), sort_nat::nat()));
  add_system_function(sort_real::times(sort_int::int_(), sort_int::int_()));
  add_system_function(sort_real::times(sort_real::real_(), sort_real::real_()));
  //more
  // add_system_function(sort_int::div(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_int::div(sort_nat::nat(), sort_pos::pos()));
  add_system_function(sort_int::div(sort_int::int_(), sort_pos::pos()));
  // add_system_function(sort_int::mod(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_int::mod(sort_nat::nat(), sort_pos::pos()));
  add_system_function(sort_int::mod(sort_int::int_(), sort_pos::pos()));
  add_system_function(sort_real::divides(sort_pos::pos(), sort_pos::pos()));
  add_system_function(sort_real::divides(sort_nat::nat(), sort_nat::nat()));
  add_system_function(sort_real::divides(sort_int::int_(), sort_int::int_()));
  add_system_function(sort_real::divides(sort_real::real_(), sort_real::real_()));
  add_system_function(sort_real::exp(sort_pos::pos(), sort_nat::nat()));
  add_system_function(sort_real::exp(sort_nat::nat(), sort_nat::nat()));
  add_system_function(sort_real::exp(sort_int::int_(), sort_nat::nat()));
  add_system_function(sort_real::exp(sort_real::real_(), sort_int::int_()));
  add_system_function(sort_real::floor());
  add_system_function(sort_real::ceil());
  add_system_function(sort_real::round());
  //Lists
  add_system_constant(sort_list::empty(data::untyped_sort()));
  add_system_function(sort_list::cons_(data::untyped_sort()));
  add_system_function(sort_list::count(data::untyped_sort()));
  add_system_function(sort_list::snoc(data::untyped_sort()));
  add_system_function(sort_list::concat(data::untyped_sort()));
  add_system_function(sort_list::element_at(data::untyped_sort()));
  add_system_function(sort_list::head(data::untyped_sort()));
  add_system_function(sort_list::tail(data::untyped_sort()));
  add_system_function(sort_list::rhead(data::untyped_sort()));
  add_system_function(sort_list::rtail(data::untyped_sort()));
  add_system_function(sort_list::in(data::untyped_sort()));

  //Sets

  add_system_function(sort_bag::set2bag(data::untyped_sort()));
  add_system_function(sort_set::in(data::untyped_sort(), data::untyped_sort(), sort_fset::fset(data::untyped_sort())));
  add_system_function(sort_set::in(data::untyped_sort(), data::untyped_sort(), sort_set::set_(data::untyped_sort())));
  add_system_function(sort_set::union_(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
  add_system_function(sort_set::union_(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
  add_system_function(sort_set::difference(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
  add_system_function(sort_set::difference(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
  add_system_function(sort_set::intersection(data::untyped_sort(), sort_fset::fset(data::untyped_sort()), sort_fset::fset(data::untyped_sort())));
  add_system_function(sort_set::intersection(data::untyped_sort(), sort_set::set_(data::untyped_sort()), sort_set::set_(data::untyped_sort())));
  add_system_function(sort_set::false_function(data::untyped_sort())); // Needed as it is used within the typechecker.
  add_system_function(sort_set::constructor(data::untyped_sort())); // Needed as it is used within the typechecker.
  //**** add_system_function(sort_bag::set2bag(data::untyped_sort()));
  // add_system_constant(sort_set::empty(data::untyped_sort()));
  // add_system_function(sort_set::in(data::untyped_sort()));
  // add_system_function(sort_set::union_(data::untyped_sort()));
  // add_system_function(sort_set::difference(data::untyped_sort()));
  // add_system_function(sort_set::intersection(data::untyped_sort()));
  add_system_function(sort_set::complement(data::untyped_sort()));

  //FSets
  add_system_constant(sort_fset::empty(data::untyped_sort()));
  // add_system_function(sort_fset::in(data::untyped_sort()));
  // add_system_function(sort_fset::union_(data::untyped_sort()));
  // add_system_function(sort_fset::intersection(data::untyped_sort()));
  // add_system_function(sort_fset::difference(data::untyped_sort()));
  add_system_function(sort_fset::count(data::untyped_sort()));
  add_system_function(sort_fset::insert(data::untyped_sort())); // Needed as it is used within the typechecker.

  //Bags
  add_system_function(sort_bag::bag2set(data::untyped_sort()));
  add_system_function(sort_bag::in(data::untyped_sort(), data::untyped_sort(), sort_fbag::fbag(data::untyped_sort())));
  add_system_function(sort_bag::in(data::untyped_sort(), data::untyped_sort(), sort_bag::bag(data::untyped_sort())));
  add_system_function(sort_bag::union_(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
  add_system_function(sort_bag::union_(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
  add_system_function(sort_bag::difference(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
  add_system_function(sort_bag::difference(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
  add_system_function(sort_bag::intersection(data::untyped_sort(), sort_fbag::fbag(data::untyped_sort()), sort_fbag::fbag(data::untyped_sort())));
  add_system_function(sort_bag::intersection(data::untyped_sort(), sort_bag::bag(data::untyped_sort()), sort_bag::bag(data::untyped_sort())));
  add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_fbag::fbag(data::untyped_sort())));
  add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_bag::bag(data::untyped_sort())));
  // add_system_constant(sort_bag::empty(data::untyped_sort()));
  // add_system_function(sort_bag::in(data::untyped_sort()));
  //**** add_system_function(sort_bag::count(data::untyped_sort()));
  // add_system_function(sort_bag::count(data::untyped_sort(), data::untyped_sort(), sort_fset::fset(data::untyped_sort())));
  //add_system_function(sort_bag::join(data::untyped_sort()));
  // add_system_function(sort_bag::difference(data::untyped_sort()));
  // add_system_function(sort_bag::intersection(data::untyped_sort()));
  add_system_function(sort_bag::zero_function(data::untyped_sort())); // Needed as it is used within the typechecker.
  add_system_function(sort_bag::constructor(data::untyped_sort())); // Needed as it is used within the typechecker.

  //FBags
  add_system_constant(sort_fbag::empty(data::untyped_sort()));
  // add_system_function(sort_fbag::count(data::untyped_sort()));
  // add_system_function(sort_fbag::in(data::untyped_sort()));
  // add_system_function(sort_fbag::union_(data::untyped_sort()));
  // add_system_function(sort_fbag::intersection(data::untyped_sort()));
  // add_system_function(sort_fbag::difference(data::untyped_sort()));
  add_system_function(sort_fbag::count_all(data::untyped_sort()));
  add_system_function(sort_fbag::cinsert(data::untyped_sort())); // Needed as it is used within the typechecker.

  // function update
  add_system_function(data::function_update(data::untyped_sort(),data::untyped_sort()));
}

void mcrl2::data::data_type_checker::add_function(const data::function_symbol& f, const std::string& msg, bool allow_double_decls)
{
  const sort_expression_list domain=function_sort(f.sort()).domain();
  const core::identifier_string& Name = f.name();
  const sort_expression& Sort = f.sort();

  if (system_constants.count(Name)>0)
  {
    throw mcrl2::runtime_error("Attempt to redeclare the system constant with a " + msg + " " + data::pp(f) + ".");
  }

  if (system_functions.count(Name)>0)
  {
    throw mcrl2::runtime_error("Attempt to redeclare a system function with a " + msg + " " + data::pp(f) + ".");
  }

  std::map <core::identifier_string,sort_expression_list>::const_iterator j=user_functions.find(Name);

  // the table user_functions contains a list of types for each
  // function name. We need to check if there is already such a type
  // in the list. If so -- error, otherwise -- add
  if (j!=user_functions.end())
  {
    sort_expression_list Types=j->second;
    if (InTypesA(Sort, Types))
    {
      if (!allow_double_decls)
      {
        throw mcrl2::runtime_error("Double declaration of " + msg + " " + core::pp(Name) + ".");
      }
    }
    Types = Types + sort_expression_list({ Sort });
    user_functions[Name]=Types;
  }
  else
  {
    user_functions[Name] = sort_expression_list({ Sort });
  }
}

void mcrl2::data::data_type_checker::read_sort(const sort_expression& SortExpr)
{
  if (is_basic_sort(SortExpr))
  {
    check_basic_sort_is_declared(down_cast<basic_sort>(SortExpr).name());
    return;
  }

  if (is_container_sort(SortExpr))
  {
    return read_sort(down_cast<container_sort>(SortExpr).element_sort());
  }

  if (is_function_sort(SortExpr))
  {
    const function_sort& fs = atermpp::down_cast<function_sort>(SortExpr);
    read_sort(fs.codomain());

    for (sort_expression_list::const_iterator i=fs.domain().begin(); i!=fs.domain().end(); ++i)
    {
      read_sort(*i);
    }
    return;
  }

  if (is_structured_sort(SortExpr))
  {
    const structured_sort& struct_sort = atermpp::down_cast<structured_sort>(SortExpr);
    for (structured_sort_constructor_list::const_iterator i=struct_sort.constructors().begin();
               i!=struct_sort.constructors().end(); ++i)
    {
      const structured_sort_constructor& Constr(*i);

      // recognizer -- if present -- a function from SortExpr to Bool
      core::identifier_string Name=Constr.recogniser();
      if (Name!=core::empty_identifier_string())
      {
        add_function(data::function_symbol(Name,function_sort(sort_expression_list({ SortExpr }),sort_bool::bool_())),"recognizer");
      }

      // constructor type and projections
      const structured_sort_constructor_argument_list& Projs=Constr.arguments();
      Name=Constr.name();
      if (Projs.empty())
      {
        add_constant(data::function_symbol(Name,SortExpr),"constructor constant");
        continue;
      }

      sort_expression_list ConstructorType;
      for (structured_sort_constructor_argument_list::const_iterator j=Projs.begin(); j!=Projs.end(); ++j)
      {
        structured_sort_constructor_argument Proj= *j;
        const sort_expression& ProjSort=Proj.sort();

        // not to forget, recursive call for ProjSort ;-)
        read_sort(ProjSort);

        const core::identifier_string& ProjName=Proj.name();
        if (ProjName!=core::empty_identifier_string())
        {
          add_function(function_symbol(ProjName,function_sort(sort_expression_list({ SortExpr }),ProjSort)),"projection",true);
        }
        ConstructorType.push_front(ProjSort);
      }
      add_function(data::function_symbol(Name,function_sort(reverse(ConstructorType),SortExpr)),"constructor");
    }
    return;
  }
}

sort_expression_list mcrl2::data::data_type_checker::InsertType(const sort_expression_list& TypeList, const sort_expression& Type)
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




bool mcrl2::data::data_type_checker::IsTypeAllowedA(const sort_expression& Type, const sort_expression& PosType)
{
  //Checks if Type is allowed by PosType
  if (data::is_untyped_sort(data::sort_expression(PosType)))
  {
    return true;
  }
  if (is_untyped_possible_sorts(PosType))
  {
    return InTypesA(Type,down_cast<const untyped_possible_sorts>(PosType).sorts());
  }

  //PosType is a normal type
  return EqTypesA(Type,PosType);
}

bool mcrl2::data::data_type_checker::IsTypeAllowedL(const sort_expression_list& TypeList, const sort_expression_list& PosTypeList)
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

bool mcrl2::data::data_type_checker::IsNotInferredL(sort_expression_list TypeList)
{
  for (; !TypeList.empty(); TypeList=TypeList.tail())
  {
    const sort_expression& Type=TypeList.front();
    if (is_untyped_sort(Type) || is_untyped_possible_sorts(Type))
    {
      return true;
    }
  }
  return false;
}



std::pair<bool,sort_expression_list> mcrl2::data::data_type_checker::AdjustNotInferredList(
            const sort_expression_list& PosTypeList,
            const term_list<sort_expression_list>& TypeListList)
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


sort_expression_list mcrl2::data::data_type_checker::GetNotInferredList(const term_list<sort_expression_list>& TypeListList)
{
  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..nFormPars-1

  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..nFormPars-1

  sort_expression_list Result;
  size_t nFormPars=TypeListList.front().size();
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
    sort_expression Sort;
    if (Pars[i-1].size()==1)
    {
      Sort=Pars[i-1].front();
    }
    else
    {
      Sort=untyped_possible_sorts(sort_expression_list(reverse(Pars[i-1])));
    }
    Result.push_front(Sort);
  }
  return Result;
}

mcrl2::data::data_type_checker::data_type_checker(const data_specification& data_spec)
//    : sort_type_checker(data_spec.user_defined_sorts(), data_spec.user_defined_aliases()),
      : sort_type_checker(data_spec),
        was_warning_upcasting(false),
        was_ambiguous(false)

{
  initialise_system_defined_functions();

  try
  {
    for (const alias& a: get_sort_specification().user_defined_aliases()) // auto i = m_aliases.begin(); i != m_aliases.end(); ++i)
    {
      read_sort(a.reference());
    }
    read_constructors_and_mappings(data_spec.user_defined_constructors(),data_spec.user_defined_mappings(),data_spec.constructors());
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nType checking of data expression failed.");
  }

  type_checked_data_spec=data_spec;
  type_checked_data_spec.declare_data_specification_to_be_type_checked();

  // Type check equations and add them to the specification.
  try
  {
    TransformVarConsTypeData(type_checked_data_spec);
  }
  catch (mcrl2::runtime_error& e)
  {
    type_checked_data_spec=data_specification(); // Type checking failed. Data specification is not usable.
    throw mcrl2::runtime_error(std::string(e.what()) + "\nFailed to type check data specification.");
  }
}

data_expression mcrl2::data::data_type_checker::operator()(
           const data_expression& data_expr,
           const std::map<core::identifier_string,sort_expression>& Vars)
{
  data_expression data=data_expr;
  sort_expression Type;
  try
  {
    Type=TraverseVarConsTypeD(Vars,Vars,data,data::untyped_sort());
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nType checking of data expression failed.");
  }
  if (data::is_untyped_sort(Type))
  {
    throw mcrl2::runtime_error("Type checking of data expression failed. Result is an unknown sort.");
  }

  assert(strict_type_check(data));
  return data;
}


variable_list mcrl2::data::data_type_checker::operator()(
           const variable_list& l)
{
  std::map<core::identifier_string,sort_expression> Vars;
  std::map<core::identifier_string,sort_expression> NewVars;
  const variable_list& data_vars=l;
  try
  {
    AddVars2Table(Vars,data_vars);
    NewVars=Vars;
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nType error while typechecking data variables.");
  }

  return data_vars;
}

// ------------------------------  Here ends the new class based data expression checker -----------------------
// ------------------------------  Here starts the new class based data specification checker -----------------------

// Type check and replace user defined equations.
void mcrl2::data::data_type_checker::TransformVarConsTypeData(data_specification& data_spec)
{
  std::map<core::identifier_string,sort_expression> DeclaredVars;
  std::map<core::identifier_string,sort_expression> FreeVars;

  //Create a new specification; admittedly, this is somewhat clumsy.
  data_specification new_specification;
  for(basic_sort_vector::const_iterator i=data_spec.user_defined_sorts().begin(); i!=data_spec.user_defined_sorts().end(); ++i)
  {
    new_specification.add_sort(*i);
  }
  for(alias_vector::const_iterator i=data_spec.user_defined_aliases().begin(); i!=data_spec.user_defined_aliases().end(); ++i)
  {
    new_specification.add_alias(*i);
  }
  for(function_symbol_vector::const_iterator i=data_spec.user_defined_constructors().begin(); i!=data_spec.user_defined_constructors().end(); ++i)
  {
    new_specification.add_constructor(*i);
  }
  for(function_symbol_vector::const_iterator i=data_spec.user_defined_mappings().begin(); i!=data_spec.user_defined_mappings().end(); ++i)
  {
    new_specification.add_mapping(*i);
  }

  data_equation_vector equations=data_spec.user_defined_equations();
  for (data_equation_vector::const_iterator i=equations.begin(); i!=equations.end(); ++i)
  {
    data_equation Eqn= *i;
    const variable_list& VarList=Eqn.variables();

    if (!VarsUnique(VarList))
    {
      throw mcrl2::runtime_error("The variables " + data::pp(VarList) + " in equation declaration " + data::pp(Eqn) + " are not unique.");
    }

    std::map<core::identifier_string,sort_expression> NewDeclaredVars;
    AddVars2Table(DeclaredVars,VarList);
    NewDeclaredVars=DeclaredVars;
    DeclaredVars=NewDeclaredVars;

    data_expression Left=Eqn.lhs();

    sort_expression LeftType;
    try
    {
      LeftType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,data::untyped_sort(),FreeVars,false,true);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nError occurred while typechecking " + data::pp(Left) + " as left hand side of equation " + data::pp(Eqn) + ".");
    }

    if (was_warning_upcasting)
    {
      was_warning_upcasting=false;
      mCRL2log(warning) << "Warning occurred while typechecking " << Left << " as left hand side of equation " << Eqn << "." << std::endl;
    }

    data_expression Cond=Eqn.condition();
    TraverseVarConsTypeD(DeclaredVars,FreeVars,Cond,sort_bool::bool_());

    data_expression Right=Eqn.rhs();
    std::map<core::identifier_string,sort_expression> dummy_empty_table;
    sort_expression RightType;
    try
    {
      RightType=TraverseVarConsTypeD(DeclaredVars,FreeVars,Right,LeftType,dummy_empty_table,false);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nError occurred while typechecking " + data::pp(Right) + " as right hand side of equation " + data::pp(Eqn) + ".");
    }

    //If the types are not uniquely the same now: do once more:
    if (!EqTypesA(LeftType,RightType))
    {
      sort_expression Type;
      if (!TypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("Types of the left- (" + data::pp(LeftType) + ") and right- (" + data::pp(RightType) + ") hand-sides of the equation " + data::pp(Eqn) + " do not match.");
      }
      Left=Eqn.lhs();
      FreeVars.clear();
      try
      {
        LeftType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Left,Type,FreeVars,true);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nTypes of the left- and right-hand-sides of the equation " + data::pp(Eqn) + " do not match.");
      }
      if (was_warning_upcasting)
      {
        was_warning_upcasting=false;
        mCRL2log(warning) << "Warning occurred while typechecking " << Left << " as left hand side of equation " << Eqn << "." << std::endl;
      }
      Right=Eqn.rhs();
      try
      {
        RightType=TraverseVarConsTypeD(DeclaredVars,DeclaredVars,Right,LeftType,FreeVars);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nTypes of the left- and right-hand-sides of the equation " + data::pp(Eqn) + " do not match.");
      }
      if (!TypeMatchA(LeftType,RightType,Type))
      {
        throw mcrl2::runtime_error("Types of the left- (" + data::pp(LeftType) + ") and right- (" + data::pp(RightType) + ") hand-sides of the equation " + data::pp(Eqn) + " do not match.");
      }
      if (detail::HasUnknown(Type))
      {
        throw mcrl2::runtime_error("Types of the left- (" + data::pp(LeftType) + ") and right- (" + data::pp(RightType) + ") hand-sides of the equation " + data::pp(Eqn) + " cannot be uniquely determined.");
      }
    }
    DeclaredVars.clear();
    // NewEqns.push_back(data_equation(VarList,Cond,Left,Right));
    new_specification.add_equation(data_equation(VarList,Cond,Left,Right));
  }
  data_spec=new_specification;
}

const data_specification mcrl2::data::data_type_checker::operator()()
{
  return type_checked_data_spec;
}

// ------------------------------  Here ends the new class based data specification checker -----------------------





namespace data
{
namespace detail
{

static std::map<core::identifier_string,sort_expression> RemoveVars(
                      std::map<core::identifier_string,sort_expression>& Vars,
                      variable_list VarDecls)
{
  for (; !VarDecls.empty(); VarDecls=VarDecls.tail())
  {
    const variable& VarDecl=VarDecls.front();
    const core::identifier_string& VarName=VarDecl.name();

    Vars.erase(VarName);
  }

  return Vars;
}

static sort_expression_list GetVarTypes(variable_list VarDecls)
{
  sort_expression_list Result;
  for (const variable& VarDecl: VarDecls)
  {
    Result.push_front(VarDecl.sort());
  }
  return reverse(Result);
}

// Replace occurrences of untyped_possible_sorts([s1,...,sn]) by selecting
// one of the possible sorts from s1,...,sn. Currently, the first is chosen.
static sort_expression replace_possible_sorts(const sort_expression& Type)
{
  if (is_untyped_possible_sorts(data::sort_expression(Type)))
  {
    return atermpp::down_cast<untyped_possible_sorts>(Type).sorts().front(); // get the first element of the possible sorts.
  }
  if (data::is_untyped_sort(data::sort_expression(Type)))
  {
    return data::untyped_sort();
  }
  if (is_basic_sort(Type))
  {
    return Type;
  }
  if (is_container_sort(Type))
  {
    const container_sort& s=down_cast<container_sort>(Type);
    return container_sort(s.container_name(),replace_possible_sorts(s.element_sort()));
  }

  if (is_structured_sort(Type))
  {
    return Type;  // I assume that there are no possible sorts in sort constructors. JFG.
  }

  if (is_function_sort(Type))
  {
    const function_sort& s=down_cast<function_sort>(Type);
    sort_expression_list NewTypeList;
    for (sort_expression_list::const_iterator TypeList=s.domain().begin(); TypeList!=s.domain().end(); ++TypeList)
    {
      NewTypeList.push_front(replace_possible_sorts(*TypeList));
    }
    const sort_expression& ResultType=s.codomain();
    return function_sort(reverse(NewTypeList),replace_possible_sorts(ResultType));
  }
  assert(0); // All cases are dealt with above.
  return Type; // Avoid compiler warnings.
}


static bool HasUnknown(const sort_expression& Type)
{
  if (data::is_untyped_sort(data::sort_expression(Type)))
  {
    return true;
  }
  if (is_basic_sort(Type))
  {
    return false;
  }
  if (is_container_sort(Type))
  {
    return HasUnknown(down_cast<container_sort>(Type).element_sort());
  }
  if (is_structured_sort(Type))
  {
    return false;
  }

  if (is_function_sort(Type))
  {
    const function_sort& s=down_cast<function_sort>(Type);
    for (sort_expression_list::const_iterator TypeList=s.domain().begin(); TypeList!=s.domain().end(); ++TypeList)
      if (HasUnknown(*TypeList))
      {
        return true;
      }
    return HasUnknown(s.codomain());
  }

  return true;
}

static bool IsNumericType(const sort_expression& Type)
{
  //returns true if Type is Bool,Pos,Nat,Int or Real
  //otherwise return fase
  if (data::is_untyped_sort(Type))
  {
    return false;
  }
  return (bool)(sort_bool::is_bool(Type)||
                  sort_pos::is_pos(Type)||
                  sort_nat::is_nat(Type)||
                  sort_int::is_int(Type)||
                  sort_real::is_real(Type));
}


static sort_expression MinType(const sort_expression_list& TypeList)
{
  return TypeList.front();
}

} //namespace detail
} //namespace data
}
