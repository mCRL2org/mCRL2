// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/builder.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_BUILDER_H
#define MCRL2_CORE_BUILDER_H

#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2 {

namespace core {

  /**
   * \brief expression builder that visits all sub expressions
   *
   * Types:
   *  \arg Derived the type of a derived class, as per CRTP
   *
   **/
  template <typename Derived>
  class builder
  {
    public:

      template <typename Expression>
      void enter(Expression const&)
      {}

      template <typename Expression>
      void leave(Expression const&)
      {}

      template <typename Expression>
      Expression operator()(Expression const& x)
      {
        return x;
      }
  };


} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_BUILDER_H
