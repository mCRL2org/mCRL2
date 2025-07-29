// Author(s): Jan Friso Groote. Based on pbesinst_algorithm.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/presinst_algorithm.h
/// \brief Algorithm for instantiating a PRES.

#ifndef MCRL2_PRES_PRESINST_ALGORITHM_H
#define MCRL2_PRES_PRESINST_ALGORITHM_H

#include "mcrl2/pres/detail/res_equation_limit.h"
#include "mcrl2/pres/detail/instantiate_global_variables.h"
#include "mcrl2/pres/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pres/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pres/rewriters/simplify_quantifiers_rewriter.h"

namespace mcrl2::pres_system
{

/// \brief Creates a substitution function for the presinst rewriter.
/// \param v A sequence of data variables
/// \param e A sequence of data expressions
/// \param sigma The substitution that maps the i-th element of \p v to the i-th element of \p e
inline
void make_presinst_substitution(const data::variable_list& v, const data::data_expression_list& e, data::rewriter::substitution_type& sigma)
{
  assert(v.size() == e.size());
  data::variable_list::iterator i = v.begin();
  data::data_expression_list::iterator j = e.begin();
  for (; i != v.end(); ++i, ++j)
  {
    sigma[*i] = *j;
  }
}

inline
bool presinst_is_constant(const pres_expression& x)
{
  return pres_system::find_free_variables(x).empty();
}

/// \brief Creates a unique name for a propositional variable instantiation. The
/// propositional variable instantiation must be closed.
/// Originally implemented by Alexander van Dam.
/// \return A name that uniquely corresponds to the propositional variable.
struct presinst_rename_long
{
  core::identifier_string operator()(const propositional_variable_instantiation& Ye) const
  {
    assert(presinst_is_constant(Ye));
    std::string name = Ye.name();
    for (const data::data_expression& exp: Ye.parameters())
    {
      if (is_function_symbol(exp) && exp.sort() != data::sort_pos::pos() && exp.sort() != data::sort_nat::nat())
      {
        // This case is dealt with separately, as it occurs often.
        // The use of pp as in the next case is correct for this case also, but very time consuming.
        // The exception to this rule is constants @c1 of sort Pos, @c0 of sort Nat.
        name += "@";
        name += atermpp::down_cast<data::function_symbol>(exp).name();
      }
      else if (is_function_symbol(exp) || is_application(exp) || is_abstraction(exp))
      {
        name += "@";
        name += data::pp(exp);
      }
      else
      {
        throw mcrl2::runtime_error(std::string("presinst_rename_long: could not rename the variable ") + pres_system::pp(Ye) + " " + data::pp(exp));
      }
    }
    return name;
  }
};

/// \brief Creates a unique name for a propositional variable instantiation. The
/// propositional variable instantiation must be closed.
/// Originally implemented by Alexander van Dam.
/// \return A name that uniquely corresponds to the propositional variable.
struct presinst_rename
{
  propositional_variable_instantiation operator()(const propositional_variable_instantiation& Ye) const
  {
    if (!presinst_is_constant(Ye))
    {
      return Ye;
    }
    return propositional_variable_instantiation(presinst_rename_long()(Ye), data::data_expression_list());
  }
};

/// \brief Algorithm class for the presinst instantiation algorithm.
class presinst_algorithm
{
  protected:
    /// \brief The data specification;
    const data::data_specification& m_data_spec;

    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief The number of generated equations.
    std::size_t m_equation_count = 0;

    /// \brief Propositional variable instantiations that need to be handled.
    std::set<propositional_variable_instantiation> todo;

    /// \brief Propositional variable instantiations that have been handled.
    std::set<propositional_variable_instantiation> done;

    /// \brief Data structure for storing the result. E[i] corresponds to the equations
    /// generated from the i-th PRES equation.
    std::vector<std::vector<pres_equation> > E;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief A lookup map for PRES equations.
    std::map<core::identifier_string, int> equation_index;

    /// \brief Print the equations to standard out.
    bool m_print_equations;

    /// \brief Prints a log message for every 1000-th equation
    std::string print_equation_count(std::size_t size) const
    {
      if (size > 0 && size % 1000 == 0)
      {
        std::ostringstream out;
        out << "Generated " << size << " BES equations" << std::endl;
        return out.str();
      }
      return "";
    }

    // renames propositional variables in x
    pres_expression rho(const pres_expression& x) const
    {
      return replace_propositional_variables(x, presinst_rename());
    }

  public:

    /// \brief Constructor.
    /// \param data_spec A data specification.
    /// \param rewrite_strategy A strategy for the data rewriter.
    /// \param print_equations If true, the generated equations are printed.
    explicit presinst_algorithm(data::data_specification const& data_spec,
        data::rewriter::strategy rewrite_strategy = data::jitty,
        bool print_equations = false)
        : m_data_spec(data_spec),
          datar(data_spec, rewrite_strategy),
          R(datar, data_spec),

          m_print_equations(print_equations)
    {}

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    /// \param p A PRES.
    void run(pres& p)
    {
      using utilities::detail::pick_element;
      using utilities::detail::contains;

      pres_system::detail::instantiate_global_variables(p);

      // simplify all right hand sides of p
      //
      // NOTE: This is not just an optimization. There are certain PRES
      // equations for which applying enumerate_quantifiers_rewriter directly
      // won't terminate, like:
      //
      // forall m: Nat . exists k: Nat . val(m == k)
      pres_system::one_point_rule_rewriter one_point_rule_rewriter;
      pres_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(m_data_spec, datar);
      for (pres_equation& eqn: p.equations())
      {
        eqn.formula() = one_point_rule_rewriter(simplify_rewriter(eqn.formula()));
      }

      // initialize equation_index and E
      int eqn_index = 0;
      auto const& equations = p.equations();
      for (const pres_equation& eqn : equations)
      {
        equation_index[eqn.variable().name()] = eqn_index++;
        E.emplace_back();
      }
      init = atermpp::down_cast<propositional_variable_instantiation>(R(p.initial_state()));
      todo.insert(init);
      while (!todo.empty())
      {
      	auto const& X_e = pick_element(todo);
        done.insert(X_e);
        int index = equation_index[X_e.name()];
        const pres_equation& eqn = p.equations()[index];
        data::rewriter::substitution_type sigma;
        make_presinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pres_expression psi_e = R(phi, sigma);
        R.clear_identifier_generator();
        for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(psi_e))
        {
          if (!contains(done, v))
          {
            todo.insert(v);
          }
        }
        pres_equation new_eqn(eqn.symbol(), propositional_variable(presinst_rename()(X_e).name(), data::variable_list()), rho(psi_e));
        if (m_print_equations)
        {
          mCRL2log(log::info) << eqn.symbol() << " " << X_e << " = " << psi_e << std::endl;
        }
        E[index].push_back(new_eqn);
        mCRL2log(log::verbose) << print_equation_count(++m_equation_count);
        detail::check_res_equation_limit(m_equation_count);
      }
    }

    /// \brief Returns the computed res in pres format
    /// \return The computed res in pres format
    pres get_result()
    {
      pres result;
      for (const std::vector<pres_equation>& equations: E)
      {
        result.equations().insert(result.equations().end(), equations.begin(), equations.end());
      }
      result.initial_state() = presinst_rename()(init);
      return result;
    }

    /// \brief Returns the flag for printing the generated res equations
    /// \return The flag for printing the generated res equations
    bool& print_equations()
    {
      return m_print_equations;
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace mcrl2::pres_system



#endif // MCRL2_PRES_PRESINST_ALGORITHM_H
