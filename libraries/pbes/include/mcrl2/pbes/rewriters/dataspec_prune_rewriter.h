#ifndef MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H
#define MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/pbes/pbes.h"


namespace mcrl2::pbes_system {

/// \brief A rewriter that removes unused data specifications.
class dataspec_prune_rewriter
{
  public:
    /// \brief The term type
    using term_type = pbes;

    /// \brief Rewrites a pres specification.
    /// \param x A term
    /// \return The rewrite result.
    pbes operator()(const pbes& p) const
    {
        data::used_data_equation_selector used_selector(p.data(),
            pbes_system::find_function_symbols(p),
            p.global_variables(),
            false);
        data::data_equation_vector equations;
        for (data::data_equation i: p.data().user_defined_equations())
        {
          if (used_selector(i))
          {
            equations.push_back(i);
          }
        }
      
        data::function_symbol_vector mappings;
        for (data::function_symbol i: p.data().user_defined_mappings()) {
            if (used_selector(i)) {
                mappings.emplace_back(i);
            }
        }
      
        data::data_specification data_spec = data::data_specification(p.data().user_defined_sorts(),
            p.data().user_defined_aliases(),
            p.data().user_defined_constructors(),
            mappings,
            equations);
      
        return pbes(data_spec, p.global_variables(), p.equations(), p.initial_state());
    }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_REWRITERS_DATASPEC_REWRITER_H
