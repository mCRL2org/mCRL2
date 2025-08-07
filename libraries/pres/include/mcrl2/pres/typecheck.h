// Author(s): Jan Friso Groote. Based on pbes/typecheck.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TYPECHECK_H
#define MCRL2_PRES_TYPECHECK_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pres/detail/pres_context.h"
#include "mcrl2/pres/normalize_sorts.h"



namespace mcrl2::pres_system
{

namespace detail
{

struct typecheck_builder: public pres_expression_builder<typecheck_builder>
{
  using super = pres_expression_builder<typecheck_builder>;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const detail::pres_context& m_pres_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::pres_context& pres_context
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variables),
    m_pres_context(pres_context)
  { }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    try 
    {
      result = m_data_type_checker.typecheck_data_expression(x, data::bool_(), m_variable_context);
    }
    catch (mcrl2::runtime_error& )
    {
      try 
      {
         result = m_data_type_checker.typecheck_data_expression(x, data::sort_real::real_(), m_variable_context);
      }
      catch  (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string("Fail to cast data_expression to type bool or real.\n") + e.what());
      }
    }
  }

  template <class T>
  void apply(T& result, const infimum& x)
  {
    try
    {
      data::detail::check_duplicate_variable_names(x.variables(), "quantifier variable");
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      pres_expression body;
      (*this).apply(body, x.body());
      m_variable_context = m_variable_context_copy;
      result = infimum(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pres_system::pp(x));
    }
  }

  template <class T>
  void apply(T& result, const supremum& x)
  {
    try
    {
      data::detail::check_duplicate_variable_names(x.variables(), "quantifier variable");
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      pres_expression body;
      (*this).apply(body, x.body());
      m_variable_context = m_variable_context_copy;
      result = supremum(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pres_system::pp(x));
    }
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    try
    {
      data::detail::check_duplicate_variable_names(x.variables(), "sum variable");
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      pres_expression body;
      (*this).apply(body, x.body());
      m_variable_context = m_variable_context_copy;
      result = supremum(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pres_system::pp(x));
    }
  }

  template <class T>
  void apply(T& result, const const_multiply& x)
  {
    try
    {
      data::data_expression factor = m_data_type_checker.typecheck_data_expression(x.left(), data::sort_real::real_(), m_variable_context);
      this->apply(result, x.right());
      make_const_multiply(result, factor, result);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pres_system::pp(x));
    }
  }

  template <class T>
  void apply(T& result, const const_multiply_alt& x)
  {
    try
    {
      data::data_expression factor = m_data_type_checker.typecheck_data_expression(x.right(), data::sort_real::real_(), m_variable_context);
      this->apply(result, x.left());
      make_const_multiply_alt(result, result, factor);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + pres_system::pp(x));
    }
  }


  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    const core::identifier_string& name = x.name();
    if (!m_pres_context.is_declared(name))
    {
      throw mcrl2::runtime_error("propositional variable " + core::pp(name) + " not declared");
    }

    data::sort_expression_list equation_sorts = m_pres_context.propositional_variable_sorts(name);
    std::vector<data::data_expression> x_parameters(x.parameters().begin(), x.parameters().end());

    if (x_parameters.size() != equation_sorts.size())
    {
      throw mcrl2::runtime_error("propositional variable " + pres_system::pp(x) + " has the wrong number of parameters");
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
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(*xi) + 
                                        " as type " + data::pp(*ei) + " (while typechecking " + pres_system::pp(x) + ")");
      }
    }
    make_propositional_variable_instantiation(result, name, data::data_expression_list(x_parameters.begin(), x_parameters.end()));
  }

  template <class T>
  void apply(T& result, const data::untyped_data_parameter& x)
  {
    const core::identifier_string& name = x.name();
    if (!m_pres_context.is_declared(name))
    {
      result = data::typecheck_untyped_data_parameter(m_data_type_checker, x.name(), x.arguments(), data::bool_(), m_variable_context);
      return;
    }

    data::sort_expression_list equation_sorts = m_pres_context.propositional_variable_sorts(name);
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
    make_propositional_variable_instantiation(result, name, data::data_expression_list(x_parameters.begin(), x_parameters.end()));
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::pres_context& propositional_variables
                   )
{
  return typecheck_builder(data_typechecker, variables, propositional_variables);
}

} // namespace detail

class pres_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    data::detail::variable_context m_variable_context;
    detail::pres_context m_pres_context;

    std::vector<propositional_variable> equation_variables(const std::vector<pres_equation>& equations)
    {
      std::vector<propositional_variable> result;
      for (const pres_equation& eqn: equations)
      {
        result.push_back(eqn.variable());
      }
      return result;
    }

  public:
    /// \brief Default constructor
    pres_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_type_checker(dataspec)
    {}

    /// \brief Constructor
    template <typename VariableContainer, typename PropositionalVariableContainer>
    pres_type_checker(const data::data_specification& dataspec, const VariableContainer& global_variables, const PropositionalVariableContainer& propositional_variables)
      : m_data_type_checker(dataspec)
    {
      m_variable_context.add_context_variables(global_variables, m_data_type_checker);
      m_pres_context.add_propositional_variables(propositional_variables, m_data_type_checker);
    }

    /// \brief Typecheck the pres presspec
    void operator()(pres& presspec)
    {
      mCRL2log(log::verbose) << "type checking PRES specification..." << std::endl;

      pres_system::normalize_sorts(presspec, m_data_type_checker.typechecked_data_specification());

      // reset the context
      m_data_type_checker = data::data_type_checker(presspec.data());
      m_variable_context.clear();
      m_pres_context.clear();
      m_variable_context.add_context_variables(presspec.global_variables(), m_data_type_checker);
      m_pres_context.add_propositional_variables(equation_variables(presspec.equations()), m_data_type_checker);

      // typecheck the equations
      for (pres_equation& eqn: presspec.equations())
      {
        data::detail::variable_context variable_context = m_variable_context;
        try
        {
          data::detail::check_duplicate_variable_names(eqn.variable().parameters(), "propositional variable parameter");
        }
        catch (mcrl2::runtime_error& e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + " while typechecking " + pres_system::pp(eqn.variable()));
        }
        variable_context.add_context_variables(eqn.variable().parameters(), m_data_type_checker);
        pres_expression formula;
        detail::make_typecheck_builder(m_data_type_checker, variable_context, m_pres_context).apply(formula, eqn.formula());
        eqn.formula() = formula;
      }

      // typecheck the initial state
      propositional_variable_instantiation initial_state;
      detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_pres_context).apply(initial_state, presspec.initial_state());
      presspec.initial_state() = initial_state;

      // typecheck the data specification
      data::data_specification d = m_data_type_checker.typechecked_data_specification();
      d.translate_user_notation();
      presspec.set_data(d);
    }

     /** \brief     Type check a process expression.
      * Throws a mcrl2::runtime_error exception if the expression is not well typed.
      *  \param[in] x A process expression that has not been type checked.
      *  \return    a process expression where all untyped identifiers have been replace by typed ones.
      **/
    pres_expression operator()(const pres_expression& x)
    {
      pres_expression result;
      detail::make_typecheck_builder(m_data_type_checker, m_variable_context, m_pres_context).
             apply(result,pres_system::normalize_sorts(x, m_data_type_checker.typechecked_data_specification()));
      return result;
    }

    protected:
      pres_expression typecheck(const pres_expression& x, const data::variable_list& parameters)
      {
        data::detail::variable_context variable_context = m_variable_context;
        variable_context.add_context_variables(parameters, m_data_type_checker);
        pres_expression result;
        detail::make_typecheck_builder(m_data_type_checker, variable_context, m_pres_context).apply(result, x);
        return result;
      }
};

/** \brief     Type check a parsed mCRL2 pres specification.
 *  Throws an exception if something went wrong.
 *  \param[in] presspec A process specification  that has not been type checked.
 *  \post      presspec is type checked.
 **/

inline
void typecheck_pres(pres& presspec)
{
  pres_type_checker type_checker;
  try
  {
    type_checker(presspec);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\nCould not type check " + pres_system::pp(presspec));
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
propositional_variable typecheck_propositional_variable(const propositional_variable& x,
                                                        const VariableContainer& variables,
                                                        const data::data_specification& dataspec = data::data_specification()
                                                       )
{
  // This function should be implemented using the PRES type checker, but it is not immediately clear how to do that.
  try
  {
    const data::variable_list& parameters = x.parameters();
    std::vector<data::variable> typed_parameters;
    for (const data::variable& parameter: parameters)
    {
      data::variable d = atermpp::down_cast<data::variable>(data::typecheck_data_expression(parameter, variables, dataspec));
      typed_parameters.push_back(d);
    }
    return propositional_variable(x.name(), data::variable_list(typed_parameters.begin(), typed_parameters.end()));
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pres_system::pp(x));
  }
}

/** \brief     Type check a parsed mCRL2 pres expression.
 *  Throws an exception if something went wrong.
 *  \param[in] x A pres expression.
 *  \param[in] variables A sequence of data variables that may appear in x.
 *  \param[in] propositional_variables A sequence of propositional variables that may appear in x.
 *  \param[in] dataspec A data specification.
 *  \return    the type checked expression
 **/
template <typename VariableContainer, typename PropositionalVariableContainer>
pres_expression typecheck_pres_expression(pres_expression& x,
                                          const VariableContainer& variables,
                                          const PropositionalVariableContainer& propositional_variables,
                                          const data::data_specification& dataspec = data::data_specification()
                                         )
{
  try
  {
    pres_type_checker type_checker(dataspec, variables, propositional_variables);
    return type_checker(x);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pres_system::pp(x));
  }
}

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_TYPECHECK_H
