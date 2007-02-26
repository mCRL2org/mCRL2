///////////////////////////////////////////////////////////////////////////////
/// \file lpe/detail/lpe2pbes.h

#ifndef LPE_DETAIL_TOOLS
#define LPE_DETAIL_TOOLS

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
#include "lpe/specification.h"
#include "lpe/pbes.h"
#include "lpe/pbes_translate.h"
#include "lpe/detail/algorithms.h"

namespace lpe {

namespace detail {

  /// Linearises the specification.
  /// N.B. this function may only be called once!
  inline
  specification mcrl22lpe(const std::string& spec, t_lin_options options = t_lin_options())
  {
    static bool first_time_called = true;
    if (first_time_called)
    {
      first_time_called = false;
    }
    else
    {
      throw std::runtime_error("mcrl22lpe may only be called once!");
    }
   
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
  pbes lpe2pbes(const specification& spec, const state_formula& formula, bool timed)
  {
    return lpe::pbes_translate(formula, spec, timed);
  }

  // 
  inline
  pbes lpe2pbes(const std::string& spec_text, const std::string& formula_text, bool timed)
  {
    pbes result;
    specification spec = mcrl22lpe(spec_text);
    std::stringstream formula_stream;
    formula_stream << formula_text;
    ATermAppl f = parse_state_formula(formula_stream);
    f = type_check_state_formula(f, spec);
    f = implement_data_state_formula(f, spec);
    f = translate_regular_formula(f);
    return lpe2pbes(spec, f, timed);
  }

} // namespace detail

} // namespace lpe

#endif // LPE_DETAIL_TOOLS
