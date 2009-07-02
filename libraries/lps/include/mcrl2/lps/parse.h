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

  /// \brief Parses a linear process specification from an input stream
  /// \param text An input stream containing a linear process specification
  /// \return The parsed specification
  /// \exception non_linear_process if a non-linear sub-expression is encountered.
  /// \exception mcrl2::runtime_error in the following cases:
  /// \li The number of equations is not equal to one
  /// \li The initial process is not a process instance, or it does not match with the equation
  /// \li A sequential process is found with a right hand side that is not a process instance,
  /// or it doesn't match the equation
  inline
  specification parse_linear_process_specification(std::istream& spec_stream)
  {
    process::process_specification pspec = mcrl2::process::parse_process_specification(spec_stream);
    assert(process::is_linear(pspec));
    process::detail::linear_process_conversion_visitor visitor;
    specification result = visitor.convert(pspec);
    return result;
  }
  /// \brief Parses a linear process specification from a string
  /// \param text A string containing a linear process specification
  /// \return The parsed specification
  /// \exception non_linear_process if a non-linear sub-expression is encountered.
  /// \exception mcrl2::runtime_error in the following cases:
  /// \li The number of equations is not equal to one
  /// \li The initial process is not a process instance, or it does not match with the equation
  /// \li A sequential process is found with a right hand side that is not a process instance,
  /// or it doesn't match the equation
  inline
  specification parse_linear_process_specification(const std::string& text)
  {
    process::process_specification pspec = mcrl2::process::parse_process_specification(text);
    assert(process::is_linear(pspec));
    process::detail::linear_process_conversion_visitor visitor;
    specification result = visitor.convert(pspec);
    return result;
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PARSE_H
