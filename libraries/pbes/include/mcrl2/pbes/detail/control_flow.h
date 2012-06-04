// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_H

#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/data/standard.h"
#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/simplify_quantifier_builder.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

// Adds some simplifications to simplify_rewrite_builder.
template <typename Term, typename DataRewriter, typename SubstitutionFunction = no_substitution>
struct control_flow_simplify_quantifier_builder: public pbes_system::detail::simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction>
{
  typedef pbes_system::detail::simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction> super;
  typedef SubstitutionFunction                                                                       argument_type;
  typedef typename super::term_type                                                                  term_type;
  typedef typename core::term_traits<term_type>::data_term_type                                      data_term_type;
  typedef typename core::term_traits<term_type>::data_term_sequence_type                             data_term_sequence_type;
  typedef typename core::term_traits<term_type>::variable_sequence_type                              variable_sequence_type;
  typedef typename core::term_traits<term_type>::propositional_variable_type                         propositional_variable_type;
  typedef core::term_traits<Term> tr;

  /// \brief Constructor.
  /// \param rewr A data rewriter
  control_flow_simplify_quantifier_builder(const DataRewriter& rewr)
    : simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction>(rewr)
  { }

  // replace !(y || z) by !y && !z
  // replace !(y && z) by !y || !z
  // replace !(y => z) by y || !z
  // replace y => z by !y || z
  term_type post_process(const term_type& x)
  {
    term_type result = x;
    if (tr::is_not(x))
    {
      term_type t = tr::not_arg(x);
      if (tr::is_and(t)) // x = !(y && z)
      {
        term_type y = utilities::optimized_not(tr::left(t));
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_and(y, z);
      }
      else if (tr::is_or(t)) // x = !(y || z)
      {
        term_type y = utilities::optimized_not(tr::left(t));
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (tr::is_imp(t)) // x = !(y => z)
      {
        term_type y = tr::left(t);
        term_type z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
    }
    else if (tr::is_imp(x)) // x = y => z
    {
      term_type y = utilities::optimized_not(tr::left(x));
      term_type z = tr::right(x);
      result = utilities::optimized_or(y, z);
    }
    return result;
  }

  // replace the data expression y != z by !(y == z)
  term_type visit_data_expression(const term_type& x, const data_term_type& d, SubstitutionFunction& sigma)
  {
    typedef core::term_traits<data::data_expression> tt;
    term_type result = super::visit_data_expression(x, d, sigma);
    data::data_expression t = result;
    if (data::is_not_equal_to_application(t)) // result = y != z
    {
      data::data_expression y = tt::left(t);
      data::data_expression z = tt::right(t);
      result = tr::not_(data::equal_to(y, z));
    }
    return post_process(result);
  }

  term_type visit_true(const term_type& x, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_true(x, sigma));
  }

  term_type visit_false(const term_type& x, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_false(x, sigma));
  }

  term_type visit_not(const term_type& x, const term_type& n, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_not(x, n, sigma));
  }

  term_type visit_and(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_and(x, left, right, sigma));
  }

  term_type visit_or(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_or(x, left, right, sigma));
  }

  term_type visit_imp(const term_type& x, const term_type& left, const term_type& right, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_imp(x, left, right, sigma));
  }

  term_type visit_forall(const term_type& x, const variable_sequence_type&  variables, const term_type&  expression, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_forall(x, variables, expression, sigma));
  }

  term_type visit_exists(const term_type& x, const variable_sequence_type&  variables, const term_type&  expression, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_exists(x, variables, expression, sigma));
  }

  term_type visit_propositional_variable(const term_type& x, const propositional_variable_type&  v, SubstitutionFunction& sigma)
  {
    return post_process(super::visit_propositional_variable(x, v, sigma));
  }
};

template <typename Term, typename DataRewriter>
class control_flow_simplifying_rewriter
{
  protected:
    DataRewriter m_rewriter;

  public:
    typedef typename core::term_traits<Term>::term_type term_type;
    typedef typename core::term_traits<Term>::variable_type variable_type;

    control_flow_simplifying_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    term_type operator()(const term_type& x) const
    {
      control_flow_simplify_quantifier_builder<Term, DataRewriter> r(m_rewriter);
      return r(x);
    }

    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      control_flow_simplify_quantifier_builder<Term, DataRewriter, SubstitutionFunction> r(m_rewriter);
      return r(x, sigma);
    }
};

/// \brief Algorithm class for the control_flow algorithm
class pbes_control_flow_algorithm
{
  public:
    struct vertex
    {
      core::identifier_string X;
      data::variable v;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(X) << ", " << data::pp(v);
        return out.str();
      }

      vertex(const core::identifier_string& X_, const data::variable& v_)
        : X(X_), v(v_)
      {}
    };

    struct edge
    {
      std::size_t i;
      std::size_t j;
      const vertex* source;
      const vertex* target;

      std::string print() const
      {
        std::ostringstream out;
        out << core::pp(source->X) << " -- " << i << ", " << j << " --> " << core::pp(target->X);
        return out.str();
      }

      edge(std::size_t i_, std::size_t j_, const vertex* source_, const vertex* target_)
        : i(i_), j(j_), source(source_), target(target_)
      {}
    };

    void print_graph() const
    {
      std::cout << "--- vertices ---\n";
      for (std::vector<vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        std::cout << i->print() << std::endl;
      }

      std::cout << "--- edges ---\n";
      for (std::vector<edge>::const_iterator i = m_edges.begin(); i != m_edges.end(); ++i)
      {
        std::cout << i->print() << std::endl;
      }
    }

  protected:
    std::vector<vertex> m_vertices;
    std::vector<edge> m_edges;

    // very inefficient
    std::vector<vertex>::const_iterator find_vertex(const core::identifier_string& X, const data::variable& v) const
    {
      for (std::vector<vertex>::const_iterator i = m_vertices.begin(); i != m_vertices.end(); ++i)
      {
        if (i->X == X && i->v == v)
        {
          return i;
        }
      }
      std::cout << "<error>" << core::pp(X) << " " << data::pp(v) << std::endl;
      return m_vertices.end();
    }

    propositional_variable find_propvar(const pbes<>& p, const core::identifier_string& X) const
    {
      const atermpp::vector<pbes_equation>& equations = p.equations();
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        if (i->variable().name() == X)
        {
          return i->variable();
        }
      }
      throw mcrl2::runtime_error("find_propvar failed!");
      return propositional_variable();
    }

    // extract the propositional variable instantiations from an expression of the form g => \/_j in J . X_j(e_j)
    std::vector<propositional_variable_instantiation> get_propvars(const pbes_expression& x)
    {
      std::vector<pbes_expression> v;
      pbes_expression y = x;
      if (is_imp(y))
      {
        y = imp(y).right();
      }
      split_or(y, v);

      std::vector<propositional_variable_instantiation> result;
      for (std::vector<pbes_expression>::iterator i = v.begin(); i != v.end(); ++i)
      {
        if (is_propositional_variable_instantiation(*i))
        {
          result.push_back(*i);
        }
      }
      return result;
    }

    /// \brief Splits a conjunction into a sequence of operands
    /// Given a pbes expression of the form p1 && p2 && ... && pn, this will yield a
    /// vector of the form [ p1, p2, ..., pn ], assuming that pi does not have a && as main
    /// function symbol. Both the 'data &&' and the 'pbes &&' are considered.
    /// \param expr A PBES expression
    /// \return A sequence of operands
    inline
    void split_and(const pbes_expression& expr, std::vector<pbes_expression>& result)
    {
      using namespace accessors;
      utilities::detail::split(expr, std::back_inserter(result), data_is_and, left, right);
    }

  public:

    /// \brief Runs the control_flow algorithm. The pbes \p is modified by the algorithm
    /// \param p A pbes
    /// \pre p is in PFNF format
    void run(pbes<>& P)
    {
      const atermpp::vector<pbes_equation>& equations = P.equations();

      // compute the vertices of the control graph
      for (atermpp::vector<pbes_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        core::identifier_string X = i->variable().name();
        data::variable_list Xparams = i->variable().parameters();
        for (data::variable_list::const_iterator j = Xparams.begin(); j != Xparams.end(); ++j)
        {
          m_vertices.push_back(vertex(X, *j));
        }
      }

      // compute the edges of the control graph
      for (atermpp::vector<pbes_equation>::const_iterator k = equations.begin(); k != equations.end(); ++k)
      {
        // we are considering the equation X(d_X) = phi
        propositional_variable X = k->variable();
        std::vector<data::variable> d_X(X.parameters().begin(), X.parameters().end());
        pbes_expression phi = k->formula();

        std::vector<pbes_expression> implications = detail::pfnf_implications(phi);
        for (std::size_t i = 0; i < implications.size(); i++)
        {
          std::vector<propositional_variable_instantiation> propvars = get_propvars(implications[i]);
          for (std::size_t j = 0; j < propvars.size(); j++)
          {
            propositional_variable_instantiation Y = propvars[j];
            std::vector<data::data_expression> Yparameters(Y.parameters().begin(), Y.parameters().end());
            propositional_variable Yvar = find_propvar(P, Y.name());
            std::vector<data::variable> d_Y(Yvar.parameters().begin(), Yvar.parameters().end());
            for (std::size_t p = 0; p < Yparameters.size(); p++)
            {
              std::set<data::variable> freevars = pbes_system::find_free_variables(Yparameters[p]);
              for (std::size_t m = 0; m < d_X.size(); m++)
              {
                if (std::find(freevars.begin(), freevars.end(), d_X[m]) != freevars.end())
                {
                  std::vector<vertex>::const_iterator source = find_vertex(X.name(), d_X[m]);
                  std::vector<vertex>::const_iterator target = find_vertex(Y.name(), d_Y[p]);
                  edge e(i, j, &(*source), &(*target));
                  m_edges.push_back(e);
                }
              }
            }
          }
        }
      }
    }

    // simplify and rewrite the expression x
    pbes_expression simplify(const pbes_expression& x) const
    {
      data::detail::simplify_rewriter r;
      control_flow_simplifying_rewriter<pbes_expression, data::detail::simplify_rewriter> R(r);
      return R(x);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_H
