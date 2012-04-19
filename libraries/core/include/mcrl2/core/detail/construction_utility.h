// Author(s): Jeroen  van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/construction_utility.h
/// \brief construction utility

#ifndef MCRL2_CORE_CONSTRUCTION_UTILITY_H
#define MCRL2_CORE_CONSTRUCTION_UTILITY_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/identifier_string.h"

namespace mcrl2
{
namespace core
{
namespace detail
{

/// Function for initialisation of static variables, takes care of protection
/// \deprecated
/// \param[in,out] target a reference to the static variable
/// \param[in] original the expression that is used to initialise the variable
/// \ return a reference to original
template < typename Expression >
Expression const& initialise_static_expression(Expression& target, Expression const& original)
{
  // TODO: This function is deprecated and should be removed from all the generated code.
  // target = original;
  // target.protect();

  return original;
}

// Component that helps applying the Singleton design pattern
template < typename Derived, typename Expression = atermpp::aterm_appl >
struct singleton_expression : public Expression
{
  static Expression const& instance()
  {
    static Expression single_instance = initialise_static_expression(single_instance, Expression(Derived::initialise()));

    return single_instance;
  }

  singleton_expression() : Expression(instance())
  { }
};

template < typename Derived >
struct singleton_identifier : public singleton_expression< Derived, core::identifier_string >
  {};

} // namespace detail
} // namespace core
} // namespace mcrl2


#endif

