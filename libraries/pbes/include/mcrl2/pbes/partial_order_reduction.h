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

#include <iomanip>
#include <boost/dynamic_bitset.hpp>
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/data/rewriters/quantifiers_inside_rewriter.h"
#include "mcrl2/data/substitution_utility.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/substitutions/maintain_variables_in_rhs.h"
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/replace_capture_avoiding_with_an_identifier_generator.h"
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
  std::set<std::size_t> NES; // TODO: use boost::dynamic_bitset<> (?)
  std::set<std::size_t> DNA;
  std::set<std::size_t> DNS;
  std::set<std::size_t> DNL;
  bool is_deterministic = false;

  summand_class() = default;

  // n is the number of PBES equations
  summand_class(data::variable_list  e_, data::data_expression f_, data::data_expression_list g_, std::size_t n)
   : e(std::move(e_)), f(std::move(f_)), g(std::move(g_))
  {
    nxt.resize(n);
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
    out << "deterministic = " << std::boolalpha << is_deterministic << std::endl;
    out << "NES = " << core::detail::print_set(NES) << std::endl;
    out << "DNA = " << core::detail::print_set(DNA) << std::endl;
    out << "DNS = " << core::detail::print_set(DNS) << std::endl;
    out << "DNL = " << core::detail::print_set(DNL) << std::endl;
  }
};

// The part of a summand used for determining equivalence classes
struct summand_equivalence_key
{
  data::variable_list e;
  data::data_expression f;
  data::data_expression_list g;

  explicit summand_equivalence_key(data::variable_list e_, data::data_expression f_, data::data_expression_list g_)
  : e(std::move(e_)), f(std::move(f_)), g(std::move(g_))
  {}

  explicit summand_equivalence_key(const summand_class& summand)
   : e(summand.e), f(summand.f), g(summand.g)
  {}

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

enum tribool
{
  no, maybe, yes
};
static inline bool operator&&(tribool a, tribool b)
{
  return a == yes || b == yes || (a == maybe && b == maybe);
}
// Short-circuit version of the operator && for tribools
static inline bool operator&&(std::function<tribool()> a, std::function<tribool()> b)
{
  tribool a_ = a();
  if(a_ == yes)
  {
    return true;
  }
  return a_ && b();
}

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
    std::size_t m_largest_equation_size = 0;

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

    // One NES for every predicate variable X_i that can be used for summand
    // class k when !depends(i,k).
    std::vector<std::set<std::size_t>> m_dependency_nes;

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

    const std::set<std::size_t>& DNS(std::size_t k) const
    {
      return m_summand_classes[k].DNS;
    }

    std::set<std::size_t>& DNS(std::size_t k)
    {
      return m_summand_classes[k].DNS;
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
    const std::set<std::size_t>& choose_minimal_NES(std::size_t k,
                                   const propositional_variable_instantiation& X_e,
                                   const std::set<std::size_t>& /* Twork */,
                                   const std::set<std::size_t>& /* Ts */,
                                   const std::set<std::size_t>& /* en_X_e */
                                  ) const
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      if(!depends(m_equation_index.index(X_e.name()), k))
      {
        return m_dependency_nes[m_equation_index.index(X_e.name())];
      }
      else
      {
        return m_summand_classes[k].NES;
      }

      //TODO implement one NES per guard, choose the smallest one
      // std::set<std::size_t> Twork_Ts = set_union(Twork, Ts);
      // const summand_class& summand_k = m_summand_classes[k];
      //
      // std::set<std::size_t> T1 = set_union(Twork_Ts, en_X_e);
      // std::set<std::size_t> T2 = set_intersection(Twork_Ts, en_X_e);
      //
      // auto h = [&](std::size_t i)
      // {
      //   const std::set<std::size_t> NES_k = summand_k.NES[i];
      //   return set_difference(NES_k, T1).size() + m_largest_equation_size * set_difference(NES_k, T2).size();
      // };
      //
      // std::size_t i_min = 0;
      // std::size_t h_min = h(0);
      // for (std::size_t i = 1; i < n; i++)
      // {
      //   std::size_t h_i = h(i);
      //   if (h_i < h_min)
      //   {
      //     i_min = i;
      //     h_min = h_i;
      //   }
      // }
      // return i_min;
    }

    const std::set<std::size_t>& DNX(std::size_t k,
                                   const std::set<std::size_t>& Twork,
                                   const std::set<std::size_t>& Ts,
                                   const std::set<std::size_t>& en_X_e
                                  ) const
    {
      using utilities::detail::set_difference;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      const summand_class& summand_k = m_summand_classes[k];
      if (!summand_k.is_deterministic)
      {
        return DNL(k);
      }

      std::set<std::size_t> Twork_Ts = set_union(Twork, Ts);

      std::set<std::size_t> T1 = set_union(Twork_Ts, en_X_e);
      std::set<std::size_t> T2 = set_intersection(Twork_Ts, en_X_e);

      auto h = [&](const std::set<std::size_t>& A)
      {
        return set_difference(A, T1).size() + m_largest_equation_size * set_difference(A, T2).size();
      };

      return h(DNS(k)) <= h(DNL(k)) ? DNS(k) : DNL(k);
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
            auto& DNS_or_DNL = DNX(k, Twork, Ts, en_X_e);
            Twork = set_union(Twork, set_difference(DNS_or_DNL, Ts));
            if (contains(m_vis, k))
            {
              Twork = set_union(Twork, set_difference(m_vis, Ts));
            }
          }
          else
          {
            auto& NES = choose_minimal_NES(k, X_e, Twork, Ts, en_X_e);
            Twork = set_union(Twork, set_difference(NES, Ts));
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

    bool is_true(const data::data_expression& expr) const
    {
      data::data_expression result = m_rewr(data::one_point_rule_rewrite(data::quantifiers_inside_rewrite(m_rewr(expr))));
      if (result != data::sort_bool::true_() && result != data::sort_bool::false_())
      {
        mCRL2log(log::verbose) << "Cannot rewrite " << result << " any further" << std::endl;
      }
      return result == data::sort_bool::true_();
    }

    bool summand_can_enable_data(const std::size_t k, const std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::set_identifier_generator id_gen;
      for(const data::variable& v: parameters)
      {
        id_gen.add_identifier(v.name());
      }

      summand_equivalence_key new_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars_k  = new_k.e;  data::data_expression condition_k  = new_k.f;
      data::variable_list qvars_k1 = new_k1.e; data::data_expression condition_k1 = new_k1.f; data::data_expression_list updates_k1 = new_k1.g;

      data::variable_list combined_quantified_vars = qvars_k + qvars_k1;

      data::assignment_list assignments_k1 = data::make_assignment_list(parameters, updates_k1);

      data::data_expression can_enable = make_abstraction(data::exists_binder(),
        parameters + combined_quantified_vars,
        data::sort_bool::and_(
          data::sort_bool::and_(
            data::sort_bool::not_(condition_k),
            condition_k1
          ),
          make_where_clause(condition_k, assignments_k1)
        )
      );

      return !is_true(data::sort_bool::not_(can_enable));
    }

    /// \brief Return true iff k1 can never happen after k happens, as deduced from
    /// predicate dependencies.
    bool permanently_disables(const std::size_t k, const std::size_t k1) const
    {
      std::size_t N = m_summand_classes.size();
      const summand_class& summand_k = m_summand_classes[k];
      std::set<std::size_t> reachable_after_k;

      // Check to which equations k can lead
      for (std::size_t i = 0; i < m_pbes.equations().size(); i++)
      {
        reachable_after_k.insert(summand_k.nxt[i].begin(), summand_k.nxt[i].end());
      }

      // Explore the rest of the dependency relation
      std::list<std::size_t> todo(reachable_after_k.begin(), reachable_after_k.end());
      while (!todo.empty())
      {
        std::size_t i = todo.front();
        todo.pop_front();
        for (std::size_t k2 = 0; k2 < N; k2++)
        {
          for (const std::size_t j: m_summand_classes[k2].nxt[i])
          {
            if (reachable_after_k.count(j) == 0)
            {
              todo.push_back(j);
              reachable_after_k.insert(j);
            }
          }
        }
      }

      return !std::any_of(reachable_after_k.begin(), reachable_after_k.end(),
        [&](const std::size_t i) { return depends(i, k1); });
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
        std::set<std::size_t>& NES = summand_k.NES;
        for (std::size_t k1 = 0; k1 < N; k1++)
        {
          if (!permanently_disables(k1, k) && !TsWs_has_empty_intersection(k, k1) && summand_can_enable_data(k, k1))
          {
            NES.insert(k1);
          }
        }
      }
      for (std::size_t i = 0; i < n; i++)
      {
        for (std::size_t k = 0; k < N; k++)
        {
          const std::set<std::size_t>& J = m_summand_classes[k].nxt[i];
          if (J.size() > 1 || (J.size() == 1 && *J.begin() != i))
          {
            m_dependency_nes[i].insert(k);
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

    summand_equivalence_key rename_duplicate_variables(data::set_identifier_generator& id_gen, const summand_equivalence_key& summ) const
    {
      std::vector<data::variable> new_variables;
      data::maintain_variables_in_rhs< data::mutable_map_substitution<> > sigma;
      for (const data::variable& var: summ.e)
      {
        core::identifier_string new_name = id_gen(var.name());
        if (new_name != var.name())
        {
          sigma[var] = data::variable(new_name, var.sort());
        }
        new_variables.emplace_back(new_name, var.sort());
      }

      auto replace_vars = [&](const data::data_expression& e)
      {
        return data::replace_variables_capture_avoiding_with_an_identifier_generator(e, sigma, id_gen);
      };

      return summand_equivalence_key(
        data::variable_list(new_variables.begin(), new_variables.end()),
        replace_vars(summ.f),
        data::data_expression_list(summ.g.begin(), summ.g.end(), replace_vars)
      );
    }

    data::data_expression make_abstraction(const data::binder_type& b, const data::variable_list& vars, const data::data_expression& expr) const
    {
      return vars.empty() ? expr : data::abstraction(b, vars, expr);
    }

    data::data_expression make_where_clause(const data::data_expression& expr, const data::assignment_list& al) const
    {
      return al.empty() ? expr : data::where_clause(expr, al);
    }

    tribool left_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::set_identifier_generator id_gen;
      for(const data::variable& v: parameters)
      {
        id_gen.add_identifier(v.name());
      }

      summand_equivalence_key new1_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new1_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars1_k  = new1_k.e;  data::data_expression condition1_k  = new1_k.f;  data::data_expression_list updates1_k  = new1_k.g;
      data::variable_list qvars1_k1 = new1_k1.e; data::data_expression condition1_k1 = new1_k1.f; data::data_expression_list updates1_k1 = new1_k1.g;
      data::assignment_list assignments1_k = data::make_assignment_list(parameters, updates1_k);
      data::assignment_list assignments1_k1 = data::make_assignment_list(parameters, updates1_k1);

      summand_equivalence_key new2_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new2_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars2_k  = new2_k.e;  data::data_expression condition2_k  = new2_k.f;  data::data_expression_list updates2_k  = new2_k.g;
      data::variable_list qvars2_k1 = new2_k1.e; data::data_expression condition2_k1 = new2_k1.f; data::data_expression_list updates2_k1 = new2_k1.g;

      data::variable_list combined_quantified_vars = qvars1_k + qvars1_k1;

      data::data_expression antecedent = data::sort_bool::and_(condition1_k1, make_where_clause(condition1_k, assignments1_k1));
      data::data_expression yes_condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::not_(antecedent));
      if (is_true(yes_condition))
      {
        return yes;
      }
      data::data_expression parameters_equal = data::sort_bool::true_();
      auto it_k = updates2_k.begin();
      auto it_k1 = updates2_k1.begin();
      while (it_k != updates2_k.end())
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(make_where_clause(*it_k, assignments1_k1), make_where_clause(*it_k1, assignments1_k)));
        ++it_k; ++it_k1;
      }
      data::data_expression consequent = make_abstraction(data::exists_binder(),
        qvars2_k + qvars2_k1,
        data::sort_bool::and_(
          data::sort_bool::and_(
            condition2_k,
            make_where_clause(condition2_k1, assignments1_k)
          ),
          parameters_equal
        )
      );
      data::data_expression condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // mCRL2log(log::verbose) << "Left condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return is_true(condition) ? maybe : no;
    }

    tribool square_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::set_identifier_generator id_gen;
      for(const data::variable& v: parameters)
      {
        id_gen.add_identifier(v.name());
      }

      summand_equivalence_key new1_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new1_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars1_k  = new1_k.e;  data::data_expression condition1_k  = new1_k.f;  data::data_expression_list updates1_k  = new1_k.g;
      data::variable_list qvars1_k1 = new1_k1.e; data::data_expression condition1_k1 = new1_k1.f; data::data_expression_list updates1_k1 = new1_k1.g;
      data::assignment_list assignments1_k = data::make_assignment_list(parameters, updates1_k);
      data::assignment_list assignments1_k1 = data::make_assignment_list(parameters, updates1_k1);

      summand_equivalence_key new2_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new2_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars2_k  = new2_k.e;  data::data_expression condition2_k  = new2_k.f;  data::data_expression_list updates2_k  = new2_k.g;
      data::variable_list qvars2_k1 = new2_k1.e; data::data_expression condition2_k1 = new2_k1.f; data::data_expression_list updates2_k1 = new2_k1.g;

      data::variable_list combined_quantified_vars = qvars1_k + qvars1_k1;


      data::data_expression antecedent = data::sort_bool::and_(condition1_k, condition1_k1);
      data::data_expression yes_condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::not_(antecedent));
      if (is_true(yes_condition))
      {
        return yes;
      }
      data::data_expression parameters_equal = data::sort_bool::true_();
      auto it_k = updates2_k.begin();
      auto it_k1 = updates2_k1.begin();
      while (it_k != updates2_k.end())
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(make_where_clause(*it_k, assignments1_k1), make_where_clause(*it_k1, assignments1_k)));
        ++it_k; ++it_k1;
      }
      data::data_expression consequent = make_abstraction(data::exists_binder(),
        qvars2_k + qvars2_k1,
        data::sort_bool::and_(
          data::sort_bool::and_(
            make_where_clause(condition2_k, assignments1_k1),
            make_where_clause(condition2_k1, assignments1_k)
          ),
          parameters_equal
        )
      );
      data::data_expression condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // mCRL2log(log::verbose) << "Square condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return is_true(condition) ? maybe : no;
    }

    tribool triangle_accords_data(std::size_t k, std::size_t k1) const
    {
      const summand_class& summand_k = m_summand_classes[k];
      const summand_class& summand_k1 = m_summand_classes[k1];

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::set_identifier_generator id_gen;
      for(const data::variable& v: parameters)
      {
        id_gen.add_identifier(v.name());
      }

      summand_equivalence_key new_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new_k1 = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k1));
      data::variable_list qvars_k  = new_k.e;  data::data_expression condition_k  = new_k.f;  data::data_expression_list updates_k  = new_k.g;
      data::variable_list qvars_k1 = new_k1.e; data::data_expression condition_k1 = new_k1.f; data::data_expression_list updates_k1 = new_k1.g;

      data::variable_list combined_quantified_vars = qvars_k + qvars_k1;

      data::assignment_list assignments_k = data::make_assignment_list(parameters, updates_k);

      data::data_expression antecedent = data::sort_bool::and_(condition_k, condition_k1);
      data::data_expression yes_condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::not_(antecedent));
      if (is_true(yes_condition))
      {
        return yes;
      }
      data::data_expression parameters_equal = data::sort_bool::true_();
      for (const data::data_expression& gi: updates_k1)
      {
        parameters_equal = data::lazy::and_(parameters_equal, data::equal_to(gi, make_where_clause(gi, assignments_k)));
      }
      data::data_expression consequent = data::sort_bool::and_(make_where_clause(condition_k1, assignments_k), parameters_equal);
      data::data_expression condition = make_abstraction(data::forall_binder(), parameters + combined_quantified_vars, data::sort_bool::implies(antecedent, consequent));

      // mCRL2log(log::verbose) << "Triangle condition for " << k << " and " << k1 << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return is_true(condition) ? maybe : no;
    }

    tribool left_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();
      tribool result = yes;

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
              result = maybe;
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
                return no;
              }
            }
          }
        }
      }
      return result;
    }

    tribool square_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();
      tribool result = yes;

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
              result = maybe;
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
                return no;
              }
            }
          }
        }
      }
      return result;
    }

    tribool triangle_accords_equations(std::size_t k, std::size_t k1) const
    {
      std::size_t n = m_pbes.equations().size();
      tribool result = yes;

      for (std::size_t i = 0; i < n; i++)
      {
        for (std::size_t i1 = 0; i1 < n; i1++)
        {
          bool X_k1_X1 = depends(i, k1, i1);
          for (std::size_t i2 = 0; i2 < n; i2++)
          {
            bool X_k_X2 = depends(i, k, i2);
            bool X2_k1_X1 = depends(i2, k1, i1);
            if (X_k1_X1 && X_k_X2)
            {
              result = maybe;
              if(!X2_k1_X1)
              {
                return no;
              }
            }
          }
        }
      }
      return result;
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
          if (k == k1)
          {
            continue;
          }
          bool DNL_DNS_affect_sets = has_empty_intersection(set_intersection(Vs(k), Vs(k1)), set_union(Ws(k), Ws(k1)));
          bool DNT_affect_sets = has_empty_intersection(Ws(k), Rs(k1)) && has_empty_intersection(Ws(k), Ts(k1)) && set_includes(Ws(k), Ws(k1));

          // Use lambda lifting for short-circuiting the && operator on tribools
          bool left_accords     = [&]{ return left_accords_equations(k, k1); }     && [&]{ return left_accords_data(k, k1); };
          bool square_accords   = [&]{ return square_accords_equations(k, k1); }   && [&]{ return square_accords_data(k, k1); };
          bool accords          = square_accords ||
                                  ([&]{ return triangle_accords_equations(k, k1); } && [&]{ return triangle_accords_data(k, k1); });

          if (!left_accords)
          {
            DNL(k).insert(k1);
          }
          if (!square_accords)
          {
            DNS(k).insert(k1);
          }
          if (!accords)
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

    bool compute_deterministic_equations(std::size_t k)
    {
      const summand_class& summand_k = m_summand_classes[k];

      std::size_t n = m_pbes.equations().size();
      for (std::size_t i = 0; i < n; i++)
      {
        if (summand_k.nxt[i].size() >= 2)
        {
          return false;
        }
      }
      return true;
    }

    bool compute_deterministic_data(std::size_t k)
    {
      const summand_class& summand_k = m_summand_classes[k];

      const data::variable_list& parameters = m_pbes.equations()[0].variable().parameters();
      data::set_identifier_generator id_gen;
      for(const data::variable& v: parameters)
      {
        id_gen.add_identifier(v.name());
      }

      summand_equivalence_key new1_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      summand_equivalence_key new2_k = rename_duplicate_variables(id_gen, summand_equivalence_key(summand_k));
      data::variable_list qvars1_k = new1_k.e; data::data_expression condition1_k = new1_k.f; data::data_expression_list updates1_k = new1_k.g;
      data::variable_list qvars2_k = new2_k.e; data::data_expression condition2_k = new2_k.f; data::data_expression_list updates2_k = new2_k.g;

      data::data_expression antecedent = data::sort_bool::and_(condition1_k, condition2_k);
      data::data_expression consequent = data::sort_bool::true_();
      auto it1_k = updates1_k.begin();
      auto it2_k = updates2_k.begin();
      while (it1_k != updates1_k.end())
      {
        consequent = data::lazy::and_(consequent, data::equal_to(*it1_k, *it2_k));
        ++it1_k; ++it2_k;
      }
      data::data_expression condition = make_abstraction(data::forall_binder(), parameters + qvars1_k + qvars2_k, data::sort_bool::implies(antecedent, consequent));

      // mCRL2log(log::verbose) << "Determinism condition for " << k << ": " << m_rewr(condition) << " original " << condition << std::endl;

      return is_true(condition);
    }

    void compute_deterministic()
    {
      std::size_t N = m_summand_classes.size();
      for (std::size_t k = 0; k < N; k++)
      {
        m_summand_classes[k].is_deterministic = compute_deterministic_equations(k) && compute_deterministic_data(k);
      }
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
      compute_deterministic();
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
      mCRL2log(log::verbose) << "   (" << k << ") ";
      if (!summand.parameters().empty())
      {
        mCRL2log(log::verbose) << (is_conjunctive ? "forall " : "exists ") << print_variables(summand.parameters()) << ". ";
      }
      mCRL2log(log::verbose) << summand.condition()
                << (is_conjunctive ? " => " : " && ")
                << summand.variable()
                << std::endl;
    }

    void print_pbes() const
    {
      mCRL2log(log::verbose) << m_pbes.to_pbes() << std::endl;
      mCRL2log(log::verbose) << "srf_pbes" << std::endl;
      for (const srf_equation& eqn: m_pbes.equations())
      {
        mCRL2log(log::verbose) << eqn.symbol() << " " << eqn.variable() << " = " << (eqn.is_conjunctive() ? "conjunction" : "disjunction") << " of summands\n";
        for (const srf_summand& summand: eqn.summands())
        {
          print_summand(summand, eqn.is_conjunctive());
        }
        mCRL2log(log::verbose) << std::endl;
      }
    }

    void print_summand_classes() const
    {
      using utilities::detail::contains;

      if(mCRL2logEnabled(log::verbose))
      {
        std::size_t N = m_summand_classes.size();
        for (std::size_t k = 0; k < N; k++)
        {
          const summand_class& summand = m_summand_classes[k];
          mCRL2log(log::verbose) << "\n--- summand class " << k << " ---" << std::endl;
          mCRL2log(log::verbose) << "visible = " << std::boolalpha << contains(m_vis, k) << "\n";
          summand.print(log::mcrl2_logger().get(log::verbose), N);
        }
        for (std::size_t i = 0; i < m_pbes.equations().size(); i++)
        {
          mCRL2log(log::verbose) << "dependency NES[" << std::setw(3) << i << "]  " << core::detail::print_set(m_dependency_nes[i]) << std::endl;
        }
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
       m_equation_index(m_pbes),
       m_dependency_nes(m_pbes.equations().size())
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

      // initialize m_largest_equation_size;
      for (std::size_t i = 0; i < m_pbes.equations().size(); i++)
      {
        m_largest_equation_size = std::max(m_largest_equation_size, m_pbes.equations()[i].summands().size());
      }

      mCRL2log(log::verbose) << p << std::endl;
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
      EmitEdge emit_edge = EmitEdge(),
      bool use_condition_L = true
    )
    {
      using utilities::detail::contains;
      using utilities::detail::has_empty_intersection;
      using utilities::detail::set_difference;
      using utilities::detail::set_includes;
      using utilities::detail::set_intersection;
      using utilities::detail::set_union;

      enum todo_state
      {
        NEW,            ///< Will be partially expanded
        DONE_PARTIALLY, ///< Has been partially expanded
        STARTS_CYCLE,   ///< Needs to be fully expanded, because it starts a cycle
        DONE            ///< Has been fully expanded
      };
      typedef std::pair<propositional_variable_instantiation, todo_state> todo_pair;

      std::unordered_set<propositional_variable_instantiation> seen;
      std::list<todo_pair> todo{todo_pair(X_init, NEW)};

      {
        std::size_t rank = m_equation_index.rank(X_init.name());
        std::size_t i = m_equation_index.index(X_init.name());
        bool is_conjunctive = m_pbes.equations()[i].is_conjunctive();
        emit_node(X_init, is_conjunctive, rank);
      }

      while (!todo.empty())
      {
        todo_pair& p = todo.back();
        const propositional_variable_instantiation X_e = p.first;
        todo_state& s = p.second;
        mCRL2log(log::debug) << "choose X_e = " << X_e << std::endl;

        if (s == DONE || s == DONE_PARTIALLY)
        {
          todo.pop_back();
          continue;
        }

        std::set<propositional_variable_instantiation> next;
        std::set<std::size_t> en_X_e = en(X_e);

        if (s == NEW)
        {
          std::set<std::size_t> stubborn_set_X_e = stubborn_set(X_e);
          mCRL2log(log::debug) << "stubborn_set(X_e) = " << core::detail::print_set(stubborn_set_X_e) << std::endl;
          next = succ(X_e, set_intersection(stubborn_set_X_e, en_X_e));

          bool fully_expanded = set_includes(stubborn_set_X_e, en_X_e);
          s = fully_expanded ? DONE : DONE_PARTIALLY;

          // Check if a cycle is closed
          // At the same time, check whether some node on the stack is fully expanded
          // If both are true, some node will be fully expanded
          bool cycle_found = false;
          auto& cycle_node = todo.front();
          bool fully_expanded_node_found = false;
          for (todo_pair& tp: todo)
          {
            if (contains(next, tp.first))
            {
              cycle_found = true;
              cycle_node = tp;
            }
            fully_expanded_node_found |= cycle_found && (tp.second == STARTS_CYCLE || tp.second == DONE);
          }
          if (use_condition_L && cycle_found && !fully_expanded_node_found)
          {
            cycle_node.second = STARTS_CYCLE;
          }
        }
        else
        {
          assert(s == STARTS_CYCLE);
          next = succ(X_e, en_X_e);
          s = DONE;
        }

        mCRL2log(log::debug) << "next = " << core::detail::print_set(next) << std::endl;
        for (const propositional_variable_instantiation& Y_f: next)
        {
          if (!contains(seen, Y_f))
          {
            std::size_t rank = m_equation_index.rank(Y_f.name());
            std::size_t i = m_equation_index.index(Y_f.name());
            bool is_conjunctive = m_pbes.equations()[i].is_conjunctive();
            emit_node(Y_f, is_conjunctive, rank);
            seen.insert(Y_f);
            todo.emplace_back(Y_f, NEW);
          }
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
