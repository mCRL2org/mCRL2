#ifndef LPE_PRETTY_PRINT_H
#define LPE_PRETTY_PRINT_H

#include "atermpp/atermpp.h"
#include "libprint.h"
#include <string>

namespace lpe {

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

#endif // LPE_PRETTY_PRINT_H
