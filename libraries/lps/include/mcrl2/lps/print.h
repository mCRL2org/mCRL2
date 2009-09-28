// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/print.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_PRINT_H
#define MCRL2_LPS_PRINT_H

#include <string>
#include "mcrl2/core/print.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2 {

namespace lps {

  inline
  std::string pp(specification spec, core::t_pp_format pp_format = core::ppDefault)
  {
    if (pp_format == core::ppDefault || pp_format == core::ppInternal)
    {
      spec.data() = mcrl2::data::remove_all_system_defined(spec.data());
    }

    return core::pp(specification_to_aterm(spec, pp_format != core::ppInternal), pp_format);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_PRINT_H
