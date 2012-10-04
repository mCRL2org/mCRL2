// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/control_flow_destination.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_CONTROL_FLOW_DESTINATION_H
#define MCRL2_PBES_DETAIL_CONTROL_FLOW_DESTINATION_H

#include "mcrl2/pbes/detail/control_flow_source.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Algorithm class for the computation of the destination function and some functions depending on it
class control_flow_destination_algorithm: public control_flow_source_algorithm
{
  public:
    typedef atermpp::vector<data::data_expression> destination_array; // N.B. data_expression() represents a non-existent value in this vector
    typedef std::map<propositional_variable_instantiation, destination_array> destination_map;

    control_flow_destination_algorithm(pfnf_pbes& p)
      : control_flow_source_algorithm(p)
    {}

  protected:
    // maps X[i] to the corresponding destination
    std::vector<std::vector<destination_map> > m_destination;

    // computes the destination function for a pbes equation
    // dest[i] maps X_ij(f_ij) to a vector of data expressions
    template <typename DataRewriter>
    void compute_destination(const pfnf_equation& eqn,
                             std::vector<destination_map>& dest,
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
        data::mutable_map_substitution<> sigma = sigma_h;
        find_equality_conjuncts(i->g(), d_X, sigma);

        destination_map dmap;
        const std::vector<propositional_variable_instantiation> propvars = i->variables();
        for (std::vector<propositional_variable_instantiation>::const_iterator j = propvars.begin(); j != propvars.end(); ++j)
        {
          const propositional_variable_instantiation& Xij = *j;
          destination_array dij;
          data::data_expression_list e = Xij.parameters();
          for (data::data_expression_list::iterator k = e.begin(); k != e.end(); ++k)
          {
            data::data_expression e_k = rewr(*k, sigma);
            if (is_constant(e_k))
            {
              dij.push_back(e_k);
            }
            else
            {
              dij.push_back(data::data_expression());
            }
          }
          dmap[Xij] = dij;
        }
        dest.push_back(dmap);
      }
    }

    void print_destination_array(const propositional_variable_instantiation& X, const destination_array& a) const
    {
      std::cout << "        dest(" << pbes_system::pp(X) << ") = [";
      for (destination_array::const_iterator i = a.begin(); i != a.end(); ++i)
      {
        if (i != a.begin())
        {
          std::cout << ", ";
        }
        if (*i == data::data_expression())
        {
          std::cout << "-";
        }
        else
        {
          std::cout << data::pp(*i);
        }
      }
      std::cout << "]" << std::endl;
    }

    void print_destination(const pfnf_equation& eqn, const std::vector<data::mutable_map_substitution<> >& src, const std::vector<destination_map>& dest) const
    {
      propositional_variable X(eqn.variable().name(), data::variable_list(eqn.parameters().begin(), eqn.parameters().end()));
      std::cout << "- predicate variable " << pbes_system::pp(X) << std::endl;

      std::cout << "h     = " << pbes_system::pp(eqn.h()) << std::endl;

      const std::vector<pfnf_implication>& g = eqn.implications();
      for (std::size_t i = 0; i < src.size(); i++)
      {
        const destination_map& dmap = dest[i];
        std::cout << "g[" << std::setw(2) << (i + 1) << "] = " << g[i] << std::endl;
        for (destination_map::const_iterator j = dmap.begin(); j != dmap.end(); ++j)
        {
          print_destination_array(j->first, j->second);
        }
        std::cout << std::endl;
      }
    }

    // returns the constant value fij[n] of equation k, or data::data_expression() if it does not exist,
    // where fij is the j-th propositional variable of the i-th conjunct of the k-th equation
    data::data_expression destination(std::size_t k, std::size_t i, std::size_t j, std::size_t n) const
    {
      const propositional_variable_instantiation& Xij = m_pbes.equations()[k].implications()[i].variables()[j];
      const destination_map& dmap = m_destination[k][i];
      destination_map::const_iterator q = dmap.find(Xij);
      assert (q != dmap.end());
      return q->second[n];
    }

    data::data_expression copy(std::size_t k, std::size_t i, std::size_t j, std::size_t n) const
    {
      data::variable d_n = m_pbes.equations()[k].parameters()[n];
      std::size_t M = m_pbes.equations()[k].implications()[i].variables().size();
      for (std::size_t m = 0; m < M; m++)
      {
        if (destination(k, i, j, m) == d_n)
        {
          return d_n;
        }
      }
      return data::data_expression();
    }

    // returns true if parameter n of X[k] rules conjunct i
    bool is_rule_parameter(std::size_t k, std::size_t n, std::size_t i) const
    {
      const pfnf_equation& eqn = m_pbes.equations()[k];
      const pfnf_implication& g = eqn.implications()[i];
      const data::variable d_n = eqn.parameters()[n];
      if (source(k, i, n) == data::data_expression())
      {
        return false;
      }
      for (std::size_t j = 0; j < g.variables().size(); ++j)
      {
        if (destination(k, i, j, n) == data::data_expression())
        {
          return false;
        }
      }
      std::cout << "parameter " << data::pp(d_n) << " rules conjunct " << i << std::endl;
      return true;
    }

    // return true if parameter n of X[k] is a control flow parameter
    bool is_control_flow_parameter(std::size_t k, std::size_t n)
    {
      const pfnf_equation& eqn = m_pbes.equations()[k];
      for (std::size_t i = 0; i < eqn.implications().size(); i++)
      {
        const pfnf_implication& g = eqn.implications()[i];
        if (is_rule_parameter(k, n, i))
        {
          continue;
        }
        const std::vector<propositional_variable_instantiation>& propvars = g.variables();
        for (std::size_t j = 0; j < propvars.size(); j++)
        {
          const propositional_variable_instantiation& Xij = propvars[j];
          std::size_t M = Xij.parameters().size();
          std::set<data::data_expression> c;
          for (std::size_t m = 0; m < M; m++)
          {
            c.insert(copy(k, i, j, m));
          }
          if (c.size() > 1 || *c.begin() == data::data_expression())
          {
            return false;
          }
        }
      }
      return true;
    }

  public:

    // rewrite the parameters of the propositional variables, using the substitutions of the source function
    void compute_destination()
    {
      data::rewriter rewr(m_pbes.data());
      const std::vector<pfnf_equation>& equations = m_pbes.equations();

      for (std::vector<pfnf_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
      {
        std::vector<destination_map> dest;
        compute_destination(*i, dest, rewr);
        m_destination.push_back(dest);
      }
    }

    void print_destination() const
    {
      std::size_t N = m_pbes.equations().size();
      for (std::size_t i = 0; i < N; i++)
      {
        print_destination(m_pbes.equations()[i], m_source[i], m_destination[i]);
      }
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_CONTROL_FLOW_DESTINATION_H
