// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/parse.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PARSE_H
#define MCRL2_LPS_PARSE_H

#include "mcrl2/data/detail/convert.h"
#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/process/parse.h"

namespace mcrl2 {

namespace lps {

  inline
  /// \brief Parses an mCRL2 specification containing a linear process
  /// If the process is not linear, in debug mode an assertion will be triggered.
  /// Throws detail::linear_process_conversion_visitor::non_linear_process if the process
  /// contains a non-linear process expression.
  /// Throws detail::linear_process_conversion_visitor::unsupported_linear_process if
  /// the process contains an unsupported linear process expression.
  /// \param text A string containing a linear mCRL2 specification
  /// \return The parsed specification
  specification parse_linear_process_specification(const std::string& text)
  {
    process::process_specification pspec = mcrl2::process::parse_process_specification(text);
    assert(process::is_linear(pspec));
    process::detail::linear_process_conversion_visitor visitor;
    specification result = visitor.convert(pspec);
    // result.repair_free_variables();
    return result;
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
