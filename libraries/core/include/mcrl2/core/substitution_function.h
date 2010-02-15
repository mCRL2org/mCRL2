// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/substitution_function.h
/// \brief Base class for substitution functions.

#ifndef MCRL2_CORE_SUBSTITUTION_FUNCTION_H
#define MCRL2_CORE_SUBSTITUTION_FUNCTION_H

#include <functional>

namespace mcrl2 {

namespace core {

  template <typename Variable, typename Expression>
  class substitution_function: public std::unary_function<Expression, Expression>
  {
    public:
      typedef Expression expression_type;
      typedef Variable variable_type;
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_SUBSTITUTION_FUNCTION_H
