// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TYPECHECK_H
#define MCRL2_MODAL_FORMULA_TYPECHECK_H

#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/is_monotonous.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace state_formulas
{

class state_formula_type_checker : lps::action_type_checker
{

  public:
    /** \brief     Type check a state formula.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A state formula that has not been type checked.
     *  \param[in] check_monotonicity Check whether the formula is monotonic, in the sense that no fixed point
     *             variable occurs in the scope of an odd number of negations.
     *  \return    a state formula where all untyped identifiers have been replace by typed ones.
     **/
    state_formula_type_checker(const data::data_specification& data_spec, const process::action_label_list& action_decls)
      : lps::action_type_checker(data_spec,action_decls)
    {}

    //check correctness of the state formula in state_formula using
    //the process specification or LPS in spec as follows:
    //1) determine the types of actions according to the definitions
    //   in spec
    //2) determine the types of data expressions according to the
    //   definitions in spec
    //3) check for name conflicts of data variable declarations in
    //   forall, exists, mu and nu quantifiers
    //4) check for monotonicity of fixpoint variables
    state_formula operator()(const state_formula& formula, bool check_monotonicity)
    {
      mCRL2log(log::verbose) << "type checking state formula..." << std::endl;

      std::map<core::identifier_string, data::sort_expression> Vars;
      std::map<core::identifier_string, data::sort_expression_list> StateVars;
      state_formula result=TraverseStateFrm(Vars,StateVars,formula);
      if (check_monotonicity && !is_monotonous(result))
      {
        throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(result));
      }
      return result;
    }

  protected:
    state_formula TraverseStateFrm(const std::map<core::identifier_string, data::sort_expression>& Vars, const std::map<core::identifier_string, data::sort_expression_list>& StateVars, const state_formula& StateFrm)
    {
      using namespace state_formulas;

      mCRL2log(log::debug) << "TraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

      if (state_formulas::is_true(StateFrm) || state_formulas::is_false(StateFrm) || state_formulas::is_delay(StateFrm) || state_formulas::is_yaled(StateFrm))
      {
        return StateFrm;
      }

      if (state_formulas::is_not(StateFrm))
      {
        const  not_& t = atermpp::down_cast<const not_>(StateFrm);
        return not_(TraverseStateFrm(Vars,StateVars,t.operand()));
      }

      if (state_formulas::is_and(StateFrm))
      {
        const and_& t=atermpp::down_cast<const and_>(StateFrm);
        return and_(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
      }

      if (state_formulas::is_or(StateFrm))
      {
        const or_& t=atermpp::down_cast<const or_>(StateFrm);
        return or_(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
      }

      if (state_formulas::is_imp(StateFrm))
      {
        const imp& t=atermpp::down_cast<const imp>(StateFrm);
        return imp(TraverseStateFrm(Vars,StateVars,t.left()),TraverseStateFrm(Vars,StateVars,t.right()));
      }

      if (state_formulas::is_forall(StateFrm))
      {
        const forall& t=atermpp::down_cast<const forall>(StateFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,t.variables());
        NewVars=CopyVars;

        return forall(t.variables(),TraverseStateFrm(NewVars,StateVars,t.body()));
      }

      if (state_formulas::is_exists(StateFrm))
      {
        const exists& t=atermpp::down_cast<const exists>(StateFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,t.variables());
        NewVars=CopyVars;

        return exists(t.variables(),TraverseStateFrm(NewVars,StateVars,t.body()));
      }

      if (is_may(StateFrm))
      {
        const may& f=atermpp::down_cast<const may>(StateFrm);
        return may(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,StateVars,f.operand()));
      }

      if (is_must(StateFrm))
      {
        const must& f=atermpp::down_cast<const must>(StateFrm);
        return must(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,StateVars,f.operand()));
      }

      if (state_formulas::is_delay_timed(StateFrm))
      {
        const delay_timed& f=atermpp::down_cast<const delay_timed>(StateFrm);
        data::data_expression Time=f.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

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
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
          }
        }
        return delay_timed(Time);
      }

      if (state_formulas::is_yaled_timed(StateFrm))
      {
        const yaled_timed& f=atermpp::down_cast<const yaled_timed>(StateFrm);
        data::data_expression Time=f.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

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
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
          }
        }
        return yaled_timed(Time);
      }

      if (state_formulas::is_variable(StateFrm))
      {
        state_formulas::variable v=atermpp::down_cast<const state_formulas::variable>(StateFrm);
        core::identifier_string StateVarName=v.name();
        auto i=StateVars.find(StateVarName);
        if (i==StateVars.end())
        {
          throw mcrl2::runtime_error("undefined state variable " + to_string(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
        }
        data::sort_expression_list TypeList=i->second;

        const data::data_expression_list Pars=v.arguments();
        if (TypeList.size()!=Pars.size())
        {
          throw mcrl2::runtime_error("incorrect number of parameters for state variable " + core::pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
        }

        data::data_expression_list r;

        for (data::data_expression Par: Pars)
        {
          data::sort_expression ParType=TypeList.front();
          data::sort_expression NewParType;
          try
          {
            NewParType=TraverseVarConsTypeD(Vars,Vars,Par,ExpandNumTypesDown(ParType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm));
          }

          data::sort_expression temp;
          if (!TypeMatchA(ParType,NewParType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              NewParType=UpCastNumericType(ParType,NewParType,Par,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(Par) + " to type " + pp(ParType) + " (typechecking state formula " + pp(StateFrm) + ")");
            }
          }

          r.push_front(Par);
        }

        return state_formulas::variable(StateVarName, atermpp::reverse(r));

      }

      if (state_formulas::is_nu(StateFrm))
      {
        const nu& f=atermpp::down_cast<const nu>(StateFrm);
        std::map<core::identifier_string, data::sort_expression_list> CopyStateVars(StateVars);

        // Make the new state variable:
        std::map<core::identifier_string,data::sort_expression> FormPars;
        data::assignment_list r;
        data::sort_expression_list t;
        for (const data::assignment& o: f.assignments())
        {
          core::identifier_string VarName=o.lhs().name();
          if (FormPars.count(VarName)>0)
          {
            throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
          }

          data::sort_expression VarType=o.lhs().sort();
          try
          {
            check_sort_is_declared(VarType);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
          }

          FormPars[VarName]=VarType;

          data::data_expression VarInit=o.rhs();
          data::sort_expression VarInitType;
          try
          {
            VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
          }

          data::sort_expression temp;
          if (!TypeMatchA(VarType,VarInitType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              VarInitType=UpCastNumericType(VarType,VarInitType,VarInit,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
            }
          }

          r.push_front(data::assignment(o.lhs(),VarInit));
          t.push_front(VarType);
        }

        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);
        CopyVars.insert(FormPars.begin(),FormPars.end());


        CopyStateVars[f.name()]=atermpp::reverse(t);

        try
        {
          return nu(f.name(),atermpp::reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + to_string(f));
        }
      }

      if (state_formulas::is_mu(StateFrm))
      {
        const mu& f=atermpp::down_cast<const mu>(StateFrm);
        std::map<core::identifier_string, data::sort_expression_list> CopyStateVars(StateVars);

        // Make the new state variable:
        std::map<core::identifier_string, data::sort_expression> FormPars;
        data::assignment_list r;
        data::sort_expression_list t;
        for (const data::assignment& o: f.assignments())
        {
          core::identifier_string VarName=o.lhs().name();
          if (FormPars.count(VarName)>0)
          {
            throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
          }

          data::sort_expression VarType=o.lhs().sort();
          try
          {
            check_sort_is_declared(VarType);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
          }

          FormPars[VarName]=VarType;

          data::data_expression VarInit=o.rhs();
          data::sort_expression VarInitType;
          try
          {
            VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
          }

          data::sort_expression temp;
          if (!TypeMatchA(VarType,VarInitType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              VarInitType=UpCastNumericType(VarType,VarInitType,VarInit,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
            }
          }

          r.push_front(data::assignment(o.lhs(),VarInit));
          t.push_front(VarType);
        }

        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);
        CopyVars.insert(FormPars.begin(),FormPars.end());
        CopyStateVars[f.name()]=atermpp::reverse(t);

        try
        {
          return mu(f.name(),atermpp::reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + to_string(f));
        }
      }

      if (data::is_data_expression(StateFrm))
      {
        data::data_expression d(StateFrm);
        data::sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, data::sort_bool::bool_());
        return d;
      }

      throw mcrl2::runtime_error("Internal error. The state formula " + pp(StateFrm) + " fails to match any known form in typechecking case analysis");
    }

    regular_formulas::regular_formula TraverseRegFrm(const std::map<core::identifier_string, data::sort_expression>& Vars, const regular_formulas::regular_formula& RegFrm)
    {
      using namespace regular_formulas;
      mCRL2log(log::debug) << "TraverseRegFrm: " << pp(RegFrm) << "" << std::endl;
      if (regular_formulas::is_nil(RegFrm))
      {
        return RegFrm;
      }

      if (is_seq(RegFrm))
      {
        const seq t=atermpp::down_cast<const seq>(RegFrm);
        return seq(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
      }

      if (is_alt(RegFrm))
      {
        const alt t=atermpp::down_cast<const alt>(RegFrm);
        return alt(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
      }

      if (is_trans(RegFrm))
      {
        const trans t=atermpp::down_cast<const trans>(RegFrm);
        return trans(TraverseRegFrm(Vars,t.operand()));
      }

      if (is_trans_or_nil(RegFrm))
      {
        const trans t=atermpp::down_cast<const trans>(RegFrm);
        return trans_or_nil(TraverseRegFrm(Vars,t.operand()));
      }

      if (action_formulas::is_action_formula(RegFrm))
      {
        action_formulas::action_formula a = TraverseActFrm(Vars, atermpp::down_cast<const action_formulas::action_formula>(RegFrm));
        return regular_formula(a);
      }

      throw mcrl2::runtime_error("Internal error. The regularformula " + pp(RegFrm) + " fails to match any known form in typechecking case analysis");
    }

    action_formulas::action_formula TraverseActFrm(const std::map<core::identifier_string,data::sort_expression>& Vars, const action_formulas::action_formula& ActFrm)
    {
      using namespace action_formulas;
      mCRL2log(log::debug) << "TraverseActFrm: " << pp(ActFrm) << std::endl;

      if (action_formulas::is_true(ActFrm) || action_formulas::is_false(ActFrm))
      {
        return ActFrm;
      }

      if (action_formulas::is_not(ActFrm))
      {
        const action_formulas::not_& f = atermpp::down_cast<action_formulas::not_>(ActFrm);
        return action_formulas::not_(TraverseActFrm(Vars,f.operand()));
      }

      if (action_formulas::is_and(ActFrm))
      {
        const action_formulas::and_& t=atermpp::down_cast<action_formulas::and_>(ActFrm);
        return action_formulas::and_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_or(ActFrm))
      {
        const action_formulas::or_& t=atermpp::down_cast<action_formulas::or_>(ActFrm);
        return action_formulas::or_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_imp(ActFrm))
      {
        const action_formulas::imp& t=atermpp::down_cast<action_formulas::imp>(ActFrm);
        return action_formulas::imp(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_forall(ActFrm))
      {
        const action_formulas::forall& t=atermpp::down_cast<action_formulas::forall>(ActFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        const data::variable_list& VarList=t.variables();
        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,VarList);
        NewVars=CopyVars;

        return action_formulas::forall(VarList, TraverseActFrm(NewVars,t.body()));
      }

      if (action_formulas::is_exists(ActFrm))
      {
        const action_formulas::exists& t=atermpp::down_cast<action_formulas::exists>(ActFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        const data::variable_list& VarList=t.variables();
        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,VarList);
        NewVars=CopyVars;

        return action_formulas::exists(VarList, TraverseActFrm(NewVars,t.body()));
      }

      if (action_formulas::is_at(ActFrm))
      {
        const action_formulas::at& t=atermpp::down_cast<action_formulas::at>(ActFrm);
        action_formula NewArg1=TraverseActFrm(Vars,t.operand());

        data::data_expression Time=t.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

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
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
          }
        }
        return action_formulas::at(NewArg1,Time);
      }

      if (action_formulas::is_untyped_multi_action(ActFrm))
      {
        const action_formulas::untyped_multi_action ma(ActFrm);
        process::action_list r;
        for (auto l=ma.arguments().begin(); l!=ma.arguments().end(); ++l)
        {
          process::untyped_action o= *l;
          r.push_front(RewrAct(Vars,o));
        }
        return action_formulas::multi_action(atermpp::reverse(r));
      }

      if (data::is_data_expression(ActFrm))
      {
        data::data_expression d(ActFrm);
        data::sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, data::sort_bool::bool_());
        return d;
      }

      throw mcrl2::runtime_error("Internal error. The action formula " + pp(ActFrm) + " fails to match any known form in typechecking case analysis");
    }

};

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] formula A state formula that has not been type checked.
 *  \post      formula is type checked.
 **/
inline
void type_check(state_formula& formula, const lps::specification& lps_spec, bool check_monotonicity = true)
{
  try
  {
    state_formula_type_checker type_checker(lps_spec.data(),lps_spec.action_labels());
    formula=type_checker(formula,check_monotonicity);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula " +
                             mcrl2::utilities::to_string(formula));
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
