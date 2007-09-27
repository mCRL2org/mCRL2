// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithms.h
/// \brief Add your file description here.

#ifndef PBES_DETAIL_LPS2PBES
#define PBES_DETAIL_LPS2PBES

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <sstream>
#include <fstream>
#include "aterm2.h"
#include "print/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"

namespace lps {

namespace detail {

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

#endif // PBES_DETAIL_LPS2PBES
