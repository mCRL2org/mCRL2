// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_source.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_SOURCE_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_SOURCE_H

#include <algorithm>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/detail/is_pfnf.h"
#include "mcrl2/pbes/detail/pfnf_pbes.h"
#include "mcrl2/pbes/detail/simplify_quantifier_builder.h"
#include "mcrl2/pbes/detail/control_flow_influence.h"
#include "mcrl2/pbes/detail/control_flow_utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the control_flow algorithm
class control_flow_source_algorithm
{
  public:
    control_flow_source_algorithm(pfnf_pbes& p)
      : m_pbes(p)
    {}

  protected:
    // the pbes that is considered
    pfnf_pbes& m_pbes;

    // maps X[i] to the corresponding source
    std::vector<std::vector<data::mutable_map_substitution<> > > m_source;

    void split_and(const pbes_expression& expr, std::vector<pbes_expression>& result) const
    {
      namespace a = combined_access;
      utilities::detail::split(expr, std::back_inserter(result), a::is_and, a::left, a::right);
    }

    // Simplifies the pbes expression x, and extracts all conjuncts d[i] == e from it, for some i in 0 ... d.size(), and with e a constant.
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
            data::data_expression left = data::application(v_i).left();
            data::data_expression right = data::application(v_i).right();
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

    // computes the source function for a pbes equation
    // source[i] contains the source parameters of g_i, represented in the form of a substitution
    template <typename DataRewriter>
    void compute_source(const pfnf_equation& eqn,
                        std::vector<data::mutable_map_substitution<> >& source,
                        DataRewriter rewr
                       ) const
    {
      typedef core::term_traits<pbes_expression> tr;
      typedef core::term_traits<data::data_expression> tt;

      const std::vector<data::variable>& d_X = eqn.parameters();
      const pbes_expression& h = eqn.h();
      const std::vector<pfnf_implication>& g = eqn.implications();

      data::mutable_map_substitution<> sigma_h;
      find_equality_conjuncts(h, d_X, sigma_h);

      for (std::vector<pfnf_implication>::const_iterator i = g.begin(); i != g.end(); ++i)
      {
        // compute source
        data::mutable_map_substitution<> sigma = sigma_h;
        find_equality_conjuncts(i->g(), d_X, sigma);
        source.push_back(sigma);
      }
    }

    void print_source(const pfnf_equation& eqn, const std::vector<data::mutable_map_substitution<> >& src) const
    {
      propositional_variable X(eqn.variable().name(), data::variable_list(eqn.parameters().begin(), eqn.parameters().end()));
      std::cout << "- predicate variable " << pbes_system::pp(X) << std::endl;

      std::cout << "h     = " << pbes_system::pp(eqn.h()) << std::endl;

      const std::vector<pfnf_implication>& g = eqn.implications();
      for (std::size_t i = 0; i < src.size(); i++)
      {
        const data::mutable_map_substitution<>& sigma = src[i];
        std::cout << "g[" << std::setw(2) << (i + 1) << "] = " << g[i] << std::endl;
        std::cout << "        source = " << data::print_substitution(sigma) << std::endl;
        std::cout << std::endl;
      }
    }

    data::data_expression source(std::size_t k, std::size_t i, std::size_t n) const
    {
      const data::mutable_map_substitution<>& sigma = m_source[k][i];
      data::variable d_n = m_pbes.equations()[k].parameters()[n];
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

  public:

    // rewrite the parameters of the propositional variables, using the substitutions of the source function
    void compute_source()
    {
      data::rewriter rewr(m_pbes.data());
      const std::vector<pfnf_equation>& equations = m_pbes.equations();

      for (std::vector<pfnf_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        std::vector<data::mutable_map_substitution<> > source;
        compute_source(*i, source, rewr);
        m_source.push_back(source);
      }
    }

    void print_source() const
    {
      std::size_t N = m_pbes.equations().size();
      for (std::size_t i = 0; i < N; i++)
      {
        print_source(m_pbes.equations()[i], m_source[i]);
      }
    }

    // rewrite the parameters of the propositional variables, using the substitutions of the source function
    // N.B. Modifies the pbes!
    void rewrite_propositional_variables()
    {
      data::rewriter r(m_pbes.data());
      pbes_system::data_rewriter<pbes_expression, data::rewriter> R(r);

      std::vector<pfnf_equation>& equations = m_pbes.equations();
      for (std::size_t k = 0; k < equations.size(); k++)
      {
        const std::vector<data::mutable_map_substitution<> >& src = m_source[k];
        std::vector<pfnf_implication>& implications = equations[k].implications();

        for (std::size_t i = 0; i < implications.size(); i++)
        {
          const data::mutable_map_substitution<>& sigma = src[i];
          std::vector<propositional_variable_instantiation>& v = implications[i].variables();
          for (std::vector<propositional_variable_instantiation>::iterator j = v.begin(); j != v.end(); ++j)
          {
            propositional_variable_instantiation& Xij = *j;
            Xij = pbes_system::rewrite(Xij, R, sigma);
          }
        }
      }
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_SOURCE_H
