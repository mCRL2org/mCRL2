///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/lps/detail/tools.h
/// Add your file description here.
//
//  Copyright 2007 Wieger Wesselink. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPS_DETAIL_TOOLS
#define LPS_DETAIL_TOOLS

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "libalpha.h"
#include "dataimpl.h"
#include "regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/lps/detail/algorithms.h"

namespace lps {

namespace detail {

  /// Linearises the specification.
  /// N.B. this function may only be called once!
  inline
  specification mcrl22lps(const std::string& spec, t_lin_options options = t_lin_options())
  {   
//    static bool first_time_called = true;
//    if (first_time_called)
//    {
//      first_time_called = false;
//    }
//    else
//    {
//      throw std::runtime_error("mcrl22lps may only be called once!");
//    }
    lin_std_initialize_global_variables();
    
    // the lineariser expects data from a stream...
    std::stringstream spec_stream;
    spec_stream << spec;

    ATermAppl result = parse_specification(spec_stream);
    result = type_check_specification(result);
    // result = alpha_reduce(result);
    result = implement_data_specification(result);
    return linearise(result, options);
  }

  inline
  pbes lps2pbes(const specification& spec, const state_formula& formula, bool timed)
  {
    return lps::pbes_translate(formula, spec, timed);
  }

  inline
  state_formula mcf2statefrm(const std::string& formula_text, const specification& spec)
  {
    std::stringstream formula_stream;
    formula_stream << formula_text;
    ATermAppl f = parse_state_formula(formula_stream);
    f = type_check_state_formula(f, spec);
    f = implement_data_state_formula(f, spec);
    f = translate_regular_formula(f);
    return f;
  }

  // 
  inline
  pbes lps2pbes(const std::string& spec_text, const std::string& formula_text, bool timed)
  {
    pbes result;
    specification spec = mcrl22lps(spec_text);
    state_formula f = mcf2statefrm(formula_text, spec);
    return lps2pbes(spec, f, timed);
  }

} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_TOOLS
