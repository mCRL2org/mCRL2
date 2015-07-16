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
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/normalize_sorts.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct data_expression_typechecker: protected data::data_type_checker
{
  /** \brief     make a data type checker.
   *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
   *  \param[in] data_spec A data specification that does not need to have been type checked.
   *  \return    a data expression where all untyped identifiers have been replace by typed ones.
   **/
  data_expression_typechecker(const data::data_specification& dataspec)
    : data::data_type_checker(dataspec)
  {}

  void check_sort_list_is_declared(const data::sort_expression_list& x)
  {
    return sort_type_checker::check_sort_list_is_declared(x);
  }

  void check_sort_is_declared(const data::sort_expression& x)
  {
    return data::data_type_checker::check_sort_is_declared(x);
  }

  /** \brief     Type check a data expression.
   *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
   *  \param[in] x A data expression that has not been type checked.
   *  \param[in] expected_sort The expected sort of the data expression.
   *  \param[in] variable_constext a mapping of variable names to their types.
   *  \return the type checked data expression.
   **/
  data::data_expression operator()(const data::data_expression& x,
                                   const data::sort_expression& expected_sort,
                                   const std::map<core::identifier_string, data::sort_expression>& variable_context
                                  )
  {
    data::data_expression x1 = x;
    TraverseVarConsTypeD(variable_context, variable_context, x1, expected_sort);
    return x1;
  }

  data::data_specification typechecked_data_specification()
  {
    return type_checked_data_spec;
  }
};

struct typecheck_builder: public pbes_expression_builder<typecheck_builder>
{
  typedef pbes_expression_builder<typecheck_builder> super;
  using super::apply;

  data_expression_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::map<core::identifier_string, data::sort_expression_list>& m_equation_sorts;

  typecheck_builder(data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_equation_sorts(equation_sorts)
  {}

  pbes_expression apply(const data::data_expression& x)
  {
    return m_data_typechecker(x, data::sort_bool::bool_(), m_variables);
  }

  pbes_expression apply(const forall& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      pbes_expression body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return forall(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pbes_system::pp(x));
    }
  }

  pbes_expression apply(const exists& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      pbes_expression body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return exists(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pbes_system::pp(x));
    }
  }

  pbes_expression apply(const propositional_variable_instantiation& x)
  {
    const core::identifier_string& name = x.name();
    auto i = m_equation_sorts.find(name);
    if (i == m_equation_sorts.end())
    {
      throw mcrl2::runtime_error("propositional variable " + core::pp(name) + " not declared");
    }

    const data::sort_expression_list& equation_sorts = i->second;
    std::vector<data::data_expression> x_parameters(x.parameters().begin(), x.parameters().end());

    if (x_parameters.size() != equation_sorts.size())
    {
      throw mcrl2::runtime_error("propositional variable " + pbes_system::pp(x) + " has the wrong number of parameters");
    }

    auto ei = equation_sorts.begin();
    auto xi = x_parameters.begin();
    for (; ei != equation_sorts.end(); ++ei, ++xi)
    {
      try
      {
        *xi = m_data_typechecker(*xi, *ei, m_variables);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(*xi) + " as type " + data::pp(*ei) + " (while typechecking " + pbes_system::pp(x) + ")");
      }
    }
    return propositional_variable_instantiation(name, data::data_expression_list(x_parameters.begin(), x_parameters.end()));
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts
                   )
{
  return typecheck_builder(data_typechecker, variables, equation_sorts);
}

} // namespace detail

class pbes_type_checker
{
  protected:
    detail::data_expression_typechecker m_data_type_checker;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;
    std::map<core::identifier_string, data::sort_expression_list> m_equation_sorts;

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

    data::sort_expression_list variable_sorts(const data::variable_list& variables)
    {
      data::sort_expression_list sorts;
      for (const data::variable& v: variables)
      {
        sorts.push_front(v.sort());
      }
      return atermpp::reverse(sorts);
    }

    std::vector<propositional_variable> equation_variables(const std::vector<pbes_equation>& equations)
    {
      std::vector<propositional_variable> result;
      for (const pbes_equation& eqn: equations)
      {
        result.push_back(eqn.variable());
      }
      return result;
    }

  public:
    /// \brief Default constructor
    pbes_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_type_checker(dataspec)
    {}

    /// \brief Constructor
    template <typename VariableContainer, typename PropositionalVariableContainer>
    pbes_type_checker(const data::data_specification& dataspec, const VariableContainer& global_variables, const PropositionalVariableContainer& propositional_variables)
      : m_data_type_checker(dataspec)
    {
      add_global_variables(global_variables);
      add_propositional_variables(propositional_variables);
    }

    /// \brief Typecheck the pbes pbesspec
    void operator()(pbes& pbesspec)
    {
      mCRL2log(log::verbose) << "type checking PBES specification..." << std::endl;

      // reset the context
      m_data_type_checker = detail::data_expression_typechecker(pbesspec.data());
      m_global_variables.clear();
      m_equation_sorts.clear();
      add_global_variables(pbesspec.global_variables());
      add_propositional_variables(equation_variables(pbesspec.equations()));

      // typecheck the equations
      for (pbes_equation& eqn: pbesspec.equations())
      {
        eqn.formula() = detail::make_typecheck_builder(m_data_type_checker, declared_variables(eqn.variable().parameters()), m_equation_sorts).apply(eqn.formula());
      }

      // typecheck the initial state
      pbesspec.initial_state() = atermpp::down_cast<propositional_variable_instantiation>(detail::make_typecheck_builder(m_data_type_checker, m_global_variables, m_equation_sorts).apply(pbesspec.initial_state()));

      // typecheck the data specification
      pbesspec.data() = m_data_type_checker.typechecked_data_specification();
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] d A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes_expression operator()(const pbes_expression& x)
    {
      return detail::make_typecheck_builder(m_data_type_checker, m_global_variables, m_equation_sorts).apply(x);
    }

    protected:
      pbes_expression typecheck(const pbes_expression& x, const data::variable_list& parameters)
      {
        return detail::make_typecheck_builder(m_data_type_checker, declared_variables(parameters), m_equation_sorts).apply(x);
      }

      template <typename VariableContainer>
      void add_global_variables(const VariableContainer& global_variables)
      {
        for (const data::variable& v: global_variables)
        {
          m_data_type_checker.check_sort_is_declared(v.sort());
          auto i = m_global_variables.find(v.name());
          if (i == m_global_variables.end())
          {
            m_global_variables[v.name()] = v.sort();
          }
          else
          {
            throw mcrl2::runtime_error("attempt to overload global variable " + core::pp(v.name()));
          }
        }
      }

      template <typename PropositionalVariableContainer>
      void add_propositional_variables(const PropositionalVariableContainer& propositional_variables)
      {
        for (const propositional_variable& v: propositional_variables)
        {
          data::sort_expression_list sorts = variable_sorts(v.parameters());
          m_data_type_checker.check_sort_list_is_declared(sorts);

          auto i = m_equation_sorts.find(v.name());
          if (i == m_equation_sorts.end())
          {
            m_equation_sorts[v.name()] = sorts;
          }
          else
          {
            throw mcrl2::runtime_error("attempt to overload propositional variable " + core::pp(v.name()));
          }
        }
      }
};

/** \brief     Type check a parsed mCRL2 pbes specification.
 *  Throws an exception if something went wrong.
 *  \param[in] pbesspec A process specification  that has not been type checked.
 *  \post      pbesspec is type checked.
 **/

inline
void type_check(pbes& pbesspec)
{
  pbes_type_checker type_checker;
  try
  {
    type_checker(pbesspec);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pbes_system::pp(pbesspec));
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
