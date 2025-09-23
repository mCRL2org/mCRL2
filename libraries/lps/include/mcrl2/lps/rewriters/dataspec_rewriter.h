#ifndef MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H
#define MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H

#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/logger.h"


namespace mcrl2::lps {

/// \brief A rewriter that pushes removed unused data specifications.
class dataspec_rewriter
{
  public:
    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    stochastic_specification operator()(const stochastic_specification& p) const
    {
        data::used_data_equation_selector used_selector(p.data(),
            lps::find_function_symbols(p),
            p.global_variables(),
            false);
        data::data_equation_vector equations;
        for (auto i: p.data().user_defined_equations())
        {
          if (used_selector(i))
          {
            equations.push_back(i);
          }
        }
      
        data::function_symbol_vector mappings;
        for (auto i: p.data().user_defined_mappings()) {
            if (used_selector(i)) {
                mappings.emplace_back(i);
            }
        }
      
        data::data_specification data_spec = data::data_specification(p.data().user_defined_sorts(),
            p.data().user_defined_aliases(),
            p.data().user_defined_constructors(),
            mappings,
            equations);
      
        return stochastic_specification(data_spec, p.action_labels(), p.global_variables(), 
            stochastic_linear_process(p.process()),
            stochastic_process_initializer(p.initial_process().expressions(),stochastic_distribution()));
    }
};

} // namespace mcrl2::lps

#endif // MCRL2_LPS_REWRITERS_DATASPEC_REWRITER_H
