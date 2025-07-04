// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/quantifier_propagate.h

#ifndef MCRL2_PBES_QUANTIFIER_PROPAGATE_H
#define MCRL2_PBES_QUANTIFIER_PROPAGATE_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/replace.h"

#include <queue>



namespace mcrl2::pbes_system {

namespace detail {

pbes_expression quantifier_propagate(const pbes_expression& x);

inline
pbes_expression make_exists_(std::set<data::variable> vars, const pbes_expression& expr)
{
  return vars.empty() ? expr : exists(data::variable_list(vars.begin(), vars.end()), expr);
}

inline
pbes_expression make_forall_(std::set<data::variable> vars, const pbes_expression& expr)
{
  return vars.empty() ? expr : forall(data::variable_list(vars.begin(), vars.end()), expr);
}

inline
pbes_expression make_quantifier(bool is_forall, const data::variable_list& vars, const pbes_expression& body)
{
  return is_forall ? pbes_expression(forall(vars, body)) : pbes_expression(exists(vars, body));
}

class quantifier
{
protected:
  bool m_is_forall;
  std::set<data::variable> m_vars;

  pbes_expression make_expr(const std::set<data::variable>& vars, const pbes_expression& expr) const
  {
    return m_is_forall ? make_forall_(vars, expr) : make_exists_(vars, expr);
  }

public:
  quantifier(bool is_forall, const data::variable_list& vars)
  : m_is_forall(is_forall)
  , m_vars(vars.begin(), vars.end())
  {}

  void add_variables(const data::variable_list& vars)
  {
    m_vars.insert(vars.begin(), vars.end());
  }

  bool is_forall() const
  {
    return m_is_forall;
  }

  const std::set<data::variable>& variables() const
  {
    return m_vars;
  }

  bool operator==(const quantifier& other) const
  {
    return m_is_forall == other.m_is_forall && m_vars == other.m_vars;
  }

  bool operator<(const quantifier& other) const
  {
    return m_is_forall < other.m_is_forall || (m_is_forall == other.m_is_forall && m_vars < other.m_vars);
  }

  pbes_expression make_expr_include_only(const std::set<data::variable>& include, const pbes_expression& expr) const
  {
    return make_expr(utilities::detail::set_intersection(m_vars, include), expr);
  }

  pbes_expression make_expr_exclude(const std::set<data::variable>& exclude, const pbes_expression& expr) const
  {
    return make_expr(utilities::detail::set_difference(m_vars, exclude), expr);
  }

  std::string to_string() const
  {
    std::ostringstream out;
    out << (is_forall() ? "forall " : "exists ") << core::detail::print_list(variables()) << ". ";
    return out.str();
  }
};

struct quantifier_propagate_builder: public pbes_expression_builder<quantifier_propagate_builder>
{
public:
  using super = pbes_expression_builder<quantifier_propagate_builder>;
  using equation_map_t = std::map<core::identifier_string, pbes_equation>;
  using super::apply;
  using super::enter;

private:
  std::list<std::pair<core::identifier_string, pbes_equation>>& m_new_equations;
  const equation_map_t& m_eqn_map;
  data::set_identifier_generator& id_gen;

  std::list<pbes_expression> quantified_context;

  std::list<quantifier> make_quantifier_list(const std::list<pbes_expression>& ctx) const
  {
    // Create list of quantified variables around X_e
    std::list<quantifier> result;
    for(const pbes_expression& pe: ctx)
    {
      assert(is_forall(pe) || is_exists(pe));

      data::variable_list vars(is_forall(pe) ? atermpp::down_cast<forall>(pe).variables() : atermpp::down_cast<exists>(pe).variables());

      if(result.empty() || result.back().is_forall() != is_forall(pe))
      {
        result.emplace_back(is_forall(pe), vars);
      }
      else
      {
        result.back().add_variables(vars);
      }
    }
    return result;
  }

  pbes_equation find_equation(const propositional_variable_instantiation& X_e)
  {
    return m_eqn_map.at(X_e.name());
  }

public:
  quantifier_propagate_builder(std::list<std::pair<core::identifier_string, pbes_equation>>& new_eqns,
                                data::set_identifier_generator& ig,
                                const equation_map_t& eq_idx)
  : m_new_equations(new_eqns)
  , m_eqn_map(eq_idx)
  , id_gen(ig)
  {}

  pbes_expression apply_quantifier(bool is_forall, const data::variable_list& vars, const pbes_expression& body)
  {
    pbes_expression x = make_quantifier(is_forall, vars, body);
    quantified_context.push_back(x);

    pbes_expression result;
    apply(result, body);

    if(quantified_context.empty())
    {
      // The body contained some other operator and the stack was cleared
      // Reconstruct the quantifier
      return make_quantifier(is_forall, vars, result);
    }
    else
    {
      // The body contained a QPVI
      quantified_context.pop_back();
      if(result == body)
      {
        // The body didn't change, we just return ourselves as well
        return x;
      }
      else
      {
        // The body changed, it reconstructed the QPVI, we should not
        // add anything
        return result;
      }
    }
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    result = apply_quantifier(true, x.variables(), x.body());
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    result = apply_quantifier(false, x.variables(), x.body());
  }

  void enter(const and_& /*x*/)
  {
    quantified_context.clear();
  }

  void enter(const or_& /*x*/)
  {
    quantified_context.clear();
  }

  void enter(const imp& /*x*/)
  {
    quantified_context.clear();
  }

  void enter(const not_& /*x*/)
  {
    quantified_context.clear();
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& X_e)
  {
    using utilities::detail::contains;
    using utilities::detail::set_difference;
    using utilities::detail::set_includes;
    using utilities::detail::set_intersection;

    std::list<quantifier> qvars = make_quantifier_list(quantified_context);
    if(qvars.empty())
    {
      result = X_e;
      return;
    }

    const std::vector<data::variable> parameters(find_equation(X_e).variable().parameters().begin(), find_equation(X_e).variable().parameters().end());
    const std::vector<data::data_expression> updates(X_e.parameters().begin(), X_e.parameters().end());

    std::list<std::size_t> independent_pars;
    std::set<data::variable> quantified_variables;
    for(const quantifier& qv: qvars)
    {
      quantified_variables.insert(qv.variables().begin(), qv.variables().end());
    }

    // Start building a list of parameters that are constant and can be pushed through X_e
    std::set<data::variable> seen;
    std::size_t i = 0;
    for(const data::data_expression& up: updates)
    {
      std::set<data::variable> fv = find_free_variables(up);
      if(set_includes(quantified_variables, fv))
      {
        independent_pars.push_back(i);
      }
      else
      {
        seen.insert(fv.begin(), fv.end());
      }
      i++;
    }
    std::queue<data::variable> todo(std::deque<data::variable>(seen.begin(), seen.end()));

    // Add all transitive dependencies, either because they occur together in one
    // of the updates, or because of their quantifier scopes
    while(!todo.empty())
    {
      data::variable elem = todo.front();
      todo.pop();

      // Check for each update if it contains elem. If so, elem also influences the
      // other free variables in that update expression an the parameter is not independent.
      for(std::list<std::size_t>::const_iterator ip = independent_pars.begin(); ip != independent_pars.end(); )
      {
        std::set<data::variable> fv = find_free_variables(updates[*ip]);
        if(contains(fv, elem))
        {
          for(const data::variable& var: set_difference(fv, seen))
          {
            todo.push(var);
            seen.insert(var);
          }
          ip = independent_pars.erase(ip);
        }
        else
        {
          ++ip;
        }
      }

      // Check if we need to add quantified variables that have a larger scope and
      // and are at least one quantifier alternation away from elem
      bool add_rest = false;
      for(auto qv = qvars.rbegin(); qv != qvars.rend(); ++qv)
      {
        const std::set<data::variable>& vars = qv->variables();
        if(add_rest)
        {
          for(const data::variable& var: set_difference(vars, seen))
          {
            seen.insert(var);
            todo.push(var);
          }
        }
        else if(contains(vars, elem))
        {
          add_rest = true;
        }
      }
    }

    // Check whether there is at least one independent parameter and we are
    // propagating at least one quantified variable.
    if(independent_pars.empty() ||
      set_difference(qvars.back().variables(), seen).empty())
    {
      result = X_e;
      return;
    }

    // Build a new equation based on the independent parameters found
    data::variable_list new_parameter_list;
    data::data_expression_list new_update_list;
    i = 0;
    auto ip = independent_pars.begin();
    data::rewriter::substitution_type sigma;

    for(const data::variable& par: parameters)
    {
      if(ip != independent_pars.end() && *ip == i)
      {
        sigma[par] = updates[i];
        ++ip;
      }
      else
      {
        new_parameter_list.push_front(par);
        new_update_list.push_front(updates[i]);
      }
      i++;
    }
    new_parameter_list = reverse(new_parameter_list);
    new_update_list = reverse(new_update_list);

    // Construct a new PVI and a new equation
    core::identifier_string new_name = id_gen(X_e.name());
    pbes_expression new_rhs_X = pbes_system::replace_free_variables(find_equation(X_e).formula(), sigma);
    propositional_variable_instantiation new_X_e(new_name, new_update_list);
    pbes_expression new_Q_X_e = new_X_e;
    for(auto qv = qvars.rbegin(); qv != qvars.rend(); ++qv)
    {
      new_Q_X_e = qv->make_expr_include_only(seen, new_Q_X_e);
      new_rhs_X = qv->make_expr_exclude(seen, new_rhs_X);
    }
    pbes_equation new_eqn(find_equation(X_e).symbol(), propositional_variable(new_name, new_parameter_list), new_rhs_X);
    m_new_equations.emplace_back(X_e.name(), new_eqn);

    result = new_Q_X_e;
  }
};

inline
void quantifier_propagate(
        std::list<std::pair<core::identifier_string, pbes_equation>>& new_equations,
        data::set_identifier_generator& id_gen,
        const quantifier_propagate_builder::equation_map_t eqn_map,
        pbes_expression& x)
{
  quantifier_propagate_builder f(new_equations, id_gen, eqn_map);
  f.update(x);
}

} // namespace detail

inline
void quantifier_propagate(pbes& p)
{
  std::list<std::pair<core::identifier_string, pbes_equation>> new_equations;
  detail::quantifier_propagate_builder::equation_map_t m_eqn_map;
  data::set_identifier_generator id_gen;
  for(const pbes_equation& eq: p.equations())
  {
    id_gen.add_identifier(eq.variable().name());
    m_eqn_map[eq.variable().name()] = eq;
  }

  for(pbes_equation& eqn: p.equations())
  {
    detail::quantifier_propagate(new_equations, id_gen, m_eqn_map, eqn.formula());
  }

  // Insert new equations
  for(const auto& [target, eqn]: new_equations)
  {
    // Find the original location, so ranks are preserved
    p.equations().insert(
      std::find_if(p.equations().begin(), p.equations().end(),
        [t = target](const pbes_equation& eq){ return eq.variable().name() == t; }),
      eqn
    );
  }
}

inline
pbes quantifier_propagate(const pbes& p)
{
  pbes result{p};
  quantifier_propagate(result);
  return result;
}

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_QUANTIFIER_PROPAGATE_H
