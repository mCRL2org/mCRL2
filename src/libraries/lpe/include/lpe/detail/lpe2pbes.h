///////////////////////////////////////////////////////////////////////////////
/// \file lpe/detail/lpe2pbes.h

#ifndef LPE_DETAIL_LPE2PBES
#define LPE_DETAIL_LPE2PBES

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
#include "lpe/detail/mcrl22lpe.h"

namespace lpe {

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
  ATermAppl type_check(ATermAppl formula, specification spec)
  {
    ATermAppl result = type_check_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("type check error");
    return result;
  }
  
  inline
  ATermAppl implement_data(ATermAppl formula, specification spec)
  {
    ATermAppl result = implement_data_state_frm(formula, spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  ATermAppl translate_formula(ATermAppl formula)
  {
    ATermAppl result = translate_reg_frms(formula);
    if (result == NULL)
      throw std::runtime_error("formula translation error");
    return result;
  }

  inline
  pbes lpe2pbes(const std::string& spec_text, const std::string& formula_text, bool untimed)
  {
    pbes result;
    specification spec = lpe::detail::mcrl22lpe(spec_text);
    std::stringstream from;
    from << formula_text;

    ATermAppl formula = parse_state_formula(from);
    formula = type_check(formula, spec);
    formula = implement_data(formula, spec);
    formula = translate_formula(formula);
    result = lpe::pbes_translate(state_formula(formula), spec, untimed);

    return result;
  }

} // namespace detail

} // namespace lpe

#endif // LPE_DETAIL_LPE2PBES
