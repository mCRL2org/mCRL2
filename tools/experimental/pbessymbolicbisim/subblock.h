// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file subblock.h

#ifndef MCRL2_PBESSYMBOLICBISIM_SUBBLOCK_H
#define MCRL2_PBESSYMBOLICBISIM_SUBBLOCK_H

#include <list>

#include "mcrl2/utilities/hash_utility.h"

#include "utilities.h"

namespace mcrl2
{
namespace data
{

/**
 * \brief A subblock is a set of states containing only nodes
 * from one PBES variable
 */
class subblock
{
  typedef pbes_system::detail::ppg_summand summand_type_t;
  typedef pbes_system::detail::ppg_equation equation_type_t;

protected:
  // invariant: m_equation.variable() == m_var
  equation_type_t m_equation;

  pbes_system::propositional_variable m_var;
  data_expression m_char_func;

  const data_manipulators m_dm;
  split_cache<subblock>* m_cache;

public:

  subblock()
  : subblock(data_manipulators())
  {}

  explicit subblock(const data_manipulators& dm)
  : m_char_func(sort_bool::false_())
  , m_dm(dm)
  {}

  subblock(const equation_type_t& eq, const data_manipulators& dm, split_cache<subblock>* cache)
  : subblock(eq, make_abstraction(lambda_binder(), eq.variable().parameters(), sort_bool::true_()), dm, cache)
  {}

  subblock(const equation_type_t& eq, const data_expression& char_func, const data_manipulators& dm, split_cache<subblock>* cache)
  : m_equation(eq)
  , m_var(eq.variable())
  , m_char_func(char_func)
  , m_dm(dm)
  , m_cache(cache)
  {}

  pbes_system::propositional_variable variable() const
  {
    return m_var;
  }

  data_expression charachteristic_function() const
  {
    return m_char_func;
  }

  equation_type_t equation() const
  {
    return m_equation;
  }

  bool has_transition(const std::list<subblock>& others) const
  {
    return std::any_of(others.cbegin(), others.cend(), [this](const subblock& other){ return has_transition(other); });
  }

  bool has_transition(const subblock& other) const
  {
    const auto find_result = m_cache->check_transition(*this, other);
    if(find_result != m_cache->transition_end())
    {
      return find_result->second;
    }
    bool result = std::any_of(m_equation.summands().cbegin(), m_equation.summands().cend(), [&](const pbes_system::detail::ppg_summand& summ)
      {
        return summ.new_state().name() == other.m_var.name() &&
          m_dm.is_satisfiable(m_var.parameters() + summ.quantification_domain(),
            m_dm.rewr(sort_bool::and_(
              make_application(m_char_func, m_var.parameters()),
              sort_bool::and_(
                summ.condition(),
                m_dm.rewr(make_application(other.m_char_func, summ.new_state().parameters()))
              )
            ))
          );
      });
    m_cache->insert_transition(*this, other, result);
    return result;
  }

  bool is_empty()
  {
    return m_char_func == sort_bool::false_();
  }

  std::pair<subblock,subblock> split(const std::list<subblock>& others) const
  {
    if(!has_transition(others))
    {
      return std::make_pair(subblock(m_dm), *this);
    }
    if(std::all_of(others.begin(), others.end(), [this](const subblock& sb)
        {
          return m_cache->check_refinement(*this, sb);
        }))
    {
      return std::make_pair(*this, subblock(m_dm));
    }
    data_expression transition_exists = sort_bool::false_();
    for(const summand_type_t& cl: m_equation.summands())
    {
      // matching_subblocks expresses whether the summand cl
      // can be used to take a transition from this subblock to
      // some subblock in others
      data_expression matching_subblocks = sort_bool::false_();
      for(const subblock& sb: others)
      {
        if(sb.m_var.name() == cl.new_state().name())
        {
          matching_subblocks = sort_bool::or_(matching_subblocks, make_application(sb.m_char_func, cl.new_state().parameters()));
        }
      }
      if(matching_subblocks != sort_bool::false_())
      {
        transition_exists = sort_bool::or_(transition_exists,
          make_abstraction(exists_binder(), cl.quantification_domain(),
            sort_bool::and_(
              cl.condition(),
              matching_subblocks
            )
          ));
      }
    }
    transition_exists = one_point_rule_rewrite(quantifiers_inside_rewrite(transition_exists));
    if(m_dm.contains_reals)
    {
      transition_exists = replace_data_expressions(transition_exists, fourier_motzkin_sigma(m_dm.rewr), true);
    }
    transition_exists = one_point_rule_rewrite(m_dm.rewr(transition_exists));
    data_expression block2_body = m_dm.rewr(
        sort_bool::and_(
          make_application(m_char_func, m_equation.variable().parameters()),
          sort_bool::not_(transition_exists)
        ));
    if(!m_dm.is_satisfiable(m_equation.variable().parameters(), block2_body))
    {
      // There are no (X,v) without transitions in this block
      // Therefore, we cannot split
      return std::make_pair(*this, subblock(m_dm));
    }
    data_expression block2 =
      make_abstraction(lambda_binder(), m_equation.variable().parameters(),
        block2_body
      );
    block2 = m_dm.simpl->at(m_equation.variable())->apply(block2);
    data_expression block1 =
      make_abstraction(lambda_binder(), m_equation.variable().parameters(),
        sort_bool::and_(
          make_application(m_char_func, m_equation.variable().parameters()),
          transition_exists
        )
      );

    block1 = m_dm.simpl->at(m_equation.variable())->apply(block1);
    if(block1 == make_abstraction(lambda_binder(), m_equation.variable().parameters(), sort_bool::false_()))
    {
      throw mcrl2::runtime_error("Found an empty block1");
    }
    return std::make_pair(subblock(m_equation, block1, m_dm, m_cache), subblock(m_equation, block2, m_dm, m_cache));
  }

  bool contains_state(const pbes_system::propositional_variable_instantiation& state) const
  {
    return m_var.name() == state.name() && m_dm.rewr(application(m_char_func, state.parameters())) == sort_bool::true_();
  }

  bool operator==(const subblock& other) const
  {
    return m_var == other.m_var && m_char_func == other.m_char_func;
  }

  bool operator<(const subblock& other) const
  {
    return m_var < other.m_var || (m_var == other.m_var && m_char_func < other.m_char_func);
  }

  friend void swap(subblock& self, subblock& other)
  {
    using std::swap;
    swap(self.m_equation, other.m_equation);
    swap(self.m_var, other.m_var);
    swap(self.m_char_func, other.m_char_func);
  }

  subblock& operator=(subblock other)
  {
    swap(*this,other);
    return *this;
  }
};

std::string pp(const subblock&);
std::string pp(const subblock& b)
{
  std::ostringstream out;
  out << b.variable().name() << " - " << b.charachteristic_function();
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const subblock& b)
{
  return out << pp(b);
}

} // namespace data
} // namespace mcrl2

namespace std
{

  template<>
  struct hash<mcrl2::data::subblock>
  {
    // Default constructor, required for const qualified hash functions.
    hash() = default;

    std::size_t operator()(const mcrl2::data::subblock& t) const
    {
      hash< atermpp::aterm > hasher1;
      hash< mcrl2::data::data_expression > hasher2;
      return mcrl2::utilities::detail::hash_combine(hasher1(t.variable()), hasher2(t.charachteristic_function()));
    }
  };

}

#endif // MCRL2_PBESSYMBOLICBISIM_SUBBLOCK_H
