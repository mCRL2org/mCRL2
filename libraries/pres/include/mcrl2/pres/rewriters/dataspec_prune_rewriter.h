#ifndef MCRL2_PRES_REWRITERS_DATASPEC_REWRITER_H
#define MCRL2_PRES_REWRITERS_DATASPEC_REWRITER_H

#include "mcrl2/data/selection.h"
#include "mcrl2/pres/pres.h"


namespace mcrl2::pres_system {

/// \brief A rewriter that removes unused data specifications.
class dataspec_prune_rewriter
{
  public:
    /// \brief The term type
    using term_type = pres;

    /// \brief Rewrites a pres specification.
    /// \param x A term
    /// \return The rewrite result.
    pres operator()(const pres& p) const
    {
      pres result(p);
      result.set_data(data::prune_data_specification(p.data(), pres_system::find_function_symbols(p), p.global_variables()));
      return result;
    }
};

} // namespace mcrl2::pres_system

#endif // MCRL2_PRES_REWRITERS_DATASPEC_REWRITER_H
