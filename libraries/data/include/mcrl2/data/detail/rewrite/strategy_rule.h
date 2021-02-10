// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file strategy_rule.h

#ifndef MCRL2_DATA_DETAIL_REWRITE_STRATEGY_RULE_H
#define MCRL2_DATA_DETAIL_REWRITE_STRATEGY_RULE_H

#include "mcrl2/data/data_equation.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/// \brief Is either a rewrite rule to be matched or an index that should be rewritten.
class strategy_rule : public atermpp::aterm
{
  public:
    strategy_rule(const std::size_t n)
      : aterm(atermpp::aterm_int(n))
    {}

    strategy_rule(const data_equation& eq)
      : aterm(eq)
    {}

    bool is_rewrite_index() const
    {
      return type_is_int();
    }

    bool is_equation() const
    {
      return !type_is_int();
    }

    const data_equation& equation() const
    {
      assert(is_equation());
      return atermpp::down_cast<data_equation>(*this);
    }

    std::size_t rewrite_index() const
    {
      assert(is_rewrite_index());
      return (atermpp::down_cast<atermpp::aterm_int>(static_cast<const atermpp::aterm&>(*this))).value();
    }
};

/// A strategy is a list of rules and the number of variables that occur in it.
class strategy
{
public:
  strategy(size_t n, const atermpp::term_list<strategy_rule>& r)
   : m_number_of_variables(n),
     m_rules(r)
  {}
 
  strategy()
   : m_number_of_variables(0)
  {}

  std::size_t number_of_variables() const { return m_number_of_variables; }
  const atermpp::term_list<strategy_rule>& rules() const { return m_rules; }

private:
  std::size_t m_number_of_variables;
  atermpp::term_list<strategy_rule> m_rules;
};

/// \brief Creates a strategy for given set of rewrite rules with head symbol f.
strategy create_strategy(data_equation_list rules);

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_REWRITE_STRATEGY_RULE_H

