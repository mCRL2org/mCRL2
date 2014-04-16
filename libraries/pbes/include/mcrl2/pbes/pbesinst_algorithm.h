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
#include "mcrl2/data/substitutions.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/bes_equation_limit.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/rewriters/custom_enumerate_quantifiers_rewriter.h"
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
/// \return The substitution that maps the i-th element of \p v to the i-th element of \p e
inline
data::mutable_map_substitution<> make_pbesinst_substitution(const data::variable_list& v, const data::data_expression_list& e)
{
  assert(v.size() == e.size());
  data::mutable_map_substitution<> sigma;
  data::variable_list::iterator i = v.begin();
  data::data_expression_list::iterator j = e.begin();

  for (; i != v.end(); ++i, ++j)
  {
    sigma[*i] = *j;
  }
  return sigma;
}

/// \brief Stream operator
/// \param out An output stream
/// \param sigma A pbesinst substitution function
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const data::mutable_map_substitution<>& sigma)
{
  for (auto i = sigma.begin(); i != sigma.end(); ++i)
  {
    out << "  " << data::pp(i->first) << " -> " << data::pp(i->second) << std::endl;
  }
  return out;
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
    auto const& e = Ye.parameters();
    std::string name = Ye.name();
    if (!e.empty())
    {
      for (auto i = e.begin(); i != e.end(); i++)
      {
        if (is_function_symbol(*i) || is_application(*i) || is_abstraction(*i))
        {
          name += "@";
          name += data::pp(*i);
        }
        else
        {
          throw mcrl2::runtime_error(std::string("pbesinst_rewrite_builder: could not rename the variable ") + pbes_system::pp(Ye) + " " + data::pp(*i));
        }
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

    /// \brief Data enumerator.
    data::data_enumerator datae;

    /// \brief The rewriter.
    custom_enumerate_quantifiers_rewriter R;

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
    /// \param print_rewriter_output If true, invocations of the rewriter are printed
    pbesinst_algorithm(data::data_specification const& data_spec,
                       data::rewriter::strategy rewrite_strategy = data::jitty,
                       bool print_equations = false,
                       bool print_rewriter_output = false
                      )
      :
        datar(data_spec, rewrite_strategy),
        datae(data_spec, datar),
        R(datar, datae),
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

      // initialize equation_index and E
      int eqn_index = 0;
      auto const& equations = p.equations();
      for (auto i = equations.begin(); i != equations.end(); ++i)
      {
        auto const& eqn = *i;
        equation_index[eqn.variable().name()] = eqn_index++;
        E.push_back(std::vector<pbes_equation>());
      }
      init = atermpp::aterm_cast<propositional_variable_instantiation>(R(p.initial_state()));
      todo.insert(init);
      while (!todo.empty())
      {
      	auto const& X_e = pick_element(todo);
        done.insert(X_e);
        int index = equation_index[X_e.name()];
        const pbes_equation& eqn = p.equations()[index];
        data::mutable_map_substitution<> sigma = make_pbesinst_substitution(eqn.variable().parameters(), X_e.parameters());
        auto const& phi = eqn.formula();
        pbes_expression psi_e = R(phi, sigma);
        std::set<propositional_variable_instantiation> psi_variables = find_propositional_variable_instantiations(psi_e);
        for (auto i = psi_variables.begin(); i != psi_variables.end(); ++i)
        {
          if (!contains(done, *i))
          {
            todo.insert(*i);
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
      for (auto i =  E.begin(); i != E.end(); ++i)
      {
        result.equations().insert(result.equations().end(), i->begin(), i->end());
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

    custom_enumerate_quantifiers_rewriter& rewriter()
    {
      return R;
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PBESINST_ALGORITHM_H
