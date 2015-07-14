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

class pbes_type_checker: public data::data_type_checker
{
  protected:
    pbes m_type_checked_pbes_spec;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;
    std::map<core::identifier_string, atermpp::term_list<data::sort_expression_list> > m_equation_sorts;

    // Returns m_global_variables with variables inserted into it
    std::map<core::identifier_string, data::sort_expression> declared_variables(const data::variable_list& variables)
    {
      std::map<core::identifier_string, data::sort_expression> result = m_global_variables;
      for (const data::variable& v: variables)
      {
        result[v.name()] = v.sort();
      }
      return result;
    }

  public:
    pbes_type_checker(const pbes& pbes_spec)
      : data::data_type_checker(pbes_spec.data())
    {
      mCRL2log(log::verbose) << "type checking PBES specification..." << std::endl;

      ReadInPBESAndInit(pbes_spec.global_variables(), pbes_spec.equations(), pbes_spec.initial_state());

      mCRL2log(log::debug) << "type checking PBES read-in phase finished" << std::endl;
      mCRL2log(log::debug) << "type checking transform Data+PBES phase started" << std::endl;

      std::vector<pbes_equation> equations = pbes_spec.equations();
      auto const& globvars = pbes_spec.global_variables();
      m_global_variables = declared_variables(data::variable_list(globvars.begin(), globvars.end()));

      for (pbes_equation& eqn: equations)
      {
        eqn.formula() = TraversePBESVarConstPB(declared_variables(eqn.variable().parameters()), eqn.formula());
      }
      pbes_expression initial_state = TraversePBESVarConstPB(m_global_variables, pbes_spec.initial_state());

      mCRL2log(log::debug) << "type checking transform Data+PBES phase finished" << std::endl;

      m_type_checked_pbes_spec = pbes(type_checked_data_spec, equations, pbes_spec.global_variables(), atermpp::down_cast<propositional_variable_instantiation>(initial_state));

      normalize_sorts(m_type_checked_pbes_spec, type_checked_data_spec);
    }


    template <typename VariableContainer, typename PropositionalVariableContainer>
    pbes_type_checker(const data::data_specification& dataspec, const VariableContainer& variables, const PropositionalVariableContainer& propositional_variables)
    : data::data_type_checker(dataspec)
    {
      for (auto i = variables.begin(); i != variables.end(); ++i)
      {
        m_global_variables[i->name()] = i->sort();
      }
      for (auto i = propositional_variables.begin(); i != propositional_variables.end(); ++i)
      {
        const data::variable_list& parameters = i->parameters();
        data::sort_expression_list sorts;
        for (auto l = parameters.begin(); l != parameters.end(); ++l)
        {
          sorts.push_front(l->sort());
        }
        sorts = atermpp::reverse(sorts);
        check_sort_list_is_declared(sorts);
        m_equation_sorts[i->name()] = atermpp::term_list<data::sort_expression_list>({ sorts });
      }
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] d A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes operator()()
    {
      return m_type_checked_pbes_spec;
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] d A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes_expression operator()(const pbes_expression& x)
    {
      return TraversePBESVarConstPB(m_global_variables, x);
    }

    protected:
      pbes_expression typecheck(const pbes_expression& x, const data::variable_list& parameters)
      {

        std::map<core::identifier_string, data::sort_expression> variables = m_global_variables;
        AddVars2Table(variables, parameters);
        return TraversePBESVarConstPB(variables, x);
      }

      void ReadInPBESAndInit(const std::set<data::variable>& global_variables, const std::vector<pbes_equation>& equations, const propositional_variable_instantiation& initial_state)
      {
        for (const data::variable& v: global_variables)
        {
          sort_type_checker::check_sort_is_declared(v.sort());
        }

        for (const pbes_equation& eqn: equations)
        {
          core::identifier_string name = eqn.variable().name();
          const data::variable_list& parameters = eqn.variable().parameters();

          data::sort_expression_list sorts;
          for (const data::variable& v: parameters)
          {
            sorts.push_front(v.sort());
          }
          sorts = atermpp::reverse(sorts);
          check_sort_list_is_declared(sorts);

          auto i = m_equation_sorts.find(name);
          atermpp::term_list<data::sort_expression_list> Types;
          if (i == m_equation_sorts.end())
          {
            Types = atermpp::term_list<data::sort_expression_list>({ sorts });
          }
          else
          {
            Types = i->second;
            // temporarily prohibit overloading here
            throw mcrl2::runtime_error("attempt to overload propositional variable " + core::pp(name));

            // the table m_equation_sorts contains a list of types for each
            // PBES name. We need to check if there is already such a type
            // in the list. If so -- error, otherwise -- add
            if (InTypesL(sorts, Types))
            {
              throw mcrl2::runtime_error("double declaration of propositional variable " + core::pp(name));
            }
            else
            {
              Types = Types + atermpp::term_list<data::sort_expression_list>({ sorts });
            }
          }
          m_equation_sorts[name] = Types;
        }
      }


      pbes_expression TraversePBESVarConstPB(const std::map<core::identifier_string,data::sort_expression>& Vars, const pbes_expression &PBESTerm)
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
            AddVars2Table(CopyVars,t.variables());
            NewVars = CopyVars;
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
            AddVars2Table(CopyVars,t.variables());
            NewVars = CopyVars;
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
              const std::map<core::identifier_string,data::sort_expression>& Vars,
              const propositional_variable_instantiation& ProcTerm)
      {
        using namespace data;
        using namespace atermpp;

        core::identifier_string Name=ProcTerm.name();


        auto j = m_equation_sorts.find(Name);
        if (j == m_equation_sorts.end())
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
            data::sort_expression_list Par=ParList.front();
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

        data::sort_expression_list Result;
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
        data::sort_expression_list PosTypeList=Result;

        data::data_expression_list NewPars;
        data::sort_expression_list NewPosTypeList;
        for (data_expression_list Pars=ProcTerm.parameters(); !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
        {
          data::data_expression Par=Pars.front();
          data::sort_expression PosType=PosTypeList.front();

          data::sort_expression NewPosType;
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
          data::data_expression_list Pars = NewPars;
          NewPars = data::data_expression_list();
          data::sort_expression_list CastedPosTypeList;
          for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
          {
            data::data_expression Par=Pars.front();
            data::sort_expression PosType=PosTypeList.front();
            data::sort_expression NewPosType=NewPosTypeList.front();

            data::sort_expression CastedNewPosType;
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
          NewPars = atermpp::reverse(NewPars);
          NewPosTypeList = atermpp::reverse(CastedPosTypeList);

          std::pair<bool, data::sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
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
