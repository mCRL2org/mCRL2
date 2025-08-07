// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/if_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_IF_REWRITER_H
#define MCRL2_PBES_REWRITERS_IF_REWRITER_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/data/rewriters/if_rewriter.h"
#include "mcrl2/pbes/builder.h"



namespace mcrl2::pbes_system {

namespace detail {

struct if_rewrite_builder: public pbes_system::data_expression_builder<if_rewrite_builder>
{
  using super = pbes_system::data_expression_builder<if_rewrite_builder>;
  using super::apply;
  using super::update;

  data::if_rewriter r;

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = r(x);
  }
};

} // namespace detail

class if_rewriter
{
  public:
  /// \brief The term type
    using term_type = pbes_expression;

    /// \brief The variable type
    using variable_type = data::variable;

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      detail::if_rewrite_builder f{};
      pbes_expression result;
      f.apply(result, x);
      return result;
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_IF_REWRITER_H
