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
#include "mcrl2/pbes/detail/pbes_context.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct typecheck_builder: public pbes_expression_builder<typecheck_builder>
{
  typedef pbes_expression_builder<typecheck_builder> super;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const detail::pbes_context& m_pbes_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::pbes_context& pbes_context
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variables),
    m_pbes_context(pbes_context)
  { }

  pbes_expression apply(const data::data_expression& x)
  {
    return m_data_type_checker.typecheck_data_expression(x, data::sort_bool::bool_(), m_variable_context);
  }

  pbes_expression apply(const forall& x)
  {
    try
    {
      data::detail::check_duplicate_variable_names(x.variables(), "quantifier variable");
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      pbes_expression body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
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
      data::detail::check_duplicate_variable_names(x.variables(), "quantifier variable");
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      pbes_expression body = (*this).apply(x.body());
      m_variable_context = m_variable_context_copy;
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
    if (!m_pbes_context.is_declared(name))
    {
      throw mcrl2::runtime_error("propositional variable " + core::pp(name) + " not declared");
    }

    data::sort_expression_list equation_sorts = m_pbes_context.propositional_variable_sorts(name);
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
        *xi = m_data_type_checker.typecheck_data_expression(*xi, *ei, m_variable_context);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(*xi) + " as type " + data::pp(*ei) + " (while typechecking " + pbes_system::pp(x) + ")");
      }
    }
    return propositional_variable_instantiation(name, data::data_expression_list(x_parameters.begin(), x_parameters.end()));
  }

  pbes_expression apply(const data::untyped_data_parameter& x)
  {
    const core::identifier_string& name = x.name();
    if (!m_pbes_context.is_declared(name))
    {
      return data::typecheck_untyped_data_parameter(m_data_type_checker, x.name(), x.arguments(), data::sort_bool::bool_(), m_variable_context);
    }

    data::sort_expression_list equation_sorts = m_pbes_context.propositional_variable_sorts(name);
    std::vector<data::data_expression> x_parameters(x.arguments().begin(), x.arguments().end());

    if (x_parameters.size() != equation_sorts.size())
    {
      throw mcrl2::runtime_error("propositional variable " + data::pp(x) + " has the wrong number of parameters");
    }

    auto ei = equation_sorts.begin();
    auto xi = x_parameters.begin();
    for (; ei != equation_sorts.end(); ++ei, ++xi)
    {
      try
      {
        *xi = m_data_type_checker.typecheck_data_expression(*xi, *ei, m_variable_context);
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(*xi) + " as type " + data::pp(*ei) + " (while typechecking " + data::pp(x) + ")");
      }
    }
    return propositional_variable_instantiation(name, data::data_expression_list(x_parameters.begin(), x_parameters.end()));
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::pbes_context& propositional_variables
                   )
{
  return typecheck_builder(data_typechecker, variables, propositional_variables);
}

} // namespace detail

class pbes_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    data::detail::variable_context m_variable_context;
    detail::pbes_context m_pbes_context;

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
      m_variable_context.add_context_variables(global_variables, m_data_type_checker);
      m_pbes_context.add_propositional_variables(propositional_variables, m_data_type_checker);
    }

    /// \brief Typecheck the pbes pbesspec
    void operator()(pbes& pbesspec)
    {
      mCRL2log(log::verbose) << "type checking PBES specification..." << std::endl;

      pbes_system::normalize_sorts(pbesspec, m_data_type_checker.typechecked_data_specification());

      // reset the context
      m_data_type_checker = data::data_type_checker(pbesspec.data());
      m_variable_context.clear();
      m_pbes_context.clear();
      m_variable_context.add_context_variables(pbesspec.global_variables(), m_data_type_checker);
      m_pbes_context.add_propositional_variables(equation_variables(pbesspec.equations()), m_data_type_checker);

      // typecheck the equations
      for (pbes_equation& eqn: pbesspec.equations())
      {
        data::detail::variable_context variable_context = m_variable_context;
        try
        {
          data::detail::check_duplicate_variable_names(eqn.variable().parameters(), "propositional variable parameter");
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + " while typechecking " + pbes_system::pp(eqn.variable()));
        }
        variable_context.add_context_variables(eqn.variable().parameters(), m_data_type_checker);
        eqn.formula() = detail::make_typecheck_builder(m_data_type_checker, variable_context, m_pbes_context).apply(eqn.formula());
      }

      // typecheck the initial state
      pbesspec.initial_state() = atermpp::down_cast<propositional_variable_instantiation>(detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_pbes_context).apply(pbesspec.initial_state()));

      // typecheck the data specification
      pbesspec.data() = m_data_type_checker.typechecked_data_specification();
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] x A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pbes_expression operator()(const pbes_expression& x)
    {
      return detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_pbes_context).apply(pbes_system::normalize_sorts(x, m_data_type_checker.typechecked_data_specification()));
    }

    protected:
      pbes_expression typecheck(const pbes_expression& x, const data::variable_list& parameters)
      {
        data::detail::variable_context variable_context = m_variable_context;
        variable_context.add_context_variables(parameters, m_data_type_checker);
        return detail::make_typecheck_builder(m_data_type_checker, variable_context, m_pbes_context).apply(x);
      }
};

/** \brief     Type check a parsed mCRL2 pbes specification.
 *  Throws an exception if something went wrong.
 *  \param[in] pbesspec A process specification  that has not been type checked.
 *  \post      pbesspec is type checked.
 **/

inline
void type_check_pbes(pbes& pbesspec)
{
  pbes_type_checker type_checker;
  try
  {
    type_checker(pbesspec);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nCould not type check " + pbes_system::pp(pbesspec));
  }
}

/** \brief     Type check a parsed mCRL2 propositional variable.
 *  Throws an exception if something went wrong.
 *  \param[in] x A propositional variable.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] dataspec A data specification.
 *  \return    the type checked expression
 **/
template <typename VariableContainer>
propositional_variable type_check_propositional_variable(const propositional_variable& x,
                                                         const VariableContainer& variables,
                                                         const data::data_specification& dataspec = data::data_specification()
                                                        )
{
  // This function should be implemented using the PBES type checker, but it is not immediately clear how to do that.
  try
  {
    const data::variable_list& parameters = x.parameters();
    std::vector<data::variable> typed_parameters;
    for (const data::variable& parameter: parameters)
    {
      data::variable d = atermpp::down_cast<data::variable>(data::type_check_data_expression(parameter, variables, dataspec));
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
 *  \param[in] x A pbes expression.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] propositional_variables A sequence of propositional variables that may appear in x.
 *  \param[in] dataspec A data specification.
 *  \return    the type checked expression
 **/
template <typename VariableContainer, typename PropositionalVariableContainer>
pbes_expression type_check_pbes_expression(pbes_expression& x,
                                           const VariableContainer& variables,
                                           const PropositionalVariableContainer& propositional_variables,
                                           const data::data_specification& dataspec = data::data_specification()
                                          )
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
