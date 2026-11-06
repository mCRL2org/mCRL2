// Author(s): Jore Booy
//
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rewriters/dataspec_prune_rewriter.h
/// \brief A rewriter that prunes a data specification to the parts reachable
///        from a given set of expressions.

#ifndef MCRL2_LPS_REWRITERS_DATASPEC_PRUNE_REWRITER_H
#define MCRL2_LPS_REWRITERS_DATASPEC_PRUNE_REWRITER_H

#include "mcrl2/data/selection.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/stochastic_specification.h"


namespace mcrl2::lps {

/// \brief A rewriter that removes unused data specifications.
template <typename Specification>
class dataspec_prune_rewriter
{
  public:
    /// \brief Rewrites an lps specification.
    /// \param x A term
    /// \return The rewrite result.
    Specification operator()(const Specification& p) const
    {
      Specification result(p);
      result.data() = data::prune_data_specification(p.data(), lps::find_function_symbols(p), p.global_variables());
      return result;
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REWRITERS_DATASPEC_PRUNE_REWRITER_H
