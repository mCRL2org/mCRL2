// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/parse.h
/// \brief Parse function for process specifications

#ifndef MCRL2_PROCESS_PARSE_H
#define MCRL2_PROCESS_PARSE_H

#include <iostream>
#include <string>
#include <sstream>
#include "mcrl2/core/parse.h"
#include "mcrl2/core/text_utility.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/typecheck.h"
#include "mcrl2/process/alphabet_reduction.h"
#include "mcrl2/process/detail/sort_normalization_builder.h"
#include "mcrl2/process/detail/translate_user_notation_builder.h"

namespace mcrl2 {

namespace process {

  /// \brief Parses a process specification from an input stream
  /// \param spec_stream An input stream
  /// \param alpha_reduce Indicates whether alphabet reductions need to be performed
  /// \return The parse result
  inline
  process_specification parse_process_specification(
                                  std::istream& spec_stream, 
                                  const bool alpha_reduce=false)
  {
    ATermAppl x = core::parse_proc_spec(spec_stream);
    if (x == NULL)
    {
      throw mcrl2::runtime_error("parse error");
    }

    process_specification result(x, false);
    type_check(result);
    if (alpha_reduce)
    {
      apply_alphabet_reduction(result);
    }
    detail::translate_user_notation(result);
    detail::normalize_sorts(result);

    return result;
  }

  /// \brief Parses a process specification from a string
  /// \param spec_string A string
  /// \param alpha_reduce Indicates whether alphabet reductions needdto be performed
  /// \return The parse result
  inline
  process_specification parse_process_specification(
                                  const std::string& spec_string, 
                                  const bool alpha_reduce=false)
  {
    std::istringstream spec_stream(spec_string);
    return parse_process_specification(spec_stream, alpha_reduce);
  }

  /// \brief Parses and type checks a process expression.
  /// \param[in] text The input text containing a process expression.
  /// \param[in] data_decl A declaration of data and actions ("glob m:Nat; act a:Nat;").
  /// \param[in] proc_decl A process declaration ("proc P(n: Nat);").
  inline
  process_expression parse_process_expression(const std::string& text,
                                              const std::string& data_decl,
                                              const std::string& proc_decl
                                             )
  {
    std::string proc_text = core::regex_replace(";", " = delta;", proc_decl);
    std::string init_text = "init\n     " + text + ";\n";
    std::string spec_text = data_decl + "\n" + proc_text + "\n" + init_text;
    process_specification spec = parse_process_specification(spec_text);
    return spec.init();
  }

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PARSE_H
