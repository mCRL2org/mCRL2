// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/remove_parameters.h
/// \brief add your file description here.

#include "mcrl2/lps/detail/lps_parameter_remover.h"

#ifndef MCRL2_LPS_REMOVE_PARAMETERS_H
#define MCRL2_LPS_REMOVE_PARAMETERS_H

namespace mcrl2 {

namespace lps {


  /// \brief Rewrites an LPS data type.
  template <typename Object, typename SetContainer>
  void remove_parameters(Object& o, const SetContainer& to_be_removed)
  {
    lps::detail::lps_parameter_remover<SetContainer> r(to_be_removed);
    r(o);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_REMOVE_PARAMETERS_H
