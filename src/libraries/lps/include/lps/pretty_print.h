#ifndef LPS_PRETTY_PRINT_H
#define LPS_PRETTY_PRINT_H

#include "atermpp/atermpp.h"
#include "libprint.h"
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

#endif // LPS_PRETTY_PRINT_H
