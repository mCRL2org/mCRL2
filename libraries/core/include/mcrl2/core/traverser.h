// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/traverser.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_TRAVERSER_H
#define MCRL2_CORE_TRAVERSER_H

#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2 {

namespace core {

  /**
   * \brief expression traverser that visits all sub expressions
   *
   * Types:
   *  \arg Derived the type of a derived class, as per CRTP
   *
   **/
  template <typename Derived>
  class traverser
  {
    public:

      template <typename Expression>
      void enter(Expression const&)
      {}

      template <typename Expression>
      void leave(Expression const&)
      {}

      template <typename Container>
      void operator()(Container const& container, typename atermpp::detail::enable_if_container<Container>::type* = 0)
      {
        for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
        {
          static_cast<Derived&>(*this)(*i);
        }
      }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_TRAVERSER_H
