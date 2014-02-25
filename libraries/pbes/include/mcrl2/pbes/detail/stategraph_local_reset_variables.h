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

template <typename Container>
std::string print_vector(const Container& v, const std::string& delim)
{
  std::ostringstream os;
  for(typename Container::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    if(i != v.begin())
    {
      os << delim;
    }
    os << data::pp(*i);
  }
  return os.str();
}

class local_reset_variables_algorithm;
pbes_expression local_reset_variables(local_reset_variables_algorithm& algorithm, const pbes_expression& x, const stategraph_equation& eq_X);

/// \brief Adds the reset variables procedure to the stategraph algorithm
class local_reset_variables_algorithm: public stategraph_local_algorithm
{
  public:
    typedef stategraph_local_algorithm super;

  protected:
    const pbes& m_original_pbes;

    // if true, the resulting PBES is simplified
    bool m_simplify;

    // if true, an optimization will be applied in the reset_variable procedure
    bool m_use_marking_optimization;

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

      std::size_t k = control_flow_index(X, j);
      if (k != data::undefined_index())
      {
        // find vertices X(e) in Gk
        control_flow_graph& Gk = m_control_flow_graphs[k];
        const std::set<stategraph_vertex*>& inst = Gk.index(X);
        for (std::set<stategraph_vertex*>::const_iterator vi = inst.begin(); vi != inst.end(); ++vi)
        {
          const stategraph_vertex& u = **vi;
          result.push_back(u.X.parameters().front());
        }
      }
      mCRL2log(log::debug1, "stategraph") << "Possible values of " << X << "," << j << " are: " << core::detail::print_container(result) << std::endl;

      return result;
    }

    bool is_relevant(const core::identifier_string& X,
                     const predicate_variable& X_i,
                     const data::variable& d,
                     const std::vector<data::data_expression>& v) const
    {
      mCRL2log(log::debug1) << "  checking whether " << data::pp(d) << " is relevant for location " << X << "(" << print_vector(v, ", ") << ")" << std::endl;
      assert(X == X_i.X.name());
      // TODO: Ugly, v only contains control flow parameters, but the analysis
      // below gives indices into the list of parameters, so we need to map this
      std::vector<size_t> index_to_cfp_index;
      std::size_t idx = 0;
      for (std::size_t j = 0; j < X_i.X.parameters().size(); ++j)
      {
        if (is_global_control_flow_parameter(X, j))
        {
          index_to_cfp_index.push_back(idx);
          idx++;
        }
        else
        {
          index_to_cfp_index.push_back(data::undefined_index());
        }
      }

      bool result = true;
      std::size_t K = m_control_flow_graphs.size();
      for (std::size_t k = 0; k < K && result; k++)
      {
        const control_flow_graph& Gk = m_control_flow_graphs[k];
        const std::map<core::identifier_string, std::set<data::variable> >& Bk = m_belongs[k];
        std::map<core::identifier_string, std::set<data::variable> >::const_iterator i = Bk.find(X);
        if (i == Bk.end())
        {
          mCRL2log(log::debug1, "stategraph") << X << " " << d << " not found in graph " << k << std::endl;
          continue;
        }
        const std::set<data::variable>& V = i->second;
        if (utilities::detail::contains(V, d))
        {
          mCRL2log(log::debug1) << "    " << data::pp(d) << " belongs to graph " << k << std::endl;
          // determine m such that m_control_flow_index[X][m] == k
          // TODO: this information is not readily available, resulting in very ugly code...
          std::map<core::identifier_string, std::map<std::size_t, std::size_t> >::const_iterator ci = m_control_flow_index.find(X);
          assert(ci != m_control_flow_index.end());
#ifndef NDEBUG
          bool found = false;
#endif
          std::size_t m = data::undefined_index();
          const std::map<std::size_t, std::size_t>& M = ci->second;
          for (std::map<std::size_t, std::size_t>::const_iterator mi = M.begin(); mi != M.end(); ++mi)
          {
            if (mi->second == k)
            {
              assert(!found);
#ifndef NDEBUG
              found = true;
#endif
              m = mi->first;
            }
          }
          mCRL2log(log::debug1) << "    with parameter index " << m << " (CFP index " << index_to_cfp_index[m] << ")" << std::endl;
          assert(found);
          assert(m != data::undefined_index());
          assert(index_to_cfp_index[m] < v.size());

          control_flow_graph::vertex_const_iterator vi = Gk.find(propositional_variable_instantiation(X, atermpp::make_list(v[index_to_cfp_index[m]])));
          assert(vi != Gk.end());
          const stategraph_vertex& u = vi->second;
          mCRL2log(log::debug1) << "      found vertex " << pp(u.X) << " with marking " << print_vector(u.marking, ", ") << std::endl;

          result = result && utilities::detail::contains(u.marking, d);
          if(!result)
          {
            mCRL2log(log::debug1) << "    " << data::pp(d) << " is not contained in the marking for " << pp(u.X) << " so it is not relevant" << std::endl;
          }
        }
      }
      if(result)
      {
        mCRL2log(log::debug1) << "  " << data::pp(d) << " is relevant for location " << X << "(" << print_vector(v, ", ") << ")" << std::endl;
      }
      return result;
    }

    // returns true if in the control flow graph corresponding to d_X[j] there is a vertex u = X(e) such that
    // marking(u) is not empty
    bool has_non_empty_marking(const core::identifier_string& X, std::size_t j) const
    {
      std::size_t k = control_flow_index(X, j);
      if (k == data::undefined_index())
      {
        return false;
      }
      const control_flow_graph& Gk = m_control_flow_graphs[k];
      const std::set<stategraph_vertex*>& inst = Gk.index(X);
      for (std::set<stategraph_vertex*>::const_iterator i = inst.begin(); i != inst.end(); ++i)
      {
        stategraph_vertex& u = **i;
        if (!u.marking.empty())
        {
          return true;
        }
      }
      return false;
    }

  public:

    pbes_expression reset(const std::vector<data::data_expression>& v_prime,
                          const core::identifier_string& Y,
                          const predicate_variable& X_i,
                          const std::vector<std::size_t>& /* I */,
                          const std::vector<data::variable>& d_Y
                         )
    {
      data::data_expression c = data::sort_bool::true_();
      std::vector<data::data_expression> e_X(X_i.X.parameters().begin(), X_i.X.parameters().end());
      std::size_t k = 0;
      std::vector<data::data_expression> v;
      // Note that v needs to be built first, since it is used in its entirety
      // in the next loop!
      for (std::size_t j = 0; j < d_Y.size(); j++)
      {
        if (is_global_control_flow_parameter(Y, j))
        {
          if (X_i.dest.find(j) != X_i.dest.end())
          {
            v.push_back(X_i.dest.find(j)->second);
          }
          else if (!m_use_marking_optimization || has_non_empty_marking(Y, j))
          {
            assert(k < v_prime.size());
            v.push_back(v_prime[k]);
            k++;
          }
          else
          {
            v.push_back(e_X[j]);
          }
        }
      }

      std::vector<data::data_expression> r;
      k = 0;
      for (std::size_t j = 0; j < d_Y.size(); j++)
      {
        if (is_global_control_flow_parameter(Y, j))
        {
          if (X_i.dest.find(j) != X_i.dest.end())
          {
            c = data::lazy::and_(c, data::equal_to(e_X[j], X_i.dest.find(j)->second));
          }
          else if (!m_use_marking_optimization || has_non_empty_marking(Y, j))
          {
            // find the index that is copied to j
            c = data::lazy::and_(c, data::equal_to(e_X[j], v_prime[k]));
            k++;
          }
          r.push_back(e_X[j]);
        }
        else if (is_relevant(Y, X_i, d_Y[j], v))
        {
          r.push_back(e_X[j]);
        }
        else
        {
          r.push_back(default_value(e_X[j].sort()));
          //r.push_back(e_X[j]);
        }
      }

      propositional_variable_instantiation Yr(Y, atermpp::convert<data::data_expression_list>(r));
      pbes_expression result = Yr;
      if (m_simplify)
      {
        c = m_datar(c);
        if (c == data::sort_bool::true_())
        {
          result = Yr;
        }
        else if (c != data::sort_bool::false_())
        {
          result = imp(c, Yr);
        }
        else
        {
          result = data::sort_bool::true_();
        }
      }
      else
      {
        result = imp(c, Yr);
      }
      mCRL2log(log::debug1, "stategraph") << "Resetting " << pbes_system::pp(X_i.X) << " to " << pbes_system::pp(result) << std::endl;
      return result;
    }

    // expands a propositional variable instantiation using the control flow graph
    // x = Y(e)
    // Y(e) = PVI(phi_X, i)
    pbes_expression reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i);

    // Applies resetting of variables to the original PBES p.
    void reset_variables_to_original(pbes& p)
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

    local_reset_variables_algorithm(const pbes& p, data::rewriter::strategy rewrite_strategy = data::jitty,
                                    bool use_alternative_lcfp_criterion = false,
                                    bool use_alternative_gcfp_relation = false,
                                    bool use_alternative_gcfp_consistency = false
                                   )
      : stategraph_local_algorithm(p, rewrite_strategy, use_alternative_lcfp_criterion, use_alternative_gcfp_relation, use_alternative_gcfp_consistency),
        m_original_pbes(p)
    {}

    /// \brief Runs the stategraph algorithm
    /// \param simplify If true, simplify the resulting PBES
    /// \param apply_to_original_pbes Apply resetting variables to the original PBES instead of the STATEGRAPH one
    pbes run(bool simplify = true, bool use_marking_optimization = false)
    {
      super::run();
      m_simplify = simplify;
      m_use_marking_optimization = use_marking_optimization;
      pbes result = m_original_pbes;
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
    mCRL2log(log::debug1, "stategraph") << "reset variable " << pbes_system::pp(x) << " with index " << i << " to " << pbes_system::pp(result) << std::endl;
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

  void leave(const pbes_system::not_& /* x */)
  {
    pbes_expression operand = pop();
    push(not_(atermpp::aterm_cast<atermpp::aterm_appl>(operand)));
  }

  void leave(const pbes_system::and_& /* x */)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(and_(left, right));
  }

  void leave(const pbes_system::or_& /* x */)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(or_(left, right));
  }

  void leave(const pbes_system::imp& /* x */)
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

struct reset_variable_helper
{
  local_reset_variables_algorithm& algorithm;
  std::vector<pbes_expression>& phi;
  std::vector<data::data_expression>& v_prime;
  const core::identifier_string& Y;
  const predicate_variable& X_i;
  const std::vector<std::size_t>& I;
  const std::vector<data::variable>& d_Y;

  reset_variable_helper(local_reset_variables_algorithm& algorithm_,
                        std::vector<pbes_expression>& phi_,
                        std::vector<data::data_expression>& v_prime_,
                        const core::identifier_string& Y_,
                        const predicate_variable& X_i_,
                        const std::vector<std::size_t>& I_,
                        const std::vector<data::variable>& d_Y_
                       )
   : algorithm (algorithm_),
     phi       (phi_),
     v_prime   (v_prime_),
     Y         (Y_),
     X_i       (X_i_),
     I         (I_),
     d_Y       (d_Y_)
   {}

   void operator()()
   {
     phi.push_back(algorithm.reset(v_prime, Y, X_i, I, d_Y));
   }
};

inline
pbes_expression local_reset_variables_algorithm::reset_variable(const propositional_variable_instantiation& x, const stategraph_equation& eq_X, std::size_t i)
{
  mCRL2log(log::debug, "stategraph") << "--- resetting variable Y(e) = " << pbes_system::pp(x) << " with index " << i << std::endl;
  assert(i < eq_X.predicate_variables().size());
  const predicate_variable& X_i = eq_X.predicate_variables()[i];
  assert(X_i.X == x);

  std::vector<pbes_expression> phi;
  core::identifier_string Y = x.name();
  const stategraph_equation& eq_Y = *find_equation(m_pbes, Y);
  const std::vector<data::variable>& d_Y = eq_Y.parameters();
  assert(d_Y.size() == X_i.X.parameters().size());

  std::vector<std::size_t> I;
  for (std::size_t j = 0; j < d_Y.size(); j++)
  {
    if (is_global_control_flow_parameter(X_i.X.name(), j) && X_i.dest.find(j) == X_i.dest.end())
    {
      if (!m_use_marking_optimization || has_non_empty_marking(Y, j))
      {
        I.push_back(j);
      }
    }
  }
  mCRL2log(log::debug1, "stategraph") << "--- I = " << core::detail::print_container(I) << std::endl;

  std::vector<std::vector<data::data_expression> > values;
  for (auto ii = I.begin(); ii != I.end(); ++ii)
  {
    auto v = compute_values(Y, *ii);
    mCRL2log(log::debug1, "stategraph") << " values(" << *ii << ") = " << core::detail::print_container(v) << std::endl;
    values.push_back(v);
  }
  std::vector<data::data_expression> v_prime;
  for (auto vi = values.begin(); vi != values.end(); ++vi)
  {
    // assert(!vi->empty());
    if (vi->empty())
    {
      mCRL2log(log::debug, "stategraph") << "--- WARNING: empty values array in local_reset_variables_algorithm" << std::endl;
      return pbes_expr::true_();
    }
    v_prime.push_back(vi->front());
  }
  utilities::foreach_sequence(values, v_prime.begin(),
    // N.B. clang 3.0 segfaults on this lambda expression, so use a function object instead...
    // [&]()
    // {
    //   phi.push_back(reset(v_prime, Y, X_i, I, d_Y));
    // }
    reset_variable_helper(*this, phi, v_prime, Y, X_i, I, d_Y)
  );

  return pbes_expr::join_and(phi.begin(), phi.end());
}


} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_LOCAL_RESET_VARIABLES_H
