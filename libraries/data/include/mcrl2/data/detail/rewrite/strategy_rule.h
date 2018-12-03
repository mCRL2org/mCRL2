// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file strategy_rule.h

#ifndef __REWR_STRATEGY_RULE_H
#define __REWR_STRATEGY_RULE_H


#include "mcrl2/data/data_equation.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

class strategy_rule: public atermpp::aterm_appl
{
  protected:

    const atermpp::function_symbol& rewrite_index_function() const
    {
      static atermpp::function_symbol f=atermpp::function_symbol("rewrite_index",1);
      return f;
    }
  
    const atermpp::function_symbol& rewrite_cpp_function() const
    {
      static atermpp::function_symbol f=atermpp::function_symbol("rewrite_cpp_function",1);
      return f;
    }
  

  public:
    strategy_rule(const std::size_t n)
      : atermpp::aterm_appl(rewrite_index_function(),atermpp::aterm_int(n))
    {}

    strategy_rule(const std::function<data_expression(const application&)> f)
      : atermpp::aterm_appl(rewrite_cpp_function(),atermpp::aterm_int(reinterpret_cast<size_t>(f.target<void*>())))
    {}

    strategy_rule(const data_equation& eq)
      : atermpp::aterm_appl(eq)
    {}

    bool is_rewrite_index() const
    {
      return function()==rewrite_index_function();
    }

    bool is_cpp_code() const
    {
      return function()==rewrite_cpp_function();
    }

    bool is_equation() const
    {
      return is_data_equation(*this);
    }

    data_equation equation() const
    {
      assert(is_equation());
      return atermpp::down_cast<data_equation>(static_cast<atermpp::aterm>(*this));
    }

    std::size_t rewrite_index() const
    {
      assert(is_rewrite_index());
      return (atermpp::down_cast<atermpp::aterm_int>(static_cast<atermpp::aterm>((*this)[0]))).value();
    }

    const std::function<data_expression(const application&)> rewrite_cpp_code() const
    {
      assert(is_cpp_code());
      size_t n=(atermpp::down_cast<atermpp::aterm_int>(static_cast<atermpp::aterm>((*this)[0]))).value();
      return std::function<data_expression(const application&)>((data_expression(const application&)*)(&n));
    }

    
};

struct strategy
{
  size_t number_of_variables;
  atermpp::term_list<strategy_rule> rules;

  strategy(size_t n, const atermpp::term_list<strategy_rule>& r)
   : number_of_variables(n),
     rules(r)
  {}
 
  strategy()
   : number_of_variables(0)
  {}
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // __REWR_STRATEGY_RULE_H

