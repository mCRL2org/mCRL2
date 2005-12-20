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

  /// Returns a pretty print representation of the term_list.
  ///
  template <typename Term>
  std::string pp(atermpp::term_list<Term> l)
  {
    return pretty_print(atermpp::aterm(l));
  }

} // namespace mcrl

#endif // LPE_PRETTY_PRINT_H
