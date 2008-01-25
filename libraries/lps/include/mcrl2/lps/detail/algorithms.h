// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithms.h
/// \brief Add your file description here.

#ifndef LPS_DETAIL_LPS2PBES
#define LPS_DETAIL_LPS2PBES

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/core/detail/parse.h"
#include "mcrl2/core/detail/typecheck.h"
#include "mcrl2/core/alpha.h"
#include "mcrl2/core/detail/data_implementation.h"
#include "mcrl2/core/detail/regfrmtrans.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/modal_formula/mucalculus.h"
#include "mcrl2/lps/lin_types.h"
#include "mcrl2/lps/lin_std.h"

namespace mcrl2 {

namespace lps {

namespace detail {

  inline
  ATermAppl parse_specification(std::istream& from)
  {
    ATermAppl result = core::detail::parse_spec(from);
    if (result == NULL)
      throw std::runtime_error("parse error");
    return result;
  }
  
  inline
  ATermAppl type_check_specification(ATermAppl spec)
  {
    ATermAppl result = core::detail::type_check_spec(spec);
    if (result == NULL)
      throw std::runtime_error("type check error");
    return result;
  }
  
  inline
  ATermAppl alpha_reduce(ATermAppl spec)
  {
    ATermAppl result = core::gsAlpha(spec);
    if (result == NULL)
      throw std::runtime_error("alpha reduction error");
    return result;
  }
  
  inline
  ATermAppl implement_data_specification(ATermAppl spec)
  {
    ATermAppl result = core::detail::implement_data_spec(spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  specification linearise(ATermAppl spec, t_lin_options options)
  {
    ATermAppl result = linearise_std(spec, options);
    if (result == NULL)
      throw std::runtime_error("linearisation error");
    return aterm_appl(result);
  }

  inline
  ATermAppl parse_state_formula(istream& from)
  {
    ATermAppl result = core::detail::parse_state_frm(from);
    if (result == NULL)
      throw std::runtime_error("parse error in parse_state_frm()");
    return result;
  }

  /// \pre spec is a specification before data implementation
  inline
  ATermAppl type_check_state_formula(ATermAppl formula, ATermAppl spec)
  {
    ATermAppl result = core::detail::type_check_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("type check error");
    return result;
  }
 
  /// \pre spec is a specification before data implementation
  inline
  ATermAppl implement_data_state_formula(ATermAppl formula, ATermAppl& spec)
  {
    ATermAppl result = core::detail::implement_data_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  modal::state_formula translate_regular_formula(ATermAppl formula)
  {
    ATermAppl result = core::detail::translate_reg_frms(formula);
    if (result == NULL)
      throw std::runtime_error("formula translation error");
    return result;
  }

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // LPS_DETAIL_LPS2PBES
