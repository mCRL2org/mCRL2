// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file utilities.h

#ifndef MCRL2_PBESSYMBOLICBISIM_UTILITIES_H
#define MCRL2_PBESSYMBOLICBISIM_UTILITIES_H

#include "mcrl2/smt/solver.h"
#include "mcrl2/smt/translation_error.h"
#include "mcrl2/pbes/rewriters/one_point_rule_rewriter.h"

namespace mcrl2::data
{

data_expression make_abstraction(const binder_type&, const variable_list&, const data_expression&);
inline data_expression make_abstraction(const binder_type& b, const variable_list& vars, const data_expression& expr)
{
  return vars.empty() ? expr : abstraction(b, vars, expr);
}

template <typename Container>
data_expression make_application(const data_expression& func, const Container& args, typename atermpp::enable_if_container<Container, data_expression>::type* = nullptr)
{
  return args.empty() ? func : application(func, args);
}

struct data_manipulators
{
  rewriter rewr;
  rewriter proving_rewr;
  std::shared_ptr<smt::smt_solver> smt_solver;
  std::shared_ptr<std::map<pbes_system::propositional_variable, simplifier*>> simpl;
  bool contains_reals = false;

  data_manipulators() = default;

  data_manipulators(const rewriter& r, const rewriter& pr, const data_specification& s)
  : rewr(r)
  , proving_rewr(pr)
  , smt_solver(std::make_shared<smt::smt_solver>(s))
  , simpl(std::make_shared<std::map<pbes_system::propositional_variable, simplifier*>>())
  {}

  bool is_satisfiable(const variable_list& vars, const data_expression& expr) const
  {
    try
    {
      switch(smt_solver->solve(vars, expr))
      {
        case smt::answer::SAT: return true;
        case smt::answer::UNSAT: return false;
        // if UNKNOWN, we continue and try the rewriter
        default: ;
      }
    }
    catch(const smt::translation_error&)
    {}

    // The SMT solver failed, so we fallback to the rewriter
    data_expression is_sat = make_abstraction(exists_binder(), vars, expr);
    is_sat = rewr(one_point_rule_rewrite(quantifiers_inside_rewrite(is_sat)));
    if(contains_reals)
    {
      is_sat = rewr(replace_data_expressions(is_sat, fourier_motzkin_sigma(rewr), true));
    }
    if(is_sat != sort_bool::true_() && is_sat != sort_bool::false_())
    {
      throw mcrl2::runtime_error("Failed to establish whether " + data::pp(expr) + " is satisfiable");
    }
    return is_sat == sort_bool::true_();
  }
};

template <typename Element, typename Container = void>
class elements_iterator;

template <typename Element>
class elements_iterator<Element, Element>
{
protected:
  bool m_past_end;
  const Element* m_elem;

  bool is_past_end() const
  {
    return m_past_end;
  }

  void increment()
  {
    m_past_end = true;
  }

  void rollover() const {}

  void update_data(const Element& elem)
  {
    m_elem = &elem;
    m_past_end = false;
  }

public:
  elements_iterator()
  : m_past_end(true)
  , m_elem(nullptr)
  {}

  explicit elements_iterator(const Element& elem)
  : m_past_end(false)
  , m_elem(&elem)
  {}

  elements_iterator& operator++()
  {
    increment();
    return *this;
  }

  const Element& operator*() const
  {
    return *m_elem;
  }

  const Element* operator->() const
  {
    return m_elem;
  }

  bool operator==(const elements_iterator& other) const
  {
    return (m_past_end && other.m_past_end) || (!m_past_end && !other.m_past_end && m_elem == other.m_elem);
  }

  bool operator!=(const elements_iterator& other) const
  {
    return !(*this == other);
  }
};

template <typename Element, typename Container>
class elements_iterator: public elements_iterator<Element, typename Container::value_type>
{
  using value_type = Element;
  using reference = value_type&;
  using pointer = value_type*;
  using difference_type = ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

protected:
  using super = elements_iterator<Element, typename Container::value_type>;

  typename Container::const_iterator m_it;
  typename Container::const_iterator m_end;

  bool is_past_end() const
  {
    return m_it == m_end;
  }

  void increment()
  {
    super::increment();
  }

  void rollover()
  {
    super::rollover();
    while(!is_past_end() && super::is_past_end())
    {
      ++m_it;
      if(!is_past_end())
      {
        super::update_data(*m_it);
        super::rollover();
      }
    }
  }

  void update_data(const Container& list)
  {
    m_it = list.begin();
    m_end = list.end();
    if(m_it != m_end)
    {
      super::update_data(*m_it);
    }
  }

public:
  elements_iterator()
  : elements_iterator(Container())
  {}

  explicit elements_iterator(const Container& list)
  : elements_iterator(list.begin(), list.end())
  {}

  elements_iterator(const typename Container::const_iterator& begin, const typename Container::const_iterator& end)
  : m_it(begin)
  , m_end(end)
  {
    if(begin != end)
    {
      super::update_data(*begin);
    }
    rollover();
  }

  elements_iterator end()
  {
    return elements_iterator(m_end,m_end);
  }

  elements_iterator& operator++()
  {
    super::increment();
    rollover();
    return *this;
  }

  const value_type& operator*() const
  {
    return super::operator*();
  }

  const value_type* operator->() const
  {
    return super::operator->();
  }

  bool operator==(const elements_iterator& other) const
  {
    return (is_past_end() && other.is_past_end()) || (m_it == other.m_it && super::operator==(other));
  }

  bool operator!=(const elements_iterator& other) const
  {
    return !(*this == other);
  }
};

template < class BLOCK >
class split_cache
{
  using refinement_cache_t = std::unordered_set<std::pair<BLOCK, BLOCK>>;
  using transition_cache_t = std::unordered_map<std::pair<BLOCK, BLOCK>, bool>;

protected:
  /// If a pair of blocks is present here, it means that the first is stable wrt to the second
  refinement_cache_t m_refinement_cache;
  transition_cache_t m_transition_cache;

public:

  void insert_refinement(const BLOCK& b1, const BLOCK& b2)
  {
    m_refinement_cache.insert(std::make_pair(b1,b2));
  }

  /**
   * Returns true if b1 is certainly stable wrt b2
   * Returns false otherwise
   */
  bool check_refinement(const BLOCK& b1, const BLOCK& b2) const
  {
    return m_refinement_cache.find(std::make_pair(b1,b2)) != m_refinement_cache.end();
  }

  void insert_transition(const BLOCK& b1, const BLOCK& b2, const bool is_present)
  {
    m_transition_cache.insert(std::make_pair(std::make_pair(b1,b2), is_present));
  }

  typename transition_cache_t::const_iterator check_transition(const BLOCK& b1, const BLOCK& b2) const
  {
    return m_transition_cache.find(std::make_pair(b1,b2));
  }

  typename transition_cache_t::const_iterator transition_end() const
  {
    return m_transition_cache.cend();
  }

  void print_size() const
  {
    mCRL2log(log::verbose) << "Refinement cache size " << m_refinement_cache.size() << " transition cache size " << m_transition_cache.size() << std::endl;
  }

  template <typename Iterator>
  void replace_after_split(Iterator begin, const Iterator& end,
    const BLOCK& original, const BLOCK& new_block1, const BLOCK& new_block2)
  {
    for(; begin != end; ++begin)
    {
      const BLOCK& other = *begin;

      typename transition_cache_t::const_iterator find_result =
        m_transition_cache.find(std::make_pair(original, other));
      if(find_result != m_transition_cache.end() && !find_result->second)
      {
        // original has no transition to other.
        // We add the new blocks to the transition cache
        insert_transition(new_block1, other, false);
        insert_transition(new_block2, other, false);
      }
      m_transition_cache.erase(std::make_pair(original, other));

      find_result =
        m_transition_cache.find(std::make_pair(other, original));
      if(find_result != m_transition_cache.end() && !find_result->second)
      {
        // other has no transitions to original.
        // We add the new blocks to the transition cache
        insert_transition(other, new_block1, false);
        insert_transition(other, new_block2, false);
      }
      m_transition_cache.erase(std::make_pair(other, original));

      if(m_refinement_cache.find(std::make_pair(original, other)) != m_refinement_cache.end())
      {
        // original is stable wrt other, so new blocks (which are contained in original)
        // are also stable wrt other.
        // We add the new blocks to the refinement cache
        insert_refinement(new_block1, other);
        insert_refinement(new_block2, other);
        m_refinement_cache.erase(std::make_pair(original, other));
      }
      m_refinement_cache.erase(std::make_pair(other, original));
      // We now check whether other has transitions to the new blocks.
      // If not, other is also stable wrt the new block
      // if(m_refinement_cache.find(std::make_pair(other, new_block1)) != m_refinement_cache.end() &&
      //      !transition_exists(other, new_block1))
      // {
      //   m_refinement_cache.insert(std::make_pair(other, new_block1));
      // }
      // if(m_refinement_cache.find(std::make_pair(other, new_block2)) != m_refinement_cache.end() &&
      //      !transition_exists(other, new_block2))
      // {
      //   m_refinement_cache.insert(std::make_tuple(other, new_block2));
      // }
    }
    m_transition_cache.erase(std::make_pair(original, original));
  }
};

} // namespace mcrl2::data

#endif // MCRL2_PBESSYMBOLICBISIM_UTILITIES_H
