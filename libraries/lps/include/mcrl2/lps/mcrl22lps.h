// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/mcrl22lps.h
/// \brief The mcrl22lps algorithm.

#ifndef MCRL2_LPS_MCRL22LPS_H
#define MCRL2_LPS_MCRL22LPS_H

#include <string>
#include <sstream>
#include "mcrl2/core/detail/algorithms.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/process/parse.h"

namespace mcrl2 {

namespace lps {

  /// \brief Applies linearization to a specification
  /// \param spec A term
  /// \param options Options for the algorithm
  /// \return The linearized specification
  inline
  specification linearise(ATermAppl spec, t_lin_options options)
  {
    return linearise_std(spec, options);
  } 
  
  /// \brief Generates a linearized process specification from a specification in text.
  /// \param spec A string containing a process specification
  /// \param options Options for the linearization algorithm.
  /// \return The linearized specification.
  inline
  specification mcrl22lps(const std::string& spec, t_lin_options options = t_lin_options())
  {
    lin_std_initialize_global_variables();

    // the lineariser expects data from a stream...
    std::stringstream spec_stream;
    spec_stream << spec;

    atermpp::aterm_appl result = core::detail::parse_specification(spec_stream);
    result                     = core::detail::type_check_specification(result);
    result                     = core::detail::alpha_reduce(result);
    result                     = data::detail::internal_format_conversion(result);
    return linearise(result, options);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_MCRL22LPS_H
