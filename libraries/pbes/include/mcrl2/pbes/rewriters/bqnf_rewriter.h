// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/bqnf_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_BQNF_REWRITER_H
#define MCRL2_PBES_REWRITERS_BQNF_REWRITER_H

#include <memory>

#include "mcrl2/pbes/detail/bqnf_quantifier_rewriter.h"



namespace mcrl2::pbes_system {

/// \brief A rewriter that rewrites universal quantifiers over conjuncts
/// in BQNF expressions to conjuncts over universal quantifiers.
class bqnf_rewriter
{
  public:
    /// \brief The equation type
    using equation_type = pbes_equation;
    /// \brief The term type
    using term_type = pbes_expression;

    /// \brief Constructor
    bqnf_rewriter() {
      bqnf_checker = std::make_unique<pbes_system::detail::bqnf_visitor>();
      bqnf_quantifier_rewriter = std::make_unique<pbes_system::detail::bqnf_quantifier_rewriter>();
    }

    /// \brief Rewrites a PBES expression in BQNF such that universal quantifier over conjuncts
    /// are replaced by conjuncts over universal quantifiers.
    /// \param t A term.
    /// \return The expression resulting from the transformation.
    term_type operator()(const term_type& t) const
    {
      bool is_bqnf = false;
      try {
        is_bqnf = bqnf_checker->visit_bqnf_expression(t);
      } catch(std::runtime_error& e) {
        std::clog << e.what() << std::endl;
      }
      if (!is_bqnf)
      {
        throw(std::runtime_error("Input expression not in BQNF."));
      }
      term_type result = bqnf_quantifier_rewriter->rewrite_bqnf_expression(t);
      return result;
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    term_type operator()(const term_type& x, SubstitutionFunction sigma) const
    {
      return sigma(this->operator()(x));
    }

private:
    std::unique_ptr<pbes_system::detail::bqnf_visitor> bqnf_checker;
    std::unique_ptr<pbes_system::detail::bqnf_quantifier_rewriter> bqnf_quantifier_rewriter;
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_BQNF_REWRITER_H
