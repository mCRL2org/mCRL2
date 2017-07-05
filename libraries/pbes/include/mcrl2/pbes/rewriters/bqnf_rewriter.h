// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/bqnf_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_BQNF_REWRITER_H
#define MCRL2_PBES_REWRITERS_BQNF_REWRITER_H

#include "mcrl2/pbes/detail/bqnf_quantifier_rewriter.h"
#include "mcrl2/pbes/detail/bqnf_visitor.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief A rewriter that rewrites universal quantifiers over conjuncts
/// in BQNF expressions to conjuncts over universal quantifiers.
class bqnf_rewriter
{
  public:
    /// \brief The equation type
    typedef pbes_equation equation_type;
    /// \brief The term type
    typedef pbes_expression term_type;

    pbes_system::detail::bqnf_visitor* bqnf_checker;
    pbes_system::detail::bqnf_quantifier_rewriter* bqnf_quantifier_rewriter;

    /// \brief Constructor
    bqnf_rewriter() {
      this->bqnf_checker = new pbes_system::detail::bqnf_visitor();
      this->bqnf_quantifier_rewriter = new pbes_system::detail::bqnf_quantifier_rewriter();
    }

    /// \brief Rewrites a PBES expression in BQNF such that universal quantifier over conjuncts
    /// are replaced by conjuncts over universal quantifiers.
    /// \param t A term.
    /// \return The expression resulting from the transformation.
    term_type operator()(const term_type& t) const
    {
      bool is_bqnf = false;
      try {
        is_bqnf = this->bqnf_checker->visit_bqnf_expression(t);
      } catch(std::runtime_error& e) {
        std::clog << e.what() << std::endl;
      }
      if (!is_bqnf)
      {
        throw(std::runtime_error("Input expression not in BQNF."));
      }
      term_type result = this->bqnf_quantifier_rewriter->rewrite_bqnf_expression(t);
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
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_BQNF_REWRITER_H
