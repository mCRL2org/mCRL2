// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H

#include <cassert>
#include <iostream>
#include <sstream>
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct predicate_variable
{
  predicate_variable(const propositional_variable_instantiation& X_, const pbes_expression& guard_)
    : X(X_), guard(guard_)
  {}

  propositional_variable_instantiation X;
  pbes_expression guard;
  data::mutable_map_substitution<> sigma;
  std::map<std::size_t, data::data_expression> source; // source[j] = e <=> source(X, i, j) = e
  std::map<std::size_t, data::data_expression> dest;   // dest[j] = c   <=> dest(X, i, j) = c
  std::map<std::size_t, std::size_t> copy;             // copy[j] = k   <=> copy(X, i, j) = k
  std::set<std::size_t> used;                          // j \in used    <=> used(X, i, j)
  std::set<std::size_t> changed;                       // j \in changed <=> changed(X, i, j)
};

class stategraph_equation: public pbes_equation
{
  protected:
    std::vector<predicate_variable> m_predvars;
    std::vector<data::variable> m_parameters;
    pbes_expression m_condition;

    void split_and(const pbes_expression& expr, std::vector<pbes_expression>& result) const
    {
      namespace a = combined_access;
      utilities::detail::split(expr, std::back_inserter(result), a::is_and, a::left, a::right);
    }

    // Extracts all conjuncts d[i] == e from the pbes expression x, for some i in 0 ... d.size(), and with e a constant.
    // The conjuncts are added to the substitution sigma.
    void find_equality_conjuncts(const pbes_expression& x, const std::vector<data::variable>& d, data::mutable_map_substitution<>& sigma) const
    {
      std::vector<data::data_expression> result;

      std::vector<pbes_expression> v;
      split_and(x, v);
      for (std::vector<pbes_expression>::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (data::is_data_expression(*i))
        {
          data::data_expression v_i = *i;
          if (data::is_equal_to_application(v_i))
          {
            data::data_expression left = data::binary_left(v_i);
            data::data_expression right = data::binary_right(v_i);
            if (data::is_variable(left) && std::find(d.begin(), d.end(), data::variable(left)) != d.end() && is_constant(right))
            {
              sigma[left] = right;
            }
            else if (data::is_variable(right) && std::find(d.begin(), d.end(), data::variable(right)) != d.end() && is_constant(left))
            {
              sigma[right] = left;
            }
          }
          // TODO: handle conjuncts b and !b, with b a variable with sort Bool
          //else if (data::is_variable(v_i) && sort_bool::is_bool(v_i.sort()) && std::find(d.begin(), d.end(), data::variable(v_i)) != d.end())
          //{
          //  sigma[data::variable(v_i)] = sort_bool::true_();
          //}
        }
      }
    }

    bool is_cf(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow, const core::identifier_string& X, std::size_t i) const
    {
      std::map<core::identifier_string, std::vector<bool> >::const_iterator j = is_control_flow.find(X);
      assert(j != is_control_flow.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    // computes the source function for a pbes equation
    void compute_source(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow)
    {
      const core::identifier_string& X = variable().name();
      for (std::vector<predicate_variable>::iterator i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        find_equality_conjuncts(i->guard, m_parameters, i->sigma);

        // convert sigma to source
        for (std::size_t j = 0; j < m_parameters.size(); j++)
        {
          if (!is_cf(is_control_flow, X, j))
          {
            continue;
          }
          data::variable d_j = m_parameters[j];
          data::data_expression e = i->sigma(d_j);
          if (e != d_j)
          {
            i->source[j] = e;
          }
        }
      }
    }

    void compute_dest(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow, data::rewriter& R)
    {
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        // PVI(X, I) = Y(e)
        const core::identifier_string& Y = m_predvars[i].X.name();
        const data::data_expression_list& e = m_predvars[i].X.parameters();
        std::size_t j_index = 0;
        for (data::data_expression_list::const_iterator j = e.begin(); j != e.end(); ++j, ++j_index)
        {
          data::data_expression c = R(*j, m_predvars[i].sigma);
          if (is_constant(c) && is_cf(is_control_flow, Y, j_index))
          {
            m_predvars[i].dest[j_index] = c;
          }
        }
      }
    }

    void compute_copy(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow)
    {
      const core::identifier_string& X = variable().name();
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        // PVI(X, I) = Y(e)
        const core::identifier_string& Y = m_predvars[i].X.name();
        const data::data_expression_list& e = m_predvars[i].X.parameters();

        for (std::size_t j = 0; j < m_parameters.size(); j++)
        {
          if (!is_cf(is_control_flow, X, j))
          {
            continue;
          }
          std::size_t k_index = 0;
          for (data::data_expression_list::const_iterator k = e.begin(); k != e.end(); ++k, ++k_index)
          {
            if (m_parameters[j] == *k && is_cf(is_control_flow, Y, k_index))
            {
              m_predvars[i].copy[j] = k_index;
            }
          }
        }
      }
    }

    void compute_used()
    {
      const std::vector<data::variable>& d_X = m_parameters;
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        predicate_variable& Y = m_predvars[i];
        std::set<data::variable> v = pbes_system::find_free_variables(Y.guard);
        for (std::size_t j = 0; j < d_X.size(); j++)
        {
          if (v.find(d_X[j]) != v.end())
          {
            Y.used.insert(j);
          }
        }
      }
    }

    void compute_changed()
    {
      const std::vector<data::variable>& d_X = m_parameters;
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        predicate_variable& Y = m_predvars[i];
        if (Y.X.name() != variable().name())
        {
          continue;
        }
        std::size_t j_index = 0;
        const data::data_expression_list& e = Y.X.parameters();
        for (data::data_expression_list::const_iterator j = e.begin(); j != e.end(); ++j, ++j_index)
        {
          if (*j != d_X[j_index])
          {
            Y.changed.insert(j_index);
          }
        }
      }
    }

  public:
    stategraph_equation(const pbes_equation& eqn, const data::data_specification& dataspec)
      : pbes_equation(eqn)
    {
      pbes_system::detail::guard_traverser f(dataspec);
      f(eqn.formula());
      const std::vector<std::pair<propositional_variable_instantiation, pbes_expression> >& guards = f.expression_stack.back().guards;
      for (std::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::const_iterator i = guards.begin(); i != guards.end(); ++i)
      {
        m_predvars.push_back(predicate_variable(i->first, i->second));
      }
      m_condition = f.expression_stack.back().condition;
      data::variable_list params = variable().parameters();
      m_parameters = std::vector<data::variable>(params.begin(), params.end());
    }

    void compute_source_dest_copy(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow, data::rewriter& R)
    {
      compute_source(is_control_flow);
      compute_dest(is_control_flow, R);
      compute_copy(is_control_flow);
      compute_used();
      compute_changed();
    }

    bool is_simple() const
    {
      for (std::vector<predicate_variable>::const_iterator i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        // TODO check this
        if (!pbes_system::is_false(i->guard))
        {
          return false;
        }
      }
      return true;
    }

    const pbes_expression& simple_guard() const
    {
      return m_condition;
    }

    const std::vector<data::variable>& parameters() const
    {
      return m_parameters;
    }

    const std::vector<predicate_variable>& predicate_variables() const
    {
      return m_predvars;
    }

    std::vector<predicate_variable>& predicate_variables()
    {
      return m_predvars;
    }

    std::string print() const
    {
      std::ostringstream out;
      out << "equation = " << print_equation(*this) << std::endl;
      out << "guards:" << std::endl;
      for (std::vector<predicate_variable>::const_iterator i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        out << "variable = " << pbes_system::pp(i->X) << " guard = " << pbes_system::pp(i->guard) << std::endl;
      }
      out << "simple = " << std::boolalpha << is_simple() << std::endl;
      return out.str();
    }

    std::string print_source_dest_copy() const
    {
      std::ostringstream out;
      std::string X(variable().name());
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        out << "    predvar[" << i << "] = " << pbes_system::pp(m_predvars[i].X) << " guard = " << pbes_system::pp(m_predvars[i].guard) << std::endl;

        // source
        const std::map<std::size_t, data::data_expression>& source = m_predvars[i].source;
        for (std::map<std::size_t, data::data_expression>::const_iterator j = source.begin(); j != source.end(); ++j)
        {
          out << "        source(" << X << ", " << i << ", " << j->first << ") = " << data::pp(j->second) << std::endl;
        }

        // sigma
        out << "        sigma = " << data::print_substitution(m_predvars[i].sigma) << std::endl;

        // dest
        const std::map<std::size_t, data::data_expression>& dest = m_predvars[i].dest;
        for (std::map<std::size_t, data::data_expression>::const_iterator j = dest.begin(); j != dest.end(); ++j)
        {
          out << "        dest(" << X << ", " << i << ", " << j->first << ") = " << data::pp(j->second) << std::endl;
        }

        // copy
        const std::map<std::size_t, std::size_t>& m = m_predvars[i].copy;
        for (std::map<std::size_t, std::size_t>::const_iterator j = m.begin(); j != m.end(); ++j)
        {
          out << "        copy(" << X << ", " << i << ", " << j->first << ") = " << j->second << std::endl;
        }
      }
      return out.str();
    }
};

// explicit representation of a pbes in STATEGRAPH format
class stategraph_pbes
{
  protected:
    data::data_specification m_data;
    std::vector<stategraph_equation> m_equations;
    std::set<data::variable> m_global_variables;
    pbes_expression m_initial_state;

  public:
    stategraph_pbes()
    {}

    /// \brief Constructor
    /// \pre The pbes p must be in STATEGRAPH format
    stategraph_pbes(const pbes<>& p)
      : m_data(p.data()), m_global_variables(p.global_variables()), m_initial_state(p.initial_state())
    {
      const std::vector<pbes_equation>& equations = p.equations();
      for (std::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        m_equations.push_back(stategraph_equation(*i, m_data));
      }
    }

    const std::vector<stategraph_equation>& equations() const
    {
      return m_equations;
    }

    std::vector<stategraph_equation>& equations()
    {
      return m_equations;
    }

    const std::set<data::variable>& global_variables() const
    {
      return m_global_variables;
    }

    std::set<data::variable>& global_variables()
    {
      return m_global_variables;
    }

    const pbes_expression& initial_state() const
    {
      return m_initial_state;
    }

    const data::data_specification& data() const
    {
      return m_data;
    }

    data::data_expression source(std::size_t k, std::size_t i, std::size_t n) const
    {
      const stategraph_equation& eqn = equations()[k];
      const data::mutable_map_substitution<>& sigma = eqn.predicate_variables()[i].sigma;
      data::variable d_n = eqn.parameters()[n];
      data::data_expression x = sigma(d_n);
      if (x == d_n)
      {
        return data::data_expression();
      }
      else
      {
        return x;
      }
    }

    void compute_source_dest_copy(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow)
    {
      data::rewriter R(m_data);
      std::vector<stategraph_equation>& eqn = equations();
      for (std::vector<stategraph_equation>::iterator i = eqn.begin(); i != eqn.end(); ++i)
      {
        i->compute_source_dest_copy(is_control_flow, R);
      }
    }

    std::string print_source_dest_copy() const
    {
      std::ostringstream out;
      const std::vector<stategraph_equation>& eqn = equations();
      for (std::vector<stategraph_equation>::const_iterator i = eqn.begin(); i != eqn.end(); ++i)
      {
        out << "equation = " << print_equation(*i) << std::endl;
        out << i->print_source_dest_copy() << std::endl;
      }
      return out.str();
    }
};

inline
std::vector<stategraph_equation>::const_iterator find_equation(const stategraph_pbes& p, const core::identifier_string& X)
{
  const std::vector<stategraph_equation>& equations = p.equations();
  for (std::vector<stategraph_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->variable().name() == X)
    {
      return i;
    }
  }
  return equations.end();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
