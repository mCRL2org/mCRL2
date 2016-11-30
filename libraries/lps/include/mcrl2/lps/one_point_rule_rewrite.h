// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rewriters/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_ONE_POINT_RULE_REWRITE_H
#define MCRL2_LPS_ONE_POINT_RULE_REWRITE_H

#include "mcrl2/data/rewriters/one_point_rule_rewriter.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2 {

namespace lps {

namespace detail {

struct one_point_rule_rewrite_builder: public lps::data_expression_builder<one_point_rule_rewrite_builder>
{
  typedef lps::data_expression_builder<one_point_rule_rewrite_builder> super;
  using super::apply;
  using super::update;

  data::one_point_rule_rewriter r;

  data::data_expression apply(const data::data_expression& x)
  {
    return r(x);
  }
};

} // namespace detail

inline
void one_point_rule_rewrite(specification& lpsspec)
{
  detail::one_point_rule_rewrite_builder f;
  f.update(lpsspec);
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ONE_POINT_RULE_REWRITE_H
