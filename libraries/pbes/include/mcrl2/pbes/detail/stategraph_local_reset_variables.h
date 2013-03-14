// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_local_reset_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H

#include "mcrl2/utilities/sequence.h"
#include "mcrl2/pbes/detail/stategraph_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_local_algorithm.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

class local_reset_variables_algorithm;
pbes_expression local_reset_variables(local_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X);

struct reset_variable_helper
{
  local_reset_variables_algorithm& algorithm;

  reset_variable_helper(local_reset_variables_algorithm& algorithm_)
    : algorithm(algorithm_)
  {}

  void operator()()
  {
  }
};

/// \brief Adds the reset variables procedure to the stategraph algorithm
class local_reset_variables_algorithm: public stategraph_local_algorithm
{
  public:
    typedef stategraph_local_algorithm super;

  protected:
    const pbes<>& m_original_pbes;

    // if true, the resulting PBES is simplified
    bool m_simplify;

    data::data_expression default_value(const data::sort_expression& x)
    {
      // TODO: make this an attribute
      data::representative_generator f(m_pbes.data());
      return f(x);
    }

    // computes the possible values of d_X[j]
    std::vector<data::data_expression> compute_values(const core::identifier_string& X, std::size_t j)
    {
      std::vector<data::data_expression> result;
      return result;
    }

    bool is_relevant(const core::identifier_string& X, const data::variable& d) const
    {
      return true;
    }

  public:

    pbes_expression reset(const std::vector<data::data_expression>& v_prime, const core::identifier_string& Y, const predicate_variable& X_i, const std::set<std::size_t>& I, const std::vector<data::variable>& d_Y, const std::vector<data::data_expression> e_X)
    {
      data::data_expression c = data::sort_bool::true_();
      std::size_t k = 0;
      std::vector<data::data_expression> v;
      for (std::size_t j = 0; j < d_Y.size(); j++)
      {
        if (is_control_flow_parameter(Y, j))
        {
          if (!contains(I, j))
          {
            v.push_back(v_prime[k]);
            k++;
          }
          else
          {
            assert(X_i.dest.find(j) != X_i.dest.end());
            v.push_back(X_i.dest.find(j)->second);
          }
        }
      }

      k = 0;
      std::vector<data::data_expression> r;
      for (std::size_t j = 0; j < d_Y.size(); j++)
      {
        if (is_control_flow_parameter(Y, j))
        {
          if (X_i.dest.find(j) == X_i.dest.end())
          {
            c = data::lazy::and_(c, data::equal_to(d_Y[j], v[k]));
          }
          r.push_back(v[k]);
        }
        else
        {
          if (is_relevant(Y, d_Y[j]))
          {
            r.push_back(e_X[j]);
          }
          else
          {
            r.push_back(default_value(e_X[j].sort()));
          }
        }
      }
      propositional_variable_instantiation Yr(Y, atermpp::convert<data::data_expression_list>(r));
      if (m_simplify)
      {
        c = m_datar(c);
        if (c != data::sort_bool::false_())
        {
          return imp(c, Yr);
        }
        else
        {
          return data::sort_bool::true_();
        }
      }
      else
      {
        return imp(c, Yr);
      }
    }

    // expands a propositional variable instantiation using the control flow graph
    // x = Y(e)
    // Y(e) = PVI(phi_X, i)
    pbes_expression reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i)
    {
      const predicate_variable& X_i = eq_X.predicate_variables()[i];
      assert(X_i.X == x);

      mCRL2log(log::debug, "stategraph") << "  resetting variable " << pbes_system::pp(x) << std::endl;
      std::vector<pbes_expression> phi;
      core::identifier_string Y = x.name();
      std::vector<data::data_expression> e_X = atermpp::convert<std::vector<data::data_expression> >(x.parameters());

      std::vector<std::size_t> I;
      for (std::size_t j = 0; j < eq_X.parameters().size(); j++)
      {
        if (X_i.dest.find(j) == X_i.dest.end())
        {
          I.push_back(j);
        }
      }

      // std::vector<std::vector<data::data_expression> > values;
      // for (std::vector<std::size_t>::const_iterator ii = I.begin(); ii != I.end(); ++ii)
      // {
      //   values.push_back(compute_values(Y, *ii));
      // }
      // std::vector<data::data_expression> v;
      // for (std::vector<std::vector<data::data_expression> >::const_iterator vi = values.begin(); vi != values.end(); ++vi)
      // {
      //   v.push_back(vi->front());
      // }
      // utilities::foreach_sequence(values, v.begin(), reset_variable_helper(v, ...));
      return pbes_expr::join_and(phi.begin(), phi.end());
    }

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes<>& p)
    {
      mCRL2log(log::debug, "stategraph") << "--- resetting variables to the original PBES ---" << std::endl;

      // apply the reset variable procedure to all propositional variable instantiations
      std::vector<pbes_equation>& p_eqn = p.equations();
      const std::vector<stategraph_equation>& s_eqn = m_pbes.equations();

      for (std::size_t k = 0; k < p_eqn.size(); k++)
      {
        p_eqn[k].formula() = local_reset_variables(*this, p_eqn[k].formula(), s_eqn[k]);
      }

      // TODO: merge the two rewriters?
      if (m_simplify)
      {
        pbes_system::simplifying_rewriter<pbes_expression, data::rewriter> pbesr(m_datar);
        pbes_system::pbes_rewrite(p, pbesr);
      }
    }

    local_reset_variables_algorithm(const pbes<>& p, data::rewriter::strategy rewrite_strategy = data::jitty)
      : stategraph_local_algorithm(p, rewrite_strategy),
        m_original_pbes(p)
    {}

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes<> run(bool simplify = true)
    {
      super::run();
      m_simplify = simplify;
      pbes<> result = m_original_pbes;
      reset_variables_to_original(result);
      return result;
    }
};

/// N.B. It is essential that this traverser uses the same traversal order as the guard_traverser.
struct local_reset_traverser: public pbes_expression_traverser<local_reset_traverser>
{
  typedef pbes_expression_traverser<local_reset_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  local_reset_variables_algorithm& algorithm;
  const stategraph_equation& eq_X;
  std::size_t& i;

  local_reset_traverser(local_reset_variables_algorithm& algorithm_, const stategraph_equation& eq_X_, std::size_t& i_)
    : algorithm(algorithm_),
      eq_X(eq_X_),
      i(i_)
  {}

  std::vector<pbes_expression> expression_stack;

  void push(const pbes_expression& x)
  {
    mCRL2log(log::debug1) << "<push>" << "\n" << x << std::endl;
    expression_stack.push_back(x);
  }

  pbes_expression& top()
  {
    return expression_stack.back();
  }

  const pbes_expression& top() const
  {
    return expression_stack.back();
  }

  pbes_expression pop()
  {
    pbes_expression result = top();
    expression_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(x);
  }

  void leave(const pbes_system::propositional_variable_instantiation& x)
  {
    pbes_expression result = algorithm.reset_variable(x, eq_X, i);
    i++;
    push(result);
  }

  void leave(const pbes_system::true_& x)
  {
    push(x);
  }

  void leave(const pbes_system::false_& x)
  {
    push(x);
  }

  void leave(const pbes_system::not_& x)
  {
    pbes_expression operand = pop();
    push(not_(atermpp::aterm_cast<atermpp::aterm_appl>(operand)));
  }

  void leave(const pbes_system::and_& x)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(and_(left, right));
  }

  void leave(const pbes_system::or_& x)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(or_(left, right));
  }

  void leave(const pbes_system::imp& x)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(imp(left, right));
  }

  void leave(const pbes_system::forall& x)
  {
    pbes_expression operand = pop();
    push(forall(x.variables(), operand));
  }

  void leave(const pbes_system::exists& x)
  {
    pbes_expression operand = pop();
    push(exists(x.variables(), operand));
  }
};

inline
pbes_expression local_reset_variables(local_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X)
{
  std::size_t i = 0;
  local_reset_traverser f(algorithm, eq_X, i);
  f(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
