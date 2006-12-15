///////////////////////////////////////////////////////////////////////////////
/// \file lpe/detail/mcrl22lpe.h

#ifndef LPE_DETAIL_MCRL22LPE
#define LPE_DETAIL_MCRL22LPE

#include <stdexcept>
#include <sstream>
#include <climits>
#include <iostream>
#include <fstream>
#include "aterm2.h"
#include "../../../../../mcrl22lpe/lin_types.h"
#include "../../../../../mcrl22lpe/lin_std.h"
#include "../../../../../mcrl22lpe/lin_alt.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libparse.h"
#include "typecheck.h"
#include "libalpha.h"
#include "dataimpl.h"
#include "lpe/specification.h"

namespace lpe {

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
  ATermAppl type_check(ATermAppl spec)
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
  ATermAppl implement_data(ATermAppl spec)
  {
    ATermAppl result = implement_data_spec(spec);
    if (result == NULL)
      throw std::runtime_error("data implementation error");
    return result;
  }
  
  inline
  ATermAppl linearise(ATermAppl spec, t_lin_options options, bool standard = true)
  {
    ATermAppl result = standard ? linearise_std(spec, options) : linearise_alt(spec, options);
    if (result == NULL)
      throw std::runtime_error("linearisation error");
    return result;
  }

  inline
  specification mcrl22lpe(const std::string& spec)
  {
    t_lin_options options;

    std::stringstream in;
    in << spec;

    ATermAppl result;
    try
    {
      result = parse_specification(in);
      result = type_check(result);
      // result = alpha_reduce(result);
      result = implement_data(result);
      result = linearise(result, options);
    }
    catch (std::runtime_error e)
    {
      cout << e.what() << endl;
      return specification();
    }
    return specification(aterm_appl(result));
  }

} // namespace detail

} // namespace lpe

#endif // LPE_DETAIL_MCRL22LPE
