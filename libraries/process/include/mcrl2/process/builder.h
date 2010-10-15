// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/builder.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_BUILDER_H
#define MCRL2_PROCESS_BUILDER_H

#include "mcrl2/core/builder.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

  /// \brief Traversal class for process expression data types
  template <typename Derived>
  class builder: public core::builder<Derived>
  {
    public:
      typedef core::builder<Derived> super;
      using super::enter;
      using super::leave;
      using super::operator();

#include "mcrl2/process/detail/builder.inc.h"
  };

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_BUILDER_H
