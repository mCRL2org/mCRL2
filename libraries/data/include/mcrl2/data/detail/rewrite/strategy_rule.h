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
class strategy_rule 
{
  protected:
    // Only one of the fields rewrite_rule, rewrite_index or cpp_function will be used
    // at any given time. As this hardly requires a lot of memory, we do not optimise
    // this using for instance a union type. 
    enum { data_equation_type, rewrite_index_type, cpp_function_type } m_strategy_element_type;
    data_equation m_rewrite_rule;
    size_t m_rewrite_index;
    std::function<data_expression(const data_expression&)> m_cpp_function;

  public:
    strategy_rule(const std::size_t n)
      : m_strategy_element_type(rewrite_index_type),
        m_rewrite_index(n)
    {}

    strategy_rule(const std::function<data_expression(const data_expression&)> f)
      :  m_strategy_element_type(cpp_function_type),
         m_cpp_function(f)
    {}

    strategy_rule(const data_equation& eq)
      : m_strategy_element_type(data_equation_type),
        m_rewrite_rule(eq)
    {}

    bool is_rewrite_index() const
    {
      return m_strategy_element_type==rewrite_index_type;
    }

    bool is_cpp_code() const
    {

      return m_strategy_element_type==cpp_function_type;
    }

    bool is_equation() const
    {
      return m_strategy_element_type==data_equation_type;
    }

    data_equation equation() const
    {
      assert(is_equation());
      assert(is_data_equation(m_rewrite_rule));
      return m_rewrite_rule;
    }

    std::size_t rewrite_index() const
    {
      assert(is_rewrite_index());
      return m_rewrite_index;
    }

    const std::function<data_expression(const data_expression&)> rewrite_cpp_code() const
    {
      assert(is_cpp_code());
      return m_cpp_function;
    }
};

/// A strategy is a list of rules and the number of variables that occur in it.
class strategy
{
  protected:
    std::size_t m_number_of_variables;
    std::vector<strategy_rule> m_rules;

  public:
    /// \brief Default constructor. 
    strategy(size_t n, const std::vector<strategy_rule>& r)
     : m_number_of_variables(n),
       m_rules(r)
    {}
   
    /// \brief Default constructor. 
    strategy()
     : m_number_of_variables(0)
    {}
  
    /// \brief Provides the maximal number of variables used in the rewrite rules making up this strategy. 
    std::size_t number_of_variables() const 
    { 
      return m_number_of_variables; 
    }

    /// \brief Yield the rules of the strategy. 
    const std::vector<strategy_rule>& rules() const 
    { 
      return m_rules; 
    }

};

/// \brief Creates a strategy for given set of rewrite rules with head symbol f.
strategy create_strategy(data_equation_list rules);

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // MCRL2_DATA_DETAIL_REWRITE_STRATEGY_RULE_H

