// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/pbesinst_algorithm.h
/// \brief Algorithm for instantiating a PBES.

#include <cassert>
#include <set>
#include <iostream>
#include <sstream>
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h"
#include "mcrl2/utilities/detail/container_utility.h"

#ifndef MCRL2_PBES_PBESINST_ALGORITHM_H
#define MCRL2_PBES_PBESINST_ALGORITHM_H

namespace mcrl2
{

namespace pbes_system
{

/// \brief Creates a substitution function for the pbesinst rewriter.
/// \param v A sequence of data variables
/// \param e A sequence of data expressions
/// \param sigma The substitution that maps the i-th element of \p v to the i-th element of \p e
inline
void make_pbesinst_substitution(const data::variable_list& v, const data::data_expression_list& e, data::rewriter::substitution_type& sigma)
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
bool pbesinst_is_constant(const pbes_expression& x)
{
  return pbes_system::find_free_variables(x).empty();
}

/// \brief Creates a unique name for a propositional variable instantiation. The
/// propositional variable instantiation must be closed.
/// Originally implemented by Alexander van Dam.
/// \return A name that uniquely corresponds to the propositional variable.
struct pbesinst_rename: public std::unary_function<propositional_variable_instantiation, propositional_variable_instantiation>
{
  propositional_variable_instantiation operator()(const propositional_variable_instantiation& Ye) const
  {
    if (!pbesinst_is_constant(Ye))
    {
      return Ye;
    }
    const data::data_expression_list& e = Ye.parameters();
    std::string name = Ye.name();
    
    for (const data::data_expression exp: e)
    {
      if (is_function_symbol(exp))
      {
        // This case is dealt with separately, as it occurs often. 
        // The use of pp as in the next case is correct for this case also, but very time consuming. 
        name += "@";
        name += atermpp::down_cast<data::function_symbol>(exp).name();
      }
      else if (is_application(exp) || is_abstraction(exp))
      {
        name += "@";
        name += data::pp(exp);
      }
      else
      {
        throw mcrl2::runtime_error(std::string("pbesinst_rewrite_builder: could not rename the variable ") + pbes_system::pp(Ye) + " " + data::pp(exp));
      }
    }
    
    return propositional_variable_instantiation(name, data::data_expression_list());
  }
};

/// \brief Algorithm class for the pbesinst instantiation algorithm.
class pbesinst_algorithm
{
  typedef core::term_traits<pbes_expression> tr;

  protected:
    /// \brief Data rewriter.
    data::rewriter datar;

    /// \brief The rewriter.
    enumerate_quantifiers_rewriter R;

    /// \brief The number of generated equations.
    int m_equation_count;

    /// \brief Propositional variable instantiations that need to be handled.
    std::set<propositional_variable_instantiation> todo;

    /// \brief Propositional variable instantiations that have been handled.
    std::set<propositional_variable_instantiation> done;

    /// \brief Data structure for storing the result. E[i] corresponds to the equations
    /// generated from the i-th PBES equation.
    std::vector<std::vector<pbes_equation> > E;

    /// \brief The initial value.
    propositional_variable_instantiation init;

    /// \brief A lookup map for PBES equations.
    std::map<core::identifier_string, int> equation_index;

    /// \brief Print the equations to standard out.
    bool m_print_equations;

    /// \brief Prints a log message for every 1000-th equation
    std::string print_equation_count(size_t size) const
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
    pbes_expression rho(const pbes_expression& x) const
    {
      return replace_propositional_variables(x, pbesinst_rename());
    }

  public:

    /// \brief Constructor.
    /// \param data_spec A data specification
    /// \param rewriter_strategy A strategy for the data rewriter
    /// \param print_equations If true, the generated equations are printed
    pbesinst_algorithm(data::data_specification const& data_spec,
                       data::rewriter::strategy rewrite_strategy = data::jitty,
                       bool print_equations = false
                      )
      :
        datar(data_spec, rewrite_strategy),
        R(datar, data_spec),
        m_equation_count(0),
        m_print_equations(print_equations)
    {}

    /// \brief Runs the algorithm. The result is obtained by calling the function \p get_result.
    /// \param p A PBES
    void run(pbes& p)
    {
      using utilities::detail::pick_element;
      using utilities::detail::contains;

      pbes_system::detail::instantiate_global_variables(p);

      // simplify all right hand sides of p
      //
      // NOTE: This is not just an optimization. There are certain PBES
      // equations for which applying enumerate_quantifiers_rewriter directly
      // won't terminate, like:
      //
      // forall m: Nat . exists k: Nat . val(m == k)
      pbes_system::one_point_rule_rewriter one_point_rule_rewriter;
      pbes_system::simplify_quantifiers_data_rewriter<mcrl2::data::rewriter> simplify_rewriter(datar);
      for (pbes_equation& eqn: p.equations())
      {
        eqn.formula() = one_point_rule_rewriter(simplify_rewriter(eqn.formula()));
      }

      // initialize equation_index and E
      int eqn_index = 0;
      auto const& equations = p.equations();
      for (const pbes_equation& eqn : equations)
      {
        equation_index[eqn.variable().name()] = eqn_index++;
        E.push_back(std::vector<pbes_equation>());
      }
      init = atermpp::down_cast<propositional_variable_instantiation>(R(p.initial_state()));
      todo.insert(init);
      while (!todo.empty())
      {
      	auto const& X_e = pick_element(todo);
        done.insert(X_e);
        int index = equation_index[X_e.name()];
        const pbes_equation& eqn = p.equations()[index];
        data::rewriter::substitution_type sigma;
        make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters(), sigma);
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);
        for (const propositional_variable_instantiation& v: find_propositional_variable_instantiations(psi_e))
        {
          if (!contains(done, v))
          {
            todo.insert(v);
          }
        }
        pbes_equation new_eqn(eqn.symbol(), propositional_variable(pbesinst_rename()(X_e).name(), data::variable_list()), rho(psi_e));
        if (m_print_equations)
        {
          mCRL2log(log::info) << eqn.symbol() << " " << X_e << " = " << psi_e << std::endl;
        }
        E[index].push_back(new_eqn);
        mCRL2log(log::verbose) << print_equation_count(++m_equation_count);
        detail::check_bes_equation_limit(m_equation_count);
      }
    }

    /// \brief Returns the computed bes in pbes format
    /// \return The computed bes in pbes format
    pbes get_result()
    {
      pbes result;
      for (const std::vector<pbes_equation>& equations: E)
      {
        result.equations().insert(result.equations().end(), equations.begin(), equations.end());
      }
      result.initial_state() = pbesinst_rename()(init);
      return result;
    }

    /// \brief Returns the flag for printing the generated bes equations
    /// \return The flag for printing the generated bes equations
    bool& print_equations()
    {
      return m_print_equations;
    }

    enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_ALGORITHM_H
