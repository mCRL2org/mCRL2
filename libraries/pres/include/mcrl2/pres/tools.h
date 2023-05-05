// Author(s): Jan Friso Groote, based on pbes tools by Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/tools.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_TOOLS_H
#define MCRL2_PRES_TOOLS_H

#include <string>

#include "mcrl2/core/print_format.h"
// #include "mcrl2/pres/absinthe_strategy.h"
// #include "mcrl2/pres/bisimulation_type.h"
// #include "mcrl2/pres/pres_rewriter_type.h"
// #include "mcrl2/pres/presinst_strategy.h"
// #include "mcrl2/pres/tools/presstategraph_options.h"
#include "mcrl2/utilities/file_utility.h"

namespace mcrl2 {

namespace pres_system {

/* void presrewr(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& input_format,
              const utilities::file_format& output_format,
              data::rewrite_strategy rewrite_strategy,
              pres_rewriter_type rewriter_type);

void presconstelm(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  data::rewrite_strategy rewrite_strategy,
                  pres_rewriter_type rewriter_type,
                  bool compute_conditions,
                  bool remove_redundant_equations,
                  bool check_quantifiers
                 ); */

void presinfo(const std::string& input_filename,
              const std::string& input_file_message,
              const utilities::file_format& file_format,
              bool opt_full
             );

/* void presparelm(const std::string& input_filename,
                const std::string& output_filename,
                const utilities::file_format& input_format,
                const utilities::file_format& output_format
               );

void prespareqelm(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  data::rewrite_strategy rewrite_strategy,
                  pres_rewriter_type rewriter_type,
                  bool ignore_initial_state
                 ); */

void prespp(const std::string& input_filename,
            const std::string& output_filename,
            const utilities::file_format& input_format,
            core::print_format_type format,
            bool use_pfnf_printer
           );

void txt2pres(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& output_format,
              bool normalize
             ); 

void lps2pres(const std::string& input_filename,
              const std::string& output_filename,
              const utilities::file_format& output_format,
              const std::string& formula_filename,
              bool timed,
              bool unoptimized,
              bool preprocess_modal_operators,
              bool check_only
             );

/* void complps2pres(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& output_format,
                  const std::string& formula_filename
                 );

void lpsbisim2pres(const std::string& input_filename1,
                   const std::string& input_filename2,
                   const std::string& output_filename,
                   const utilities::file_format& output_format,
                   bisimulation_type type,
                   bool normalize
                  );

void presabstract(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  const std::string& parameter_selection,
                  bool value_true
                 );

void presabsinthe(const std::string& input_filename,
                  const std::string& output_filename,
                  const utilities::file_format& input_format,
                  const utilities::file_format& output_format,
                  const std::string& abstraction_file,
                  absinthe_strategy strategy,
                  bool print_used_function_symbols,
                  bool enable_logging
                 );

void presstategraph(const std::string& input_filename,
                    const std::string& output_filename,
                    const utilities::file_format& input_format,
                    const utilities::file_format& output_format,
                    const presstategraph_options& options
                   ); */

} // namespace rres_system

} // namespace mcrl2

#endif // MCRL2_PRES_TOOLS_H
