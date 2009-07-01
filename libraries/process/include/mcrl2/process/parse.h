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

#include <string>
#include <sstream>
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/core/detail/algorithms.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2 {

namespace process {

  /// \brief Parses a process specification from a string
  /// \param spec A string
  /// \return The parse result
  inline
  process_specification parse_process_specification(
                                  const std::string& spec, 
                                  const bool alpha_reduce=false)
  {
    std::stringstream spec_stream;
    spec_stream << spec;
    ATermAppl result = core::detail::parse_process_specification(spec_stream);
    result           = core::detail::type_check_process_specification(result);
    if (alpha_reduce)
    { result           = core::detail::alpha_reduce_process_specification(result);
    }
    result           = data::detail::internal_format_conversion(result);
    return atermpp::aterm_appl(result);
  }

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PARSE_H
