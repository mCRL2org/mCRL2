// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/print.h
/// \brief Basic print function for PBES.

#ifndef MCRL2_PBES_PRINT_H
#define MCRL2_PBES_PRINT_H

#include <string>
#include "mcrl2/core/print.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2 {

namespace pbes_system {

  template <typename Container>
  inline
  std::string pp(pbes<Container> pbes_spec, core::t_pp_format pp_format = core::ppDefault)
  {
    if (pp_format == core::ppDefault || pp_format == core::ppInternal)
    {
      //pbes_spec.data() = mcrl2::data::remove_all_system_defined(pbes_spec.data());
    }

    return core::pp(pbes_to_aterm(pbes_spec), pp_format);
  }

} // namespace pbes

} // namespace mcrl2

#endif // MCRL2_PBES_PRINT_H
