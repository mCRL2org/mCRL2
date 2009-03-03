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

#include "mcrl2/lps/detail/linear_process_conversion_visitor.h"
#include "mcrl2/lps/process.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  inline
  /// \brief Parses an mCRL2 specification containing a linear process
  /// Throws detail::linear_process_conversion_visitor::non_linear_process if the process
  /// contains a non-linear process expression.
  /// Throws detail::linear_process_conversion_visitor::unsupported_linear_process if
  /// the process contains an unsupported linear process expression.
  /// \param text A string containing a linear mCRL2 specification
  /// \return The parsed specification
  specification parse_linear_process_specification(const std::string& text)
  {
    process_specification pspec = parse_process_specification(text);
    detail::linear_process_conversion_visitor visitor;
    return visitor.convert(pspec);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
