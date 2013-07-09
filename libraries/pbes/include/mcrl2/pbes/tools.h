// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_H
#define MCRL2_PBES_TOOLS_H

#include <string>
#include "mcrl2/core/print_format.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/pbes/absinthe_strategy.h"
#include "mcrl2/pbes/bisimulation_type.h"
#include "mcrl2/pbes/file_formats.h"
#include "mcrl2/pbes/pbesinst_strategy.h"
#include "mcrl2/pbes/pbes_rewriter_type.h"

namespace mcrl2 {

namespace pbes_system {

void pbesrewr(const std::string& input_filename,
              const std::string& output_filename,
              const data::rewrite_strategy rewrite_strategy,
              pbes_rewriter_type rewriter_type,
              bool skip_data
             );

void pbesconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  data::rewrite_strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations
                 );

bool pbesinst(const std::string& input_filename,
              const std::string& output_filename,
              pbes_file_format pbes_input_format,
              pbes_file_format pbes_output_format,
              data::rewrite_strategy rewrite_strategy,
              pbesinst_strategy m_strategy,
              const std::string& finite_parameter_selection,
              bool remove_redundant_equations,
              bool aterm_ascii
             );

void pbesinfo(const std::string& input_filename,
              const std::string& input_file_message,
              pbes_file_format file_format,
              bool opt_full
             );

void pbesparelm(const std::string& input_filename,
                const std::string& output_filename
               );

void pbespareqelm(const std::string& input_filename,
                  const std::string& output_filename,
                  data::rewrite_strategy rewrite_strategy,
                  pbes_rewriter_type rewriter_type,
                  bool ignore_initial_state
                 );

void pbespp(const std::string& input_filename,
            const std::string& output_filename,
            pbes_file_format pbes_input_format,
            core::print_format_type format
           );

void txt2pbes(const std::string& input_filename,
              const std::string& output_filename,
              bool normalize
             );

void lps2pbes(const std::string& input_filename,
              const std::string& output_filename,
              const std::string& formula_filename,
              bool timed,
              bool structured,
              bool unoptimized
             );

void complps2pbes(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& formula_filename
                 );

void lpsbisim2pbes(const std::string& input_filename1,
                   const std::string& input_filename2,
                   const std::string& output_filename,
                   bisimulation_type type,
                   bool normalize
                  );

void pbesabstract(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& parameter_selection,
                  bool value_true
                 );

void pbesabsinthe(const std::string& input_filename,
                  const std::string& output_filename,
                  const std::string& abstraction_file,
                  absinthe_strategy strategy,
                  bool print_used_function_symbols,
                  bool enable_logging
                 );

void pbesstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    data::rewrite_strategy rewrite_strategy,
                    bool simplify,
                    bool apply_to_original,
                    bool use_local_variant,
                    bool print_influence_graph,
                    bool use_marking_optimization,
                    bool use_alternative_cfp_criterion
                   );

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_H
