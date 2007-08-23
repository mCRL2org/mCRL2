// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/basic/pretty_print.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_PRETTY_PRINT_H
#define MCRL2_BASIC_PRETTY_PRINT_H

#include "atermpp/atermpp.h"
#include "print/libprint.h"
#include <string>

namespace lps {

  inline
  std::string pretty_print(ATerm t)
  {
    return PrintPart_CXX(t, ppDefault);
  }

  template <typename Term>
  std::string pp(Term t)
  {
    return pretty_print(atermpp::aterm_traits<Term>::term(t));
  }

} // namespace mcrl

#endif // MCRL2_BASIC_PRETTY_PRINT_H
