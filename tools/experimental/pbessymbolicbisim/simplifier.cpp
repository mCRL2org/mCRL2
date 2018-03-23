// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file simplifier.cpp

#ifdef DBM_PACKAGE_AVAILABLE
  #include "simplifier_dbm.h"
#endif
#include "simplifier_finite_domain.h"
#include "simplifier_fourier_motzkin.h"
#include "simplifier_identity.h"
#include "simplifier_mode.h"

namespace mcrl2
{
namespace data
{

simplifier* get_simplifier_instance(const simplifier_mode& mode, const rewriter& rewr, const rewriter& proving_rewr, const variable_list& process_parameters, const data_specification& dataspec,
  const std::map< variable, std::pair<data_expression, data_expression> >& lu_map)
{
  switch(mode)
  {
    case simplify_fm:
      return new simplifier_fourier_motzkin(rewr, proving_rewr, dataspec);
#ifdef DBM_PACKAGE_AVAILABLE
    case simplify_dbm:
      return new simplifier_dbm(rewr, proving_rewr, process_parameters, dataspec, lu_map);
#endif
    case simplify_finite_domain:
      return new simplifier_finite_domain(rewr, proving_rewr, dataspec);
    case simplify_identity:
      return new simplifier_identity(rewr, proving_rewr);
    case simplify_auto:
    {
      if(std::find_if(process_parameters.begin(), process_parameters.end(), [&](const variable& v){ return v.sort() == sort_real::real_();}) != process_parameters.end())
      {
#ifdef DBM_PACKAGE_AVAILABLE
        return new simplifier_dbm(rewr, proving_rewr, process_parameters, dataspec, lu_map);
#else
        return new simplifier_fourier_motzkin(rewr, proving_rewr, dataspec);
        (void) lu_map;
#endif
      }
      else
      {
        return new simplifier_finite_domain(rewr, proving_rewr, dataspec);
      }
    }
    default:
      throw mcrl2::runtime_error("Unknown simplifier_mode.");
  }
}

} // namespace data
} // namespace mcrl2
