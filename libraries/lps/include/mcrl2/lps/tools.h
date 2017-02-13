// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/tools.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TOOLS_H
#define MCRL2_LPS_TOOLS_H

#include <string>
#include "mcrl2/core/print.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/prover/solver_type.h"

namespace mcrl2 {

namespace lps {

void lpsbinary(const std::string& input_filename,
               const std::string& output_filename);

void lpsconstelm(const std::string& input_filename,
                 const std::string& output_filename,
                 data::rewriter::strategy rewrite_strategy,
                 bool instantiate_free_variables,
                 bool ignore_conditions,
                 bool remove_trivial_summands,
                 bool remove_singleton_sorts
                );

void lpsinfo(const std::string& input_filename,
             const std::string& input_file_message
            );

bool lpsinvelm(const std::string& input_filename,
               const std::string& output_filename,
               const std::string& invariant_filename,
               const std::string& dot_file_name,
               data::rewriter::strategy rewrite_strategy,
               data::detail::smt_solver_type solver_type,
               const bool no_check,
               const bool no_elimination,
               const bool simplify_all,
               const bool all_violations,
               const bool counter_example,
               const bool path_eliminator,
               const bool apply_induction,
               const int time_limit
              );

void lpsparelm(const std::string& input_filename,
               const std::string& output_filename
              );

void lpspp(const std::string& input_filename,
           const std::string& output_filename,
           bool print_summand_numbers,
           core::print_format_type format
          );

void lpsrewr(const std::string& input_filename,
             const std::string& output_filename,
             const data::rewriter::strategy rewrite_strategy
            );

void lpssumelm(const std::string& input_filename,
               const std::string& output_filename,
               const bool decluster);

void lpssuminst(const std::string& input_filename,
                const std::string& output_filename,
                const data::rewriter::strategy rewrite_strategy,
                const std::string& sorts_string,
                const bool finite_sorts_only,
                const bool tau_summands_only);

void lpsuntime(const std::string& input_filename,
               const std::string& output_filename,
               const bool add_invariants,
               const data::rewriter::strategy rewrite_strategy);

void txt2lps(const std::string& input_filename,
             const std::string& output_filename
            );

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TOOLS_H
