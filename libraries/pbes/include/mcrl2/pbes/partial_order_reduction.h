// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/partial_order_reduction.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_PARTIAL_ORDER_REDUCTION_H
#define MCRL2_PBES_PARTIAL_ORDER_REDUCTION_H

#include <boost/dynamic_bitset.hpp>
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/pbes/unify_parameters.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/skip.h"

namespace mcrl2 {

namespace pbes_system {

struct summand_class
{
  data::variable_list e;
  data::data_expression f;
  data::data_expression_list g;
  std::vector<std::set<std::size_t>> nxt;
  std::vector<std::set<std::size_t>> NES; // TODO: use boost::dynamic_bitset<> (?)
  std::set<std::size_t> DNA;
  std::set<std::size_t> DNL;

  summand_class() = default;

  // n is the number of PBES equations
  summand_class(data::variable_list  e_, data::data_expression f_, data::data_expression_list g_, std::size_t n)
   : e(std::move(e_)), f(std::move(f_)), g(std::move(g_))
  {
    nxt.resize(n);
    NES.resize(n);
  }

  // returns X_i -k->
  bool depends(std::size_t i) const
  {
    return !nxt[i].empty();
  }

  // returns X_i -k-> j
  bool depends(std::size_t i, std::size_t j) const
  {
    using utilities::detail::contains;

    return contains(nxt[i], j);
  }

  void print(std::ostream& out, const std::set<std::size_t>& s, const std::size_t N) const
  {
    using utilities::detail::contains;

    for (std::size_t i = 0; i < N; i++)
    {
      out << (contains(s, i) ? '1' : '0');
    }
  }

  // N is the number of summand classes
  void print(std::ostream& out, const std::size_t N) const
  {
    std::size_t n = nxt.size();

    for (std::size_t i = 0; i < n; i++)
    {
      out << "nxt " << std::setw(3) << i << " ";
      print(out, nxt[i], n);
      out << std::endl;
    }

    out << std::endl;

    for (std::size_t i = 0; i < n; i++)
    {
      out << "NES " << std::setw(3) << i << " ";
      print(out, NES[i], N);
      out << std::endl;
    }

    out << std::endl;
    out << "DNA = " << core::detail::print_set(DNA) << std::endl;
    out << "DNL = " << core::detail::print_set(DNL) << std::endl;
  }
};

// The part of a summand used for determining equivalence classes
struct summand_equivalence_key
{
  data::variable_list e;
  data::data_expression f;
  data::data_expression_list g;

  explicit summand_equivalence_key(const srf_summand& summand)
   : e(summand.parameters()), f(summand.condition()), g(summand.variable().parameters())
  {}

  bool operator<(const summand_equivalence_key& other) const
  {
    return std::tie(e, f, g) < std::tie(other.e, other.f, other.g);
  }

  bool operator==(const summand_equivalence_key& other) const
  {
    return std::tie(e, f, g) == std::tie(other.e, other.f, other.g);
  }
};

} // namespace pbes_system

} // namespace mcrl2

namespace std {

/// \brief specialization of the standard std::hash function.
template<>
struct hash<mcrl2::pbes_system::summand_equivalence_key>
{
  std::size_t operator()(const mcrl2::pbes_system::summand_equivalence_key& x) const
  {
    std::size_t seed = std::hash<atermpp::aterm>()(x.f);
    if (!x.e.empty())
    {
      seed = std::hash<atermpp::aterm>()(x.e) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    if (!x.g.empty())
    {
      seed = std::hash<atermpp::aterm>()(x.g) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

} // namespace std

namespace mcrl2 {

namespace pbes_system {

class partial_order_reduction_algorithm
{
  protected:
    using enumerator_element = data::enumerator_list_element_with_substitution<pbes_expression>;

    struct invis_pair
    {
      std::set<std::size_t> Twork;
      std::set<std::size_t> Ts;
      invis_pair(std::set<std::size_t> Twork_, std::set<std::size_t> Ts_)
       : Twork(std::move(Twork_)), Ts(std::move(Ts_))
      {}

      bool operator<(const invis_pair& other) const
      {
        return std::tie(Twork, Ts) < std::tie(other.Twork, other.Ts);
      }
    };

    struct parameter_info
    {
      std::set<std::size_t> Ts; // test set
      std::set<std::size_t> Ws; // write set
      std::set<std::size_t> Rs; // read set
      std::set<std::size_t> Vs; // variable set
    };

    data::rewriter m_rewr;
    enumerate_quantifiers_rewriter m_pbes_rewr;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<enumerate_quantifiers_rewriter, data::rewriter> m_enumerator;
    srf_pbes m_pbes;
    pbes_equation_index m_equation_index;
    data::mutable_indexed_substitution<> m_sigma;

    // the parameters of the PBES equations
    std::vector<data::variable> m_parameters;

    // maps parameters to their corresponding index
    std::map<data::variable, std::size_t> m_parameter_positions;

    // maps summands to the index of the corresponding summand class
    std::unordered_map<summand_equivalence_key, std::size_t> m_summand_index;

    // X_j \in nxt_k(X_i) <=> j \in m_summand_classes[k].nxt[i]
    // (X_i |- k -> X_j) <=> m_summand_classes[k].NES[i][j]
    std::vector<summand_class> m_summand_classes;

    std::set<std::size_t> m_invis; // invisible summand classes
    std::set<std::size_t> m_vis;   // visible summand classes

    std::size_t summand_index(const srf_summand& summand) const
    {
      auto i = m_summand_index.find(summand_equivalence_key(summand));
      assert(i != m_summand_index.end());
      return i->second;
    }

    std::size_t parameter_position(const data::variable& v) const
    {
      auto i = m_parameter_positions.find(v);
      return i->second;
    }

    const std::set<std::size_t>& DNA(std::size_t k) const
    {
      return m_summand_classes[k].DNA;
    }

    std::set<std::size_t>& DNA(std::size_t k)
    {
      return m_summand_classes[k].DNA;
    }

    const std::set<std::size_t>& DNL(std::size_t k) const
    {
      return m_summand_classes[k].DNL;
    }

    std::set<std::size_t>& DNL(std::size_t k)
    {
      return m_summand_classes[k].DNL;
    }

    std::set<std::size_t> en(const propositional_variable_instantiation& X_e)
    {
      std::size_t N = m_summand_classes.size();

      std::set<std::size_t> result;
      std::size_t i = m_equation_index.index(X_e.name());
      const data::variable_list& d = m_pbes.equations()[i].variable().parameters();
      const data::data_expression_list& e = X_e.parameters();
      add_assignments(m_sigma, d, e);
      for (std::size_t k = 0; k < N; k++)
      {
        if (!depends(i, k))
        {
          continue;
        }
        const summand_class& summand_k = m_summand_classes[k];
        const data::variable_list& e_k = summand_k.e;
        const pbes_expression& f_k = summand_k.f;
        m_enumerator.enumerate(enumerator_element(e_k, f_k),
                               m_sigma,
                               [&](const enumerator_element&) {
                                 result.insert(k);
                                 return false;
                               },
                               pbes_system::is_false
        );
        remove_assignments(m_sigma, e_k);
      }
      remove_assignments(m_sigma, d);
      return result;
    }

    std::set<std::size_t> invis(const std::set<std::size_t>& K)
    {
      return utilities::detail::set_intersection(m_invis, K);
    }

    // Choose a NES according to the heuristic function h.
    std::size_t choose_minimal_NES(std::size_t k,
                                   const std::set<std::size_t>& Twork,
                                   const std::set<std::size_t>& Ts,
                                   const std::set<std::size_t>& en_X_e
                                  ) const
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      std::size_t n = m_pbes.equations().size();
      std::set<std::size_t> Twork_Ts = set_union(Twork, Ts);
      const summand_class& summand_k = m_summand_classes[k];

      std::set<std::size_t> T1 = set_union(Twork_Ts, en_X_e);
      std::set<std::size_t> T2 = set_intersection(Twork_Ts, en_X_e);

      auto h = [&](std::size_t i)
      {
        const std::set<std::size_t> NES_k = summand_k.NES[i];
        return set_difference(NES_k, T1).size() + n * set_difference(NES_k, T2).size();
      };

      std::size_t i_min = 0;
      std::size_t h_min = h(0);
      for (std::size_t i = 1; i < n; i++)
      {
        std::size_t h_i = h(i);
        if (h_i < h_min)
        {
          i_min = i;
          h_min = h_i;
        }
      }
      return i_min;
    }

    std::set<std::size_t> stubborn_set(const propositional_variable_instantiation& X_e)
    {
      using utilities::detail::contains;
      using utilities::detail::has_empty_intersection;
      using utilities::detail::set_difference;
      using utilities::detail::set_includes;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      std::set<std::size_t> en_X_e = en(X_e);
      // return en_X_e;

      auto size = [&](const invis_pair& p)
      {
        return std::count_if(en_X_e.begin(), en_X_e.end(), [&](std::size_t k) { return contains(p.Twork, k) || contains(p.Ts, k); });
      };

      auto choose_minimum_element = [&](std::set<invis_pair>& C)
      {
        auto i = std::min_element(C.begin(), C.end(),
                                  [&](const invis_pair& x, const invis_pair& y)
                                  {
                                    return size(x) < size(y);
                                  }
        );
        invis_pair result = *i;
        C.erase(i);
        return result;
      };

      if (has_empty_intersection(en_X_e, m_invis))
      {
        return en_X_e;
      }

      std::set<invis_pair> C;
      for (std::size_t k: invis(en_X_e))
      {
        C.insert(invis_pair(std::set<std::size_t>{k}, std::set<std::size_t>()));
      }
      assert(!C.empty());

      while (true)
      {
        auto p = choose_minimum_element(C);
        auto& Twork = p.Twork;
        auto& Ts = p.Ts;
        if (Twork.empty())
        {
          std::set<std::size_t> T = set_intersection(Ts, en_X_e);
          for (std::size_t k: T)
          {
            if (set_includes(Ts, DNA(k)))
            {
              return Ts;
            }
          }
          std::size_t k = *T.begin(); // TODO: choose k according to D2t
          Twork = set_union(Twork, set_difference(DNA(k), Ts));
        }
        else
        {
          std::size_t k = *Twork.begin();
          Twork.erase(k);
          Ts.insert(k);
          if (contains(en_X_e, k))
          {
            Twork = set_union(Twork, set_difference(DNL(k), Ts));
            if (contains(m_vis, k))
            {
              Twork = set_union(Twork, set_difference(m_vis, Ts));
            }
          }
          else
          {
            std::size_t i = choose_minimal_NES(k, Twork, Ts, en_X_e);
            Twork = set_union(Twork, set_difference(m_summand_classes[k].NES[i], Ts));
          }
        }
        C.insert(invis_pair(Twork, Ts));
      }
    }

    std::set<propositional_variable_instantiation> succ(const propositional_variable_instantiation& X_e, const std::set<std::size_t>& K)
    {
      const auto& d = m_parameters;
      const auto& e = X_e.parameters();
      data::add_assignments(m_sigma, d, e);

      std::set<propositional_variable_instantiation> result;
      std::size_t i = m_equation_index.index(X_e.name());
      for (std::size_t k: K)
      {
        const summand_class& summand_k = m_summand_classes[k];
        const data::variable_list& e_k = summand_k.e;
        const pbes_expression& f_k = summand_k.f;
        const data::data_expression_list& g_k = summand_k.g;
        const auto& J = summand_k.nxt[i];
        m_enumerator.enumerate(enumerator_element(e_k, f_k),
                               m_sigma,
                               [&](const enumerator_element& p) {
                                 p.add_assignments(e_k, m_sigma, m_rewr);
                                 data::data_expression_list g(g_k.begin(), g_k.end(), [&](const data::data_expression& x) { return m_rewr(x, m_sigma); });
                                 for (std::size_t j: J)
                                 {
                                   const core::identifier_string& X_j = m_pbes.equations()[j].variable().name();
                                   result.insert(propositional_variable_instantiation(X_j, g));
                                 }
                                 return false;
                               },
                               pbes_system::is_false
        );
        data::remove_assignments(m_sigma, e_k);
      }
      return result;
    }

    void compute_nxt()
    {
      std::size_t n = m_pbes.equations().size();
      for (std::size_t i = 0; i < n; i++)
      {
        const srf_equation& eqn = m_pbes.equations()[i];
        for (const srf_summand& summand: eqn.summands())
        {
          std::size_t j = m_equation_index.index(summand.variable().name());
          std::size_t k = summand_index(summand);
          m_summand_classes[k].nxt[i].insert(j);
        }
      }
    }

    // returns the indices of the parameters that occur freely in x
    std::set<std::size_t> FV(const pbes_expression& x) const
    {
      std::set<std::size_t> result;
      for (const data::variable& v: find_free_variables(x))
      {
        result.insert(parameter_position(v));
      }
      return result;
    }

    void compute_NES(const std::vector<parameter_info>& info)
    {
      using utilities::detail::set_union;
      using utilities::detail::has_empty_intersection;

      std::unordered_map<std::pair<std::size_t, std::size_t>, bool> TsWs_empty_intersection_cache;

      // returns true if Ts(k1) and Ws(k2) have an empty intersection
      auto TsWs_has_empty_intersection = [&](std::size_t k1, std::size_t k2)
      {
        auto key = std::make_pair(k1, k2);
        auto i = TsWs_empty_intersection_cache.find(key);
        if (i == TsWs_empty_intersection_cache.end())
        {
          bool value = has_empty_intersection(info[k1].Ts, info[k2].Ws);
          i = TsWs_empty_intersection_cache.insert(std::make_pair(key, value)).first;
        }
        return i->second;
      };

      std::size_t n = m_pbes.equations().size();
      std::size_t N = m_summand_classes.size();

      // compute NES
      for (std::size_t k = 0; k < N; k++)
      {
        summand_class& summand_k = m_summand_classes[k];
        for (std::size_t i = 0; i < n; i++)
        {
          std::set<std::size_t>& NES_i = summand_k.NES[i];
          if (summand_k.depends(i))
          {
            for (std::size_t k1 = 0; k1 < N; k1++)
            {
              if (!TsWs_has_empty_intersection(k, k1))
              {
                NES_i.insert(k1);
              }
            }
          }
          else
          {
            for (std::size_t k1 = 0; k1 < N; k1++)
            {
              const std::set<std::size_t>& J = m_summand_classes[k1].nxt[i];
              if (J.size() > 1 || (J.size() == 1 && *J.begin() != i))
              {
                NES_i.insert(k1);
              }
            }
          }
        }
      }
    }

    // returns X_i |--k--> X_j
    bool depends(std::size_t i, std::size_t k, std::size_t j) const
    {
      return m_summand_classes[k].depends(i, j);
    };

    // TODO: precompute this function
    // returns X_i |--k-->
    bool depends(std::size_t i, std::size_t k) const
    {
      std::size_t n = m_pbes.equations().size();
      for (std::size_t j = 0; j < n; j++)
      {
        if (depends(i, k, j))
        {
          return true;
        }
      }
      return false;
    };

    bool left_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::data_expression condition_k = summand_k.f;
      const data::data_expression condition_k1 = summand_k1.f;

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::variable_list combined_quantified_vars = summand_k.e + summand_k1.e;

      data::assignment_list assignments_k = data::make_assignment_list(parameters, summand_k.g);
      data::assignment_list assignments_k1 = data::make_assignment_list(parameters, summand_k1.g);

      data::data_expression antecedent = data::sort_bool::and_(condition_k1, data::where_clause(condition_k, assignments_k1));
      data::data_expression parameters_equal = data::sort_bool::true_();
      auto it_k = summand_k.g.begin();
      auto it_k1 = summand_k1.g.begin();
      while (it_k != summand_k.g.end())
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(data::where_clause(*it_k, assignments_k1), data::where_clause(*it_k1, assignments_k)));
        ++it_k; ++it_k1;
      }
      data::data_expression consequent =
        data::sort_bool::and_(
          data::sort_bool::and_(
            condition_k,
            data::where_clause(condition_k1, assignments_k)
          ),
          parameters_equal
        );
      data::data_expression condition = data::forall(parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // std::cout << "Left condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return m_rewr(condition) == data::sort_bool::true_();
    }

    bool square_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::data_expression condition_k = summand_k.f;
      const data::data_expression condition_k1 = summand_k1.f;

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::variable_list combined_quantified_vars = summand_k.e + summand_k1.e;

      data::assignment_list assignments_k = data::make_assignment_list(parameters, summand_k.g);
      data::assignment_list assignments_k1 = data::make_assignment_list(parameters, summand_k1.g);

      data::data_expression antecedent = data::sort_bool::and_(condition_k, condition_k1);
      data::data_expression parameters_equal = data::sort_bool::true_();
      auto it_k = summand_k.g.begin();
      auto it_k1 = summand_k1.g.begin();
      while (it_k != summand_k.g.end())
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(data::where_clause(*it_k, assignments_k1), data::where_clause(*it_k1, assignments_k)));
        ++it_k; ++it_k1;
      }
      data::data_expression consequent =
        data::sort_bool::and_(
          data::sort_bool::and_(
            data::where_clause(condition_k, assignments_k1),
            data::where_clause(condition_k1, assignments_k)
          ),
          parameters_equal
        );
      data::data_expression condition = data::forall(parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // std::cout << "Square condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return m_rewr(condition) == data::sort_bool::true_();
    }

    bool triangle_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::data_expression condition_k = summand_k.f;
      const data::data_expression condition_k1 = summand_k1.f;

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::variable_list combined_quantified_vars = summand_k.e + summand_k1.e;

      data::assignment_list assignments_k = data::make_assignment_list(parameters, summand_k.g);

      data::data_expression antecedent = data::sort_bool::and_(condition_k, condition_k1);
      data::data_expression parameters_equal = data::sort_bool::true_();
      for (const data::data_expression& gi: summand_k1.g)
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(gi, data::where_clause(gi, assignments_k)));
      }
      data::data_expression consequent = data::sort_bool::and_(data::where_clause(condition_k1, assignments_k), parameters_equal);
      data::data_expression condition = data::forall(parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // std::cout << "Triangle condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return m_rewr(condition) == data::sort_bool::true_();
    }

    bool left_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();

      for (std::size_t i = 0; i < n; i++)
      {
        for (std::size_t i1 = 0; i1 < n; i1++)
        {
          bool X_k1_X1 = depends(i, k1, i1);
          for (std::size_t i_prime = 0; i_prime < n; i_prime++)
          {
            bool X1_k_Xprime = depends(i1, k, i_prime);
            if (X_k1_X1 && X1_k_Xprime)
            {
              bool found = false;
              for (std::size_t i2 = 0; i2 < n; i2++)
              {
                bool X_k_X2 = depends(i, k, i2);
                bool X2_k1_Xprime = depends(i2, k1, i_prime);
                if (X_k_X2 && X2_k1_Xprime)
                {
                  found = true;
                }
              }
              if (!found)
              {
                return false;
              }
            }
          }
        }
      }
      return true;
    }

    bool square_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();

      for (std::size_t i = 0; i < n; i++)
      {
        for (std::size_t i1 = 0; i1 < n; i1++)
        {
          bool X_k1_X1 = depends(i, k1, i1);
          for (std::size_t i2 = 0; i2 < n; i2++)
          {
            bool X_k_X2 = depends(i, k, i2);
            if (X_k1_X1 && X_k_X2)
            {
              bool found = false;
              for (std::size_t i_prime = 0; i_prime < n; i_prime++)
              {
                bool X1_k_Xprime = depends(i1, k, i_prime);
                bool X2_k1_Xprime = depends(i2, k1, i_prime);
                if (X1_k_Xprime && X2_k1_Xprime)
                {
                  found = true;
                  break;
                }
              }
              if (!found)
              {
                return false;
              }
            }
          }
        }
      }
      return true;
    }

    bool triangle_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();

      for (std::size_t i = 0; i < n; i++)
      {
        for (std::size_t i1 = 0; i1 < n; i1++)
        {
          bool X_k1_X1 = depends(i, k1, i1);
          for (std::size_t i2 = 0; i2 < n; i2++)
          {
            bool X_k_X2 = depends(i, k, i2);
            bool X2_k1_X1 = depends(i2, k1, i1);
            if (X_k1_X1 && X_k_X2 && !X2_k1_X1)
            {
              return false;
            }
          }
        }
      }
      return true;
    }

    void compute_DNA_DNL(const std::vector<parameter_info>& info)
    {
      using utilities::detail::has_empty_intersection;
      using utilities::detail::set_includes;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      std::size_t N = m_summand_classes.size();

      auto Rs = [&](const std::size_t k) { return info[k].Rs; };
      auto Ts = [&](const std::size_t k) { return info[k].Ts; };
      auto Vs = [&](const std::size_t k) { return info[k].Vs; };
      auto Ws = [&](const std::size_t k) { return info[k].Ws; };

      for (std::size_t k = 0; k < N; k++)
      {
        for (std::size_t k1 = 0; k1 < N; k1++)
        {
          bool DNL_DNS_affect_sets = set_intersection(set_intersection(Vs(k), Vs(k1)), set_union(Ws(k), Ws(k1))).empty();
          bool DNT_affect_sets = has_empty_intersection(Ws(k), Rs(k1)) && has_empty_intersection(Ws(k), Ts(k1)) && set_includes(Ws(k), Ws(k1));

          bool left_accords     = (DNL_DNS_affect_sets || left_accords_data(k, k1))   && left_accords_equations(k, k1);
          bool square_accords   = (DNL_DNS_affect_sets || square_accords_data(k, k1)) && square_accords_equations(k, k1);
          bool triangle_accords = (DNT_affect_sets || triangle_accords_data(k, k1))   && triangle_accords_equations(k, k1);

          if (!left_accords)
          {
            DNL(k).insert(k1);
          }

          if (!square_accords && !triangle_accords)
          {
            DNA(k).insert(k1);
          }
        }
      }
    }

    void compute_NES_DNA_DNL()
    {
      using utilities::detail::set_union;

      std::size_t N = m_summand_classes.size();
      std::vector<parameter_info> info(N);
      const std::vector<data::variable>& d = m_parameters;

      auto compute_parameter_info = [&](summand_class& summand, parameter_info& info)
      {
        // compute Ts
        std::set<data::variable> FV = find_free_variables(summand.f);
        for (const data::variable& v: summand.e)
        {
          FV.erase(v);
        }
        for (const data::variable& v: FV)
        {
          info.Ts.insert(parameter_position(v));
        }

        // compute Ws and Rs
        auto gi = summand.g.begin();
        auto di = d.begin();
        for ( ; di != d.end(); ++di, ++gi)
        {
          if (*di != *gi)
          {
            std::size_t i = di - d.begin();
            info.Ws.insert(i);

            for (const data::variable& v: find_free_variables(*gi))
            {
              info.Rs.insert(parameter_position(v));
            }
          }
        }

        // compute Vs
        info.Vs = set_union(info.Ts, set_union(info.Ws, info.Rs));
      };

      for (std::size_t k = 0; k < N; k++)
      {
        compute_parameter_info(m_summand_classes[k], info[k]);
      }

      compute_NES(info);
      compute_DNA_DNL(info);
    }

    void compute_summand_classes()
    {
      std::size_t n = m_pbes.equations().size();

      for (const srf_equation& eqn: m_pbes.equations())
      {
        for (const srf_summand& summand: eqn.summands())
        {
          summand_equivalence_key key(summand);
          auto i = m_summand_index.find(summand_equivalence_key(summand));
          if (i == m_summand_index.end())
          {
            std::size_t k = m_summand_index.size();
            m_summand_index[key] = k;
            m_summand_classes.emplace_back(summand.parameters(), summand.condition(), summand.variable().parameters(), n);
          }
        }
      }
      compute_nxt();
      compute_NES_DNA_DNL();
    }

    void compute_vis_invis()
    {
      using utilities::detail::contains;

      std::size_t n = m_pbes.equations().size();
      std::size_t N = m_summand_classes.size();

      for (std::size_t i = 0; i < n; i++)
      {
        const srf_equation& eqn = m_pbes.equations()[i];
        const core::identifier_string& X_i = eqn.variable().name();
        bool op_i = eqn.is_conjunctive();
        std::size_t rank_i = m_equation_index.rank(X_i);

        for (const srf_summand& summand: eqn.summands())
        {
          const core::identifier_string& X_j = summand.variable().name();
          std::size_t j = m_equation_index.index(X_j);
          std::size_t rank_j = m_equation_index.rank(X_j);
          bool op_j = m_pbes.equations()[j].is_conjunctive();
          bool is_invisible = op_i == op_j && rank_i == rank_j;
          if (!is_invisible)
          {
            std::size_t k = summand_index(summand);
            m_vis.insert(k);
          }
        }
      }

      for (std::size_t k = 0; k < N; k++)
      {
        if (!contains(m_vis, k))
        {
          m_invis.insert(k);
        }
      }
    }

    std::string print_variables(const data::variable_list& v) const
    {
      std::ostringstream out;
      for (auto i = v.begin(); i != v.end(); ++i)
      {
        if (i != v.begin())
        {
          out << ", ";
        }
        out << *i << ": " << i->sort();
      }
      return out.str();
    }

    void print_summand(const srf_summand& summand, bool is_conjunctive) const
    {
      std::size_t k = summand_index(summand);
      std::cout << "   (" << k << ") ";
      if (!summand.parameters().empty())
      {
        std::cout << (is_conjunctive ? "forall " : "exists ") << print_variables(summand.parameters()) << ". ";
      }
      std::cout << summand.condition()
                << (is_conjunctive ? " => " : " && ")
                << summand.variable()
                << std::endl;
    }

    void print_pbes() const
    {
      std::cout << m_pbes.to_pbes() << std::endl;
      std::cout << "srf_pbes" << std::endl;
      for (const srf_equation& eqn: m_pbes.equations())
      {
        std::cout << eqn.symbol() << " " << eqn.variable() << " = " << (eqn.is_conjunctive() ? "conjunction" : "disjunction") << " of summands\n";
        for (const srf_summand& summand: eqn.summands())
        {
          print_summand(summand, eqn.is_conjunctive());
        }
        std::cout << std::endl;
      }
    }

    void print_summand_classes() const
    {
      using utilities::detail::contains;

      std::size_t N = m_summand_classes.size();
      for (std::size_t k = 0; k < N; k++)
      {
        const summand_class& summand = m_summand_classes[k];
        std::cout << "\n--- summand class " << k << " ---" << std::endl;
        std::cout << "visible = " << std::boolalpha << contains(m_vis, k) << "\n\n";
        summand.print(std::cout, N);
      }
    }

  public:
    explicit partial_order_reduction_algorithm(const pbes& p, data::rewrite_strategy strategy)
     : m_rewr(p.data(),
              //TODO temporarily disabled used_data_equation_selector so the rewriter can rewrite accordance conditions
              // data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()),
              strategy),
       m_pbes_rewr(m_rewr, p.data()),
       m_enumerator(m_pbes_rewr, p.data(), m_rewr, m_id_generator, false),
       m_pbes(pbes2srf(p)),
       m_equation_index(m_pbes)
    {
      unify_parameters(m_pbes);

      // initialize m_parameters and m_parameter_positions
      const data::variable_list& parameters = m_pbes.equations().front().variable().parameters();
      m_parameters = std::vector<data::variable>{parameters.begin(), parameters.end()};
      for (std::size_t m = 0; m < m_parameters.size(); m++)
      {
        m_parameter_positions[m_parameters[m]] = m;
      }

      compute_summand_classes();
      compute_vis_invis();

      std::cout << p << std::endl;
      print_pbes();
    }

    const propositional_variable_instantiation& initial_state() const
    {
      return m_pbes.initial_state();
    }

    const std::vector<data::variable>& parameters() const
    {
      return m_parameters;
    }

    const fixpoint_symbol& symbol(const core::identifier_string& X) const
    {
      std::size_t i = m_equation_index.index(X);
      return m_pbes.equations()[i].symbol();
    }

    void print() const
    {
      print_summand_classes();
    }

    template <
      typename EmitNode = utilities::skip,
      typename EmitEdge = utilities::skip
    >
    void explore(
      const propositional_variable_instantiation& X_init,
      EmitNode emit_node = EmitNode(),
      EmitEdge emit_edge = EmitEdge()
    )
    {
      using utilities::detail::contains;
      using utilities::detail::has_empty_intersection;
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      std::unordered_set<propositional_variable_instantiation> seen;
      std::unordered_set<propositional_variable_instantiation> todo{X_init};
      while (!todo.empty())
      {
        auto iter = todo.begin();
        propositional_variable_instantiation X_e = *iter;
        mCRL2log(log::debug) << "choose X_e = " << X_e << std::endl;
        std::size_t rank = m_equation_index.rank(X_e.name());
        std::size_t i = m_equation_index.index(X_e.name());
        bool is_conjunctive = m_pbes.equations()[i].is_conjunctive();
        emit_node(X_e, is_conjunctive, rank);
        seen.insert(X_e);
        std::set<std::size_t> stubborn_set_X_e = stubborn_set(X_e);
        mCRL2log(log::debug) << "stubborn_set(X_e) = " << core::detail::print_set(stubborn_set_X_e) << std::endl;
        std::set<std::size_t> en_X_e = en(X_e);
        std::set<propositional_variable_instantiation> next = succ(X_e, set_intersection(stubborn_set_X_e, en_X_e));
        mCRL2log(log::debug) << "next = " << core::detail::print_set(next) << std::endl;
        if (!has_empty_intersection(next, todo))
        {
          next = set_union(next, succ(X_e, set_difference(en_X_e, stubborn_set_X_e)));
        }
        todo.erase(iter);
        for (const propositional_variable_instantiation& Y_f: next)
        {
          if (contains(todo, Y_f) || contains(seen, Y_f))
          {
            continue;
          }
          todo.insert(Y_f);
        }
        for (const propositional_variable_instantiation& Y_f: next)
        {
          emit_edge(X_e, Y_f);
        }
      }
    }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_PARTIAL_ORDER_REDUCTION_H
