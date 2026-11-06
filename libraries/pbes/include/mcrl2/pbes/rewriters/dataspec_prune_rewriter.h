#ifndef MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H
#define MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H

#include "mcrl2/data/selection.h"
#include "mcrl2/pbes/pbes.h"


namespace mcrl2::pbes_system {

/// \brief A rewriter that removes unused data specifications.
class dataspec_prune_rewriter
{
  public:
    /// \brief The term type
    using term_type = pbes;

    /// \brief Rewrites a pbes specification.
    /// \param x A term
    /// \return The rewrite result.
    pbes operator()(const pbes& p) const
    {
      pbes result(p);
      result.data() = data::prune_data_specification(p.data(), pbes_system::find_function_symbols(p), p.global_variables());
      return result;
    }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H
