///////////////////////////////////////////////////////////////////////////////
/// \file mcrl2/lps/detail/algorithms.h
/// Add your file description here.
//
//  Copyright 2007 Wieger Wesselink. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LPS_DETAIL_LPS2PBES
#define LPS_DETAIL_LPS2PBES

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
#include "../../../../../../tools/mcrl22lps/lin_types.h"
#include "../../../../../../tools/mcrl22lps/lin_std.h"
#include "../../../../../../tools/mcrl22lps/lin_alt.h"

namespace lps {

namespace detail {

  inline
  ATermAppl parse_specification(std::istream& from)
  {
    ATermAppl result = parse_spec(from);
    if (result == NULL)
      throw std::runtime_error("parse error");
    return result;
  }
  
  inline
  ATermAppl type_check_specification(ATermAppl spec)
  {
    ATermAppl result = type_check_spec(spec);
    if (result == NULL)
      throw std::runtime_error("type check error");
    return result;
  }
  
  inline
  ATermAppl alpha_reduce(ATermAppl spec)
  {
    ATermAppl result = gsAlpha(result);
    if (result == NULL)
      throw std::runtime_error("alpha reduction error");
    return result;
  }
  
  inline
  ATermAppl implement_data_specification(ATermAppl spec)
  {
    ATermAppl result = implement_data_spec(spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  specification linearise(ATermAppl spec, t_lin_options options, bool standard = true)
  {
    ATermAppl result = standard ? linearise_std(spec, options) : linearise_alt(spec, options);
    if (result == NULL)
      throw std::runtime_error("linearisation error");
    return aterm_appl(result);
  }

  inline
  ATermAppl parse_state_formula(istream& from)
  {
    ATermAppl result = parse_state_frm(from);
    if (result == NULL)
      throw std::runtime_error("parse error in parse_state_frm()");
    return result;
  }
  
  inline
  ATermAppl type_check_state_formula(ATermAppl formula, specification spec)
  {
    ATermAppl result = type_check_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("type check error");
    return result;
  }
  
  inline
  ATermAppl implement_data_state_formula(ATermAppl formula, specification spec)
  {
    ATermAppl result = implement_data_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  state_formula translate_regular_formula(ATermAppl formula)
  {
    ATermAppl result = translate_reg_frms(formula);
    if (result == NULL)
      throw std::runtime_error("formula translation error");
    return result;
  }
} // namespace detail

} // namespace lps

#endif // LPS_DETAIL_LPS2PBES
