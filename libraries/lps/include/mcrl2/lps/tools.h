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

namespace mcrl2 {

namespace lps {

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

void lpsparelm(const std::string& input_filename,
               const std::string& output_filename
              );

void lpspp(const std::string& input_filename,
           const std::string& output_filename,
           core::t_pp_format format
          );

void lpsrewr(const std::string& input_filename,
             const std::string& output_filename,
             const data::rewriter::strategy rewrite_strategy,
             bool benchmark,
             unsigned long bench_times
            );

void txtlps(const std::string& input_filename,
            const std::string& output_filename
           );

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TOOLS_H
