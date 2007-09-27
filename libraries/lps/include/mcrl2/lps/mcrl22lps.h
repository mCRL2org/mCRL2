// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/tools.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_MCRL22LPS_H
#define MCRL2_LPS_MCRL22LPS_H

#include <string>
#include <sstream>
#include "mcrl2/lps/lin_std.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/detail/algorithms.h"

namespace lps {

  /// Linearises the specification.
  inline
  specification mcrl22lps(const std::string& spec, t_lin_options options = t_lin_options())
  {   
    using namespace lps::detail;

    lin_std_initialize_global_variables();
    
    // the lineariser expects data from a stream...
    std::stringstream spec_stream;
    spec_stream << spec;

    ATermAppl result = parse_specification(spec_stream);
    result = type_check_specification(result);
    result = alpha_reduce(result);
    result = implement_data_specification(result);
    return linearise(result, options);
  }

} // namespace lps

#endif // MCRL2_LPS_MCRL22LPS_H
