// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

class strategy_rule:public atermpp::aterm
{
  public:
    strategy_rule(const size_t n)
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

    data_equation equation() const
    {
      assert(is_equation());
      return atermpp::down_cast<data_equation>(static_cast<atermpp::aterm>(*this));
    }

    size_t rewrite_index() const
    {
      assert(is_rewrite_index());
      return (atermpp::down_cast<atermpp::aterm_int>(static_cast<atermpp::aterm>(*this))).value();
    }
};

typedef atermpp::term_list<strategy_rule> strategy;

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif // __REWR_STRATEGY_RULE_H

