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
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/stategraph_utility.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class predicate_variable
{
  friend class stategraph_equation;

  protected:
    propositional_variable_instantiation X;
    pbes_expression m_guard;
    data::mutable_map_substitution<> m_sigma;
    std::map<std::size_t, data::data_expression> m_source; // source[j] = e <=> source(X, i, j) = e
    std::map<std::size_t, data::data_expression> m_dest;   // dest[j] = c   <=> dest(X, i, j) = c
    std::map<std::size_t, std::size_t> m_copy;             // copy[j] = k   <=> copy(X, i, j) = k
    std::set<std::size_t> m_used;                          // j \in used    <=> used(X, i, j)
    std::set<std::size_t> m_changed;                       // j \in changed <=> changed(X, i, j)

  public:

    predicate_variable(const propositional_variable_instantiation& X_, const pbes_expression& guard_)
      : X(X_), m_guard(guard_)
    {}

    const propositional_variable_instantiation& variable() const
    {
      return X;
    }

    const pbes_expression& guard() const
    {
      return m_guard;
    }

    const core::identifier_string& name() const
    {
      return X.name();
    }

    const data::mutable_map_substitution<>& sigma() const
    {
      return m_sigma;
    }

    const data::data_expression_list& parameters() const
    {
      return X.parameters();
    }

    const std::map<std::size_t, data::data_expression>& source() const
    {
      return m_source;
    }

    const std::map<std::size_t, data::data_expression>& dest() const
    {
      return m_dest;
    }

    const std::map<std::size_t, std::size_t>& copy() const
    {
      return m_copy;
    }

    const std::set<std::size_t>& used() const
    {
      return m_used;
    }

    const std::set<std::size_t>& changed() const
    {
      return m_changed;
    }

    void simplify_guard()
    {
      data::detail::simplify_rewriter r;
      stategraph_simplifying_rewriter<pbes_expression, data::detail::simplify_rewriter> R(r);
      m_guard = R(m_guard);
    }
};

inline
std::ostream& operator<<(std::ostream& out, const predicate_variable& x)
{
  return out << x.variable();
}

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
    void find_equality_conjuncts(const pbes_expression& x, const std::vector<data::variable>& d, std::map<data::variable, data::data_expression>& result) const
    {
      std::vector<pbes_expression> v;
      split_and(x, v);
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        if (data::is_data_expression(*i))
        {
          const data::data_expression& v_i = atermpp::aterm_cast<const data::data_expression>(*i);
          if (data::is_equal_to_application(v_i))
          {
            data::data_expression left = data::binary_left1(v_i);
            data::data_expression right = data::binary_right1(v_i);
            if (data::is_variable(left) && std::find(d.begin(), d.end(), data::variable(left)) != d.end() && is_constant(right))
            {
              const data::variable& vleft = core::static_down_cast<const data::variable&>(left);
              result[vleft] = right;
            }
            else if (data::is_variable(right) && std::find(d.begin(), d.end(), data::variable(right)) != d.end() && is_constant(left))
            {
              const data::variable& vright = core::static_down_cast<const data::variable&>(right);
              result[vright] = left;
            }
          }
          // TODO: handle conjuncts b and !b, with b a variable with sort Bool
          //else if (data::is_variable(v_i) && sort_bool::is_bool(v_i.sort()) && std::find(d.begin(), d.end(), data::variable(v_i)) != d.end())
          //{
          //  sigma[data::variable(v_i)] = sort_bool::true_();
          //}
        }
      }
      mCRL2log(log::debug2, "stategraph") << "  computing source: expression = " << x << ", d_X = " << core::detail::print_list(d) << ", result = " << core::detail::print_map(result) << std::endl;
    }

    bool is_cf(const std::map<core::identifier_string, std::vector<bool> >& is_control_flow, const core::identifier_string& X, std::size_t i) const
    {
      auto j = is_control_flow.find(X);
      assert(j != is_control_flow.end());
      const std::vector<bool>& cf = j->second;
      assert(i < cf.size());
      return cf[i];
    }

    // computes the source function for a pbes equation
    void compute_source()
    {
      for (auto i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        std::map<data::variable, data::data_expression> sigma;
        find_equality_conjuncts(i->m_guard, m_parameters, sigma);

        // convert sigma to source
        for (std::size_t j = 0; j < m_parameters.size(); j++)
        {
          data::variable d_j = m_parameters[j];
          auto k = sigma.find(d_j);
          if (k != sigma.end())
          {
            data::data_expression e = k->second;
            i->m_source[j] = e;
            i->m_sigma[d_j] = e;
          }
        }
      }
    }

    void compute_dest(data::rewriter& R)
    {
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        auto const& Ye = m_predvars[i];
        auto const& e = Ye.parameters();
        std::size_t j_index = 0;
        for (auto j = e.begin(); j != e.end(); ++j, ++j_index)
        {
          data::data_expression c = R(*j, m_predvars[i].sigma());
          if (is_constant(c))
          {
            m_predvars[i].m_dest[j_index] = c;
          }
        }
      }
    }

    void compute_copy()
    {
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        auto const& e = m_predvars[i].X.parameters();
        for (std::size_t j = 0; j < m_parameters.size(); j++)
        {
          std::size_t k_index = 0;
          for (auto k = e.begin(); k != e.end(); ++k, ++k_index)
          {
            if (m_parameters[j] == *k)
            {
              m_predvars[i].m_copy[j] = k_index;
            }
          }
        }
      }
    }

    void compute_used()
    {
      using utilities::detail::contains;

      auto const& d_X = m_parameters;
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        predicate_variable& Ye = m_predvars[i];
        auto v = pbes_system::find_free_variables(Ye.guard());
        for (std::size_t j = 0; j < d_X.size(); j++)
        {
          if (contains(v, d_X[j]))
          {
            Ye.m_used.insert(j);
          }
          std::size_t p_index = 0;
          auto const& e = Ye.parameters();
          for (auto p = e.begin(); p != e.end(); ++p, ++p_index)
          {
            auto freevars = data::find_free_variables(*p);
            if (contains(freevars, d_X[j]))
            {
              if ( (Ye.name() != variable().name()) || p_index != j )
              {
                Ye.m_used.insert(j);
              }
            }
          }
        }
      }
    }

    void compute_changed()
    {
      auto const& d_X = m_parameters;
      for (std::size_t i = 0; i < m_predvars.size(); i++)
      {
        auto& Ye = m_predvars[i];
        if (Ye.name() != variable().name())
        {
          continue;
        }
        std::size_t j_index = 0;
        auto const& e = Ye.parameters();
        for (auto j = e.begin(); j != e.end(); ++j, ++j_index)
        {
          if (*j != d_X[j_index])
          {
            Ye.m_changed.insert(j_index);
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
      for (auto i = guards.begin(); i != guards.end(); ++i)
      {
        m_predvars.push_back(predicate_variable(i->first, i->second));
      }
      m_condition = f.expression_stack.back().condition;
      data::variable_list params = variable().parameters();
      m_parameters = std::vector<data::variable>(params.begin(), params.end());
    }

    void compute_source_dest_copy(data::rewriter& R)
    {
      compute_source();
      compute_dest(R);
      compute_copy();
      compute_used();
      compute_changed();
    }

    bool is_simple() const
    {
      for (auto i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        // TODO check this
        if (!pbes_system::is_false(i->guard()))
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
      for (auto i = m_predvars.begin(); i != m_predvars.end(); ++i)
      {
        out << "variable = " << i->variable() << " guard = " << i->guard() << std::endl;
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
        out << "    predvar[" << i << "] = " << m_predvars[i] << " guard = " << m_predvars[i].guard() << std::endl;

        // source
        auto const& source = m_predvars[i].source();
        for (auto j = source.begin(); j != source.end(); ++j)
        {
          out << "        source(" << X << ", " << i << ", " << j->first << ") = " << j->second << std::endl;
        }

        // sigma
        out << "        sigma = " << data::print_substitution(m_predvars[i].sigma()) << std::endl;

        // dest
        const std::map<std::size_t, data::data_expression>& dest = m_predvars[i].dest();
        for (auto j = dest.begin(); j != dest.end(); ++j)
        {
          out << "        dest(" << X << ", " << i << ", " << j->first << ") = " << j->second << std::endl;
        }

        // copy
        auto const& copy = m_predvars[i].copy();
        for (auto j = copy.begin(); j != copy.end(); ++j)
        {
          out << "        copy(" << X << ", " << i << ", " << j->first << ") = " << j->second << std::endl;
        }
        out <<   "        used    = " << core::detail::print_set(m_predvars[i].used()) << std::endl;
        out <<   "        changed = " << core::detail::print_set(m_predvars[i].changed()) << std::endl;
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
    propositional_variable_instantiation m_initial_state;

  public:
    stategraph_pbes()
    {}

    /// \brief Constructor
    /// \pre The pbes p must be in STATEGRAPH format
    stategraph_pbes(const pbes& p)
      : m_data(p.data()), m_global_variables(p.global_variables()), m_initial_state(p.initial_state())
    {
      const std::vector<pbes_equation>& equations = p.equations();
      for (auto i = equations.begin(); i != equations.end(); ++i)
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

    const propositional_variable_instantiation& initial_state() const
    {
      return m_initial_state;
    }

    const data::data_specification& data() const
    {
      return m_data;
    }

    data::data_expression source(std::size_t k, std::size_t i, std::size_t n) const
    {
      auto const& eqn = equations()[k];
      auto const& sigma = eqn.predicate_variables()[i].sigma();
      data::variable d_n = eqn.parameters()[n];
      data::data_expression x = sigma(d_n);
      if (x == d_n)
      {
        return data::undefined_data_expression();
      }
      else
      {
        return x;
      }
    }

    void compute_source_dest_copy()
    {
      data::rewriter R(m_data);
      std::vector<stategraph_equation>& eqn = equations();
      for (auto i = eqn.begin(); i != eqn.end(); ++i)
      {
        i->compute_source_dest_copy(R);
      }
    }

    std::string print_source_dest_copy() const
    {
      std::ostringstream out;
      auto const& eqn = equations();
      for (auto i = eqn.begin(); i != eqn.end(); ++i)
      {
        out << "equation = " << print_equation(*i) << std::endl;
        out << i->print_source_dest_copy() << std::endl;
      }
      return out.str();
    }
};

inline
std::vector<stategraph_equation>::const_iterator find_equation(const stategraph_pbes& p, const core::identifier_string& X, bool warn = true)
{
  auto const& equations = p.equations();
  for (auto i = equations.begin(); i != equations.end(); ++i)
  {
    if (i->variable().name() == X)
    {
      return i;
    }
  }
  if (warn)
  {
    mCRL2log(log::debug, "stategraph") << "find_equation: could not find predicate variable " << X << std::endl;
  }
  return equations.end();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_PBES_H
