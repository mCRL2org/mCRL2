#ifndef MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H
#define MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
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
        data::used_data_equation_selector used_selector(p.data(),
            lps::find_function_symbols(p),
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
      
        Specification result(data_spec, p.action_labels(), p.global_variables(), p.process(), p.initial_process());
        return result;
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H
