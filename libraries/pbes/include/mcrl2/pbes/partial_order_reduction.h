#include <utility>

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
#include "mcrl2/pbes/pbes_equation_index.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/srf_pbes.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace pbes_system {

// TODO: reuse this code
struct enumerator_error: public mcrl2::runtime_error
{
  explicit enumerator_error(const std::string& message): mcrl2::runtime_error(message)
  { }
};

// TODO: reuse this code
/// \brief The skip operation with a variable number of arguments
struct skip
{
  template<typename... Args>
  void operator()(const Args&...) const {}
};

struct summand_class
{
  data::variable_list e;
  pbes_expression f;
  data::data_expression_list g;
  std::vector<std::set<std::size_t>> nxt;
  std::vector<std::set<std::size_t>> NES; // TODO: use boost::dynamic_bitset<> (?)
  std::set<std::size_t> invis;

  summand_class() = default;

  summand_class(const data::variable_list& e_, const pbes_expression& f_, const data::data_expression_list& g_)
   : e(e_), f(f_), g(g_)
  {}
};

// The part of a summand used for determining equivalence classes
struct summand_equivalence_key
{
  data::variable_list e;
  pbes_expression f;
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

// TODO: reuse this code
template <typename VariableSequence, typename DataExpressionSequence>
inline
void add_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v, const DataExpressionSequence& e)
{
  assert(v.size() <= e.size());
  auto vi = v.begin();
  auto ei = e.begin();
  for (; vi != v.end(); ++vi, ++ei)
  {
    sigma[*vi] = *ei;
  }
}

// TODO: reuse this code
template <typename VariableSequence>
inline
void remove_assignments(data::mutable_indexed_substitution<>& sigma, const VariableSequence& v)
{
  for (const data::variable& vi: v)
  {
    sigma[vi] = vi;
  }
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

    struct accordance_pair
    {
      std::set<std::size_t> DNA;
      std::set<std::size_t> DNL;
      accordance_pair(std::set<std::size_t> DNA_, std::set<std::size_t> DNL_)
       : DNA(std::move(DNA_)), DNL(std::move(DNL_))
      {}
    };

    data::rewriter m_rewr;
    enumerate_quantifiers_rewriter m_pbes_rewr;
    data::enumerator_identifier_generator m_id_generator;
    data::enumerator_algorithm<enumerate_quantifiers_rewriter, data::rewriter> m_enumerator;
    pbes_equation_index m_equation_index;
    srf_pbes m_pbes;
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

    std::vector<accordance_pair> m_accordance;

    // j \in m_enabled[i] <=> ???
    std::vector<std::set<std::size_t>> m_enabled;

    // TODO: remove one of the attributes below
    std::set<std::size_t> m_invis; // invisible summand classes
    std::set<std::size_t> m_vis;   // visible summand classes

    std::set<std::size_t> en(const propositional_variable_instantiation& X_e)
    {
      std::set<std::size_t> result;
      std::size_t i = m_equation_index.index(X_e.name());
      const data::variable_list& d = m_pbes.equations()[i].variable().parameters();
      const data::data_expression_list& e = X_e.parameters();
      add_assignments(m_sigma, d, e);
      for (std::size_t k: m_enabled[i])
      {
        const summand_class& summand = m_summand_classes[k];
        const data::variable_list& e_k = summand.e;
        const pbes_expression& f_k = summand.f;
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
      // TODO: check this
      std::set<std::size_t> result;
      for (std::size_t k: K)
      {
        const summand_class& summand = m_summand_classes[k];
        result.insert(summand.invis.begin(), summand.invis.end());
      }
      return result;
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
            if (set_includes(Ts, m_accordance[k].DNA))
            {
              return Ts;
            }
          }
          std::size_t k = *T.begin(); // TODO: choose k according to D2t
          Twork = set_union(Twork, set_difference(m_accordance[k].DNA, Ts));
        }
        else
        {
          std::size_t k = *Twork.begin();
          Twork.erase(k);
          Ts.insert(k);
          if (contains(en_X_e, k))
          {
            Twork = set_union(Twork, set_difference(m_accordance[k].DNL, Ts));
            if (contains(m_vis, k))
            {
              Twork = set_union(Twork, m_vis);
            }
          }
          else
          {
            std::size_t i = 0; // TODO: choose i such that h is minimal
            Twork = set_union(Twork, m_summand_classes[k].NES[i]);
          }
        }
        C.insert(invis_pair(Twork, Ts));
      }
    }

    std::set<propositional_variable_instantiation> succ(const propositional_variable_instantiation& X_e, const std::set<std::size_t>& K)
    {
      std::set<propositional_variable_instantiation> result;
      std::size_t i = m_equation_index.index(X_e.name());
      for (std::size_t k: K)
      {
        const summand_class& summand = m_summand_classes[k];
        const data::variable_list& e_k = summand.e;
        const pbes_expression& f_k = summand.f;
        const data::data_expression_list& g_k = summand.g;
        const auto& J = summand.nxt[i];
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
        remove_assignments(m_sigma, e_k);
      }
      return result;
    }

    std::size_t index(const srf_summand& summand) const
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

    void compute_nxt()
    {
      std::size_t n = m_pbes.equations().size();
      for (std::size_t i = 0; i < n; i++)
      {
        const srf_equation& eqn = m_pbes.equations()[i];
        for (const srf_summand& summand: eqn.summands())
        {
          std::size_t j = m_equation_index.index(summand.variable().name());
          std::size_t k = index(summand);
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

    void compute_NES()
    {
      using utilities::detail::set_union;

      struct parameter_info
      {
        std::set<std::size_t> Ts; // test set
        std::set<std::size_t> Ws; // write set
        std::set<std::size_t> Rs; // read set
        std::set<std::size_t> Vs; // variable set
      };

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

      // compute NES

    }

    void compute_summand_classes()
    {
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
            m_summand_classes.emplace_back(summand.parameters(), summand.condition(), summand.variable().parameters());
          }
        }
      }
      compute_nxt();
      compute_NES();
    }

  public:
    explicit partial_order_reduction_algorithm(const pbes& p, data::rewrite_strategy strategy)
     : m_rewr(p.data(),
              data::used_data_equation_selector(p.data(), pbes_system::find_function_symbols(p), p.global_variables()),
              strategy),
       m_pbes_rewr(m_rewr, p.data()),
       m_enumerator(m_pbes_rewr, p.data(), m_rewr, m_id_generator, false),
       m_equation_index(p),
       m_pbes(pbes2srf(p))
    {
      // initialize m_parameters and m_parameter_positions
      const data::variable_list& parameters = m_pbes.equations().front().variable().parameters();
      m_parameters = std::vector<data::variable>{parameters.begin(), parameters.end()};
      for (std::size_t m = 0; m < m_parameters.size(); m++)
      {
        m_parameter_positions[m_parameters[m]] = m;
      }

      compute_summand_classes();
    }

    template <
      typename EmitNode = skip,
      typename EmitEdge = skip
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
        todo.erase(iter);
        seen.insert(X_e);
        std::set<std::size_t> stubborn_set_X_e = stubborn_set(X_e);
        std::set<std::size_t> en_X_e = en(X_e);
        std::set<propositional_variable_instantiation> next = succ(X_e, set_intersection(stubborn_set_X_e, en_X_e));
        if (!has_empty_intersection(next, todo))
        {
          next = set_union(next, succ(X_e, set_difference(en_X_e, stubborn_set_X_e)));
        }
        for (const propositional_variable_instantiation& Y_f: next)
        {
          if (contains(todo, Y_f) || contains(seen, Y_f))
          {
            continue;
          }
          emit_node(Y_f);
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
