// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TYPECHECK_H
#define MCRL2_PBES_TYPECHECK_H

#include "mcrl2/data/typecheck.h"
#include "mcrl2/pbes/normalize_sorts.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

class pbes_type_checker:public data::data_type_checker
{
  protected:
    pbes type_checked_pbes_spec;
    std::map<core::identifier_string,data::sort_expression> glob_vars;      //name -> Type: global variables (for proc, pbes and init)
    std::map <propositional_variable,pbes_expression> pbes_bodies;          //propositional_variable -> pbes_expression
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > PBs;

  public:
    pbes_type_checker(const pbes& pbes_spec)
      : data::data_type_checker(pbes_spec.data())
    {
      using namespace log;
      using namespace atermpp;
      using namespace data;
      //check correctness of the PBES specification in pbes_spec

      mCRL2log(verbose) << "type checking PBES specification..." << std::endl;

      std::vector<pbes_equation> pb_eqn_spec = pbes_spec.equations();
      propositional_variable_instantiation pb_init = pbes_spec.initial_state();
      std::set<data::variable> glob_var_spec = pbes_spec.global_variables();


      mCRL2log(debug) << "type checking of PBES specification read-in phase of sorts finished" << std::endl;

      // Check sorts for loops
      // Unwind sorts to enable equiv and subtype relations

      mCRL2log(debug) << "type checking of PBES specification read-in phase of functions finished" << std::endl;

      std::map<core::identifier_string,sort_expression> dummy;
      AddVars2Table(glob_vars, variable_list(glob_var_spec.begin(),glob_var_spec.end()),dummy);

      mCRL2log(debug) << "type checking of PBES specification read-in phase of global variables finished" << std::endl;

      std::vector<pbes_equation> pbes_equations(pb_eqn_spec.begin(),pb_eqn_spec.end());
      ReadInPBESAndInit(pbes_equations,pb_init);

      mCRL2log(debug) << "type checking PBES read-in phase finished" << std::endl;

      mCRL2log(debug) << "type checking transform Data+PBES phase started" << std::endl;

      TransformPBESVarConst();

      mCRL2log(debug) << "type checking transform Data+PBES phase finished" << std::endl;

      pbes_equations=WritePBES(pbes_equations);
      pb_eqn_spec=std::vector<pbes_equation>(pbes_equations.begin(),pbes_equations.end());

      pb_init=propositional_variable_instantiation(pbes_bodies[INIT_PBES()]);

      type_checked_pbes_spec=pbes(type_checked_data_spec,pb_eqn_spec,glob_var_spec,pb_init);

      // type_checked_pbes_spec=gstcFoldSortRefs(type_checked_pbes_spec);
      normalize_sorts(type_checked_pbes_spec,type_checked_data_spec);
    }


    template <typename VariableContainer, typename PropositionalVariableContainer>
    pbes_type_checker(const data::data_specification& dataspec, const VariableContainer& variables, const PropositionalVariableContainer& propositional_variables)
    : data::data_type_checker(dataspec)
    {
      for (auto i = variables.begin(); i != variables.end(); ++i)
      {
        glob_vars[i->name()] = i->sort();
      }
      for (auto i = propositional_variables.begin(); i != propositional_variables.end(); ++i)
      {
        const data::variable_list& PBVars = i->parameters();
        data::sort_expression_list PBType;
        for (auto l = PBVars.begin(); l != PBVars.end(); ++l)
        {
          PBType.push_front(l->sort());
        }
        PBType = atermpp::reverse(PBType);
        check_sort_list_is_declared(PBType);
        PBs[i->name()] = atermpp::make_list<data::sort_expression_list>(PBType);
      }
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] d A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes operator()()
    {
      return type_checked_pbes_spec;
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] d A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes_expression operator()(const pbes_expression &d)
    {
      return TraversePBESVarConstPB(glob_vars,d);
    }

    protected:

      /* void TransformPBESVarConst(void);
      std::vector<pbes_equation> WritePBES(const std::vector<pbes_equation>& oldPBES);
      void ReadInPBESAndInit(std::vector<pbes_equation> PBEqnSpec, propositional_variable_instantiation PBInit); */
      const propositional_variable INIT_PBES(void)
      {
        static propositional_variable init_pbes(core::identifier_string("init"),data::variable_list());
        return init_pbes;
      }

      void TransformPBESVarConst(void)
      {
        using namespace data;
        std::map<core::identifier_string,sort_expression> Vars;

        //PBEs and data terms in PBEqns and init
        for (std::map <propositional_variable,pbes_expression>::const_iterator i=pbes_bodies.begin(); i!=pbes_bodies.end(); ++i)
        {
          propositional_variable PBVar=i->first;

          Vars=glob_vars;

          std::map<core::identifier_string,sort_expression> NewVars;
          AddVars2Table(Vars,PBVar.parameters(),NewVars);
          Vars=NewVars;

          pbes_expression NewPBTerm=TraversePBESVarConstPB(Vars,i->second);
          pbes_bodies[PBVar]=NewPBTerm;
        }
      }

      std::vector<pbes_equation> WritePBES(const std::vector<pbes_equation>& oldPBES)
      {
        using namespace data;
        std::vector<pbes_equation> Result;
        for (std::vector<pbes_equation>::const_iterator PBEqns=oldPBES.begin(); PBEqns!=oldPBES.end(); ++PBEqns)
        {
          const pbes_equation PBEqn=*PBEqns;
          const propositional_variable PBESVar=PBEqn.variable();

          if (PBESVar==INIT_PBES())
          {
            continue;
          }
          Result.push_back(pbes_equation(PBEqn.symbol(),PBESVar,pbes_bodies[PBESVar]));
        }
        return Result;
      }


      void ReadInPBESAndInit(const std::vector<pbes_equation> &PBEqns, const propositional_variable_instantiation &PBInit)
      {
        using namespace data;
        using namespace atermpp;

        for (std::vector<pbes_equation>::const_iterator j=PBEqns.begin(); j!=PBEqns.end(); ++j)
        {
          const pbes_equation PBEqn= *j;
          core::identifier_string PBName=PBEqn.variable().name();

          const variable_list &PBVars=PBEqn.variable().parameters();

          sort_expression_list PBType;
          for (variable_list::const_iterator l=PBVars.begin(); l!=PBVars.end(); ++l)
          {
            PBType.push_front(l->sort());
          }
          PBType=reverse(PBType);

          check_sort_list_is_declared(PBType);

          const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator i=PBs.find(PBName);
          term_list<sort_expression_list> Types;
          if (i==PBs.end())
          {
            Types=make_list<sort_expression_list>(PBType);
          }
          else
          {
            Types=i->second;
            // temporarily prohibit overloading here
            throw mcrl2::runtime_error("attempt to overload propositional variable " + core::pp(PBName));

            // the table PBs contains a list of types for each
            // PBES name. We need to check if there is already such a type
            // in the list. If so -- error, otherwise -- add
            if (InTypesL(PBType, Types))
            {
              throw mcrl2::runtime_error("double declaration of propositional variable " + core::pp(PBName));
            }
            else
            {
              Types=Types+make_list(PBType);
            }
          }
          PBs[PBName]=Types;

          //This is a fake ProcVarId (There is no PBVarId)

          pbes_bodies[PBEqn.variable()]=PBEqn.formula();
        }
        pbes_bodies[INIT_PBES()]=PBInit;
      }


      pbes_expression TraversePBESVarConstPB(const std::map<core::identifier_string,data::sort_expression> &Vars, const pbes_expression &PBESTerm)
      {
        using namespace data;
        using namespace atermpp;
        if (is_data_expression(PBESTerm))
        {
          data_expression d(PBESTerm);
          TraverseVarConsTypeD(Vars,Vars,d,sort_bool::bool_());
          return d;
        }

        if (is_pbes_not(PBESTerm))
        {
          const not_& argument=down_cast<const not_>(PBESTerm);
          return not_(TraversePBESVarConstPB(Vars,argument.operand()));
        }

        if (is_pbes_and(PBESTerm))
        {
          const and_& t=down_cast<const and_>(PBESTerm);
          return and_(TraversePBESVarConstPB(Vars,t.left()),TraversePBESVarConstPB(Vars,t.right()));
        }

        if (is_pbes_or(PBESTerm))
        {
          const or_& t=down_cast<const or_>(PBESTerm);
          return or_(TraversePBESVarConstPB(Vars,t.left()),TraversePBESVarConstPB(Vars,t.right()));
        }

        if (is_pbes_imp(PBESTerm))
        {
          const imp& t=down_cast<const imp>(PBESTerm);
          return imp(TraversePBESVarConstPB(Vars,t.left()),TraversePBESVarConstPB(Vars,t.right()));
        }

        if (is_pbes_forall(PBESTerm))
        {
          const forall& t=down_cast<const forall>(PBESTerm);
          std::map<core::identifier_string,sort_expression> CopyVars(Vars);

          std::map<core::identifier_string,sort_expression> NewVars;
          try
          {
            AddVars2Table(CopyVars,t.variables(),NewVars);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pbes_system::pp(PBESTerm));
          }
          try
          {
            return forall(t.variables(),TraversePBESVarConstPB(NewVars,t.body()));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pbes_system::pp(PBESTerm));
          }
        }

        if (is_pbes_exists(PBESTerm))
        {
          const exists& t=down_cast<const exists>(PBESTerm);
          std::map<core::identifier_string,sort_expression> CopyVars(Vars);

          std::map<core::identifier_string,sort_expression> NewVars;
          try
          {
            AddVars2Table(CopyVars,t.variables(),NewVars);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error while typechecking " + pbes_system::pp(PBESTerm));
          }
          try
          {
            return exists(t.variables(),TraversePBESVarConstPB(NewVars,t.body()));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pbes_system::pp(PBESTerm));
          }
        }

        if (is_propositional_variable_instantiation(PBESTerm))
        {
          const propositional_variable_instantiation& var=down_cast<const propositional_variable_instantiation>(PBESTerm);
          return RewrPbes(Vars, var);
        }
        throw mcrl2::runtime_error("Internal error. The pbes term " + pbes_system::pp(PBESTerm) + " fails to match any known form in typechecking case analysis");
      }


      propositional_variable_instantiation RewrPbes(
              const std::map<core::identifier_string,data::sort_expression> &Vars,
              const propositional_variable_instantiation& ProcTerm)
      {
        using namespace data;
        using namespace atermpp;

        core::identifier_string Name=ProcTerm.name();


        const std::map<core::identifier_string,term_list<sort_expression_list> >::const_iterator j=PBs.find(Name);
        if (j==PBs.end())
        {
          throw mcrl2::runtime_error("propositional variable " + core::pp(Name) + " not declared");
        }
        term_list<sort_expression_list> ParList=j->second;

        assert(!ParList.empty());

        size_t nFactPars=ProcTerm.parameters().size();

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
          throw mcrl2::runtime_error("no propositional variable " + core::pp(Name)
                          + " with " + mcrl2::utilities::to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                          + " is declared (while typechecking " + pbes_system::pp(ProcTerm) + ")");
        }

        sort_expression_list Result;
        if (ParList.size()==1)
        {
          Result=ParList.front();
        }
        else
        {
          // we need typechecking to find the correct type of the action.
          // make the list of possible types for the parameters
          Result=GetNotInferredList(ParList);
        }

        //process the arguments

        //possible types for the arguments of the action. (not inferred if ambiguous action).
        sort_expression_list PosTypeList=Result;

        data_expression_list NewPars;
        sort_expression_list NewPosTypeList;
        for (data_expression_list Pars=ProcTerm.parameters(); !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
        {
          data_expression Par=Pars.front();
          sort_expression PosType=PosTypeList.front();

          sort_expression NewPosType;
          try
          {
            NewPosType=TraverseVarConsTypeD(Vars,Vars,Par,PosType); //gstcExpandNumTypesDown(PosType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(Par) + " as type " + data::pp(ExpandNumTypesDown(PosType)) + " (while typechecking " + pbes_system::pp(ProcTerm) + ")");
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
          PosTypeList=Result;
          data_expression_list Pars=NewPars;
          NewPars=data_expression_list();
          sort_expression_list CastedPosTypeList;
          for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
          {
            data_expression Par=Pars.front();
            sort_expression PosType=PosTypeList.front();
            sort_expression NewPosType=NewPosTypeList.front();

            sort_expression CastedNewPosType;
            try
            {
              std::map<core::identifier_string,sort_expression> dummy_table;
              CastedNewPosType=UpCastNumericType(PosType,NewPosType,Par,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + data::pp(NewPosType) + " to " + data::pp(PosType) + "(while typechecking " + data::pp(Par) + " in " + pbes_system::pp(ProcTerm));
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
            throw mcrl2::runtime_error("no propositional variable " + core::pp(Name) + "with type " + data::pp(NewPosTypeList) + " is declared (while typechecking " + pbes_system::pp(ProcTerm) + ")");
          }
        }

        if (IsNotInferredL(PosTypeList))
        {
          throw mcrl2::runtime_error("ambiguous propositional variable " + core::pp(Name));
        }

        return propositional_variable_instantiation(Name,NewPars);
      }
};



/** \brief     Type check a parsed mCRL2 pbes specification.
 *  Throws an exception if something went wrong.
 *  \param[in] pbes_spec A process specification  that has not been type checked.
 *  \post      pbes_spec is type checked.
 **/

inline
void type_check(pbes& pbes_spec)
{
  pbes_type_checker type_checker(pbes_spec);
  try
  {
    pbes_spec=type_checker(); // Get the type checked specification back.
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pbes_system::pp(pbes_spec));
  }
}

/** \brief     Type check a parsed mCRL2 propositional variable.
 *  Throws an exception if something went wrong.
 *  \param[in] dataspec A data specification.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \return    the type checked expression
 **/
template <typename VariableContainer>
propositional_variable type_check(const propositional_variable& x, const VariableContainer& variables, const data::data_specification& dataspec = data::data_specification())
{
  // This function should be implemented using the PBES type checker, but it is not immediately clear how to do that.
  try
  {
    const data::variable_list& parameters = x.parameters();
    std::vector<data::variable> typed_parameters;
    for (auto i = parameters.begin(); i != parameters.end(); ++i)
    {
      data::variable d = *i;
      data::type_check(d, variables.begin(), variables.end(), dataspec);
      typed_parameters.push_back(d);
    }
    return propositional_variable(x.name(), data::variable_list(typed_parameters.begin(), typed_parameters.end()));
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pbes_system::pp(x));
  }
}

/** \brief     Type check a parsed mCRL2 pbes expression.
 *  Throws an exception if something went wrong.
 *  \param[in] dataspec A data specification.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] propositional_variables A sequence of propositional variables that may appear in x.
 *  \return    the type checked expression
 **/
template <typename VariableContainer, typename PropositionalVariableContainer>
pbes_expression type_check(pbes_expression& x, const VariableContainer& variables, const PropositionalVariableContainer& propositional_variables, const data::data_specification& dataspec = data::data_specification())
{
  try
  {
    pbes_type_checker type_checker(dataspec, variables, propositional_variables);
    return type_checker(x);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pbes_system::pp(x));
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TYPECHECK_H
